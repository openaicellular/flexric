/*
 * ws protocol handler plugin for "lws-minimal"
 *
 * Written in 2010-2019 by Andy Green <andy@warmcat.com>
 *
 * This file is made available under the Creative Commons CC0 1.0
 * Universal Public Domain Dedication.
 *
 * This version holds a single message at a time, which may be lost if a new
 * message comes.  See the minimal-ws-server-ring sample for the same thing
 * but using an lws_ring ringbuffer to hold up to 8 messages at a time.
 */

#include <libwebsockets.h>
#include <string.h>
#include "json-c/json.h"

#include "ser_ran.h"
#include "ran_msg_hdlr.h"


/* one of these created for each message */

struct msg {
	void *payload; /* is malloc'd */
	size_t len;
};

/* one of these is created for each client connecting to us */

struct per_session_data__minimal {
	struct per_session_data__minimal *pss_list;
	struct lws *wsi;
	int last; /* the last message number we sent */
};

/* one of these is created for each vhost our protocol is used with */

struct per_vhost_data__minimal {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;

	struct per_session_data__minimal *pss_list; /* linked-list of live pss*/

	struct msg amsg; /* the one pending message... */
	int current; /* the current message number we are caching */
};

/* destroys the message when everyone has had a copy of it */

static void
__minimal_destroy_message(void *_msg)
{
	struct msg *msg = _msg;

	free(msg->payload);
	msg->payload = NULL;
	msg->len = 0;
}

static int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
			void *user, void *in, size_t len)
{
	struct per_session_data__minimal *pss =
			(struct per_session_data__minimal *)user;

	struct per_vhost_data__minimal *vhd =
			(struct per_vhost_data__minimal *)
			lws_protocol_vh_priv_get(lws_get_vhost(wsi),
					lws_get_protocol(wsi));
	int m;

  lwsl_err("Getting data from the RAN endpoint: %d\n", reason);
	switch (reason) {
	case LWS_CALLBACK_PROTOCOL_INIT:
		vhd = lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
				lws_get_protocol(wsi),
				sizeof(struct per_vhost_data__minimal));
		vhd->context = lws_get_context(wsi);
		vhd->protocol = lws_get_protocol(wsi);
		vhd->vhost = lws_get_vhost(wsi);
		break;

	case LWS_CALLBACK_ESTABLISHED:
		/* add ourselves to the list of live pss held in the vhd */
		lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
		pss->wsi = wsi;
		pss->last = vhd->current;

    const char* ready_msg = ran_ser_get_instance()->encode_ready();

    vhd->amsg.len = strlen(ready_msg);
    /* notice we over-allocate by LWS_PRE */
    vhd->amsg.payload = malloc(LWS_PRE + vhd->amsg.len);
    if (!vhd->amsg.payload) {
      lwsl_user("OOM: dropping\n");
      break;
    }

    memcpy((char *)vhd->amsg.payload + LWS_PRE, ready_msg, vhd->amsg.len);
    vhd->current++;

    /*
     * let everybody know we want to write something on them
     * as soon as they are ready
     */
    lws_start_foreach_llp(struct per_session_data__minimal **,
    ppss, vhd->pss_list) {
      lws_callback_on_writable((*ppss)->wsi);
    } lws_end_foreach_llp(ppss, pss_list);
		break;

	case LWS_CALLBACK_CLOSED:
		/* remove our closing pss from the list of live pss */
		lws_ll_fwd_remove(struct per_session_data__minimal, pss_list,
				  pss, vhd->pss_list);
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		if (!vhd->amsg.payload)
			break;

		if (pss->last == vhd->current)
			break;

		/* notice we allowed for LWS_PRE in the payload already */
		m = lws_write(wsi, ((unsigned char *)vhd->amsg.payload) +
			      LWS_PRE, vhd->amsg.len, LWS_WRITE_TEXT);
		if (m < (int)vhd->amsg.len) {
      printf("ERROR %d writing to ws\n", m);
			return -1;
		}

		pss->last = vhd->current;
		break;

	case LWS_CALLBACK_RECEIVE:
    lwsl_info("Getting data from the RAN endpoint: %s\n", in ? (char *)in : "(null)");
    if (vhd->amsg.payload)
      __minimal_destroy_message(&vhd->amsg);

    size_t size;
    const char* tx_msg = "";
    struct json_tokener *tok = json_tokener_new();
    if (!tok)
      return 0;
    struct json_object *json_main_obj = json_tokener_parse_ex(tok, in, len);
    json_type type = json_object_get_type(json_main_obj);
    if (type == json_type_array){
      size = json_object_array_length(json_main_obj);
    } else {
      size = 1;
    }

    for (size_t i = 0; i < size; i++){
      if (size == 1){
        tx_msg = ran_msg_handle(json_main_obj);
      } else {
        struct json_object* cur_json_obj = json_object_array_get_idx(json_main_obj, i);
        tx_msg = ran_msg_handle(cur_json_obj);
      }

      vhd->amsg.len = strlen(tx_msg);
      /* notice we over-allocate by LWS_PRE */
      // TODO: Fix BIG memory leak here. LWS_CALLBACK_RECEIVE should only handle 1 message not 2.
      vhd->amsg.payload = malloc(LWS_PRE + vhd->amsg.len);
      if (!vhd->amsg.payload) {
        lwsl_user("OOM: dropping\n");
        break;
      }

      memcpy((char *)vhd->amsg.payload + LWS_PRE, tx_msg, vhd->amsg.len);
      vhd->current++;

      /*
       * let everybody know we want to write something on them
       * as soon as they are ready
       */
      lws_start_foreach_llp(struct per_session_data__minimal **,
      ppss, vhd->pss_list) {
        lws_callback_on_writable((*ppss)->wsi);
      } lws_end_foreach_llp(ppss, pss_list);
    }

    json_tokener_free(tok);
    json_object_put(json_main_obj);
    break;
	default:
		break;
	}

	return 0;
}

#define LWS_PLUGIN_PROTOCOL_MINIMAL \
	{ \
		"ws-client", \
		callback_minimal, \
		sizeof(struct per_session_data__minimal), \
		3000, \
		0, NULL, 0 \
	}