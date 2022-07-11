#include<VL53L0X.h>
#include<GFButton.h>
#include <MD_TCS230.h>
#include "ColorMatch.h"

// Pin definitions
const int s0 = 4;  
const int s1 = 3;  
const int s2 = 41;  
const int s3 = 43;  
const int out = 47;
const int OE = 2;
int vermelho = 0;  
int verde = 0;  
int azul = 0;  

MD_TCS230 CS(s2, s3, s0, s1, OE);

// Global variables
colorData  rgb;

long previousMillis = 0;        // will store time between colors
int last_i = -1; // previos color index

//DECLARAÇÕES
VL53L0X sensor;
GFButton botaoSensor(A0);
bool sensorDistancia;
unsigned long instanteAnterior = 0;
unsigned long instanteAnterior2 = 0;
unsigned long var;
int campainha = 12;

void setup() {
  Serial.begin(9600); 
  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensorDistancia = false;
  pinMode(campainha, OUTPUT);

  // initialise color sensor
  CS.begin();
  // use calibration parameters from the header file:
  CS.setDarkCal(&sdBlack);
  CS.setWhiteCal(&sdWhite);
  
  //sensor de cor
  pinMode(s0,OUTPUT);  
  pinMode(s1,OUTPUT);  
  pinMode(s2,OUTPUT);  
  pinMode(s3,OUTPUT);  
  pinMode(out,INPUT);   
  digitalWrite(s0,HIGH);  
  digitalWrite(s1,HIGH);

  //codigo para medir longas distancias (sem ele só chega a 100 cm)
  sensor.setSignalRateLimit(0.1);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

  //codigo para habilitar alta acurácia
  //oscilação com esse codigo vai para +-6mm em longo alcance
  sensor.setMeasurementTimingBudget(200000);

  botaoSensor.setPressHandler(acionaSensorDistancia);
}

void loop() {
  unsigned long dist = sensor.readRangeSingleMillimeters(); //distancia está vindo em mm
  botaoSensor.process();
  
  unsigned long instanteAtual = millis();
  unsigned long instanteAtual2 = millis();

  if (instanteAtual2 > instanteAnterior2 + 250) {
    Serial.print("Distância: ");
    Serial.println(dist);
    instanteAnterior2 = instanteAtual2;
    var = dist * 3;
  }

  if (instanteAtual > instanteAnterior + var and sensorDistancia == true) {
    tone(campainha, 220, 50);
    instanteAnterior = instanteAtual;
  }


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
  else{
  Serial.println("  ");  
  }
  delay(900);    



  //Cores RGB
  static uint8_t  runState = 0;   
  static uint8_t  readState = 0;
  int i = -1;

  unsigned long currentMillis = millis();
 
  // Matching mode
  switch(runState) {
    case 0: // read a value
      readState = fsmReadValue(readState);
      if (readState == 0) runState++;
      break;

    case 1: // find the matching color
        i = colorMatch(&rgb);
       Serial.print(i); Serial.print(" ");
       Serial.print(last_i); Serial.print(" ");
       if (i!=last_i) { // new color?
          Serial.print(currentMillis-previousMillis);
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

void acionaSensorDistancia(GFButton& botaoSensor) {
  if (sensorDistancia == false) {
    sensorDistancia = true;
    Serial.println("Sensor ligado");
  }
  else {
    sensorDistancia = false;
    Serial.println("Sensor desligado");
  }
}

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

uint8_t fsmReadValue(uint8_t state) {
// Finite State Machine for reading a value from the sensor
// Current FSM state is passed in and returned
// Current reading stored in a global rgb buffer
  switch(state) {
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

  return(state);
}

uint8_t colorMatch(colorData *rgb) {
// Root mean square distance between the color and colors in the table.
// For a limited range of colors this method works ok using RGB
// We don't work out the square root or the mean as it has no effect on the 
// comparison for minimum. Square of the distance is used to ensure that 
// negative distances do not subtract from the total.

  int32_t   d;
  uint32_t  v, minV = 999999L;
  uint8_t   minI;

  for (uint8_t i=0; i<ARRAY_SIZE(ct); i++) {
    v = 0;
    for (uint8_t j=0; j<RGB_SIZE; j++) {
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

  return(minI);
}
