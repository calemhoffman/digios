#!/usr/bin/env python3
"""
GenElogExtra.py -- Generate HTML table of detector thresholds + HV status for elog

Outputs HTML to stdout. Redirect to file:
  GenElogExtra.py > /tmp/elog_extra.txt

Reads:
  - Detector thresholds (E/XF/XN) via pyepics caget
  - HV voltage + leakage current via raw SNMP (snmpget -Op .12 for full precision)

Mapping: reads GeneralSortMapping.h dynamically (common.py)

[!!] Run on Pi ONLY (192.168.1.100)
"""

import sys
import os
import subprocess
import time

# Add script dir to path for common.py
sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
from common import load_mapping, build_sides
import epics

# ── Configuration ───────────────────────────────────────────────────────────

SNMP_HOST = '192.168.1.155'
SNMP_COMMUNITY = 'guru'
SKIP_DETS = {11, 21}  # known broken
CA_DELAY = 0.3        # seconds between caget calls
SNMP_DELAY = 0.3      # seconds between SNMP calls

# ── HV channel mapping (verified 2026-04-10 with Ryan) ─────────────────────
# Module 0 (u0-u15):  Left(0-5), Bottom(6-11), Top(18-21)
# Module 2 (u200-u215): Top(22-23), Right(12-17), u208-u215 unused
# Module 3 (u300-u315): RDT dE0,E0,dE1,E1,..., u308-u315 unused

DET_TO_HV = {
    # Module 0: Left
    0: 'u0',   1: 'u1',   2: 'u2',   3: 'u3',   4: 'u4',   5: 'u5',
    # Module 0: Bottom
    6: 'u6',   7: 'u7',   8: 'u8',   9: 'u9',  10: 'u10', 11: 'u11',
    # Module 0: Top (first 4)
    18: 'u12', 19: 'u13', 20: 'u14', 21: 'u15',
    # Module 2: Top (last 2)
    22: 'u200', 23: 'u201',
    # Module 2: Right
    12: 'u202', 13: 'u203', 14: 'u204', 15: 'u205', 16: 'u206', 17: 'u207',
    # Module 3: RDT (dE0,E0,dE1,E1,...)
    101: 'u300', 100: 'u301', 103: 'u302', 102: 'u303',
    105: 'u304', 104: 'u305', 107: 'u306', 106: 'u307',
}


def safe_caget(pv):
    time.sleep(CA_DELAY)
    return epics.caget(pv)


def get_threshold(vme, dig, ch):
    pv = f"VME0{vme}:MDIG{dig}:led_threshold{ch}"
    val = safe_caget(pv)
    return int(val) if val is not None else '-'


def snmp_get(oid_suffix, channel):
    """Read one SNMP float value with full precision (-Op .12)."""
    time.sleep(SNMP_DELAY)
    cmd = [
        'snmpget', '-v', '2c', '-c', SNMP_COMMUNITY, '-Op', '.12',
        SNMP_HOST, f'WIENER-CRATE-MIB::{oid_suffix}.{channel}'
    ]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
        if result.returncode == 0:
            parts = result.stdout.strip().split()
            for i, p in enumerate(parts):
                if p == 'Float:':
                    return float(parts[i + 1])
        return None
    except (subprocess.TimeoutExpired, Exception):
        return None


def get_hv_voltage(det):
    """Get measured output voltage via SNMP."""
    ch = DET_TO_HV.get(det)
    if ch is None:
        return None
    return snmp_get('outputMeasurementSenseVoltage', ch)


def get_hv_current(det):
    """Get leakage current via SNMP (full precision with -Op .12)."""
    ch = DET_TO_HV.get(det)
    if ch is None:
        return None
    return snmp_get('outputMeasurementCurrent', ch)


def fmt_current(val):
    """Format current in uA with 2 decimal places."""
    if val is None:
        return '-'
    return f"{val * 1e6:.2f}"


def fmt_voltage(val):
    """Format measured voltage with 2 decimal places."""
    if val is None:
        return '-'
    return f"{val:.2f}"


def main():
    det_map, narray, mapping_file = load_mapping()
    sides = build_sides(det_map, narray)

    psd_dets = sorted(d for d in det_map if d < 100)

    # Build side lookup
    side_names = {}
    for side_name in ['Left', 'Bottom', 'Right', 'Top']:
        for d in sides[side_name]:
            side_names[d] = side_name

    # Start HTML
    print('<br/>')
    print('<b>Detector Status:</b><br/>')
    print('<table border="1" cellpadding="3">')
    print('<tr><th>Det</th><th>Side</th><th>E</th><th>XF</th><th>XN</th><th>HV (V)</th><th>I (uA)</th></tr>')

    for det in psd_dets:
        side = side_names.get(det, '?')

        if det in SKIP_DETS:
            print(f'<tr><td>{det}</td><td>{side}</td><td colspan="5">- broken -</td></tr>')
            continue

        det_entry = det_map.get(det, {})

        # Thresholds
        e_thresh = '-'
        xf_thresh = '-'
        xn_thresh = '-'
        if 'E' in det_entry:
            vme, dig, ch = det_entry['E']
            e_thresh = get_threshold(vme, dig, ch)
        if 'XF' in det_entry:
            vme, dig, ch = det_entry['XF']
            xf_thresh = get_threshold(vme, dig, ch)
        if 'XN' in det_entry:
            vme, dig, ch = det_entry['XN']
            xn_thresh = get_threshold(vme, dig, ch)

        # HV
        hv_v = fmt_voltage(get_hv_voltage(det))
        hv_i = fmt_current(get_hv_current(det))

        print(f'<tr><td>{det}</td><td>{side}</td><td>{e_thresh}</td><td>{xf_thresh}</td><td>{xn_thresh}</td><td>{hv_v}</td><td>{hv_i}</td></tr>')

    # RDT
    rdt_dets = sorted(d for d in det_map if d >= 100)
    if rdt_dets:
        for det in rdt_dets:
            det_entry = det_map.get(det, {})
            # RDT: all thresholds go in E column (E and dE on same row)
            thresh = '-'
            if 'E' in det_entry:
                vme, dig, ch = det_entry['E']
                thresh = get_threshold(vme, dig, ch)
            elif 'dE' in det_entry:
                vme, dig, ch = det_entry['dE']
                thresh = get_threshold(vme, dig, ch)

            hv_v = fmt_voltage(get_hv_voltage(det))
            hv_i = fmt_current(get_hv_current(det))

            print(f'<tr><td>RDT{det-100}</td><td>Recoil</td><td>{thresh}</td><td>-</td><td>-</td><td>{hv_v}</td><td>{hv_i}</td></tr>')

    print('</table>')


if __name__ == '__main__':
    main()
