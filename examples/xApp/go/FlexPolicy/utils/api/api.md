# Slice Utils Functionalities

This package provides utility functions for creating xApps compatible with Non-RT RIC.
A plethora of useful functionalities are provided:
- `SliceStatsDict`:  A global structure for storing the latest indication message from the SLICE Service Model (SM). The variable with name `"SliceStats"` of type `SliceStatsDict` is used to store & retreive the latest indication message.

- `ReadSliceStats()`:  A function for parsing the desired stats items from the global structure and specifically from the variable with name `"SliceStats"`.

  *Tip: Consider reading all the desired values at once in a single iteration of the policy enforcement. In other words, do not call this function multiple times inside the policy enforcement callback. Otherwise, inconsistencies may occur if the global structure is updated between the different readings.*

  The best tactic is to extend the function to include more cases, which retrieve the desired data. See the `"multiple_rntis_num_of_ues"` case, which returns multiple items at once. When extending the function, be mindful to keep the mutex locking consistency.

- `SliceIndToDictJSON()`: A function for storing the latest indication message to the global structure and save an output to a json file. This function is called by the SLICE SM Callback function in order to store the latest indication message.

- `CreateSlice() (C SDK)`: A function for creating a slice of type `xapp.Fr_slice_t`.

- `RanNametoInt()`: A Function for returning the appropriate Integer for the input RAN Name.

- `Request`: A global structure for defining a control message request to the RIC.

- `FillSliceCtrlMsg() (C SDK)`: A function for filling the control message of type `xapp.Slice_ctrl_msg_t`.

- `InitNetwork()`: A Function to initialize the network with one normal slice (id=0) and one idle slice (id=2). Used only in DEBUG mode and for specific Use Cases usually for demos.

- `XAppConf`: A global structure for defining the xApp configuration. E.g. IP Address of Non-RT RIC, Mode (DEBUG or PRODUCTION). The configuration is stored in `"XAppConfVar"` variable.

- `ParseXappConfig()`: A function for parsing the xApp configuration.

- `Init()`: A function for initializing the xApp.

- `Configuration`:  A global structure for storing the xApp policy configuration coming from the Non-RT RIC via the APIs.

- `StartXAppServer()`: A function for starting the xApp server APIs. Currently, three APIs are supported:
  - `/api/policy`: For receiving the policy configuration. When receiving the new Policy, the Policy Callback function `PolicyEnforcementCallback()` is called to enforce the Policy. The policy Callback function is defined inside the xApp.
  - `/api/finish`: For receiving the finish command to stop the xApp.
  - `/api/slice/stats`: For sending the current state `"SliceStats"` to the Non-RT RIC.


# xApp Functionalities 
- `PolicyEnforcementCallback()`: A callback function that is called from the API  `/api/policy` in order to enforce the new policy.


# How to write the xApp
## Define Policy enforcement Logic:
Define the main logic of how the policy should be enforced inside the `PolicyEnforcementCallback()` function inside the xApp. The new Policy is coming in its input variable `PolicyConfiguration`. Parse it and write the code to enforce the policy using the Slice Utils functionalities to read the current state of the RAN (indication messages) and apply control acions. 
