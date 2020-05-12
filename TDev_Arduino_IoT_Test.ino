// libraries
#include <MKRNB.h>
#include <PubSubClient.h>
//The MKRNB lib needs this even if its blank
const char PINNUMBER[]     = "";
#define token "nbFUs&JmTw5eZoxoJw"
#define ORG "43wcdg"
#define DEVICE_TYPE "mkr"
#define DEVICE_ID "mkr"
//broker url in here, demo is using eclipse test server
char mqttBroker[]  = ORG ".messaging.internetofthings.ibmcloud.com";
//broker port in here
int mqttPort = 1883;
char authMethod[] = "use-token-auth";
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
//var things for storage
//char payload[200];
const char topicSubscribe[] = "iot-2/cmd/default/fmt/text";
// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;
//connect the pubsub client
PubSubClient conn(client);
//for the callbacks from the broker
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String msg(p);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  Serial.println(msg);
  if(msg=="on"){
    digitalWrite(LED_BUILTIN, HIGH);
  } else if ( msg == "off") {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
//connection and reconnection function 
void reconnect() {
    while (!conn.connected()) {
    // Attemp to connect
    if (conn.connect(clientId, authMethod, token)) {
      Serial.println("Connected");
      conn.subscribe(topicSubscribe);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(conn.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Warming up....");
  // connection state
  boolean connected = false;
  while (!connected) {
    if ((nbAccess.begin(PINNUMBER) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  Serial.println("connecting...");
  //set the connection
  conn.setServer(mqttBroker, mqttPort);
  //set the callback 
  conn.setCallback(callback);
  //subscribe to a topic 
  conn.subscribe(topicSubscribe);
}
void loop() {
  if (!conn.connected()) {
    reconnect();
  }
  conn.loop();
}
