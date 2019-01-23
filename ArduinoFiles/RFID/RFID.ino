#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x3F, 16,2);

#define SS_PIN 10
#define RST_PIN 9
#define RX_pin 0
#define TX_pin 1

SoftwareSerial mySerial(RX_pin, TX_pin); // RX, TX on arduino
int i=0;
int readfix=0;
int pos=0;
int n=0;
int screenWidth = 16;
int screenHeight = 2;
int stringStart, stringStop = 0;
int scrollCursor = screenWidth;
String line1 = "BY ROBOTICS & CIRCUITS ";
String member_name;
String club_name;
String returncode;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int red= 4;
int green= 3;
void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);   // Initiate a serial communication
  mySerial.begin(115200);
  lcd.setCursor(0, 0);
  lcd.print("CONNECTING...");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print("CONNECTED!");
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("INITIALIZING...");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print("PLEASE WAIT");
  delay(1000);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  lcd.clear();
}

void loop()
{
  Serial.println();
  Serial.println("\nApproximate your card to the reader...");
  lcd.setCursor(0,0);
  lcd.print("Flash Your Card");
  lcd.setCursor(1,1);
  scroll();
  lcd.print(line1.substring(stringStart,stringStop));
  // Look for new cards
  while ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    delay(10);
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i],HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  for(int i=0;i<13;i++)
  {
    mySerial.write(content[i]);
  }
  mySerial.print(420);
  if(mySerial.available()) returncode = mySerial.read();
  if(returncode=="-1")
  {
    Serial.println("Invalid Card");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Corrupt data card");
    lcd.setCursor(0, 1);
    lcd.print("Authorization Failed");
    Serial.println();
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
    delay(3000);
    digitalWrite(red,LOW);
    lcd.clear();
  }
  else if(returncode=="0")
  {
    Serial.println("No Match");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UID mismatch error");
    lcd.setCursor(0, 1);
    lcd.print("UID doesn't exist in database");
    Serial.println();
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
    delay(3000);
    digitalWrite(red,LOW);
    lcd.clear();
  }
  else {
    Serial.println("Match found!");
    n=(returncode).length();
    for(int i=0;i<n;i++)
    {
      if(returncode[i]=='-')
      {
        pos=i;
        break;
      }
    }
    member_name=returncode.substring(0,pos); //appends Name of the member to member_name
    club_name=returncode.substring(pos+1,n); //appends Club Name of the member to club_name
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(member_name);
    lcd.setCursor(0, 1);
    lcd.print(club_name);
    Serial.println();
    digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
    delay(3000);
    digitalWrite(green,LOW);
    lcd.clear();
  }
}

int str_length()
{
  if(line1)
  {
    return line1.length();
  }
}

void scroll()
{
  lcd.setCursor(scrollCursor, 1);
   if(stringStart == 0 && scrollCursor > 0){
    scrollCursor--;
    stringStop++;
  } else if (stringStart == stringStop){
    stringStart = stringStop = 0;
    scrollCursor = screenWidth;
  } else if (stringStop == str_length() && scrollCursor == 0) {
    stringStart++;
  } else {
    stringStart++;
    stringStop++;
  }
  delay(300);
}
