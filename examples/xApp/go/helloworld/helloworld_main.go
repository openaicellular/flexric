package main

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	"fmt"
	"time"
	"os"
)

// ------------------------------------------------------------------------ //
//  Function for returning the appropriate Int for the RAN Name
// ------------------------------------------------------------------------ //
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
	xapp.Init(xapp.SlToStrVec(os.Args))

	var nodes xapp.E2NodeVector = xapp.Conn_e2_nodes()

	if nodes.Size() <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes.Size())


	var i int64 = 0
	for ; i <= nodes.Size()-1; i++ {
		var e2Node xapp.E2Node = nodes.Get(int(i))
		
		var ranTypeName string = xapp.Get_e2ap_ngran_name(e2Node.GetId().GetXtype())
	
		// if Node is monolithic
		if xapp.E2ap_ngran_eNB == RanNametoInt(ranTypeName) ||
			xapp.E2ap_ngran_ng_eNB == RanNametoInt(ranTypeName) ||
			xapp.E2ap_ngran_gNB == RanNametoInt(ranTypeName) {
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
				e2Node.GetId().GetCu_du_id(),
			)
		}
		fmt.Printf("E2 node %d supported RAN function's IDs: \n", i)

		var j int64 = 0
		for ; j <= nodes.Get(int(i)).GetRan_func().Size()-1; j++ {
			fmt.Printf(", %d", int(nodes.Get(int(i)).GetRan_func().Get(int(j)).GetId()))
		}
		fmt.Printf("\n")
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
