#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h> 
#include <string>

Servo servo1;
Servo servo2;
//SSID and Password to your ESP Access Point
const char* ssid = "Home_Security";
const char* password = "12345678";
const int LED1 = 16;
const int LED2 = 5;
const int LED3 = 4;
const int Trig = 0;
const int Echo = 2;
const int IR_Pin = 14;
const int servo1_pin = 12;
const int servo2_pin = 13;
const int speakerPin = 15;
ESP8266WebServer server(80); //Server on port 80
IPAddress    apIP(42, 42, 42, 42);
String Status = "";
String webSite = "";
String webSite_usresult = "";
String usreading = "";
int IR_State;
int length = 28; // the number of notes
char notes1[] = "GGAGcB GGAGdc GGxecBA yyecdc";
int beats1[] = { 2, 2, 8, 8, 8, 16, 1, 2, 2, 8, 8,8, 16, 1, 2,2,8,8,8,8,16, 1,2,2,8,8,8,16 };
char notes2[] = "dddddd ddddddd dddddd dddddd";
int beats2[] = { 2, 2, 8, 8, 8, 16, 1, 2, 2, 8, 8,8, 16, 1, 2,2,8,8,8,8,16, 1,2,2,8,8,8,16 };
int tempo = 150;

//     This routine is executed when you open its IP in browser
//==============================================================
void handleRoot() {
  webSite= "";
  webSite+="<body id='BackgroundColor' style='background-color:rgb(135, 156, 201)'> </body>";
  webSite+="<p><button>HOME</button></p>";
  webSite+="<h1>Home Security Device Control Panel</h1>";
  webSite+="<font id='FontColor' color='Green'><h2 id='Status'>Status: ";
  webSite+="Normal Mode";
  webSite+="</h2></font>";
  webSite += "<p>Normal Mode <a href=\"socket1On\"><button>Enable</button></a></p>";
  webSite += "<p>Security Mode <a href=\"socket2On\"><button>Enable</button></a></p>";
  webSite += "<p>Birthday Mode <a href=\"socket3On\"><button>Enable</button></a></p>";
  webSite += "<p>For Refill Paint Only <a href=\"socket4On\"><button>Lock1</button></a></p>";
  webSite += "<p>For Refill Ribbon Only <a href=\"socket5On\"><button>Lock2</button></a></p>";
  server.send(200, "text/html",webSite);
  digitalWrite(LED1,HIGH);
}


void playTone(int tone, int duration) {
for (long i = 0; i < duration * 1000L; i += tone * 2) {
   digitalWrite(speakerPin, HIGH);
   delayMicroseconds(tone);
   digitalWrite(speakerPin, LOW);
   delayMicroseconds(tone);
}
}

void playNote(char note, int duration) {
char names[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B',           

                 'c', 'd', 'e', 'f', 'g', 'a', 'b',

                 'x', 'y' };
int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014,

                 956,  834,  765,  593,  468,  346,  224,

                 655 , 715 };
int SPEE = 5;
for (int i = 0; i < 17; i++) {
   if (names[i] == note) {
    int newduration = duration/SPEE;
     playTone(tones[i], newduration);
   }
}
}

long read_sonic(void){
      long duration,  cm;
      pinMode(Trig, OUTPUT);
      digitalWrite(Trig, LOW);   // send a pulse of low – high- low 
      delayMicroseconds(2);
      digitalWrite(Trig, HIGH);    // the high is 5ms
      delayMicroseconds(5);
      digitalWrite(Trig, LOW);
      pinMode(Echo, INPUT);
      duration = pulseIn(Echo, HIGH);   // read the reflected signal
      cm = duration/(29*2);
      delay(100);
      return cm;
      }
