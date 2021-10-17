//ROBO1

#include "mbed.h"
#include "nRF24L01P.h"
#include "Motor.h"

class Counter
{
public:
    Counter(PinName pin) : _interrupt(pin) {        // create the InterruptIn on the pin specified to Counter
        _interrupt.rise(this, &Counter::increment); // attach increment function of this counter instance
    }

    void increment() {
        _count++;
    }

    int read() {
        return _count;
    }

private:
    InterruptIn _interrupt;
    volatile int _count;
};

// Função para comparar a velocidade do motor com a velocidade do requerida. Por enquanto, para apenas um motor
int comparar (int var)
{

    Counter counter(PTA17); //liga a interrupção no pino PTA17
    int valor1 = counter.read();
    wait(0.1);
    int valor2 = counter.read();
    int pulsos = valor2 - valor1;

//    float rotacao = pulsos / 24; // primeira regra de três. se eu tenho 24 pulsos em cada rotação, quantas rotações eu tive com X pulsos?
//    int rpm = rotacao x 600;  // segunda regra de três. se eu tenho tantas rotações por 0.1s, quantas rotações por minuto? seria por 600, mas eu vou dividir por 10 depois, então não tem pra quê
//    int compara = rpm/9; // para comparar com o valor de 0 a 100

    int compara = ( pulsos * 600 ) / ( 24 * 9 );
    int retorno;
    if ( var == compara ) {
        retorno = 0;
    }
    if (var < compara) {
        retorno = 1 ;
    }
    if (var > compara) {
        retorno = 2;
    }
    return retorno;
}



Counter counter(PTA17);

Timer enc; 						//timer encoder
PwmOut ch1(PTA2); 					//pwm1 para carregar o chute 
PwmOut ch2(PTA5); 					//pwm2 para carregar o chute
DigitalOut chute(PTC16); 				// chute output
DigitalOut passe(PTC13); 				//passe output
DigitalOut drible(PTC12); 				//drible output
Timer drib; 						//timer para o drible
Timer sole; 						//timer do solenoide
Motor m1(PTD4,  PTE2, PTE5);				// pwm, fwd, rev
Motor m2(PTA12, PTB11, PTE4); 				// pwm, fwd, rev
Motor m3(PTA4,  PTB10, PTE3); 				// pwm, fwd, rev



//    float s1; // valor de -1.0 a 1.0 para a velocidade do motor
//    float s2;
//    float s3;



int var1 = 0; // valor de 0 a 100 para a velocidade do motor
int var2;
int var3;
int d1 = 1;  // se o motor gira na direção horária ou anti horária
int d2;
int d3;
float s1;
float s2;
float s3;

Serial pc(USBTX, USBRX); // tx, rx

nRF24L01P robo(PTD2, PTD3, PTD1, PTD0, PTC5, PTA13);    // mosi, miso, sck, csn, ce, irq
InterruptIn transmissor(PTA13);


#define TRANSFER_SIZE   4 
#define BATTERY_SIZE    3 
#define ID		48 // MUDAR PARA 0 QUANDO TESTAR COM NETBEANS

    char txData[BATTERY_SIZE], rxData[TRANSFER_SIZE];
    int txDataCnt = BATTERY_SIZE;
    int rxDataCnt = 0;
	
    int flagch;
    int flagchh;
    int flagdb;
    int flagdbb;
    int flagbat;
    int flagenc;

void flip(){
       if ( robo.readable(1) ) {

            // ...read the data into the receive buffer
            rxDataCnt = robo.read( 1, rxData, sizeof( rxData ) );
            
            flagch = 1;
            flagdb = 1;
            flagdbb = 1;
            flagchh = 1;
            
           /* if (rxData[0]==49){
                flagbat=1;
            }*/
        }
    }



