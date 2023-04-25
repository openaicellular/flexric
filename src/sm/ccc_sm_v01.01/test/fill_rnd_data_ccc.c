#include <assert.h>

#include "fill_rnd_data_ccc.h"

static o_gnb_du_node_attribute_name_e * fill_ccc_gnb_du_attribute_name_list(size_t len)
{
  o_gnb_du_node_attribute_name_e * gnb_du = calloc(len, sizeof(o_gnb_du_node_attribute_name_e));
  assert(gnb_du != NULL);

  for (size_t i = 0; i<len; i++)
  {
    gnb_du[i] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
  }

  return gnb_du;
}

static o_gnb_cu_cp_node_attribute_name_e * fill_ccc_gnb_cu_cp_attribute_name_list(size_t len)
{
  o_gnb_cu_cp_node_attribute_name_e * gnb_cu_cp = calloc(len, sizeof(o_gnb_cu_cp_node_attribute_name_e));
  assert(gnb_cu_cp != NULL);

  for (size_t i = 0; i<len; i++)
  {
    gnb_cu_cp[i] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
  }

  return gnb_cu_cp;
}

static o_gnb_cu_up_node_attribute_name_e * fill_ccc_gnb_cu_up_attribute_name_list(size_t len)
{
  o_gnb_cu_up_node_attribute_name_e * gnb_cu_up = calloc(len, sizeof(o_gnb_cu_up_node_attribute_name_e));
  assert(gnb_cu_up != NULL);

  for (size_t i = 0; i<len; i++)
  {
    gnb_cu_up[i] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION;
  }

  return gnb_cu_up;
}

static o_rrm_policy_ratio_attribute_name_e * fill_ccc_rrm_policy_ratio_attribute_name_list(size_t len)
{
  o_rrm_policy_ratio_attribute_name_e * rrm_policy_ratio = calloc(len, sizeof(o_rrm_policy_ratio_attribute_name_e));
  assert(rrm_policy_ratio != NULL);

  for (size_t i = 0; i<len; i++)
  {
    rrm_policy_ratio[i] = rand()%END_RRM_POLICY_RATIO_ATTRIBUTE_NAME;
  }

  return rrm_policy_ratio;
}

static node_level_ran_conf_struct_report_et_ad_t fill_ccc_node_level_ran_conf_struct_report_et_ad(void)
{
  node_level_ran_conf_struct_report_et_ad_t node_struct = {0};
  
  // Mandatory
  // RAN Configuration Structure Name
  // 9.3.7 => 8.2.1
  node_struct.type = rand()%END_NODE_RAN_CONF_NAME_REPORT;

  // Optional
  // List of Attributes  -  [0..65535]
  // 8.8.1
  node_struct.attribute_name_list_len = 5;

  switch (node_struct.type)
  {
  case O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT:
    node_struct.attribute_name_list.o_gnb_du = fill_ccc_gnb_du_attribute_name_list(node_struct.attribute_name_list_len);
    break;

  case O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT:
    node_struct.attribute_name_list.o_gnb_cu_cp = fill_ccc_gnb_cu_cp_attribute_name_list(node_struct.attribute_name_list_len);
    break;

  case O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME_REPORT:
    node_struct.attribute_name_list.o_gnb_cu_up = fill_ccc_gnb_cu_up_attribute_name_list(node_struct.attribute_name_list_len);
    break;

  case O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_REPORT:
    node_struct.attribute_name_list.o_rrm_policy_ratio_node = fill_ccc_rrm_policy_ratio_attribute_name_list(node_struct.attribute_name_list_len);
    break;
    
  default:
    assert(false && "Unknown Node Level RAN Configuration Name");
  }


  return node_struct;
}

