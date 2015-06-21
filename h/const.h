#define BIT0 22
#define BIT1 23
#define BIT2 24
#define BIT3 25

#define DIGITA 4
#define DIGITB 2
#define DIGITC 3
#define DIGITD 8
#define DIGITE 7
#define DIGITF 10
#define DIGITG 9

#define MAX_TIME 50
#define DHT11PIN 17

#define INPUT 0x0
#define OUTPUT 0x1

#define HIGH 1
#define LOW 0

#ifdef _OS_Dly_
    #define TIMEOUT_SHORT 10
    #define TIMEOUT_LONG 40
#else
    #define TIMEOUT_SHORT 100
    #define TIMEOUT_LONG 400
#endif

extern void PUT32 ( INT32U, INT32U );
extern INT32U GET32 ( INT32U );


//#define RESET
