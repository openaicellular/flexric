package prb

import (
	"sync"
	mac "build/examples/xApp/go/FlexPolicy/utils/mac"
	"fmt"
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


// Global variable for the bandwidth and numerology
var (
	cell_BandWidth = 40000000
	scs            = 30000
)


// TotalPrbUtilization function to calculate the total prb utilisation
func TotalPrbUtilization() int {
	var TotalPrbUtil int

	Mutex.Lock()
	for _, ue := range MultipleUeStatistics.Stats {
		TotalPrbUtil += ue.PrbUtilisation
	}
	Mutex.Unlock()

	return TotalPrbUtil
}


func Calculate_UE_PRB_utilisation() {

	mac.Mutex.Lock()
	ind := mac.MacStats.Indication
	mac.Mutex.Unlock()


	Mutex.Lock()

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

	Mutex.Unlock()

}