#ifndef _UTILS_H
#define _UTILS_H

void pinMode(INT32U pin, INT32U st);
void pinPullUp(int pin);
void digitalWrite(int pin, int v);
INT32U digitalRead(int pin);
INT32U abs(int x);
int waitEdge(int pin, int timeOut);
void error(int len, char *msg);
void delay(int time);

extern int q[200], top, _delay;

#endif
