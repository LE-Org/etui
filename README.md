# EPICS Terminal User Interface

ETUI is a minimalist EPICS user interface using NCURSES which provides information about a list of PVs.

## Quickstart
- Build
```bash
# modify RELEASE to set EPICS_BASE
vim configure/RELEASE
make
```
- Create a list of PVs
```bash
iocsh> dbl > pv.lst
```
- Open it
```bash
etui pv.lst
```

## Built with
- [ncurses](https://invisible-island.net/ncurses/)
- [ca](https://github.com/epics-base/epics-base)
