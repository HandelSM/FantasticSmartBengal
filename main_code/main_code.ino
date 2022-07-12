#include<GFButton.h>
#include <MD_TCS230.h>
#include <FreqCount.h>
#include "ColorMatch.h"
#include "player_mini.h"

#define BUSYPIN 7
#define RXPIN 12
#define TXPIN 13

// Pin definitions
const int s0 = 4;
const int s1 = 3;
const int s2 = 5;
const int s3 = 6;
const int out = 1;
const int OE = 2;
int vermelho = 0;
int verde = 0;
int azul = 0;

//Sensor de distancia
int trigPin = 11;
int echoPin = 10;

//Sensor de cor
MD_TCS230 CS(s2, s3, s0, s1, OE);

// Global variables
//colorData  rgb;
int estadoCampainha = LOW;
DFPlayerMini player;

long previousMillis = 0; // will store time between colors
int last_i = -1; // previos color index

GFButton button(8);
bool sensorDistancia = true;
unsigned long instanteAnterior = 0;
unsigned long instanteAnterior2 = 0;

unsigned long instanteAtual = millis();
unsigned long instanteAtual2 = millis();

int campainha = 9;
int state = 0;
//DFPlayerMini player;

unsigned long lastTone = millis();
int freq = 0;

void funcaoTone()
{
  if (freq == 0)
  {
    estadoCampainha = LOW;
    return;
  }

  if (instanteAtual - lastTone > freq)
  {
    lastTone = millis();
    estadoCampainha = !estadoCampainha;
  }
}

void acionaSensorDistancia()
{
  if (sensorDistancia == false) 
  {
    sensorDistancia = true;
    Serial.println("Sensor ligado");
  }
  else 
  {
    sensorDistancia = false;
    Serial.println("Sensor desligado");
  }
}


//Funções sensor cor

void color()
{
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  vermelho = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  digitalWrite(s3, HIGH);
  azul = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  digitalWrite(s2, HIGH);
  verde = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
}

uint8_t fsmReadValue(uint8_t state) 
{
  // Finite State Machine for reading a value from the sensor
  // Current FSM state is passed in and returned
  // Current reading stored in a global rgb buffer
  switch (state) {
    case 0: // start the reading process
      CS.read();
      state++;
      break;

    case 1: // wait for a read to complete
      if (CS.available()) {
        CS.getRGB(&rgb);
        state++;
      }
      break;

    default:  // reset fsm
      state = 0;
      break;
  }

  return (state);
}

uint8_t colorMatch(colorData *rgb) 
{
  // Root mean square distance between the color and colors in the table.
  // For a limited range of colors this method works ok using RGB
  // We don't work out the square root or the mean as it has no effect on the
  // comparison for minimum. Square of the distance is used to ensure that
  // negative distances do not subtract from the total.
  
  int32_t   d;
  uint32_t  v, minV = 999999L;
  uint8_t   minI;

  for (uint8_t i = 0; i < ARRAY_SIZE(ct); i++) 
  {
    v = 0;
    for (uint8_t j = 0; j < RGB_SIZE; j++) {
      d = ct[i].rgb.value[j] - rgb->value[j];
      v += (d * d);
    }
    if (v < minV) { // new best
      minV = v;
      minI = i;
    }
    if (v == 0)   // perfect match, no need to search more
      break;
  }

  return (minI);
}

String getColorCode(String cor)
{
  String code;
  
  if (cor.compareTo("WHITE") == 0)
  {
    code = "b";
  }
  else if (cor.compareTo("BLACK") == 0)
  {
    code = "p";
  }
  else if (cor.compareTo("YELLOW") == 0)
  {
    code = "a";
  }
  else if (cor.compareTo("ORANGE") == 0)
  {
    code = "l";
  }
  else if (cor.compareTo("RED") == 0)
  {
    code = "R";
  }
  else if (cor.compareTo("GREEN") == 0)
  {
    code = "G";
  }
  else if (cor.compareTo("BLUE") == 0)
  {
    code = "B";
  }
  else if (cor.compareTo("BROWN") == 0)
  {
    code = "?";
  }
}

