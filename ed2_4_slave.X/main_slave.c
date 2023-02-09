/*
 * File:   main_slave.c
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
#include "i2c.h"

#define _XTAL_FREQ 8000000 //Frecuencia 8MHz
//SLAVE SLAVE SLAVE

unsigned char voltaje;
unsigned char recibido;

void setup(void);
void setupADC(void);

void main(void) {
    setup();
    setupADC();
    while(1){
        if (ADCON0bits.GO == 0){
            ADCON0bits.GO = 1;
        }
    }
}

void __interrupt() isr(void){ //Interrupciones
    if (PIR1bits.ADIF == 1){
        voltaje = ADRESH;
        PIR1bits.ADIF = 0;
    }
    if (PIR1bits.SSPIF == 1){
        if(I2C_Error_Read() != 0)
        {
            I2C_Error_Data();
        }
        if(I2C_Write_Mode() == 1)
        {
            recibido = I2C_Read_Slave();
        }
        if(I2C_Read_Mode() == 1)
        {
            I2C_Write_Slave(voltaje);
        }
        PIR1bits.SSPIF = 0;
    }
}

void setup(void){
    ANSELbits.ANS0 = 1;
    ANSELH = 0;
    
    PORTC = 0;
    
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    
    OSCCONbits.SCS = 1;
    I2C_Init_Slave(0x50);
}

void setupADC(void){
    ADCON0bits.ADCS1 = 0; // Fosc/8        
    ADCON0bits.ADCS0 = 0; //       
    
    ADCON1bits.VCFG1 = 0; // Referencia VSS (0 Volts)
    ADCON1bits.VCFG0 = 0; // Referencia VDD (5 Volts)
    
    ADCON1bits.ADFM = 0;  // Justificado hacia izquierda
    
    ADCON0bits.CHS3 = 0; // Canal AN0
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;        
    
    ADCON0bits.ADON = 1; // Habilitamos el ADC
    __delay_us(100);     //delay de 100 us
}