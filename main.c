
#include <stdbool.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"
#include <stdint.h>            // has to be added to use uint8_t
#include <avr/interrupt.h>



uint8_t mod=0;
//volatile bool input1, input2, input5, input0;
void pwm_setup(void);
void converter_start(float percent);
void converter_soft_off(void);
void converter_soft_on(uint8_t target);
void converter_emergency_off(void);
void print_duty(float percent_duty, int hor, int ver);
void print_voltage(float voltage, int hor, int ver);
uint8_t  init_duty(void);
volatile float int2percent(uint8_t int_duty);
volatile float int2voltage(uint8_t int_duty);
uint8_t mod_bak;
bool converter_stat=0;
int main(void)
{
	TCCR1B = (0 << CS10) | (0 << CS11) | (0 << CS12);
	mod=0;
	//count=0;
    OCR1A = mod;
	DDRD = (0xFF);
	DDRB = (0xFF);
	DDRC = 0;
	
	/* clear display and home cursor */
	
	
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_puts("convertidor buck\n");
	_delay_ms(1000);
	lcd_clrscr();
	pwm_setup();
	converter_start(init_duty());
	converter_stat=1;
	mod=OCR1A;
	
	lcd_home();
	lcd_puts(" %        Volt    ");
	print_duty(int2percent(mod),0,1);
	print_voltage(int2voltage(mod), 8,1);
	
while(1)
	{
	print_duty(int2percent(mod),0,1);
	print_voltage(int2voltage(mod), 8,1);
	_delay_ms(200);
	 if ((PINC & (1<<PINC1))==0)
	 {
				 if((mod<254) & (converter_stat==1)) 
				 
				 {
			     OCR1A = mod;
			     mod=mod+1;};
	 }
	 if((PINC & (1<<PINC2))==0 )
     {

			 if((mod>1) & (converter_stat==1))  
			     {
				 OCR1A = mod;
				 mod=mod-1;
				 };
	  }
	  if((PINC & (1<<PINC3))==0)
	  {
		    if (converter_stat==0)
		    {
				converter_soft_on(mod_bak);
				mod=mod_bak;
			}
			else
			{
				mod_bak=mod;
				converter_soft_off();
				converter_stat=0;
			}
	  }
	  if((PINC & (1<<PINC0))==0)
	  {
		    converter_emergency_off();
	  }
     }
}

void pwm_setup(void)
{
		    TCCR1A |= (1 << COM1A1);


		    TCCR1A |=  (0 << WGM11) | (1 << WGM10);
		    

		    TCCR1B |= (1 << CS10) |  (0 << CS11) |  (0 << CS12)  | (0 << WGM13) | (1 << WGM12);
}
void converter_start(float percent)
{
            lcd_clrscr();
			lcd_puts("arrancando...\n");
			//unsigned int i;
			float target_float;
			volatile int target;
			target_float=((percent/100)*255);
			target=(int)target_float;
			converter_soft_on(target); 
			lcd_home();
			lcd_puts(" %        Volt    ");
}
void converter_soft_off(void)
{
	lcd_clrscr();
	lcd_puts("apagando...\n");
	mod=OCR1A;
	while(mod>0)
	{
		_delay_ms(20);
		OCR1A = mod;
		mod=mod-1;
	}
	TCCR1B = (0 << CS10) | (0 << CS11) | (0 << CS12);
	lcd_home();
	lcd_puts(" %        Volt    ");
}
void converter_soft_on(uint8_t target)
{
				lcd_clrscr();
			    lcd_puts("arrancando...\n");
				converter_stat=1;
				unsigned int i;
				//TCCR1B |= (1 << CS10) |  (0 << CS11) |  (0 << CS12)  | (0 << WGM13) | (1 << WGM12);
				pwm_setup();
				for (i=0; i<=target; i++)
				{
					_delay_ms(100);
					OCR1A = i;
				}
				lcd_home();
	            lcd_puts(" %        Volt    ");
}
void converter_emergency_off(void)

{   
	TCCR1B = (0 << CS10) | (0 << CS11) | (0 << CS12);
	OCR1A=0;
}

uint8_t init_duty()
{
			unsigned int i_duty=0;
			lcd_puts("arranque %   V  \n");
			print_duty(int2percent(i_duty),0,1);
			print_voltage(int2voltage(i_duty), 7,1);
while(1)
	{
		print_duty(int2percent(i_duty),0,1);
		print_voltage(int2voltage(i_duty),7,1);
		_delay_ms(200);
		if ((PINC & (1<<PINC1))==0)
		{
			if(i_duty<254) 
			{
			i_duty++;
			}
		}
		if((PINC & (1<<PINC2))==0)
		{

			if(i_duty>1)
			{
			i_duty--;
			}
		}
		if((PINC & (1<<PINC3))==0)
		{
			return(int2percent(i_duty));
		}
	}	
}

void print_duty(float percent_duty, int hor, int ver)
{
		char duty_float_string[5];
		dtostrf(percent_duty,3,2,duty_float_string);
        lcd_gotoxy(hor,ver);
		lcd_puts("      ");
		lcd_gotoxy(hor,ver);
		lcd_puts(duty_float_string);
		lcd_puts("%");
}
void print_voltage(float voltage, int hor, int ver)
{
		char voltage_string[5];
		dtostrf(voltage,3,2,voltage_string);
		lcd_gotoxy(hor,ver);
		lcd_puts("       ");
		lcd_gotoxy(hor,ver);
		lcd_puts(voltage_string);
		lcd_puts("V");
}

volatile float int2percent(uint8_t int_duty)
{
		float fduty=0;
		float percent_duty=0;
		fduty=(float)int_duty;
		percent_duty=(fduty*100)/255;
		return(percent_duty);
}
volatile float int2voltage(uint8_t int_duty)
{
		float fduty=0;
		float voltage=0;
		
		fduty=(float)int_duty;
		voltage=311*(fduty/255);
		return(voltage);
}