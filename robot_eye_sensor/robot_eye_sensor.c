/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
� Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 12/1/2019
Author  : 
Company : 
Comments: 


Chip type               : ATmega16
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*******************************************************
installation:

receiver    - A0
transmitter - A1
button      - C0        
output led  - C1
RS          - B0
RD          - B1
EN          - B2
D4          - B4
D5          - B5
D6          - B6
D7          - B7

gnd - vdd - vo - rs - rw - e - d0 - d1 - d2 - d3 - d4 - d5 - d6 - d7 - bla - blk

*******************************************************/

#include <mega16.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <alcd.h>
#include <delay.h>

//      RECIEVER        (PORTA.0)
#define TRANSMITTER_LED (PORTA.1)
#define BUTTON          (PINC.0)
#define OUTPUT_LED      (PORTC.1)
#define button_pressed (BUTTON == 0)

// Voltage Reference: AREF pin
#define ADC_VREF_TYPE ((0<<REFS1) | (0<<REFS0) | (0<<ADLAR))

// Declare your global variables here
static unsigned int adc_data = 512;
static unsigned int threshold = 512;
static unsigned int white_calibration = 0;
static unsigned int black_calibration = 0;

static unsigned int timer_overflows = 0;
static unsigned int max_timer_overflows_to_accept_button_state = 50;

enum State {RECIEVE = 0, WHITE_CALIBRATION = 1, BLACK_CALIBRATION = 2};
char* state_label [3] = {"RE", "WC", "BC"};
enum State state = RECIEVE;

// lcd printing format
// 0123456789abcdef
// st|t:0512|a:1024
// wc|w:0000|b:0000

// labels:
// st = state
// t = threshold
// a = adc_data
// w = white_calibration
// b = black_calibration

void lcd_print_char(int column, int row, char *data) {
    lcd_gotoxy(column, row);
    lcd_puts(data);
}

void lcd_print_int(int start_column, int row, int data) {
    char data_string[4];
    itoa(data, data_string);
    
    lcd_print_char(start_column, row, "    ");
    lcd_print_char(start_column, row, data_string);
}

void print_labels() {
    lcd_print_char(0x0, 0, "st");
    lcd_print_char(0x2, 0, "|T:");
    lcd_print_char(0x9, 0, "|A:");
    lcd_print_char(0x2, 1, "|W:");
    lcd_print_char(0x9, 1, "|B:");
}

void print_threshold()          { lcd_print_int(0x5, 0, threshold); }
void print_adc_data()           { lcd_print_int(0xc, 0, adc_data); }
void print_white_calibration()  { lcd_print_int(0x5, 1, white_calibration); }
void print_black_calibration()  { lcd_print_int(0xc, 1, black_calibration); }
void print_state ()             { lcd_print_char(0x0, 1, state_label[state]); }

void initialize_lcd() {
    print_labels();
    print_state();
    print_threshold();
}

void change_state(enum State new_state) {
    state = new_state;
    print_state();
} 

void start_conversion () {
    ADCSRA = ADCSRA | 0x40;
}

void set_led_output() {
    if(adc_data < threshold) {
        OUTPUT_LED = 1;
    } else {
        OUTPUT_LED = 0;
    }
}

void set_threshold() {
    threshold = (white_calibration + black_calibration) / 2;
    print_threshold();
}

void set_white_calibration(int data) {
    white_calibration = data;
    print_white_calibration();
}

void set_black_calibration(int data) {
    black_calibration = data;
    print_black_calibration();
}


void on_button_pressed() {
    switch(state) {
        case RECIEVE:
            change_state(WHITE_CALIBRATION);
            set_white_calibration(0);
            set_black_calibration(0);
            break;
        case WHITE_CALIBRATION:
            change_state(BLACK_CALIBRATION);
            set_white_calibration(adc_data);
            break;
        case BLACK_CALIBRATION:
            change_state(RECIEVE);
            set_black_calibration(adc_data);
            set_threshold();
            break;
        default:
            break;
    }
}

void on_adc_new_conversion() {
    print_adc_data();
    set_led_output();
}

// ADC interrupt service routine
interrupt [ADC_INT] void adc_isr(void) {
    adc_data = ADCW;
    on_adc_new_conversion();
    delay_ms(10);
    start_conversion();
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void) {
    // just for debouncing
    TCNT0 = 6;
    if(button_pressed) {
        timer_overflows ++;
    } else if(timer_overflows != 0) {
        if(timer_overflows > max_timer_overflows_to_accept_button_state) {
            on_button_pressed();
        }
        timer_overflows = 0;
    }
}

void start () {
    DDRA.1 = 1;
    DDRC.1 = 1;
    PORTC.0 = 1;

    TRANSMITTER_LED = 1;
    initialize_lcd();
    start_conversion();
}

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 1000.000 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
// Timer Period: 0.256 ms
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (1<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);

// USART initialization
// USART disabled
UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (0<<RXEN) | (0<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);

// ADC initialization
// ADC Clock frequency: 125.000 kHz
// ADC Voltage Reference: AREF pin
// ADC Auto Trigger Source: ADC Stopped
ADMUX=ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);
SFIOR=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

// SPI initialization
// SPI disabled
SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTB Bit 0
// RD - PORTB Bit 1
// EN - PORTB Bit 2
// D4 - PORTB Bit 4
// D5 - PORTB Bit 5
// D6 - PORTB Bit 6
// D7 - PORTB Bit 7
// Characters/line: 16
lcd_init(16);

// Global enable interrupts
#asm("sei")

start();

while (1)
      {
      // Place your code here

      }
}
