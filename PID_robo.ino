#include <wiring_analog.c>   //permite mudar a frequência de um pwm
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>


//***************MAPEAMENTO DE PINOS*******************
#define Speed_M1        PB_5
#define Speed_M2        PB_0
#define Speed_M3        PB_1
#define Motor1_sentido  PA_2
#define Motor2_sentido  PA_3
#define Motor3_sentido  PA_4
#define Motor1_hall     PB_6
#define Motor2_hall     PB_7
#define Motor3_hall     PE_0


//**************VARIÁVEIS GLOBAIS*****************
unsigned long timer;
unsigned long delta_t_1[660], delta_t_2[660], delta_t_3[660];
unsigned long atual1[660], atual2[660], atual3[660];
unsigned long t_hall1, t_hall2, t_hall3;
int i=0,j=0,k=0,l=0,m=0,n=0;
char resultado[4];
int s_hall1, s_hall2, s_hall3;


//*****************RADIO****************
Enrf24 radio(PD_1, PC_4, PC_5); //PD_1: CE   PC_4: CSN   PC_5:IRQ
const uint8_t txaddr[] = {0x11, 0x22, 0x33, 0x44, 0x55};





void setup() {

//************DEFINIÇÃO DOS PINOS*************
  pinMode(Motor1_sentido, OUTPUT);
  pinMode(Motor2_sentido, OUTPUT);
  pinMode(Motor3_sentido, OUTPUT);
  pinMode(Motor1_hall, INPUT);
  pinMode(Motor2_hall, INPUT);
  pinMode(Motor3_hall, INPUT);
  pinMode(RED_LED,OUTPUT);
  pinMode(GREEN_LED,OUTPUT);
  pinMode(BLUE_LED,OUTPUT);

//************INICIALIZAÇÃO DO RADIO****************
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);           //MSB primeiro
  SPI.setModule(3);         //seleciona o modulo de SPI 3 do micro

  radio.begin();  //padrões 1Mbps, canal 0, max TX power
  radio.setTXaddress((void*)txaddr);


//************PARAR AS RODAS*****************
  PWMWrite(Speed_M1, 128, 0, 1000);
  PWMWrite(Speed_M2, 128, 0, 1000);
  PWMWrite(Speed_M3, 128, 0, 1000);

//********GARANTIR QUE O RADIO ESTÁ PRONTO PARA TRANSMITIR**********
 while(radio.radioState() == ENRF24_STATE_NOTPRESENT){
    digitalWrite(RED_LED,HIGH);
  }
  if(radio.radioState()==ENRF24_STATE_IDLE){
    digitalWrite(GREEN_LED,HIGH);
    digitalWrite(RED_LED,LOW);
  }



//************ESTADOS INICIAIS********* 
  s_hall1=digitalRead(Motor1_hall);
  s_hall2=digitalRead(Motor2_hall);
  s_hall3=digitalRead(Motor3_hall);
  timer=millis();
  i=0;

}