static e2sm_ccc_ev_trg_frm_1_t fill_ccc_event_trigger_def_frm_1(void)
{
  e2sm_ccc_ev_trg_frm_1_t ev_trg_frm_1 = {0};

  // Mandatory
  // List of Node-level Configuration Structures  -  [1..256]
  ev_trg_frm_1.node_level_conf_list_len = 10;

  ev_trg_frm_1.node_level_ran_conf_struct_list = calloc(ev_trg_frm_1.node_level_conf_list_len, sizeof(node_level_ran_conf_struct_report_et_ad_t));
  assert(ev_trg_frm_1.node_level_ran_conf_struct_list != NULL);

  for (size_t i = 0; i<ev_trg_frm_1.node_level_conf_list_len; i++)
  {
    ev_trg_frm_1.node_level_ran_conf_struct_list[i] = fill_ccc_node_level_ran_conf_struct_report_et_ad();
  }

  return ev_trg_frm_1;
}

static cell_global_id_t fill_ccc_cell_global_id(void)
{
  cell_global_id_t cell_global_id = {0};

  cell_global_id.type = rand()%END_CGI_RAT_TYPE;
  
  switch (cell_global_id.type)
  {
  case NR_CGI_RAT_TYPE:
    cell_global_id.nr_cgi.plmn_id = (plmn_t) {.mcc = 505, .mnc = 15, .mnc_digit_len = 2};
    cell_global_id.nr_cgi.nr_cell_id = (rand() % 2^36) + 1;
    break;

  case EUTRA_CGI_RAT_TYPE:
    cell_global_id.eutra.plmn_id = (plmn_t) {.mcc = 505, .mnc = 15, .mnc_digit_len = 2};
    cell_global_id.eutra.eutra_cell_id = (rand() % 2^28) + 0;
    break;
  
  default:
    assert(false && "Unknown Cell Global ID Type");
  }

  return cell_global_id;
}

static o_nr_cell_cu_attribute_name_e * fill_ccc_nr_cell_cu_attribute_name_list(size_t len)
{
  o_nr_cell_cu_attribute_name_e * nr_cell_cu = calloc(len, sizeof(o_nr_cell_cu_attribute_name_e));
  assert(nr_cell_cu != NULL);

  for (size_t i = 0; i<len; i++)
  {
    nr_cell_cu[i] = rand()%END_ATTRIBUTE_NAME_REPORT_NR_CELL_CU;
  }

  return nr_cell_cu;
}

static o_nr_cell_du_attribute_name_e * fill_ccc_nr_cell_du_attribute_name_list(size_t len)
{
  o_nr_cell_du_attribute_name_e * nr_cell_du = calloc(len, sizeof(o_nr_cell_du_attribute_name_e));
  assert(nr_cell_du != NULL);

  for (size_t i = 0; i<len; i++)
  {
    nr_cell_du[i] = rand()%END_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
  }

  return nr_cell_du;
}

static o_bwp_attribute_name_e * fill_ccc_bwp_attribute_name_list(size_t len)
{
  o_bwp_attribute_name_e * bwp = calloc(len, sizeof(o_bwp_attribute_name_e));
  assert(bwp != NULL);

  for (size_t i = 0; i<len; i++)
  {
    bwp[i] = rand()%END_ATTRIBUTE_NAME_BWP_CELL;
  }

  return bwp;
}


static cell_level_ran_conf_struct_report_et_ad_t fill_ccc_cell_level_ran_conf_struct_report_et_ad(void)
{
  cell_level_ran_conf_struct_report_et_ad_t cells_list = {0};  

  // Mandatory
  // RAN Configuration Structure Name
  // 9.3.7 => 8.2.2
  cells_list.type = rand()%END_CELL_RAN_CONF_NAME_REPORT;

  // Optional
  // List of Attributes  -  [0..65535]
  // 8.8.2
  cells_list.attribute_name_list_len = 5;

  switch (cells_list.type)
  {
  case O_NR_Cell_CU_RAN_CONF_NAME_REPORT:
    cells_list.attribute_name_list.o_nr_cell_cu_cell = fill_ccc_nr_cell_cu_attribute_name_list(cells_list.attribute_name_list_len);
    break;

  case O_NR_Cell_DU_RAN_CONF_NAME_REPORT:
    cells_list.attribute_name_list.o_nr_cell_du_cell = fill_ccc_nr_cell_du_attribute_name_list(cells_list.attribute_name_list_len);
    break;

  case O_BWP_RAN_CONF_NAME_REPORT:
    cells_list.attribute_name_list.o_bwp_cell = fill_ccc_bwp_attribute_name_list(cells_list.attribute_name_list_len);
    break;

  case O_RRM_Policy_Ratio_CELL_RAN_CONF_NAME_REPORT:
    cells_list.attribute_name_list.o_rrm_policy_ratio_cell = fill_ccc_rrm_policy_ratio_attribute_name_list(cells_list.attribute_name_list_len);
    break;
      
  default:
    assert("Unknown Cell RAN Configuration Structure Name");
  }


  return cells_list;
}


