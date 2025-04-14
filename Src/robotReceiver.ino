#include <esp_now.h>
#include <WiFi.h>
#define speedPin 26
#define steerPin 25
//robotReceiver.ino
#define acc 20
#define accDelay 100
#define maxspeed 80 // max value frome the center (between 0 and 126)
#define deadband 10
#define Max 230
#define SIGNAL_TIMEOUT 1000  // This is signal timeout in milli seconds. We will reset the data if no signal


int speedMap;
int dirMap ;
int previousTime2;
int cmdD;
int cmdT;
int cmdDV;
int cmdTV;
unsigned long lastRecvTime = 0;

struct PacketData
{
   int speed;
   int  steer; 
};

PacketData receiverData;

// callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) 
{
  if (len == 0)
  {
    return;
  }
  memcpy(&receiverData, incomingData, sizeof(receiverData));
  char inputValuesString[100];
  sprintf(inputValuesString, "%3d,%3d",receiverData.speed,receiverData.steer);// print receiverData
  Serial.println(inputValuesString); 
  lastRecvTime = millis(); 
}

void setup() 
{
  Serial.begin(115200);
  pinMode(speedPin,OUTPUT);
  pinMode(steerPin,OUTPUT);
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  previousTime2=millis();
}
 


void loop()
{ 
  cmdDV=map( receiverData.speed,0,4095,126-maxspeed,126+maxspeed);
  cmdTV=map(receiverData.steer,0,4095,126-maxspeed,126+maxspeed);
  if (cmdDV<(126+deadband) && cmdDV>(126-deadband)) cmdDV=126;
  if (cmdTV<(126+deadband) && cmdTV>(126-deadband)) cmdTV=126;
  cmdDV=constrain(cmdDV,0,Max);
  cmdTV=constrain(cmdTV,0,Max);
  if( (millis() - previousTime2) > accDelay)
  {
   previousTime2 = millis();
   if( cmdD > cmdDV )
   {
    cmdD -= acc;
    cmdD = max(cmdDV, cmdD) ;
   }
   if( cmdD < cmdDV )
   {
    cmdD += acc;
    cmdD = min(cmdDV, cmdD) ;
   }   
   if( cmdT > cmdTV )
   {
    cmdT -= acc;
    cmdT = max(cmdTV, cmdT) ;
   }
   if( cmdT < cmdTV )
   {
    cmdT += acc;
    cmdT = min(cmdTV, cmdT) ;
   }      
  }
 dacWrite(speedPin, cmdD);
 dacWrite(steerPin, cmdT);
 Serial.print(cmdD);
 Serial.print("     ;;;   ");
 Serial.println(cmdT);
 //Check Signal lost.
 unsigned long now = millis();
 if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    Serial.println("No Signal");  
  }
  
  
}















    