// Demo_Landscape 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of the loadBitmap()-function.
//
// This program requires UTFT_tinyFAT, UTFT v2.41 or higher, 
// as well as tinyFAT v3.0 or higher.
//
// The image files must be present in the root folder 
// of a FAT16 formatted SDcard in the module cardslot.
//
// Display size:
//      480x320

// Definition of interrupt names
#include <avr/io.h>
// ISR interrupt service routine
#include <avr/interrupt.h>

#include <tinyFAT.h>
#include <UTFT.h>
#include <UTFT_tinyFAT.h>
#include <avr/pgmspace.h>

// Boolean values
#define TRUE 1
#define FALSE 0

// Communication pins
#define MEGA_TO_SOUNDBOARD_PIN 12   //Board activates at pull DOWN
#define MEGA_TO_POWERRELAY_PIN 0

// SD pins
//  uint8_t _SS   = 53;
//  uint8_t _MOSI = 51;
//  uint8_t _MISO = 50;
//  uint8_t _SCK  = 52;
  
// GSM pins
#define GSM_PW 7
// SERIAL 3
// #define GSM_RX 15
// #define GSM_TX 14

//RGB Strip 1 (knop_: D4(rood), D5(groen), D6(blauw)
#define rood1 4
#define groen1 6
#define blauw1 5

//RGB strip 2 (basis): D9(rood), D10(groen), D11(blauw)
#define Rood_2 9
#define Groen_2 11
#define Blauw_2 10

//RFID input: D13
#define RFID_IRQ_PIN 2
// SERIAL 2
//#define RFID_RX 17
//#define RFID_TX 16

#define Fadespeed 1200

// Interupts
#define INT_PIN1 18 //left
#define INT_PIN2 19 //top
#define INT_PIN3 20 //right
#define INT_PIN4 21 //bottom

/*********************************************/
// *GSM Variables:
/*********************************************/
   // Telefoon nummers van actieve spelleiders
const char nummer1[] = "AT+CMGS=\"+31621477770\""; //Vincent  V
const char nummer2[] = "AT+CMGS=\"+31652342086\""; //Gerard   V
//const char nummer3[] = "AT+CMGS=\"+31640295176\""; //Katinka  V
//const char nummer4[] = "AT+CMGS=\"+31600000000\""; //Joris    ?
//const char nummer5[] = "AT+CMGS=\"+31648758505\""; //Irma/Koen?
//const char nummer6[] = "AT+CMGS=\"+31641076444\""; //Rob      ?

const char* nummers[] = {nummer1,nummer2};//,nummer3,nummer4,nummer5,nummer6};                          // Bevat een lijst met de nummers van de huidige actieve spelleiders
const int aantalNummers = 2;

char actiefNummer[23];
char powerBuffer[40];
char powerDown[10];
int i_buff = 0;
int powerIndex = 0;

char bericht[200];
char RX_kar;
char ontvangen[100];
const char code[11] = "yso4jkd36f";
int send_message = 0;

/*********************************************/
// *Beeldscherm UTFT Variables:
// Declare which fonts we will be using
/*********************************************/
extern uint8_t SmallFont[];

UTFT           myGLCD(CTE32HR, 38, 39, 40, 41);   // Remember to change the model parameter to suit your display module!
UTFT_tinyFAT   myFiles(&myGLCD);

// List of filenames for pictures to display. 
char* menuFiles[]={"Menu01.RAW", "Menu02.RAW", "Menu03.RAW", "Menu04.RAW", "Menu05.RAW","","","","",""}; // 480x320
char* itemFiles[]={"item01.RAW", "item02.RAW", "item03.RAW","item04.RAW","","","","","",""}; // 240x180                               //INSTELLEN: Aantal tekens in de naam constant houden
char* itemNames[]={"Kistje van Verbinding","Kaarsen van Canitop","Armband van Gifdetectie","Amulet van Afwending","","","","","",""};      //INSTELLEN: Namen per item aangeven (op volgorde)
int itemCost[]={5,5,5,5,0,0,0,0,0,0};         //INSTELLEN: Itemkosten per Item instellen (op volgorde)
int itemActive[]={0,0,0,0,0,0,0,0,0,0};       //INSTELLEN: Items die aan staan (1) en die uit staan (0) bij begin van spel
int maxPutGrootte = 10;                      //INSTELLEN: Maximale putgrootte

