package mac

import "C"
import (
	utils "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
	xapp "build/examples/xApp/go/xapp_sdk"

	"sync"
)

// MACCallback MAC INDICATION CALLBACK
type MACCallback struct {
}

func (mac_cb MACCallback) Handle(ind xapp.Swig_mac_ind_msg_t) {
	Calculate_UE_PRB_utilisation(ind)
	CalculateUeThroughput(ind)
	FillMacStorage(ind)
}

type E2NodeMacStorageMap struct {
	Mcc            uint16     `json:"mcc,omitempty"`
	Mnc            uint16     `json:"mnc,omitempty"`
	NbId           uint32     `json:"nb_id,omitempty"`
	CuDuId         uint32     `json:"cu_du_id,omitempty"`
	RanType        string     `json:"ran_type,omitempty"`
	E2NodeMacStats MacStorage `json:"slice_stats,omitempty"`
}

// MacStorage Definition of the global structure for storing the latest indication message
type MacStorage struct {
	UeStats            UeStatsType
	Id                 IdType      // TODO: fill it
	Tstampt            TstamptType // TODO: fill it
	GlobalPrbUtil      int
	GlobalDlThroughput int
}

type IdType struct {
	// TODO: fill it
}

type TstamptType struct {
	// TODO: fill it
}

type UeStatsType struct {
	Size           int64
	Capacity       int64
	IsEmpty        bool
	MACStatsVector []MACStatsType
}

type MACStatsType struct {
	Dl_aggr_tbs        uint64
	Ul_aggr_tbs        uint64
	Dl_aggr_bytes_sdus uint64
	Ul_aggr_bytes_sdus uint64
	Dl_curr_tbs        uint64
	Ul_curr_tbs        uint64
	Dl_sched_rb        uint64
	Ul_sched_rb        uint64
	Pusch_snr          float32
	Pucch_snr          float32
	Dl_bler            float32
	Ul_bler            float32
	// Dl_harq
	// Ul_harq
	Dl_num_harq      uint
	Ul_num_harq      uint
	Rnti             uint
	Dl_aggr_prb      uint
	Ul_aggr_prb      uint
	Dl_aggr_sdus     uint
	Ul_aggr_sdus     uint
	Dl_aggr_retx_prb uint
	Ul_aggr_retx_prb uint
	Bsr              uint
	Frame            uint16
	Slot             uint16
	Wb_cqi           byte
	Dl_mcs1          byte
	Ul_mcs1          byte
	Dl_mcs2          byte
	Ul_mcs2          byte
	Phr              int8
}

// MacStats Definition of variable of the global structure for storing the latest indication message
var MacStats []E2NodeMacStorageMap

// Mutex for locking the global structure SliceStatsDict
var Mutex sync.Mutex

func FillMacStorage(ind xapp.Swig_mac_ind_msg_t) {

	Mutex.Lock()

	// get E2node details
	NbId := uint32(ind.GetId().GetNb_id().GetNb_id())
	// CuDuId := ind.GetId().GetCu_du_id() // TODO: not yet supported, maybe needs swig extension
	RanType := xapp.Get_e2ap_ngran_name(ind.GetId().GetXtype())
	Mcc := ind.GetId().GetPlmn().GetMcc()
	Mnc := ind.GetId().GetPlmn().GetMnc()

	// Find the entry in the global structure SliceStats if it exists or create a new entry
	entryFound := false
	entryIndex := -1
	for i, entry := range MacStats {
		if entry.Mcc == Mcc && entry.Mnc == Mnc && entry.NbId == NbId && entry.RanType == RanType {
			entryFound = true
			entryIndex = i
			break
		}
	}

	if entryFound == false {
		// Create a new entry in the global structure SliceStats
		newEntry := E2NodeMacStorageMap{
			Mcc:  Mcc,
			Mnc:  Mnc,
			NbId: NbId,
			//CuDuId:           CuDuId,
			RanType:        RanType,
			E2NodeMacStats: MacStorage{},
		}
		MacStats = append(MacStats, newEntry)
		entryIndex = len(MacStats) - 1

	}

	MacStats[entryIndex].E2NodeMacStats.UeStats.Size = ind.GetUe_stats().Size()
	MacStats[entryIndex].E2NodeMacStats.UeStats.Capacity = ind.GetUe_stats().Capacity()
	MacStats[entryIndex].E2NodeMacStats.UeStats.IsEmpty = ind.GetUe_stats().IsEmpty()

	MacStats[entryIndex].E2NodeMacStats.UeStats.MACStatsVector = []MACStatsType{}

	for i := 0; i < int(MacStats[entryIndex].E2NodeMacStats.UeStats.Size); i++ {
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
		MacStats[entryIndex].E2NodeMacStats.UeStats.MACStatsVector = append(MacStats[entryIndex].E2NodeMacStats.UeStats.MACStatsVector, macStats)
	}

	e2nodeId := utils.E2NodeId{
		Mcc:     Mcc,
		Mnc:     Mnc,
		NbId:    NbId,
		CuDuId:  0,
		RanType: RanType,
	}
	MacStats[entryIndex].E2NodeMacStats.GlobalPrbUtil = TotalPrbUtilization(e2nodeId)
	MacStats[entryIndex].E2NodeMacStats.GlobalDlThroughput, _ = TotalThroughput(e2nodeId)
	// TODO: add more

	Mutex.Unlock()

}
func DeepCopyMacStats(original []E2NodeMacStorageMap) []E2NodeMacStorageMap {
	Mutex.Lock()
	defer Mutex.Unlock()

	copied := make([]E2NodeMacStorageMap, len(original))
	for i, storageMap := range original {
		copied[i] = E2NodeMacStorageMap{
			Mcc:            storageMap.Mcc,
			Mnc:            storageMap.Mnc,
			NbId:           storageMap.NbId,
			CuDuId:         storageMap.CuDuId,
			RanType:        storageMap.RanType,
			E2NodeMacStats: DeepCopyMacStorage(storageMap.E2NodeMacStats),
		}
	}
	return copied
}

func DeepCopyMacStorage(original MacStorage) MacStorage {

	copied := MacStorage{
		UeStats:            DeepCopyUeStats(original.UeStats),
		Id:                 original.Id,      // Assuming IdType has no pointers or slices
		Tstampt:            original.Tstampt, // Assuming TstamptType has no pointers or slices
		GlobalPrbUtil:      original.GlobalPrbUtil,
		GlobalDlThroughput: original.GlobalDlThroughput,
	}

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
