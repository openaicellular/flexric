package main

import "C"
import (
	xapp "final.com/xapp_sdk"
	"fmt"
	"time"
)

// ------------------------------------------------------------------------ //
//
//	MAIN
//
// ------------------------------------------------------------------------ //
func main() {
	// Initial of xApp
	xapp.Init()

	// Connection of E2 Nodes
	var conn xapp.E2NodeVector = xapp.Conn_e2_nodes()

	var nodes_len int64 = conn.Size()

	if nodes_len <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	var i int64 = 0
	for ; i <= nodes_len-1; i++ {
		var ranType xapp.E2Node = conn.Get(int(i))
		// TODO: Add condition if node is monolothic
		fmt.Printf("E2 node %d info: nb_id %d, mcc %d, mnc %d, mnc_digit_len %d\n",
			i,
			ranType.GetId().GetNb_id(),
			ranType.GetId().GetPlmn().GetMcc(),
			ranType.GetId().GetPlmn().GetMnc(),
			ranType.GetId().GetPlmn().GetMnc_digit_len(),
			// TODO: get RAN type
		)
	}

	// ----------------------- END ----------------------- //
	fmt.Printf("Hello World\n")
	// --------------------------------------------------------- //
	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}
