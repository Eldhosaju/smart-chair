#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
char auth[] = "HRPXPHHDA0xFlVylIue8l0i6sg3901Z7";
char ssid[] = "rpi1";
char pass[] = "12345678";
float distance=5; 
int safe=25;
int danger=75;
int trigPin=14;             //Sensor Trip pin connected to nodemcu pin D5
int echoPin=13;             //Sensor Echo pin connected to nodemcu pin D7
int buzzer = 5;             //Connected to nodemcu pin D1
int myCounter=1;            //declare your variable myCounter and set to 0
float pingTime;             //time for ping to travel from sensor to target and return
float targetDistance;       //Distance to Target in meters
float speedOfSound=343;     //Speed of sound in meter per second.
int pdanger;                //percentage of bin filled
int s;
int state=0;
int button=16;              //Nodmcu pin 0
WidgetMap myMap(V1);



void setup()
{
  
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(button, HIGH);
  Blynk.begin(auth, ssid, pass);
  int index=1;
  float lat=10.031390;
  float lon=76.329870;
  myMap.location(index,lat,lon,"value");
}
 
void loop()
{
  Blynk.run();
  ebutton();
  danger_percentage();
  buzzer_tone();
}


void ebutton()
{
  s=digitalRead(button);
  if(s==0)
  {
    Blynk.notify("Help me");
    state=state+1;
    delay(1000);
  }
  else
  state=0;
  if(state==3)
  {
    Serial.println("Calling the number");
    state=0;
  }
}

  
void danger_percentage()
{
  digitalWrite(trigPin,LOW);              //Set trigPin low
  delayMicroseconds(2000);                //Let signal settle
  digitalWrite(trigPin,HIGH);             //Set trigger pin high
  delayMicroseconds(15);                  //Delay in high state
  digitalWrite(trigPin,LOW);              //Ping has now been send
  delayMicroseconds(10);                  //Delay in high state
  
  pingTime = pulseIn(echoPin, HIGH);      //pingTime is presented in microceconds
  pingTime=pingTime/1000000;              //convert pingTime to seconds by dividing by 1000000 (microseconds in a second)
  targetDistance=speedOfSound*pingTime;   //This will be in miles, since speed of sound was miles per hour
  targetDistance=targetDistance/2;        //Remember ping travels to target and back from target, so you must divide by 2 for actual target distance.
  targetDistance=targetDistance/100;      //Converting to centimeters.
  
  pdanger=100-((targetDistance/distance)*100);
  Serial.println(targetDistance);
  Serial.println(pdanger);
  Blynk.virtualWrite(V0,pdanger);
}

  
void buzzer_tone()
{
  if(pdanger<0)
  {
    pdanger=0;
  }
  if(pdanger>=danger)
  {
    tone(buzzer,100);    
    delay(1000);
    noTone(buzzer); 
  }
  else if(pdanger>=safe)
  {
    tone(buzzer,200);    
    delay(200);
    noTone(buzzer); 
  }
}

