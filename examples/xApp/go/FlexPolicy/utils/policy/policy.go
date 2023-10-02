package policy

import (
	slice "build/examples/xApp/go/FlexPolicy/utils/slice"
)
// xApp policy configuration API
// Configuration represents the JSON configuration received by the first server.

// ------------------------------------------------------------------------ //
//	Configuration struct for storing the xApp policy configuration
//	coming from the Non-RT RIC via the APIs
// ------------------------------------------------------------------------ //
type Configuration struct {
	PolicyID   int             `json:"PolicyId,omitempty"`
	PolicyType string          `json:"PolicyType,omitempty"`
	Scope      ScopeConfig     `json:"scope,omitempty"`
	Statement  StatementConfig `json:"statement,omitempty"`
}

type ScopeConfig struct {
	SliceID int `json:"sliceId,omitempty"`
	CellID  int `json:"cellId,omitempty"`
}

type StatementConfig struct {
	// -----  For ORAN xApps -----------------//
	MaxNumberOfUEs int `json:"maxNumberOfUes,omitempty"`
	MacPrbUtilisation int `json:"macPrbUtilisation,omitempty"`
	MaxThroughput int `json:"maxThroughput,omitempty"` 

	// -----  For Br (Non-ORAN) xApps --------//
	// Type of the Control Request
	//     string: "ADDMOD", "DEL", "ASSOC_UE_SLICE"
	CtrlType string `json:"type,omitempty"`

	// Requested Structure
	CtrlRequest slice.Request `json:"request,omitempty"`
}

// ------------------------------------------------------------------------ //
//	PolicyEnforcementCallback function for enforcing the policy
//	The calback function is defined in the xApp
// ------------------------------------------------------------------------ //
type PolicyEnforcementCallback func(Configuration)
