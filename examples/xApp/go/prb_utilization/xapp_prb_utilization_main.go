package main

import "C"
import (
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
	xapp "build/examples/xApp/go/xapp_sdk"
	"fmt"
	"os"
	"sync"
	"time"
)

// Mutex for locking the global structure
var Mutex sync.Mutex

// Global variable to store the stats of UEs
var MultipleUeStatistics MultiUeStats

type UePrbCalculationMetrics struct {
	DLRbCount     int
	DiffDlSlotSt  int
	DiffDlSlotEnd int
	DlFrameSt     int
	DlSlotSt      int
	DlAggrPrbSt   int
	NumberOfSlots int
	DiffDlFr      int
}

// Global Structure for UE Stats
type UeStats struct {
	Qfi            uint
	Teidupf        uint
	Teidgnb        uint
	PrbUtilisation int
	UePrbMetrics   UePrbCalculationMetrics
}

type RNTI uint

type MultiUeStats struct {
	Stats    map[RNTI]UeStats // map of RNTI to UEStats
	NumOfUEs int
}

// ------------------------------------------------------------------------ //
//
//	SLICE INDICATION CALLBACK
//
// ------------------------------------------------------------------------ //
type SLICECallback struct {
}

func (c SLICECallback) Handle(ind xapp.Swig_slice_ind_msg_t) {
	// store ind to global structure
	slice.SliceIndToDictJSON(ind)

	e2nodeId := slice.E2NodeId{
		Mcc:     ind.GetId().GetPlmn().GetMcc(),
		Mnc:     ind.GetId().GetPlmn().GetMnc(),
		NbId:    uint32(ind.GetId().GetNb_id().GetNb_id()),
		CuDuId:  0,
		RanType: xapp.Get_e2ap_ngran_name(ind.GetId().GetXtype()),
	}
	// read and store the number of UEs
	Rntis := slice.ReadSliceStats("rntis", -1, e2nodeId).([]uint16)

	Mutex.Lock()
	// store the num of UEs
	MultipleUeStatistics.NumOfUEs = len(Rntis)

	// store the rntis. If rnti already exists, do not add
	for i := 0; i < len(Rntis); i++ {
		// store the rntis as a key
		rnti := RNTI(Rntis[i])

		// check if exists and add
		if _, ok := MultipleUeStatistics.Stats[rnti]; !ok {
			MultipleUeStatistics.Stats[rnti] = UeStats{}
		}
	}
	Mutex.Unlock()
}

// ------------------------------------------------------------------------ //
//
//	GTP INDICATION CALLBACK
//
// ------------------------------------------------------------------------ //
type GTPCallback struct {
}

func (gtp_cb GTPCallback) Handle(ind xapp.Swig_gtp_ind_msg_t) {

	Mutex.Lock()

	// iterate over the number of UEs
	for i := 0; i < int(ind.GetGtp_stats().Size()); i++ {

		// extract the rnti, teidgnb, teidupf, qfi
		Rnti := RNTI(ind.GetGtp_stats().Get(i).GetRnti())
		Teidgnb := uint(ind.GetGtp_stats().Get(i).GetTeidgnb())
		Teidupf := uint(ind.GetGtp_stats().Get(i).GetTeidupf())
		Qfi := uint(ind.GetGtp_stats().Get(i).GetQfi())

		// check if rnti exists and store the stats
		if ue, ok := MultipleUeStatistics.Stats[Rnti]; ok {
			ue.Teidgnb = Teidgnb
			ue.Teidupf = Teidupf
			ue.Qfi = Qfi
			MultipleUeStatistics.Stats[Rnti] = ue
		}
	}
	Mutex.Unlock()

}

// ------------------------------------------------------------------------ //
//
//	MAC INDICATION CALLBACK
//
// ------------------------------------------------------------------------ //
type MACCallback struct {
}

