

#include <reg52.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "1602.h"
#include "delay.h"
#define uchar unsigned char
#define uint unsigned int


							
sbit SDA=P1^1;	//����TEA5767�����ݶ˿�	���и��Ķ˿��������Լ��Ŀ�����						
sbit SCL=P1^0;	//����TEA5767��ʱ�Ӷ˿�								
sbit KEY1=P3^3;		//Ƶ�����Ӱ����˿�							
sbit KEY2=P3^4;		//Ƶ�ʼ�С�����˿�							


unsigned char temp[16];//������ʾ������ʱ�洢����
/////////////////////////////////////////////////////////////////////////////////////////////////////��

unsigned char radio_write_data[5]={0x30,0x50,0x70,0x16,0x80};        //Ҫд��TEA5767������,101.1M
unsigned long frequency=101100;	//������ʼ���ĵ�̨Ƶ��Ϊ101.1MHz
unsigned int pll=101100;		//����Ƶ�ʺϳ�ʱ�õı���





void Delayus(unsigned char x)	//��ʱ����
{
 for(;x>0;x--);
}

void iic_start()		//��ʼ�ź�   
{
 SDA=1;
 Delayus(4);
 SCL=1;
 Delayus(4);
 SDA=0;
 Delayus(4);
 SCL=0;
 Delayus(4);
}

void iic_stop()			 //ֹͣ�ź�
{
 SCL=0;
 Delayus(4);
 SDA=0;
 Delayus(4);
 SCL=1;
 Delayus(4);
 SDA=1;
 Delayus(4);
}

void iic_ack()			  //Ӧ���Ӧ
{
 SDA=0;
 Delayus(4);
 SCL=1;
 Delayus(4);
 SCL=0;
 Delayus(4);
 SDA=1;
 Delayus(4);
}


bit iic_testack()			// ���Է��ش�
{
 bit ErrorBit;
 SDA=1;
 Delayus(4);
 SCL=1;
 Delayus(4);
 ErrorBit=SDA;
 Delayus(4);
 SCL=0;
 return ErrorBit;
}

void iic_write8bit(unsigned char input)		  	//д8λ��������
{
 unsigned char temp;
 for(temp=8;temp>0;temp--)
 {
  SDA=(bit)(input&0x80);
  Delayus(4);
  SCL=1;
  Delayus(4);
  SCL=0;
  Delayus(4);
  input=input<<1;
 }
}


void delays (unsigned char b)	//���������õ���ʱ����
{
  unsigned char i;
  for(b;b>0;b--)
  for(i=0;i<250;i++);
}

void radio_write(void)		   //������д����
{
    unsigned char i;
    iic_start();
    iic_write8bit(0xc0);        //TEA5767д��ַ
    if(!iic_testack())
    {
        for(i=0;i<5;i++)
        {
            iic_write8bit(radio_write_data[i]);
            iic_ack();
        }
    }
    iic_stop();    
}


 //����ʾ��Ƶ����ú�д��TEA5767оƬ����ʵ�ֵ���,���ÿ���TEA5767������̨�����λ:SM,SUD
void search()				   //��̨
{        
    pll=(unsigned int)((float)((frequency+225))/(float)8.192);    //����ϳ�Ƶ�����ݣ�Ƶ�ʵ�λ:k
    radio_write_data[0]=pll/256;	//����õ�Ƶ�ʸ�λ����д��TEA5767
    radio_write_data[1]=pll%256;	//����õ�Ƶ�ʵ�λ����д��TEA5767
    radio_write_data[2]=0x70;		//����TEA5767�����������ؾ���
    radio_write_data[3]=0x16;		//�й���ʽ������32768���������أ���������������
    radio_write_data[4]=0x80;		//6.5M���໷����ȥ����ʱ��75US
    radio_write();
}




void display(uint frq)				  //��ʾ����
{ 
  sprintf(temp,"   %5.1f MHz",((float)frq)/10);//��������ʾ����һλС���㣬���Ȱ�int����ǿ�Ʊ�Ϊfloat�ͣ�Ȼ�����10����
  LCD_Write_String(0,1,temp);//��ʾ��һ��
}

void main()				//������
{
 LCD_Init();           //��ʼ��Һ��
 DelayMs(20);          //��ʱ�������ȶ�
 LCD_Clear();          //����
 LCD_Write_String(0,0,"   FM TEA5767   ");//��ʾ��һ��
 search();
 radio_write();		   //д���ʼ����̨Ƶ��
 while(1)
 {
  	if(!KEY1) 	   //�ֶ�����Ƶ��,+0.1MHz;
  	{
  	  delays(250); //��ʱ����
	  if(!KEY1)
	  {
  	  frequency+=100;
      if(frequency>108500)	//Ƶ���������108.5MHz
          frequency=87500;	//Ƶ���趨Ϊ87.5MHz
	      search();
		  display(frequency/100);  //������ʾ
	  }
  	}

    if(!KEY2) 		//�ֶ�����Ƶ��,-0.1MHz;
    {
     delays(250);   //��ʱ����
        if(!KEY2)
	    {
        frequency-=100;
        if(frequency<87500)	//Ƶ�����С��87.5MHz
          frequency=108500;	//Ƶ���趨Ϊ108.5MHz
		  search();
		  display(frequency/100);  //������ʾ
    }
	display(frequency/100);  //������ʾ
   } 
  }
}















