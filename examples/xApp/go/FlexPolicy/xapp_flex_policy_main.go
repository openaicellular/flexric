package main

import "C"
import (
	callbacks "build/examples/xApp/go/FlexPolicy/callbacks"
	api "build/examples/xApp/go/FlexPolicy/utils/api"
	sm "build/examples/xApp/go/FlexPolicy/utils/sm"
	xapp "build/examples/xApp/go/xapp_sdk"

	"fmt"
	"os"
	"time"
)

func main() {

	// Initialization
	xapp.Init(xapp.SlToStrVec(os.Args))

	// Parse A1 parameters
	a1 := xapp.Get_conf("xApp_A1")
	A1IP := a1.Get("ip")
	A1Port := a1.Get("port")
	policyID := a1.Get("policy")

	// Get sm
	custSm := xapp.Get_cust_sm_conf()
	oranSm := xapp.Get_oran_sm_conf()
	fmt.Printf("A1: IP %s, PORT %s, POLICY %s\n", A1IP, A1Port, policyID)

	// E2 Connect
	sm.E2Nodes = xapp.Conn_e2_nodes()

	var nodes_len int64 = sm.E2Nodes.Size()

	if nodes_len <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	// Subscribe to the SMs
	sm.SmSubscription(custSm, oranSm)

	// find policy callback id
	callback := callbacks.FindCallback(policyID)
	if callback == nil {
		fmt.Printf("Policy Callback with given ID %s not found.\n", policyID)
	} else {
		api.OpenA1Apis(callback, A1IP, A1Port)
	}

	// Unsubscribe from the SMs
	sm.SmUnsubscription(custSm, oranSm)

	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")

}
