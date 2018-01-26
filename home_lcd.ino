#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// security
// =================================
const String ACCESS_TOKEN = "sky";

// wifi cred & settings
// =================================
const char* ssid = "coldspot-DAF";
const char* password = "";
bool wifiConnected = false;

// the web api port
// =================================
const int API_SERVER_PORT = 8000;

ESP8266WebServer *apiServer = NULL;

// the switch controller
// =================================
const int PIN_OUT_SWITCH_CONTROLLER_D0 = 16;

// relay values
const bool ON_OUTPUT = LOW;
const bool OFF_OUTPUT = HIGH;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
// =================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(115200);
  
  // setup pin
  pinMode(PIN_OUT_SWITCH_CONTROLLER_D0, OUTPUT);
  digitalWrite(PIN_OUT_SWITCH_CONTROLLER_D0, OFF_OUTPUT); 
  
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.noCursor();
  lcd.clear();
  
  lcd.print("HOME AUTOMATION");
  lcd.setCursor(0,1);
  lcd.print("By DF Systems");

  delay(2000);

  wifiConnected = connectWifi();

  delay(2000);

  if(wifiConnected) {
    setupControlServer();
  }

  if(apiServer != NULL) {
    
    digitalWrite(PIN_OUT_SWITCH_CONTROLLER_D0, ON_OUTPUT);

    lcd.clear();
    lcd.print("Controller");
    lcd.setCursor(0,1);
    lcd.print("Switched ON");

    delay(2000);

    lcd.clear();
    lcd.print("Waiting for");
    lcd.setCursor(0,1);
    lcd.print("commands..");
  }
}

void loop()
{
  
  if(wifiConnected) {
    
    if(apiServer != NULL) {
      apiServer->handleClient();
      delay(1);
    }
  }
}

// setup web api
void setupControlServer() {

  lcd.clear();
  lcd.print("Setup Web Api");
  lcd.setCursor(0,1);
  lcd.print("........");

  delay(1500);

  apiServer = new ESP8266WebServer(API_SERVER_PORT);

  apiServer->on("/", [&]() {
    handleWebApi();
  });
  
  apiServer->begin();

  lcd.clear();
  lcd.print("Web API READY!");
  lcd.setCursor(0,1);
  lcd.print("PORT: " + String(API_SERVER_PORT));

  delay(5000);

  lcd.clear();
  lcd.print("Switching");
  lcd.setCursor(0,1);
  lcd.print("Controller..");

  delay(5000);
}

// handle web api
void handleWebApi() {
  
    String argName1 = apiServer->argName(0); // access token
    String argName2 = apiServer->argName(1); // display1
    String argName3 = apiServer->argName(2); // display2
    
    String token = apiServer->arg(0);

    if(token != ACCESS_TOKEN) {
      apiServer->send(401, "text/plain", "access denied!");
      return;
    }

    lcd.clear();

    if(argName2 != "") {
      String text1 = apiServer->arg(1);
      lcd.print(text1);
    }
    if(argName3 != "") {
      String text2 = apiServer->arg(2);
      lcd.setCursor(0,1);
      lcd.print(text2);
    }

    apiServer->send(200, "text/plain", "ok");
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  
  boolean state = true;
  int i = 0;
  IPAddress ip(192, 168, 22, 21);
  IPAddress gateway(192, 168, 22,1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  lcd.clear();
  lcd.print("Connecting Wifi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    lcd.clear();
    lcd.print(ssid);
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
    delay(2000);
  }
  else {
    lcd.clear();
    lcd.print("Connection Failed.");
  }
  
  return state;
}
