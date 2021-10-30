#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>  //Ticker Library

Ticker blinker;

#define LED 2  //On board LED

void changeState()
{
  digitalWrite(LED, !(digitalRead(LED)));  //Invert Current State of LED  
}

//WiFi credentials
const char* ssid = "itsWiFi";
const char* password = "P@ssW0rdEdhukuquestionmark";

//DuckDns token and domain
const char* _getLink = "http://www.duckdns.org/update?domains=myrdp&token=e27759d9-023e-4a14-90d1-5eb7410785f1&ip=";

//variable declaration
String ip;
int cycle = 1;

//object declaration
WiFiClient client;
HTTPClient http;

void setup() {
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  Serial.begin(9600);
  delay(1000);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  //connect to local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1500);
    Serial.print(".");
    digitalWrite(LED,LOW);
    blinker.attach(0.5, changeState);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(LED,LOW);
  //blinker.attach(2, changeState);   //Initialize Ticker every 2s
}

void loop()
{
  blinker.attach(0.1, changeState);
  Serial.print("Update Cycle: ");
  Serial.println(cycle);
  
  GetExternalIP();  //Get or Update public ip address
  
  //Configure DuckDns update link
  String getLink;
  getLink = _getLink + ip + "&verbose=true";
  
  if (WiFi.status() == WL_CONNECTED) //Check WiFi connection status 
  { 
    //Send request
    //never ever use https use only http damn
    http.begin(getLink);    
    int httpCode = http.GET();                                 

    //Check if the request got any valid response
    //Check the returning code
    if (httpCode > 0)
    { 
      String payload = http.getString();   //Get the request response payload
      
      //Print the response payload
      Serial.println("Response from DuckDns");
      delay(3000);//for led indication
      Serial.println("{");
      Serial.println(payload);             
      Serial.println("}");
      Serial.println("**********************************");
    }

    //Detects error in http request 
    if (httpCode !=200)
    {
      Serial.println("!!!!ERROR BAD RESPONSE from DuckDns!!!!");
    }
    http.end();     //Close connection
  }
  cycle++;         //Update cycle number
  Serial.println();
  Serial.println();
  
  blinker.attach(2, changeState);
  delay(30000);    //Send a request every 30 seconds
}

void GetExternalIP()
{
  HTTPClient http;
  http.begin("http://api64.ipify.org/?format=json");     //Specify request destination

  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload
  ip = "";
  //Serial.println(httpCode1);   //Print HTTP return code
  //Serial.println(payload1);      //Print request response payload
  http.end();
  
  int n=7;
  
  while(payload[n]!='"')
  {
    ip += payload[n];
    n++;
  }
  //Serial.println("**********************************");
  Serial.print("Current Public IP: ");
  Serial.println(ip);
}
