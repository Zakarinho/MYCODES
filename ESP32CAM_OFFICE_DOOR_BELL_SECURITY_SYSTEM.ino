
#define BLYNK_TEMPLATE_ID "TMPL22JCUhJn5"
#define BLYNK_DEVICE_NAME "IOT OFFICE SECURITY AND ALERTING ESP32 CAM"
#define BLYNK_AUTH_TOKEN "RWazgVoFNk7g601mQWxVIH00-JK1AbIm"

#include "esp_camera.h"
#include <WiFi.h>
//#include <SimpleTimer.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial
BlynkTimer timer;
// Select camera model
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

bool eventTrigger;
#define GreenLed 14 // not used yet
#define RedLed 2 // not used yet

#define anything2 16 // not used yet
#define anything4 0 // not used yet also this will be used to ground when programming the esp32 cam

// used pins for esp32 cam
#define LED 4 // USING FOR PICTURE TAKING LED
#define MOTION_SENSOR 12 // USING FOR LOCK ON AND OFF
#define FLAME_SENSOR 15 // USING FOR DOOR BELL PUSH BUTTON
#define BELL 13 // USING FOR BUZZER SOUND
#define VPIN_BUTTON_1    V2



int FlameData;
int MotionData;
bool pinValue = 0;
const char* ssid = "Mr. impossible ";       //wifi name
const char* password = "abdullahi02";       //password
char auth[] = BLYNK_AUTH_TOKEN;          //sent by Blynk
String local_IP;
int count = 0;
void startCameraServer();

void takePhoto()
{
  digitalWrite(LED, HIGH);
  delay(200);
  uint32_t randomNum = random(50000);
  Serial.println("http://"+local_IP+"/capture?_cb="+ (String)randomNum);
  Blynk.setProperty(V1, "urls", "http://"+local_IP+"/capture?_cb="+(String)randomNum);
  digitalWrite(LED, LOW);
  delay(1000);
}
BLYNK_WRITE(V0) {
  pinValue = param.asInt();
}
void setup() {
  Serial.begin(115200);
  
  // used pins and input and output sections
  pinMode(MOTION_SENSOR,INPUT);
  pinMode(FLAME_SENSOR,INPUT);
  pinMode(GreenLed,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(BELL,OUTPUT);
  pinMode(RedLed,OUTPUT);
//  pinMode(IN_BUTTON,INPUT);

  // start low 
  digitalWrite(BELL,LOW);
  digitalWrite(RedLed,LOW);
  timer.setInterval(10L, Office_Control);
  Serial.setDebugOutput(true);
  Serial.println();
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  local_IP = WiFi.localIP().toString();
  Serial.println("' to connect");
  Blynk.begin(auth, ssid, password);
}
// the system funtion of working
void Office_Control() {
  digitalWrite(GreenLed, HIGH);
  Blynk.virtualWrite(V3, HIGH); 

  FlameData = digitalRead(FLAME_SENSOR);
  MotionData = digitalRead(MOTION_SENSOR);
  int FlameReal = map(FlameData, 0, 1, 1, 0);
  
  if (MotionData==1){
    Blynk.virtualWrite(V5, MotionData);  
      digitalWrite(BELL, HIGH);
      digitalWrite(RedLed, HIGH);
      delay(5000);
      digitalWrite(BELL, LOW); 
      digitalWrite(RedLed, LOW);
      Blynk.email("setsom0f0@gmail.com", "Motion_alert", "Qof ayaa Officekaga yimid, Mataqanaa!");
      Blynk.logEvent("Motion_alert","Qof ayaa Officekaga yimid, Mataqanaa!"); 
      takePhoto();
          
  }
     if (FlameReal==1){
      Blynk.virtualWrite(V4, FlameReal); 
      digitalWrite(RedLed, HIGH);
      digitalWrite(BELL, HIGH);
      delay(5000);
      digitalWrite(BELL, LOW); 
      digitalWrite(RedLed, LOW);
      Blynk.email("setsom0f0@gmail.com", "Flame_alert", "Dab ayaa ka,kacay Officekaga, Badbaadi!");
      Blynk.logEvent("Flame_alert","Dab ayaa ka,kacay Officekaga, Badbaadi!"); 
      takePhoto();  
           
  }
  else {
    digitalWrite(BELL, LOW);
    digitalWrite(RedLed, LOW);
    Blynk.virtualWrite(V5, MotionData); 
    Blynk.virtualWrite(V4, FlameReal);   
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run();
  //Blynk.virtualWrite(V3, HIGH); 
 // Blynk.virtualWrite(V1, 0); // show off image
 // Blynk.virtualWrite(V1, 1); // show on image
 
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  int toggleState_1 = param.asInt();
  if(toggleState_1 == 1){
    takePhoto();
  }
  }
