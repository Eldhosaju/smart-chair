#include <Wire.h>   //Library for I2C communication

int lwf=2;  //left wheel forward
int lwb=3;  //left wheel backward+++
int rwf=4;  //Right whell forward
int rwb=5;  //Right wheel backward
int speed;  //Speed control.
int jx=A0;  //X of Joystick
int jy=A1;  //Y of Joystick
int selection=6;    //To select whether acclerometer or joystick

int s;       //Seclection variable
int ADXL345=0x53;   // The ADXL345 sensor I2C address         connect Acclerometer SDA-> A4
float Xa, Ya, Za;   // x, y and z axis of accilarometer       connect Acclerometer SCL-> A5 

float Adistance=30;                //Allowed distance in cm
int safe=25;
int danger=75;
const unsigned int TRIG_PIN=8;   //D8
const unsigned int ECHO_PIN=7;   //D7
int buzzer = 9;                   //Connected to arduino pin D9
int pdanger;                      //percentage of bin filled

void setup() {
  Serial.begin(9600);
  pinMode(lwf,OUTPUT);
  pinMode(lwb,OUTPUT);
  pinMode(rwf,OUTPUT);
  pinMode(rwb,OUTPUT);
  pinMode(speed,OUTPUT);
  pinMode(jx,INPUT);
  pinMode(jy,INPUT);
  pinMode(selection,INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

   pinMode(buzzer, OUTPUT);
  
  Wire.begin();
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D);
  Wire.write(8);  
  Wire.endTransmission();
  delay(10);
  // put your setup code here, to run once:

}

void loop() {
  danger_percentage();
  buzzer_tone();
  s=digitalRead(selection);
  if(s==1)
  {
    acclerometer();
  }
  else if(s==0)
  {
    joystick();
  }



  
  
  
 
  // put your main code here, to run repeatedly:

}


void forward()
{
  digitalWrite(lwf,HIGH);
  digitalWrite(lwb,LOW);
  digitalWrite(rwf,HIGH);
  digitalWrite(rwb,LOW);
  Serial.println("Forward");
}
void backward()
{
  digitalWrite(lwf,LOW);
  digitalWrite(lwb,HIGH);
  digitalWrite(rwf,LOW);
  digitalWrite(rwb,HIGH);
  Serial.println("Backward");
}
void left()
{
  digitalWrite(lwf,LOW);
  digitalWrite(lwb,LOW);
  digitalWrite(rwf,HIGH);
  digitalWrite(rwb,LOW);
  Serial.println("Left");
  
}
void right()
{
  digitalWrite(lwf,HIGH);
  digitalWrite(lwb,LOW);
  digitalWrite(rwf,LOW);
  digitalWrite(rwb,LOW);
  Serial.println("Right");
}
void halt()
{
  digitalWrite(lwf,LOW);
  digitalWrite(lwb,LOW);
  digitalWrite(rwf,LOW);
  digitalWrite(rwb,LOW);
  Serial.println("Halt");
}
void joystick()
{
  Serial.println("joystick mode on");
  int y=analogRead(jy);
  int x=analogRead(jx);
  if(x>1000 && y>500)
    forward();
  else if(x<10 && y>500)
    backward();
  else if(y>1000 && x>500)
    right();
  else if(y<10 && x>500)
    left();
  else
    halt();
    Serial.println(x);
    Serial.println(y);
    
}
void acclerometer()
{
  Serial.println("acclerometer mode on");
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  Xa = ( Wire.read()| Wire.read() << 8); // X-axis value
  Xa = Xa/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Ya = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Ya = Ya/256;
  Za = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Za = Za/256;
  if(Xa>-.3)
  left();
  else if(Xa<-.8)
  right();
  else if(Ya>0.2)
  forward();
  else if(Ya<-1)
  backward();
  else
  halt();
}

void danger_percentage()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  const unsigned long duration= pulseIn(ECHO_PIN, HIGH);
  int distance= duration/29/2;
  pdanger=100-((distance/Adistance)*100);
  
  if(duration==0)
  {
    Serial.println("Warning: no pulse from sensor");
  } 
  else
  {
    Serial.print("distance to nearest object:");
    Serial.println(distance);
    Serial.println(" cm");
    if(pdanger<0)
    {
      pdanger=0;
    }
    Serial.println(pdanger);
  }
}

void buzzer_tone()
{
  if(pdanger>=danger)
  {
    tone(buzzer,250);    
    delay(500);
    noTone(buzzer); 
    delay(250);
  }
  else if(pdanger>=safe)
  {
    tone(buzzer,250);    
    delay(1000);
    noTone(buzzer); 
    delay(500);
  }
}
