#include <ModbusRtu.h>
#include <VarSpeedServo.h>

#define  f1 3
#define  f2 4
#define  r1 5
#define  r2 6
#define en1 30
#define en2 31
//decalarations

//===========================speed servo=====
VarSpeedServo b;
VarSpeedServo s;
VarSpeedServo j1;
VarSpeedServo j2;
VarSpeedServo j3;
VarSpeedServo j4;
//===========================================



//============================modbus=======
Modbus modbus_port;
uint16_t modbus_array[] = {0,0,0};
 int outdata;
 String rdata;
 char indata;
//==========================================


//==============climbing declarations===========
int ir;
int i;
int count;
int IRSensor = 7; 
//===============================================

 
void setup() 
{
  modbus_port = Modbus(1, 0, 2);//(slave id,rx tx,re de enable)
  modbus_port.begin(9600);

 b.attach(12);
s.attach(11);
j1.attach(10);
j2.attach(9);
j4.attach(8);
b.write(90,12,true);
delay(500);
s.write(140,12,true);
delay(500);
j1.write(180,12,true);
delay(500);
j2.write(90,12,true);
delay(500);
j4.write(145,12,true);
delay(500);


 pinMode(f1,OUTPUT);
   pinMode(f2,OUTPUT);
    pinMode(r1,OUTPUT);
     pinMode(r2,OUTPUT);
      pinMode(en1,OUTPUT);
       pinMode(en2,OUTPUT);
       pinMode (IRSensor, INPUT);


  
analogWrite(en1,255);
analogWrite(en2,255);

Serial1.begin(9600);
}


//===================================================================================ARM ACTIONS
void defaulpos()
{
 b.write(90,12,true);
delay(500);
s.write(140,12,true);
delay(500);
j1.write(180,12,true);
delay(500);
j2.write(90,12,true);
delay(500);
j4.write(180,12,true);
delay(500);
}

void pickposr()
{
  b.write(145,12,true);//d
delay(500);
j4.write(180,12,true);//e
delay(500);
j2.write(90,12,true);//need a
delay(500);
j1.write(130,12,true);// b
delay(500);
s.write(100,12,true);//c
delay(500);
j2.write(90,12,true);//need a
delay(500);
j4.write(0,12,true);//need e
delay(500);
}

void pickposl()
{
   b.write(45,12,true);
delay(500);
j4.write(180,12,true);
delay(500);
j2.write(180,12,true);//need a
delay(500);
j1.write(130,12,true);
delay(500);
s.write(100,12,true);
delay(500);
j2.write(180,12,true);//need a
delay(500);
j4.write(0,12,true);//need
delay(500);
}


void droppos()
{
  j4.write(0,12,true);//need
delay(500);
s.write(140,12,true);
delay(500);
j1.write(180,12,true);
delay(500);
b.write(0,12,true);
delay(500);
j1.write(130,12,true);// b
delay(500);
s.write(100,12,true);//c
delay(500);
 j4.write(180,12,true);//need
delay(500);
}
//======================================================================================ARM ACTIONS END


//=====================================================================================CLIMBING ACTIONS
//=================================================================
// attaching to rack
void attach1()
{
  Serial.println("\n attach");
digitalWrite(f1,HIGH);
delay(4000);
}
//================================================================


//=================================================================
// deattaching to rack
void deattach1()
{
   Serial.println("\n deattach");
digitalWrite(r1,HIGH);
delay(4000);
}
//====================================================================

//=================================================================
void climb()
{
  analogWrite(en2,255);
  digitalWrite(f2,HIGH);
   digitalWrite(r2,LOW);
}
//=============================================================

//=============================================================
void dcnd()
{
  analogWrite(en2,40);
  digitalWrite(f2,LOW);
   digitalWrite(r2,HIGH);
}
//================================================================

//===============================================================
void declimb()
{
    digitalWrite(f2,LOW);
}

//==============================================================

///=============================base return
void base()
{
  while(count!=3)
  {
    delay(500);
    ir=digitalRead(IRSensor);
    declimb();
    Serial.println("declimbing");
    Serial.println(count);
    dcnd();
    if(ir==0)
    {
      count++;
    }
  }
  delay(1000);
}
//===========================base return

//==============================rack1 climb
void rack1()
{
 while(count!=2)
  {
    delay(500);
    ir=digitalRead(IRSensor);
    climb();
     Serial.println("climbing");
     Serial.println(ir);
    if(ir==0)
    {
      count++;
    }
  }
  delay(1000);
}
//================================rack1 climb

//==================================rack2 climb
void rack2()
{
 while(count!=3)
  {
    delay(500);
    ir=digitalRead(IRSensor);
    climb();
     Serial.println("climbing");
     Serial.println(ir);
    Serial.println(count);
    if(ir==0)
    {
      count++;
    }
  }
  delay(1000);
}
//==================================rack2 climb 


//==========================================CLIMBING END



void loop() 
{
   if (Serial1.available()) 
{
  rdata = Serial1.read();
 while(i!=1)
  {
    Serial.println(ir);
    attach1();
    delay(1000);
    climb();
char rack=rdata.charAt(0);

  switch(rack)
{
  
  //if rack 1 is detected
  case 'f':
  {
  rack1();
  if(rdata.substring(1)=='l')
  {
    pickposl();
    droppos();
    defaulpos();
  }
    if(rdata.substring(1)=='r')
  {
    pickposr();
    droppos();
    defaulpos();
  }
  count=0;
  base();
  deattach1();
  i=1;
  break;
  }
  //if rack 2 is detected 
  case 's':
  {
rack2();
 if(rdata.substring(1)=='l')
  {
    Serial.println("arm action");
    pickposl();
    droppos();
    defaulpos();
  }
    if(rdata.substring(1)=='r')
  {
    pickposr();
    droppos();
    defaulpos();
  }
 count=0;
  base();
 deattach1();
  i=1;
  break;
  }

}
  }
  
}  
}
