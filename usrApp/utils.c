#include "uart.h"
#include "regs.h"
#include "timer.h"
#include "ucos/includes.h"
#include "const.h"
#include "utils.h"

void pinMode(INT32U pin, INT32U st) {
	INT32U x, offset, gpfsel, val;
	x = pin / 10;
	offset = pin % 10;
	gpfsel = GPFSEL0 + x * 4;
	val = GET32(gpfsel);
	val &= ~ (0x7 << offset * 3);
	val |= st << offset * 3;
	PUT32(gpfsel, val);
}

void pinPullUp(int pin) {
	PUT32(GPPUD, 2);
	delay(100);
	PUT32(GPPUDCLK0, 1<<pin);
    delay(100);
	PUT32(GPPUD, 0);
	PUT32(GPPUDCLK0, 0);
}

void digitalWrite(int pin, int v) {
	if (v) PUT32(GPSET0, 1<<pin);
	else   PUT32(GPCLR0, 1<<pin);
}

INT32U digitalRead(int pin) {
	return (GET32(GPIN0) >> pin) & 1;
}

int q[200], top=0;

INT32U abs(int x) {
	return x > 0 ? x : -x;
}

int waitEdge(int pin, int timeOut) {
	int len = 1, v = digitalRead(pin);
	for (; len < timeOut; ++len) {
		if (digitalRead(pin) != v) break;
		len++;
	}
	q[top++]=(len<<4)|v;
	return v ? len : -len;
}

void error(int len, char *msg) {
	uart_string(msg);
	hexstring(len);
	uart_flush();
}

int _delay;
void delay(int time) {
	while(time--)_delay+=time;
}
