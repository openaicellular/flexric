# FlexRIC

This repository contains [O-RAN Alliance](https://www.o-ran.org/) compliant E2 node Agent emulators, a NearRT-RIC, xApps written in C/C++, Python and Go. 
It implements O-RAN service models (KPM v2, KPM v3, and RC) and various customized service models (NG/GTP, PDCP, RLC, MAC, SLICE, TC) and a built-in emulation.
Depending on the service model, different encoding schemes have been developed (ASN.1, flatbuffer, plain).
The indication data received in the xApp uses SQLite3 or MySQL database to store the data for enabling offline processing applications
(e.g., ML/AI). 

If you want to know more about FlexRIC and its original architecture, you can find more details at: Robert Schmidt, Mikel Irazabal, and Navid Nikaein. 2021.
FlexRIC: an SDK for next-generation SD-RANs. In Proceedings of the 17th International Conference on emerging Networking EXperiments and Technologies (CoNEXT
'21). Association for Computing Machinery, New York, NY, USA, 411–425. DOI: https://doi.org/10.1145/3485983.3494870. A pdf copy is available at
https://bit.ly/3uOXuCV 

---

## 1. Installation

### 1.1 Install prerequisites

- **A recent CMake (at least v3.15)**

  On Ubuntu, you might want to use [this PPA](https://apt.kitware.com/) to install an up-to-date version.

- **GCC version (at least v10)**

  > **NOTE: avoid using gcc-11**

- **SWIG (at least  v.4.1)**

  We use SWIG as an interface generator to enable the multi-language feature (i.e., C/C++ and Python) for the xApps. Please, check your SWIG version (i.e, `swig
  -version`) and install it from scratch if necessary as described here: https://swig.org/svn.html or via the code below:
  - Required dependencies for SWIG:
    ```bash
    sudo apt install autotools-dev automake libpcre2-dev yacc build-essential
    ```
  - Install SWIG:
    ```bash
    git clone https://github.com/swig/swig.git
    cd swig
    git checkout v4.1.1
    ./autogen.sh
    ./configure --prefix=/usr/
    make
    make install
    ```

- (Optional) Flatbuffer encoding. 
  
  We also provide a flatbuffers encoding/decoding scheme as alternative to ASN.1. In case you want to use it, follow the
  instructions at https://github.com/dvidelabs/flatcc and provide the path for the lib and include when selecting it at `ccmake ..` from the build directory 

### 1.2 Download the required dependencies.

- **Install common dependencies in Ubuntu:  (at least python3.8)**
  ```bash
  sudo apt install libsctp-dev python3 cmake-curses-gui python3-dev pkg-config libconfig-dev libconfig++-dev
  ```

- **Install MySQL as a storage for xApps:**
  ```bash
  sudo apt install libmysqlclient-dev mysql-server
  ```

- (Optional) Install websocket for proxy agent:
  ```bash
  sudo apt install libwebsockets-dev libjson-c-dev
  ```
  > Proxy agent has been tested with specific version of `libwebsockets-dev` and `libjson-c-dev`;
  cmake will check for that and promt an error in case the version you are trying to install is not compatible.

- (Optional) Install Go for xApp:
  ```bash
  sudo snap install go --channel=1.19/stable
  ```

- (Optional) Install Python ML xApp:
  ```
  sudo add-apt-repository -y ppa:deadsnakes/ppa
  sudo apt install libsnappy-dev python3.11 python3.11-dev
  ```
  
  
### 1.3 Clone the FlexRIC project, build and install it. 

- **Download the code**

  Check the [release page](https://gitlab.eurecom.fr/mosaic5g/flexric/-/releases) and read the release notes for deciding which release you want to install. You
  can download directly the code from the web or use git in the following way:

  ```bash
  # i.e.: 
  $ git clone https://gitlab.eurecom.fr/mosaic5g/flexric.git
  $ git checkout br-flexric
  ```

- **Build and install**

  List of options in CmakeList:
  - `E2AP_VERSION=E2AP_V1/E2AP_V2/E2AP_V3` (E2AP_V2 by default)
  - `KPM_VERSION=KPM_V2/KPM_V3` (KPM_V3 by default)
  - `XAPP_DB=SQLITE3_XAPP/MYSQL_XAPP/NONE_XAPP` (MYSQL_XAPP by default)
  - `XAPP_PYTHON_SWIG=ON/OFF` (ON by default)
  - `XAPP_GO_SWIG=ON/OFF` (OFF by default)
  - `PROXY_AGENT=ON/OFF` (OFF by default)
   ```bash
   $ cd flexric
   $ mkdir build
   $ cd build
   $ cmake -D{your options} ..
   $ make -j
   $ sudo make install
   ```
  > By default the service model libraries will be installed in the path `/usr/local/lib/flexric` while the
  configuration files in `/usr/local/etc/flexric`. 


### 1.4 Unit Test (Optional step)

- Ctest
  ```bash
  cd build/
  ctest
  ```
- Service Model unit test
  ```bash
  cd build/test/sm
  # example: KPM
  cd sm/kpm_sm/kpm_sm_v03.00
  ./test_kpm_sm
  ```
---

## 2. Usage

The default configuration assumes all the components are located in the same localhost.

### 2.1 Steps to run
- Terminal1: start the nearRT-RIC
  ```bash
  $ /usr/local/bin/flexric/ric/nearRT-RIC
  ```
- Terminal2: start first E2-emulator:
  ```bash
  $ /usr/local/bin/flexric/e2agent/emu_agent_gnb
  ```
  > Note: you can run up multiple E2-emulators depends on your scenario, four different type E2-emulators are provided: `emu_agent_gnb`, `emu_agent_gnb_du`, `emu_agent_gnb_cu`, `emu_agent_enb`
- Terminal3: start monitoring ORAN service models xApp in C:
  ```bash
  $ /usr/local/bin/flexric/xApp/c/xapp_oran_moni -c /usr/local/etc/flexric/xapp_oran_sm.conf
  ```
- Terminal4: start monitoring ORAN service models xApp in Python:
  ```bash
  $ python3 /usr/local/lib/python3/dist-packages/monitor/xapp_oran_moni.py -c /usr/local/etc/flexric/xapp_oran_sm.conf
  ```
- (Optional) Terminal5: start monitoring customized service models xApp in Go:
  ```bash
  $ /usr/local/bin/flexric/xApp/go/go_xapp_cust_moni -c /usr/local/etc/flexric/xapp_cust_sm.conf
  ```
  
### 2.2 Configuration

> By default, without assigning any .conf file while running the executables (ex: `nearRT-RIC`, `emu_agent_gnb`) with flag `-c`, all the executables will use `/usr/local/etc/flexric/ric/ric.conf`.

- **configuration file for nearRT-RIC:**

  In `ric.conf`, we need to assign IP address for nearRT-RIC, and Port for both E2 and E42 interface:
  ```
  Name = "NearRT_RIC"
  NearRT_RIC_IP = "127.0.0.1"
  E2_Port = 36421
  E42_Port = 36422
  ```
- **configuration file for E2-emulator:**

  In `e2agent.conf`, we need to assign IP address to connect to nearRT-RIC, and Port for both E2 interface:
  ```
  Name = "E2_Agent"
  NearRT_RIC_IP = "127.0.0.1"
  E2_Port = 36421
  ```
- **configuration file for xApp:**

  In `xapp_cust_sm.conf`, `xapp_oran_sm.conf`, and `xapp_all_sm.conf`,
  we need to assign IP address to connect to nearRT-RIC, and Port for both E42 interface:
  ```
  Name = "xApp"
  NearRT_RIC_IP = "127.0.0.1"
  E42_Port = 36422
  ```
  To subscribe specific service models by xApp, the required info of service models are written in the config:
  - subscribe to customized service models:
    - supported name : MAC, RLC, PDCP, GTP, SLICE;
    - supported time: 1_ms, 2_ms, 5_ms, 10_ms, 100_ms, 1000_ms;
  - example in  `xapp_cust_sm.conf`:
    ```
     Sub_CUST_SM_List = (
         { name = "MAC", time = "100_ms" },
         { name = "RLC", time = "100_ms" },
         { name = "PDCP", time = "100_ms" }
     )
     ```
  - subscribe to ORAN service models:
    - supported name : KPM
    - supported time (ms): 1, 2, 5, 10, 100, 1000
    - supported format: 1, 4 
    - supported ran_type: ngran_gNB, ngran_gNB_CU, ngran_gNB_DU
  - example in `xapp_oran_sm.conf`:
    ```
    Sub_ORAN_SM_List = (
    { name = "KPM", time = 1000,
      format = 4,
      ran_type = "ngran_gNB",
      actions = (
            { name = "DRB.PdcpSduVolumeDL" },
            { name = "DRB.PdcpSduVolumeUL" },
            { name = "DRB.RlcSduDelayDl" },
            { name = "DRB.UEThpDl" },
            { name = "RRU.UEThpUl" },
            { name = "RRU.PrbTotDl" },
            { name = "RRU.PrbTotUl" }
            )
    },
    { name = "KPM", time = 1000,
      format = 4,
      ran_type = "ngran_gNB_DU",
      actions = (
            { name = "DRB.RlcSduDelayDl" },
            { name = "DRB.UEThpDl" },
            { name = "RRU.UEThpUl" },
            { name = "RRU.PrbTotDl" },
            { name = "RRU.PrbTotUl" }
            )
    },
    { name = "KPM", time = 1000,
      format = 4,
      ran_type = "ngran_gNB_CU",
      actions = (
            { name = "DRB.PdcpSduVolumeDL" },
            { name = "DRB.PdcpSduVolumeUL" }
            )
    }
    )
    ```
  To configure database:

  - example by default:
  ```
  xApp_DB = {
      enable = "ON"
      ip = "127.0.0.1"
      dir = "/tmp/"
      filename = "testdb"
      username = "xapp" # if using mysql
      password = "eurecom" # if using mysql
  }
  ```
  - if you want to disable using database while running the specific xApp:
  ```
  xApp_DB = {
      enable = "OFF"
  }
  ```

### 2.3 Wireshark
Configure the preference for the port number of E2AP protocol to be able see the E2 message between E2-emulator and nearRT-RIC.

---
# [to be completed, not yet updated]

## 3. Integration with RAN and example of deployment

### 3.1 Integration with OpenAirInterface 4G/5G RAN

We will use a specific branch provided in OAI repository. Below the commands to achieve that.
```bash
$ git clone https://gitlab.eurecom.fr/oai/openairinterface5g.git oai
$ cd oai/
$ git checkout develop
$ source oaienv
$ cd cmake_targets
$ ./build_oai -i -I  #For OAI first time installation. it will install some dependencies
# to test with USRP
$ ./build_oai --gNB -c -C -w USRP --build-e2 --ninja
# to test with 5G RF sim
$ ./build_oai --gNB --nrUE -c -C -w SIMU --build-e2 --ninja
```

The compilation of OAI may take 10 minutes. Example configuration files using a B210 USRP are provided in flexric to facilitate the integration.

### 3.2 Integration with srsRAN 4G RAN

Install srsRAN from source following [their](https://docs.srsran.com/en/latest/general/source/1_installation.html#installation-from-source) instructions.

We will use a patch provided in the flexric repository. Below the commands to do that:
```bash
cd path/to/srsran
git checkout release_21_10
git am path/to/flexric/multiRAT/srsran/srsenb.patch --whitespace=nowarn
mkdir build && cmake .. && make -j8 
cd srsenb/src/
sudo ./srsenb
```

### 3.3 (opt) Synchronize clock

Before running the various components (RAN/flexric), you probably want to align the machines' clock. At this aim, you can use `ptp4l` in all the machines
involved (if you have for example deployed the various components on different hosts)

```bash
sudo ptp4l -m -i InterfaceName #for master
sudo ptp4l -m -i InterfaceName -s #for slaves
```

![alt text](fig/2.png)

Following make sure that no ntpd, chrondy or timesyncd is running in the system (e.g., `sudo systemctl stop systemd-timesyncd`). 

```bash
sudo phc2sys -m -s InterfaceName -w
```

![alt text](fig/3.png)

### 3.4 Start the processes

* Start some eNodeB/gNodeB

  Below an example with 5G or 4G OAI gNodeB/eNodeB
  ```bash
  # eNB
  $ cd oai/cmake_targets/ran_build/build
  $ sudo ./lte-softmodem -O path/to/flexric/multiRAT/oai/enb.band7.tm1.25PRB.usrpb210.replay.conf

  # gNB
  $ cd oai/cmake_targets/ran_build/build
  $ sudo ./nr-softmodem -O path/to/flexric/multiRAT/oai/gnb.sa.band78.fr1.106PRB.usrpb210.conf --sa -E --continuous-tx
  ```

* Start the nearRT-RIC

  ```bash
  $ ./build/examples/ric/nearRT-RIC
  ```

* Start different xApps

  e.g, start the monitoring xApp with `build/examples/xApp/c/monitor/xapp_mac_rlc_pdcp_moni`. The monitoring sequence diagram is represented below: 

  ![alt text](fig/4.png)


  e.g, start the slicing control xApp via `$ python3 build/examples/xApp/python3/xapp_slice_moni_ctrl.py`. The controlling sequence diagram is represented below:

  ![alt text](fig/5.png)

## 4. Support/further resources

* Mailing list: if you need help or have some questions, you can subscribe to the mailing list `techs@mosaic-5g.io` that you can find at
  https://gitlab.eurecom.fr/mosaic5g/mosaic5g/-/wikis/mailing-lists. The emails are archived and available publicly. 
* [Demo](DEMO.md) for flexric in July 2022
* [The Wiki space](https://gitlab.eurecom.fr/mosaic5g/flexric/-/wikis/home) contains tutorials and presentations
* [Original FlexRIC paper ACM CoNEXT 2021](https://bit.ly/3uOXuCV)

## 5. Roadmap

Check on https://gitlab.eurecom.fr/mosaic5g/flexric/-/milestones and in https://openairinterface.org/mosaic5g/
