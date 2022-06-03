#include "WiFi.h"
#include "ESPAsyncWebServer.h"
//***************************************************************************************************************************************************************
#define LED 2
#define reed_switch 4

//***************************************************************************************************************************************************************
// Replace with your network credentials
const char* ssid = "Ali";
const char* password = "554151955";

unsigned long previousMillis = 0;
unsigned long interval = 30000;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//***************************************************************************************************************************************************************
String readDoorStatus() {
  int reed_status;
  reed_status = digitalRead(reed_switch);
  if (reed_status == 1){
    digitalWrite(LED, LOW);
    return String("Opened");
  }
  else{
    digitalWrite(LED, HIGH);
    return String("Closed");
  }
  
  delay(1000);
  return String(reed_status);
}

//***************************************************************************************************************************************************************

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 Intruder Alert!</h2>
  <p>
    <span class="dht-labels">Door Status</span> 
    <span id="doorStatus">%DOORSTATUS%</span>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("doorStatus").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/doorStatus", true);
  xhttp.send();
}, 1000 ) ;
</script>
</html>)rawliteral";

String processor(const String& var){
  //Serial.println(var);
if(var == "DOORSTATUS"){
    return readDoorStatus();
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

//  dht.begin();
pinMode(LED, OUTPUT);
pinMode(reed_switch, INPUT);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("Connected to WiFi");
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/doorStatus", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDoorStatus().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  unsigned long currentMillis = millis();
// if WiFi is down, try reconnecting
if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
  Serial.print(millis());
  Serial.println("Reconnecting to WiFi...");
  WiFi.disconnect();
  Serial.println("Disconnected from WiFi...");
  WiFi.reconnect();
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi Again...");
  Serial.println(WiFi.localIP());
  previousMillis = currentMillis;
  
}
}
