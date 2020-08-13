#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define D1 4
#define D2 8

#define VR1 3
#define VR2 5

int SPEED = 0;
int line_follower_speed =20;
int fast = 4;
int medium = 2;
int slow = 4;
int Sensor1 = 0;
int Sensor2 = 0;
int Sensor3 = 0;
int Sensor4 = 0;

struct Signal {
byte throttle;      
byte pitch;
byte roll;
byte yaw;
byte aux1;
byte aux2;
};

Signal data;

const uint64_t pipeIn = 0xE9E8F0F0E1LL;
RF24 radio(9, 10); 

void ResetData()
{
// Define the inicial value of each data input. | Veri girişlerinin başlangıç değerleri
// The middle position for Potenciometers. (254/2=127) | Potansiyometreler için orta konum
data.roll = 127;   // Center | Merkez
data.pitch = 127;  // Center | Merkez
data.throttle = 12; // Motor Stop | Motor Kapalı
data.yaw = 127;   // Center | Merkez
data.aux1 = 255;   // Center | Merkez
data.aux2 = 0;   // Center | Merkez
digitalWrite(A4,LOW);
analogWrite(VR1, 0);
analogWrite(VR2, 0);
}

void setup()
{
  Serial.begin(9600);
  //Configure the NRF24 module
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(VR1,OUTPUT);
  pinMode(VR2,OUTPUT);
  
  pinMode(Sensor1, INPUT);
  pinMode(Sensor2, INPUT);
  pinMode(Sensor3, INPUT);
  pinMode(Sensor4, INPUT);
  
  ResetData();
  radio.begin();
  radio.openReadingPipe(1,pipeIn);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening(); //start the radio comunication for receiver | Alıcı olarak sinyal iletişimi başlatılıyo

}

unsigned long lastRecvTime = 0;

void recvData()
{
while ( radio.available() ) {
  digitalWrite(A4,HIGH);
radio.read(&data, sizeof(Signal));
lastRecvTime = millis();   // receive the data | data alınıyor
}
}

