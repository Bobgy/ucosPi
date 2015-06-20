#include "uart.h"
#include "ucos/includes.h"
#include "regs.h"
#include "const.h"

extern void PUT32(INT32U, INT32U);
extern INT32U GET32(INT32U);
extern void pinMode(int pin, int v);
void digitalWrite(int pin, int v);
INT8U digitalRead(int pin);
int dht11_read_val();

void light(int bit, int val);

INT32U num = 0, placeholder = 0;
void userApp1(void * args) {
	while(1) {
		num++;
		digitalWrite(16, num&1);
		digitalWrite(27, num&1);
		uart_string("tick");
		//hexstrings(digitalRead(DHT11PIN));
		//dht11_read_val();
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}
void displayLED(void * args) {
    while(1) {
		INT32U tmp = num, i;
		for (i = 0; i < 4; ++ i, tmp /= 10) {
			light(3 - i, tmp % 10);
			OSTimeDlyHMSM(0, 0, 0, 5);
		}
	}
}

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

void digitalWrite(int pin, int v) {
	if (v) PUT32(GPSET0, 1<<pin);
	else   PUT32(GPCLR0, 1<<pin);
}

INT8U digitalRead(int pin) {
	return (GET32(GPIN0) >> pin) & 1;
}

int dht11_val[5]={0,0,0,0,0};
int dht11_read_val(){
    INT8U lststate=HIGH;         //last state
    INT8U counter=0;
    INT8U j=0,i;

	uart_string("reading dht11");
	uart_flush();

    for(i=0;i<5;i++)dht11_val[i]=0;

    //host send start signal
    pinMode(DHT11PIN,OUTPUT);      //set pin to output
    digitalWrite(DHT11PIN,LOW);    //set to low at least 18ms
	OSTimeDlyHMSM(0, 0, 0, 18);
    digitalWrite(DHT11PIN,HIGH);   //set to high 20-40us
	OSTimeDly(4); // 40us

    //start recieve dht response
    pinMode(DHT11PIN,INPUT);       //set pin to input
    for(i=0;i<MAX_TIME;i++)
    {
        counter=0;
        while(digitalRead(DHT11PIN)==lststate){     //read pin state to see if dht responsed. if dht always high for 255 + 1 times, break this while circle
            counter++;
            OSTimeDly(1); // 10us
            if(counter==25)
                break;
        }
        lststate=digitalRead(DHT11PIN);             //read current state and store as last state.
        if(counter==25)                            //if dht always high for 255 + 1 times, break this for circle
            break;
        // top 3 transistions are ignored, maybe aim to wait for dht finish response signal
        if((i>=4)&&(i%2==0)){
            dht11_val[j/8]<<=1;                     //write 1 bit to 0 by moving left (auto add 0)
            if(counter>8)                          //long mean 1
                dht11_val[j/8]|=1;                  //write 1 bit to 1
            j++;
        }
    }
	uart_string("j=");
	hexstring(j);
	uart_flush();
    // verify checksum and print the verified data
    if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF))){
		uart_string("RH:");
		hexstring(dht11_val[0]);
		uart_string("TEMP:");
		hexstring(dht11_val[2]);
    } else {
		int k;
		for(k=0;k<5;++k)
			hexstring(dht11_val[k]);
		uart_string("Failed.");
	}
	uart_flush();
	return 0;
}
