#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define BIT0 3
#define BIT1 4
#define BIT2 5
#define BIT3 6

#define DIGITA 7
#define DIGITB 8
#define DIGITC 9
#define DIGITD 10
#define DIGITE 11
#define DIGITF 12
#define DIGITG 13

#define MAX_TIME 85
#define DHT11PIN 14
#define ATTEMPTS 5                 //retry 5 times when no response

char digit[10][8] =  //数字显示定义
    {
        {0,0,0,0,0,0,1,1}, //0
        {1,0,0,1,1,1,1,1}, //1
        {0,0,1,0,0,1,0,1}, //2
        {0,0,0,0,1,1,0,1}, //3
        {1,0,0,1,1,0,0,1}, //4
        {0,1,0,0,1,0,0,1}, //5
        {0,1,0,0,0,0,0,1}, //6
        {0,0,0,1,1,1,1,1}, //7
        {0,0,0,0,0,0,0,1}, //8
        {0,0,0,0,1,0,0,1}  //9
    };

void light(int bit, int val){
	if(bit == 0)
		digitalWrite(BIT0,1);
	else
		digitalWrite(BIT0,0);
	if(bit == 1)
		digitalWrite(BIT1,1);
	else
		digitalWrite(BIT1,0);
	if(bit == 2)
		digitalWrite(BIT2,1);
	else
		digitalWrite(BIT2,0);
	if(bit == 3)
		digitalWrite(BIT3,1);
	else
		digitalWrite(BIT3,0);

	digitalWrite(DIGITA,digit[val][0]);
	digitalWrite(DIGITB,digit[val][1]);
	digitalWrite(DIGITC,digit[val][2]);
	digitalWrite(DIGITD,digit[val][3]);
	digitalWrite(DIGITE,digit[val][4]);
	digitalWrite(DIGITF,digit[val][5]);
	digitalWrite(DIGITG,digit[val][6]);

}

int dht11_val[5]={0,0,0,0,0};

int dht11_read_val(){
    uint8_t lststate=HIGH;         //last state
    uint8_t counter=0;
    uint8_t j=0,i;
    for(i=0;i<5;i++)
	dht11_val[i]=0;

    //host send start signal
    pinMode(DHT11PIN,OUTPUT);      //set pin to output
    digitalWrite(DHT11PIN,LOW);    //set to low at least 18ms
    delay(18);
    digitalWrite(DHT11PIN,HIGH);   //set to high 20-40us
    delayMicroseconds(40);

    //start recieve dht response
    pinMode(DHT11PIN,INPUT);       //set pin to input
    for(i=0;i<MAX_TIME;i++)
    {
        counter=0;
        while(digitalRead(DHT11PIN)==lststate){     //read pin state to see if dht responsed. if dht always high for 255 + 1 times, break this while circle
            counter++;
            delayMicroseconds(1);
            if(counter==255)
                break;
        }
        lststate=digitalRead(DHT11PIN);             //read current state and store as last state.
        if(counter==255)                            //if dht always high for 255 + 1 times, break this for circle
            break;
        // top 3 transistions are ignored, maybe aim to wait for dht finish response signal
        if((i>=4)&&(i%2==0)){
            dht11_val[j/8]<<=1;                     //write 1 bit to 0 by moving left (auto add 0)
            if(counter>16)                          //long mean 1
                dht11_val[j/8]|=1;                  //write 1 bit to 1
            j++;
        }
    }
    // verify checksum and print the verified data
    if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF))){
        printf("RH:%d,TEMP:%d\n",dht11_val[0],dht11_val[2]);
	while(1){
int ds = 5;
	light(0, dht11_val[0]/10);
delay(ds);
	light(1, dht11_val[0]%10);
delay(ds);
	light(2, dht11_val[2]/10);
delay(ds);
	light(3, dht11_val[2]%10);
delay(ds);
}
        return 1;
    }
    else
        return 0;
}

int main(){
	if (wiringPiSetup () == -1) //test the install status of wiringPi
    {
        printf("ERROR setup...");
        exit (1) ;
    }
    /*初始化引脚*/
	pinMode (BIT0, OUTPUT) ;
    pinMode (BIT1, OUTPUT) ;
    pinMode (BIT2, OUTPUT) ;
    pinMode (BIT3, OUTPUT) ;

    pinMode (DIGITA, OUTPUT) ;
    pinMode (DIGITB, OUTPUT) ;
    pinMode (DIGITC, OUTPUT) ;
    pinMode (DIGITD, OUTPUT) ;
    pinMode (DIGITE, OUTPUT) ;
    pinMode (DIGITF, OUTPUT) ;
    pinMode (DIGITG, OUTPUT) ;

    digitalWrite(BIT0,0);
    digitalWrite(BIT1,0);
    digitalWrite(BIT2,0);
    digitalWrite(BIT3,0);


    int attempts=ATTEMPTS;
    while(attempts){                        //you have 5 times to retry
	    int success = dht11_read_val();     //get result including printing out
	    if (success) {                      //if get result, quit program; if not, retry 5 times then quit
		    break;
	    }
	    attempts--;
	    delay(2500);
    }


    return 0;
}
