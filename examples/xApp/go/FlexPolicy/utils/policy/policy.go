package policy

import (
	slice "build/examples/xApp/go/FlexPolicy/utils/sm/slice"
)

// xApp policy configuration API
// Configuration represents the JSON configuration received by the first server.

// ------------------------------------------------------------------------ //
//
//	Configuration struct for storing the xApp policy configuration
//	coming from the Non-RT RIC via the APIs
//
// ------------------------------------------------------------------------ //
type Configuration struct {
	PolicyID   int16           `json:"PolicyId,omitempty"`
	PolicyType string          `json:"PolicyType,omitempty"`
	Scope      ScopeConfig     `json:"scope,omitempty"`
	Statement  StatementConfig `json:"statement,omitempty"`
}

type ScopeConfig struct {
	SliceID int16  `json:"sliceId,omitempty"`
	CellID  int16  `json:"cellId,omitempty"`
	Mcc     uint16 `json:"plmn,omitempty"`
	Mnc     uint16 `json:"mnc,omitempty"`
	RanType string `json:"ranType,omitempty"`
	NbId    uint32 `json:"nbId,omitempty"`
	CuDuId  uint32 `json:"cuDuId,omitempty"`
}

type StatementConfig struct {
	// -----  For ORAN xApps -----------------//
	MaxNumberOfUEs    int16 `json:"maxNumberOfUes,omitempty"`
	MacPrbUtilisation int16 `json:"macPrbUtilisation,omitempty"`
	MaxThroughput     int16 `json:"maxThroughput,omitempty"`

	// -----  For Custom xApps --------//
	// Type of the Control Request
	//     string: "ADDMOD", "DEL", "ASSOC_UE_SLICE"
	CtrlType string `json:"type,omitempty"`

	// Requested Structure
	CtrlRequest slice.Request `json:"request,omitempty"`
}

// ------------------------------------------------------------------------ //
//
//	PolicyEnforcementCallback function for enforcing the policy
//	The calback functions are defined in the callbacks folder
//
// ------------------------------------------------------------------------ //
type PolicyEnforcementCallback func(Configuration)
