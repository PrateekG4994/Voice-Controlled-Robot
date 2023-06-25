#include <ESP8266WiFi.h>
WiFiClient client;
WiFiServer server(80);
const char* ssid = "CATronix1234";
const char* password = "CATronix1234";
String  command =""; // Command received from Android device

// Set Motor Control Pins
int rightMotor2 = 5;    // D1 - right Motor -
int rightMotor1 = 4;    // D2 - right Motor +
int leftMotor2 = 13;    // D7 - left Motor - 
int leftMotor1 = 15;    // D8 - left Motor +
int trigger = 14;       //D5
int echo = 12;          //D6
int duration=0,distance=0;

void setup()
{
  Serial.begin(115200);
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(leftMotor1, OUTPUT); 
  pinMode(leftMotor2, OUTPUT);  
  pinMode(rightMotor1, OUTPUT);  
  pinMode(rightMotor2, OUTPUT);  

  digitalWrite(leftMotor1,LOW);
  digitalWrite(leftMotor2,LOW);
  digitalWrite(rightMotor1,LOW);
  digitalWrite(rightMotor2,LOW);
      
  connectWiFi();
  server.begin();
}

void loop()
{
 digitalWrite(trigger,LOW);
 delayMicroseconds(2);
 digitalWrite(trigger,HIGH);
 delayMicroseconds(10);
 digitalWrite(trigger,LOW);
 delayMicroseconds(2);
 duration=pulseIn(echo,HIGH);
 distance=duration*340/20000;
 Serial.println(distance);
 if(distance<=20) stopMotor();
 delay(50);
    client = server.available();
    if (!client) return;
    command = checkClient ();
    Serial.println(command);
         if (command == "forward" && distance >20)  forwardMotor();
    else if (command == "backward" ) reverseMotor();
    else if (command == "left") leftMotor();    
    else if (command == "right") rightMotor();     
    else if (command == "stop")     stopMotor();     
    else if(distance<=20) stopMotor();
     delay(100);
 
    sendBackEcho(command); // send command echo back to android device
    command = "";   
} 

/* command motor forward */
void forwardMotor(void)   
{
  digitalWrite(leftMotor1,HIGH);
  digitalWrite(leftMotor2,LOW);
  digitalWrite(rightMotor1,HIGH);
  digitalWrite(rightMotor2,LOW);
}

/* command motor backward */
void reverseMotor(void)   
{

  digitalWrite(leftMotor1,LOW);
  digitalWrite(leftMotor2,HIGH);
  digitalWrite(rightMotor1,LOW);
  digitalWrite(rightMotor2,HIGH);
}

/* command motor turn left */
void leftMotor(void)   
{
  digitalWrite(leftMotor1,HIGH);
  digitalWrite(leftMotor2,LOW);
  digitalWrite(rightMotor1,LOW);
  digitalWrite(rightMotor2,HIGH);
}

/* command motor turn right */
void rightMotor(void)   
{
  digitalWrite(leftMotor1,LOW);
  digitalWrite(leftMotor2,HIGH);
  digitalWrite(rightMotor1,HIGH);
  digitalWrite(rightMotor2,LOW);
}

/* command motor stop */
void stopMotor(void)   
{

  digitalWrite(leftMotor1,LOW);
  digitalWrite(leftMotor2,LOW);
  digitalWrite(rightMotor1,LOW);
  digitalWrite(rightMotor2,LOW);
}

/* connecting WiFi */
void connectWiFi()
{
  Serial.println("Connecting to WIFI");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(300);
    Serial.print("..");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("NodeMCU Local IP is : ");
  Serial.print((WiFi.localIP()));
}

/* check command received from Android Device */
String checkClient (void)
{
  while(!client.available()) delay(1); 
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}

/* send command echo back to android device */
void sendBackEcho(String echo)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println(echo);
  client.println("</html>");
  client.stop();
  delay(1);
}
