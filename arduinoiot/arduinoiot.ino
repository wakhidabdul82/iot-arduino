
#define BLYNK_PRINT Serial

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // Library untuk LCD 
#include <ESP8266_Lib.h> // Library Modul ESP
#include <BlynkSimpleShieldEsp8266.h> // Library untuk Aplikasi Blynk
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Copy Paste Kode Autentifikasi yang terdapat di blynk apps ke Char Auth.
// Setiap pembuatan project baru akan memdapatkan kode autentifikasi baru juga.
char auth[] = "9aHYreBZwUfxwKnwzWN6TvFpjpmzK6XO";

// Buat wifi dengan menggunakan Smartphone sebagai media transfer data.
char ssid[] = "namawifi";
char pass[] = "katasandiwifi";

// Untuk Hardware Mega, Leonardo, dan Micro menggunakan.... 
#define EspSerial Serial1

// Sedangkan untuk Uno dan Nano menggunakan...
//#include <SoftwareSerial.h>
//SoftwareSerial EspSerial(2, 3); // RX, TX

//ESP8266 baud rate:
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

#define DHTPIN 8          // Pin sensor yang dikoneksikan ke mikro (menggunakan pin digital 1-53)


#define DHTTYPE DHT11     // untuk tipe sensor DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;



void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // atau dht.readTemperature(true) untuk menampilkan temperatur dalam skala Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Sensor DHT gagal membaca!");
    return;

  }
  
  Blynk.virtualWrite(V5, h); 
  Blynk.virtualWrite(V6, t); // Menampilkan hasil pengukuran suhu pad Virtual 6 aplikasi Blynk
}
const int sensorIn = A1; // menyambungkan sensor arus 1 pada pin A1 mikrokontroller
int mVperAmp = 100; // gunakan 100 untuk modul tipe 20A dan 66 untuk modul tipe 30A

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

SimpleTimer timer0;

void readACS712() {
 Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707; 
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 Serial.print(AmpsRMS);
 Serial.println(" Amps RMS");
 Blynk.virtualWrite(V13, AmpsRMS); // menampilkan hasil pengukuran sensor arus pada virtual 13 aplikasi blynk
}

const int sensorIn1 = A2; // menyambungkan sensor arus 2 pada pin A2 Mikrokontroller
int mVperAmp1 = 100; // gunakan 100 untuk modul tipe 20A dan 66 untuk modul tipe 30A

double Voltage1 = 0;
double VRMS1 = 0;
double AmpsRMS1 = 0;

SimpleTimer timer1;

void readACS712s() {
 Voltage1 = getVPP1();
 VRMS1 = (Voltage1/2.0) *0.707; 
 AmpsRMS1 = (VRMS1 * 1000)/mVperAmp1;
 Serial.print(AmpsRMS1);
 Serial.println(" Amps RMS");
 Blynk.virtualWrite(V12, AmpsRMS1); // menampilkan hasil pengukuran sensor arus pada virtual 12 aplikasi blynk
 pinMode(sensorIn1, INPUT);
}

const int sensorIn2 = A4; // menyambungkan sensor arus 3 pada pin A4 mikrokontroler
int mVperAmp2 = 100; // gunakan 100 untuk modul tipe 20A dan 66 untuk modul tipe 30A

double Voltage2 = 0;
double VRMS2 = 0;
double AmpsRMS2 = 0;

SimpleTimer timer2;

void readACS712t() {
 Voltage2 = getVPP2();
 VRMS2 = (Voltage2/2.0) *0.707; 
 AmpsRMS2 =(VRMS2 * 1000)/mVperAmp2;
 Serial.print(AmpsRMS2);
 Serial.println(" Amps RMS");
 Blynk.virtualWrite(V11, AmpsRMS2); // menampilkan hasil pengukuran sensor arus pada virtual 11 aplikasi blynk
 pinMode(sensorIn2, INPUT);
}


const int btnPin = 9; // Pengaruran Relay pemutus daya yang disambungkan dengan pin 9 mikrokontroller

WidgetLED led5(V3); // membuat tombol ON/OFF pada virtual 5 aplikasi Blynk


boolean btnState = false;
void buttonLedWidget()
{
  // Read button
  boolean isPressed = (digitalRead(btnPin) == LOW);

  // If state has changed...
  if (isPressed != btnState) {
    if (isPressed) {
      led5.on();
    } else {
      led5.off();
    }
    btnState = isPressed;
  }
}

const int TOMBOLEMERG=9;

void setup(){ 
 Serial.begin(9600); 
  lcd.init();                      // initialize the lcd 
  lcd.init();
  
  lcd.backlight(); 
  
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);
  dht.begin();

  // Setup fungsi untuk memanggil data setiap 1 detik sekali
  timer.setInterval(1000L, sendSensor);
  timer0.setInterval(1000L, readACS712);
  timer1.setInterval(1000L, readACS712s);
  timer2.setInterval(1000L, readACS712t);

pinMode(TOMBOLEMERG, OUTPUT);
 
}

void loop(){
 lcd.clear();
 float t = dht.readTemperature();

  lcd.setCursor(0,0);
  lcd.print("REALTIME PROTECT");
  lcd.setCursor(0,1);
  lcd.print("Suhu : ");
  lcd.print(t);
  lcd.print(" C");

  delay(1000);
  
 Blynk.run();
 timer.run();
 timer0.run();
 timer1.run();
 timer2.run();

if(t >=35 || AmpsRMS >=1.73 || AmpsRMS1>=1.73 || AmpsRMS2>=1.73){
  digitalWrite(btnPin, HIGH); 
}
}


float getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }

 float getVPP1()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn1);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }

 float getVPP2()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn2);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }

 
