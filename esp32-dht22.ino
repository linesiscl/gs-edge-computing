#include <PubSubClient.h>
#include <WiFi.h>
#define botao 12
#define led 2

#define TOPIC_PUBLISH_LED "topic/led/valor"  
#define ID_MQTT "clientId-GS-edgecomp"

const char *SSID = "Wokwi-GUEST"; 
const char *PASSWORD = "";
const char *BROKER_MQTT = "broker.hivemq.com";
int BROKER_PORT = 1883;

unsigned long publishUpdate;

WiFiClient espClient;         
PubSubClient MQTT(espClient);

void initWiFi(void);
void initMQTT(void);
void reconnectMQTT(void);
void checkWiFIAndMQTT(void);
void reconnectWiFi(void);

void initWiFi(void)
{
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconnectWiFi();
}

void initMQTT(void)
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); // Informa qual broker e porta deve ser conectado
  MQTT.setCallback(callbackMQTT);
}

void callbackMQTT(char *topic, byte *payload, unsigned int length) {
  String msg = String((char*)payload).substring(0, length);
  
  Serial.printf("Mensagem recebida via MQTT: %s do tópico: %s\n", msg.c_str(), topic);

  
}

void reconnectMQTT() {
  while (!MQTT.connected()){
    Serial.print("Tentando conexão com MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)){
      Serial.println("Conectado ao broker");

    }else{
      Serial.println("Falha na conexão com MQTT. Tentando novamente em dois segundos");
      Serial.print(MQTT.state());
      delay(2000);
    }
  }
}

void checkWiFIAndMQTT(void)
{
  if (!MQTT.connected())
    reconnectMQTT(); // se não há conexão com o Broker, a conexão é refeita

  reconnectWiFi(); // se não há conexão com o WiFI, a conexão é refeita
}


void reconnectWiFi(void)
{
  // se já está conectado a rede WI-FI, nada é feito.
  // Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(115200);
  pinMode(botao, INPUT);
  pinMode(led, OUTPUT);
  initWiFi();
  initMQTT();
}

void loop() {
    checkWiFIAndMQTT();
    MQTT.loop();

    int estado = digitalRead(botao);
    if (estado == HIGH) {
      digitalWrite(led, HIGH);
      MQTT.subscribe(TOPIC_PUBLISH_LED);
      MQTT.publish(TOPIC_PUBLISH_LED, "Batimento cardíaco em alta");
      Serial.println("Batimento cardíaco em alta");
    }
    else{
      digitalWrite(led, LOW);
    }

  

}