void loop()
{
recvData();
unsigned long now = millis();
if ( now - lastRecvTime > 1000 ) {
ResetData(); // Signal lost.. Reset data | Sinyal kayıpsa data resetleniyor
}
//Serial.print("throttle:");
//Serial.println(data.throttle);
//Serial.print("                  YAW:");
//Serial.println(data.yaw);
//Serial.print("                              PITCH:");
//Serial.println(data.pitch);
//Serial.print("                                               ROLL:");
//Serial.println(data.roll);
//Serial.print("                                                            AUX1:");
//Serial.println(data.aux1);
//Serial.print("                                                                         AUX2:");
//Serial.println(data.aux2);
if(data.aux1==255){
SPEED = map(data.aux2,0,255,0,255);

if(data.pitch>120)
{
  digitalWrite(D1,HIGH);
  digitalWrite(D2,LOW);
  analogWrite(VR1,SPEED);
  analogWrite(VR2,SPEED);
  Serial.println("forward");
}
else if(data.pitch<118)
{
  digitalWrite(D1,LOW);
  digitalWrite(D2,HIGH);
  analogWrite(VR1,SPEED);
  analogWrite(VR2,SPEED);
  Serial.println("backward");
}
else if (data.roll>133)
{
 digitalWrite(D1,HIGH);
  analogWrite(VR1,SPEED);
  Serial.println("right");
}
else if(data.roll<131)
{
  digitalWrite(D2,LOW);
  analogWrite(VR2,SPEED);
  Serial.println("left");
}
else if(data.yaw<128)
{
  digitalWrite(D1,HIGH);
  digitalWrite(D2,HIGH);
  analogWrite(VR1,SPEED);
  analogWrite(VR2,SPEED);
  Serial.println("cw");
}
else if(data.yaw>132)
{
digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);
  analogWrite(VR1,SPEED);
  analogWrite(VR2,SPEED);
  Serial.println("ccw");
}
else 
{
   analogWrite(VR1,0);
   analogWrite(VR2,0);
 Serial.println("stop");
}
}
else if(data.aux1==0)
{
  Sensor1 = digitalRead(A0);
  Sensor2 = digitalRead(A1);
  Sensor3 = digitalRead(A2);
  Sensor4 = digitalRead(A3);
// Serial.println(Sensor1);
// Serial.println(Sensor2);
// Serial.println(Sensor3);
// Serial.println(Sensor4);
if ((Sensor4 == 0 && Sensor3 == 1 && Sensor2 == 0 && Sensor1 == 0)||
    (Sensor4 == 0 && Sensor3 == 0 && Sensor2 == 1 && Sensor1 == 0)||
    (Sensor4 == 1 && Sensor3 == 0 && Sensor2 == 1 && Sensor1 == 0)||
    (Sensor4 == 0 && Sensor3 == 1 && Sensor2 == 1 && Sensor1 == 0)||
    (Sensor4 == 1 && Sensor3 == 0 && Sensor2 == 0 && Sensor1 == 1)||
    (Sensor4 == 0 && Sensor3 == 1 && Sensor2 == 0 && Sensor1 == 1)||
    (Sensor4 == 1 && Sensor3 == 1 && Sensor2 == 0 && Sensor1 == 1)||
    (Sensor4 == 1 && Sensor3 == 0 && Sensor2 == 1 && Sensor1 == 1)||
    (Sensor4 == 1 && Sensor3 == 1 && Sensor2 == 1 && Sensor1 == 1))
    {
      //forward
    digitalWrite(D1, HIGH);
    digitalWrite(D2, LOW);
    analogWrite(VR1, line_follower_speed);
    analogWrite(VR2, line_follower_speed);
    }
else if (Sensor4 == 1 && Sensor3 == 0 && Sensor2 == 0 && Sensor1 == 0)
{
  //turn fast left
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  analogWrite(VR2, line_follower_speed+fast);
  analogWrite(VR1, line_follower_speed+fast);
}

else if(Sensor4 == 1 && Sensor3 == 1 && Sensor2 == 0 && Sensor1 == 0)
        {
          //med left
       digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  analogWrite(VR2, line_follower_speed-medium);
  analogWrite(VR1, line_follower_speed-medium);
        }
        else if(Sensor4 == 1 && Sensor3 == 1 && Sensor2 == 1 && Sensor1 == 0)
         {
          //slow left
            digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  analogWrite(VR2, line_follower_speed-slow);
  analogWrite(VR1, line_follower_speed-slow);
        }
else if(Sensor4 == 0 && Sensor3 == 0 && Sensor2 == 0 && Sensor1 == 1)
{
 //turn fast right
   digitalWrite(D1, HIGH);
   digitalWrite(D2, HIGH);
   analogWrite(VR1, line_follower_speed+fast);
   analogWrite(VR2, line_follower_speed+fast);
}
else if(Sensor4 == 0 && Sensor3 == 0 && Sensor2 == 1 && Sensor1 == 1)
        {
          //med right
   digitalWrite(D1, HIGH);
 digitalWrite(D2, HIGH);
   analogWrite(VR1, line_follower_speed-medium);
   analogWrite(VR2, line_follower_speed-medium);
        }
else if (Sensor4 == 0 && Sensor3 == 1 && Sensor2 == 1 && Sensor1 == 1)
        {
          //slow right
   digitalWrite(D1, HIGH);
   digitalWrite(D2, HIGH);
   analogWrite(VR1, line_follower_speed-slow);
   analogWrite(VR2, line_follower_speed-slow);
        }

else
{
 //stop
    analogWrite(VR1, 0);
    analogWrite(VR2, 0); 
}

  
}
else
{
 //stop
    analogWrite(VR1, 0);
    analogWrite(VR2, 0); 
}
}
