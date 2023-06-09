
> To build OAI gNB and nrUE, follow the instructions in [Integration with OpenAirInterface 4G/5G RAN](https://gitlab.eurecom.fr/mosaic5g/flexric/-/tree/br-flexric?ref_type=heads#31-integration-with-openairinterface-4g5g-ran)

After build the OAI gNB and nrUE, you can run the `nr-softmodem` and `nr-uesoftmodem` by following commands:

## Running OAI with RFSIM
### Run OAI gNB - `nr-softmodem`
> Note: modify the IP address for your gNB and AMF in `gnb_2023w19_rfsim.conf`
```bash
$ cd /path/to/oai/
$ cd cmake_targets/ran_build/build/
$ sudo ./nr-softmodem -O gnb_2023w19_rfsim.conf --sa
```
### Run OAI nrUE - `nr-uesoftmodem` with `ue_rfsim.conf`
> Note: modify the configuration of `uicc0` in `ue_rfsim.conf` based on your core network setting
```bash
$ cd /path/to/oai/
$ cd cmake_targets/ran_build/build/
$ sudo ./nr-uesoftmodem -r 106 --numerology 1 --band 78 -C 3619200000 --rfsim --sa --nokrnmod -O ue_rfsim.conf
```

## Running OAI with USRP B210
TODO