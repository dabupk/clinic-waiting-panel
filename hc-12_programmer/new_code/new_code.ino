#include <SoftwareSerial.h>
#include <FastLED.h>

CRGB leds[1];
int hc_set=7;
int led_gnd =10;
#define led_data 11
int led_vcc =12;
int dip_pin1 = A0;
int dip_pin2 = A1;
int dip_pin3 = A2;
int dip_pin4 = A3;
int dip_value1;
int dip_value2;
int dip_value3;
int dip_value4;
int data_address;
String command_address;
String inputString = ""; 
SoftwareSerial HC12(6, 5); // RX, TX
bool flag = true;
const byte interruptPin = 2;
int interval =1000;
int button_state = false;

void setup() {   
  //setting led power                  
 pinMode(led_gnd,OUTPUT); 
 pinMode(led_vcc,OUTPUT);
 pinMode(hc_set,OUTPUT);
 pinMode(dip_pin1,INPUT_PULLUP);
 pinMode(dip_pin2,INPUT_PULLUP);
 pinMode(dip_pin3,INPUT_PULLUP);
 pinMode(dip_pin4,INPUT_PULLUP);
 digitalWrite(led_vcc,HIGH);
 digitalWrite(led_gnd,LOW);
 digitalWrite(hc_set,LOW);
 Serial.begin(9600);
 HC12.begin(9600);
 pinMode(interruptPin, INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);
 FastLED.addLeds<NEOPIXEL, led_data>(leds, 1);
                   
}

void loop() {
dip_value1 = digitalRead(dip_pin1);
dip_value2 = digitalRead(dip_pin2);
dip_value3 = digitalRead(dip_pin3);
dip_value4 = digitalRead(dip_pin4);

data_address = dip_value1 + (dip_value2 << 1) + (dip_value3 << 2) + (dip_value4 << 3) + 1;
if(data_address < 10){
  command_address = "AT+C00" + String(data_address);
  }
  else {
command_address = "AT+C0" + String(data_address);
  }

Serial.println(command_address);

if(button_state == true){
    button_state = false;
    inputString = "";
    
    leds[0] = CRGB::Blue;
     FastLED.show();
     HC12.write("AT"); 
    delay(interval);

    HC12.write("AT"); 
 delay(interval);

    if(HC12.available()) // Check if received
    {
        if(HC12.find("OK"))// test the received buffer for "on"
        {
            Serial.println("OK is done");
        }
        else{
          flag = false;
          }
      
    }
 
   HC12.write("AT+DEFAULT"); 
 delay(interval);

    if(HC12.available()) // Check if received
    {
        if(HC12.find("OK+DEFAULT")) // test the received buffer for "on"
        {
            Serial.println("DEFAULT is done");
        }
        else{
          flag = false;
          }
      
    }
  
   HC12.print(command_address);
 delay(interval);

    if(HC12.available()) // Check if received
    {

        if(HC12.find("OK+C"))// test the received buffer for "on"
        {
            Serial.println("COMMAND is done");
        }
        else{
          flag = false;
          }
      
    }
  
   HC12.write("AT+B9600"); 
 delay(interval);

    if(HC12.available()) // Check if received
    {
        if(HC12.find("OK+B9600"))// test the received buffer for "on"
        {
            Serial.println("BUAD is done");
        }
        else{
          flag = false;
          }
      
    }
  
   HC12.write("AT+FU3"); 
 delay(interval);

    if(HC12.available()) // Check if received
    {
        if(HC12.find("OK+FU3"))// test the received buffer for "on"
        {
            Serial.println("FU is done");
        }
        else{
          flag = false;
          }
      
    }
  
     HC12.write("AT+P8"); 
 delay(interval);

    if(HC12.available()) // Check if received
    {
        if(HC12.find("OK+P8"))// test the received buffer for "on"
        {
            Serial.println("POWER is done");
        }
        else{
          flag = false;
          }
      
    }
        }

  if(flag == false){
    flag = true;
    leds[0] = CRGB::Red;
     FastLED.show();
     delay(5000);
    }
  leds[0] = CRGB::Green;
     FastLED.show();
     delay(interval);
     leds[0] = CRGB::Black;
     FastLED.show();
     delay(interval);      
}

void blink(){
     button_state = true;
}
