#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI   u8g2  (U8G2_R0, 5, 22, 17); //cs=5, dc=22, res=17

////////////////////////////////////
//Variabler                       //
const int buttonAcornPin = 25;    //
const int buttonRandoPin = 36;    //
int buttonCase = 0;               //
int buttonAcorn = 0;              //
int buttonRando = 0;              //
////////////////////////////////////

/////////////////////////////////////////////////////////////
//Hvert pixel er on/off og har et antal naboer op til 8.   //
struct pixel {                                             //
  bool lys;                                                //
  int naboer;                                              //
} Pixels[128][64];                                         //
//Plads i array er koordinaterne til skærmen               //
/////////////////////////////////////////////////////////////

////////////////////////////////
//Tegnefunktion               //
void draw(int i, int j) {     //
  u8g2.drawPixel(i, j);       //
}                             //
////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Denne funktion tæller antal naboer for hver celle                           //
void nabotaeller() {                                                          //
  //For at undgå array[-1] og at læse uden for arrayet                        //
  //startes og sluttes loop 1 fra kanten af array.                            //
  for (int i = 1; i < 127; i++) {                                             //
    for (int j = 1; j < 63; j++) {                                            //
      Pixels[i][j].naboer = 0; //Starter med at tælle fra nul hver gang       //
      if (Pixels[i - 1][j - 1].lys == true) {                                 //
        Pixels[i][j].naboer++;                                                //
      }                                                                       //
      if (Pixels[i - 1][j].lys == true) {                                     //
        Pixels[i][j].naboer++;                                                //
      }                                                                       //
      if (Pixels[i - 1][j + 1].lys == true) {                                 //
        Pixels[i][j].naboer++;                                                //
      }                                                                       //
      if (Pixels[i][j - 1].lys == true) {                                     //
        Pixels[i][j].naboer++;                                                //
      }                                                                       //
      if (Pixels[i][j + 1].lys == true) {                                     //
        Pixels[i][j].naboer++;                                                //
      }                                                                       //
      if (Pixels[i + 1][j - 1].lys == true) {                                 //
        Pixels[i][j].naboer++;                                                //
      }                                                                       //
      if (Pixels[i + 1][j].lys == true) {                                     //
        Pixels[i][j].naboer++;                                                //
      }                                                                       //
      if (Pixels[i + 1][j + 1].lys == true) {                                 //
        Pixels[i][j].naboer++;                                                //
      }                                                                       //
    }                                                                         //
  }                                                                           //
}                                                                             //
//Ville gerne optimere dette om muligt                                        //
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//Denne funktion udfører alle reglerne baseret på antal naboer.             //
//Reglerne kan komprimeres til to udsagn:                                   //
//1. Hvis cellen er i live og har enten under 2 eller over 3 naboer dør den //
//2. Hvis cellen er død og har 3 naboer opstår der liv                      //
void regler() {                                                             //
  for (int i = 0; i < 128; i++) {                                           //
    for (int j = 0; j < 64; j++) {                                          //
      if (Pixels[i][j].lys == true) {                                       //
        if (Pixels[i][j].naboer < 2 || Pixels[i][j].naboer > 3) {           //
          Pixels[i][j].lys = false;                                         //
        }                                                                   //
      }                                                                     //
      else if (Pixels[i][j].lys == false && Pixels[i][j].naboer == 3) {    //
        Pixels[i][j].lys = true;                                            //
      }                                                                     //
    }                                                                       //
  }                                                                         //
}                                                                           //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
//Funktionen opdaterer skærmen.                 //
//For hvert koordinat checkes om det er tændt.  //
//Hvis det er tændt, tegnes det.                //
void opdater() {                                //
  for (int i = 0; i < 128; i++) {               //
    for (int j = 0; j < 64; j++) {              //
      if (Pixels[i][j].lys == true) {           //
        draw(i, j);                             //
      }                                         //
    }                                           //
  }                                             //
}                                               //
//////////////////////////////////////////////////

//////////////////////////////////////////
//Denne funktion tegner en kendt figur  //
//fundet af Charles Corderman.          //
void acorn() {                          //
  //Slukker først for alle celler       //
  for (int i = 0; i < 128; i++) {       //
    for (int j = 0; j < 64; j++) {      //
      Pixels[i][j].lys = false;         //
    }                                   //
  }                                     //
  //Dernæst tegnes figuren i arrayet    //
  //Centreret på øjemål                 //
  Pixels[62][32].lys = true;            //
  Pixels[63][32].lys = true;            //
  Pixels[63][34].lys = true;            //
  Pixels[65][33].lys = true;            //
  Pixels[66][32].lys = true;            //
  Pixels[67][32].lys = true;            //
  Pixels[68][32].lys = true;            //
}                                       //
//Figuren ser således ud:               //
//  O                                   //
//    O                                 //
// OO  OOO                              //
//////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//En funktion der tænder eller slukker hver celle tilfældigt.         //
//I stedet for 0 til 100 med skæl i midten kunne man bruge 0 til 2.   //
void rando() {                                                        //
  for (int i = 0; i < 128; i++) {                                     //
    for (int j = 0; j < 64; j++) {                                    //
      //tilfældige tal skal være nyt for hver koordinat               //
      int RTal = random(100);                                         //
      //random() giver her et tal fra 0 t.o.m. 99                     //
      //skællet er derfor mellem 49 og 50 (< kunne også have været >) //
      if (RTal < 50) {                                                //
        Pixels[i][j].lys = true;                                      //
      }                                                               //
      else {                                                          //
        Pixels[i][j].lys = false;                                     //
      }                                                               //
    }                                                                 //
  }                                                                   //
}                                                                     //
////////////////////////////////////////////////////////////////////////

void setup() {
  u8g2.begin(); //Starter skærmforbindelse
  pinMode(buttonAcornPin, INPUT); //Knappers pins sat til input
  pinMode(buttonRandoPin, INPUT);
}

void loop() {
  u8g2.firstPage();
  do {
    //Hvis en af knapperne bliver trykket,
    //vil det ændre hvilken case, der bliver valgt
    //Dette kan nemt udviddes med flere knapper
    buttonAcorn = digitalRead(buttonAcornPin);
    buttonRando = digitalRead(buttonRandoPin);
    if ((buttonAcorn == HIGH) && (buttonRando == LOW)) {
      buttonCase = 1;
    }
    else if ((buttonRando == HIGH) && (buttonAcorn == LOW)) {
      buttonCase = 2;
    }
    else {
      buttonCase = 0;
    }

    //Der checkes hvilken case, der er lig med buttonCase.
    //Det, der står i den case, bliver udført. Hvis ikke der står break;,
    //fortsættes der gennem den efterfølgende case.
    switch (buttonCase) {
      case 1:
        acorn();
        break;
      case 2:
        rando();
        break;
    }
    //Funktionsrækkefølge:
    nabotaeller();  //1. Tælle antal naboer
    regler();       //2. Udføre regler for celler
    opdater();      //3. Opdater display
    delay(200); //Hvor hurtigt der skal opdateres
  } while ( u8g2.nextPage() );
  delay(500);
}
