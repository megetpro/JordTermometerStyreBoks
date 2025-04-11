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
#include <pic16f819.h>

void IOInit() {
    ADCON1 = 0x06; // Set all pins in portA to be digital
    PORTA = 0;
    PORTB = 0;
    TRISB = 0;          // Set PORTB to be output
    TRISA = 0b00001000;
}

void TimerInit() {
    OPTION_REG = 0b11000011;
    
    return;
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
    return;
}

void readMessage() {
    
    short int Termometer_id = 0;
    short int temp_data = 0;
    short int Batt_status = 0;
    char Expected_check_bit = 0;
    
    wait(5);
    Expected_check_bit ^= PORTAbits.RA3; // Updater forventet checkbit ved at XOR med inputtet
    Termometer_id = (Termometer_id << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(10); // fine
    Expected_check_bit ^= PORTAbits.RA3; // Updater forventet checkbit ved at XOR med inputtet
    Termometer_id = (Termometer_id << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(13); // fine
    Expected_check_bit ^= PORTAbits.RA3;
    Termometer_id = (Termometer_id << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(12);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(12);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(12);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(12);
    Expected_check_bit ^= PORTAbits.RA3;
    Batt_status = (Batt_status << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    Batt_status = (Batt_status << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(26);
    
    CommandLCD(0b10000000); 
    writeChar(Termometer_id + '1');
    
    CommandLCD(0b10001100);
    if (Batt_status >= 3) {
        writeString("Okay", 4);
    } else if (Batt_status < 1) {
        writeString(" Lav", 4);
    } else {
        writeString("Fejl", 4);
    }

}

char checkSignal() {
    for (int i = 0; i < 15; i++) {
        if (!PORTAbits.RA3) {
            return 0;
        }
    }
    while (PORTAbits.RA3) {}
    for (int i = 0; i < 7; i++) {
        if (PORTAbits.RA3) {
            return 0;
        }
    }
    while (!PORTAbits.RA3) {}
    for (int i = 0; i < 7; i++) {
        if (!PORTAbits.RA3) {
            return 0;
        }
    }
    while (PORTAbits.RA3) {}
    wait(13);

    return 1;
}

void main(void) {
    IOInit();
    TimerInit();
    
    wait(100);
    
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
    
    wait(200);
    
    while (1) {
        if (PORTAbits.RA3) {
            if (checkSignal()) {
                readMessage();
            }
        }
    }
    
    return;
}
