/*
 * automatic_valve.c
 *
 * Created: 16.05.2022 11:41:52
 * Author : m.lubieniecki
 */ 
#define F_CPU 8000000UL

#define valve_closed 100 /*PWM pulse width 0.8 ms */
#define valve_opened 250 /*PWM pluse width 2 ms*/

#define reference_voltage 5 
#define threshold_adc_value 593 /* 300 kPa calculated for 10bit adc from sensor datasheet */

#include <avr/io.h>
#include <util/delay.h>

void ADC_Init()
{
	DDRA = 0x0;      /* Make ADC port as input */
	ADCSRA = 0x87;   /* 10000111  Enable ADC, fr/128  */
	ADMUX = 0x40;	 /* 01000000 Vref: Avcc, ADC channel: 0 */
}

void PWM_Init()
{
		DDRD |= (1<<PD5);	/* Make OC1A pin as output */
		TCNT1 = 0;		    /* Set timer1 count zero */
		ICR1 = 2499;		/* Set TOP count for timer1 in ICR1 register */

		/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
		TCCR1A = (1<<WGM11)|(1<<COM1A1);
		TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
}


int ADC_Read(char channel)
{
	int Ain,AinLow;
	
	ADMUX=ADMUX|(channel & 0x0f);	/* Set input channel to read */

	ADCSRA |= (1<<ADSC);		/* Start conversion */
	while((ADCSRA&(1<<ADIF))==0);	/* Monitor end of conversion interrupt */
	
	_delay_us(10);
	AinLow = (int)ADCL;		/* Read lower byte*/
	Ain = (int)ADCH*256;		/* Read higher 2 bits and 
					Multiply with weight */
	Ain = Ain + AinLow;				
	return(Ain);			/* Return digital value*/
}


int main(void)
{
	ADC_Init();
	PWM_Init();
	int adc_value;
	/*float voltage_value, pressure_value; */
	
	while(1)
	
	{
		adc_value = ADC_Read(0);	/* Read ADC channel 0 */
		
		
		if (adc_value > threshold_adc_value)
		{
			OCR1A = valve_opened;	/* Set servo shaft at -90° position */
			_delay_ms(2000); /* open for 2 s to avoid unstable operation */
		}
		else
		{
			OCR1A = valve_closed;	/* Set servo at +90° position */
			_delay_ms(100);
		}


	}

}