func (mac_cb MACCallback) Handle(ind xapp.Swig_mac_ind_msg_t) {

	Mutex.Lock()

	Calculate_UE_PRB_utilisation(ind)

	Mutex.Unlock()
}

// Global variable for the bandwidth and numerology
var (
	cell_BandWidth = 40000000
	scs            = 30000
)

func Calculate_UE_PRB_utilisation(ind xapp.Swig_mac_ind_msg_t) {

	// iterate over the number of UEs
	for i := 0; i < int(ind.GetUe_stats().Size()); i++ {

		Rnti := RNTI(ind.GetUe_stats().Get(i).GetRnti())
		// extract the rnti, teidgnb, teidupf, qfi
		Frame := ind.GetUe_stats().Get(i).GetFrame()
		Slot := ind.GetUe_stats().Get(i).GetSlot()
		Dl_aggr_prb := ind.GetUe_stats().Get(i).GetDl_aggr_prb()

		// Check if the UE RNTI exists in the map
		if ue, ok := MultipleUeStatistics.Stats[Rnti]; ok {

			// Store the first indication messages
			if ue.UePrbMetrics.DLRbCount == 0 {

				ue.UePrbMetrics.DlFrameSt = int(Frame)
				ue.UePrbMetrics.DlSlotSt = int(Slot)
				ue.UePrbMetrics.DlAggrPrbSt = int(Dl_aggr_prb)

			}

			// Wait for 1 second to get the last indication messages
			ue.UePrbMetrics.DLRbCount++

			if ue.UePrbMetrics.DLRbCount == 101 {

				DlFrameEnd := int(Frame)
				DlSlotEnd := int(Slot)
				DlAggrPrbEnd := int(Dl_aggr_prb)

				//Calculate the frame difference between the start frame and the end frame

				//check if the end frame is less then the start frame
				if DlFrameEnd < ue.UePrbMetrics.DlFrameSt {

					for i := ue.UePrbMetrics.DlSlotSt; i <= 19; i++ {
						if i >= 0 && i <= 7 || i >= 10 && i <= 17 {
							ue.UePrbMetrics.DiffDlSlotSt += 1
						}
					}

					for i := 0; i <= DlSlotEnd; i++ {
						if i >= 0 && i <= 7 || i >= 10 && i <= 17 {
							ue.UePrbMetrics.DiffDlSlotEnd += 1
						}
					}

					// Frame size is always between 0 - 1023

					ue.UePrbMetrics.DiffDlFr = ((1023 - ue.UePrbMetrics.DlFrameSt) + DlFrameEnd) //-2

					ue.UePrbMetrics.NumberOfSlots = ue.UePrbMetrics.DiffDlSlotSt + (ue.UePrbMetrics.DiffDlFr * 16) + ue.UePrbMetrics.DiffDlSlotEnd
				}

				//check if the end frame is greater than the start frame
				if DlFrameEnd > ue.UePrbMetrics.DlFrameSt {

					for i := ue.UePrbMetrics.DlSlotSt; i <= 19; i++ {
						if i >= 0 && i <= 7 || i >= 10 && i <= 17 {
							ue.UePrbMetrics.DiffDlSlotSt += 1
						}
					}

					for i := 0; i <= DlSlotEnd; i++ {
						if i >= 0 && i <= 7 || i >= 10 && i <= 17 {
							ue.UePrbMetrics.DiffDlSlotEnd += 1
						}
					}
					ue.UePrbMetrics.DiffDlFr = (DlFrameEnd - ue.UePrbMetrics.DlFrameSt) - 1 //-2

					ue.UePrbMetrics.NumberOfSlots = ue.UePrbMetrics.DiffDlSlotSt + (ue.UePrbMetrics.DiffDlFr * 16) + ue.UePrbMetrics.DiffDlSlotEnd
				}

				// find the difference between the last aggregated prb blocks and first aggrated prb blocks
				dl_aggr_prb_per_second := DlAggrPrbEnd - ue.UePrbMetrics.DlAggrPrbSt

				//Calculat the total number of blocks by multiplying numberof slots with number of resource blocks per slot
				//The number of resource block per slots is calculated by considering bandwith 40 MHz and numerology 30khz = 106
				total_prbs_per_second := ue.UePrbMetrics.NumberOfSlots * 106 //((int(float64(cell_BandWidth)/(float64(12*scs))))-5)

				Load := int((float64(dl_aggr_prb_per_second) / float64(total_prbs_per_second)) * 100) // TODO: this assumes a 10ms interval

				//Store the calculated prb utilisation to the Global variable
				ue.PrbUtilisation = Load

				// fmt.Printf("prb_utilisation:  %d %%\n", int(Load))
				// fmt.Println("Bandwidth", cell_BandWidth)
				// fmt.Println("numerology", scs)
				// fmt.Println("Number of resource blocks", (int(float64(cell_BandWidth)/float64(12*scs)))-5)

				fmt.Println("Rnti: ", Rnti, ", Teidupf: ", ue.Teidupf, ", Teidgnb: ", ue.Teidgnb, ", Qfi: ", ue.Qfi, "Prb Util: ", Load, " %")

				ue.UePrbMetrics.DLRbCount = 0
				ue.UePrbMetrics.DiffDlSlotSt = 0
				ue.UePrbMetrics.DiffDlSlotEnd = 0

			}

			MultipleUeStatistics.Stats[Rnti] = ue
		}
	}
}

