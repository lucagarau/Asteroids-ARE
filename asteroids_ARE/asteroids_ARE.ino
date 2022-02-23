
#include <IRremote.h>
#include <IRremoteInt.h>

#include "LedControl.h"

//dichiarazione pin
#define BUZ 9
#define receiver 6

//procedura per il riavvio di Arduino
void (* Riavvia)(void)=0;

void left();
void right();

//dichiarazione oggetto matrice led
LedControl lc = LedControl(12, 11, 10, 1);

//dichiarazione IR Reciver
IRrecv irrecv(receiver);
decode_results results;

unsigned long delaytime = 1000; //delay di update dello schermo

//variabili per il punteggio
String scoreString;
int score;    //varaibile che contiene le decine e le unità del punteggio
int hundreds; //variabile che indica le centinaia nel punteggio
String scoreStr;
String scoreArr[] = {"" , "" , "" };

//variabili dei controlli

volatile unsigned long buttonPressed;
int buttonDelay = 150;  //delay pressione di un pulsante

volatile bool gameOver = false;
bool gameStart = false;

//variabili per il tempo
int tick;
int tickCounter = 1;
unsigned long now;

//variabili per il display
int ship;
int columns[] = {0, 0, 0, 0, 0, 0, 0, 0};
int randomInt;

/* Procedura che esegue un tema dopo il game over*/
void GOSong() {

    tone(BUZ, 523, 150.0);
    delay(166.666666667);
    delay(166.666666667);
    tone(BUZ, 391, 150.0);
    delay(166.666666667);
    delay(166.666666667);
    tone(BUZ, 329, 225.0);
    delay(250.0);
    tone(BUZ, 440, 172.5);
    delay(191.666666667);
    delay(2.77777777778);
    tone(BUZ, 493, 172.5);
    delay(191.666666667);
    delay(2.77777777778);
    tone(BUZ, 440, 175.0);
    delay(194.444444444);
    tone(BUZ, 415, 225.0);
    delay(250.0);
    tone(BUZ, 466, 225.0);
    delay(250.0);
    tone(BUZ, 415, 225.0);
    delay(250.0);
    tone(BUZ, 391, 112.5);
    delay(150.0);

    tone(BUZ, 349, 112.5);
    delay(150.0);
    tone(BUZ, 370, 30.0);
    delay(33.3333333333);
    tone(BUZ, 380, 600.0);
    delay(666.666666667);
}

/*procedura che riproduce un tema iniziale all'avvio del gioco*/
void startSong(){
   tone(BUZ, 370, 300);
    delay(450);
    tone(BUZ, 440, 200);
    delay(250);
    tone(BUZ, 554, 300);
    delay(450.0);
    tone(BUZ, 440, 300);
    delay(450);
    tone(BUZ, 370, 200);
    delay(250);
    tone(BUZ, 294, 240);
    delay(270);
    tone(BUZ, 294, 240);
    delay(270);
    tone(BUZ, 294, 240);
    delay(270);
}

void setup() {
  //inizializzazione punteggio e sensore ir
  // Serial.begin(9600);
  irrecv.enableIRIn();

  gameOver = false;
  hundreds = 0;
  scoreArr[0] = "";
  scoreArr[1] = "";
  scoreArr[2] = "";
  score = 0;
  tick = 300;
  tickCounter = 1;

  ship = 3;       //inizializzazione posizione del player
  now = millis(); //inizializzazione tempo di gioco
  buttonPressed = millis();   //inizializzazione tempo pressione pulsanti
  randomSeed(analogRead(15)); //seed generazione numeri casuali

  //inizializzazione schermo
  for (int i = 0; i < 8; i++)
    columns[i] = 0;


  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);

  //riproduzione musica iniziale
  startSong();
}

/*procedura che controlla il valore rilevato dal sensore ad infrarossi*/
void command()
{

  switch(results.value){
    case 0xFF10EF: //tasto 4
      left();
      break;
    
    case 0xFF5AA5: //tasto 6
      right();
      break;
    
    case 0xFF02FD: //tasto PLAY
      Riavvia();
      break;
  }
}

