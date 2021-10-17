#include "mbed.h"
#include "Motor.h"


Serial pc(USBTX, USBRX); // tx, rx
Motor m1(PTA4,  PTE2, PTE5); // pwm, fwd, rev

class Counter {
public:
    Counter(PinName pin) : _interrupt(pin) {        // create the InterruptIn on the pin specified to Counter
        _interrupt.fall(this, &Counter::increment); // attach increment function of this counter instance
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

int compare (){ 
    // Returns a number from 0 to 100, correspondent to the speed of the motor
    Counter counter(PTA12); //enables interrupt at pin PTA12
    int value1 = counter.read();
    dalay(100);
    int value2 = counter.read();
    int pulses = value2 - value1;
    
//    float rotation = pulsos / 24; // 24 pulses per rotation
//    int rpm = rotation x 600;  
//    int speed = rpm/9; // for speed between 0 and 100
  
    int speed = ( pulses * 600 ) / ( 24 * 9 );  
    return speed; 
}

int vetor[25];
int setpoint = 100; // desired motor speed
int d1 = 1;  // direction of motor rotation (forward or backward)
float s1;
int mediamovel [4]; // array for storing moving average elements. window of size 4
int i = 0;
int a = 0;
int speed = 0; // filtered speed
//variables for PI control
int output = setpoint; // var1 é o output
int somaerro = 0;
int e = 0; 
int P;
int I;
int Kp = 1; 
int Ki = 1; 

 int main() {
        while(1){
        
        //if (pc.readable() ){
        //    var1 = pc.getc();
        //}
        
        pc.printf("pwm %d\n", output);
    
         
            if (d1 == 0) {
             s1 = 0.0 - output/100.0; 
             m1.speed(s1);
                 }
            else if (d1 ==1) {
             s1 = output/100.0;
             m1.speed(s1);
                 }
                 
    int aux = compare (); 
    
    //moving average filter to filter some of the noise from the motor
    mediamovel[i] = aux;
        if (i < 4){
            i++;
            }
        else{ 
            i = 0;
            }
    
         for(int j=0;j<4;j++) { 
            velocidade = velocidade + mediamovel[j]; 
            } 

        velocidade = velocidade/4; 
       
        pc.printf("%d\n", velocidade);
        
    // controle PI
    
 //   e = setpoint - velocidade;
 //   somaerro =+ e;
  //  P = Kp * e;
  //  I = Ki * somaerro;
  //  output = P + I;
    
  //  if (output > 100){    
  //      output = 100;
  //      {
   // if (output < 0) {
   //     output = 0;
   //     }
   
  //  vetor[a] = velocidade;
  //      if (a < 25){
  //          a++;
   //         }
   //     else{ 
   //         a = 0;
    //        }
 
   // for(int b=0;b<20;b++){   
   // pc.printf("%d ", vetor[b]); 
  //      }

    wait(1);
    }
    
}
