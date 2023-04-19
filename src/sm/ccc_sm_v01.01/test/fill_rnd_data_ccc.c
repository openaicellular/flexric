#include <assert.h>

#include "fill_rnd_data_ccc.h"

static e2sm_ccc_ev_trg_frm_1_t fill_ccc_event_trigger_def_frm_1(void)
{
  e2sm_ccc_ev_trg_frm_1_t ev_trg_frm_1 = {0};

  ev_trg_frm_1.node_level_conf_list_len = 2;

  ev_trg_frm_1.node_level_ran_conf_struct_list = calloc(ev_trg_frm_1.node_level_conf_list_len, sizeof(node_level_ran_conf_struct_report_et_ad_t));
  assert(ev_trg_frm_1.node_level_ran_conf_struct_list != NULL);

  for (size_t i = 0; i<ev_trg_frm_1.node_level_conf_list_len; i++)
  {
    ev_trg_frm_1.node_level_ran_conf_struct_list[i].type = rand()%END_NODE_RAN_CONF_NAME_REPORT;

    ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len = 3;

    switch (ev_trg_frm_1.node_level_ran_conf_struct_list[i].type)
    {
    case O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT:
      ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du = calloc(ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_gnb_du_node_attribute_name_e));

      for (size_t j = 0; j<ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
      {
        ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du[j] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
      }
      break;

    case O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT:
      ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_cp = calloc(ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_gnb_cu_cp_node_attribute_name_e));

      for (size_t j = 0; j<ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
      {
        ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_cp[j] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
      }
      break;

    case O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME_REPORT:
      ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_up = calloc(ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_gnb_cu_up_node_attribute_name_e));

      for (size_t j = 0; j<ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
      {
        ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_up[j] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION;
      }
      break;

    case O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_REPORT:
      ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_rrm_policy_ratio_node = calloc(ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_rrm_policy_ratio_attribute_name_e));

      for (size_t j = 0; j<ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
      {
        ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_rrm_policy_ratio_node[j] = rand()%END_RRM_POLICY_RATIO_ATTRIBUTE_NAME;
      }
      break;
    
    default:
      assert(false && "Unknown Node Level RAN Configuration Name");
    }

  }

  return ev_trg_frm_1;
}

static e2sm_ccc_ev_trg_frm_2_t fill_ccc_event_trigger_def_frm_2(void)
{
  e2sm_ccc_ev_trg_frm_2_t ev_trg_frm_2 = {0};

  ev_trg_frm_2.cell_level_conf_list_len = 2;

  ev_trg_frm_2.cell_level_conf_list = calloc(ev_trg_frm_2.cell_level_conf_list_len, sizeof(cell_level_conf_list_ev_trg_frm_2_t));
  assert(ev_trg_frm_2.cell_level_conf_list != NULL);

  for (size_t i = 0; i<ev_trg_frm_2.cell_level_conf_list_len; i++)
  {
    ev_trg_frm_2.cell_level_conf_list[i].cell_global_id = calloc(1, sizeof(*ev_trg_frm_2.cell_level_conf_list[i].cell_global_id));
    ev_trg_frm_2.cell_level_conf_list[i].cell_global_id->type = rand()%END_CGI_RAT_TYPE;
  
    switch (ev_trg_frm_2.cell_level_conf_list[i].cell_global_id->type)
    {
    case NR_CGI_RAT_TYPE:
      ev_trg_frm_2.cell_level_conf_list[i].cell_global_id->nr_cgi.plmn_id = (plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
      ev_trg_frm_2.cell_level_conf_list[i].cell_global_id->nr_cgi.nr_cell_id = (rand() % 2^36) + 1;
      break;

    case EUTRA_CGI_RAT_TYPE:
      ev_trg_frm_2.cell_level_conf_list[i].cell_global_id->eutra.plmn_id = (plmn_t) {.mcc = 505, .mnc = 1, .mnc_digit_len = 2};
      ev_trg_frm_2.cell_level_conf_list[i].cell_global_id->eutra.eutra_cell_id = (rand() % 2^28) + 0;
      break;
  
    default:
      assert(false && "Unknown Cell Global ID Type");
    }


    ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len = 3;

    ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list = calloc(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len, sizeof(cell_level_ran_conf_struct_report_et_ad_t));
    assert(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list != NULL);

    for (size_t j = 0; j<ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len; j++)
    {
      ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].type = rand()%END_CELL_RAN_CONF_NAME_REPORT;

      ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len = 3;

      switch (ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].type)
      {
      case O_NR_Cell_CU_RAN_CONF_NAME_REPORT:
        ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_nr_cell_cu_cell = calloc(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len, sizeof(o_nr_cell_cu_attribute_name_e));
        assert(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_nr_cell_cu_cell != NULL);

        for (size_t z = 0; z<ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len; z++)
        {
          ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_nr_cell_cu_cell[z] = rand()%END_ATTRIBUTE_NAME_REPORT_NR_CELL_CU;
        }
        break;

      case O_NR_Cell_DU_RAN_CONF_NAME_REPORT:
        ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_nr_cell_du_cell = calloc(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len, sizeof(o_nr_cell_du_attribute_name_e));
        assert(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_nr_cell_du_cell != NULL);

        for (size_t z = 0; z<ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len; z++)
        {
          ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_nr_cell_du_cell[z] = rand()%END_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
        }
        break;

      case O_BWP_RAN_CONF_NAME_REPORT:
        ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_bwp_cell = calloc(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len, sizeof(o_bwp_attribute_name_e));
        assert(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_bwp_cell != NULL);

        for (size_t z = 0; z<ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len; z++)
        {
          ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_bwp_cell[z] = rand()%END_ATTRIBUTE_NAME_BWP_CELL;
        }
        break;

      case O_RRM_Policy_Ratio_CELL_RAN_CONF_NAME_REPORT:
        ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_rrm_policy_ratio_cell = calloc(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len, sizeof(o_rrm_policy_ratio_attribute_name_e));
        assert(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_rrm_policy_ratio_cell != NULL);

        for (size_t z = 0; z<ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len; z++)
        {
          ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_rrm_policy_ratio_cell[z] = rand()%END_RRM_POLICY_RATIO_ATTRIBUTE_NAME;
        }
        break;
      
      default:
        break;
      }
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
