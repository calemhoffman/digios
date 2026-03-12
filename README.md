[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3517238.svg)](https://doi.org/10.5281/zenodo.3517238)

# digios

Digital I/O System for HELIOS — DAQ and analysis software for the HELIOS solenoidal spectrometer at Argonne National Laboratory.

---

## Infrastructure (as of March 2026)

- **HELIOS-DAQ** (`digios1`, 192.168.1.2): DAQ host running EPICS soft IOC and VME digitizer crates. No direct internet access.
- **HELIOS-MAC** (`phywl094`, 192.168.1.164 / Mac2020): Primary analysis machine.
- **HELIOS-PI** (`pi5-2`, 192.168.1.100): Raspberry Pi 5 — acts as git relay between DAQ/MAC and GitHub, and hosts HELIOS AI.
- **HELIOS-DB** (`192.168.1.193` / Mac2017): InfluxDB + Grafana slow-control database.
- **LCRC** (`/lcrc/project/HELIOS/digios/`): Argonne computing cluster for parallel analysis.

### Git Relay (since Aug 2025)
DAQ can no longer reach GitHub directly (SSL too old). The Pi acts as a bare git mirror:
- DAQ and MAC use Pi as `origin` (`ryan@192.168.1.100:~/digios.git`)
- Pi syncs to GitHub daily at 4AM via cron
- To sync manually from Pi: `~/digios_sync_github.sh`

---

## Setup

After cloning, add to `~/.bashrc`:
```bash
source ~/path_to_repository/heliosrc.sh
```

Key environment variables set by `heliosrc.sh`:
- `HELIOSSYS` — root of digios repo
- `HELIOSDAQ` — `$HELIOSSYS/daq`
- `HELIOSANA` — `$HELIOSSYS/analysis/working`
- Armory scripts added to `PATH`

---

## Repository Structure

```
digios/
├── daq/
│   ├── edm/
│   │   ├── screens/       — EPICS Display Manager panels
│   │   └── scripts/       — DAQ control scripts (start_run, stop_run, etc.)
│   └── saves/             — Saved array/trigger configurations
├── analysis/
│   ├── working/           — Experiment-specific files (changes per branch)
│   │   ├── Analyzer.C/.h        — ROOT TSelector event-by-event analysis
│   │   ├── Monitors.C/.h        — Online/offline monitor histograms
│   │   ├── ChainMonitors.C      — Chain multiple ROOT files for analysis
│   │   ├── GeneralSortMapping.h — Channel mapping (authoritative detector map)
│   │   ├── reactionConfig.txt   — Reaction kinematics configuration
│   │   ├── detectorGeo.txt      — Silicon array geometry
│   │   └── Simulation_Helper.C  — GUI for kinematics/DWBA simulation
│   ├── Armory/            — Shared utilities and calibration routines
│   │   ├── Apollo.C/.h          — Core analysis library
│   │   ├── Cali_*.C             — Calibration routines
│   │   ├── Check_*.C            — Diagnostic checks
│   │   └── Process_RUN          — Main data processing pipeline
│   ├── Cleopatra/         — Kinematics and DWBA simulation
│   │   ├── Transfer.C/.h        — Monte Carlo transfer reaction simulation
│   │   ├── HELIOS_LIB.h         — HELIOS geometry + kinematics library
│   │   ├── Cleopatra.C/.sh      — Ptolemy DWBA wrapper
│   │   └── Isotope.h/.C         — Nuclear mass table (AME2016)
│   ├── EventBuilder/      — Coincidence event builder (replaces GEBSort)
│   │   └── EventBuilder_S       — Builds events with 10µs coincidence window
│   ├── Woods-Saxon/       — Woods-Saxon potential tools
│   └── SRIM/              — SRIM stopping power tables
├── expName.sh             — Current experiment name + data path + LastRunNum
├── heliosrc.sh            — Bash environment setup
└── SetUpNewExp            — Experiment branch setup script (interactive, for humans)
```

---

## Experiment Workflow

### Branch Convention
- Each experiment gets its own git branch: `h094_19Ne_pp`, `h095_11C_dp_2`, etc.
- `master` branch = between experiments (ARR01)
- DAQ and MAC must **always be on the same branch**
- `analysis/working/` is experiment-specific — all files change per experiment
- `analysis/Armory/`, `Cleopatra/`, `EventBuilder/` are shared — do not change per experiment

### Switching Experiments
Use `SetUpNewExp <expName>` on MAC and DAQ. This script:
1. `git fetch` + checks repo is clean
2. Checks disk space
3. Checks out or creates the experiment branch
4. Creates data directories and symbolic links
5. Notifies InfluxDB database
6. If new branch: cleans `working/`, commits, pushes

### Data Flow
1. Raw data acquired by DAQ → stored at `<daqDataPath>/<expName>/` (no subdirs on DAQ)
2. EventBuilder_S builds coincidence events (10µs window)
3. `Process_RUN` sorts → ROOT files at `analysis/root_data/gen_run{N}.root`
4. `Monitors.C` / `ChainMonitors.C` for online and offline monitoring
5. `AutoCalibrationTrace.C` / Armory for energy and position calibration
6. `Transfer` + `Cleopatra` for kinematic simulation and DWBA
7. Globus transfer to LCRC for archival and parallel analysis

### Calibration Sequence (alpha + beam)
1. Option 0: Energy + xf/xn calibration (alpha source)
2. Option 5: X-scale calibration
3. Option 1: xf+xn → energy calibration (beam run)
4. `ln -sf correction_e_alpha.dat correction_e.dat`
5. Option 3: Generate calibrated ROOT file
6. Option 2 (optional): Kinematic auto-calibration if states visible
7. `ln -sf correction_e_KE.dat correction_e.dat` then option 3 again

---

## DAQ

### VME Hardware
- 4 digitizer VME crates (VME01–04), each with 4 MDIG boards
- 1 trigger crate (VME32)
- 16 digitizer boards total (board IDs 101–116)
- VxWorks 5.5 on all VME IOCs

### EPICS
- EPICS 7 soft IOC on DAQ for run control PVs
- VME IOCs broadcast Channel Access on 192.168.1.0/24 subnet
- Key PVs: `VMExx:MDIGn:led_threshold<ch>`, `VMExx:MDIGn:channel_enable<ch>`
- Run control: `Online_CS_StartStop`, `Online_CS_SaveData`
- Threshold PV pattern: `VMExx:MDIGn:led_threshold<ch>` (ch = 0–9)

### HV Control
- Iseg MPOD at 192.168.1.155, SNMP v2c, WIENER-CRATE-MIB, community `guru`
- HV mapping defined per-experiment in `IsegSNMPGUI/<expName>.csv`
- Controlled via `IsegSNMPGUI/IsegGUI.py` (requires display) or `IsegMonitor.py` (headless)
- ⚠️ Always ramp HV in 10–20V steps; stop if leakage current > 2µA

---

## LCRC Usage

In LCRC, **always use the master branch**:
```bash
cd /lcrc/project/HELIOS/digios
# Edit expName.sh to set the experiment name
# Run MakeDataLinks to create symbolic links to data
```
Do not switch branches on LCRC — use `expName.sh` and `MakeDataLinks` instead.

---

## Notes
- `SetUpNewExp` is protected by `.gitattributes` (`merge=ours`) — master version is always preserved on merge
- `GeneralSortMapping.h` is the authoritative detector map — EDM panels may be stale
- Det index 11 is always dead/disabled
- `correction_e.dat` should always be a symlink to either `correction_e_alpha.dat` or `correction_e_KE.dat`
- Do not use `GLBL:DIG:*` PVs — use individual `VMExx:MDIGn:*` PVs directly
