//Librerias
#include <WiFi101.h>  //Libreria del Arduino MKR1000 para WiFi

//Etiquetado de pines de I/O
#define POT 0 //Potenciometro conectado al pin A0
#define POT100 1

//Constantes
const unsigned long tpost = 1000;  //Constante de tiempo de enviar al servidor de TWX, 1000 millisegundos
const unsigned int sensorCount = 2;  //Constante del # de variables a enviar a TWX (2)
//const char* ssid = "IoT-B19"; //WiFi SSID
//const char* password = "meca2017*"; //WiFi Pass
const char* ssid = "PCOF-2174"; //WiFi SSID
const char* password = "alejocram"; //WiFi Pass
const char* host = "67.205.161.46";  //TWX Host

//Variables
//->WiFi Shield
WiFiClient client;
//->TWX Vars
char thingName[] = "clima_thing";  //Nombre del objeto en TWX
char serviceName[] = "clima_service";  //Nombre del servicio en TWX
char* propertyNames[] = {"var1", "var2"};  //Vector de los nombres de las vars a enviar a TWX
float propertyValues[sensorCount]; //Vector de los valores de las vars a enviar
//->Variables para temporización
unsigned long lastConnectionTime = 0; //Variable para almacenar el último tiempo que se envió al servidor las vars

//Subrutinas y/o funciones
void printWifiStatus() {
  //Imprimir nombre de la red SSID
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  //Imprimir la ipV4 asignada al modulo
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  //Imprimir la fuerza de la señal WiFi
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void WiFiInit() {
  delay(1000);  //Esperar un tiempo a que el modulo inicie (1 sec)

  //Revisar que el modulo WiFi si este conectado
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    //No continuar
    while (true);
  }

  //Tratar de conectarse a la red WiFI por medio del SSID
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, password);
    delay(10000); //Esperar 10 secs a la conexion
  }
  //Imprimir el estado de la red
  printWifiStatus();
}

void POST(const int sensorCount, char* sensorNames[], float values[]) {
  String url = "/add";
  String body = "";
  body += "{";
  for (int idx = 0; idx < sensorCount; idx++) {
    if (idx != 0) body += ",";
    body += "\"";
    body += sensorNames[idx];
    body += "\"";
    body += ": ";
    body += "\"";
    body += values[idx];
    body += "\"";
  }
  body += "}";
  Serial.println(body);
  if (client.connect(host, 5000)) {
    Serial.println("Connected to: " + String(host) + ":" + String(5000));
    //Send the HTTP POST request:
    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
    "Host: " + host + ":5000\r\n" +
    "Content-Type: application/json\r\n" +
    "Content-Length: " + body.length() + "\r\n" +
//    "Connection: close" + "\r\n"
    "\r\n" +
    body + "\r\n\r\n");

    Serial.print(String("POST ") + url + " HTTP/1.1\r\n" +
    "Host: " + host + ":5000\r\n" +
    "Content-Type: application/json\r\n" +
    "Content-Length: " + body.length() + "\r\n" +
    "Connection: close" + "\r\n\r\n" +
//    "appKey: " + appKey + "\r\n\r\n" +
    body + "\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    client.stop();
  }
  else {
    Serial.println("The connection could not be established");
    client.stop();
  }
}

void setup() {
  //Decir que es entrada y que es salida

  //Limpieza de salidas fisicas

  //Communicaciones
  Serial.begin(9600); //Inicio comunicaciones seriales con el PC a 9600 bauds
  WiFiInit(); //Inicio de comunicaciones WiFi
}

void loop() {
//  propertyValues[0] = analogRead(POT) * 100.0 / 1023.0; //Leo lo que tenga el potenciometro y lo escalizo de 0 a 100%
//  propertyValues[1] = analogRead(POT100) * 100.0 / 1023.0;; //Escribo otro valor para ensayar envio de 2 vars
  propertyValues[0] = (random(100)/100.0)*29.0; //Comment if using real potentiometer
  propertyValues[1] = (random(100)/100.0)*29.0; //Comment if using real potentiometer
  if (millis() - lastConnectionTime > tpost) {
    POST(sensorCount, propertyNames, propertyValues); //Envio los valores a la plataforma IoT
    lastConnectionTime = millis();  //Tomo de nuevo el tiempo actual de conexion
  }
}


