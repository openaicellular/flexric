package mac

import "C"
import (
	xapp "build/examples/xApp/go/xapp_sdk"
	
	"sync"
)

// ------------------------------------------------------------------------ //
//	MAC INDICATION CALLBACK
// ------------------------------------------------------------------------ //
type MACCallback struct {
}

func (mac_cb MACCallback) Handle(ind xapp.Swig_mac_ind_msg_t) {
	Calculate_UE_PRB_utilisation(ind)
	CalculateUeThroughput(ind)
	fillMacStorage(ind)
}

// Definition of the global structure for storing the latest indication message
type MacStorage struct {
	UeStats  UeStatsType
	Id IdType // TODO: fill it
	Tstampt TstamptType // TODO: fill it
	GlobalPrbUtil int 
	GlobalDlThroughput int
}

type IdType struct {
	// TODO: fill it
}

type TstamptType struct {
	// TODO: fill it
}

type UeStatsType struct {
	Size int64
	Capacity int64
	IsEmpty bool
	MACStatsVector []MACStatsType
}

type MACStatsType struct {
	Dl_aggr_tbs uint64
	Ul_aggr_tbs uint64
	Dl_aggr_bytes_sdus uint64
	Ul_aggr_bytes_sdus uint64
	Dl_curr_tbs uint64
	Ul_curr_tbs uint64
	Dl_sched_rb uint64
	Ul_sched_rb uint64
	Pusch_snr float32
	Pucch_snr float32
	Dl_bler float32
	Ul_bler float32
	// Dl_harq
	// Ul_harq
	Dl_num_harq uint
	Ul_num_harq uint
	Rnti uint 
	Dl_aggr_prb uint 
	Ul_aggr_prb uint
	Dl_aggr_sdus uint
	Ul_aggr_sdus uint
	Dl_aggr_retx_prb uint
	Ul_aggr_retx_prb uint
	Bsr uint
	Frame uint16 
	Slot uint16 
	Wb_cqi byte 
	Dl_mcs1 byte
	Ul_mcs1 byte
	Dl_mcs2 byte
	Ul_mcs2 byte
	Phr int8 
}

// Definition of variable of the global structure for storing the latest indication message
var MacStats MacStorage 

// Mutex for locking the global structure SliceStatsDict
var Mutex sync.Mutex


func fillMacStorage(ind xapp.Swig_mac_ind_msg_t) {

	Mutex.Lock()
	MacStats.UeStats.Size = ind.GetUe_stats().Size()
	MacStats.UeStats.Capacity = ind.GetUe_stats().Capacity()
	MacStats.UeStats.IsEmpty = ind.GetUe_stats().IsEmpty()

	MacStats.UeStats.MACStatsVector = []MACStatsType{}

	for i := 0; i < int(MacStats.UeStats.Size); i++ {
		var macStats MACStatsType

		macStats.Dl_aggr_tbs = ind.GetUe_stats().Get(i).GetDl_aggr_tbs()
		macStats.Ul_aggr_tbs = ind.GetUe_stats().Get(i).GetUl_aggr_tbs()
		macStats.Dl_aggr_bytes_sdus = ind.GetUe_stats().Get(i).GetDl_aggr_bytes_sdus()
		macStats.Ul_aggr_bytes_sdus = ind.GetUe_stats().Get(i).GetUl_aggr_bytes_sdus()
		macStats.Dl_curr_tbs = ind.GetUe_stats().Get(i).GetDl_curr_tbs()
		macStats.Ul_curr_tbs = ind.GetUe_stats().Get(i).GetUl_curr_tbs()
		macStats.Dl_sched_rb = ind.GetUe_stats().Get(i).GetDl_sched_rb()
		macStats.Ul_sched_rb = ind.GetUe_stats().Get(i).GetUl_sched_rb()
		macStats.Pusch_snr = ind.GetUe_stats().Get(i).GetPusch_snr()
		macStats.Pucch_snr = ind.GetUe_stats().Get(i).GetPucch_snr()
		macStats.Dl_bler = ind.GetUe_stats().Get(i).GetDl_bler()
		macStats.Ul_bler = ind.GetUe_stats().Get(i).GetUl_bler()
		// macStats.Dl_harq = ind.GetUe_stats().Get(i).GetDl_harq()
		// macStats.Ul_harq = ind.GetUe_stats().Get(i).GetUl_harq()
		macStats.Dl_num_harq = ind.GetUe_stats().Get(i).GetDl_num_harq()
		macStats.Ul_num_harq = ind.GetUe_stats().Get(i).GetUl_num_harq()
		macStats.Rnti = ind.GetUe_stats().Get(i).GetRnti()
		macStats.Dl_aggr_prb = ind.GetUe_stats().Get(i).GetDl_aggr_prb()
		macStats.Ul_aggr_prb = ind.GetUe_stats().Get(i).GetUl_aggr_prb()
		macStats.Dl_aggr_sdus = ind.GetUe_stats().Get(i).GetDl_aggr_sdus()
		macStats.Ul_aggr_sdus = ind.GetUe_stats().Get(i).GetUl_aggr_sdus()
		macStats.Dl_aggr_retx_prb = ind.GetUe_stats().Get(i).GetDl_aggr_retx_prb()
		macStats.Ul_aggr_retx_prb = ind.GetUe_stats().Get(i).GetUl_aggr_retx_prb()
		macStats.Bsr = ind.GetUe_stats().Get(i).GetBsr()
		macStats.Frame = ind.GetUe_stats().Get(i).GetFrame()
		macStats.Slot = ind.GetUe_stats().Get(i).GetSlot()
		macStats.Wb_cqi = ind.GetUe_stats().Get(i).GetWb_cqi()
		macStats.Dl_mcs1 = ind.GetUe_stats().Get(i).GetDl_mcs1()
		macStats.Ul_mcs1 = ind.GetUe_stats().Get(i).GetUl_mcs1()
		macStats.Dl_mcs2 = ind.GetUe_stats().Get(i).GetDl_mcs2()
		macStats.Ul_mcs2 = ind.GetUe_stats().Get(i).GetUl_mcs2()
		macStats.Phr = ind.GetUe_stats().Get(i).GetPhr()

		// add it to the global structure
		MacStats.UeStats.MACStatsVector = append(MacStats.UeStats.MACStatsVector, macStats)
	}

	
	MacStats.GlobalPrbUtil = TotalPrbUtilization()
	MacStats.GlobalDlThroughput, _ = TotalThroughput()
	// TODO: add more

	Mutex.Unlock()

}

func DeepCopyMacStorage(original MacStorage) MacStorage {
	Mutex.Lock()
	copied := MacStorage{
		UeStats:           DeepCopyUeStats(original.UeStats),
		Id:                original.Id, // Assuming IdType has no pointers or slices
		Tstampt:           original.Tstampt, // Assuming TstamptType has no pointers or slices
		GlobalPrbUtil:     original.GlobalPrbUtil,
		GlobalDlThroughput:  original.GlobalDlThroughput,
	}
	Mutex.Unlock()
	return copied
}

func DeepCopyUeStats(original UeStatsType) UeStatsType {
	copied := UeStatsType{
		Size:           original.Size,
		Capacity:       original.Capacity,
		IsEmpty:        original.IsEmpty,
		MACStatsVector: make([]MACStatsType, len(original.MACStatsVector)),
	}
	copy(copied.MACStatsVector, original.MACStatsVector)
	return copied
}