# FlexPolicy
Flexible Policy Enforcement xApp

## Description

FlexPolicy is a Go-based xApp that allows users to enforce A1 policies to the RAN received from the ODIN controller. The xApp exposes a Gin server with three APIs to be used by ODIN:
```
-POST /api/policy:     Odin posts the policy configuration
-POST /api/finish:     Odin posts the finish command to terminate the xApp
-GET  /api/feedback:   Odin gets the current state of the policy
```

---

## Folder Structure

```
FlexPolicy/
├── callbacks/                     # Policy Callbacks
│   ├── cb_policy.go               # Callback function enforcing a policy
│   ├── ...                        # More callbacks
│   └── registry.go                # Registry for all policies callbacks
├── conf_files/                    # Configuration Files
│   ├── policy.conf                # Conf file specifying a unique policy
│   └── ...                        # More conf files
├── non_rt_emu/                    # Emulation of ODIN Non-Real Time RIC
│   ├── policy/                    # Gin client to consume APIs
│   │   └── non_rt_policy_main.go  # Client use case for a specific policy type
│   └── ...                        # More clients for other policy types
├── utils/                         # Utility functions
│   ├── api/                       # A1 server APIs utils
│   ├── parser/                    # A1 params parser from conf file 
│   ├── policy/                    # A1 Policy utils
│   └── sm/                        # Service model utils
│       ├── slice/                 # Slice service model utils
│       ├── mac/                   # Mac service model utils
│       └── sm.go                  # SM subscription utils
├── README.md                      # README
└── xapp_flex_policy_main.go       # FlexPolicy Main Function
```

### callbacks/

This folder contains the callback functions that represents the supported policies. Each callback enforces with a unique way a specific policy type. The callbacks are registerded automatically using the `registry.go`. When a new policy description is received at the APIs, the appropriate callback is chosen to enforce it.

### conf_files/

This folder contains configuration file for specific policy types. This is used when executing the FlexPolicy xApp in order to specify which specific policy callback should be used to enforce policy descriptions of this type.

### non_rt_emu/

This folder contains example scripts of Gin clients that send policy description to the FlexPolicy xApp to emulate ODIN behavior.

### utils/

This folder contains all the necessary utility functions to realize the policy enforcement lifecycle, including APIs, parsing, service models utilization and policy description.

---

## How to Run the FlexPolicy xApp

### Prerequisites

- Go version at least 1.19

### Steps

1. **Build Repo with Go enabled**
    ```bash
    mkdir build && cd build
    cmake ../ -DXAPP_GO_SWIG=ON && make -j && sudo make install
    ```

2. **Run E2 Nodes and RIC**
    Prepare the environment appropriately by running the E2 Nodes and the Near-RT RIC
    ```
    # terminal 1: start E2 Node agent
    ./build/examples/emulator/agent/emu_agent_gnb
    
    # terminal 2: start nearRT-RIC
    ./build/examples/ric/nearRT-RIC 
    ```

3. **Execute FlexPolicy giving a conf file**
    ```bash
    /path/to/go_xapp_flex_policy -c /path/to/conf_file
    ```

    For example:
    ```bash
    /usr/local/bin/flexric/xApp/go/go_xapp_flex_policy -c /usr/local/etc/flexric/a1_slice_enforce.conf
    ```
4. **Execute Non-RT RIC emulator**
    ```bash
    /path/to/go_non_rt_policy 
    ```

    For example:
    ```
    /usr/local/bin/flexric/xApp/go/go_non_rt_slice_enforce
    ```

---

