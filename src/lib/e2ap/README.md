FlexRIC supports multiple O-RAN E2AP versions - v1.01/v2.03/v3.01.

By default, E2AP v2.03 is compiled. If you would like to change the version, please, execute this command:
```bash
mkdir build && cd build && cmake .. -DE2AP_VERSION=<e2ap_version> && make -j8
sudo make install
```
Note: Command `sudo make install` installs shared libraries that represent Service Models. Every time E2AP or/and KPM versions are modified, this command must be executed afterwards.
