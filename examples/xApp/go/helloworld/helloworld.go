package main

import "C"
import (
	xapp "final.com/xapp_sdk"
	"fmt"
	"time"
)

func RanNametoInt(value string) int {
	switch value {
	case "ngran_eNB":
		return 0
	case "ngran_ng_eNB":
		return 1
	case "ngran_gNB":
		return 2
	case "ngran_eNB_CU":
		return 3
	case "ngran_ng_eNB_CU":
		return 4
	case "ngran_gNB_CU":
		return 5
	case "ngran_eNB_DU":
		return 6
	case "ngran_gNB_DU":
		return 7
	case "ngran_eNB_MBMS_STA":
		return 8
	}
	return -1
}

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
		var e2Node xapp.E2Node = conn.Get(int(i))
		// TODO: Add condition if node is monolothic
		var ranTypeName string = xapp.Get_ngran_name(e2Node.GetId().GetXtype())
		// if Node is monolithic
		if xapp.Ngran_eNB == RanNametoInt(ranTypeName) ||
			xapp.Ngran_ng_eNB == RanNametoInt(ranTypeName) ||
			xapp.Ngran_gNB == RanNametoInt(ranTypeName) {
			fmt.Printf("E2 node %d info: nb_id %d, mcc %d, mnc %d, mnc_digit_len %d, ran_type %s\n",
				i,
				e2Node.GetId().GetNb_id(),
				e2Node.GetId().GetPlmn().GetMcc(),
				e2Node.GetId().GetPlmn().GetMnc(),
				e2Node.GetId().GetPlmn().GetMnc_digit_len(),
				ranTypeName,
			)
		} else {
			fmt.Printf("E2 node %d info: nb_id %d, mcc %d, mnc %d, mnc_digit_len %d, ran_type %s, cu_du_id %d\n",
				i,
				e2Node.GetId().GetNb_id(),
				e2Node.GetId().GetPlmn().GetMcc(),
				e2Node.GetId().GetPlmn().GetMnc(),
				e2Node.GetId().GetPlmn().GetMnc_digit_len(),
				ranTypeName,
				*e2Node.GetId().GetCu_du_id(),
			)
		}
		fmt.Printf("E2 node %d supported RAN function's IDs: \n", i)
		// TODO: print all the RAN function
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
