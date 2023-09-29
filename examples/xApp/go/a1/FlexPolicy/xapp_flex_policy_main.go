package main

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	parser "build/examples/xApp/go/a1/utils/parser"
	callbacks "build/examples/xApp/go/a1/FlexPolicy/callbacks"
	sm "build/examples/xApp/go/a1/utils/sm"
	api "build/examples/xApp/go/a1/utils/api"

	"fmt"
	"time"
)

// ------------------------------------------------------------------------ //
//  MAIN
// ------------------------------------------------------------------------ //
func main() {
	// ----------------------- Initialization ----------------------- //
	args := []string{"", "-c", "flex_policy.conf"}
	xapp.Init(xapp.SlToStrVec(args))

	// Parse A1 parameters
	A1IP, A1Port, smList, xappID := parser.ParseXAppConfig("flex_policy.conf")

	// Connect
	sm.E2Nodes = xapp.Conn_e2_nodes()

	var nodes_len int64 = sm.E2Nodes.Size()

	if nodes_len <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	// Subscribe to the SMs
	sm.SmSubscription(smList)

	// find the xApp Callback ID
	if callback, found := callbacks.Registry[xappID]; found {
		api.OpenA1Apis(callback, A1IP, A1Port)
	} else {
		fmt.Printf("xApp Callback with given ID %s not found.\n", xappID)
	}

	// Unsubscribe from the SMs
	sm.SmUnsubscription(smList)

	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}