int printLine;
boolean display_rendertime=false;  // Set this to true if you want the rendertime to be displayed after a picture is loaded
boolean display_filename=true;  // Set this to false to disable showing of filename

word res;
long sm, em;

/*********************************************/
// Loop variables:
/*********************************************/
int menuIndex;
int savedMenuIndex;
int menuIndexMax = 10;
int menuIndexRightEdge;
int itemIndexRightEdge;
int spelerIndex = 0;
int maxWellRadius = 80;
int totaleItemKosten = 0;
volatile bool skillUitvoeren = false;
volatile bool stuurBericht = false;
volatile bool toegang = false;
volatile bool updateMenu = false;
volatile bool subMenu = false;
volatile bool laadItemMenu = false;
volatile bool tekenHoofdMenu = true;
volatile bool tekenPutGrootte = false;

/*********************************************/
// *Interupt Variables:
// Mega2560 external interrupt int.0    int.1    int.2   int.3   int.4   int.5            
// pin                  2         3      21      20      19      18
/*********************************************/
int inter1 = 5;   //Button
int inter2 = 4;   //Button
int inter3 = 3;   //Button
int inter4 = 2;   //Button

/*********************************************/
// Timing variables:
/*********************************************/
unsigned long debounceTimeout = 500000; // Debounce time (500000 us = 500 ms = 0.5 s)
unsigned long skillTimeout = 300000000; // Time before a skill activates in seconds (300000000 us = 300000 ms = 300 s = 5 min)
unsigned long toegangTimeout = 10000000; // 10 sec voordat het scherm weer uit gaat
unsigned long rdyTimeout = 1000000; // 1 sec voordat er nog een tag gelezen wordt.
unsigned long lichtTimeout = 130000;
unsigned long debounceTimer = 0;
unsigned long toegangTimer = 0;
unsigned long rdyTimer = 0;
unsigned long lichtTimer = 0;
unsigned long skillTimer = 0;

/*********************************************/
// Communication variables:
/*********************************************/
int Mega_to_SoundBoard = 0;
int Mega_to_PowerRelay = 0;

/*********************************************/
// LED variables:
/*********************************************/
int updown = 1;
int rood1Val;
int groen1Val;
int blauw1Val;

int rood2Val;
int rood2ValUD = 1;
int groen2Val;
int groen2ValUD = 2;
int blauw2Val;
int blauw2ValUD = 1;

/*********************************************/
// RFID Functions:
/*********************************************/
void rfid_read(void);

// RFID Variables:
const int rfid_irq = 0;
uint8_t buff[14];
char c_buff[14];
uint8_t* buffer_at = buff;
uint8_t* buffer_end = buff + sizeof(buff);
volatile bool rdy = false;
uint32_t prevTag = 0;

// Lijst van geldige RFID tags
// Elke tag bestaat uit 8 cijfers
// Vervang 0 met het 8 cijferig getal van de tag
uint32_t tag01 = 13420748; //eerste 10 tags zijn voor constructeurs        // Speler01
uint32_t tag02 = 13400227;                                                 // Speler02
uint32_t tag03 = 13421553;                                                 // Speler03
uint32_t tag04 = 0;                                                 // Speler04
uint32_t tag05 = 0;                                                 // Speler05
uint32_t tag06 = 0;                                                 // Speler06
uint32_t tag07 = 0;                                                 // Speler07
uint32_t tag08 = 0;                                                 // Speler08
uint32_t tag09 = 0;                                                 // Speler09
uint32_t tag10 = 0;                                                 // Speler10
uint32_t tag11 = 0; //vervolgens zijn er nog 5 NPC Tags             // Figurant
uint32_t tag12 = 0;                                                 // Figurant
uint32_t tag13 = 0;                                                 // Figurant
uint32_t tag14 = 0;                                                 // Figurant
uint32_t tag15 = 0;                                                 // Figurant
uint32_t tag16 = 13426395; //laatste 5 tags zijn voor spelleiding   // GGooijer
uint32_t tag17 = 13421961;                                          // VvanHoek
uint32_t tag18 = 0;                                                 // Master03
uint32_t tag19 = 0;                                                 // Master04
uint32_t tag20 = 0;                                                 // Master05

