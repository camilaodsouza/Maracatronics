#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>

Enrf24 radio(PB_5, PC_4,PC_5);  //PD_1: CE   PC_4: CSN   PC_5:IRQ
const uint8_t rxaddr[]={0x11, 0x22, 0x33, 0x44, 0x55};

void Status_do_radio_serial(uint8_t status);

unsigned long resultado=0;
char msg[4];


void setup() {
  Serial.begin(38400);
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);           //MSB primeiro
  SPI.setModule(2);  //seleciona o modulo de SPI 2 do micro
  
  radio.begin();  //padrões 1Mbps, canal 0, max TX power
  radio.setRXaddress((void*)rxaddr);
  radio.enableRX();  //começa a receber
  Status_do_radio_serial(radio.radioState());

  delay(500);
  
  Status_do_radio_serial(radio.radioState());

  
}

void loop() {

  while (!radio.available(true));  //loop que aguarda o inicio da recepção
  
  if(radio.read(msg)){
    if (!strcmp(msg,"Move") || !strcmp(msg,"V 1:") || !strcmp(msg,"V 2:") || !strcmp(msg,"V 3:") || !strcmp(msg,"T 1:") || !strcmp(msg,"T 2:") || !strcmp(msg,"T 3:")){
      Serial.println(msg); 
    }
    else{
      for(int i=0;i<=3;i++){
        resultado=(resultado*256)+msg[i];
      }
      Serial.println(resultado);
    }
  }
  
}





void Status_do_radio_serial(uint8_t status)
{
  Serial.print("Enrf24 radio transceiver status: ");
  switch (status) {
    case ENRF24_STATE_NOTPRESENT:
      Serial.println("NO TRANSCEIVER PRESENT");
      break;

    case ENRF24_STATE_DEEPSLEEP:
      Serial.println("DEEP SLEEP <1uA power consumption");
      break;

    case ENRF24_STATE_IDLE:
      Serial.println("IDLE module powered up w/ oscillators running");
      break;

    case ENRF24_STATE_PTX:
      Serial.println("Actively Transmitting");
      break;

    case ENRF24_STATE_PRX:
      Serial.println("Receive Mode");
      break;

    default:
      Serial.println("UNKNOWN STATUS CODE");
  }
}
