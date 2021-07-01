#include <SPI.h>
#include <DMD2.h>
#include <fonts/MyBigFont.h>
#include <string.h>
//code rearranged by Javier Muñoz 10/11/2016 ask me at javimusama@hotmail.com
#include <SoftwareSerial.h>

#define ARDUINO_RX 3//should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 10//connect to RX of the module
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);//init the serial protocol, tell to myserial wich pins are TX and RX
int a,b,c,d,e;
int buzzer = 12;
int cmd_mode = 2;
int hc_pwr = 4;
byte new_data_flag = true;
String myString = "";
String thisString = "000";

////////////////////////////////////////////////////////////////////////////////////
//all the commands needed in the datasheet(http://geekmatic.in.ua/pdf/Catalex_MP3_board.pdf)
static int8_t Send_buf[8] = {0} ;//The MP3 player undestands orders in a 8 int string
                                 //0X7E FF 06 command 00 00 00 EF;(if command =01 next song order) 
#define NEXT_SONG 0X01 
#define PREV_SONG 0X02 

#define CMD_PLAY_W_INDEX 0X03 //DATA IS REQUIRED (number of song)

#define VOLUME_UP_ONE 0X04
#define VOLUME_DOWN_ONE 0X05
#define CMD_SET_VOLUME 0X06//DATA IS REQUIRED (number of volume from 0 up to 30(0x1E))
#define SET_DAC 0X17
#define CMD_PLAY_WITHVOLUME 0X22 //data is needed  0x7E 06 22 00 xx yy EF;(xx volume)(yy number of song)

#define CMD_SEL_DEV 0X09 //SELECT STORAGE DEVICE, DATA IS REQUIRED
#define DEV_TF 0X02 //HELLO,IM THE DATA REQUIRED
                
#define SLEEP_MODE_START 0X0A
#define SLEEP_MODE_WAKEUP 0X0B

#define CMD_RESET 0X0C//CHIP RESET
#define CMD_PLAY 0X0D //RESUME PLAYBACK
#define CMD_PAUSE 0X0E //PLAYBACK IS PAUSED

#define CMD_PLAY_WITHFOLDER 0X0F//DATA IS NEEDED, 0x7E 06 0F 00 01 02 EF;(play the song with the directory \01\002xxxxxx.mp3

#define STOP_PLAY 0X16

#define PLAY_FOLDER 0X17// data is needed 0x7E 06 17 00 01 XX EF;(play the 01 folder)(value xx we dont care)

#define SET_CYCLEPLAY 0X19//data is needed 00 start; 01 close

#define SET_DAC 0X17//data is needed 00 start DAC OUTPUT;01 DAC no output
////////////////////////////////////////////////////////////////////////////////////

int  recording,data;

SoftDMD dmd(1,1);  // DMD controls the entire display
DMD_TextBox box(dmd, 0, 0,34,16);  // "box" provides a text box to automatically write to/scroll the display

void setup() {

  // initialize serial:
  Serial.begin(9600);
  Serial.setTimeout(250);
  pinMode(buzzer,OUTPUT);
  pinMode(cmd_mode,OUTPUT);
  digitalWrite(cmd_mode,HIGH);
  pinMode(hc_pwr,OUTPUT);
  digitalWrite(hc_pwr,LOW);
  dmd.setBrightness(255);
  dmd.selectFont(MyBigFont);
  dmd.begin();
  mySerial.begin(9600);//Start our Serial coms for THE MP3
delay(500);//Wait chip initialization is complete
   sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card  
delay(200);//wait for 200ms
d=0;
e=0;
}

void loop(){

  if(new_data_flag){
    new_data_flag = false;
    thisString = String(e);
  Serial.println(thisString);
    if(e < 100){
      thisString = ("0"+thisString);
      if(e < 10){
        thisString = ("0"+thisString);
         }
      }
      
    sendCommand(CMD_PLAY_WITHVOLUME, 0xE10B);
      
    digitalWrite(buzzer,HIGH);
    dmd.drawFilledBox(0,0,32,16);
    delay_time();
    dmd.clearScreen();
    delay_time();
    digitalWrite(buzzer,LOW);
    box.clear();
    box.print(thisString);  
    voice();
  }
  
  delay(100);
}




void delay_time(){
  unsigned counter=0;
    while (counter < 200 && new_data_flag == false){
      if (Serial.available()){
         //Serial.println("HY");
        serialEvent();
        }
    counter++;
    delay(5);
   }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
       delay(10);
       if (Serial.read() == 'T'){
           myString = "";
             myString = Serial.readString();// read the incoming data as string
             d = myString.toInt();
             if(e != d){
              e = d;
               new_data_flag = true;
              }
            // Serial.println(d);
       }
    }
}

void sendCommand(int8_t command, int16_t dat){
 Send_buf[0] = 0x7e; //starting byte
 Send_buf[1] = 0xff; //version
 Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
 Send_buf[3] = command; //
 Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
 Send_buf[5] = (int8_t)(dat >> 8);//datah
 Send_buf[6] = (int8_t)(dat); //datal
 Send_buf[7] = 0xef; //ending byte
 for(uint8_t i=0; i<8; i++)//
 {
   mySerial.write(Send_buf[i]) ;//send bit to serial mp3
   //Serial.print(Send_buf[i],HEX);//send bit to serial monitor in pc
 }
}

void voice(){
 // Serial.println("voice start");
  if(e < 10){
    recording = 0xE101 + e;
   sendCommand(CMD_PLAY_WITHVOLUME, recording);
    delay_time();
  }
   else if(e < 100){
    recording = e/10;
     data = 0XE101 + recording; 
     sendCommand(CMD_PLAY_WITHVOLUME, data);
     delay_time();
    recording = e%10;
   data = 0XE101 + recording;
   sendCommand(CMD_PLAY_WITHVOLUME, data);
    delay_time();
  }
   else if(e < 1000){
    recording = e/100;
     data = 0XE101 + recording; 
     sendCommand(CMD_PLAY_WITHVOLUME, data);
     delay_time();
    recording = e%100;
    recording = recording/10;
   data = 0XE101 + recording;
   sendCommand(CMD_PLAY_WITHVOLUME, data);
     delay_time();
     recording = e%10;
   data = 0XE101 + recording;
   sendCommand(CMD_PLAY_WITHVOLUME, data);//play the first song with volume 15 class
    delay_time();
  }
  else {
   // Serial.println("invalid value");
    }
    //Serial.println("voice end");
}
