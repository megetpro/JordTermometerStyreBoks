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
    
    //wait(50);
    
    PORTA = 0b00000101;
            
    //wait(50);
    
    PORTA = 0b00000001;
    return;
}

void writeString(char* string, char length) {
    for (unsigned char i = 0; i < length; i++) {
        writeChar(string[i]);
    }
    return;
}

char* mV2Celcius(unsigned int inputValue){
    
    //Total mV value
    int milivolts = inputValue;
    
    //Convert to temerature in C from kelvin
    int totalC = milivolts - 2731 + 29;
    
    static char tempStr[6]; // Format: " XX.X" + '\0'
    
    //Handle negative temps
    if (totalC < 0) {
        totalC = -totalC;
        tempStr[0] = '-';
    }
    else {
        tempStr[0] = ' ';
    }
    
    int intPart = totalC / 10;
    int decPart = totalC % 10;
    
    tempStr[1] = (intPart / 10) + '0';
    tempStr[2] = (intPart % 10) + '0';
    tempStr[3] = '.';
    tempStr[4] = decPart + '0';
    tempStr[5] = '\0';
    
    return tempStr;
}

void updateLCD(unsigned int value, char line, char batStatus, char error) {
    
    if(line){
        CommandLCD(0b11000011);
        char* tempStr = mV2Celcius(value);
        writeString(tempStr, 5);
        
        if (!error) {
            if (batStatus) {
                CommandLCD(0b11001100);
                writeString("Okay", 4);
            } else {
                CommandLCD(0b11001100);
                writeString(" Low", 4);
            }
        } else {
            CommandLCD(0b11001100);
            writeString(" Err", 4);
        }
    } else {
        CommandLCD(0b10000011);
        char* tempStr = mV2Celcius(value);
        writeString(tempStr, 5);
        
        if (!error) {
            if (batStatus) {
                CommandLCD(0b10001100);
                writeString("Okay", 4);
            } else {
                CommandLCD(0b10001100);
                writeString(" Low", 4);
            }
        } else {
            CommandLCD(0b10001100);
            writeString(" Err", 4);
        }
    }
    
    
}

void readMessage() {
    
    short int Termometer_id = 0;
    unsigned short int temp_data = 0;
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
    while(!PORTAbits.RA3) {}
    
    wait(18);
    
    //wait(13);
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
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(13);
    while(!PORTAbits.RA3) {}
    
    wait(18);
    
    // wait(13);
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
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 0;
    
    wait(13);
    Expected_check_bit ^= PORTAbits.RA3;
    temp_data = (temp_data << 1) | PORTAbits.RA3;
    PORTAbits.RA4 = 1;
    
    wait(13);
    while(!PORTAbits.RA3) {}
    
    wait(18);
    
    //wait(12);
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
    
    unsigned int temp = (temp_data & 0x00ff) << 8 | (temp_data & 0xff00) >> 8; // Bytter p� bits s� fedcba9876543210 bliver 76543210fedcba98
    temp = (temp & 0x0f0f) << 4 | (temp & 0xf0f0) >> 4; // 32107654ba98fedc
    temp = (temp & 0x3333) << 2 | (temp & 0xcccc) >> 2; // 1032547698badcfe
    temp = (temp & 0x5555) << 1 | (temp & 0xaaaa) >> 1; // 0123456789abcdef
    
    temp = temp >> 6;
    
    char Batt = 0;
    char Error_state = 0;
    
    if (Batt_status == 3) {
        Batt = 1;
    } else if (Batt_status == 0) {
        Batt = 0;
    } else {
        Error_state = 1;
    }
    
    //temp -= 500;
    
    updateLCD(temp*5 - (temp >> 3), 0, Batt, Error_state);

}

char checkSignal() {
    int j = 0;
    for (int i = 0; i < 15; i++) {
        if (!PORTAbits.RA3) {
            return 0;
        }
    }
    while (PORTAbits.RA3) {
        if (j > 5) {
            return 0;
        }
        j++;
    }
    j = 0;
    for (int i = 0; i < 7; i++) {
        if (PORTAbits.RA3) {
            return 0;
        }
    }
    while (!PORTAbits.RA3) {
        if (j > 10) {
            return 0;
        }
        j++;
    }
    j = 0;
    for (int i = 0; i < 7; i++) {
        if (!PORTAbits.RA3) {
            return 0;
        }
    }
    while (PORTAbits.RA3) {
        if (j > 10) {
            return 0;
        }
        j++;
    }
    j = 0;
    wait(13);

    return 1;
}

void staticInfo (){
    writeString("1: ", 3);
    
    CommandLCD(0b10001000); //Set courser to the 8 position in 1 row
    writeChar(0b11011111); //Degree symbol
    
    writeString("C", 1);

    CommandLCD(0b11000000); //Next line
    
    writeString("2: ", 3);
    
    CommandLCD(0b11001000); //Set courser to the 8 position in 2 row
    writeChar(0b11011111); //Degree symbol
    writeString("C", 1);
}


void main(void) {
    //PIC setup
    IOInit();
    TimerInit();
    
    wait(100);
    
    // LCD setup
    CommandLCD(0b00111000);
    CommandLCD(0b00001110);    
    CommandLCD(0b00000001);
    CommandLCD(0b00000110);     
    
    wait(200);
    
    staticInfo();
            
    while(1) {
        if (checkSignal()) {
            readMessage();
        }
        
    }
    
    return;
}