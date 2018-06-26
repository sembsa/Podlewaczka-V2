#include <PubSubClient.h>

class shk_mqtt {
    private:
        PubSubClient _mqtt;
        String _name;
        String _module;
        String _function;
    public:
        String _topicSet;
        String _topicStatus;
        shk_mqtt(PubSubClient &mqtt, String name, String module, String function);
        void subscribeTopic();
        void showTopic();
        void publishStatus(String payload);
        String returnTopicSet();
        String returnTopicStatus();
        ~shk_mqtt();
};

shk_mqtt::shk_mqtt(PubSubClient &mqtt, String name, String module, String function) {
    _mqtt = mqtt;
    _name = name;
    _module = module;
    _function = function;
    _topicSet = "/shk/"+name+"/"+module+"/set/"+function;
    _topicStatus = "/shk/"+name+"/"+module+"/status/"+function;
}

void shk_mqtt::subscribeTopic() {
    _mqtt.subscribe(_topicSet.c_str());
    //_mqtt.subscribe(_topicStatus.c_str());
}

void shk_mqtt::showTopic() {
    Serial.println("---------------------\nName: "+_name+"\nModule: "+_module+"\nFunction: "+_function+"\n\nTopic Set: "+_topicSet+"\nTopic Status: "+_topicStatus+"\n---------------------");
}

void shk_mqtt::publishStatus(String payload) {
    _mqtt.publish(_topicStatus.c_str(), payload.c_str());
}

shk_mqtt::~shk_mqtt() {

}

