#include <EEPROM.h>
/*
 * Sensor TRS para cambio A604 41TE
 * Funcional porém ainda existe erros nas trocas entrando e saindo do R que pode ser contornado repetindo a manobra.
 * Somente será possível a correção adicionando mais um relé independente para o controle da LUZ DE RÉ numa nova versão de placa.
 * Testado com arduino NANO com Atmega268 na placa TRS numa Gran Caravan 3.3 V6 LE 96.
 * Atte: Eng. Eduardo Blanco M.
 * 06-06-2024
 */
#define senPin  A7
#define temPin  A4
#define maxPin  A1
#define minPin  A0
#define t1  11
#define t3   9
#define t42  5
#define t41  2
#define numReads 10
#define serialPrintLimit 100
#define minAddress 0
#define maxAddress 3
bool maxBut = false, maxButOld = false, minBut = false, minButOld = false;
int maxim, minim, range, steps, tolerance;
double accum, tempAccum;
int readings[numReads];
int readFlag = 0;
int serialPrintFlag = 0;
int limPmin, limPmax, limRmin, limRmax, limNmin, limNmax, limDmin, limDmax, lim3min, lim3max, limLmin, limLmax;
int sensor;
int temp;
int select = 0;

void relayCtrl(){
  switch(select){
    case 10: //L
      digitalWrite(t1, HIGH);
      digitalWrite(t3, LOW);
      digitalWrite(t42, LOW);
      digitalWrite(t41, HIGH);
    break;
    case 9:
      digitalWrite(t1, HIGH);
      digitalWrite(t3,  LOW);
      digitalWrite(t42,HIGH);
      digitalWrite(t41,HIGH);
    break;
    case 8: //3
      digitalWrite(t1,  LOW);
      digitalWrite(t3,  LOW);
      digitalWrite(t42,HIGH);
      digitalWrite(t41,HIGH);
    break;
    case 7:
      digitalWrite(t1, HIGH);
      digitalWrite(t3,  LOW);
      digitalWrite(t42,HIGH);
      digitalWrite(t41,HIGH);
    break;
    case 6: //D
      digitalWrite(t1, HIGH);
      digitalWrite(t3, HIGH);
      digitalWrite(t42,HIGH);
      digitalWrite(t41,HIGH);
    break;
    case 5:
      digitalWrite(t1, HIGH);
      digitalWrite(t3, HIGH);
      digitalWrite(t42, LOW);
      digitalWrite(t41,HIGH);
    break;
    case 4: //N
      digitalWrite(t1, HIGH);
      digitalWrite(t3, HIGH);
      digitalWrite(t42, LOW);
      digitalWrite(t41, LOW);
    break;
    case 3:
      digitalWrite(t1,HIGH);
      digitalWrite(t3,HIGH);
      digitalWrite(t42,LOW);
      digitalWrite(t41,HIGH);
    break;
    case 2: //R
      digitalWrite(t1,LOW);
      digitalWrite(t3,HIGH);
      digitalWrite(t42,LOW);
      digitalWrite(t41,HIGH);
    break;
    case 1:
      digitalWrite(t1,LOW);
      digitalWrite(t3,LOW);
      digitalWrite(t42,LOW);
      digitalWrite(t41,HIGH);
    break;
    case 0: //P
      digitalWrite(t1,LOW);
      digitalWrite(t3,LOW);
      digitalWrite(t42,LOW);
      digitalWrite(t41,LOW);
    break;
  }
}
void EEPROMWriteInt(int address, int value) {
   byte hiByte = highByte(value);
   byte loByte = lowByte(value);
   EEPROM.write(address, hiByte);
   EEPROM.write(address + 1, loByte);   
}
int EEPROMReadInt(int address) {
   byte hiByte = EEPROM.read(address);
   byte loByte = EEPROM.read(address + 1);   
   return word(hiByte, loByte); 
}
void setup() {
  pinMode(t1, OUTPUT);
  pinMode(t3, OUTPUT);
  pinMode(t42,OUTPUT);
  pinMode(t41,OUTPUT);
  relayCtrl();
  Serial.begin(115200);
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);
  maxim = EEPROMReadInt(maxAddress);
  minim = EEPROMReadInt(minAddress);
  for(int i=0;i<numReads;i++){
    readings[i] = analogRead(senPin);
  }
  Serial.print("P,_,R,_,N,_,D,_,3,_,L,_,C/filtro,S/filtro,Temp");
}
void loop() {
  if(serialPrintFlag < serialPrintLimit){
    if(analogRead(maxPin) > 512){
      maxBut = true;
    }else{
      maxBut = false;
    }
    if(maxButOld > maxBut){
      EEPROMWriteInt(maxAddress, sensor);
      maxim = EEPROMReadInt(maxAddress);
    }
    maxButOld = maxBut;
    
    if(analogRead(minPin) > 512){
      minBut = true;
    }else{
      minBut = false;
    }
    if(minButOld > minBut){
      EEPROMWriteInt(minAddress, sensor);
      minim = EEPROMReadInt(minAddress);
    }
    minButOld = minBut;
    
    readings[readFlag] = analogRead(senPin);
    if(readFlag < numReads){
      readFlag++;
    } else{
      readFlag = 0;
    }
    accum = 0;
    for(int i=0;i<numReads;i++){
      accum = accum + readings[i];
    }
    sensor = accum/numReads;
    range = maxim - minim;
    steps = range/5;
    tolerance = steps/2.5;
    limPmin = minim + (steps*0) - tolerance;
    limPmax = minim + (steps*0) + tolerance;
    limRmin = minim + (steps*1) - tolerance;
    limRmax = minim + (steps*1) + tolerance;
    limNmin = minim + (steps*2) - tolerance;
    limNmax = minim + (steps*2) + tolerance;
    limDmin = minim + (steps*3) - tolerance;
    limDmax = minim + (steps*3) + tolerance;
    lim3min = minim + (steps*4) - tolerance;
    lim3max = minim + (steps*4) + tolerance;
    limLmin = minim + (steps*5) - tolerance;
    limLmax = minim + (steps*5) + tolerance;
    select = 0;
    if(sensor>limPmax){select++;}
    if(sensor>limRmin){select++;}
    if(sensor>limRmax){select++;}
    if(sensor>limNmin){select++;}
    if(sensor>limNmax){select++;}
    if(sensor>limDmin){select++;}
    if(sensor>limDmax){select++;}
    if(sensor>lim3min){select++;}
    if(sensor>lim3max){select++;}
    if(sensor>limLmin){select++;}
    relayCtrl();

    
    serialPrintFlag++;
  } else{ //FOR PLOTTER SERIAL
    Serial.print(limPmin);
    Serial.print(",");
    Serial.print(limPmax);
    Serial.print(",");
    Serial.print(limRmin);
    Serial.print(",");
    Serial.print(limRmax);
    Serial.print(",");
    Serial.print(limNmin);
    Serial.print(",");
    Serial.print(limNmax);
    Serial.print(",");
    Serial.print(limDmin);
    Serial.print(",");
    Serial.print(limDmax);
    Serial.print(",");
    Serial.print(lim3min);
    Serial.print(",");
    Serial.print(lim3max);
    Serial.print(",");
    Serial.print(limLmin);
    Serial.print(",");
    Serial.print(limLmax);
    Serial.print(",");
    Serial.print(sensor);
    Serial.print(",");
    Serial.print(analogRead(senPin));
    Serial.print(",");
    temp = temp + analogRead(temPin);
    temp = temp / 2;
    temp = temp * 0.48828125;
    Serial.println(temp + limPmin);
    serialPrintFlag = 0;
  }
  if(maxim < minim){
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
  /*if(sensor < minim || sensor > maxim){
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }*/
}
