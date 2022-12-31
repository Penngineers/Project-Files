#include <Arduino.h>
#ifdef USE_TINYUSB
#include <Adafruit_TinyUSB.h>
#endif

#ifndef PIN_BUZZER
#define PIN_BUZZER    A0
#endif

#define PIN_SDA 4
#define PIN_SCL 5

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define MAX_DEVICES 4
#define CS_PIN 13
#define CLK_PIN   10
#define DATA_PIN 11

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

uint8_t incr = 0;
int counter1 =100;
uint8_t const pin_buzzer = PIN_BUZZER;

#define NOTE_G4  392
#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_G5  784
#define WHOLE         2200       // Length of time in milliseconds of a whole note (i.e. a full bar).
#define HALF          WHOLE/2
#define QUARTER       HALF/2
#define EIGHTH        QUARTER/2
#define EIGHTH_TRIPLE QUARTER/3
#define SIXTEENTH     EIGHTH/2

#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>

#define  DEFAULT_I2C_ADDR1 0x3B
#define  DEFAULT_I2C_ADDR2 0x3A
#define  DEFAULT_I2C_ADDR3 0x3C

#define  SWITCH1  18  // PA01
#define  SWITCH2  19 // PA02
#define  SWITCH3 20 // PA03
#define  SWITCH4  2 // PA06
#define  PWM1  12  // PC00            //LEDS
#define  PWM2  13 // PC01
#define  PWM3  0 // PA04
#define  PWM4  1 // PA05

#define  SWITCH5  18  // PA01
#define  SWITCH6  19 // PA02
#define  SWITCH7  20 // PA03
#define  SWITCH8  2 // PA06
#define  PWM5  12  // PC00
#define  PWM6  13 // PC01
#define  PWM7  0 // PA04
#define  PWM8  1 // PA05

#define  SWITCH9  18  // PA01
#define  SWITCH10  19 // PA02
#define  SWITCH11  20 // PA03
#define  SWITCH12  2 // PA06
#define  PWM9  12  // PC00
#define  PWM10  13 // PC01
#define  PWM11  0 // PA04
#define  PWM12  1 // 

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#define TFT_CS         20
#define TFT_RST        19 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         2
#define TFT_MOSI 7  // Data out
#define TFT_SCLK 6  // Clock out
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

Adafruit_seesaw ss1;
Adafruit_seesaw ss2;
Adafruit_seesaw ss3;

int waitTime =20000000;
int currentScore1 = 0; 


