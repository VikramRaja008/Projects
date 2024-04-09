#include <ros.h>
#include <geometry_msgs/Twist.h>

#define left_encoder_pinA 3
#define left_encoder_pinB 2

#define right_encoder_pinA 19
#define right_encoder_pinB 18

#define left_motor 10
#define right_motor 11

#define pwm_left 8
#define pwm_right 9

// SoftwareSerial BT1(2, 3); // Tx1, Rx1

ros::NodeHandle node;
geometry_msgs::Twist msg;

void pwm()
{
  analogWrite(pwm_left,100);
  delay(1);
  analogWrite(pwm_right,100);
  delay(1);
}


void fwd()
{

  pwm();
  
  digitalWrite(left_motor,LOW); // driection 0 - fwd 1- back
  digitalWrite(right_motor,LOW);
 
}

void right()
{

 
  pwm();
  digitalWrite(right_motor,LOW);
  digitalWrite(left_motor,HIGH);
  
}

void brk()
{

  analogWrite(pwm_left,0);
  analogWrite(pwm_right,0);
}

void left()
{

    pwm();
  digitalWrite(left_motor,LOW);
  digitalWrite(right_motor,HIGH);
  
}

void rev()
{

    pwm();
   digitalWrite(left_motor,HIGH);
  digitalWrite(right_motor,HIGH);
 
}
void lefto()
{

     analogWrite(pwm_left,90);
  delay(1);
  analogWrite(pwm_right,100);
  delay(1);
    digitalWrite(right_motor,HIGH);
  digitalWrite(left_motor,HIGH);
}
void righto()
{

      analogWrite(pwm_left,100);
  delay(1);
  analogWrite(pwm_right,90);
  delay(1);
   digitalWrite(left_motor,HIGH);
  digitalWrite(right_motor,HIGH);
 
}
void lefti()
{

      analogWrite(pwm_left,100);
  delay(1);
  analogWrite(pwm_right,90);
  delay(1);
    digitalWrite(right_motor,LOW);
  digitalWrite(left_motor,LOW);
 
}
void righti()
{

     analogWrite(pwm_left,90);
  delay(1);
  analogWrite(pwm_right,100);
  delay(1);
   digitalWrite(left_motor,LOW);
  digitalWrite(right_motor,LOW);
 
}
void roverCallBack(const geometry_msgs::Twist& cmd_vel)
{
  if(cmd_vel.linear.x > 0 && cmd_vel.angular.z > 0)
  {
    lefto(); //u
  }
  else 
  {
  if(cmd_vel.linear.x > 0 && cmd_vel.angular.z < 0)
  {
    righto(); //o
  }
  else
  {
   if(cmd_vel.linear.x < 0 && cmd_vel.angular.z > 0)
  {
    lefti(); //m
  }
  else
  {
  if(cmd_vel.linear.x <0 && cmd_vel.angular.z < 0)
  {
    righti(); ///
  }
  else
  {
  if(cmd_vel.linear.x > 0 && cmd_vel.angular.z == 0)
  {
    
    fwd(); //i
  }
  else
  {
    if(cmd_vel.linear.x == 0 && cmd_vel.angular.z > 0)
    {
      right(); //j
    }
    else
    {
      if(cmd_vel.linear.x == 0 && cmd_vel.angular.z == 0)
      {
        brk(); //k
      }
      else
      {
        if(cmd_vel.linear.x == 0 && cmd_vel.angular.z < 0)
        {
          left(); //l
        }
        else
        {
          if(cmd_vel.linear.x < 0 && cmd_vel.angular.z == 0)
          {
            rev(); //,
          }
          else
          {
            brk(); //default
          }
        }
      }
    }
  }
}
}
}
}
}
ros::Subscriber <geometry_msgs::Twist> sub("cmd_vel", roverCallBack);

void setup()
{

//  Serial.begin(57600);
//  BT1.begin(57600);
  
  node.initNode();
  node.subscribe(sub);
   pinMode(right_motor,OUTPUT); //motor control
 pinMode(left_motor,OUTPUT); 

 pinMode(pwm_left,OUTPUT); //pwm for motor
 pinMode(pwm_right,OUTPUT);
  
} 

void loop()
{


//  if(Serial.available())
//  {
//    BT1.print((char)Serial.read());
//  } 
//  if(BT1.available())
//  {
//    Serial.print((char)BT1.read());
//  }
  
  node.spinOnce(); 
  delay(1);
}
