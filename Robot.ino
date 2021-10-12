/*Protocolo de comunicacao para 2018
 * [M][ID][Flags][V1][V2][V3][CRCHi][CRCLo]
 * [Flags]=b [XXXX HCCD]
 */

//bibliotecas do radio
#include <SPI.h>
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>

//biblioteca de manipulação do pwm
#include <wiring_analog.c>

#define ROBOT_ID  0x01   //trocar para cada agente
#define CHANNEL  10
#define DUTY 0.8

//mapeamento de pinos
#define Chute_PWM       PB_4    // pwm do chute
#define disparo         PD_6    // comando de chute 
#define disparo_chip    PC_7    //Comando de chip kick
#define drible          PE_5    //motor do drible
#define Speed_M1        PB_5    //modulo das velocidades dos motores
#define Speed_M2        PB_0    
#define Speed_M3        PB_1    
#define Direction_M1    PA_2    //Sentido de rotacao dos motores
#define Direction_M2    PA_3
#define Direction_M3    PA_4
#define Hall_M1         PB_6    //sensores Hall dos motores
#define Hall_M2         PB_7
#define Hall_M3         PE_0
#define Infrared        PE_4    //sensor de posse de bola
#define adc_bateria     PE_1    //A2
#define adc_chute       PE_3    //A0
#define Led_aux         PF_0


//bytes do protocolo
#define PROTOCOL_SIZE   8
#define START_BYTE      0x4D
#define HORUS_BIT       0x08
#define CHUTE_BIT       0x40
#define PASSE_BIT       0x20
#define DRIBLE_BIT      0x01
#define ID_BIT          0x07
#define ANTI_HORARIO    0x7F

#define PASS 80
#define KICK 170



  const byte _auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40};

/* Table of CRC values for low�order byte */
const byte _auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
  0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
  0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
  0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
  0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
  0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
  0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
  0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
  0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
  0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
  0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
  0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
  0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
  0x40};




//radio
Enrf24 radio(PD_1, PC_4, PC_5); //PD_1: CE   PC_4: CSN   PC_5:IRQ
const uint8_t rxaddr[] = {0x11, 0x22, 0x33, 0x44, 0x55};

//Funcoes utilizadas
void Stop(void); //Parar todas as ações do robô
void receivePackage(void);

//variaveis globais
unsigned long fail_safe=0;
bool kick_state=false, green_led_state=false, blue_led_state=false;



void setup() {
  pinMode(Chute_PWM, OUTPUT);
  pinMode(disparo, OUTPUT);
  pinMode(disparo_chip, OUTPUT);
  pinMode(drible, OUTPUT);
  pinMode(Speed_M1, OUTPUT);
  pinMode(Speed_M2, OUTPUT);
  pinMode(Speed_M3, OUTPUT);
  pinMode(Direction_M1, OUTPUT);
  pinMode(Direction_M2, OUTPUT);
  pinMode(Direction_M3, OUTPUT);
  pinMode(Hall_M1, INPUT);
  pinMode(Hall_M2, INPUT);
  pinMode(Hall_M3, INPUT);
  pinMode(Infrared, INPUT);
  pinMode(adc_bateria, INPUT);
  pinMode(adc_chute, INPUT);

  pinMode(Led_aux, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  Stop(); //para todos os motores para que não venha lixo na comunicação

  Serial.begin(38400);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);           //MSB primeiro
  SPI.setModule(3);  //seleciona o modulo de SPI 3 do micro

  radio.begin();  //padrões 1Mbps, canal 0, max TX power
  radio.setChannel(CHANNEL);
  radio.setRXaddress((void*)rxaddr);
  radio.enableRX();  //começa a receber

  
}

void loop() {
  /*
   * Adicionar funcoes para controle interno do robo
   */


  if (radio.radioState() != ENRF24_STATE_PRX) {  //radio nao esta presente
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    Stop();
    fail_safe = 0;
  }
  else{
    digitalWrite(RED_LED, LOW);
    if (radio.available(true)) { //recebimento de nova informacao
      digitalWrite(BLUE_LED, LOW);
      green_led_state=!green_led_state;
      digitalWrite(GREEN_LED, green_led_state);
      receivePackage();
    } //end recebimento
    
    //Se o robo nao receber nenhuma informacao por 1 segundo ele deve parar (Fail safe)->estado de espera de comandos
    else{     //estado de espera
      if(fail_safe==0){
        fail_safe=millis();
      }
  
    } //end else do estado de espera
  } //end recepcao

  if(millis()-fail_safe>=500){
    digitalWrite(GREEN_LED, LOW);
  }
  if(millis()-fail_safe>=1000){
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);
    Stop();
    fail_safe=0;
  }
   
  
} //end main loop



