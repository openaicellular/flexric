package mac

import (
	utils "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
	"fmt"
	"sync"
	//"fmt"
	xapp "build/examples/xApp/go/xapp_sdk"
)

//// Initialization function for the package
//func init() {
//
//	// Initialize the MultipleUeStatistics global variable
//	MultipleUeStatistics =  make([]E2NodeMultiUeStats, 0)
//
//			MultiUeStats{
//		Stats:    make(map[RNTI]UeStats), // Initialize an empty map for UeStats
//		NumOfUEs: 0,                      // Initialize the number of UEs to 0
//	}
//}

// Mutex for locking the global structure
var PrbMutex sync.Mutex
var ThptMutex sync.Mutex

type UeThroughputCalculationMetrics struct {
	DLAggtbsSt int
	ULAggtbsSt int
	Count      int
	DLThr      int
	ULThr      int
}

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
	UeThrMetrics   UeThroughputCalculationMetrics
}

type RNTI uint

type MultiUeStats struct {
	Stats    map[RNTI]UeStats // map of RNTI to UEStats
	NumOfUEs int
}

type E2NodeMultiUeStats struct {
	Mcc           uint16
	Mnc           uint16
	NbId          uint32
	RanType       string
	E2NodeUeStats MultiUeStats
}

// Global variable to store the stats of UEs
var MultipleUeStatistics []E2NodeMultiUeStats

// Global variable for the bandwidth and numerology TODO: Fix these hardcoded values
var (
	cell_BandWidth = 40000000
	scs            = 30000
)

// TotalPrbUtilization function to calculate the total prb utilisation
func TotalPrbUtilization(e2nodeId utils.E2NodeId) int {
	TotalPrbUtil := 0

	PrbMutex.Lock()

	entryFound := false
	entryIndex := -1
	for i, entry := range MacStats {
		if entry.Mcc == e2nodeId.Mcc && entry.Mnc == e2nodeId.Mnc && entry.NbId == e2nodeId.NbId && entry.RanType == e2nodeId.RanType {
			entryFound = true
			entryIndex = i
			break
		}

	}
	if entryFound == false {
		fmt.Println("ERROR: Entry not found in TotalPrbUtilization()")
		return -1
	}

	for _, ue := range MultipleUeStatistics[entryIndex].E2NodeUeStats.Stats {
		TotalPrbUtil += ue.PrbUtilisation
	}
	PrbMutex.Unlock()

	return TotalPrbUtil
}

func Calculate_UE_PRB_utilisation(ind xapp.Swig_mac_ind_msg_t) {

	PrbMutex.Lock()

	// get E2node details
	NbId := uint32(ind.GetId().GetNb_id().GetNb_id())
	// CuDuId := ind.GetId().GetCu_du_id() // TODO: not yet supported, maybe needs swig extension
	RanType := xapp.Get_e2ap_ngran_name(ind.GetId().GetXtype())
	Mcc := ind.GetId().GetPlmn().GetMcc()
	Mnc := ind.GetId().GetPlmn().GetMnc()

	// Find the entry in the global structure MultipleUeStatistics if it exists or create a new entry
	entryFound := false
	entryIndex := -1
	for i, entry := range MultipleUeStatistics {
		if entry.Mcc == Mcc && entry.Mnc == Mnc && entry.NbId == NbId && entry.RanType == RanType {
			entryFound = true
			entryIndex = i
			break
		}
	}

	if entryFound == false {
		// Create a new entry in the global structure MultipleUeStatistics
		newEntry := E2NodeMultiUeStats{
			Mcc:  Mcc,
			Mnc:  Mnc,
			NbId: NbId,
			//CuDuId:        CuDuId,
			RanType: RanType,
			E2NodeUeStats: MultiUeStats{
				Stats: make(map[RNTI]UeStats), // Initialize an empty map for UeStats
			},
		}
		MultipleUeStatistics = append(MultipleUeStatistics, newEntry)
		entryIndex = len(MultipleUeStatistics) - 1
	}

	// iterate over the number of UEs
	for i := 0; i < int(ind.GetUe_stats().Size()); i++ {

		Rnti := RNTI(ind.GetUe_stats().Get(i).GetRnti())
		// extract the rnti, teidgnb, teidupf, qfi
		Frame := ind.GetUe_stats().Get(i).GetFrame()
		Slot := ind.GetUe_stats().Get(i).GetSlot()
		Dl_aggr_prb := ind.GetUe_stats().Get(i).GetDl_aggr_prb()

		// Check if the UE RNTI exists in the map
		if ue, ok := MultipleUeStatistics[entryIndex].E2NodeUeStats.Stats[Rnti]; ok {

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

				//fmt.Println("Rnti: ", Rnti, ", Teidupf: ", ue.Teidupf, ", Teidgnb: ", ue.Teidgnb, ", Qfi: ", ue.Qfi, "Prb Util: ", Load, " %")

				ue.UePrbMetrics.DLRbCount = 0
				ue.UePrbMetrics.DiffDlSlotSt = 0
				ue.UePrbMetrics.DiffDlSlotEnd = 0

			}

			MultipleUeStatistics[entryIndex].E2NodeUeStats.Stats[Rnti] = ue
		} else {
			MultipleUeStatistics[entryIndex].E2NodeUeStats.Stats[Rnti] = UeStats{

				PrbUtilisation: 0,
				UePrbMetrics: UePrbCalculationMetrics{
					DLRbCount:     0,
					DiffDlSlotSt:  0,
					DiffDlSlotEnd: 0,
					DlFrameSt:     0,
					DlSlotSt:      0,
					DlAggrPrbSt:   0,
					NumberOfSlots: 0,
					DiffDlFr:      0,
				},
				UeThrMetrics: UeThroughputCalculationMetrics{
					DLAggtbsSt: 0,
					ULAggtbsSt: 0,
					Count:      0,
					DLThr:      0,
					ULThr:      0,
				},
			}
		}
	}

	PrbMutex.Unlock()
}

