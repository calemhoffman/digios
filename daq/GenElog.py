#!/usr/bin/env python3
"""
GenElog.py -- Generate HTML elog entry for HELIOS run start/stop

Usage:
  GenElog.py start   -> reads expName.sh, detectorGeo.txt, PVs, writes ~/elog.txt
  GenElog.py stop "reason"  -> writes ~/elog_stop.txt

Reads PVs via pyepics (EPICS_CA_AUTO_ADDR_LIST=YES)
Reads detector geometry from ~/digios/analysis/working/detectorGeo.txt
"""

import sys
import os
import time
from datetime import datetime

os.environ['EPICS_CA_ADDR_LIST'] = ''
os.environ['EPICS_CA_AUTO_ADDR_LIST'] = 'YES'

import epics

def read_expname():
    """Read expName.sh and return dict of variables."""
    info = {}
    path = os.path.expanduser('~/digios/expName.sh')
    with open(path) as f:
        for line in f:
            line = line.strip()
            if line.startswith('#') or '=' not in line:
                continue
            key, val = line.split('=', 1)
            info[key.strip()] = val.strip().strip('"').strip("'")
    return info

def read_detector_geo():
    """Read detectorGeo.txt and return dict of key values."""
    geo = {}
    path = os.path.expanduser('~/digios/analysis/working/detectorGeo.txt')
    try:
        with open(path) as f:
            lines = f.readlines()
        if len(lines) >= 16:
            geo['bfield'] = float(lines[0].split('//')[0].strip())
            geo['rdt_pos'] = float(lines[6].split('//')[0].strip())
            geo['array_pos'] = float(lines[15].split('//')[0].strip())
    except:
        pass
    return geo

def safe_caget(pv, as_string=True, timeout=3):
    """Read a PV with timeout. as_string=True returns enum labels."""
    try:
        val = epics.caget(pv, as_string=as_string, timeout=timeout)
        return val if val is not None else '?'
    except:
        return '?'

def gen_start():
    info = read_expname()
    geo = read_detector_geo()
    run = int(info.get('LastRunNum', 0))
    run_str = f'{run:03d}'
    comment = info.get('lastRunComment', '')

    now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    # Read trigger PVs (as_string=True returns enum labels)
    trig_mux = safe_caget('GLBL:DIG:trigger_mux_select')
    sum_x = safe_caget('VME32:MTRG:SUM_X')
    sum_y = safe_caget('VME32:MTRG:SUM_Y')

    # Geometry
    bfield = geo.get('bfield', '?')
    array_pos = geo.get('array_pos', '?')
    rdt_pos = geo.get('rdt_pos', '?')

    # Build HTML
    lines = []
    lines.append(f'<b>RUN-{run_str}</b> started at {now}.<br/>')
    lines.append(f'<b>Comment:</b> {comment}<br/>')
    lines.append(f'<b>B-field:</b> {bfield} T | <b>Array pos:</b> {array_pos} mm | <b>RDT pos:</b> {rdt_pos} mm<br/>')
    lines.append(f'<b>Trigger:</b> {trig_mux} | SUM_X: {sum_x} | SUM_Y: {sum_y}<br/>')
    lines.append('-----------------------------------------------<br/>')

    outpath = os.path.expanduser('~/elog.txt')
    with open(outpath, 'w') as f:
        f.write('\n'.join(lines))

    print(f'GenElog: wrote {outpath} for RUN-{run_str}')

def gen_stop(reason=''):
    info = read_expname()
    run = int(info.get('LastRunNum', 0))
    run_str = f'{run:03d}'

    now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    lines = []
    lines.append(f'<b>RUN-{run_str}</b> stopped at {now}.<br/>')
    lines.append(f'<b>Stop reason:</b> {reason}<br/>')
    lines.append('-----------------------------------------------<br/>')

    outpath = os.path.expanduser('~/elog_stop.txt')
    with open(outpath, 'w') as f:
        f.write('\n'.join(lines))

    print(f'GenElog: wrote {outpath} for RUN-{run_str} stop')

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: GenElog.py start|stop [reason]')
        sys.exit(1)

    action = sys.argv[1]
    if action == 'start':
        gen_start()
    elif action == 'stop':
        reason = sys.argv[2] if len(sys.argv) > 2 else ''
        gen_stop(reason)
    else:
        print(f'Unknown action: {action}')
        sys.exit(1)
