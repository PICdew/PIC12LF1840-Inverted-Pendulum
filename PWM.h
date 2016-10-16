#include <xc.h> // include standard header file

long int PWM_Set(long int _u)
{
	if(_u>1023)_u=1023;			//�]�w�W��, can't use 1024, that is 11 bit!
	else if(_u<-1023)_u=-1023;	//�]�w�U��
   
	if( _u>0)
	{
		//if(STR1A==0 && STR1B==1)__delay_us(1);	//���ϩ���
		STR1A=1;STR1B=0;
	}
	else if( _u<0)
	{
	  //if(STR1A==1 && STR1B==0)__delay_us(1);	//���ϩ���
		STR1A=0;STR1B=1;_u=-_u;
	}
	else if(_u==0){STR1A=0;STR1B=0;}

	//10bit PWM ����8bit�bCCPR1L, �C2bit�bCCP1CONbits.DC1B
	CCPR1L = _u>>2;	//����8bit
	CCP1CONbits.DC1B = _u&0x03;	//���C2bit
	
	//�Yu�쥻�O�t��, return���٭�^�h
	//�Yu�쥻���O�t, return�ɭ�ʤ���
	return (0==STR1A && 1==STR1B)? -_u:_u;
}
