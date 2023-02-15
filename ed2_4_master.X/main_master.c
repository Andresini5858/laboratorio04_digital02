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

uint8_t selector = 0;
uint8_t bandera = 0;
uint8_t segundo;
uint8_t minuto;
uint8_t hora;
uint8_t dow;
uint8_t dia;
uint8_t mes;
uint8_t ano;
uint8_t temporal;
unsigned char voltaje;
char buffer[20];

void setup(void);
void Slave1(void);
void Set_sec(uint8_t sec);
void Set_min(uint8_t min);
void Set_hour(uint8_t hour);
void Set_day_week(uint8_t day_week);
void Set_day(uint8_t day);
void Set_month(uint8_t month);
void Set_year(uint8_t year);
uint8_t Read(uint8_t address);
void Read_Time(uint8_t *s, uint8_t *m, uint8_t *h);
void Read_Fecha(uint8_t *d, uint8_t *mo, uint8_t *y);
uint8_t Dec_to_Bcd(uint8_t dec_number);
uint8_t Bcd_to_Dec(uint8_t bcd);

void main(void) {
    setup();
    Lcd_Init();
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("S1:");
    Set_sec(0);
    Set_day(0);
    Set_hour(12);
    Set_day_week(3);
    Set_day(14);
    Set_month(2);
    Set_year(23);
    while(1){
        if (selector == 0){
            Slave1();
            Read_Time(&segundo, &minuto, &hora);
            Lcd_Set_Cursor(1,7);
            sprintf(buffer, "%02u:%02u:%02u ", hora, minuto, segundo);
            Lcd_Write_String(buffer);

            Read_Fecha(&dia, &mes, &ano);
            Lcd_Set_Cursor(2,7);
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano);
            Lcd_Write_String(buffer);

            Lcd_Set_Cursor(2,5);
            Lcd_Write_String("T");
        }
        else if (selector == 1){
            Slave1();
            Lcd_Set_Cursor(2,7);
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano);
            Lcd_Write_String(buffer);
            Lcd_Set_Cursor(2,5);
            Lcd_Write_String("D ");
        }
        else if (selector == 2){
            Slave1();
            Lcd_Set_Cursor(2,7);
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano);
            Lcd_Write_String(buffer);
            Lcd_Set_Cursor(2,5);
            Lcd_Write_String("M ");
        }
        else if (selector == 3){
            Slave1();
            Lcd_Set_Cursor(2,7);
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano);
            Lcd_Write_String(buffer);
            Lcd_Set_Cursor(2,5);
            Lcd_Write_String("A ");
        }
    }
}

void __interrupt() isr(void){ //Interrupciones 
    if (INTCONbits.RBIF == 1){
        INTCONbits.RBIF = 0;
        if (PORTBbits.RB3 == 0){
            bandera = 1;
            __delay_ms(1);
        }
        if (PORTBbits.RB3 == 1 && bandera == 1){
            if (selector == 0){
                selector = 1;
                bandera = 0;
            }
            else if (selector == 1){
                selector = 2;
                bandera = 0;
            }
            else if (selector == 2){
                selector = 3;
                bandera = 0;
            }
            else if (selector == 3){
                selector = 0;
                bandera = 0;
            }
        }
        if (PORTBbits.RB2 == 0){
            bandera = 2;
            __delay_ms(1);
        }
        if (PORTBbits.RB2 == 1 && bandera == 2){
            if (selector == 1){
                if (mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12){
                    if (dia == 31){
                        dia = 31;
                        Set_day(dia);
                    }
                    else {
                        dia++;
                        Set_day(dia);   
                    }
                }
                else if (mes == 4 || mes == 6 || mes == 9 || mes == 11){
                    if (dia == 30){
                        dia = 30;
                        Set_day(dia);
                    }
                    else {
                        dia++;
                        Set_day(dia);   
                    }
                }
                else if (mes == 2){
                    if (dia == 28){
                        dia = 28;
                        Set_day(dia);
                    }
                    else {
                        dia++;
                        Set_day(dia);   
                    }
                }
                bandera = 0;
            }
            else if (selector == 2){
                if (mes == 12){
                    mes = 12;
                    Set_month(mes);
                }
                else {
                    mes++;
                    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && (dia >> 30)){
                        Set_day(30);
                    }
                    else if (mes == 2 && dia >> 28){
                        Set_day(28);
                    }
                    Set_month(mes);   
                }
                bandera = 0;
            }
            else if (selector == 3){
                if (ano == 99){
                    ano = 99;
                    Set_year(ano);
                }
                else {
                    ano++;
                    Set_year(ano);   
                }
                bandera = 0;
            }
        }
        if (PORTBbits.RB1 == 0){
            bandera = 3;
            __delay_ms(1);
        }
        if (PORTBbits.RB1 == 1 && bandera == 3){
            if (selector == 1){
                if (dia == 1){
                    dia = 1;
                    Set_day(dia);
                }
                else {
                    dia--;
                    Set_day(dia);
                }
                bandera = 0;
            }
            else if (selector == 2){
                if (mes == 1){
                    mes = 1;
                    Set_month(mes);
                }
                else {
                    mes--;
                    Set_month(mes);
                }
                bandera = 0;
            }
            else if (selector == 3){
                if (ano == 0){
                    ano = 0;
                    Set_year(ano);
                }
                else {
                    ano--;
                    Set_year(ano);
                }
                bandera = 0;
            }
        }
    }
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0b00001110;
    TRISD = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    INTCONbits.GIE = 1;
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = 1;
    
    OPTION_REGbits.nRBPU = 0; //Pull-ups activos
    IOCBbits.IOCB3 = 1; //Activar interrupt-onchange del pin B3
    IOCBbits.IOCB2 = 1; //Activar interrupt-onchange del pin B2
    IOCBbits.IOCB1 = 1; //Activar interrupt-onchange del pin B1
    
    WPUBbits.WPUB3 = 1; //Activar pull-up del pin B3
    WPUBbits.WPUB2 = 1; //Activar pull-up del pin B2
    WPUBbits.WPUB1 = 1; //Activar pull-up del pin B1
    
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    
    OSCCONbits.SCS = 1;
    I2C_Init_Master(I2C_100KHZ);
}

