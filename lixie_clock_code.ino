#include <DS1302RTC.h>                                                      //Uključivanje potrebnih
                                                                            //library-a
#include <EEPROM.h>

#include <Time.h>
#include <TimeLib.h>

#include <LiquidCrystal_I2C.h>                                      
#include <Wire.h>

#include <FastLED.h>

#define po 7
#define push 10
#define DS1302_GND_PIN 5
#define DS1302_VCC_PIN 6
#define LED_PIN     A0
#define NUM_LEDS    120

LiquidCrystal_I2C lcd(0x3F,20,4);                                           //Inicijalizacija LCD zaslona
uint8_t arrow[8] = {0x0, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};         //Kreiranje znaka strelice

int w = 0;
int v = 0;
int i = 0;
int j1 = 0;                                                                 //Varijable koje služe kao
int j2 = 0;                                                                 //privremeni spremnici 
int j3 = 0;                                                                 //pomoću kojih se dekodira
int j4 = 0;                                                                 //koji broj se treba 
int j5 = 0;                                                                 //prikazati na segmentu
int j6 = 0;                                                                 //
int z1 = 0;                                                                 
int z2 = 20;                                                                
int z3 = 40;                                                                
int z4 = 60;                                                                
int z5 = 80;                                                                
int z6 = 100;                                                               
int n = 0;                                                                  
int h[2];                                                                   //Varijable koje upravljaju
int m[2];                                                                   //segmentima
int s[2];                                                                   //
int h1p = 0;                                                                //
int h2p = 0;                                                                //
int m1p = 0;                                                                //
int m2p = 0;                                                                //
int s1p = 0;                                                                //
int s2p = 0;                                                                //

int svj = 255;                                                              //Varijabla koja upravlja
                                                                            //svjetlinom WS2812B dioda
int r = 0;                                                                  //Varijabla za crvenu boju
int g = 0;                                                                  //Varijabla za zelenu boju
int b = 0;                                                                  //Varijabla za plavu boju

int lcounter = 0;
int counter = 0; 
int page=1;
int Ready=1;
int submenu=0;
int last_counter = 0; 
bool clk_State;
bool Last_State; 
bool dt_State;  
int pushed = 0;

int ch = 0;                                                                 //Varijabla za alarm (sati)
int cm = 0;                                                                 //Varijabla za alarm (minute)
int x[4];
int a = 1;                                                                  //Varijabla koja određuje
int as = 0;                                                                 //ima li alarma ili ne
int l = 0;
int cler = 0;
int clr = 0;

int zet=0;
int sh=0;
int sm=0;
int ss=0;
int sh1=0;
int sh2=0;
int sm1=0;
int sm2=0;
int ss1=0;
int ss2=0;

DS1302RTC RTC(2, 3, 4);                                                     //Inicijalizacija RTC modula
                                                                            //na pinovima 2, 3, 4
CRGB leds[NUM_LEDS];                                                        //Inicijalizacija WS2812B dioda

void setup() {
  svj = EEPROM.read(0);                         //Čitaju se spremljene
  r = EEPROM.read(1);                           //vrijednosti iz EEPROM-a
  g = EEPROM.read(2);                           //mikroupravljača ATmega328p
  b = EEPROM.read(3);                           //
  x[0] = EEPROM.read(4);                        //Učitavaju se vrijednosti
  x[1] = EEPROM.read(5);                        //za svjetlinu, boju i
  x[2] = EEPROM.read(6);                        //vrijeme za kad je alarm
  x[3] = EEPROM.read(7);                        //postavljen
  as = EEPROM.read(8);                          //Učitavanje ima li alarma ili ne

  PCICR |= (1 << PCIE0);                                           
  PCMSK0 |= (1 << PCINT0);
  PCMSK0 |= (1 << PCINT1);
  DDRB &= B11111100;

  Last_State =   (PINB & B00000001);
  
  pinMode (push, INPUT_PULLUP);
  
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
  Serial.begin(9600);
  
  digitalWrite(DS1302_GND_PIN, LOW);
  pinMode(DS1302_GND_PIN, OUTPUT);

  digitalWrite(DS1302_VCC_PIN, HIGH);
  pinMode(DS1302_VCC_PIN, OUTPUT);

  Serial.print("RTC activated!\n");
  

  if (RTC.haltRTC())                                        
    Serial.print("Clock stopped!\n");
  else
    Serial.print("Clock working.\n");

  if (RTC.writeEN())
    Serial.print("Write allowed\n.");
  else
    Serial.print("Write protected.\n");

  setSyncProvider(RTC.get);

  if(timeStatus() == timeSet)
    Serial.print(" Ok!\n");
  else
    Serial.print(" FAIL!\n");

  lcd.init();
  lcd.begin(20,4);
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello, Mate!");
  lcd.createChar(1, arrow);
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("Welcome to menu");
  delay(2000);
  Serial.print("Setup complete!");
  Serial.print("\n");
}

