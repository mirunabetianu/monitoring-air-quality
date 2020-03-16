#include "DHTesp.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
DHTesp dht;

float t = 0.0;
float h = 0.0;

char* ssid = "WPS-Dongle 104";
char* password = "";

ESP8266WebServer server;

WebSocketsServer webSocket = WebSocketsServer(81);

const int redPin = 13;
const int greenPin = 12;
const int bluePin = 14;

char webpage[] PROGMEM = R"=====(
<html>
<!-- Adding a data chart using Chart.js -->
<head>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js'></script>
</head>
<body onload="javascript:init()">
<hr />
<div>
  <canvas id="temp-chart" width="800" height="450"></canvas>
  <canvas id="hum-chart" width ="800" height ="450"> </canvas>
  <canvas id="gas-chart" width ="800" height ="450"> </canvas>
</div>
<script>
  var webSocket, dataPlot1, dataPlot2, dataPlot3;
  var maxDataPoints = 20;
  function removeData(){
    dataPlot1.data.labels.shift();
    dataPlot1.data.datasets[0].data.shift();
    dataPlot2.data.labels.shift();
    dataPlot2.data.datasets[0].data.shift();
    dataPlot3.data.labels.shift();
    dataPlot3.data.datasets[0].data.shift();
  }
  function addData(label, data1, data2, data3) {
    if(dataPlot1.data.labels.length > maxDataPoints) removeData();
    dataPlot1.data.labels.push(label);
    dataPlot1.data.datasets[0].data.push(data1);
    dataPlot1.update();

    if(dataPlot2.data.labels.length > maxDataPoints) removeData();
    dataPlot2.data.labels.push(label);
    dataPlot2.data.datasets[0].data.push(data2);
    dataPlot2.update();

    if(dataPlot3.data.labels.length > maxDataPoints) removeData();
    dataPlot3.data.labels.push(label);
    dataPlot3.data.datasets[0].data.push(data3);
    dataPlot3.update();
  }
  function init() {
    webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
    dataPlot1 = new Chart(document.getElementById("temp-chart"), {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          data: [],
          label: "Temperature (C)",
          borderColor: "#3e95cd",
          fill: false
        }
        ]
      }
    });
    dataPlot2 = new Chart(document.getElementById("hum-chart"), {
      type: 'line',
      data: {
        labels: [],
        datasets: [
        {
          data: [],
          label: "Humidity (%)",
          borderColor: "#3e95cd",
          fill: false
        },
        ]
      }
    });
    dataPlot3 = new Chart(document.getElementById("gas-chart"), {
      type: 'line',
      data: {
        labels: [],
        datasets: [
        {
          data: [],
          label: "Air quality",
          borderColor: "#3e95cd",
          fill: false
        },
        ]
      }
    });
    webSocket.onmessage = function(event) {
      var data = JSON.parse(event.data);
      console.log(data.value1);
      console.log(data.value2);
      console.log(data.value3);
      var today = new Date();
      var t = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
      addData(t, data.value1, data.value2, data.value3);
    }
  }
</script>
</body>
</html>
)=====";

void setup()
{
  digitalWrite(0, LOW); // sets output to gnd
  pinMode(0, OUTPUT); // switches power to DHT on
  delay(1000); // delay necessary after power up for DHT to stabilize
  WiFi.begin(ssid,password);
  Serial.begin(9600);
  while(WiFi.status()!= WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",[](){
    server.send_P(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  dht.setup(0, DHTesp::DHT22);
}

void loop()
{
  getData();
  webSocket.loop();
  server.handleClient();
}
void getData()
{
  delay(2000);
  float temp = dht.getTemperature();
  float hum = dht.getHumidity();
  float gas = analogRead(A0);

  if(hum>55 || gas > 510)
  {
    setColor(255,0,0); 
  }else
  {
    if(temp < 20 || temp > 27)
    {
      setColor(0,0,255);
    }else
      setColor(0,255,0);
  }
  
  if(!isnan(temp)) {Serial.println(temp); t = temp;}
  else Serial.println("Failed reading from the sensor");
  if(!isnan(hum)) {Serial.println(temp); h = hum;}
  else Serial.println("Failed reading from the sensor");
  String json = "{\"value1\":";
  json += t;
  json += ",";
  json += "\"value2\":";
  json += h;
  json += ",";
  json += "\"value3\":";
  json += gas;
  json += "}";
  webSocket.broadcastTXT(json.c_str(), json.length());
}

void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  
  
}
