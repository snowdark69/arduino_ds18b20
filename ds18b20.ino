#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <String.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char tempF[6]; // buffer for temp incl. decimal point & possible minus sign

String message3; //Temp
String message4; //For Line Notify
String message5; //humidity
String message6 = "Server_Main"; //สถานที่
//String message6 = "ทดสอบระบบ"; //สถานที่    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้
String message7 = "2"; //iot_main_id   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้

const int LED1 =  D7;  // เชื่อมต่อ Wifi ได้
const int LED2 =  D8;  // แจ้งเตือน มีปัญห

const char* host = "HOST"; //Host    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้

#define DHTPIN D6     // wDhat digital pin we're connected to
#define DHTTYPE DHT21   // DHT 21
DHT dht(DHTPIN, DHTTYPE); // กำหนดให้ SENSOR อุณหภูมิความชื่น เข้าทาง D2

#define ONE_WIRE_BUS D3 //กำหนดขาที่จะเชื่อมต่อ Sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid     = "ssid"; //Set Wifi SSID    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้
const char* password = "password";//Set Wifi password    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้

WiFiClient client;


void setup() {
  Serial.begin(115200);
  //Serial.begin(57600);
  //Serial.println("DHT21_KSH");
  
  pinMode(LED1, OUTPUT); // กำหนดขาทำหน้าที่ให้ขา D7 เป็น OUTPUT 
  pinMode(LED2, OUTPUT); // กำหนดขาทำหน้าที่ให้ขา D8 เป็น OUTPUT 
  lcd.begin();
  lcd.backlight();
  
  WiFi.begin(ssid, password);

  //Set WiFi mode
  //You can choose between WIFI_AP, WIFI_STA, WIFI_AP_STA or WIFI_OFF
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Waiting For Connect Wifi.");
    lcd.print("Waiting Wifi.");
    lcd.setCursor(0, 0);
    digitalWrite(LED2, HIGH);
    delay(300);
    digitalWrite(LED2, LOW);

  }

  Serial.println("");
  Serial.println("WiFi connected");
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  
  ThingSpeak.begin(client);

  digitalWrite(LED1, HIGH);
  sensors.begin();
  dht.begin();
  delay(2000);// = 2 วิ
}



void loop(void) {
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
  //if (!client.connectSSL(host, httpPort)) {
    Serial.println("Connection Failed");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error Connect !!");
    digitalWrite(LED1, LOW);
    
    digitalWrite(LED2, HIGH);
    delay(300);
    digitalWrite(LED2, LOW);
    delay(300);
    digitalWrite(LED2, HIGH);
    delay(300);
    digitalWrite(LED2, LOW);
    return;
  }

  float h = dht.readHumidity();

  sensors.requestTemperatures(); //อ่านข้อมูลจาก library
  float t = sensors.getTempCByIndex(0);
  Serial.print("Temperature is: ");
  Serial.print(t); // แสดงค่า อูณหภูมิ
  Serial.println(" *C");
  Serial.print("Humidity: ");
  Serial.println(h);


  message3 = dtostrf(t, 5, 2, tempF);
  //message5 = dtostrf(55.00, 6, 2, tempF);
  message5 = dtostrf(h, 5, 2, tempF);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T="+message3+"/H="+message5);

  delay(2000);// = 2 วิ

  if (client.connect(host, httpPort)) {
    HTTPClient http;

    http.begin("http://localhost/iot/add_data.php");    //host post <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้
  
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("temp=" + message3  + "&humidity=" + message5 + "&place=" + message6 + "&iot_main_id=" + message7);
  
    if (httpCode == HTTP_CODE_OK)
    {
      Serial.println("Insert to database success !!!");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Insert OK !!");
    }
    else
    {
      Serial.println("Error in HTTP request");
      Serial.println(httpCode);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Can't insert !!");
    }
  
    http.end();
  }

  //delay(10000);// = 10 วิ    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้
  //delay(60000);// = 1 นาที    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้
  delay(300000);// = 5 นาที    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้
  //delay(900000);// = 15 นาที    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้
  //delay(1800000);// = 30 นาที    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<แก้
}