void buttonFunction() //AÇÃO DO BOTÃO
{
  Serial.println("normal trigger");

  switch (state)
  {
    case 0: // sonar
      acionaSensorDistancia();
      break;

    case 1: //funcao detector de cor
      Serial.println("Lendo cor");
      
      String cor = getCor();
      Serial.println(a);
      String colorCode = getColorCode(cor);
      playSound(player, colorCode, 10);
      
      break;

    case 2: //funcao detector de objetos - fala a distancia do objeto (playsound)
      String distancia = String(getDist(), 2);
      Serial.print("Acionamento da distancia ");
      Serial.println(distancia);
      //playSound(player, distancia, 10);
      break;

    case 3:
      //desligado
      break;

    default:
      break;
  }
}

void buttonFunctionSwitch() //TROCA FUNCIONALIDADE
{
  Serial.println("switch mode");
  state++;
  
  if (state == 4)
  {
    state = 0;
  }

  switch(state)
  {
    case 0:
      Serial.println("Modo sonar");
      playSound(player, "S", 10);
      break;
    case 1:
      Serial.println("Modo cor");
      playSound(player, "C", 10);
      break;
    case 2:
      Serial.println("Modo distância");
      playSound(player, "D", 10);
      break;
    case 3:
      Serial.println("Modo desligado");
      playSound(player, "P", 10);
      break;  
  }
  
  Serial.println(state);
}

float getDist()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  long duration = pulseIn(echoPin, HIGH);
  float dist = (duration/2)/29.1;

  return dist;
}

void setup()
{
  Serial.begin(9600);
  player.init(BUSYPIN, TXPIN, RXPIN, NULL);

  
  //INICIO DO SETUP DO SENSOR VL
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensorDistancia = false;

  //codigo para medir longas distancias (sem ele só chega a 100 cm)
  sensor.setSignalRateLimit(0.1);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

  //codigo para habilitar alta acurácia
  //oscilação com esse codigo vai para +-6mm em longo alcance
  sensor.setMeasurementTimingBudget(200000);
  // FIM SETUP DO SENSOR VL
  

  //CAMPAINHA
  pinMode(campainha, OUTPUT);

  button.setPressHandler(buttonFunction);
  button.setClicksHandler(buttonFunctionSwitch);


  //INICIO SETUP SENSOR DE COR
  CS.begin();
  // CALIBRACAO DO SENSOR DE COR
  CS.setDarkCal(&sdBlack);
  CS.setWhiteCal(&sdWhite);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);
  //FIM SETUP SENSOR DE COR


  //INICIO SETUP SENSOR DISTANCIA
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop()
{
  button.process();

  //SENSOR DISTANCIA
  //unsigned long dist = ; //sensor.readRangeSingleMillimeters(); //distancia está vindo em mm

  instanteAtual = millis();
  instanteAtual2 = millis();

  if(state == 0) //state0 é sonar
  {
    if(sensorDistancia == true)
    {
      float dist = getDist();
      freq = dist * 10;
      funcaoTone();
    }
    else
    {
      estadoCampainha = LOW;
    }
    
    digitalWrite(campainha, estadoCampainha);
  }
  else if(state == 1)
  {
    
    //definição de cor
    color();
    Serial.print("   ");
    Serial.print(vermelho, DEC);
    Serial.print("   ");
    Serial.print(verde, DEC);
    Serial.print("   ");
    Serial.print(azul, DEC);
  
    if (vermelho < azul && verde > azul && vermelho < 35)
    {
      Serial.println("   Vermelho");
    }
    else if (azul < vermelho && azul < verde && verde < vermelho)
    {
      Serial.println("   Azul");
    }
  
    else if (vermelho > verde && azul > verde )
    {
      Serial.println("   Verde");
    }
    else
    {
      Serial.println("  ");
    }
    
  }

  delay(10);

  //Cores RGB
  
  static uint8_t  runState = 0;
  static uint8_t  readState = 0;
  i = -1;

  unsigned long currentMillis = millis();
V
  // Matching mode
  switch (runState) 
  {
    case 0: // read a value
      readState = fsmReadValue(readState);
      if (readState == 0) runState++;
      break;

    case 1: // find the matching color
      i = colorMatch(&rgb);
      Serial.print(i); Serial.print(" ");
      Serial.print(last_i); Serial.print(" ");
      if (i != last_i) { // new color?
        Serial.print(currentMillis - previousMillis);
        Serial.print(" ");
        previousMillis = currentMillis;
        last_i = i;
      }
      Serial.println(ct[i].name);
      runState++;
      break;

    default:
      runState = 0; // start again if we get here as something is wrong
  }

  
}