static e2sm_ccc_ev_trg_frm_2_t fill_ccc_event_trigger_def_frm_2(void)
{
  e2sm_ccc_ev_trg_frm_2_t ev_trg_frm_2 = {0};

  // Mandatory
  // List of Cell-level Configuration Structures  -  [1..1024]
  ev_trg_frm_2.cell_level_conf_list_len = 2;

  ev_trg_frm_2.cell_level_conf_list = calloc(ev_trg_frm_2.cell_level_conf_list_len, sizeof(cell_level_conf_list_ev_trg_frm_2_t));
  assert(ev_trg_frm_2.cell_level_conf_list != NULL);

  for (size_t i = 0; i<ev_trg_frm_2.cell_level_conf_list_len; i++)
  {
    // Optional
    // Cell Global ID
    // 9.3.6
    ev_trg_frm_2.cell_level_conf_list[i].cell_global_id = calloc(1, sizeof(*ev_trg_frm_2.cell_level_conf_list[i].cell_global_id));
    assert(ev_trg_frm_2.cell_level_conf_list[i].cell_global_id != NULL);
    *ev_trg_frm_2.cell_level_conf_list[i].cell_global_id = fill_ccc_cell_global_id();

    // Mandatory
    // List of RAN Configuration Structures  -  [1..1024]
    ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len = 10;

    ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list = calloc(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len, sizeof(cell_level_ran_conf_struct_report_et_ad_t));
    assert(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list != NULL);

    for (size_t j = 0; j<ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len; j++)
    {
      ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j] = fill_ccc_cell_level_ran_conf_struct_report_et_ad();
    }
    
  }


  return ev_trg_frm_2;
}

static e2sm_ccc_ev_trg_frm_3_t fill_ccc_event_trigger_def_frm_3(void)
{
  e2sm_ccc_ev_trg_frm_3_t ev_trg_frm_3 = {0};

  ev_trg_frm_3.report_period = (rand() % 4294967295) + 10;


  return ev_trg_frm_3;
}

/*  CCC RIC EVENT TRIGGER DEFINITION FILL DATA  */

e2sm_ccc_event_trigger_t fill_ccc_event_trigger_def(void)
{
  e2sm_ccc_event_trigger_t ev_trg = {0};

  ev_trg.type = rand()%END_EVENT_TRIGGER_CCC;

  switch (ev_trg.type)
  {
  case FORMAT_1_EVENT_TRIGGER_CCC:
    ev_trg.frm_1 = fill_ccc_event_trigger_def_frm_1();
    break;

  case FORMAT_2_EVENT_TRIGGER_CCC:
    ev_trg.frm_2 = fill_ccc_event_trigger_def_frm_2();
    break;

  case FORMAT_3_EVENT_TRIGGER_CCC:
    ev_trg.frm_3 = fill_ccc_event_trigger_def_frm_3();
    break;
  
  default:
    assert(false && "Unknown CCC RIC Event Trigger Definition Format");
  }

  return ev_trg;
}



