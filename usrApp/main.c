#include "uart.h"
#include "regs.h"
#include "timer.h"
#include "ucos/includes.h"
#include "const.h"
#include "utils.h"

OS_STK  userAppTaskStk1[1000];
OS_STK  displayLEDTaskStk[1000];

extern void userApp1(void *);
extern void displayLED(void *);
void init();

void main()
{
	uart_init();

	InitInterruptController();

	DisableInterrupts();

	timer_init();

	OSInit();

	init();

	OSTaskCreate(userApp1, (void *) 0, &userAppTaskStk1[1000-1],5);

	OSTaskCreate(displayLED, (void *) 0, &displayLEDTaskStk[1000-1],4);

	OSStart();

	while(1);
}

void init() {
	pinMode(BIT0, OUTPUT);
    pinMode(BIT1, OUTPUT);
    pinMode(BIT2, OUTPUT);
    pinMode(BIT3, OUTPUT);

    pinMode(DIGITA, OUTPUT);
    pinMode(DIGITB, OUTPUT);
    pinMode(DIGITC, OUTPUT);
    pinMode(DIGITD, OUTPUT);
    pinMode(DIGITE, OUTPUT);
    pinMode(DIGITF, OUTPUT);
    pinMode(DIGITG, OUTPUT);

	pinMode(16, OUTPUT);
	pinMode(27, OUTPUT);
	pinMode(DHT11PIN, INPUT);
	pinPullUp(DHT11PIN);
}