// Array met alle tags en alle bijbehorende namen
uint32_t sleutelSet[] = {tag01,tag02,tag03,tag04,tag05,tag06,tag07,tag08,tag09,tag10,tag11,tag12,tag13,tag14,tag15,tag16,tag17,tag18,tag19,tag20};                    
char* naamSpeler[] = {"FrankW01","MathijsF","Speler03","Speler04","Speler05","Speler06","Speler07","Speler08","Speler09",
  "Speler10","Figurant","Figurant","Figurant","Figurant","Figurant","GGooijer","VvanHoek","Master03","Master04","Master05"};
int sleutelIndex = 0;

/*********************************************/
// PROGRAM
// ALGEMENE SETUP VOOR ARDUINO MEGA EN SUBSYSTEMS
/*********************************************/
void setup()
{
  // Start USB connectie
  Serial.begin(19200);
  randomSeed(analogRead(0));
  
  // Configuratie van LEDs
  pinMode(rood1, OUTPUT);
  pinMode(groen1, OUTPUT);
  pinMode(blauw1, OUTPUT);
  pinMode(Rood_2, OUTPUT);
  pinMode(Groen_2, OUTPUT);
  pinMode(Blauw_2, OUTPUT);
  rood1Val = 0;
  analogWrite(rood1, rood1Val);
  groen1Val = 0;
  analogWrite(groen1, groen1Val);
  blauw1Val = 0;
  analogWrite(blauw1, blauw1Val);
  rood2Val = 40;
  analogWrite(Rood_2,rood2Val);
  groen2Val = 1;
  analogWrite(Groen_2,groen2Val);
  blauw2Val = 150;
  analogWrite(Blauw_2,blauw2Val);

  // Configuratie van Comm lines
  pinMode(MEGA_TO_SOUNDBOARD_PIN,OUTPUT);
  pinMode(MEGA_TO_POWERRELAY_PIN,OUTPUT);
  digitalWrite(MEGA_TO_SOUNDBOARD_PIN,HIGH);                //Soundboard activates at pull DOWN
  digitalWrite(MEGA_TO_POWERRELAY_PIN,LOW);
  
  // Configuratie van RFID
  Serial.println("Start RFID");
  Serial2.begin(9600);
  attachInterrupt(rfid_irq,rfid_read,FALLING);
//  pinMode(RFID_IRQ_PIN,INPUT);
//  analogWrite(RFID_IRQ_PIN,HIGH);

  attachInterrupt(inter1, knopLinks, RISING);
  attachInterrupt(inter2, knopBoven, RISING);
  attachInterrupt(inter3, knopRechts, RISING);
  attachInterrupt(inter4, knopOnder, RISING);
  pinMode(INT_PIN1, INPUT);
  pinMode(INT_PIN2, INPUT);
  pinMode(INT_PIN3, INPUT);
  pinMode(INT_PIN4, INPUT);
  analogWrite(INT_PIN1,LOW);
  analogWrite(INT_PIN2,LOW);
  analogWrite(INT_PIN3,LOW);
  analogWrite(INT_PIN4,LOW);

  // Configuratie van afbeelding geheugen
  menuIndex = 0;
  for (int z = menuIndexMax-1; z>0; z--){
    if (menuFiles[z] == "Menu05.RAW")
      menuIndexRightEdge = z-1;
    if(itemFiles[z] != "")
      itemIndexRightEdge = z;
  }
  
  // Configuratie van LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();
  file.initFAT();
  myGLCD.setFont(SmallFont);

  // Startscherm
  myGLCD.setColor(0, 200, 0);
  myGLCD.fillRect(0, 0, 479, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 306, 479, 319);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 200, 0);
  myGLCD.print("* Emphebion Construteurs Aansluitpunt Bootscreen *", CENTER, 1);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255,255,0);
  myGLCD.print("<http://www.emphebion.nl/>", CENTER, 307);
  printLine = 20;
  myGLCD.setColor(0,0,0);
  myGLCD.drawRect(0,0,479,319);
  myGLCD.setColor(0,255,0);
  myGLCD.setBackColor(0,0,0);
  myGLCD.print("*********************************************",LEFT,printLine);
  
  // Configuratie van GSM
  printLine = printLine+15;
  myGLCD.print("* Scannen voor putten...",LEFT,printLine);
  delay(1000);
  myGLCD.print("DONE",RIGHT,printLine);
  printLine = printLine+15;
  myGLCD.print("* Verbinden met inspiratie paden...",LEFT,printLine);
  Serial.println("Start GSM");
  Serial3.begin(19200);       
  Serial3.print("AT+IPR=19200");                   // Stel de Baudrate in
  delay(100);
  while(Serial3.available()){
    Serial.write(Serial3.read());
  }
  delay(100);
  GSM_Shield_power();                                 // Zet het GSM-Shield aan
  Serial.println("Power responce:");
  delay(100);
  while(Serial3.available()){
    powerBuffer[i_buff] = (char)Serial3.read();
    if(powerBuffer[i_buff] == 'P'){
      powerIndex = i_buff;
    }
    i_buff++;
  }
  Serial.print(powerBuffer);
  for(int i = 0; i <= 9;i++){
    powerDown[i] = powerBuffer[powerIndex+i];
  }
  Serial.print(powerDown);
  if(powerDown[1] == 'O'){
    Serial.println("Power is down! REBOOT");
    GSM_Shield_power();
  }
  Serial3.print("AT+CPIN=\"0000\"");
  Serial3.write(0x0D);
  while(Serial3.available()){
    Serial.write(Serial3.read());
  }
  Serial3.print("AT+CGATT");
  Serial3.write(0x0D);
  delay(100);
  while(Serial3.available()){
    Serial.write(Serial3.read());
  }
  myGLCD.print("DONE",RIGHT,printLine);
  Serial.print("Size of nummers = ");
  Serial.println(aantalNummers);
  delay(1000);
  myGLCD.clrScr();
}