// Global Variables
var Conn xapp.E2NodeVector
var NodeIdx int
var Hndlr int

// ------------------------------------------------------------------------ //
//
//	MAIN
//
// ------------------------------------------------------------------------ //s
func main() {
	// ----------------------- Initialization ----------------------- //
	xapp.Init(xapp.SlToStrVec(os.Args))

	// Connect
	Conn = xapp.Conn_e2_nodes()

	var nodes_len int64 = Conn.Size()

	if nodes_len <= 0 {
		panic(fmt.Sprintf("panic"))
	}

	fmt.Printf("Connected E2 nodes = %d\n", nodes_len)

	NodeIdx = 0

	MultipleUeStatistics = MultiUeStats{
		Stats:    make(map[RNTI]UeStats),
		NumOfUEs: 0,
	}

	// ----------------------- SLICE Indication ----------------------- //
	innerSlice := SLICECallback{}
	callbackSlice := xapp.NewDirectorSlice_cb(innerSlice)
	HndlrSlice := xapp.Report_slice_sm(Conn.Get(NodeIdx).GetId(), xapp.Interval_ms_10, callbackSlice)
	time.Sleep(1 * time.Second)

	// ----------------------- MAC Indication ----------------------- //
	innerMac := MACCallback{}
	callbackMac := xapp.NewDirectorMac_cb(innerMac)
	HndlrMac := xapp.Report_mac_sm(Conn.Get(NodeIdx).GetId(), xapp.Interval_ms_10, callbackMac)
	time.Sleep(1 * time.Second)

	// ----------------------- GTP Indication ----------------------- //
	innerGtp := GTPCallback{}
	callbackGtp := xapp.NewDirectorGtp_cb(innerGtp)
	HndlrGtp := xapp.Report_gtp_sm(Conn.Get(NodeIdx).GetId(), xapp.Interval_ms_10, callbackGtp)
	time.Sleep(1 * time.Second)

	time.Sleep(10 * time.Second)

	// ----------------------- END ----------------------- //
	xapp.Rm_report_slice_sm(HndlrSlice)
	xapp.Rm_report_mac_sm(HndlrMac)
	xapp.Rm_report_gtp_sm(HndlrGtp)

	// --------------------------------------------------------- //

	// Stop the xApp. Avoid deadlock.
	for xapp.Try_stop() == false {
		time.Sleep(1 * time.Second)
	}

	fmt.Printf("Test xApp run SUCCESSFULLY\n")
}
