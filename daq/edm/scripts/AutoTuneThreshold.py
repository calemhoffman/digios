#!/usr/bin/env python3
"""
AutoTuneThreshold.py -- Auto-tune LED thresholds for HELIOS silicon array

For each detector channel (det 0-23, signals E/XF/XN):
  1. Measure baseline disc_count rate at current threshold
  2. Step threshold down in increments
  3. Stop when rate increases > tolerance vs baseline (noise floor hit)
  4. Revert to previous (safe) threshold

Usage:
  AutoTuneThreshold.py [options]

Options:
  --side <side>      Only tune: Left, Bottom, Right, Top, All (default: All)
  --signal <signal>  Only tune: E, XF, XN, All (default: All)
  --det <N>          Only tune detector N (0-23)
  --step <N>         Threshold step size (default: 50)
  --settle <N>       Seconds to wait after threshold change (default: 5)
  --samples <N>      Number of rate samples to average (default: 3)
  --nsigma <f>       N-sigma tolerance for noise floor detection (default: 3.0)
  --min-thresh <N>   Minimum threshold to try (default: 50)
  --dry-run          Show what would be done without changing anything
  --help             Show this help

Mapping: reads GeneralSortMapping.h dynamically (common.py)
"""

import sys
import os
import time
import argparse
import subprocess
from datetime import datetime

# Add script dir to path for common.py
sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
from common import load_mapping, build_sides
import epics

# ── Discord reporting ───────────────────────────────────────────────────────

DISCORD_CHANNEL = '1492292081375248405'

def discord_send(msg):
    """Post a message to the HELIOS reporting Discord channel via openclaw CLI."""
    try:
        subprocess.run(
            ['openclaw', 'message', 'send',
             '--channel', 'discord',
             '--target', DISCORD_CHANNEL,
             '-m', msg],
            timeout=15, capture_output=True
        )
    except Exception as e:
        print(f"  [discord] send failed: {e}")

# ── EPICS helpers (with pacing) ─────────────────────────────────────────────

CA_DELAY = 0.5   # seconds between CA requests to avoid jamming IOC
RATE_SAMPLES = 10  # number of disc_count reads to average (1s apart)
STEP_SAMPLES = 3  # fast samples per step (after baseline)

# Known broken detectors -- skip these
SKIP_DETS = {11, 21}

COARSE_STEP = 50
FINE_STEP = 10

def safe_caget(pv_name):
    time.sleep(CA_DELAY)
    return epics.caget(pv_name)

def safe_caput(pv_name, value):
    time.sleep(CA_DELAY)
    epics.caput(pv_name, value)

def get_rate(vme, dig, ch, samples=RATE_SAMPLES, label=''):
    """Average disc_count rate over N samples (1s between reads = EPICS update frequency)."""
    total = 0
    vals = []
    pv = f"VME0{vme}:MDIG{dig}:disc_count{ch}_RBV"
    if label:
        print(f"  {label} sampling", end='', flush=True)
    for _ in range(samples):
        time.sleep(1.0)
        val = epics.caget(pv)
        v = int(val) if val is not None else 0
        vals.append(v)
        total += v
        if label:
            print('.', end='', flush=True)
    avg = total // samples
    if label:
        result = f"  {label} avg={avg}  [{', '.join(str(v) for v in vals)}]"
        print(f"\r{result}" + ' ' * 20)
    return avg

def get_threshold(vme, dig, ch):
    pv = f"VME0{vme}:MDIG{dig}:led_threshold{ch}"
    val = safe_caget(pv)
    return int(val) if val is not None else 400

def set_threshold(vme, dig, ch, value):
    pv = f"VME0{vme}:MDIG{dig}:led_threshold{ch}"
    safe_caput(pv, value)

def is_enabled(vme, dig, ch):
    pv = f"VME0{vme}:MDIG{dig}:channel_enable{ch}"
    val = safe_caget(pv)
    return val is not None and str(val).strip() in ('Run', '1')


# ── Main tuning logic ──────────────────────────────────────────────────────

import math

def get_baseline(vme, dig, ch, samples, label=''):
    """Measure baseline: returns (mean, stddev, vals)."""
    vals = []
    pv = f"VME0{vme}:MDIG{dig}:disc_count{ch}_RBV"
    if label:
        print(f"  {label} sampling", end='', flush=True)
    for _ in range(samples):
        time.sleep(1.0)
        val = epics.caget(pv)
        v = int(val) if val is not None else 0
        vals.append(v)
        if label:
            print('.', end='', flush=True)
    mean = sum(vals) / len(vals)
    variance = sum((v - mean) ** 2 for v in vals) / len(vals)
    std = math.sqrt(variance)
    if label:
        result = f"  {label} mean={mean:.1f} std={std:.1f}  [{', '.join(str(v) for v in vals)}]"
        print(f"\r{result}" + ' ' * 20)
    return mean, std, vals


