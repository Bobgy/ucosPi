#include "uart.h"
#include "ucos/includes.h"
#include "regs.h"
#include "const.h"
#include "utils.h"

int dht11_read_val();
void light(int bit, int val);

INT32U num = 0;
void userApp1(void * args) {
	while(1) {
		num++;
		digitalWrite(16, num&1);
		digitalWrite(27, num&1);
		dht11_read_val();
		OSTimeDlyHMSM(0, 0, 5, 0);
	}
}

void displayLED(void * args) {
    while(1) {
		INT32U tmp = num, i;
		for (i = 0; i < 4; ++ i, tmp /= 10) {
			light(3 - i, tmp % 10);
			OSTimeDly(1);
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
	digitalWrite(BIT0,bit==0);
	digitalWrite(BIT1,bit==1);
	digitalWrite(BIT2,bit==2);
	digitalWrite(BIT3,bit==3);

	digitalWrite(DIGITA,digit[val][0]);
	digitalWrite(DIGITB,digit[val][1]);
	digitalWrite(DIGITC,digit[val][2]);
	digitalWrite(DIGITD,digit[val][3]);
	digitalWrite(DIGITE,digit[val][4]);
	digitalWrite(DIGITF,digit[val][5]);
	digitalWrite(DIGITG,digit[val][6]);
}

inline int waitDHT(int v, int timeOut) {
	int len = waitEdge(DHT11PIN, timeOut);
	if ((len > 0) == v) return abs(len);
	return 0;
}

int dht11_val[8]={0,0,0,0,0}, act=0;
int dht11_read_val(){
    INT8U i;
	int len;

	uart_string("\r\nreading dht11\r\n");
	uart_flush();

    for(i=0;i<5;i++)dht11_val[i]=0;
	top=0;

	act^=1;
	if (act) {
		uart_string("\r\nactive\r\n");

	    //host send start signal
	    pinMode(DHT11PIN,OUTPUT);      //set pin to output

		digitalWrite(DHT11PIN,LOW);    //set to low at least 18ms
		delay(1000); // 40 us

		digitalWrite(DHT11PIN,HIGH);   //set to high 20-40us
		delay(1000); // 40 us

		//start recieve dht response
		//set pin to input
		pinMode(DHT11PIN,INPUT);
		delay(800); // 30 us

	} else {
	    //host send start signal
	    pinMode(DHT11PIN,OUTPUT);      //set pin to output

		digitalWrite(DHT11PIN,LOW);    //set to low at least 18ms
		OSTimeDlyHMSM(0, 0, 0, 18);

		digitalWrite(DHT11PIN,HIGH);   //set to high 20-40us
		delay(1000); // 40 us

		//start recieve dht response
		//set pin to input
		pinMode(DHT11PIN,INPUT);
		delay(400); // 30 us
	}

	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();

	for (i=0;i<90;++i) {
		waitEdge(DHT11PIN, TIMEOUT_LONG);
	}

	/*
	if (len > 0) {
		len = waitDHT(LOW, TIMEOUT_LONG);
		if (len == 0 || len == TIMEOUT_LONG) {
			error(len, "no response 0\r\n");
			return -1;
		}
	}

	len = waitDHT(HIGH, TIMEOUT_LONG);
	if (len == 0 || len == TIMEOUT_LONG) {
		error(len, "no response 1\r\n");
		return -1;
	}

    for(i=0;i<40;i++)
    {
        len = waitDHT(LOW, TIMEOUT_SHORT);
		if (len == 0 || len == TIMEOUT_SHORT) {
			error(len, "Error on low: ");
			break;
		}

		len = waitDHT(HIGH, TIMEOUT_LONG);
		if (len == 0 || len == TIMEOUT_LONG) {
			error(len, "Error on high: ");
			break;
		}

        dht11_val[i/8] <<= 1;                     //write 1 bit to 0 by moving
        dht11_val[i/8] |= len > 3;                //write 1 bit to 1
    }
	*/

	OS_EXIT_CRITICAL();

	uart_string("queue:\r\n");
	for (i=0; i<top; ++i) {
		if (q[i]&1) hexstring(q[i]|i<<24);
		else hexstrings(q[i]|i<<24);
	}
	uart_string("queue end\r\n");

	return 0;

	if (i<MAX_TIME) {
		error(i, "Not enough packs received.");
		return -1;
	}

	for (i=0;i<5;++i)
		hexstring(dht11_val[i]);

    // verify checksum and print the verified data
    if(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF)){
		uart_string("RH: ");
		hexstring(dht11_val[0]);
		uart_string("TEMP: ");
		hexstring(dht11_val[2]);
		uart_flush();
    } else {
		uart_string("Checksum failed.\r\n");
		uart_flush();
		return -1;
	}
	return 0;
}