/*********************************************/
// MAIN LOOP CHECKING RFID
/*********************************************/
void loop(){
  // 3. Wacht op een RFID tag interrupt.
  if( rdy ){
    toegang = vergelijkSleutelMetSlot();
    toegangTimer = micros();
    
    if((micros() - rdyTimer) >= rdyTimeout)
      rdy = false;

    if(toegang){
      Serial.println("TOEGANG OK");
      if(tekenHoofdMenu){
        tekenMenu(menuFiles,0,0,0,480,320);
        tekenHoofdMenu = false;
        groen1Val = 0;
        rood1Val = 0;
        blauw1Val = 100;
        while(groen1Val < 150){
          analogWrite(rood1,rood1Val);
          analogWrite(groen1,groen1Val);
          analogWrite(blauw1,blauw1Val);
          blauw1Val = blauw1Val - 1;
          groen1Val = groen1Val + 1;
          delay(10);
        }
      }
    }
    else{
      tekenHoofdMenu = true;
      myGLCD.clrScr();
      skillTimer = micros();
      subMenu = false;
      updateMenu = false;
      skillUitvoeren = false;
      rood1Val = 0;
      groen1Val = 0;
      blauw1Val = 100;
      while(rood1Val < 150){
        analogWrite(rood1,rood1Val);
        analogWrite(groen1,groen1Val);
        analogWrite(blauw1,blauw1Val);
        blauw1Val = blauw1Val - 1;
        rood1Val = rood1Val + 1;
        delay(10);
      }
    }
  }

  if((micros() - toegangTimer) >= toegangTimeout){
    tekenHoofdMenu = true;
    toegang = false;
    myGLCD.clrScr();
    skillTimer = micros();
    subMenu = false;
    updateMenu = false;
    skillUitvoeren = false;
    rood1Val = 0;
    groen1Val = 0;
    blauw1Val = 100;
    analogWrite(rood1,rood1Val);
    analogWrite(groen1,groen1Val);
    analogWrite(blauw1,blauw1Val);
  }

  if(updateMenu){
    detachInterrupt(rfid_irq);
    digitalWrite(MEGA_TO_SOUNDBOARD_PIN,LOW);
    Serial.println("update Menu");
    if(subMenu){
      if(laadItemMenu){
        tekenMenu(menuFiles,4,0,0,480,320);
        laadItemMenu = false;
      }
      tekenMenu(itemFiles,menuIndex,120,75,240,180);
    }
    else{
      Serial.println("Next menuIndex = ");
      Serial.println(menuIndex);
      tekenMenu(menuFiles,menuIndex,0,0,480,320);
    }

    if(tekenPutGrootte){
      tekenPutVerbruik();
      tekenPutGrootte = false;
    }
    attachInterrupt(rfid_irq,rfid_read,FALLING);
    updateMenu = false;
  }

  if(skillUitvoeren){
    if((micros() - skillTimer) >= skillTimeout){                // Vergelijk of de speler genoeg tijd heeft besteed
      skillTimer = micros();                                    // Reset de timer
      skillUitvoeren = false;                                       // disable timer loop
      if(subMenu)
        activeerItem(menuIndex,sleutelIndex);
      else
        ontkoppelAlles();
      stuurBericht = true;
      analogWrite(rood1,255);
      analogWrite(groen1,255);
      analogWrite(blauw1,255);
    }
  }

  if(stuurBericht){
    Serial2.end();                                          // Stop RFID Serial connectie tijdens versturen van bericht -
    verstuurSMS(bericht,nummers,aantalNummers);             // - om interrupts te voorkomen (toe voegen: detach RFID_IRQ interrupt pin?)
    delay(5000);
    Serial2.begin(9600);                                    // Schakel RFID Serial weer in
    stuurBericht = false;
  }

  lichtAanpassen();
}

