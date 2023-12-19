FlexRIC supports multiple O-RAN KPM SM versions - v2.01/v2.03/v3.00.

By default, KPM v2.03 is compiled. If you would like to change the version, please, execute this command:
```bash
mkdir build && cd build && cmake .. -DKPM_VERSION=<kpm_version> && make -j8
sudo make install
```
Note: Command `sudo make install` installs shared libraries that represent Service Models. Every time E2AP or/and KPM versions are modified, this command must be executed afterwards.

The document 28_552_kpm_meas.txt contains the list of all measurements defined in 3GPP TS 28.552.

At the moment, the following measurements are supported in OAI RAN:
"DRB.PdcpSduVolumeDL"
"DRB.PdcpSduVolumeUL"
"DRB.RlcSduDelayDl"
"DRB.UEThpDl"
"DRB.UEThpUl"
"RRU.PrbTotDl"
"RRU.PrbTotUl"
