/*
 * File:   main_master.c
 * Author: Andres Lemus
 *
 * Created on February 8, 2023, 12:27 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "LCD.h" //librería LCD

#define _XTAL_FREQ 8000000 //Frecuencia 8MHz
//MASTER MASTER MASTER

void setup(void);
void setupI2C(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Restart(void);
void I2C_Ack(void);
void I2C_Nack(void);
short I2C_Write(unsigned char data);
unsigned char I2C_Read(void);

void main(void) {
    setup();
    setupI2C();
    while(1){
        
    }
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS = 1;
}

void setupI2C(void){
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC3 = 1;
    
    SSPSTAT = 0b10000000;
    SSPCON = 0b00101000;
    SSPCON2 = 0;
    SSPADD = 19;
}

void I2C_Start(void){
    SSPCON2bits.SEN = 1;
    while (PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_Stop(void){
    SSPCON2bits.PEN = 1;
    while (PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_Restart(void){
    SSPCON2bits.RSEN = 1;
    while (PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_Ack(void){
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while (PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

void I2C_Nack(void){
    SSPCON2bits.ACKDT = 1;
    SSPCON2bits.ACKEN = 1;
    while (PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
}

short I2C_Write(unsigned char data){
    SSPBUF = data;
    while (PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
    short ready = SSPCON2bits.ACKSTAT;
    return ready;
}

unsigned char I2C_Read(void){
    SSPCON2bits.RCEN = 1;
    while (PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;
    return SSPBUF;
}