int main()
{

ch2.period(1.0);
ch2 = 0.5;
ch1.period(1.0);
ch1 = 0.5;


    float tempo;
    int a = 1;
    float tempo2;


    robo.powerUp();

    robo.setTxAddress( 0xE2E2E2E2E2, 5 );
    robo.setRxAddress( 0xE2E2E2E2E2, 5, 1);

    // Display the (default) setup of the nRF24L01+ chip
    pc.printf( "ROBO" );
    pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  robo.getRfFrequency() );
    pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  robo.getRfOutputPower() );
    pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", robo.getAirDataRate() );
    pc.printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", robo.getTxAddress() );
    pc.printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", robo.getRxAddress(1) );

    pc.printf( "Type keys to test transfers:\r\n  (transfers are grouped into %d characters)\r\n", TRANSFER_SIZE );

    robo.setTransferSize( TRANSFER_SIZE, 1 );

    robo.setReceiveMode();
    robo.enable();


    if (d1 == 0) {
        s1 = 0.0 - var1/100.0;
        m1.speed(s1);
    } else if (d1 ==1) {
        s1 = var1/100.0;
        m1.speed(s1);
    }

    if (d2 == 0) {
        s2 = 0.0 - var2/100.0;
        m2.speed(s2);
    } else {
        s2 = var2/100.0;
        m2.speed(s2);
    }

    if (d3 == 0) {
        s3 = 0.0 - var3/100.0;
        m3.speed(s3);S
    } else {
        s3 = var3/100.0;
        m3.speed(s3);
    }

    int aux = comparar (var1);
    pc.printf("%d", aux);

//    while (aux != 0){
//        if (aux == 1) {
//            s1 = s1 - 0.01;
//            m1.speed(s1);
//            aux = comparar(var1);
//            }
//        else {
//            s1 = s1 + 0.01;
//            m1.speed(s1);
//            aux = comparar(var1);
//          }
    // }

//     m1.speed(s1);
//     m2.speed(s2);
//     m3.speed(s3);
    wait(0.02);

	transmissor.fall(&flip);  // attach the address of the flip function to the rising ed

    while (1) {  // wait around, interrupts will interrupt this!


       if ( (rxData[3]&8)==8 && flagbat == 1 ) {
            pc.printf( "ENVIAR BATERIA" );
        pc.printf("envio bateria\r\n");
                
                for ( int i = 0; i < BATTERY_SIZE; i++ ) {
                    txData[i]=50;
                }
                
                 robo.write( 1, txData, txDataCnt );
                 
            flagbat=0;
        }

        Counter counter(PTA17);  //Início encoder
        int valor1 = counter.read();

        if ((valor1 > 49)) {
            if (flagenc == 1) {
                enc.start();
                flagenc = 0;
            }
        }
        if (enc.read() > 0.1) {
            int valor2 = counter.read();
            int pulsos = valor2 - valor1;
            valor1 = 49;
            pc.printf("The time taken was %f seconds\n", enc.read());
            pc.printf("pulsos %d\n", pulsos);
            enc.stop();
            enc.reset(); //final encoder

        }

        if ((rxData[3]&4)==4 && flagdbb==1) { //programa do drible
            drible = 1;
            if (flagdb==1) {
                drib.start();
                flagdb=0;
            }
            pc.printf( "drible ligado \r\n");
            tempo = drib.read();
            pc.printf("The time taken was %f seconds\n", drib.read());
            if ( tempo > 5.0) {
                drible = 0;
                flagdbb=0;
                pc.printf( "drible desligado \r\n");
                drib.stop();
                drib.reset();

            }
        } else if ((rxData[3]&4)==0 && flagdb==1) {
            drible = 0;
            pc.printf( "drible desligado \r\n");
            flagdb=0; //final do drible
        }
        if ((rxData[3]&2)==2 && flagch==1) { //início do chute
            chute = 1;
            drible = 0;
            if (flagchh == 1) {
                sole.start();
                flagchh = 0;
            }
            tempo2 = sole.read_ms();
            pc.printf( "chute ligado \r\n");
            pc.printf("The time CHUTE taken was %f seconds\n", tempo2);
            if ( tempo2 > 200) {
                chute = 0;
                flagch = 0;
                pc.printf( "chute desligado \r\n");
                sole.stop();
                sole.reset();
            }
        }
        if ((rxData[3]&1)==1 && flagch==1) {
            passe = 1;
            drible = 0;
            chute = 1;
            if (flagchh == 1) {
                sole.start();
                flagchh = 0;
            }
            pc.printf( "passe ligado \r\n");
            pc.printf("The time taken was %f seconds\n", sole.read_ms());
            if ( sole.read_ms() > 200) {
                chute = 0;
                passe = 0;
                flagch = 0;
                pc.printf( "passe desligado \r\n");
                sole.stop();
                sole.reset();
                //final do chute
            }
        }

    }
}