uint8_t Read(uint8_t address){
    uint8_t dato = 0;
    I2C_Start();
    I2C_Write(0xD0);
    I2C_Write(address);
    I2C_Restart();
    I2C_Write(0xD1);
    dato = I2C_Read();
    I2C_Nack();
    I2C_Stop();
    __delay_us(10);
    return dato;
}

void Read_Time(uint8_t *s, uint8_t *m, uint8_t *h){
    *s = Bcd_to_Dec(Read(0x00));
    *m = Bcd_to_Dec(Read(0x01));
    *h = Bcd_to_Dec(Read(0x02));
}

void Read_Fecha(uint8_t *d, uint8_t *mo, uint8_t *y){
    *d = Bcd_to_Dec(Read(0x04));
    *mo = Bcd_to_Dec(Read(0x05));
    *y = Bcd_to_Dec(Read(0x06));
}

void Set_sec(uint8_t sec){
    I2C_Start();
    I2C_Write(0xD0);
    I2C_Write(0x00);
    I2C_Write(Dec_to_Bcd(sec));
    I2C_Stop();
}

void Set_min(uint8_t min){
    I2C_Start();
    I2C_Write(0xD0);
    I2C_Write(0x01);
    I2C_Write(Dec_to_Bcd(min));
    I2C_Stop();
}

void Set_hour(uint8_t hour){
    I2C_Start();
    I2C_Write(0xD0);
    I2C_Write(0x02);
    I2C_Write(Dec_to_Bcd(hour));
    I2C_Stop();
}

void Set_day_week(uint8_t day_week){
    I2C_Start();
    I2C_Write(0xD0);
    I2C_Write(0x03);
    I2C_Write(Dec_to_Bcd(day_week));
    I2C_Stop();
}

void Set_day(uint8_t day){
    I2C_Start();
    I2C_Write(0xD0);
    I2C_Write(0x04);
    I2C_Write(Dec_to_Bcd(day));
    I2C_Stop();
}

void Set_month(uint8_t month){
    I2C_Start();
    I2C_Write(0xD0);
    I2C_Write(0x05);
    I2C_Write(Dec_to_Bcd(month));
    I2C_Stop();
}

void Set_year(uint8_t year){
    I2C_Start();
    I2C_Write(0xD0);
    I2C_Write(0x06);
    I2C_Write(Dec_to_Bcd(year));
    I2C_Stop();
}

uint8_t Dec_to_Bcd(uint8_t dec_number){
    uint8_t bcd_number;
    bcd_number = 0;
    while(1){
        if (dec_number >= 10){
            dec_number = dec_number - 10;
            bcd_number = bcd_number + 0b00010000;
        }
        else {
            bcd_number = bcd_number + dec_number;
            break;
        }
    }
    return bcd_number;
}

uint8_t Bcd_to_Dec(uint8_t bcd){
    uint8_t dec;
    dec = ((bcd>>4)*10)+(bcd & 0b00001111);
    return dec;
}

void Slave1(void){
    I2C_Start();
    I2C_Write(0x51);
    voltaje = I2C_Read();
    Lcd_Set_Cursor(2,1);
    sprintf(buffer, "%d ", voltaje);
    Lcd_Write_String(buffer);
    I2C_Stop();
    __delay_us(10);
}