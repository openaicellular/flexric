#include <assert.h>

#include "fill_rnd_data_ccc.h"

e2sm_ccc_event_trigger_t fill_ccc_event_trigger_def(void)
{
  e2sm_ccc_event_trigger_t ev_trg = {0};

  ev_trg.type = FORMAT_1_EVENT_TRIGGER_CCC;

  ev_trg.frm_1.node_level_conf_list_len = 2;

  ev_trg.frm_1.node_level_ran_conf_struct_list = calloc(ev_trg.frm_1.node_level_conf_list_len, sizeof(node_level_ran_conf_struct_report_et_ad_t));
  assert(ev_trg.frm_1.node_level_ran_conf_struct_list != NULL);

  for (size_t i = 0; i<ev_trg.frm_1.node_level_conf_list_len; i++)
  {
    ev_trg.frm_1.node_level_ran_conf_struct_list[i].type = rand()%END_NODE_RAN_CONF_NAME_REPORT;

    ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len = 3;

    switch (ev_trg.frm_1.node_level_ran_conf_struct_list[i].type)
    {
    case O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT:
      ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du = calloc(ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_gnb_du_node_attribute_name_e));

      for (size_t j = 0; j<ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
      {
        ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du[j] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
      }
      break;

    case O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT:
      ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_cp = calloc(ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_gnb_cu_cp_node_attribute_name_e));

      for (size_t j = 0; j<ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
      {
        ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_cp[j] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
      }
      break;

    case O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME_REPORT:
      ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_up = calloc(ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_gnb_cu_up_node_attribute_name_e));

      for (size_t j = 0; j<ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
      {
        ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_up[j] = rand()%END_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION;
      }
      break;

    case O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_REPORT:
      ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_rrm_policy_ratio_node = calloc(ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_rrm_policy_ratio_attribute_name_e));

      for (size_t j = 0; j<ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
      {
        ev_trg.frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_rrm_policy_ratio_node[j] = rand()%END_RRM_POLICY_RATIO_ATTRIBUTE_NAME;
      }
      break;
    
    default:
      assert(false && "Unknown ");
    }

    

  }

  return ev_trg;
}