//procedura che gestisce il pulsante sinistro
void left()
{
  if (millis() - buttonPressed > buttonDelay) //controllo se il tasto non è stato premuto troppo velocemente
  {
    //spostamento del player verso sinistra
    if (ship != 0)
      ship--;
    else
      ship = 7;

    lc.clearDisplay(0);
    buttonPressed = millis();
  }

  //se il gioco è terminato ricomincia una nuova partita
  if (gameOver == true) {
    gameOver = false;
    setup();
  }
}


//procedura che gestisce il pulsante destro
void right()
{
  if (millis() - buttonPressed > buttonDelay) //controllo se il tasto non è stato premuto troppo velocemente
  {
    //spostamento del player verso destra
    if (ship != 7)
      ship++;
    else
      ship = 0;

    lc.clearDisplay(0);
    buttonPressed = millis();
  }

  //se il gioco è terminato ricomincia una nuova partita
  if (gameOver == true) {
    gameOver = false;
    setup();
  }
}


void loop() {
  //verifico se è stato rilevato un segnale
   if (irrecv.decode(&results)){
    command();//gestione del segnale
    irrecv.resume(); //ripristino del sensore IR
  }

  //aggiornamento del punteggio,viene aggiornato in base al tempo trascorso
  if (millis() - now > tick) {//controllo del tempo trascorso
    score++;

    now = millis();

    if (tickCounter == 1) {

      //aumento della difficoltà (aumento velocità ostacoli)
      tick = tick / 1.02;

      //creazione ostacoli
      randomInt = random(0, 8);

      if (columns[randomInt] == 0) {
        columns[randomInt] = 1;
      }
    }

    if (tickCounter != 4)
      tickCounter++;
    else
      tickCounter = 1;

    for (int i = 0; i < 8; i++) {

      //eliminazione ostacoli quando raggiungono il limite della matrice
      if (columns[i] == 10)
        columns[i] = 0;

      //scorrimento degli ostacoli verso il basso
      if (columns[i] != 0)
        columns[i]++;
    }

    lc.clearDisplay(0);
  }


  //grafica

  //stampa dell'astronave del player
  lc.setLed(0, 7, ship, true);

  //stampa degli asteroidi
  for (int i = 0; i < 8; i++) {
    if (columns[i] > 0)
      lc.setLed(0, columns[i] - 2, i, true);
    lc.setLed(0, columns[i] - 3, i, true);
  }

  //controllo delle collisioni
  if (columns[ship] == 10 or columns[ship] == 9) {
    lc.clearDisplay(0);

    //animazione dell'esplosione dopo una collisione
    for (int i = 0; i < 4; i++) {
      lc.setLed(0, 7, ship + i, true);
      lc.setLed(0, 7, ship - i, true);
      lc.setLed(0, 7 - i, ship + i, true);
      lc.setLed(0, 7 - i, ship - i, true);
      lc.setLed(0, 7 - 1.5 * i, ship, true);

      //generazione di un suono casuale per l'esplosione
      unsigned long time = millis();
      int randomSound = 1000;
      while (millis() - time <= 250)  {
        randomSound--;
        tone(9, random(randomSound, 1000));  
      }

      lc.clearDisplay(0);
      noTone(9);
    }

    delay(500);

    //visualizzazione punteggio

    scoreStr = String(score);

    scoreArr[0] = scoreStr.charAt(0);
    scoreArr[1] = scoreStr.charAt(1);
    scoreArr[2] = scoreStr.charAt(2);

    if (score < 100) {
      //stampa della prima cifra
      for (int i = 0; i < 2; i++) {
        if (scoreArr[i] == "0")
          draw0(1 + i * 4);
        if (scoreArr[i] == "1")
          draw1(1 + i * 4);
        if (scoreArr[i] == "2")
          draw2(1 + i * 4);
        if (scoreArr[i] == "3")
          draw3(1 + i * 4);
        if (scoreArr[i] == "4")
          draw4(1 + i * 4);
        if (scoreArr[i] == "5")
          draw5(1 + i * 4);
        if (scoreArr[i] == "6")
          draw6(1 + i * 4);
        if (scoreArr[i] == "7")
          draw7(1 + i * 4);
        if (scoreArr[i] == "8")
          draw8(1 + i * 4);
        if (scoreArr[i] == "9")
          draw9(1 + i * 4);
      }

    }
    else {

      //stampa della seconda cifra
      for (int i = 1; i < 3; i++) {
        if (scoreArr[i] == "0")
          draw0(1 + (i - 1) * 4);
        if (scoreArr[i] == "1")
          draw1(1 + (i - 1) * 4);
        if (scoreArr[i] == "2")
          draw2(1 + (i - 1) * 4);
        if (scoreArr[i] == "3")
          draw3(1 + (i - 1) * 4);
        if (scoreArr[i] == "4")
          draw4(1 + (i - 1) * 4);
        if (scoreArr[i] == "5")
          draw5(1 + (i - 1) * 4);
        if (scoreArr[i] == "6")
          draw6(1 + (i - 1) * 4);
        if (scoreArr[i] == "7")
          draw7(1 + (i - 1) * 4);
        if (scoreArr[i] == "8")
          draw8(1 + (i - 1) * 4);
        if (scoreArr[i] == "9")
          draw9(1 + (i - 1) * 4);
      }

      //conteggio delle centinaia del punteggio
      for (int i = 1; i < 10; i++) {
        if (scoreArr[0] == String(i))
          hundreds = i;
      }
      //stampa dell'indicatore delle centinaia
      for (int i = 1; i <= hundreds; i++) {
        lc.setLed(0, 0, i - 1, true);
        lc.setLed(0, 1, i - 1, true);
        delay(200);
      }

    }
    //segnalazione della terminazione del gioco
    gameOver = true;

    //riproduzione musica di game over
    GOSong();
    
    delay(1000);
    //ciclo finche non viene premuto un tasto per il riavvio
    while (gameOver == true) 
    {
       if (irrecv.decode(&results))
         {
          command();
          irrecv.resume();
          }
    }
  }

}


