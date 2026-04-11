"""
common.py -- Shared utilities for HELIOS DAQ scripts

Provides:
  - GeneralSortMapping.h parser (detID map, side groupings)
  - EPICS CA helpers with pacing
  - PV alias maps

Used by: SetChannel, GetChannel, AutoTuneThreshold.py
"""

import os
import re
from collections import defaultdict

# ── Point pyepics at the compiled EPICS CA library ──────────────────────────
_libca = os.path.expanduser("~/epics-base/lib/linux-aarch64/libca.so")
if os.path.exists(_libca):
    os.environ.setdefault("PYEPICS_LIBCA", _libca)

# ── GeneralSortMapping.h parser ─────────────────────────────────────────────

def find_mapping_file(script_dir=None):
    """Search for GeneralSortMapping.h relative to script or standard locations."""
    if script_dir is None:
        script_dir = os.path.dirname(os.path.realpath(__file__))
    candidates = [
        os.path.join(script_dir, '../../../analysis/working_Helios/GeneralSortMapping.h'),
        os.path.join(script_dir, '../../../analysis/working/GeneralSortMapping.h'),
        os.environ.get('HELIOS_MAPPING', ''),
    ]
    for p in candidates:
        if not p:
            continue
        p = os.path.normpath(p)
        if os.path.isfile(p):
            return p
    return None


def parse_int_array(text, name, expected_len=160):
    """Extract a C int array initializer by name from header text."""
    m = re.search(rf'Int_t\s+{name}\s*\[', text)
    if not m:
        raise ValueError(f"Could not find {name} in mapping file")
    start = m.start()
    brace_start = text.index('{', start)
    depth = 0; i = brace_start; chars = []
    while i < len(text):
        c = text[i]
        if c == '{': depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0: break
        chars.append(c)
        i += 1
    body = ''.join(chars[1:])
    body = re.sub(r'//[^\n]*', '', body)
    nums = re.findall(r'-?\d+', body)
    return [int(x) for x in nums[:expected_len]]


def build_det_map(mapping_file):
    """Parse GeneralSortMapping.h and return (det_map, narray).
    
    det_map: {det_id: {signal: (vme, dig, ch), ...}, ...}
      - det 0-23: array PSD detectors, signals E/XF/XN
      - det 100-107: RDT detectors, signals E/dE
    narray: number of array detectors (from #define NARRAY)
    """
    with open(mapping_file) as f:
        text = f.read()

    narray_m = re.search(r'#define\s+NARRAY\s+(\d+)', text)
    narray = int(narray_m.group(1)) if narray_m else 24

    size_m = re.search(r'Int_t\s+idDetMap\s*\[\s*(\d+)\s*\]', text)
    arr_size = int(size_m.group(1)) if size_m else 160

    det_map_raw  = parse_int_array(text, 'idDetMap',  arr_size)
    kind_map_raw = parse_int_array(text, 'idKindMap', arr_size)

    kind_name = {0: 'E', 1: 'XF', 2: 'XN'}
    det_map = defaultdict(dict)

    for idx, (det, kind) in enumerate(zip(det_map_raw, kind_map_raw)):
        if det == -1:
            continue
        board_idx = idx // 10
        ch = idx % 10
        vme = board_idx // 4 + 1
        dig = board_idx % 4 + 1
        if det >= 100:
            sig = 'dE' if det % 2 == 1 else 'E'
        else:
            if kind == -1:
                continue
            sig = kind_name.get(kind)
            if sig is None:
                continue
        if sig not in det_map[det]:
            det_map[det][sig] = (vme, dig, ch)

    return dict(det_map), narray


def parse_string_array(text, name):
    """Extract a C string array (const char*) by name. Returns list of strings."""
    m = re.search(rf'const\s+char\s*\*\s*{name}\s*\[', text)
    if not m:
        return None
    start = m.start()
    brace_start = text.index('{', start)
    brace_end = text.index('}', brace_start)
    body = text[brace_start+1:brace_end]
    body = re.sub(r'//[^\n]*', '', body)
    return re.findall(r'"([^"]+)"', body)


