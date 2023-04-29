#include <mega16.h>
#include <stdio.h>
#include <delay.h>
#include <alcd.h>
#include <sleep.h>
#define  zarib 0.0977517101;
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))  // Voltage Reference: AVCC pin

unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | ADC_VREF_TYPE;    // Delay needed for the stabilization of the ADC input voltage
delay_us(5);
ADCSRA|=(1<<ADSC);  // Start the AD conversion
while ((ADCSRA & (1<<ADIF))==0);   // Wait for the AD conversion to complete
ADCSRA|=(1<<ADIF);
return ADCW;
}

//void sleep_enable (void);
//void powerdown(void);

interrupt [EXT_INT0] void ext_int0_isr(void)
{
lcd_clear();
delay_ms(50);
lcd_putsf("    sleep!!!");
sleep_enable();
powerdown();

}


void main(void)
{
//variables
char st[20]={"#knock = 0"};
int knock=0;
int set_point;
int s;
int larzesh;
char at[20];
int p;

//I/O
DDRD.2=0;
PORTD.2=1;
DDRD.7=1;
PORTD.7=0;

 //ADC configuration
ADMUX=ADC_VREF_TYPE;    // ADC Clock frequency: 500.000 kHz      // ADC Voltage Reference: AVCC pin              // ADC Auto Trigger Source: ADC Stopped
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);
SFIOR=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

// Alphanumeric LCD initialization  on PORT c
lcd_init(16);

//intrrupt
GICR|=(0<<INT1) | (1<<INT0) | (0<<INT2);
MCUCR=(0<<ISC11) | (0<<ISC10) | (1<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);
GIFR=(0<<INTF1) | (1<<INTF0) | (0<<INTF2);

// Global enable interrupts
#asm("sei")


// USART Baud Rate: 4800   // Communication Parameters: 8 Data, 1 Stop, No Parity     // USART Mode: Asynchronous      // USART Transmitter: On   // USART Receiver: Off
UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (0<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
UBRRH=0x00;
UBRRL=0b00001100;


//START
lcd_puts("     START");
delay_ms(1000); 
lcd_clear();
lcd_puts("  Connect your ");
lcd_gotoxy(0,1);
lcd_puts("     phone ");
delay_ms(2000); 
lcd_clear();
printf(" #knock=%d ", knock );          

while (1)
      {
     p = read_adc(7); 
     set_point = p*zarib;
     s= read_adc (0);   
     larzesh = 100- s*zarib;  
     sprintf(at , "set_point = %d" ,set_point );
     lcd_gotoxy(0,0); 
     lcd_puts(st);
     lcd_gotoxy(0,1);
     lcd_puts(at); 
     

          if (larzesh >=set_point)
          {
          PORTD.7= 1;
          knock ++;      
          sprintf(st , "#knock = %d         " , knock );      
          lcd_gotoxy(0,0); 
          lcd_puts(st);    
          lcd_gotoxy(0,1);
          lcd_puts(at); 
           
          printf(" \r\n #knock=%d ", knock );
          delay_ms(5000 );  
     }               
      else {PORTD.7=0;}
     }      

}
