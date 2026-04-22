#!/usr/bin/env python3
"""
GenElogExtra.py -- HTML table of detector thresholds + HV status for elog.

Outputs HTML to stdout. Reads thresholds via pyepics, HV via snmpget.
Uses common.py to parse GeneralSortMapping.h dynamically.

Usage: GenElogExtra.py > ~/elog_extra.txt
"""

import os
import sys
import subprocess
import time
import threading

os.environ['EPICS_CA_ADDR_LIST'] = ''
os.environ['EPICS_CA_AUTO_ADDR_LIST'] = 'YES'

sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
from common import load_mapping, build_sides, build_hv_map
import epics

SNMP_HOST = '192.168.1.155'
SNMP_COMMUNITY = 'guru'
SNMP_DELAY = 0.3
SKIP_DETS = {11}

# HV mapping read from GeneralSortMapping.h (HVdetMap, HVrdtMap)

def safe_caget(pv):
    try:
        val = epics.caget(pv, timeout=3)
        return int(val) if val is not None else '-'
    except:
        return '-'

def snmp_get(oid_suffix, channel):
    time.sleep(SNMP_DELAY)
    cmd = ['snmpget', '-v', '2c', '-c', SNMP_COMMUNITY, SNMP_HOST,
           f'WIENER-CRATE-MIB::{oid_suffix}.{channel}']
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
        if result.returncode == 0:
            parts = result.stdout.strip().split()
            for i, p in enumerate(parts):
                if p == 'Float:':
                    return float(parts[i + 1])
        return None
    except:
        return None

def fmt_v(val):
    return f'{val:.1f}' if val is not None else '-'

def fmt_i(val):
    return f'{val*1e6:.2f}' if val is not None else '-'

def make_pv(vme, dig, ch):
    return f'VME0{vme}:MDIG{dig}:led_threshold{ch}'

def make_trace_pv(vme, dig, ch):
    return f'VME0{vme}:MDIG{dig}:raw_data_window{ch}'

def make_pretrig_pv(vme, dig, ch):
    return f'VME0{vme}:MDIG{dig}:raw_data_length{ch}'

def safe_caget_float(pv):
    try:
        val = epics.caget(pv, timeout=3)
        return f'{val:.2f}' if val is not None else '-'
    except:
        return '-'

def fetch_all_hv(det_map, hv_det_map, hv_rdt_map):
    """Fetch all HV voltages and currents in a background thread."""
    hv_data = {}
    # Array detectors: HVdetMap indexed by det number
    for det in sorted(d for d in det_map if d < 100):
        if hv_det_map and det < len(hv_det_map):
            hv_ch = hv_det_map[det]
            v = snmp_get('outputMeasurementSenseVoltage', hv_ch)
            i = snmp_get('outputMeasurementCurrent', hv_ch)
            hv_data[det] = (fmt_v(v), fmt_i(i))
        else:
            hv_data[det] = ('-', '-')
    # RDT detectors: HVrdtMap indexed by rdt channel (0-7)
    # idDetMap order: ch0=101(dE), ch1=100(E), ch2=103(dE), ...
    # rdt[i] -> det 100+i in the idDetMap order
    rdt_dets = sorted(d for d in det_map if d >= 100)
    for rdt_idx, det in enumerate(rdt_dets):
        if hv_rdt_map and rdt_idx < len(hv_rdt_map):
            hv_ch = hv_rdt_map[rdt_idx]
            v = snmp_get('outputMeasurementSenseVoltage', hv_ch)
            i = snmp_get('outputMeasurementCurrent', hv_ch)
            hv_data[det] = (fmt_v(v), fmt_i(i))
        else:
            hv_data[det] = ('-', '-')
    return hv_data