def build_hv_map(mapping_file):
    """Parse HVdetMap and HVrdtMap from GeneralSortMapping.h.
    Returns (hv_det_map, hv_rdt_map) as lists of SNMP channel strings.
    Returns (None, None) if not found."""
    with open(mapping_file) as f:
        text = f.read()
    hv_det = parse_string_array(text, 'HVdetMap')
    hv_rdt = parse_string_array(text, 'HVrdtMap')
    return hv_det, hv_rdt


def load_mapping(script_dir=None):
    """Convenience: find + parse mapping file. Returns (det_map, narray, mapping_file)."""
    mapping_file = find_mapping_file(script_dir)
    if mapping_file is None:
        raise FileNotFoundError("GeneralSortMapping.h not found")
    det_map, narray = build_det_map(mapping_file)
    return det_map, narray, mapping_file


# ── Side groupings ──────────────────────────────────────────────────────────

N_SIDES = 4

def build_sides(det_map, narray):
    """Build side -> [det_ids] mapping for 4-side array."""
    psd = sorted(d for d in det_map if d < 100)
    rdt = sorted(d for d in det_map if d >= 100)
    q = narray // N_SIDES
    return {
        'Left':   psd[0:q],
        'Bottom': psd[q:2*q],
        'Right':  psd[2*q:3*q],
        'Top':    psd[3*q:4*q],
        'All':    psd,
        'Recoil': rdt,
    }


# ── Signal aliases ──────────────────────────────────────────────────────────

SIGNAL_ALIASES = {
    'Energy': 'E', 'energy': 'E', 'e': 'E',
    'xf': 'XF', 'xn': 'XN',
    'de': 'dE', 'DE': 'dE',
}

VALID_SIGNALS = {'E', 'XF', 'XN', 'dE', 'All'}

def resolve_signal(s):
    """Resolve signal name alias to canonical form."""
    resolved = SIGNAL_ALIASES.get(s, s)
    return resolved


# ── PV helpers ──────────────────────────────────────────────────────────────

# PV field aliases (for SetChannel/GetChannel argument parsing)
PV_ALIASES = {
    'threshold':  'led_threshold',
    'T':          'led_threshold',
    'rate':       'disc_count',
    'Rate':       'disc_count',
    'count':      'disc_count',
    'M':          'm_window',
    'D':          'd_window',
    'K':          'k_window',
    'P1':         'p1_window',
    'disc':       'disc_width',
    'reset':      'preamp_reset_delay',
    'enable':     'channel_enable',
    'raw_length': 'raw_data_length',
    'RL':         'raw_data_length',
    'trace':      'raw_data_length',
    'raw_delay':  'raw_data_window',
    'RD':         'raw_data_window',
    'preTrigger': 'raw_data_window',
    'pretrigger': 'raw_data_window',
    'preTrig':    'raw_data_window',
    'pretrig':    'raw_data_window',
}

# PVs that are board-level (per MDIG, no channel suffix)
BOARD_LEVEL_PVS = {'raw_data_length', 'raw_data_window'}

def resolve_pv(pv_arg):
    """Resolve PV alias to actual PV field name."""
    return PV_ALIASES.get(pv_arg, pv_arg)

def make_set_pv_name(vme, dig, ch, pv_field):
    """Build PV name for caput (Set): VME0x:MDIGy:pv_field{ch}"""
    if pv_field in BOARD_LEVEL_PVS:
        return f"VME0{vme}:MDIG{dig}:{pv_field}"
    return f"VME0{vme}:MDIG{dig}:{pv_field}{ch}"

def make_get_pv_name(vme, dig, ch, pv_field):
    """Build PV name for caget (Get): VME0x:MDIGy:pv_field{ch}_RBV"""
    if pv_field in BOARD_LEVEL_PVS:
        return f"VME0{vme}:MDIG{dig}:{pv_field}"
    return f"VME0{vme}:MDIG{dig}:{pv_field}{ch}_RBV"
