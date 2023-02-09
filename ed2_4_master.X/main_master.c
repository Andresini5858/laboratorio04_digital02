/*
 * File:   main_master.c
 * Author: Andres
 *
 * Created on February 9, 2023, 3:18 PM
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
#include "i2c.h"

#define _XTAL_FREQ 8000000 //Frecuencia 8MHz

unsigned char voltaje;
char buffer[20];

void setup(void);

void main(void) {
    setup();
    Lcd_Init();
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("S1:");
    while(1){
        I2C_Start();
        I2C_Write(0x51);
        voltaje = I2C_Read();
        Lcd_Set_Cursor(2,1);
        sprintf(buffer, "%d", voltaje);
        Lcd_Write_String(buffer);
        I2C_Stop();
        __delay_us(10);
    }
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0;
    TRISD = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    
    OSCCONbits.SCS = 1;
    I2C_Init_Master(I2C_100KHZ);
}