package main

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	"fmt"
	"time"
	"os"
)

// ------------------------------------------------------------------------ //
//  GTP INDICATION CALLBACK
// ------------------------------------------------------------------------ //
type GTPCallback struct {

}

func (gtp_cb GTPCallback) Handle(ind xapp.Swig_gtp_ind_msg_t) {
	if ind.GetGtp_stats().Size() > 0 {
		nowTime := time.Now().UnixNano()/1000   //in microseconds
		indTime := ind.GetTstamp() / 1.0
		diff := nowTime - indTime
		fmt.Printf("GTP Indication latency %f microsecs\n", float64(diff))
	}	
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
		// plmn
	
	}

	// GTP INDICATION
	var handleSlice []int

	i = 0
	for ; i <= nodes.Size()-1; i++ {
		// ----------------------- GTP Indication ----------------------- //
		innerGtp := GTPCallback{}
		callbackGtp := xapp.NewDirectorGtp_cb(innerGtp)
		HndlrGtp := xapp.Report_gtp_sm(nodes.Get(int(i)).GetId(), xapp.Interval_ms_10, callbackGtp)

		// Append values to the slice
		handleSlice = append(handleSlice, HndlrGtp)

		time.Sleep(1 * time.Second)
	}

	time.Sleep(5 * time.Second)

	// unsubscribe from sms
	i = 0
	for _, value := range handleSlice {
		xapp.Rm_report_gtp_sm(value)
	}

	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}