static e2sm_ccc_action_def_frm_1_t fill_ccc_action_def_frm_1(void)
{
  e2sm_ccc_action_def_frm_1_t act_def_frm_1 = {0};

  // Mandatory
  // List of Node-level RAN Configuration Structures  -  [1..256]
  act_def_frm_1.node_level_conf_list_len = 3;

  act_def_frm_1.node_level_conf_list_frm_1 = calloc(act_def_frm_1.node_level_conf_list_len, sizeof(node_level_conf_list_act_def_frm_1_t));
  assert(act_def_frm_1.node_level_conf_list_frm_1 != NULL);

  for (size_t i = 0; i<act_def_frm_1.node_level_conf_list_len; i++)
  {
    // Mandatory
    // Report Type
    // 9.3.9
    act_def_frm_1.node_level_conf_list_frm_1[i].report_type = rand()%END_REPORT_TYPE_CCC;

    // Mandatory
    // Node-Level RAN Configuration Structures
    // 8.2.1
    act_def_frm_1.node_level_conf_list_frm_1[i].node_level_ran_conf_struct = fill_ccc_node_level_ran_conf_struct_report_et_ad();

  }

  return act_def_frm_1;
}

static e2sm_ccc_action_def_frm_2_t fill_ccc_action_def_frm_2(void)
{
  e2sm_ccc_action_def_frm_2_t act_def_frm_2 = {0};

  // Mandatory
  // List of Cell Configurations To Be Reported  -  [1..1024]
  act_def_frm_2.cell_level_conf_list_len = 5;

  act_def_frm_2.cell_level_conf_list_frm_2 = calloc(act_def_frm_2.cell_level_conf_list_len, sizeof(cell_configurations_list_t));
  assert(act_def_frm_2.cell_level_conf_list_frm_2 != NULL);

  for (size_t i = 0; i<act_def_frm_2.cell_level_conf_list_len; i++)
  {
    // Optional
    // Cell Global ID
    // 9.3.6
    act_def_frm_2.cell_level_conf_list_frm_2[i].cell_global_id = calloc(1, sizeof(*act_def_frm_2.cell_level_conf_list_frm_2[i].cell_global_id));
    assert(act_def_frm_2.cell_level_conf_list_frm_2[i].cell_global_id != NULL);
    *act_def_frm_2.cell_level_conf_list_frm_2[i].cell_global_id = fill_ccc_cell_global_id();


    // Mandatory
    // List of Cell-level RAN Configuration Structures  -  [1..1024]
    act_def_frm_2.cell_level_conf_list_frm_2[i].node_level_conf_list_len = 4;

    act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2 = calloc(act_def_frm_2.cell_level_conf_list_frm_2[i].node_level_conf_list_len, sizeof(cell_level_conf_list_act_def_frm_2_t));
    assert(act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2 != NULL);

    for (size_t j = 0; j<act_def_frm_2.cell_level_conf_list_frm_2[i].node_level_conf_list_len; j++)
    {
      // Mandatory
      // Report Type
      // 9.3.9
      act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2[j].report_type = rand()%END_REPORT_TYPE_CCC;

      // Mandatory
      // Cell-Level RAN Configuration Structures
      // 8.2.2
      act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2[j].cell_level_ran_conf_struct = fill_ccc_cell_level_ran_conf_struct_report_et_ad();

    }
  }

  return act_def_frm_2;
}

/*  CCC RIC ACTION DEFINITION FILL DATA  */

e2sm_ccc_action_def_t fill_ccc_action_def(void)
{
  e2sm_ccc_action_def_t act_def = {0};

  act_def.report_style_type = rand()%END_RIC_SERVICE_REPORT_CCC;

  switch (act_def.report_style_type)
  {
  case STYLE_1_RIC_SERVICE_REPORT_CCC:
    act_def.type = FORMAT_1_ACTION_DEFINITION_CCC;
    act_def.frm_1 = fill_ccc_action_def_frm_1();
    break;

  case STYLE_2_RIC_SERVICE_REPORT_CCC:
    act_def.type = FORMAT_2_ACTION_DEFINITION_CCC;
    act_def.frm_2 = fill_ccc_action_def_frm_2();
    break;
  
  default:
    assert("Unknown CCC RIC Action Definition Format Type");
  }


  return act_def;
}
