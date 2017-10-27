/********************************************************************
 * Power Advanced Strip
 * 
 * ON/OFF Channel with bluetooth and IR remote control
 * -----------------------------------------------------------------
 * @author  Mattia Leonardo Angelillo
 * @email   mattia.angelillo@gmail.com
 * @version 2.0.1
********************************************************************/

/*LIBRERIE*/
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <Time.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
/*---------------------------------------------*/

/**
 * Author name
 *
 * @type string
 */
String name = "Mattia Leonardo Angelillo";
/**
 * Author name
 *
 * @type string
 */
String email = "mattia.angelillo@gmail.com";
/**
 * Version value
 *
 * @type string
 */
String version = "1.0";
/**
* 
* @type int
*/
#define BT_TX_PIN  7
/**
* 
* @type int
*/
#define BT_RX_PIN  8
/**
 * IR sensor receiver
 *
 * @type integer
*/
int receiver = 9;
/**
 * Rele PIN
 *
 * @type integer array
*/
#define SIZE 4
/**
* 
*/
const int rele[SIZE] = {2, 3, 4, 5};
/**
 * Flag rele ON/OFF
*/
int flag_rele[SIZE] = {0, 0, 0, 0};
/**
*
* @type int
*/
int previousSec = 0;
/**
* @type char
*/
char lettera;
/**
* @type char
*/
char buffer[16];
/**
 * Instance of IRrecv
 *
 * @type IRrecv
*/
IRrecv irrecv(receiver); // create instance of 'irrecv'
/**
 * Results of receiver data
 *
 * @type decode_results
*/
decode_results results;

 
uint8_t SPENTO[8] = {0x0E, 0x11, 0x11, 0x11, 0x0A, 0x0E, 0x0E, 0x04,};
uint8_t ACCESO[8] = {0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x04,};


/*******************************************************************/
SoftwareSerial mySerial(BT_TX_PIN, BT_RX_PIN);
RTC_DS1307 RTC;
LiquidCrystal_I2C lcd(0x3F, 16, 2);
/*******************************************************************/

/**
 * Setup data
 *
 * @return void
*/
void setup()
{
  /*SERIAL*/
  Serial.begin(9600); // for serial monitor output
  Serial.println("Author: " + name);
  Serial.println("Email: " + email);
  Serial.println("Version: " + version);
  Serial.println();
  Serial.println("* PIN IR: 2");
  Serial.println("* PIN Rele: 3,4,5,6");
  Serial.println();
  Serial.println("Manage only 4 relè!");
  Serial.println();
  Serial.println();
  
  lcd.backlight();
  mySerial.begin(9600);
  Wire.begin();
  RTC.begin();
  lcd.init();


  /*IR*/
  irrecv.enableIRIn(); // Start the receiver

  /*Rele PIN*/
  for (int i = 0; i < SIZE; i++) {
    pinMode(rele[i], OUTPUT);
  }

  /*OFF all relè*/
  of_all_rele();
     
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }else{
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
   
     
  lcd.createChar(0, SPENTO);
  lcd.createChar(1, ACCESO);
  lcd.setCursor (0, 0);
  lcd.print("Ciabatta BT v.2");
  delay(2000);
  lcd.clear();
  lcd.setCursor (12, 0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
  lcd.write(0);
}
void loop()
{
  DateTime now = RTC.now();
  
  if(now.second() != previousSec) {
    lcd.setCursor( 0,0 );
    sprintf(buffer,  "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    lcd.print( buffer );
    previousSec = now.second();
  }
  
  if(mySerial.available()){
    lettera = mySerial.read();
    
    switch(lettera){
        case 'A':
          on_off_rele(0);
        break;
         
        case 'S':
          on_off_rele(0);
        break;
         
        case 'Q':
          on_off_rele(1);
        break;
         
        case 'W':
          on_off_rele(1);
        break;
         
        case 'E':
          on_off_rele(2);
        break;
         
        case 'R':
          on_off_rele(2);
        break;
         
        case 'T':
          on_off_rele(3);
        break;
         
        case 'Y':
          on_off_rele(3);
        break;
    }
 } 
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    /*Serial.println(results.value, HEX); // display it on serial monitor in hexadecimal*/
    irrecv.resume();// receive the next value
  }

  //ON/OFF Relè
  if ( results.value == 0xFF30CF && results.value != 0xFFFFFFFF) {//Button 1
    Serial.print("R1: ");
    on_off_rele(0);
  } else if ( results.value == 0xFF18E7 && results.value != 0xFFFFFFFF) {
    Serial.print("R2: ");
    on_off_rele(1);
  } else if ( results.value == 0xFF7A85 && results.value != 0xFFFFFFFF) {
    Serial.print("R3: ");
    on_off_rele(2);
  } else if ( results.value == 0xFF10EF && results.value != 0xFFFFFFFF) {
    Serial.print("R4: ");
    on_off_rele(3);
  }

  //Delete signal
  results.value = 0xFFFFFFFF;



  delay(200);
}


/**
 * Turn ON/OFF relè selected
 *
 * @param int index
 * @return void
 */
void on_off_rele(int index) {
  if (flag_rele[index] == LOW) {
    Serial.println("ON");

    flag_rele[index] = HIGH;
    
    viewSocket(1, true);

    //ON relè
    digitalWrite(rele[index], LOW);

  } else if (flag_rele[index] == HIGH) {
    Serial.println("OFF");

    flag_rele[index] = LOW;
    
    viewSocket(1, false);

    //OFF relè
    digitalWrite(rele[index], HIGH);
  }
}

/**
 * Turn off all relè
 *
 * @return void
 */
void of_all_rele() {
  for (int i = 0; i < SIZE; i ++) {
    flag_rele[i] = 0;
    digitalWrite(rele[i], HIGH);
  }
}


/**
* 
*
* @return void
*/
void viewSocket(byte num, boolean status) {
  lcd.setCursor( ( 11+num ),0 );
  if ( status ) { 
     lcd.write(1); 
     lcd.setCursor ( 0,1 );
     lcd.print("ON CH: "); 
  } else { 
    lcd.write(0);
    lcd.setCursor ( 0,1 );
    lcd.print("OFF CH: ");
  }
  lcd.setCursor ( 12,1 );
  lcd.print(num);
}
