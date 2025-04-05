/*
 * File:   JordTermometerStyreBox.c
 * Author: chris
 *
 * Created on March 18, 2025, 10:04 AM
 */

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB2      // CCP1 Pin Selection bit (CCP1 function on RB2)
#pragma config CP = OFF        // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <xc.h>

void IOInit() {
    PORTA = 0;
    PORTB = 0;
    TRISB = 0;
    TRISA = 0;
}

void wait(unsigned int time) {
    unsigned int t = time;
    while (t > 0) {
        t--;
    }
    return;
}

void CommandLCD(unsigned char command) {
    PORTB = command & 0xFF;
    
    if (PORTA != 0x00) {
        PORTA = 0x00;
        wait(50);
    }
    
    wait(10);
        
    PORTAbits.RA2 = 1;
    
    wait(50);
    
    PORTAbits.RA2 = 0;
    
    wait(100);
    
    return;
}

void writeChar(char character) {
    PORTA = 0b00000001;
    PORTB = character;
    
    wait(50);
    
    PORTA = 0b00000101;
            
    wait(50);
    
    PORTA = 0b00000001;
    return;
}

void writeString(char* string, char length) {
    for (unsigned char i = 0; i < length; i++) {
        writeChar(string[i]);
    }
}

char* doubbelDabbel(int rawData) {
    int raw = rawData;
    
    for (int i = 0; i < 12; i ++) {
        raw = raw << 1;
        
        if (raw & 0xF000 >= 0x5000){
            raw += 0x3000;
        }
        
        if (raw & 0xF0000 >= 0x50000){
            raw += 0x30000;
        }
        
        if (raw & 0xF00000 >= 0x500000){
            raw += 0x300000;
        }
    
    }
    
    raw = raw >> 12;
    
    char* digits[3];
    
    digits[0] = raw & 0x0F;
    raw >> 4;
    
    digits[1] = raw & 0x0F;
    raw >> 4;
    
    digits[2] = raw & 0x0F;
    raw >> 4;
    
    return digits;
}



void main(void) {
    //PIC setup
    IOInit();
    
    wait(100);
    
    // LCD setup
    CommandLCD(0b00111000);
    CommandLCD(0b00001110);    
    CommandLCD(0b00000001);
    CommandLCD(0b00000110);     
    
    wait(200);
    
    writeString("1: -13.9", 8);
    writeChar(0b11011111);
    writeString("C    9%", 7);


    wait(200);

    CommandLCD(0b11000000);     
    
    writeString("2:  19.9", 8);
    writeChar(0b11011111);
    writeString("C  100%", 7);

    
    while (1) {
        
    }
    
    return;
}