/*********************************************/
// CONTROLEER DE SLEUTEL
/*********************************************/
bool vergelijkSleutelMetSlot(){
  delay(140);
  uint32_t sleutelTag = leesSleutelTag(prevTag);
  prevTag = sleutelTag;
  bool sleutelFit = false;                    // Standaard return waarde als er geen match is
  if(sleutelTag){
    for(int i = 0; i <= 19; i = i+1){           // Vergelijk met alle (20) sleutels
      if (sleutelSet[i] == sleutelTag){
        sleutelIndex = i;                       // Sla de index op als er een match gevonden is
        Serial.println("Sleutel gevonden");
        sleutelFit = true;
      }
    }
  }
  else{
    if(toegang)
      sleutelFit = true;
  }
    
  return sleutelFit;
}

/*********************************************/
// LEES DE BESCHIKBARE SLEUTEL
/*********************************************/
uint32_t leesSleutelTag(uint32_t prevTag){

  // Stop overige interrupts
  detachInterrupt(inter1);
  detachInterrupt(inter2);
  detachInterrupt(inter3);
  detachInterrupt(inter4);
  // Vul de buffer
  buffer_at = buff;
  while ( buffer_at < buffer_end )
    *buffer_at++ = Serial2.read();

  // Reset de buffer pointer om het uitlezen makkelijker te maken
  buffer_at = buff;

  // Schakel interrupts weer in
  attachInterrupt(inter1, knopLinks, RISING);
  attachInterrupt(inter2, knopBoven, RISING);
  attachInterrupt(inter3, knopRechts, RISING);
  attachInterrupt(inter4, knopOnder, RISING);
    
  // Zet de buffer om in een 32 bit nummer.
  uint32_t result = 0;
    
  // Skip the preamble
  ++buffer_at;
  // Begin de sommatie van de Checksum
  uint8_t checksum = rfid_get_next();
  
  // De tag bestaat uit nog 4 waardes
  for(int i = 4; i >= 1; i--){
    // Pak de volgende waarde
    uint8_t value = rfid_get_next();
    
    // Voer de waarde toe aan result
    result <<= 8;                     // schuif eerste de vorige waarde 8 bit op
    result |= value;                  // Voeg de nieuwe waarde achter de eerdere waardes.
    
    // Xor de waarde met de checksum
    checksum ^= value;
  }
  
  // Vraag de checksum op
  uint8_t data_checksum = rfid_get_next();
  
  // Print het resultaat
  Serial.print("Reading: ");
  Serial.print(result);

  // Controleer de checksum
  if ( checksum == data_checksum ){
    Serial.println(" OK");
  }
  else{
    Serial.println(" CHECKSUM FAILED");
    int i = 10;
    while(i > 0){
      Serial2.flush();
      Serial2.read();
      i--;
    }
    result = prevTag;
  }
//  while(Serial2.available()){
//    Serial2.read();
//  }
  return result;
}

