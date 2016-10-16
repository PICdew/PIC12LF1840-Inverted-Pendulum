#include <xc.h> // include standard header file

unsigned int Read_ADC_Value(void)
{
	unsigned int ADCValue;
	
	//Set ADCON1
	ADCON1=0x00;		//��8+�C2,Fosc/2,Vref=Vdd
	
	//Set ADCON0
	ADCON0bits.CHS=0x01;	//��q�D1
	ADCON0bits.ADON=1;		//����
	__delay_us(5);			//�R�q
	
	ADCON0bits.GO = 1;              // start conversion
	while (ADCON0bits.GO);          // wait for conversion to finish
	ADCValue = (ADRESH<<2)|(ADRESL>>6); 
	ADCON0bits.ADON=PIR1bits.ADIF=0;
	return (ADCValue);              // return the 10bit result in a single variable
}