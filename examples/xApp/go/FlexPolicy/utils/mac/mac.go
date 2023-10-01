package utils

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

	Mutex.Lock()
	MacStats.Indication = ind
	Mutex.Unlock()

}

// Definition of the global structure for storing the latest indication message
type MacStorage struct {
	Indication xapp.Swig_mac_ind_msg_t
}

// Definition of variable of the global structure for storing the latest indication message
var MacStats MacStorage 

// Mutex for locking the global structure SliceStatsDict
var Mutex sync.Mutex