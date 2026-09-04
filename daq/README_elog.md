# elog client for the DAQ

The DAQ scripts (`start_run_Mac.sh`, `stop_run_Mac.sh`, `push2Elog.sh`,
`edm/scripts/WriteComment`) post to `elog.phy.anl.gov` which sits behind
CloudFlare. This requires the **`elog-ryan`** client, built from the **`ANL`
branch** — the default `master` branch is vanilla upstream and does **not**
return the entry ID against the CloudFlare-fronted server.

## Install

```bash
cd ~
git clone https://gitlab.phy.anl.gov/ttang/elog-ryan.git
cd elog-ryan
git checkout ANL                       # <-- required; do not use master
git submodule update --init --recursive

mkdir -p ~/elog-ryan-build
cd ~/elog-ryan-build
cmake ~/elog-ryan -DCMAKE_BUILD_TYPE=Release
make elog -j4
```

The DAQ scripts look for the binary at `~/elog-ryan-build/elog`.

## Verify

```bash
~/elog-ryan-build/elog -s -p 443 -h elog.phy.anl.gov -l ARR01 \
    -u MasterHelios helios \
    -a Category=Runs -a Subject="install check" \
    -n 1 "install check"
```

Expected output:

```
Message successfully transmitted, ID=<number>
```

If you see empty output, or `elogd server does not run SSL protocol`, you are
running the wrong binary (upstream `master`, or the system `/usr/local/bin/elog`).
Confirm with `which elog` and rebuild from the `ANL` branch.