void loop(){
  counter = 2;
  lcd.setCursor(0,0);
  lcd.print("Standby & Ready!");
  lcd.setCursor(0,1);
  lcd.print("Press button to open");
  lcd.setCursor(0,2);
  lcd.print("the menu. Clock will");
  lcd.setCursor(0,3);
  lcd.print("stop when menu opens");
  FastLED.setBrightness(svj);
  tmElements_t tm;
  RTC.read(tm);
  n = tm.Hour;
  for (i=0; i<2; i++){
    if(i == 0){
      h[i] = n / 10;
    }
    if(i == 1){
      h[i] = n % 10;
    }
  }
  n = tm.Minute;
  for (i=0; i<2; i++){
    if(i == 0){
      m[i] = n / 10;
    }
    if(i == 1){
      m[i] = n % 10;
    }
  }
  n = tm.Second;
  for (i=0; i<2; i++){
    if(i == 0){
      s[i] = n / 10;
    }
    if(i == 1){
      s[i] = n % 10;
    }
  }
  if(w==0){
    h[0] = 0;
    h[1] = 0;
    m[0] = 0;
    m[1] = 0;
    s[0] = 0;
    s[1] = 0; 
    j1 = 103;
    j2 = 83;
    j3 = 63;
    j4 = 43;
    j5 = 23;
    j6 = 3;
    w = 1;
  }
  if(h[0] != h1p){
    h1p = h[0];
    //Hours
    if(h[0] == 0){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 103;
        //0
    }
    if(h[0] == 1){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 104;
        //1
    }
    if(h[0] == 2){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 102;
        //2
    }
    if(h[0] == 3){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 100;
        //3
    }
    if(h[0] == 4){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 108;
        //4
    }
    if(h[0] == 5){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 106;
        //5
    }
    if(h[0] == 6){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 105;
        //6
    }
    if(h[0] == 7){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 107;
        //7
    }
    if(h[0] == 8){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 109;
        //8
    }
    if(h[0] == 9){
        for(z1 = 100; z1 < 120; z1++){
          leds[z1] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j1 = 101;
        //9
   }
  }
  if(h[1] != h2p){
    h2p = h[1];
    //Hours
    if(h[1] == 0){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 83;
        //0
    }
    if(h[1] == 1){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 84;
        //1
    }
    if(h[1] == 2){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 82;
        //2
    }
    if(h[1] == 3){
        for(z2 = 20; z2 < 40; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 80;
        //3
    }
    if(h[1] == 4){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 88;
        //4
    }
    if(h[1] == 5){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 86;
        //5
    }
    if(h[1] == 6){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 85;
        //6
    }
    if(h[1] == 7){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 87;
        //7
    }
    if(h[1] == 8){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 89;
        //8
    }
    if(h[1] == 9){
        for(z2 = 80; z2 < 100; z2++){
          leds[z2] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j2 = 81;
        //9
   }
  }
  if(m[0] != m1p){
    m1p = m[0];
    //Minutes
    if(m[0] == 0){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 63;
        //0
    }
    if(m[0] == 1){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 64;
        //1
    }
    if(m[0] == 2){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 62;
        //2
    }
    if(m[0] == 3){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 60;
        //3
    }
    if(m[0] == 4){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 68;
        //4
    }
    if(m[0] == 5){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 66;
        //5
    }
    if(m[0] == 6){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 65;
        //6
    }
    if(m[0] == 7){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 67;
        //7
    }
    if(m[0] == 8){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 69;
        //8
    }
    if(m[0] == 9){
        for(z3 = 60; z3 < 80; z3++){
          leds[z3] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j3 = 61;
        //9
   }
  }
  if(m[1] != m2p){
    m2p = m[1];
    //Minutes
    if(m[1] == 0){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 43;
        //0
    }
    if(m[1] == 1){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 44;
        //1
    }
    if(m[1] == 2){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 42;
        //2
    }
    if(m[1] == 3){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 40;
        //3
    }
    if(m[1] == 4){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 48;
        //4
    }
    if(m[1] == 5){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 46;
        //5
    }
    if(m[1] == 6){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 45;
        //6
    }
    if(m[1] == 7){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 47;
        //7
    }
    if(m[1] == 8){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 49;
        //8
    }
    if(m[1] == 9){
        for(z4 = 40; z4 < 60; z4++){
          leds[z4] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j4 = 41;
        //9
   }
  }
  if(s[0] != s1p){
    s1p = s[0];
    //Seconds
    if(s[0] == 0){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 23;
        //0
    }
    if(s[0] == 1){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 24;
        //1
    }
    if(s[0] == 2){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 22;
        //2
    }
    if(s[0] == 3){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 20;
        //3
    }
    if(s[0] == 4){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 28;
        //4
    }
    if(s[0] == 5){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 26;
        //5
    }
    if(s[0] == 6){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 25;
        //6
    }
    if(s[0] == 7){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 27;
        //7
    }
    if(s[0] == 8){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 29;
        //8
    }
    if(s[0] == 9){
        for(z5 = 20; z5 < 40; z5++){
          leds[z5] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j5 = 21;
        //9
   }
  }
  if(s[1] != s2p){
    s2p = s[1];
    //Seconds
    if(s[1] == 0){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 3;
        //0
    }
    if(s[1] == 1){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 4;
        //1
    }
    if(s[1] == 2){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 2;
        //2
    }
    if(s[1] == 3){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 0;
        //3
    }
    if(s[1] == 4){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 8;
        //4
    }
    if(s[1] == 5){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 6;
        //5
    }
    if(s[1] == 6){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 5;
        //6
    }
    if(s[1] == 7){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6
          ] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 7;
        //7
    }
    if(s[1] == 8){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 9;
        //8
    }
    if(s[1] == 9){
        for(z6 = 0; z6 < 20; z6++){
          leds[z6] = CRGB(0, 0, 0);
          FastLED.show();
        }
        j6 = 1;
        //9
   }
  }
  showing();
  
  if(digitalRead(10) == 0){
    menu();
  }
  x[0] = EEPROM.read(4);                                                    
  x[1] = EEPROM.read(5);                                                    
  x[2] = EEPROM.read(6);                                                    
  x[3] = EEPROM.read(7);
  Serial.print(x[0]);
  Serial.print(x[1]);
  Serial.print(":");
  Serial.print(x[2]);
  Serial.print(x[3]);
  Serial.print("\n");
  if(as != 0 && x[0] == h[0] && x[1] == h[1] && x[2] == m[0] && x[3] == m[1]){
    Serial.print("Alarm detected!");
    if(a == 1){                      
      l=0;
      do{
        lcd.print("Get up!");
        tone(po, 500, 500);
        delay(500);
        noTone(po);
        lcd.clear();
        delay(500);
        l++;
        if(l==20){
          as=0;
          EEPROM.write(8, as);
        }
      }while(as==1);
    }
    if(a == 2){
      l=0;
      do{
        lcd.print("Get up!");
        tone(po, 500, 500);
        delay(100);
        noTone(po);
        lcd.clear();
        delay(100);
        l++;
        if(l==20){
          as=0;
          EEPROM.write(8, as);
        }
      }while(as==1);
    }
  }
}

void menu(){
  if((last_counter > counter) || (last_counter < counter)  || pushed){
    Ready = 1;
    if(submenu == 0){
    do{
      if(counter != lcounter){
        clr = 0;
        lcounter = counter;
      }
      if(0 < counter && counter < 4){
         if(clr == 0){
           lcd.clear();
           clr = 1;
         }
         lcd.setCursor(0,0);
         lcd.print("Welcome to menu");
         lcd.setCursor(0,1);
         lcd.write(1);
         lcd.print("Set Brightness");
         lcd.setCursor(1,2);
         lcd.print("Set Color");
         lcd.setCursor(1,3);
         lcd.print("Set Alarm Tone");
         page = 1;
         delay(200);
         if(digitalRead(10) == 0){
           tone(po, 500, 100);
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("Just a second!");
           delay(1000);
           submenu=1;
           counter=2;
         }
      }
      if(4 < counter && counter < 8){
        if(clr == 0){
          lcd.clear();
          clr = 1;
        }
        lcd.setCursor(0,0);
        lcd.print("Welcome to menu");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("Set Color");
        lcd.setCursor(1,2);
        lcd.print("Set Alarm Tone");
        lcd.setCursor(1,3);
        lcd.print("Set Alarm Time");
        page=2;
        delay(200);
        if(digitalRead(10) == 0){
          tone(po, 500, 100);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Just a second!");
          delay(1000);
          submenu=2;
          counter=2;
        }     
      }
      if(8 < counter && counter < 12){
        if(clr == 0){
          lcd.clear();
          clr = 1;
        }
        lcd.setCursor(0,0);
        lcd.print("Welcome to menu");
        lcd.setCursor(0,1);
        lcd.write(1);  
        lcd.print("Set Alarm Tone");
        lcd.setCursor(1,2);  
        lcd.print("Set Alarm Time");
        lcd.setCursor(1,3);
        lcd.print("Delete Alarm Time");
        page=3;
        delay(200);
        if(digitalRead(10) == 0){
          tone(po, 500, 100);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Just a second!");
          delay(1000);
          submenu=3;
          counter=2;
        }       
      }
      if(12 < counter && counter < 16){
        if(clr == 0){
          lcd.clear();
          clr = 1;
        }
        lcd.setCursor(0,0);
        lcd.print("Welcome to menu");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("Set Alarm Time");
        lcd.setCursor(1,2);
        lcd.print("Delete Alarm Time");
        lcd.setCursor(1,3);
        lcd.print("Use Stopwatch");
        page=4;
        delay(200);
        if(digitalRead(10) == 0){
          tone(po, 500, 100);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Just a second!");
          delay(1000);
          submenu=4;
          counter=2;
        }
     }
     if(16 < counter && counter < 20){
        if(clr == 0){
          lcd.clear();
          clr = 1;
        }
        lcd.setCursor(0,0);
        lcd.print("Welcome to menu");
        lcd.setCursor(0,1);
        lcd.write(1);
        lcd.print("Delete Alarm Time");
        lcd.setCursor(1,2);
        lcd.print("Use Stopwatch");
        lcd.setCursor(1,3);
        lcd.print("EXIT");
        page=5;
        delay(200);
        if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            submenu=5;
            counter=2;
          }
     }
     if(20 < counter && counter < 24){
        if(clr == 0){
          lcd.clear();
          clr = 1;
        }
        lcd.setCursor(0,0);
        lcd.print("Welcome to menu");
        lcd.setCursor(1,1);
        lcd.print("Delete Alarm Time");
        lcd.setCursor(0,2);
        lcd.write(1);
        lcd.print("Use Stopwatch");
        lcd.setCursor(1,3);
        lcd.print("EXIT");
        page=6;
        delay(200);
        if(digitalRead(10) == 0){
          tone(po, 500, 100);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Just a second!");
          delay(1000);
          submenu=6;
          counter=2;
        }
     }
     if(24 < counter && counter < 28){
        if(clr == 0){
          lcd.clear();
          clr = 1;
        }
        lcd.setCursor(0,0);
        lcd.print("Welcome to menu");
        lcd.setCursor(1,1);
        lcd.print("Delete Alarm Time");
        lcd.setCursor(1,2);
        lcd.print("Use Stopwatch");
        lcd.setCursor(0,3);
        lcd.write(1);
        lcd.print("EXIT");
        page=7;
        delay(200);
        if(digitalRead(10) == 0){
          tone(po, 500, 100);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Just a second!");
          delay(1000);
          submenu=0;
          counter=2;
          return;
        }
     }
    }while(submenu == 0);
   }
   if(submenu == 1){
    do{
        if(counter != lcounter){
          clr = 0;
          lcounter = counter;
        }
        if(0 < counter && counter < 4){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set brightness");
          lcd.setCursor(0,1);
          lcd.write(1);  
          lcd.print("25%");
          lcd.setCursor(1,2);  
          lcd.print("50%");
          lcd.setCursor(1,3);  
          lcd.print("75%");
          page=1;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            svj = 63;
            EEPROM.write(0, svj);
            submenu=0;
            counter=2;
          }
        }
        if(4 < counter && counter < 8){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set brightness");
          lcd.setCursor(0,1);
          lcd.write(1);  
          lcd.print("50%");
          lcd.setCursor(1,2);  
          lcd.print("75%");
          lcd.setCursor(1,3);  
          lcd.print("100%");
          page=2;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            svj = 127;
            EEPROM.write(0, svj);
            submenu=0;
            counter=2;
          }      
        }
        if(8 < counter && counter < 12){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set brightness");
          lcd.setCursor(0,1);
          lcd.write(1);  
          lcd.print("75%");
          lcd.setCursor(1,2);  
          lcd.print("100%");
          lcd.setCursor(1,3);  
          lcd.print("BACK");
          page=3;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            svj = 191;
            EEPROM.write(0, svj);
            submenu=0;
            counter=2;
          }      
        }
        if(12 < counter && counter < 16){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set brightness");
          lcd.setCursor(1,1);
          lcd.print("75%");
          lcd.setCursor(0,2);
          lcd.write(1);  
          lcd.print("100%");
          lcd.setCursor(1,3);  
          lcd.print("BACK");
          page=4;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            svj = 255;
            EEPROM.write(0, svj);
            submenu=0;
            counter=2;
          }        
        }
        if(16 < counter && counter < 20){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set brightness");
          lcd.setCursor(1,1);
          lcd.print("75%");
          lcd.setCursor(1,2);  
          lcd.print("100%");
          lcd.setCursor(0,3);
          lcd.write(1); 
          lcd.print("BACK");
          page=5;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            submenu=0;
            counter=2;
          }
        }
    }while(submenu == 1);
  }
  if(submenu == 2){
    do{  
        if(counter != lcounter){
          clr = 0;
          lcounter = counter;
        }
        if(0 < counter && counter < 4){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set Color");
          lcd.setCursor(0,1);
          lcd.write(1);  
          lcd.print("Red");
          lcd.setCursor(1,2);  
          lcd.print("Green");
          lcd.setCursor(1,3);  
          lcd.print("Blue");
          page=1;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            r = 255;
            g = 0;
            b = 0;
            EEPROM.write(1, r);
            EEPROM.write(2, g);
            EEPROM.write(3, b);
            submenu=0;
            counter=2;
          }
        }
        if(4 < counter && counter < 8){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set Color");
          lcd.setCursor(0,1);
          lcd.write(1);  
          lcd.print("Green");
          lcd.setCursor(1,2);  
          lcd.print("Blue");
          lcd.setCursor(1,3);  
          lcd.print("Orange");
          page=2;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            r = 0;
            g = 255;
            b = 0;
            EEPROM.write(1, r);
            EEPROM.write(2, g);
            EEPROM.write(3, b);
            submenu=0;
            counter=2;
          }      
        }
        if(8 < counter && counter < 12){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set Color");
          lcd.setCursor(0,1);
          lcd.write(1);  
          lcd.print("Blue");
          lcd.setCursor(1,2);  
          lcd.print("Orange");
          lcd.setCursor(1,3);  
          lcd.print("BACK");
          page=3;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            r = 0;
            g = 0;
            b = 255;
            EEPROM.write(1, r);
            EEPROM.write(2, g);
            EEPROM.write(3, b);
            submenu=0;
            counter=2;
          }      
        }
        if(12 < counter && counter < 16){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set Color");
          lcd.setCursor(1,1);
          lcd.print("Blue");
          lcd.setCursor(0,2);
          lcd.write(1);
          lcd.print("Orange");
          lcd.setCursor(1,3);  
          lcd.print("BACK");
          page=4;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            r = 255;
            g = 32;
            b = 0;
            EEPROM.write(1, r);
            EEPROM.write(2, g);
            EEPROM.write(3, b);
            submenu=0;
            counter=2;
          }        
        }
        if(16 < counter && counter < 20){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set Color");
          lcd.setCursor(1,1);
          lcd.print("Blue");
          lcd.setCursor(1,2);
          lcd.print("Orange");
          lcd.setCursor(0,3); 
          lcd.write(1); 
          lcd.print("BACK");
          page=5;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            submenu=0;
            counter=2;
            return;
          }
        }
    }while(submenu == 2);
  }
  if(submenu == 3){
    do{  
        if(counter != lcounter){
          clr = 0;
          lcounter = counter;
        }
        if(0 < counter && counter < 4){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set Alarm Tone");
          lcd.setCursor(0,1);
          lcd.write(1);  
          lcd.print("Alarm 1");
          lcd.setCursor(1,2);  
          lcd.print("Alarm 2");
          lcd.setCursor(1,3);  
          lcd.print("BACK");
          page=1;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            a=1;
            submenu=0;
            counter=2;
          }
        }
        if(4 < counter && counter < 8){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set Alarm Tone");
          lcd.setCursor(1,1);  
          lcd.print("Alarm 1");
          lcd.setCursor(0,2);
          lcd.write(1);  
          lcd.print("Alarm 2");
          lcd.setCursor(1,3);  
          lcd.print("BACK");
          page=2;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            a=2;
            submenu=0;
            counter=2;
          }      
        }
        if(8 < counter && counter < 12){
          if(clr == 0){
            lcd.clear();
            clr = 1;
          }
          lcd.setCursor(0,0);
          lcd.print("Set Alarm Tone");
          lcd.setCursor(1,1);
          lcd.print("Alarm 1");
          lcd.setCursor(1,2);
          lcd.print("Alarm 2");
          lcd.setCursor(0,3);
          lcd.write(1);
          lcd.print("BACK");
          page=3;
          delay(200);
          if(digitalRead(10) == 0){
            tone(po, 500, 100);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Just a second!");
            delay(1000);
            submenu=0;
            counter=2;
            return;
          }    
        }
    }while(submenu == 3);
  }
  if(submenu == 4){
    do{
        delay(1000);
        ch = 0;
        cm = 0;
        int sh=0;
        int sm=0;
        int sh1=0;
        int sh2=0;
        int sm1=0;
        int sm2=0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Set Alarm Time");
        if(sh1 == 0 && sh2 == 0){
            do{
              lcd.setCursor(0,1);
              lcd.print("Hours:");
              if(sh >= 0 && sh <= 9){
                lcd.setCursor(0,2);
                lcd.print("0");
                lcd.setCursor(1,2);
                lcd.print(sh);
              }
              delay(100);
              if(sh >= 10 && sh <=23){
                lcd.setCursor(0,2);
                lcd.print(sh);
              }
              delay(100);
              if(ch == 0){
                if(counter > last_counter){
                  sh++;
                  last_counter = counter;
                }
                delay(100);
                if(counter < last_counter){
                  sh--;
                  last_counter = counter;
                }
                delay(100);
                if(sh >= 24){
                  lcd.setCursor(0,2);
                  lcd.print("00");
                  sh = 0;
                }
                if(sh < 0){
                  sh = 23;
                }
                delay(100);
                if(digitalRead(10) == 0){
                  sh1 = sh/10;
                  sh2 = sh%10;
                  EEPROM.write(4, sh1);
                  EEPROM.write(5, sh2);
                  ch = 1;
                  last_counter = 0;
                  counter = 0;
                }       
              }
            }while(ch == 0);
          }
          if(sm1 == 0 && sm2 == 0){
            do{
              lcd.setCursor(0,1);
              lcd.print("Minutes:");
              if(sm >= 0 && sm<= 9){
                lcd.setCursor(0,2);
                lcd.print("0");
                lcd.setCursor(1,2);
                lcd.print(sm);
              }
              delay(100);
              if(sm >= 10 && sm <=59){
                lcd.setCursor(0,2);
                lcd.print(sm);
              }
              delay(100);
              if(cm == 0){
                if(counter > last_counter){
                  sm++;
                  last_counter = counter;
                }
                delay(100);
                if(counter < last_counter){
                  sm--;
                  last_counter = counter;
                }
                delay(100);
                if(sm >= 60){
                  lcd.setCursor(0,2);
                  lcd.print("00");
                  sm = 0;
                }
                if(sm < 0){
                  sm = 59;
                }
                if(digitalRead(10) == 0){
                  sm1 = sm/10;
                  sm2 = sm%10;
                  EEPROM.write(6, sm1);
                  EEPROM.write(7, sm2);
                  cm = 1;
                  last_counter = 0;
                  counter = 0;
                }       
              }
            }while(cm == 0);
          }
          as=1;
          EEPROM.write(8, as);
          lcd.setCursor(0,1);
          lcd.print("                    ");
          lcd.setCursor(0,2);
          lcd.print("Alarm Set!");
          delay(1000);
          submenu=0;
          counter=2;
    }while(submenu == 4);
  }
  if(submenu == 5){
    do{
      if(counter != lcounter){
          clr = 0;
          lcounter = counter;
      }
      if(0 < counter && counter < 5){
        if(clr == 0){
            lcd.clear();
            clr = 1;
        }
        lcd.setCursor(0,0);
        lcd.print("Welcome to menu");
        lcd.setCursor(0,1);
        lcd.print("Delete Alarm Time?");
        lcd.setCursor(0,2);
        lcd.write(1);
        lcd.print("YES");
        lcd.setCursor(1,3);
        lcd.print("NO");
        delay(300);
        if(digitalRead(10) == 0){
          tone(po, 500, 100);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Just a second!");
          delay(1000);
          lcd.clear();
          lcd.print("Alarm Time Deleted");
          delay(1000);
          as=0;
          submenu=0;
          counter=2;
          EEPROM.write(4,0);
          EEPROM.write(5,0);
          EEPROM.write(6,0);
          EEPROM.write(7,0);
          return;
        }
      }
      if(5 < counter && counter < 10){
        if(clr == 0){
            lcd.clear();
            clr = 1;
        }
        lcd.setCursor(0,0);
        lcd.print("Welcome to menu");
        lcd.setCursor(0,1);
        lcd.print("Delete Alarm Time?");
        lcd.setCursor(1,2);
        lcd.print("YES");
        lcd.setCursor(0,3);
        lcd.write(1);
        lcd.print("NO");
        delay(300);
        if(digitalRead(10) == 0){
          tone(po, 500, 100);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Just a second!");
          delay(1000);
          submenu=0;
          counter=2;
          return;
        }
      }
    }while(submenu == 5);
  }
  if(submenu == 6){
    int o = 0;
    int h = 0;
    int e = 10;
    int r[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    for(v=0; v<120; v++){
      leds[v] = CRGB(0, 0, 0);
      FastLED.show();
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Stopwatch starts in:");
    lcd.setCursor(0,1);
    lcd.print("3...");
    delay(1000);
    lcd.print("2...");
    delay(1000);
    lcd.print("1...");
    delay(1000);
    lcd.setCursor(0,2);
    lcd.print("Stopwatch started!");
    j1=103;
    j2=83;
    j3=63;
    j4=43;
    j5=23;
    j6=3;
    showing();
    do{
         o = millis();
         if(o - h > 1000){
            h = o;
            r[5]++;
            if(r[5] == 10){
              r[4]++;
              r[5] = 0;
            }
            if(r[4] == 6 && r[5] == 0){
              r[3]++;
              r[4] = 0;
              r[5] = 0;
            }
            if(r[3] == 10){
              r[2]++;
              r[3] = 0;
              r[4] = 0;
              r[5] = 0;
            }
            if(r[2] == 6 && r[3] == 0){
              r[1]++;
              r[2] = 0;
              r[3] = 0;
              r[4] = 0;
              r[5] = 0;
            }
            if(r[1] == 10){
              r[0]++;
              r[1] = 0;
            }
            if(r[0] == 2 && r[1] == 4){
              r[0] = 0;
              r[1] = 0;
              r[2] = 0;
              r[3] = 0;
              r[4] = 0;
              r[5] = 0;
              submenu = 0;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("One day has passed!");
              lcd.setCursor(0,1);
              lcd.print("Ending Stopwatch");
              delay(100);
              lcd.print(".");
              delay(100);
              lcd.print(".");
              delay(100);
              lcd.print(".");
              delay(100);
            }
            if(r[0] != r[6]){
              r[6] = r[0];
              //Hours d1
              if(r[0] == 0){
                  for(z1 = 100; z1 < 120; z1++){
                    leds[z1] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j1 = 103;
                  //0
              }
              if(r[0] == 1){
                  for(z1 = 100; z1 < 120; z1++){
                    leds[z1] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j1 = 104;
                  //1
              }
              if(r[0] == 2){
                  for(z1 = 100; z1 < 120; z1++){
                    leds[z1] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j1 = 102;
                  //2
              }
            }
            if(r[1] != r[7]){
              r[7] = r[1];
              //Hours d2
              if(r[0] == 0){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 83;
                  //0
              }
              if(r[1] == 1){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 84;
                  //1
              }
              if(r[1] == 2){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 82;
                  //2
              }
              if(r[1] == 3){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 80;
                  //3
              }
              if(r[1] == 4){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 88;
                  //4
              }
              if(r[1] == 5){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 86;
                  //5
              }
              if(r[1] == 6){
                  for(z2= 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 85;
                  //6
              }
              if(r[1] == 7){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 87;
                  //7
              }
              if(r[1] == 8){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 89;
                  //8
              }
              if(r[1] == 9){
                  for(z2 = 80; z2 < 100; z2++){
                    leds[z2] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j2 = 81;
                  //9
             }
            }
            if(r[2] != r[2]){
              r[8] = r[2];
             //Minutes d1
             if(r[2] == 0){
                  for(z3 = 60; z3 < 80; z3++){
                    leds[z3] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j3 = 63;
                  //0
              }
              if(r[2] == 1){
                  for(z3 = 60; z3 < 80; z3++){
                    leds[z3] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j3 = 64;
                  //1
              }
              if(r[2] == 2){
                  for(z3 = 60; z3 < 80; z3++){
                    leds[z3] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j3 = 62;
                  //2
              }
              if(r[2] == 3){
                  for(z3 = 60; z3 < 80; z3++){
                    leds[z3] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j3 = 60;
                  //3
              }
              if(r[2] == 4){
                  for(z3 = 60; z3 < 80; z3++){
                    leds[z3] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j3 = 68;
                  //4
              }
              if(r[2] == 5){
                  for(z3 = 60; z3 < 80; z3++){
                    leds[z3] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j3 = 66;
                  //5
              }
              if(r[2] == 6){
                  for(z3 = 60; z3 < 80; z3++){
                    leds[z3] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j3 = 65;
                  //6
              }
            }
            if(r[3] != r[9]){
              r[9] = r[3];
              //Minutes d2
              if(r[3] == 0){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 43;
                  //0
              }
              if(r[3] == 1){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 44;
                  //1
              }
              if(r[3] == 2){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 42;
                  //2
              }
              if(r[3] == 3){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4= 40;
                  //3
              }
              if(r[3] == 4){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 48;
                  //4
              }
              if(r[3] == 5){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 46;
                  //5
              }
              if(r[3] == 6){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 45;
                  //6
              }
              if(r[3] == 7){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 67;
                  //7
              }
              if(r[3] == 8){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 49;
                  //8
              }
              if(r[3] == 9){
                  for(z4 = 40; z4 < 60; z4++){
                    leds[z4] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j4 = 41;
                  //9
             }
            }
            if(r[4] != r[10]){
              r[10] = r[4];
             //Seconds d1
             if(r[4] == 0){
                  for(z5 = 20; z5 < 40; z5++){
                    leds[z5] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j5 = 23;
                  //0
              }
              if(r[4] == 1){
                  for(z5 = 20; z5 < 40; z5++){
                    leds[z5] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j5 = 24;
                  //1
              }
              if(r[4] == 2){
                  for(z5 = 20; z5 < 40; z5++){
                    leds[z5] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j5 = 22;
                  //2
              }
              if(r[4] == 3){
                  for(z5 = 20; z5 < 40; z5++){
                    leds[z5] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j5 = 20;
                  //3
              }
              if(r[4] == 4){
                  for(z5 = 20; z5 < 40; z5++){
                    leds[z5] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j5 = 28;
                  //4
              }
              if(r[4] == 5){
                  for(z5 = 20; z5 < 40; z5++){
                    leds[z5] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j5 = 26;
                  //5
              }
              if(r[4] == 6){
                  for(z5 = 20; z5 < 40; z5++){
                    leds[z5] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j5 = 25;
                  //6
              }
            }
            if(r[5] != r[11]){
              r[11] = r[5];
              //Seconds d2
              if(r[5] == 0){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 3;
                  //0
              }
              if(r[5] == 1){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 4;
                  //1
              }
              if(r[5] == 2){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 2;
                  //2
              }
              if(r[5] == 3){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 0;
                  //3
              }
              if(r[5] == 4){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 8;
                  //4
              }
              if(r[5] == 5){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 6;
                  //5
              }
              if(r[5] == 6){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 5;
                  //6
              }
              if(r[5] == 7){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 7;
                  //7
              }
              if(r[5] == 8){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 9;
                  //8
              }
              if(r[5] == 9){
                  for(z6 = 0; z6 < 20; z6++){
                    leds[z6] = CRGB(0, 0, 0);
                    FastLED.show();
                  }
                  j6 = 1;
                  //9
              }
            }
            if(digitalRead(10) == 0){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Stopping timer!");
              delay(600);
              lcd.setCursor(0,0);
              lcd.print("Go back to menu?");
              lcd.setCursor(1,1);
              lcd.print("YES");
              lcd.setCursor(1,2);
              lcd.print("NO");
              do{
                if(0 < counter && counter < 5){
                  lcd.setCursor(0,1);
                  lcd.write(1);
                  lcd.setCursor(0,2);
                  lcd.print(" ");
                  if(digitalRead(10) == 0){
                    tone(po, 500, 100);
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Just a second!");
                    delay(1000);
                    submenu=0;
                    counter=2;
                    cler = 0;
                    return;
                  }
                }
                if(5 < counter && counter < 10){
                  o = millis();
                  lcd.setCursor(0,1);
                  lcd.print(" ");
                  lcd.setCursor(0,2);
                  lcd.write(1);
                  if(digitalRead(10) == 0){
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("OK");
                    delay(1000);
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Going back in:");
                    lcd.setCursor(0,1);
                    lcd.print("10 seconds");
                    do{
                      o = millis();
                      if(o - h > 1000){
                        h = o;
                        e--;
                        if(e == 9){
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Going back in:");
                        }
                        lcd.setCursor(0,1);
                        lcd.print(e);
                        lcd.print(" seconds");
                        if(e == 0){
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Going back!");
                          submenu = 0;
                          counter = 2;
                          cler = 0;
                          return;
                        }
                      }
                    }while(submenu == 6);
                  }
                }
              }while(submenu == 6);
            }
           }
           showing();
      }while(submenu == 6);
    }
  }
}

void showing(){
  //H[0]
  leds[j1] = CRGB(r, g, b);
  FastLED.show();
  leds[j1+10] = CRGB(r, g, b);
  FastLED.show();
  //H[1]
  leds[j2] = CRGB(r, g, b);
  FastLED.show();
  leds[j2+10] = CRGB(r, g, b);
  FastLED.show();
  //M[0]
  leds[j3] = CRGB(r, g, b);
  FastLED.show();
  leds[j3+10] = CRGB(r, g, b);
  FastLED.show();
  //M[1]
  leds[j4] = CRGB(r, g, b);
  FastLED.show();
  leds[j4+10] = CRGB(r, g, b);
  FastLED.show();
  //S[0]
  leds[j5] = CRGB(r, g, b);
  FastLED.show();
  leds[j5+10] = CRGB(r, g, b);
  FastLED.show();
  //S[1]
  leds[j6] = CRGB(r, g, b);
  FastLED.show();
  leds[j6+10] = CRGB(r, g, b);
  FastLED.show();
}

ISR(PCINT0_vect){
  
  clk_State =   (PINB & B00000001);
  dt_State  =   (PINB & B00000010); 
  if (clk_State != Last_State){
     if (dt_State != clk_State){ 
       counter ++;
     }else{
       counter --;
     } 
   } 
   Last_State = clk_State;
}