void setup() {
  Serial.begin(115200);      // open the serial port at 9600 bps: 
  
  // Intialize the object
  myDisplay.begin();
  
  // Set the brightness of the display (0-15)
  myDisplay.setIntensity(8);

  // Clear the display
  myDisplay.displayClear();
  
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &i2c_program);
    i2c_program_init(pio, sm, offset, PIN_SDA, PIN_SCL);
 
    printf("\nPIO I2C Bus\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }
        // Perform a 0-byte read from the probe address. The read function
        // returns a negative result NAK'd any time other than the last data
        // byte. Skip over reserved addresses.
        int result;
        if (reserved_addr(addr))
            result = -1;
        else
            result = pio_i2c_read_blocking(pio, sm, addr, NULL, 0);

        printf(result < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
  
  while (!Serial) 
  delay(10);   // wait until serial port is opened

  Serial.println(F("Testing!"));
  
  if (!ss1.begin(DEFAULT_I2C_ADDR1)) {
    Serial.println(F("seesaw1 not found!"));
    while(1) 
    delay(10);
  }
  if (!ss2.begin(DEFAULT_I2C_ADDR2)) {
    Serial.println(F("seesaw2 not found!"));
    while(1) 
    delay(10);
  }  

  if (!ss3.begin(DEFAULT_I2C_ADDR3)) {
    Serial.println(F("seesaw3 not found!"));
    while(1) 
    delay(10);
  } 


   pinMode(pin_buzzer, OUTPUT);            //buzzer set as output
   digitalWrite(pin_buzzer, LOW);
  
  ss1.pinMode(SWITCH1, INPUT_PULLUP);     // buttons are set to high when not pressed and  change to LOW when closed (pressed)
  ss1.pinMode(SWITCH2, INPUT_PULLUP);
  ss1.pinMode(SWITCH3, INPUT_PULLUP);
  ss1.pinMode(SWITCH4, INPUT_PULLUP);
  ss1.pinMode(PWM1, OUTPUT);             //Leds set as output
  ss1.pinMode(PWM2, OUTPUT);
  ss1.pinMode(PWM3, OUTPUT);
  ss1.pinMode(PWM4, OUTPUT);

  ss2.pinMode(SWITCH5, INPUT_PULLUP);
  ss2.pinMode(SWITCH6, INPUT_PULLUP);
  ss2.pinMode(SWITCH7, INPUT_PULLUP);
  ss2.pinMode(SWITCH8, INPUT_PULLUP);

  ss2.pinMode(PWM5, OUTPUT);             //Leds set as output
  ss2.pinMode(PWM6, OUTPUT);
  ss2.pinMode(PWM7, OUTPUT);
  ss2.pinMode(PWM8, OUTPUT);

  ss3.pinMode(SWITCH9, INPUT_PULLUP);
  ss3.pinMode(SWITCH10, INPUT_PULLUP);
  ss3.pinMode(SWITCH11, INPUT_PULLUP);
  ss3.pinMode(SWITCH12, INPUT_PULLUP);

  ss3.pinMode(PWM9, OUTPUT);             //Leds set as output
  ss3.pinMode(PWM10, OUTPUT);
  ss3.pinMode(PWM11, OUTPUT);
  ss3.pinMode(PWM12, OUTPUT);

  randomSeed(analogRead(0));
}

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void playNote(int frequency, int duration, bool hold = false, bool measure = true) {
  (void) measure;

  if (hold) {
    
    tone(pin_buzzer, frequency, duration + duration / 32);
  } 
  else {
    
    tone(pin_buzzer, frequency, duration);
  }

  delay(duration + duration / 16);
}

void loop() {
  
  myDisplay.setTextAlignment(PA_CENTER);
 
  if(currentScore1 == 0){
  counter1=100;
  rp2040.idleOtherCore();  
  playNote(NOTE_G5, HALF, false);                    //start sound
  playNote(NOTE_E5, SIXTEENTH, false);
  playNote(NOTE_G5, EIGHTH, true, false);
  playNote(NOTE_E5, EIGHTH_TRIPLE, false, false);
  playNote(NOTE_C5, EIGHTH_TRIPLE, true, false);
  playNote(NOTE_G4, EIGHTH_TRIPLE, true, false);  

  myDisplay.print("WHACK");
  delay(650);
  myDisplay.print("IT");
  delay(650);
  myDisplay.print("FIRST");  
  delay(650);
  myDisplay.print("Ready?");    
  delay(650);

  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  testdrawtext1("Welcome to Whack-It-First!", ST77XX_RED);
  delay(2000);
  tft.fillScreen(ST77XX_BLACK);
  delay(2000);
  testdrawtext1("Press the pink button to  begin the game!", ST77XX_RED);
   
  while(ss3.digitalRead(SWITCH11)==1) {        //PINK button
    
    ss3.analogWrite(PWM9, incr);
    incr += 5;
    ss3.analogWrite(PWM10, incr);
    incr += 5;
    delay(100);
  }
   rp2040.resumeOtherCore();
  ss3.analogWrite(PWM9, 0);
  ss3.analogWrite(PWM10, 0);
  myDisplay.print(" GO !!!");
  tft.fillScreen(ST77XX_BLACK);
  delay(500);
  testdrawtext1("GAME HAS STARTED", ST77XX_RED); 
  delay(2000);
  }
  
  int newColor1 = pickNewColor1();
 

  ss1.digitalWrite(newColor1, HIGH);
  ss2.digitalWrite(newColor1, HIGH);        
                                                
  //check to see if whacked or not
  if (isWhacked1(newColor1) ) {
    Serial.println("You whacked the mole!");
    ss1.digitalWrite(newColor1, LOW);
    ss2.digitalWrite(newColor1, LOW); 

    currentScore1++;

    myDisplay.print(currentScore1);
    delay(200);                      //pause briefly before showing a new mole
    waitTime = 0.9*waitTime;
  } else {
    Serial.println("You failed to whack the mole! Game over.");
    gameover();
    waitTime = 20000000;
  } 

}

void setup1() {
  delay(1000);
}

void loop1() {

  
  counter1--;
  delay(500);
  Serial.print(counter1);
  Serial.print('\n');


   if(counter1==1)
   {
    
    counter1=100;

    gameover();

    waitTime = 20000000;

   }

}



boolean isWhacked1(int newColor1) {
  int i = 0;
  int chkButton;
  boolean whacked = false;
  boolean buttonPressed = false;

  Serial.print("Wait Time: ");
  Serial.print(waitTime);
  Serial.print('\n');
  while ((i < waitTime) and (!buttonPressed)) {
    
    chkButton = isButtonPressedcheck1();     //0 if nothing is pressed
    if (newColor1 == chkButton) {           //correct button is pressed
      whacked = true;
      buttonPressed = true;
    } else if (chkButton >= 0) { //incorrect button is pressed
      whacked = false;
      buttonPressed = true;
    } 
    i++;
    
    delay(1); //make each loop about 1ms
  }
  if (buttonPressed) {
    Serial.println("..button detected.");
  } else {
    Serial.println("..time out");
  }
  return whacked;
}


int isButtonPressedcheck1() {
  
    unsigned int buttonPressed = -1;
  

    if (! ss1.digitalRead(SWITCH1)) {
      ss1.digitalWrite(PWM1, HIGH);
      buttonPressed = PWM1;
    } 
  
    if (! ss1.digitalRead(SWITCH2)) {
    
      ss1.digitalWrite(PWM2, HIGH);
      buttonPressed = PWM2;
    } 
  
    if (! ss1.digitalRead(SWITCH3)) {
    
      ss1.digitalWrite(PWM3, HIGH);
      buttonPressed = PWM3;
    } 
  
    if (! ss1.digitalRead(SWITCH4)) {
      ss1.digitalWrite(PWM4, HIGH);
      buttonPressed = PWM4;
    } 
    
    if (! ss2.digitalRead(SWITCH5)) {
      ss2.digitalWrite(PWM5, HIGH);
      buttonPressed = PWM5;
    } 

    if (! ss2.digitalRead(SWITCH6)) {
      ss2.digitalWrite(PWM6, HIGH);
      buttonPressed = PWM6;
    } 

    if (! ss2.digitalRead(SWITCH7)) {
      ss2.digitalWrite(PWM7, HIGH);
      buttonPressed = PWM7;
    } 

    if (! ss2.digitalRead(SWITCH8)) {

      ss2.digitalWrite(PWM8, HIGH);
      buttonPressed = PWM8;
    } 

    if (! ss3.digitalRead(SWITCH9)) {
      ss3.digitalWrite(PWM9, HIGH);
      buttonPressed = PWM9;
    } 


    if (! ss3.digitalRead(SWITCH10)) {
      ss3.digitalWrite(PWM10, HIGH);
      buttonPressed = PWM10;
    }


    return buttonPressed;
}



void gameover() {
   counter1=100;
   myDisplay.print(currentScore1);
   for (int i=0; i <= 2; i++){
    ss1.digitalWrite(PWM1, HIGH);
    ss1.digitalWrite(PWM2, HIGH);
    ss1.digitalWrite(PWM3, HIGH);
    ss1.digitalWrite(PWM4, HIGH);

    ss2.digitalWrite(PWM5, HIGH);
    ss2.digitalWrite(PWM6, HIGH);
    ss2.digitalWrite(PWM7, HIGH);
    ss2.digitalWrite(PWM8, HIGH);

    ss3.digitalWrite(PWM9, HIGH);
    ss3.digitalWrite(PWM10, HIGH);   
       
     
    playNote(NOTE_G4, EIGHTH_TRIPLE, true, false);      // end sound
    playNote(NOTE_C5, EIGHTH_TRIPLE, true, false);
    playNote(NOTE_E5, EIGHTH_TRIPLE, false, false);
    playNote(NOTE_G5, EIGHTH, true, false);
    playNote(NOTE_E5, SIXTEENTH, false);
    playNote(NOTE_G5, HALF, false);   

    delay(200);

    ss1.digitalWrite(PWM1, LOW);
    ss1.digitalWrite(PWM2, LOW);
    ss1.digitalWrite(PWM3, LOW);
    ss1.digitalWrite(PWM4, LOW);

    ss2.digitalWrite(PWM5, LOW);
    ss2.digitalWrite(PWM6, LOW);
    ss2.digitalWrite(PWM7, LOW);
    ss2.digitalWrite(PWM8, LOW);

    ss3.digitalWrite(PWM9, LOW);
    ss3.digitalWrite(PWM10, LOW);  


   }
    Serial.println(currentScore1);
    tft.fillScreen(ST77XX_BLACK);
    testdrawtext2("Final Score of player1 is:",currentScore1, ST77XX_RED);
    delay(3000);
    tft.fillScreen(ST77XX_BLACK);
    testdrawtext1("The Game Ends !",ST77XX_RED);                              
    delay(5000);

    currentScore1 = 0;
}  


int pickNewColor1 () {
  
  int randomColor;
  int randomArray[10] = {PWM1, PWM2, PWM3, PWM4, PWM5, PWM6, PWM7, PWM8};
  int randIndex = (int)random(0, 10);
  randomColor = randomArray[randIndex];
  return randomColor;

}



void testdrawtext1(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testdrawtext2(char *text1, int value1, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text1);
  tft.print(value1);
  
}