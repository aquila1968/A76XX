#ifndef A76XX_MQTT_CLIENT_H_
#define A76XX_MQTT_CLIENT_H_

class MQTTOnMessageRx : public EventHandler_t {
  public:
    StaticQueue<MQTTMessage_t, MQTT_MESSAGE_SIZE>  messageQueue;
    
    MQTTOnMessageRx()
        : EventHandler_t(A76XXURC_t::MQTT_MESSAGE_RX, "+CMQTTRXSTART: ") {}
    
    void process(ModemSerial* serial);
};

class MQTTOnConnectionLost : public EventHandler_t {
  public:
    MQTTOnConnectionLost()
        : EventHandler_t(A76XXURC_t::MQTT_CONNECTION_LOST, "+CMQTTCONNLOST: ") {}
    
    void process(ModemSerial* serial) {}
};

class MQTTOnNoNet : public EventHandler_t {
  public:
    MQTTOnNoNet()
        : EventHandler_t(A76XXURC_t::MQTT_NO_NET, "+CMQTTNONET: ") {}
    
    void process(ModemSerial* serial) {}
};


class A76XXMQTTClient : public A76XXSecureClient {
  private:
    MQTTCommands                                     _mqtt_cmds;
    const char*                                       _clientID;
    bool                                               _use_ssl;
    MQTTOnMessageRx                      _on_message_rx_handler;
    MQTTOnConnectionLost            _on_connection_lost_handler;
    MQTTOnNoNet                              _on_no_net_handler;

    // these two are set to zero by default until a use
    // case for allowing these to change comes up
    uint8_t                   _client_index;
    uint8_t                     _session_id;

  public:
    /*
        @brief Construct a native MQTT client instance.

        @details This class inherits from A76XXBaseClient the functionality to
            set the certificates used for SSL/TLS encription. After instantiation
            you should call the relevant function to download the required certificates
            to the SIMCOM module.
        @param [IN] modem An A76XX modem instance.
        @param [IN] clientID The client ID used for connecting to the broker.
        @param [IN] use_ssl Whether SSL/TLS encryption should be used.
    */
    A76XXMQTTClient(A76XX& modem, const char* clientID, bool use_ssl = false);

    /*
        @brief Start the MQTT service.

        @detail Must be called before any other MQTT operations.
        @return True if the service was started successfully. If false, use
            getLastError() to get detail on the error
    */
    bool begin();

    /*
        @brief Connect to the MQTT server.

        @param [IN] server the server domain name or IP address. Must start with
            "tcp://", e.g. "tcp://test.mosquitto.org".
        @param [IN] port the port we should connect to.
        @param [IN] clean_session where we should start a clean MQTT session.
        @param [IN] keep_alive time in second between PINGREQ packets to keep the
            connection alive.
        @param [IN] username Optional username to authenticate with the broker.
        @param [IN] password Optional password to authenticate with the broker.
        @param [IN] will_topic The topic for the will message - optional.
        @param [IN] will_message The will message - optional.
        @param [IN] will_qos The quality of service of the will message - optional.
        @return True if the connection was established successfully. If false, use
            getLastError() to get detail on the error.
    */
    bool connect(const char* server_name,
                 int port,
                 bool clean_session,
                 int keep_alive = 60,
                 const char* username = NULL,
                 const char* password = NULL,
                 const char* will_topic = NULL,
                 const char* will_message = NULL,
                 int will_qos = 0);

    /*
        @brief Disconnect from the broker.

        @param [IN] timeout Return if the server does not respond before this timeout
            in seconds.
        @return True on success. If false, use getLastError() to get detail on the error.
    */
    bool disconnect(uint8_t timeout = 60);

    /*
        @brief Release the client and terminate the MQTT service.

        @return True on success. If false, use getLastError() to get detail on the error.
    */
    bool end();

    /*
        @brief Publish a message.

        @param [IN] topic The message topic.
        @param [IN] payload The message.
        @param [IN] length The length of the message.
        @param [IN] qos The quality of service of the message: 0, 1 or 2.
        @param [IN] pub_timeout Return if the message is not acknowledged before
            this timeout. The range is from 1 to 180 seconds.
        @param [IN] retained The retain flag of the publish message.
        @param [IN] dup The dup flag to the message.

        @return True on success. If false, use getLastError() to get detail on the error
    */
    bool publish(const char* topic,
                 const uint8_t* payload,
                 uint32_t length,
                 uint8_t qos,
                 uint8_t pub_timeout,
                 bool retained = false,
                 bool dup = false);

    /*
        @brief Publish a message.

        @param [IN] topic The message topic.
        @param [IN] payload The message as a character string
        @param [IN] qos The quality of service of the message: 0, 1 or 2.
        @param [IN] pub_timeout Return if the message is not acknowledged before
            this timeout. The range is from 1 to 180 seconds.
        @param [IN] retained The retain flag of the publish message.
        @param [IN] dup The dup flag to the message.

        @return True on success. If false, use getLastError() to get detail on the error
    */
    bool publish(const char* topic,
                 const char* payload,
                 uint8_t qos,
                 uint8_t pub_timeout,
                 bool retained = false,
                 bool dup = false);

    /*
        @brief Subscribe to a topic.

        @param [IN] topic The topic to subscribe to.
        @param [IN] qos The quality of service of the subscription. Default is 0.
        @return True on successful subscription, false otherwise.
    */
    bool subscribe(const char* topic, uint8_t qos = 0);

    /*
        @brief Check if a message has been received.

        @details This is a blocking function that waits for data available from 
            the module serial connection and parses MQTT messages, if any. You
            should call this function as often as possible to avoid missing 
            any messages.

        @param [IN] timeout Return if no message is received within this time in 
            milliseconds. Default is 100 ms.
        @return True if a message is available.
    */
    uint32_t messageAvailable();

    /*
        @brief Get last message received.

        @details Calling this function repeatedly returns the same message. To avoid 
            duplicate messages call this function only once after `checkMessage` has
            returned true.

        @return A MQTTMessage_t object, with fields `topic` and `payload`.
    */
    MQTTMessage_t getMessage();

    /*
        @brief Check if the connection with the broker is active or not.
    */
    bool isConnected();
};

#endif A76XX_MQTT_CLIENT_H_