bool Boost(int volt){
  const int D=(int)(256*(1-DUTY));
  int valor_capacitor=analogRead(adc_chute);
  /*
  Divisor de tensão (1,5M e 22K):  200 volts -------- 2.644 volts   2.85                        pode ser 2M e 27k       1.97M   26.9k 
                          carga do capacitor ----- tensão na porta
                          
  ADC:   3 volts (máximo)-------4095 (12 bits de resolução)
         tensão na porta --------- valor de contagem
  
  
                       200*3*valor de contagem
  Carga do capacitor= ---------------------------
                              4095*2.891
  */
  
  float carga_capacitor=(0.05541609172471502274830565299552*valor_capacitor);  //2,6942=valor físico      

  
  if(carga_capacitor<=volt){
    PWMWrite(Chute_PWM,256,D,4000);
    delay(2);
    return false;
  }
  
  else{
    PWMWrite (Chute_PWM,256,256,4000);
    delay(2);
    return true;
  }
} //end boost




void receivePackage(void){
  char full_frame[PROTOCOL_SIZE];
  byte frame[PROTOCOL_SIZE-4];
  radio.read(full_frame,PROTOCOL_SIZE);

   for(int j=0;j<PROTOCOL_SIZE;j++){
      Serial.write(&full_frame[j],1);
   }
  
  //retornar valores lidos para debug
  //Serial.write(full_frame,PROTOCOL_SIZE);
  //Serial.println();

  
  //check sart byte and id
  if (full_frame[0]!=START_BYTE || full_frame[1]!=ROBOT_ID){
    return;
  }
  u_int crc =  ((full_frame[PROTOCOL_SIZE - 2] << 8) | full_frame[PROTOCOL_SIZE - 1]);
  
  for(int i=0;i<PROTOCOL_SIZE-4;i++){
    frame[i]=full_frame[i+2];
  }

  //checksum
  if(crc != calcCrc(full_frame[1],frame,PROTOCOL_SIZE-4)){
    return;
  }

  analyzeData(frame);
} //end receivePackage



void analyzeData(byte* frame){
  byte Velocidade[3] ={0,0,0};
  fail_safe=0;
  blue_led_state=!blue_led_state;
  digitalWrite(BLUE_LED,blue_led_state);
  //Flags

  //drible
  if(frame[0]&DRIBLE_BIT){
    digitalWrite(drible,HIGH);
  }else{
    digitalWrite(drible,LOW);
  }
  //passe
  if(frame[0]&PASSE_BIT){
    if(kick_state && digitalRead(Infrared)){
        //passe a bola
    }
  }

  //chute
  if(frame[0]&CHUTE_BIT){
     if(kick_state && digitalRead(Infrared)){
        //chute a bola
    }
  }

  //enviar ao supervisorio
  if(frame[0]&HORUS_BIT){
    //cod para enviar as informacoes
  }

  //Roda 1
  if(frame[1]<=ANTI_HORARIO){
    digitalWrite(Direction_M1, LOW);
    Velocidade[1] = 2*frame[1];
  }
  else{
    digitalWrite(Direction_M1, HIGH);
    Velocidade[1] = 2*(frame[1] & ANTI_HORARIO);
  }
  analogWrite(Speed_M1,Velocidade[1]);
  delay(2);

  //Roda 2
   if(frame[2]<=ANTI_HORARIO){
    digitalWrite(Direction_M2, LOW);
    Velocidade[2] = 2*frame[2];
  }
  else{
    digitalWrite(Direction_M2, HIGH);
    Velocidade[2] = 2*(frame[2] & ANTI_HORARIO);
  }
  analogWrite(Speed_M2,Velocidade[2]);
  delay(2);

  //Roda 3
   if(frame[3]<=ANTI_HORARIO){
    digitalWrite(Direction_M3, LOW);
    Velocidade[3] = 2*frame[3];
  }
  else{
    digitalWrite(Direction_M3, HIGH);
    Velocidade[3] = 2*(frame[3] & ANTI_HORARIO);
  }
  analogWrite(Speed_M3,Velocidade[3]);
  delay(2);
  
} //end analyzeData



word calcCrc(byte address, byte* pduFrame, byte pduLen) {
    byte CRCHi = 0xFF, CRCLo = 0x0FF, Index;

    Index = CRCHi ^ address;
    CRCHi = CRCLo ^ _auchCRCHi[Index];
    CRCLo = _auchCRCLo[Index];

    while (pduLen--) {
      Index = CRCHi ^ *pduFrame++;
      CRCHi = CRCLo ^ _auchCRCHi[Index];
      CRCLo = _auchCRCLo[Index];
    }

    return (CRCHi << 8) | CRCLo;
} //end calcCrc 

  

void Stop(void) {
  //********parar as rodas desativando os enables******
  PWMWrite(Speed_M1, 128, 0, 1000);
  delay(2);
  PWMWrite(Speed_M2, 128, 0, 1000);
  delay(2);
  PWMWrite(Speed_M3, 128, 0, 1000);
  delay(2);
  
  //**********não chutar nem driblar************
  digitalWrite(disparo, HIGH);
  digitalWrite(disparo_chip, HIGH);
  digitalWrite(drible, LOW);
} //end Stop

