#include <esp_now.h>
#include <WiFi.h>
#define speedAxisX 32
#define steerAxisY 33


// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t receiverMacAddress[] = {0x08,0xA6,0xF7,0xBC,0x23,0xFC};  
struct PacketData 
{
  int speed;
  int  steer; 
};
PacketData data;


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

void setup() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else
  {
    Serial.println("Succes: Initialized ESP-NOW");
  }
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  else
  {
    Serial.println("Succes: Added peer");
  } 
  pinMode(speedAxisX,INPUT);
  pinMode(steerAxisY,INPUT);
}
 
void loop() 
{
 data.speed=analogRead(speedAxisX);//read Xaxis value as speed 
 data.steer=analogRead(steerAxisY);//read Yaxis value as steer 
 Serial.print(data.speed);
 Serial.print(";;;");
 Serial.println(data.steer);
 esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &data, sizeof(data));
 if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
 else 
  {
    Serial.println("Error sending the data");
  }    
  
  delay(50);
}



















    