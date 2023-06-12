#define BLYNK_TEMPLATE_ID           "TMPL2kdKKmUiV"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "_14Hn9MOsalMu9I6S7eZZt59G2HY_VeC"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
//====library============
#include "HX711.h"

//=====variables===========
const int LOADCELL_DOUT_PIN = 27;
const int LOADCELL_SCK_PIN =  25;
#define FILTER_CONSTANT 0.5
const int led = 18;
int move_sensor = 32;
//======filter==========================================================================
double raw_value = 0, filtered_value = 0;
const int MA_WINDOW_SIZE = 5; // Set moving average window size 
int ma_window[MA_WINDOW_SIZE]; // Moving average window
int ma_index = 0; // Index of the oldest value in the moving average window
int ma_sum = 0; // Sum of the values in the moving average window
//=======================================================================================
float weight;
int sense;
//===========================
char ssid[] = "wifi name";
char pass[] = "wifi pasword";
//===========================
HX711 scale;
BlynkTimer timer;
//=============================================================
void notifyWeight() 
{  
  raw_value = scale.get_units() * 40;
  ma_sum -= ma_window[ma_index]; // Remove the oldest value from the sum
  ma_sum += raw_value; // Add the new value to the sum
  ma_window[ma_index] = raw_value; // Update the moving average window
  ma_index = (ma_index + 1) % MA_WINDOW_SIZE; // Update the index of the oldest value
  
  filtered_value = ma_sum / MA_WINDOW_SIZE;
  
  if (filtered_value < 20) 
  { Blynk.logEvent("final_notify_less","weight is less"); }
}
//============================================================
BLYNK_WRITE(V0)
{
  int value = param.asInt();
  digitalWrite(2, value);
}
//============================================================
void setup()
{

  pinMode(led, OUTPUT);
  pinMode(move_sensor, INPUT);
  Serial.begin(115200);

  //================================================
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-29664);
  scale.tare();   
  //================================================
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2500, notifyWeight);
}

void loop()
{
  Blynk.run();
  timer.run();


  
  raw_value = scale.get_units() * 40;
  ma_sum -= ma_window[ma_index]; 
  ma_sum += raw_value; 
  ma_window[ma_index] = raw_value; 
  ma_index = (ma_index + 1) % MA_WINDOW_SIZE; 
  
  filtered_value = ma_sum / MA_WINDOW_SIZE;

  //====================================
  sense = digitalRead(move_sensor);
  if (sense == 0) 
  {
    Serial.print("Reading:\t");
    Serial.println(filtered_value);
    
    if(filtered_value < 20)
      digitalWrite(led, HIGH);
    else
      digitalWrite(led, LOW);

    scale.power_down(); // sleep mode
    delay(200);
    scale.power_up();
  }
  else
  { 
    Serial.println("!!!STOP!!!");
    delay(2000); 
  }      
  //====================================
}
