#include <xc.h> // include standard header file

long int PID_Control(long int Err,int Kp,int Ki,int Kd,long int LIMIT,int Position,unsigned int AnalogValue,unsigned char ScaleRotateBit)
{
	static long int Ki_Sum=0,Kd_Diff=0;
	static long int tempErr=0;
	long int Kp_OutValue,Ki_OutValue,Kd_OutValue; //�ȦsPID�T�حp�⪺���G
	long int K_OutValue=0;//�̫�[�`�b�o��
	
  Err=Err<<ScaleRotateBit;	//�ثש�j
    
  //*************��Ҷ}�l***********//
	Kp_OutValue = Kp*Err;
	//*************��ҵ���***********//
	
	//*************�n���}�l***********//
	Ki_Sum += Err;
	if	   (Ki_Sum> LIMIT)Ki_Sum= LIMIT;
	else if(Ki_Sum<-LIMIT)Ki_Sum=-LIMIT;
	
	if((Position-5)<=AnalogValue && (Position+5)>=AnalogValue)
		Ki_Sum=0;//�n�����m,�n������0,�N���A��X��Ki_OutValue
	else
		Ki_OutValue = (Ki*Ki_Sum)>>ScaleRotateBit;
	//*************�n������***********//
	
	//*************�L���}�l***********//
	Kd_Diff = (Err-tempErr)<<ScaleRotateBit;
	
	tempErr=Err;
	
	if	   (Kd_Diff> LIMIT)Kd_Diff= LIMIT;
	else if(Kd_Diff<-LIMIT)Kd_Diff=-LIMIT;
	
	Kd_OutValue = Kd*Kd_Diff;
	//*************�L������***********//
	
	K_OutValue = Kp_OutValue + Ki_OutValue + Kd_OutValue;
	K_OutValue = K_OutValue>>ScaleRotateBit;	//�ث��٭�(�`�Nsigned bit�򥢰��D!)
 
  return K_OutValue;
} 