/*********************************************/
// TEKEN HET MENU EN SPEEL GELUID
/*********************************************/
void tekenMenu(char* files[],int index, int picoffset_x,int picoffset_y,int picsize_x, int picsize_y){
  int res=myFiles.loadBitmap(picoffset_x, picoffset_y, picsize_x, picsize_y, files[index]);     // Lees file van SD card
  if (res!=0){                                // Debug check of de file goed is geladen
//    if (res==0x10){
//      myGLCD.print("File not found...", 0, 0);
//      myGLCD.print(files[index], 0, 14);
//    }
//    else{
//      myGLCD.print("ERROR: ", 0, 0);
//      myGLCD.printNumI(res, 56, 0);
//    }
//    delay(3000);
    myGLCD.clrScr();
  }
  else{                                       // Teken afbeelding
    myGLCD.print(files[index], CENTER, myGLCD.getDisplayYSize());
    digitalWrite(MEGA_TO_SOUNDBOARD_PIN,HIGH);// Speel het geluid van een bladzijde af
    delay(1000);
  }
}

/*********************************************/
// TEKEN HET GEBRUIK RELATIE TOV PUTGROOTTE 
/*********************************************/
void tekenPutVerbruik(){
  int rHuidig = maxWellRadius;
  int rMin = 0;
  long aDiff;
//  char mess[30];
  for(int i = 0; i < menuIndexMax; i++){
    if(itemActive[i]){
      aDiff = (maxWellRadius*maxWellRadius*itemCost[i])/maxPutGrootte;
      rMin = sqrt((rHuidig*rHuidig) - aDiff);
      myGLCD.setColor(random(255), random(255), random(255));
      while(rHuidig>=rMin){
        myGLCD.drawCircle(myGLCD.getDisplayXSize()-240, myGLCD.getDisplayYSize()-160, rHuidig);
        rHuidig--;
      }
    }
  }
}
/*********************************************/
// SLUIT ITEM AAN (TENZIJ PUT VOL)
/*********************************************/
void activeerItem(int itemIndex, int spelerIndex){
  totaleItemKosten = totaleItemKosten + itemCost[itemIndex];
  if(totaleItemKosten > maxPutGrootte){                    // When there is an overflow remove EVERYTHING
    ontkoppelAlles();
    leegString(bericht);
    strcat(bericht,"Emphebion Mobile Dungeon System: ");
    strcat(bericht,naamSpeler[spelerIndex]);
    strcat(bericht," heeft zojuist teveel proberen aan te sluiten, alle items zijn ontkoppeld");
  }
  else{
    itemActive[itemIndex] = 1;
    leegString(bericht);
    strcat(bericht,"Emphebion Mobile Dungeon System: ");
    strcat(bericht,"Het item \"");
    strcat(bericht,itemNames[itemIndex]);
    strcat(bericht,"\" is zojuist door ");
    strcat(bericht,naamSpeler[spelerIndex]);
    strcat(bericht," geactiveerd");
  }
  menuIndex = savedMenuIndex;
  subMenu = false;
  updateMenu = true;
}