def main():
    det_map, narray, mapping_file = load_mapping()
    sides = build_sides(det_map, narray)

    # Parse HV map from GeneralSortMapping.h
    hv_det_map, hv_rdt_map = build_hv_map(mapping_file)

    # Start HV reads in background thread (SNMP, separate from EPICS CA)
    hv_result = [None]
    def hv_thread():
        hv_result[0] = fetch_all_hv(det_map, hv_det_map, hv_rdt_map)
    t = threading.Thread(target=hv_thread)
    t.start()

    side_names = {}
    for sn in ['Left', 'Bottom', 'Right', 'Top']:
        for d in sides[sn]:
            side_names[d] = sn

    # Read all thresholds + trace/pretrig via EPICS CA (while HV reads in parallel)
    psd_data = {}
    for det in sorted(d for d in det_map if d < 100):
        entry = det_map.get(det, {})
        if det in SKIP_DETS:
            psd_data[det] = None
            continue
        e_t = safe_caget(make_pv(*entry['E'])) if 'E' in entry else '-'
        xf_t = safe_caget(make_pv(*entry['XF'])) if 'XF' in entry else '-'
        xn_t = safe_caget(make_pv(*entry['XN'])) if 'XN' in entry else '-'
        trace = safe_caget_float(make_trace_pv(*entry['E'])) if 'E' in entry else '-'
        pretrig = safe_caget_float(make_pretrig_pv(*entry['E'])) if 'E' in entry else '-'
        psd_data[det] = (e_t, xf_t, xn_t, trace, pretrig)

    rdt_data = {}
    rdt_dets = sorted(d for d in det_map if d >= 100)
    # Find which det is E and which is dE per telescope
    done_tel = set()
    for det in rdt_dets:
        tel = (det - 100) // 2
        if tel in done_tel:
            continue
        done_tel.add(tel)
        id_a = 100 + tel * 2
        id_b = 100 + tel * 2 + 1
        entry_a = det_map.get(id_a, {})
        entry_b = det_map.get(id_b, {})
        # Figure out which is E and which is dE from the mapping keys
        if 'E' in entry_a:
            e_id, e_entry = id_a, entry_a
            de_id, de_entry = id_b, entry_b
        else:
            e_id, e_entry = id_b, entry_b
            de_id, de_entry = id_a, entry_a

        e_key = 'E' if 'E' in e_entry else list(e_entry.keys())[0] if e_entry else None
        de_key = 'dE' if 'dE' in de_entry else list(de_entry.keys())[0] if de_entry else None

        e_t = safe_caget(make_pv(*e_entry[e_key])) if e_key else '-'
        e_trace = safe_caget_float(make_trace_pv(*e_entry[e_key])) if e_key else '-'
        e_pretrig = safe_caget_float(make_pretrig_pv(*e_entry[e_key])) if e_key else '-'

        de_t = safe_caget(make_pv(*de_entry[de_key])) if de_key else '-'
        de_trace = safe_caget_float(make_trace_pv(*de_entry[de_key])) if de_key else '-'
        de_pretrig = safe_caget_float(make_pretrig_pv(*de_entry[de_key])) if de_key else '-'

        rdt_data[tel] = {
            'e': (e_t, e_trace, e_pretrig, e_id),
            'de': (de_t, de_trace, de_pretrig, de_id),
        }

    # Wait for HV thread to finish
    t.join()
    hv = hv_result[0]

    # Now print everything
    print('<br/>')
    print('<b>Detector Status:</b><br/>')
    print('<table border="1" cellpadding="3">')
    print('<tr><th>Det</th><th>VME</th><th>Side</th><th>E</th><th>XF</th><th>XN</th><th>PreTrig</th><th>Trace</th><th>HV(V)</th><th>I(uA)</th></tr>')

    for det in sorted(d for d in det_map if d < 100):
        side = side_names.get(det, '?')
        entry = det_map.get(det, {})
        if det in SKIP_DETS:
            vme_str = '-'
            if 'E' in entry:
                v, d2, c = entry['E']
                vme_str = f'{v}-{d2}-{c}'
            print(f'<tr><td>{det}</td><td>{vme_str}</td><td>{side}</td><td colspan="7">- dead -</td></tr>')
            continue
        e_t, xf_t, xn_t, trace, pretrig = psd_data[det]
        # VME channel from E signal
        vme_str = '-'
        if 'E' in entry:
            v, d2, c = entry['E']
            vme_str = f'{v}-{d2}-{c}'
        hv_v, hv_i = hv.get(det, ('-', '-'))
        print(f'<tr><td>{det}</td><td>{vme_str}</td><td>{side}</td><td>{e_t}</td><td>{xf_t}</td><td>{xn_t}</td><td>{pretrig}</td><td>{trace}</td><td>{hv_v}</td><td>{hv_i}</td></tr>')

    # RDT -- E/XF/XN merged into single Thresh column
    print('<tr><th>Det</th><th>VME</th><th>Side</th><th colspan="3">Thresh</th><th>PreTrig</th><th>Trace</th><th>HV(V)</th><th>I(uA)</th></tr>')
    for tel in sorted(rdt_data.keys()):
        rd = rdt_data[tel]
        e_t, e_trace, e_pretrig, e_id = rd['e']
        de_t, de_trace, de_pretrig, de_id = rd['de']
        e_entry = det_map.get(e_id, {})
        de_entry = det_map.get(de_id, {})
        e_key = 'E' if 'E' in e_entry else list(e_entry.keys())[0] if e_entry else None
        de_key = 'dE' if 'dE' in de_entry else list(de_entry.keys())[0] if de_entry else None
        e_vme = f'{e_entry[e_key][0]}-{e_entry[e_key][1]}-{e_entry[e_key][2]}' if e_key else '-'
        de_vme = f'{de_entry[de_key][0]}-{de_entry[de_key][1]}-{de_entry[de_key][2]}' if de_key else '-'
        e_hv_v, e_hv_i = hv.get(e_id, ('-', '-'))
        de_hv_v, de_hv_i = hv.get(de_id, ('-', '-'))
        print(f'<tr><td>RDT{tel} E</td><td>{e_vme}</td><td>Recoil</td><td colspan="3">{e_t}</td><td>{e_pretrig}</td><td>{e_trace}</td><td>{e_hv_v}</td><td>{e_hv_i}</td></tr>')
        print(f'<tr><td>RDT{tel} dE</td><td>{de_vme}</td><td>Recoil</td><td colspan="3">{de_t}</td><td>{de_pretrig}</td><td>{de_trace}</td><td>{de_hv_v}</td><td>{de_hv_i}</td></tr>')

    print('</table>')
    print('<br/>VME format is vme-dig-ch (e.g. 4-1-0 = VME04:MDIG1:CH0).<br/>')

if __name__ == '__main__':
    main()
