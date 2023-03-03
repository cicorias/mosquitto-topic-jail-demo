#include <stdio.h>
#include <string.h>

#include "mosquitto_broker.h"
#include "mosquitto_plugin.h"
#include "mosquitto.h"
#include "mqtt_protocol.h"

#define PLUGIN_NAME "topic-jail"
#define PLUGIN_VERSION "1.0"

#define UNUSED(A) (void)(A)

MOSQUITTO_PLUGIN_DECLARE_VERSION(5);

static mosquitto_plugin_id_t *mosq_pid = NULL;

static bool is_jailed(const char *str)
{
	return strncmp("jailed", str, 6) == 0;
}

static int callback_message_in(int event, void *event_data, void *userdata)
{
	struct mosquitto_evt_message *ed = event_data;
	char *new_topic;
	size_t new_topic_len;

	UNUSED(event);
	UNUSED(userdata);

	const char *clientid = mosquitto_client_id(ed->client);

	if(!is_jailed(clientid)){
		/* will only modify the topic of jailed clients */
		return MOSQ_ERR_SUCCESS;
	}

	/* put the clientid on front of the topic */

	/* calculate the length of the new payload */
	new_topic_len = strlen(clientid) + sizeof('/') + strlen(ed->topic) + 1;

	/* Allocate some memory - use
	 * mosquitto_calloc/mosquitto_malloc/mosquitto_strdup when allocating, to
	 * allow the broker to track memory usage */
	new_topic = mosquitto_calloc(1, new_topic_len);
	if(new_topic == NULL){
		return MOSQ_ERR_NOMEM;
	}

	/* prepend the clientid to the topic */
	snprintf(new_topic, new_topic_len, "%s/%s", clientid, ed->topic);

	/* Assign the new topic to the event data structure. You
	 * must *not* free the original topic, it will be handled by the
	 * broker. */
	ed->topic = new_topic;

	return MOSQ_ERR_SUCCESS;
}

static int callback_message_out(int event, void *event_data, void *userdata)
{
	struct mosquitto_evt_message *ed = event_data;
	size_t clientid_len;

	UNUSED(event);
	UNUSED(userdata);

	const char *clientid = mosquitto_client_id(ed->client);

	if(!is_jailed(clientid)){
		/* will only modify the topic of jailed clients */
		return MOSQ_ERR_SUCCESS;
	}

	/* remove the clientid from the front of the topic */
	clientid_len = strlen(clientid);

	if(strlen(ed->topic) <= clientid_len + 1){
		/* the topic is not long enough to contain the
		 * clientid + '/' */
		return MOSQ_ERR_SUCCESS;
	}

	if(!strncmp(clientid, ed->topic, clientid_len) && ed->topic[clientid_len] == '/'){
		/* Allocate some memory - use
		 * mosquitto_calloc/mosquitto_malloc/mosquitto_strdup when allocating, to
		 * allow the broker to track memory usage */

		/* skip the clientid + '/' */
		char *new_topic = mosquitto_strdup(ed->topic + clientid_len + 1);

		if(new_topic == NULL){
			return MOSQ_ERR_NOMEM;
		}

		/* Assign the new topic to the event data structure. You
		 * must *not* free the original topic, it will be handled by the
		 * broker. */
		ed->topic = new_topic;
	}

	return MOSQ_ERR_SUCCESS;
}

static int callback_subscribe(int event, void *event_data, void *userdata)
{
	struct mosquitto_evt_subscribe *ed = event_data;
	char *new_sub;
	size_t new_sub_len;

	UNUSED(event);
	UNUSED(userdata);

	const char *clientid = mosquitto_client_id(ed->client);

	if(!is_jailed(clientid)){
		/* will only modify the topic of jailed clients */
		return MOSQ_ERR_SUCCESS;
	}

	/* put the clientid on front of the topic */

	/* calculate the length of the new payload */
	new_sub_len = strlen(clientid) + sizeof('/') + strlen(ed->data.topic_filter) + 1;

	/* Allocate some memory - use
	 * mosquitto_calloc/mosquitto_malloc/mosquitto_strdup when allocating, to
	 * allow the broker to track memory usage */
	new_sub = mosquitto_calloc(1, new_sub_len);
	if(new_sub == NULL){
		return MOSQ_ERR_NOMEM;
	}

	/* prepend the clientid to the subscription */
	snprintf(new_sub, new_sub_len, "%s/%s", clientid, ed->data.topic_filter);

	/* Assign the new topic to the event data structure. You
	 * must *not* free the original topic, it will be handled by the
	 * broker. */
	ed->data.topic_filter = new_sub;

	return MOSQ_ERR_SUCCESS;
}

static int callback_unsubscribe(int event, void *event_data, void *userdata)
{
	struct mosquitto_evt_unsubscribe *ed = event_data;
	char *new_sub;
	size_t new_sub_len;

	UNUSED(event);
	UNUSED(userdata);

	const char *clientid = mosquitto_client_id(ed->client);

	if(!is_jailed(clientid)){
		/* will only modify the topic of jailed clients */
		return MOSQ_ERR_SUCCESS;
	}

	/* put the clientid on front of the topic */

	/* calculate the length of the new payload */
	new_sub_len = strlen(clientid) + sizeof('/') + strlen(ed->data.topic_filter) + 1;

	/* Allocate some memory - use
	 * mosquitto_calloc/mosquitto_malloc/mosquitto_strdup when allocating, to
	 * allow the broker to track memory usage */
	new_sub = mosquitto_calloc(1, new_sub_len);
	if(new_sub == NULL){
		return MOSQ_ERR_NOMEM;
	}

	/* prepend the clientid to the subscription */
	snprintf(new_sub, new_sub_len, "%s/%s", clientid, ed->data.topic_filter);

	/* Assign the new topic to the event data structure. You
	 * must *not* free the original topic, it will be handled by the
	 * broker. */
	ed->data.topic_filter = new_sub;

	return MOSQ_ERR_SUCCESS;
}


int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **user_data, struct mosquitto_opt *opts, int opt_count)
{
	UNUSED(user_data);
	UNUSED(opts);
	UNUSED(opt_count);

	mosq_pid = identifier;
	mosquitto_plugin_set_info(identifier, PLUGIN_NAME, PLUGIN_VERSION);

	int rc;

	rc = mosquitto_callback_register(mosq_pid, MOSQ_EVT_MESSAGE_IN, callback_message_in, NULL, NULL);
	if(rc) return rc;
	rc = mosquitto_callback_register(mosq_pid, MOSQ_EVT_MESSAGE_OUT, callback_message_out, NULL, NULL);
	if(rc) return rc;
	rc = mosquitto_callback_register(mosq_pid, MOSQ_EVT_SUBSCRIBE, callback_subscribe, NULL, NULL);
	if(rc) return rc;
	rc = mosquitto_callback_register(mosq_pid, MOSQ_EVT_UNSUBSCRIBE, callback_unsubscribe, NULL, NULL);
	return rc;
}
