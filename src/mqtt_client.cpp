#include "A76XX.h"

A76XXMQTTClient::A76XXMQTTClient(A76XX& modem, const char* clientID, bool use_ssl)
    : A76XXBaseClient(modem)
    , _mqtt_cmds(modem)
    , _clientID(clientID)
    , _use_ssl(use_ssl)
    , _client_index(0)
    , _session_id(0) {}

bool A76XXMQTTClient::begin() {
    // start
    int8_t retcode = _mqtt_cmds.start();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // acquire client
    uint8_t server_type = _use_ssl ? 1 : 0;
    retcode = _mqtt_cmds.acquire_client(_client_index, _clientID, server_type);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    // set ssl context for mqtt
    if (_use_ssl) {
        retcode = _mqtt_cmds.set_SSL_context(_session_id, _ssl_ctx_index);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    return true;
}

bool A76XXMQTTClient::connect(const char* server_name, int port,
                              bool clean_session,
                              int keepalive,
                              const char* username,
                              const char* password,
                              const char* will_topic,
                              const char* will_message,
                              int will_qos) {
    int8_t retcode;

    if (will_message != NULL && will_topic != NULL) {
        retcode = _mqtt_cmds.set_will_topic(_client_index, will_topic);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
        retcode = _mqtt_cmds.set_will_message(_client_index, will_message, will_qos);
        A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    }

    retcode = _mqtt_cmds.connect(_client_index, server_name, port, clean_session, keepalive, username, password);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXMQTTClient::disconnect(uint8_t timeout) {
    int8_t retcode = _mqtt_cmds.disconnect(_client_index, timeout);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);
    return true;
}

bool A76XXMQTTClient::end() {
    int8_t retcode = _mqtt_cmds.release_client(_client_index);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _mqtt_cmds.stop();
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXMQTTClient::publish(const char* topic,
                              const uint8_t* payload,
                              uint32_t length,
                              uint8_t qos,
                              uint8_t pub_timeout,
                              bool retained,
                              bool dup) {
    int8_t retcode = _mqtt_cmds.set_topic(_client_index, topic);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _mqtt_cmds.set_payload(_client_index, payload, length);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    retcode = _mqtt_cmds.publish(_client_index, qos, pub_timeout, retained, dup);
    A76XX_CLIENT_RETCODE_ASSERT_BOOL(retcode);

    return true;
}

bool A76XXMQTTClient::publish(const char* topic,
                              const char* payload,
                              uint8_t qos,
                              uint8_t pub_timeout,
                              bool retained,
                              bool dup) {
    return publish(topic, reinterpret_cast<const uint8_t*>(payload), strlen(payload), qos,
            pub_timeout, retained, dup);
}

bool A76XXMQTTClient::isConnected() {
    return _mqtt_cmds.is_connected(_client_index);
}