def get_quick_rate(vme, dig, ch, samples=STEP_SAMPLES):
    """Fast rate check: N samples at 1s intervals, returns mean."""
    total = 0
    pv = f"VME0{vme}:MDIG{dig}:disc_count{ch}_RBV"
    for _ in range(samples):
        time.sleep(1.0)
        val = epics.caget(pv)
        total += int(val) if val is not None else 0
    return total / samples



def step_down(det, sig, vme, dig, ch, limit, step, min_thresh, log, dry_run=False):
    """Step threshold down until rate exceeds limit. Returns final threshold."""
    label = f"det{det:02d}-{sig}"
    current = get_threshold(vme, dig, ch)

    if dry_run:
        msg = f"    -> [dry-run] {label}: would step down from {current} by {step}"
        print(msg); log.append(msg)
        return current

    prev = current
    final = current
    new = current - step

    while new >= min_thresh:
        set_threshold(vme, dig, ch, new)
        time.sleep(1.0)
        rate = get_quick_rate(vme, dig, ch, STEP_SAMPLES)
        msg = f"    {label} thresh={new}: rate={rate:.1f} (limit={limit:.1f})"
        print(msg); log.append(msg)

        if rate > limit:
            msg = f"    -> {label} noise floor at {new}, reverting to {prev}"
            print(msg); log.append(msg)
            set_threshold(vme, dig, ch, prev)
            return prev

        prev = new
        final = new
        new -= step

    msg = f"    -> {label} reached min_thresh={min_thresh}, final={final}"
    print(msg); log.append(msg)
    return final


def tune_detector(det, det_entry, args, log):
    """Tune one detector: E (or dE for RDT) first (baseline + coarse/fine), then XF/XN to match.
    Returns dict of {signal: final_threshold}."""
    results = {}
    originals = {}  # track original thresholds for reporting

    # Skip known broken detectors
    if det in SKIP_DETS:
        msg = f"  det{det:02d}: KNOWN BROKEN -- skip"
        print(msg); log.append(msg)
        return results

    print(f"\n  === det{det:02d} ===")

    # --- Step 1: Baseline on primary channel (E for array, E or dE for RDT) ---
    if 'E' in det_entry:
        primary_sig = 'E'
    elif 'dE' in det_entry:
        primary_sig = 'dE'
    else:
        msg = f"  det{det:02d}: no E or dE channel -- skip detector"
        print(msg); log.append(msg)
        return results

    e_vme, e_dig, e_ch = det_entry[primary_sig]
    e_label = f"det{det:02d}-{primary_sig} (VME{e_vme}-DIG{e_dig}-CH{e_ch})"

    if not is_enabled(e_vme, e_dig, e_ch):
        msg = f"  {e_label}: DISABLED -- skip detector"
        print(msg); log.append(msg)
        return results

    e_current = get_threshold(e_vme, e_dig, e_ch)
    originals[primary_sig] = e_current
    bl_mean, bl_std, bl_vals = get_baseline(e_vme, e_dig, e_ch, args.samples,
                                             label=f"{e_label} baseline")
    msg = f"  {e_label}: thresh={e_current}, baseline mean={bl_mean:.1f}, std={bl_std:.1f}"
    print(msg); log.append(msg)

    # Compute limit from baseline
    nsigma = args.nsigma
    limit = bl_mean + nsigma * bl_std
    if limit < 2.0:
        limit = 2.0
    msg = f"    -> limit = {limit:.1f} (mean + {nsigma}*std)"
    print(msg); log.append(msg)

    # --- Step 2: Tune primary -- coarse then fine ---
    msg = f"  Tuning {primary_sig}: coarse (step={COARSE_STEP})"
    print(msg); log.append(msg)
    coarse_result = step_down(det, primary_sig, e_vme, e_dig, e_ch, limit, COARSE_STEP,
                               args.min_thresh, log, args.dry_run)

    msg = f"  Tuning {primary_sig}: fine (step={FINE_STEP})"
    print(msg); log.append(msg)
    fine_result = step_down(det, primary_sig, e_vme, e_dig, e_ch, limit, FINE_STEP,
                             args.min_thresh, log, args.dry_run)

    results[primary_sig] = fine_result
    e_changed = (fine_result != e_current)
    msg = f"  RESULT: {e_label} threshold {e_current} -> {fine_result}" + \
          (" [CHANGED]" if e_changed else " [unchanged]")
    print(msg); log.append(msg)

    # --- Step 3: Tune secondary signals (XF/XN for array, none for RDT) ---
    for sig in ['XF', 'XN']:
        if sig not in det_entry:
            continue
        vme, dig, ch = det_entry[sig]
        label = f"det{det:02d}-{sig} (VME{vme}-DIG{dig}-CH{ch})"

        if not is_enabled(vme, dig, ch):
            msg = f"  {label}: DISABLED -- skip"
            print(msg); log.append(msg)
            continue

        current = get_threshold(vme, dig, ch)
        originals[sig] = current
        msg = f"  Tuning {sig} to match E (limit={limit:.1f})"
        print(msg); log.append(msg)

        # Coarse then fine, same limit as E
        coarse_result = step_down(det, sig, vme, dig, ch, limit, COARSE_STEP,
                                   args.min_thresh, log, args.dry_run)
        fine_result = step_down(det, sig, vme, dig, ch, limit, FINE_STEP,
                                 args.min_thresh, log, args.dry_run)

        results[sig] = fine_result
        changed = (fine_result != current)
        msg = f"  RESULT: {label} threshold {current} -> {fine_result}" + \
              (" [CHANGED]" if changed else " [unchanged]")
        print(msg); log.append(msg)

    # Report per-detector results to Discord
    if results and not args.dry_run:
        parts = []
        for sig in ['E', 'dE', 'XF', 'XN']:
            if sig in results:
                orig = originals.get(sig, '?')
                parts.append(f"{sig} {orig}->{results[sig]}")
        summary = ', '.join(parts)
        discord_send(f"[AutoTune] det{det:02d}: {summary}")
    elif det in SKIP_DETS:
        discord_send(f"[AutoTune] det{det:02d}: SKIPPED (known broken)")

    return results