/*********************************************/
// SLUIT ALLE ITEMS AF VAN DE PUT
/*********************************************/
void ontkoppelAlles(){
  for(int i = 0; i < menuIndexMax;i++)
    itemActive[i] = 0;
  totaleItemKosten = 0;
  leegString(bericht);
  strcat(bericht,"Emphebion Mobile Dungeon System: ");
  strcat(bericht,naamSpeler[spelerIndex]);
  strcat(bericht," heeft zojuist alle items ontkoppeld");
}

/*********************************************/
// VERANDER DE BACKLIGHTING
/*********************************************/
void lichtAanpassen(){
  if(toegang){
    if((micros() - lichtTimer) >= lichtTimeout){ 
      if(!skillUitvoeren){
        if(groen1Val <= 1||groen1Val >= 55){
          updown = -updown;
        }
        groen1Val = groen1Val + updown;
        analogWrite(rood1,100);
        analogWrite(groen1,groen1Val);
        analogWrite(blauw1,10);
      }
      else{
        analogWrite(rood1,50);
        analogWrite(groen1,groen1Val);
        analogWrite(blauw1,blauw1Val);
        if(groen1Val > 254)
          updown = -1;
        if(blauw1Val > 254)
          updown = 1;
        groen1Val = groen1Val + updown;
        blauw1Val = blauw1Val - updown;
      }
    }
  }

  if(rood2Val >= 150){
    rood2ValUD = -1;
  }
  if (rood2Val <= 1){
    rood2ValUD = 1;
  }
  if(groen2Val >= 150){
    groen2ValUD = -1;
  }
  if (groen2Val <= 1){
    groen2ValUD = 1;
  }
  if(blauw2Val >= 150){
    blauw2ValUD = -1;
  }
  if (blauw2Val <= 1){
    blauw2ValUD = 1;
  }
  rood2Val = rood2Val + rood2ValUD;
  groen2Val = groen2Val + groen2ValUD;
  blauw2Val = blauw2Val + blauw2ValUD;
  analogWrite(Rood_2,rood2Val);
  analogWrite(Groen_2,groen2Val);
  analogWrite(Blauw_2,blauw2Val);
  delay(10);
}

/*********************************************/
// GSM Functions
/*********************************************/

/*********************************************/
// Verzeker dat de GSM module aan staat
/*********************************************/
void GSM_Shield_power(){
  digitalWrite(GSM_PW, LOW);
  delay(1000);
  digitalWrite(GSM_PW, HIGH);
  delay(2500);
  digitalWrite(GSM_PW, LOW);
  delay(3000);                                       // Geef het GSM-Shield de tijd een netwerk te vinden
}

/*********************************************/
// Functie voor het versturen van een SMS bericht:
// Input: bericht, nummers, aantal nummer om naar te versturen
/*********************************************/
void verstuurSMS(char bericht[], const char* nummers[], int aantalNummers){
  for(int i = 0; i < aantalNummers; i++){
    while(Serial3.available()){
      Serial.write(Serial3.read());
    }
    leegString(actiefNummer);
    strcpy(actiefNummer, nummers[i]);
    Serial3.print("AT+CMGF=1\r");
    Serial3.write(0x0D);
    delay(200);
    while(Serial3.available()){
      Serial.write(Serial3.read());
    }
    Serial3.print(actiefNummer);                      // AT commando om het bericht te sturen naar het gegeven telefoon nummer
    Serial3.write(0x0D);
    delay(200);
    while(Serial3.available()){
      Serial.write(Serial3.read());
    }
    Serial3.print(bericht);                        // Het bericht zelf, max 200 tekens (heeft geen AT commando meer nodig)
    delay(100);
    Serial3.write(26);                               // Sluit het vorige AT commando at met een ^z teken (ASCII Decimaal: 26)
    Serial3.println();                               // Geeft het GSM-Shield tijd om het bericht te versturen
    while(Serial3.available()){
      Serial.write(Serial3.read());
    }
    delay(10000);
    Serial.println(i);
  }
}