//===============================================================
//                  SETUP
//===============================================================
void setup(void){
  pinMode(LED1, OUTPUT);     
  pinMode(LED2, OUTPUT);  
  pinMode(LED3, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); 
 IPAddress myIP = WiFi.softAPIP(); //Get IP address
 server.on("/", handleRoot);      //Which routine to handle at root location
 Serial.print(myIP);
 server.on("/socket1On", [](){  
 Status = "Normal Mode"; 
 webSite="";
  webSite+="<body id='BackgroundColor' style='background-color:rgb(135, 156, 201)'> </body>";
  webSite+="<p><button>HOME</button></p>";
  webSite+="<h1>Home Security Device Control Panel</h1>";
  webSite+="<font id='FontColor' color='Green'><h2 id='Status'>Status: ";
  webSite+="Normal Mode";
  webSite+="</h2></font>";
  webSite += "<p>Normal Mode <a href=\"socket1On\"><button>Enable</button></a></p>";
  webSite += "<p>Security Mode <a href=\"socket2On\"><button>Enable</button></a></p>";
  webSite += "<p>Birthday Mode <a href=\"socket3On\"><button>Enable</button></a></p>";
  webSite += "<p>For Refill Paint Only <a href=\"socket4On\"><button>Lock1</button></a></p>";
  webSite += "<p>For Refill Ribbon Only <a href=\"socket5On\"><button>Lock2</button></a></p>";   
 server.send(200, "text/html", webSite);  
 digitalWrite(LED2, LOW);
 digitalWrite(LED1, HIGH);
 digitalWrite(LED3, LOW);
});
 server.on("/socket2On", [](){  
 Status = "Security Mode";
 long dist=read_sonic();
 usreading = String(dist);
 webSite ="<meta http-equiv='refresh' content='3'><title>ESP8266 Demo</title>\n";
  webSite+="<body id='BackgroundColor' style='background-color:rgb(135, 156, 201)'> </body>";
  webSite+="<p><button>HOME</button></p>";
  webSite+="<h1>Home Security Device Control Panel</h1>";
  webSite+="<font id='FontColor' color='Green'><h2 id='Status'>Status: ";
  webSite+="Security Mode";
  webSite+="</h2></font>";
  webSite += "<p>Normal Mode <a href=\"socket1On\"><button>Enable</button></a></p>";
  webSite += "<p>Security Mode <a href=\"socket2On\"><button>Enable</button></a></p>";
  webSite += "<p>Birthday Mode <a href=\"socket3On\"><button>Enable</button></a></p>";
  webSite += "<p>For Refill Paint Only <a href=\"socket4On\"><button>Lock1</button></a></p>";
  webSite += "<p>For Refill Ribbon Only <a href=\"socket5On\"><button>Lock2</button></a></p>";  
  webSite+="<p> Reading from sonic sensor:";
  webSite+=usreading;
  webSite+="</p>\n";     
 server.send(200, "text/html", webSite);
 digitalWrite(LED2, HIGH);
 digitalWrite(LED1, LOW);
 digitalWrite(LED3, LOW);
});
 server.on("/socket3On", [](){     
 Status = "Birthday Mode";
  long dist=read_sonic();
 usreading = String(dist);
  webSite ="<meta http-equiv='refresh' content='3'><title>ESP8266 Demo</title>\n";
  webSite+="<body id='BackgroundColor' style='background-color:rgb(135, 156, 201)'> </body>";
  webSite+="<p><button>HOME</button></p>";
  webSite+="<h1>Home Security Device Control Panel</h1>";
  webSite+="<font id='FontColor' color='Green'><h2 id='Status'>Status: ";
  webSite+="Birthday Mode";
  webSite+="</h2></font>";
  webSite += "<p>Normal Mode <a href=\"socket1On\"><button>Enable</button></a></p>";
  webSite += "<p>Security Mode <a href=\"socket2On\"><button>Enable</button></a></p>";
  webSite += "<p>Birthday Mode <a href=\"socket3On\"><button>Enable</button></a></p>";
  webSite += "<p>For Refill Paint Only <a href=\"socket4On\"><button>Lock1</button></a></p>";
  webSite += "<p>For Refill Ribbon Only <a href=\"socket5On\"><button>Lock2</button></a></p>"; 
  webSite+="<p> Reading from sonic sensor:";
  webSite+=usreading;
  webSite+="</p>\n";     
      
 server.send(200, "text/html", webSite);
 digitalWrite(LED3, HIGH);
 digitalWrite(LED2, LOW);
 digitalWrite(LED1, LOW);
});
 server.on("/socket4On", [](){     
 server.send(200, "text/html", webSite);
 int pos;
 for(pos = 90; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
      {                                
      servo1.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(3);                       // waits 15ms for the servo to reach the position 
      } 
 
});
 server.on("/socket5On", [](){     
 server.send(200, "text/html", webSite);
 int pos;
 for(pos = 90; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
      {                                
      servo2.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(3);                       // waits 15ms for the servo to reach the position 
      } 
 
});

  server.begin();                  //Start server

}
//===============================================================
//                     LOOP
//===============================================================
void loop(void){
  server.handleClient();          //Handle client requests
      if (Status == "Security Mode"){
      IR_State = digitalRead(IR_Pin);
      int x = 1 ;
      if ((IR_State == 0    ) and (x == 1)){
      long dist=read_sonic();
      usreading = String(dist);
      Serial.println(usreading);
      server.send(200, "text/html",webSite_usresult);
      int pos;
      for(pos = 0; pos <= 90; pos += 1) // goes from 0 degrees to 180 degrees 
      {                                  // in steps of 1 degree 
         servo1.write(pos);              // tell servo to go to position in variable 'pos' 
         delay(3);                       // waits 15m s for the servo to reach the position 
      } 
      for (int i = 0; i < length; i++) {
      if (notes2[i] == ' ') {
      delay(beats2[i] * tempo); // rest
      } else {
      playNote(notes2[i], beats2[i] * tempo);
      }
      delay(tempo);
      }
      }
      x = 0;
      }

      if (Status == "Birthday Mode"){
      IR_State = digitalRead(IR_Pin);
      int x = 1;
      if ((IR_State == 0) and (x == 1)){
      long dist=read_sonic();
      usreading = String(dist);
      Serial.println(usreading);
      int pos;
      for(pos = 0; pos <= 90 ; pos += 1) // goes from 0 degrees to 180 degrees 
      {                                  // in steps of 1 degree 
         servo2.write(pos);              // tell servo to go to position in variable 'pos' 
         delay(3);                       // waits 15ms for the servo to reach the position 
      } 
      for (int i = 0; i < length; i++) {
      if (notes1[i] == ' ') {
      delay(beats1[i] * tempo); // rest
      } else {
      playNote(notes1[i], beats1[i] * tempo);
      }
      delay(tempo);
      }
      }
     x = 0; 
    }

}
     