//funzioni di supporto grafico per visualizzare le cifre
void draw1(int position) {

  lc.setColumn(0, 0 + position, B00001000);
  lc.setColumn(0, 1 + position, B00011111);

}

void draw2(int position) {

  lc.setColumn(0, 0 + position, B00010111);
  lc.setColumn(0, 1 + position, B00010101);
  lc.setColumn(0, 2 + position, B00011101);
}
void draw3(int position) {

  lc.setColumn(0, 0 + position, B00010001);
  lc.setColumn(0, 1 + position, B00010101);
  lc.setColumn(0, 2 + position, B00011111);
}
void draw4(int position) {

  lc.setColumn(0, 0 + position, B00011100);
  lc.setColumn(0, 1 + position, B00000100);
  lc.setColumn(0, 2 + position, B00011111);
}
void draw5(int position) {

  lc.setColumn(0, 0 + position, B00011101);
  lc.setColumn(0, 1 + position, B00010101);
  lc.setColumn(0, 2 + position, B00010111);
}
void draw6(int position) {

  lc.setColumn(0, 0 + position, B00011111);
  lc.setColumn(0, 1 + position, B00010101);
  lc.setColumn(0, 2 + position, B00010111);
}
void draw7(int position) {

  lc.setColumn(0, 0 + position, B00010000);
  lc.setColumn(0, 1 + position, B00010011);
  lc.setColumn(0, 2 + position, B00011100);
}
void draw8(int position) {

  lc.setColumn(0, 0 + position, B00011111);
  lc.setColumn(0, 1 + position, B00010101);
  lc.setColumn(0, 2 + position, B00011111);
}
void draw9(int position) {

  lc.setColumn(0, 0 + position, B00011101);
  lc.setColumn(0, 1 + position, B00010101);
  lc.setColumn(0, 2 + position, B00011111);
}

void draw0(int position) {

  lc.setColumn(0, 0 + position, B00011111);
  lc.setColumn(0, 1 + position, B00010001);
  lc.setColumn(0, 2 + position, B00011111);
}