/*********************************************/
/* RFID Functies*/
// Zet twee Hex chars om in een byte en return dat
/*********************************************/
uint8_t rfid_get_next(void)
{
  // sscanf needs a 2-byte space to put the result but we
  // only need one byte.
  uint16_t result;
 
  // Working space to assemble each byte
  static char byte_chars[3];
  
  // Pull out one byte from this position in the stream
  snprintf(byte_chars,3,"%c%c",buffer_at[0],buffer_at[1]);
  sscanf(byte_chars,"%x",&result);
  buffer_at += 2;
  
  return static_cast<uint8_t>(result);
}

/*********************************************/
/* INTERUPT SERVICE ROUTINES                 */
/*********************************************/
void rfid_read(void){
  // Lees alleen een waarde als er niet nog een verwerkt word
  if ( !rdy ){
    rdy = true;                            // Zet de rdy flag
    rdyTimer = micros();
  }
}

// Voer de interrupt voor de linker knop uit
void knopLinks(void){
  if(toegang){
    if((micros() - debounceTimer) >= debounceTimeout){
      Serial.println("Left Button Pressed");
      debounceTimer = micros();
      menuIndex--;
      if(menuIndex <= 0){
        if(subMenu)
          menuIndex = itemIndexRightEdge;
        else
          menuIndex = menuIndexRightEdge;
      }
      updateMenu = true;
    }
  }
}

// Voer de interrupt voor de rechter knop uit
void knopRechts(void){
  if(toegang){
    if((micros() - debounceTimer) >= debounceTimeout){
      Serial.println("Right Button Pressed");
      debounceTimer = micros();
      menuIndex++;
      if(subMenu){
        if(itemFiles[menuIndex] == "")
          menuIndex = 0;
      }
      else{
        if(menuFiles[menuIndex] == "Menu05.RAW")
          menuIndex = 1;
      }

      if(menuIndex == menuIndexMax)
          menuIndex = 0;
          
      updateMenu = true;
    }
  }
}

// Voer de interrupt voor de bovenste knop uit
void knopBoven(void){
  if(toegang){
    if((micros() - debounceTimer) >= debounceTimeout){
    Serial.println("Top Button Pressed");
      if(subMenu){
        debounceTimer = micros();
        menuIndex = savedMenuIndex;
        subMenu = false;
        updateMenu = true;
      }
    }
  }

}

// Voer de interrupt voor de onderste knop uit
void knopOnder(void){
  if(toegang){
    if((micros() - debounceTimer) >= debounceTimeout){
      Serial.println("Bottom Button Pressed");
      debounceTimer = micros();
      if(subMenu){
        skillTimer = micros();
        skillUitvoeren = true;
        updateMenu = true;
        blauw1Val = 255;
        groen1Val = 0;
      }
      else{
        // Put Grootte bepalen sub_menu
        if(menuFiles[menuIndex] == "Menu02.RAW"){
          savedMenuIndex = menuIndex;
          menuIndex = 4;
          skillUitvoeren = false;
          tekenPutGrootte = true;
          updateMenu = true;
        }
        if(menuFiles[menuIndex] == "Menu03.RAW"){
          skillTimer = micros();
          skillUitvoeren = true;
          updateMenu = true;
          blauw1Val = 255;
          groen1Val = 0;
        }
        // Item Aansluiten sub_menu
        if(menuFiles[menuIndex] == "Menu04.RAW"){  //Go to sub menu for item select
          savedMenuIndex = menuIndex;
          menuIndex = 0;
          subMenu = true;
          updateMenu = true;
          laadItemMenu = true;
        }
      }
    }
  }
}

/*********************************************/
// Leeg een string
/*********************************************/
void leegString(char string[]){
  for (int i = 0; i < sizeof(string); i++){
    string[i] = (char)0;
  }
}