// TotalThroughput function to calculate the total prb utilisation // TODO: fix position of e2nodeId
func TotalThroughput(e2nodeId utils.E2NodeId) (int, int) {
	TotalDlThroughput, TotalUlThroughput := 0, 0

	ThptMutex.Lock()

	entryFound := false
	entryIndex := -1
	for i, entry := range MacStats {
		if entry.Mcc == e2nodeId.Mcc && entry.Mnc == e2nodeId.Mnc && entry.NbId == e2nodeId.NbId && entry.RanType == e2nodeId.RanType {
			entryFound = true
			entryIndex = i
			break
		}

	}
	if entryFound == false {
		fmt.Println("ERROR: Entry not found in TotalThroughput()")
		return -1, -1
	}

	for _, ue := range MultipleUeStatistics[entryIndex].E2NodeUeStats.Stats {
		TotalDlThroughput += ue.UeThrMetrics.DLThr
		TotalUlThroughput += ue.UeThrMetrics.ULThr
	}
	ThptMutex.Unlock()

	return TotalDlThroughput, TotalUlThroughput
}

func CalculateUeThroughput(ind xapp.Swig_mac_ind_msg_t) {

	ThptMutex.Lock()

	NbId := uint32(ind.GetId().GetNb_id().GetNb_id())
	// CuDuId := ind.GetId().GetCu_du_id() // TODO: not yet supported, maybe needs swig extension
	RanType := xapp.Get_e2ap_ngran_name(ind.GetId().GetXtype())
	Mcc := ind.GetId().GetPlmn().GetMcc()
	Mnc := ind.GetId().GetPlmn().GetMnc()

	// Find the entry in the global structure MultipleUeStatistics if it exists or create a new entry
	entryFound := false
	entryIndex := -1
	for i, entry := range MultipleUeStatistics {
		if entry.Mcc == Mcc && entry.Mnc == Mnc && entry.NbId == NbId && entry.RanType == RanType {
			entryFound = true
			entryIndex = i
			break
		}
	}

	if entryFound == false {
		// Create a new entry in the global structure MultipleUeStatistics
		newEntry := E2NodeMultiUeStats{
			Mcc:  Mcc,
			Mnc:  Mnc,
			NbId: NbId,
			//CuDuId:        CuDuId,
			RanType:       RanType,
			E2NodeUeStats: MultiUeStats{},
		}
		MultipleUeStatistics = append(MultipleUeStatistics, newEntry)
		entryIndex = len(MultipleUeStatistics) - 1
	}

	// iterate over the number of UEs
	for i := 0; i < int(ind.GetUe_stats().Size()); i++ {

		Rnti := RNTI(ind.GetUe_stats().Get(i).GetRnti())
		DLAggtbsSt := int(ind.GetUe_stats().Get(i).GetDl_aggr_tbs())
		ULAggtbsSt := int(ind.GetUe_stats().Get(i).GetUl_aggr_tbs())

		// Check if the UE RNTI exists in the map
		if ue, ok := MultipleUeStatistics[entryIndex].E2NodeUeStats.Stats[Rnti]; ok {

			// Store the first indication messages
			if ue.UeThrMetrics.Count == 0 {
				ue.UeThrMetrics.DLAggtbsSt = DLAggtbsSt
				ue.UeThrMetrics.ULAggtbsSt = ULAggtbsSt
			}

			// Wait for 1 second to get the last indication messages
			ue.UeThrMetrics.Count++

			if ue.UeThrMetrics.Count == 101 {

				ue.UeThrMetrics.DLThr = (int((int(ind.GetUe_stats().Get(i).GetDl_aggr_tbs()) - ue.UeThrMetrics.DLAggtbsSt) * 8)) / 1000000
				ue.UeThrMetrics.ULThr = (int((int(ind.GetUe_stats().Get(i).GetUl_aggr_tbs()) - ue.UeThrMetrics.ULAggtbsSt) * 8)) / 1000000
				ue.UeThrMetrics.Count = 0

			}

			MultipleUeStatistics[entryIndex].E2NodeUeStats.Stats[Rnti] = ue
		}
	}

	ThptMutex.Unlock()
}