def main():
    parser = argparse.ArgumentParser(description='Auto-tune LED thresholds for HELIOS silicon array')
    parser.add_argument('--side', default='All', help='Side: Left, Bottom, Right, Top, All')
    parser.add_argument('--det', type=int, default=None, help='Single detector number (0-23)')
    parser.add_argument('--samples', type=int, default=RATE_SAMPLES, help=f'Rate samples to average (default: {RATE_SAMPLES})')
    parser.add_argument('--nsigma', type=float, default=1.0, help='N-sigma tolerance for noise floor (default: 3.0)')
    parser.add_argument('--min-thresh', type=int, default=50, help='Minimum threshold (default: 50)')
    parser.add_argument('--dry-run', action='store_true', help='Preview without changing anything')
    args = parser.parse_args()

    det_map, narray, mapping_file = load_mapping()
    sides = build_sides(det_map, narray)

    print(f"Mapping: {mapping_file}")
    psd = len([d for d in det_map if d < 100])
    rdt = len([d for d in det_map if d >= 100])
    print(f"Detectors: {psd} PSD, {rdt} RDT")

    if args.det is not None:
        dets = [args.det]
    elif args.side in sides:
        dets = sides[args.side]
    else:
        print(f"Unknown side: {args.side}")
        sys.exit(1)

    now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    print(f"\n======== AutoTuneThreshold ========")
    print(f"  Time:      {now}")
    print(f"  Side:      {args.side}  Dets: {dets}")
    print(f"  Coarse:    {COARSE_STEP}  Fine: {FINE_STEP}")
    print(f"  Baseline:  {args.samples} samples")
    print(f"  N-sigma:   {args.nsigma}")
    print(f"  Min:       {args.min_thresh}")
    print(f"  Dry-run:   {args.dry_run}")
    print(f"{'=' * 40}")

    log = []
    all_results = {}
    changed_count = 0

    for det in dets:
        det_entry = det_map.get(det)
        if det_entry is None:
            print(f"  det {det}: not in map -- skip")
            continue
        det_results = tune_detector(det, det_entry, args, log)
        for sig, final in det_results.items():
            all_results[(det, sig)] = final
            # count as changed if different from original (400 default)
            changed_count += 1

    print(f"\n{'=' * 40}")
    print(f"Tuning complete at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Channels tuned: {len(all_results)}, Changed: {changed_count}")

    logfile = f"/tmp/autotune_{datetime.now().strftime('%Y%m%d_%H%M%S')}.log"
    with open(logfile, 'w') as f:
        for line in log:
            f.write(line + '\n')
    print(f"Log: {logfile}")

    # Final summary to Discord
    if not args.dry_run:
        skipped = len([d for d in dets if d in SKIP_DETS])
        discord_send(
            f"[AutoTune] Complete: {len(all_results)} channels tuned across "
            f"{len(dets) - skipped} detectors" +
            (f", {skipped} skipped" if skipped else '') +
            f". Log: {logfile}"
        )


if __name__ == '__main__':
    main()