void loop() {

//*********DEFINIR A DIREÇÃO DO ROBÔ BASEADO EM QUAL CICLO DE ANÁLISE ELE SE ENCONTRA
  if(i==1){
     digitalWrite(Motor1_sentido,LOW);
     digitalWrite(Motor3_sentido,LOW);
     PWMWrite(Speed_M1,128,128,1000);   //PWMWrite(pin, resolution, duty, frequency);
     PWMWrite(Speed_M3,128,128,1000);
  
  }
  if(i==2){
     digitalWrite(Motor1_sentido,HIGH);
     digitalWrite(Motor3_sentido,HIGH);
     PWMWrite(Speed_M1,128,128,1000);   //PWMWrite(pin, resolution, duty, frequency);
     PWMWrite(Speed_M3,128,128,1000);
  
  }
  if(i==3){
     digitalWrite(Motor1_sentido,HIGH);
     digitalWrite(Motor2_sentido,LOW);
     digitalWrite(Motor3_sentido,LOW);
     PWMWrite(Speed_M1,128,64,1000);   //PWMWrite(pin, resolution, duty, frequency);
     PWMWrite(Speed_M2,128,128,1000);
     PWMWrite(Speed_M3,128,64,1000);
  
  }
  if(i==4){
     digitalWrite(Motor1_sentido,LOW);
     digitalWrite(Motor2_sentido,HIGH);
     digitalWrite(Motor3_sentido,HIGH);
     PWMWrite(Speed_M1,128,64,1000);   //PWMWrite(pin, resolution, duty, frequency);
     PWMWrite(Speed_M2,128,128,1000);
     PWMWrite(Speed_M3,128,64,1000);
  
  }


//***********AQUISIÇÃO DE DADOS REFERENTES AVELOCIDADE E TEMPO*******************
  if(( (s_hall1==1) && (digitalRead(Motor1_hall)==0) )||( (s_hall1==0) && (digitalRead(Motor1_hall)==1))){
    atual1[l]=micros();
    delta_t_1[l]=atual1[l]-t_hall1;  // o delta é utilizado para calcular a velocidade
    t_hall1=atual1[l];               // o tempo do hall refere-se ao momento em que a velocidade foi adquirida
    s_hall1=digitalRead(Motor1_hall); 
    l++;
  }
  if(( (s_hall2==1) && (digitalRead(Motor2_hall)==0) )||( (s_hall2==0) && (digitalRead(Motor2_hall)==1))){
    atual2[m]=micros();
    delta_t_2[m]=atual2[m]-t_hall2;
    t_hall2=atual2[m];
    s_hall2=digitalRead(Motor2_hall); 
    m++;
  }
  if(( (s_hall3==1) && (digitalRead(Motor3_hall)==0) )||( (s_hall3==0) && (digitalRead(Motor3_hall)==1))){
    atual3[n]=micros();
    delta_t_3[n]=atual3[n]-t_hall3;
    t_hall3=atual3[n];
    s_hall3=digitalRead(Motor3_hall); 
    n++;
  }



//********CADA CICLO DE ANALISE DURA 2 SEGUNDOS E NO FIM DE CADA UM O ROBO PARA, ENVIA OS DADOS COLETADOS E ESPERA 5 SEGUNDOS PARA INICIAR O PRÓXIMO CICLO******************** 
  if(millis()-timer>=2000){
    PWMWrite(Speed_M1,128,0,1000);   //PWMWrite(pin, resolution, duty, frequency);
    PWMWrite(Speed_M2,128,0,1000);
    PWMWrite(Speed_M3,128,0,1000);



   radio.print("Move");
   radio.flush();  //forçar transmissão
   radio.print("V 1:");
   radio.flush();
   for(j=0;j<660;j++){
      for(k=3;k>=0;k--){
        resultado[k]=delta_t_1[j]%256;  //transforma os tempos coletados em 4 bytes separados para poderem ser enviados pelo radio
        delta_t_1[j]=delta_t_1[j]/256;
      }
      radio.print(resultado);
      radio.flush();
   }

   radio.print("T 1:");
   radio.flush();
   for(j=0;j<660;j++){
      for(k=3;k>=0;k--){
        resultado[k]=atual1[j]%256;
        atual1[j]=atual1[j]/256;
      }
      radio.print(resultado);
      radio.flush();
   }
   radio.print("V 2:");
   radio.flush();
   for(j=0;j<660;j++){
      for(k=3;k>=0;k--){
        resultado[k]=delta_t_2[j]%256;
        delta_t_2[j]=delta_t_2[j]/256;
      }
      radio.print(resultado);
      radio.flush();
   }

   radio.print("T 2:");
   radio.flush();
   for(j=0;j<660;j++){
      for(k=3;k>=0;k--){
        resultado[k]=atual2[j]%256;
        atual2[j]=atual2[j]/256;
      }
      radio.print(resultado);
      radio.flush();
   }

   radio.print("V 3:");
   radio.flush();
   for(j=0;j<660;j++){
      for(k=3;k>=0;k--){
        resultado[k]=delta_t_3[j]%256;
        delta_t_3[j]=delta_t_3[j]/256;
      }
      radio.print(resultado);
      radio.flush();
   }

   radio.print("T 3:");
   radio.flush();
   for(j=0;j<660;j++){
      for(k=3;k>=0;k--){
        resultado[k]=atual3[j]%256;
        atual3[j]=atual3[j]/256;
      }
      radio.print(resultado);
      radio.flush();
   }

    
    
    if(i<4){
      i++;
    }
    else{
      i=1;
    }
    PWMWrite(Speed_M1,128,0,1000);   //PWMWrite(pin, resolution, duty, frequency);
    PWMWrite(Speed_M2,128,0,1000);
    PWMWrite(Speed_M3,128,0,1000);
    delay(5000); //tempo de cooldown
    timer=millis();
  }

  
}



