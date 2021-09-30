/*
 * File:   LAB7.c
 * Author: José Santizo
 *
 * Created on 27 de septiembre de 2021, 14:56
 * 
 * Descripción: Contador de TMR0 aumenta 1 contador y pushbuttons en puerto B aumentan o disminuyen 
 */

//---------------------Bits de configuración-------------------------------
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <stdio.h>

//-----------------------Constantes----------------------------------
#define  valor_tmr0 237              // valor_tmr0 = 61

//-----------------------Variables------------------------------------
uint8_t  cont1 = 0;
uint8_t  cont = 0;                  // Cont = 0
int  cont_portc = 0;            // Cont_portc = 0
int  cont_decenas = 0;          // Cont_decenas = 0
int  cont_centenas = 0;         // Cont_centenas = 0
uint8_t  disp_selector = 0b001;

//------------Funciones sin retorno de variables----------------------
void setup(void);                   // Función de setup
void tmr0(void);                    // Función para reiniciar TMR0
void tabla_unidades(void);          // Tabla para traducir unidades a displays de 7 segmentos
void limites(void);                 // Limites del aumento de contadores
void displays(void);

//-------------Funciones que retornan variables-----------------------
int  tabla(int a);


//----------------------Interrupciones--------------------------------
void __interrupt() isr(void){
    if(T0IF){
        tmr0();                     // Reiniciar TMR0
        cont ++;                    // Incrementar variable cont en 1 cada 0.05 segundos
        if(cont == 21){              // Si cont = 2 o timer 0 = 0.1 segundos
            cont = 0;               // Limpiar cont
            PORTA ++;               // Incrementar puerto C en 1
        }
        displays();                 // Rutina para alternar valores en displays
    }
}

//----------------------Main Loop--------------------------------
void main(void) {
    setup();                        // Subrutina de setup
    while(1){
       if(PORTBbits.RB0){
           while(RB0);              // Mientras RB0 no se suelte, no aumentar el puerto C
           cont_portc ++;
           cont1 ++;
       }
       if(PORTBbits.RB1){
           while(RB1);              // Mientras RB1 no se suelte, no decrementar el puerto C
           cont_portc --;
           cont1 --;
       }
       limites();
       //int PORTC = tabla(cont_decenas);
    }
}

//----------------------Subrutinas--------------------------------
void setup(void){
    
    //Configuración de entradas y salidas
    ANSEL = 0;                      // Pines digitales
    ANSELH = 0;
    
    TRISA = 0;                      // PORTA como salida
    TRISC = 0;                      // PORTC como salida
    TRISB = 0b00000011;             // PORTB, pin 0 Y 1 como entrada
    TRISD = 0;
    
    PORTA = 0;
    PORTD = 0;
    PORTB = 0;
    PORTC = 0;
    
    //Configuración de oscilador
    OSCCONbits.IRCF = 0b0110;       // Oscilador a 4 MHz = 110
    OSCCONbits.SCS = 1;
    
    //Configuración de TMR0
    OPTION_REGbits.T0CS = 0;        // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0;        // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;         // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;         // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    TMR0 = valor_tmr0;              // preset for timer register
    
    //Configuración de interrupciones
    INTCONbits.T0IF = 0;            // Habilitada la bandera de TIMER 0      
    INTCONbits.T0IE = 1;            // Habilitar las interrupciones de TIMER 0
    INTCONbits.GIE = 1;             // Habilitar interrupciones globales
    
    return;
}

void tmr0(void){
    INTCONbits.T0IF = 0;            // Limpiar bandera de TIMER 0
    TMR0 = valor_tmr0;              // TMR0 = 61
    return;
}

void limites(void){
    if(cont_portc == 10){           // Si cont_portc == 10
        cont_decenas ++;            //  entonces sumar uno a decenas y reiniciar cont_portc
        cont_portc = 0;
    }
    if(cont_decenas == 10){         // Si cont_decenas == 10
        cont_centenas ++;           //  entonces sumar uno a centenas y reiniciar decenas
        cont_decenas = 0;
    }
    if(cont_portc == -1){
        cont_portc = 9;
        cont_decenas --;
    }
    if(cont_decenas == -1){
        cont_decenas = 9;
        cont_centenas --;
    }
    if(cont1 == 255){
        cont_portc = 5;
        cont_decenas = 5;
        cont_centenas = 2;
    }
    if(cont1 == 256){
        cont_portc = 0;
        cont_decenas = 0;
        cont_centenas = 0;
    }
    return;
}

void displays(void){
    PORTD = disp_selector;
    if(disp_selector == 0b001){
        PORTC = tabla(cont_portc);
        disp_selector = 0b010;
    }
    else if(disp_selector == 0b010){
        PORTC = tabla(cont_decenas);
        disp_selector = 0b100;
    }
    else if(disp_selector == 0b100){
        PORTC = tabla(cont_centenas);
        disp_selector = 0b001;
    }
}

int tabla(int a){
    switch (a){
        case 0:
            return 0b00111111;
            break;
        case 1:
            return 0b00000110;
            break;
        case 2:
            return 0b01011011;
            break;
        case 3:
            return 0b01001111;
            break;
        case 4:
            return 0b01100110;
            break;
        case 5:
            return 0b01101101;
            break;
        case 6:
            return 0b01111101;
            break;
        case 7:
            return 0b00000111;
            break;
        case 8:
            return 0b01111111;
            break;
        case 9:
            return 0b01101111;
            break;
        case 10:
            return 0b01111011;
        default:
            break;
            
    }
}