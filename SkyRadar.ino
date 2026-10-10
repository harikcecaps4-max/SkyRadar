#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>
TFT_eSPI tft = TFT_eSPI();
const int CENTER_X=120;
const int CENTER_Y=120;
const int MAX_RADIUS=115;
const int BUTTON_PIN=9;
const char* ssid = "my_wifi_ssid";
const char* password = "wifi_password";
const float HOME_LAT =12.9716;
const float HOME_LON =77.5946;
const float LAT_MIN =HOME_LAT-0.45;
const float LAT_MAX=HOME_LAT+0.45;
const float LON_MIN = HOME_LON-0.45;
const float LON_MAX = HOME_LON+0.45;
float sweepAngle =0.0;
unsigned long lastButtonPress=0;
unsigned long lastApiFetch=0;
const unsigned long FETCH_INTERVAL=15000;
int currentRangeIndex=2;
const int ranges[]={10,25,50,100};
struct Flight{
  char callsign[9];
  char type[8];
  int distanceKm;
  float angleRad;
  bool active;
};
const int MAX_FLIGHTS=8;
Flight flights[MAX_FLIGHTS];
int activeFlightCount=0;
void setup(){
  Serial.begin(115200);
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN,TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Connecting Wi-Fi...",CENTER_X,CENTER_Y, 2);
  WiFi.begin(ssid,password);
  int attempts=0;
  while (WiFi.status()!=WL_CONNECTED&&attempts<15){
    delay(500);
    attempts++;
  }
  tft.fillScreen(TFT_BLACK);
  drawRadarGrid();
  loadFallbackData();
}
void loop(){
  if (digitalRead(BUTTON_PIN)==LOW){
    if (millis()-lastButtonPress>250){
      currentRangeIndex =(currentRangeIndex+1)%4;
      lastButtonPress=millis();
      tft.fillScreen(TFT_BLACK);
      drawRadarGrid();
    }
  }
  if (WiFi.status()==WL_CONNECTED && (millis()-lastApiFetch>FETCH_INTERVAL)){
    fetchLiveFlightData();
    lastApiFetch = millis();
  }
  updateRadarSweep();
  drawAircraft();
  delay(15);
}
void drawRadarGrid(){
  tft.drawCircle(CENTER_X,CENTER_Y,40,TFT_GREEN);
  tft.drawCircle(CENTER_X,CENTER_Y,75,TFT_GREEN);
  tft.drawCircle(CENTER_X,CENTER_Y,MAX_RADIUS,TFT_GREEN);
  tft.drawFastHLine(15,CENTER_Y,210,TFT_DARKGREEN);
  tft.drawFastVLine(CENTER_X,15,210,TFT_DARKGREEN);
  tft.setTextColor(TFT_GREEN,TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("N",CENTER_X,2,1);
  tft.setTextDatum(BC_DATUM);
  tft.drawString("S",CENTER_X,222,1);
  tft.setTextDatum(MR_DATUM);
  tft.drawString("W",10,CENTER_Y,1);
  tft.setTextDatum(ML_DATUM);
  tft.drawString("E",230,CENTER_Y,1);
  tft.setTextDatum(ML_DATUM);
  String rangeStr = String(ranges[currentRangeIndex])+"km";
  tft.drawString(rangeStr,15,CENTER_Y+12,1);
  tft.setTextDatum(BC_DATUM);
  tft.drawString("SkyRadar",CENTER_X,238,1);
}
void updateRadarSweep(){
  int oldX = CENTER_X+(int)(MAX_RADIUS*cos(sweepAngle));
  int oldY = CENTER_Y+(int)(MAX_RADIUS*sin(sweepAngle));
  tft.drawLine(CENTER_X,CENTER_Y,oldX,oldY,TFT_BLACK);
  tft.drawCircle(CENTER_X,CENTER_Y,40,TFT_GREEN);
  tft.drawCircle(CENTER_X,CENTER_Y,75,TFT_GREEN);
  tft.drawCircle(CENTER_X,CENTER_Y,MAX_RADIUS,TFT_GREEN);
  sweepAngle+=0.04;
  if (sweepAngle>=6.28318){
    sweepAngle=0.0;
  }
  int newX = CENTER_X+(int)(MAX_RADIUS*cos(sweepAngle));
  int newY = CENTER_Y+(int)(MAX_RADIUS*sin(sweepAngle));
  tft.drawLine(CENTER_X,CENTER_Y,newX,newY,TFT_GREEN);
}
void drawAircraft(){
  int currentMaxKm = ranges[currentRangeIndex];
  for (int i  0;i<activeFlightCount;i++){
    if (!flights[i].active) continue;
    if (flights[i].distanceKm > currentMaxKm) continue;
    int pixelRadius = map(flights[i].distanceKm,0,currentMaxKm,0,MAX_RADIUS);
    int bx = CENTER_X +(int)(pixelRadius*cos(flights[i].angleRad));
    int by = CENTER_Y + (int)(pixelRadius*sin(flights[i].angleRad));
    tft.fillCircle(bx,by,3,TFT_WHITE);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(flights[i].callsign,bx+5,by-8,1);
    tft.drawString(flights[i].type,bx+5,by+2,1);
  }
}
void fetchLiveFlightData(){
  HTTPClient http;
  String url = "http://opensky-network.org/api/states/all?lamin=" + String(LAT_MIN) + "&lomin=" + String(LON_MIN) + "&lamax=" + String(LAT_MAX) + "&lomax=" + String(LON_MAX);
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK){
    String payload = http.getString();
    DynamicJsonDocument doc(8192);
    deserializeJson(doc,payload);
    JsonArray states = doc["states"];
    activeFlightCount = 0;
    for (JsonArray state:states){
      if (activeFlightCount >= MAX_FLIGHTS) break;
      const char* cs = state[1];
      float lon = state[5];
      float lat = state[6];
      if (cs&&lat != 0.0 && lon != 0.0) {
        strncpy(flights[activeFlightCount].callsign,cs,8);
        strcpy(flights[activeFlightCount].type,"A320");
        float dLat = (lat-HOME_LAT)*111.0;
        float dLon = (lon-HOME_LON)*111.0 *cos(HOME_LAT*0.0174533);
        flights[activeFlightCount].distanceKm = sqrt(dLat*dLat+dLon*dLon);
        flights[activeFlightCount].angleRad = atan2(dLat,dLon);
        flights[activeFlightCount].active = true;
        activeFlightCount++;
      }
    }
  }
  http.end();
}
void loadFallbackData(){
  activeFlightCount = 3;
  strcpy(flights[0].callsign,"AAL123");
  strcpy(flights[0].type,"A320");
  flights[0].distanceKm = 18;
  flights[0].angleRad = 0.8;
  flights[0].active = true;
  strcpy(flights[1].callsign,"DAL456");
  strcpy(flights[1].type,"B737");
  flights[1].distanceKm = 35;
  flights[1].angleRad = 2.4;
  flights[1].active = true;
  strcpy(flights[2].callsign,"BAW789");
  strcpy(flights[2].type,"DA40");
  flights[2].distanceKm = 42;
  flights[2].angleRad = 4.5;
  flights[2].active = true;
}