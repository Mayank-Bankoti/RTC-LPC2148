#include "lpc214x.h"
#include "stdio.h"
#include "UART0.h"

unsigned int alarm, flag;

void RTC_ISR(void) __irq 
{
	if (ILR & 0x01)
	{
		flag = 1;
		ILR = ILR | 0x01;
	}
	if (ILR & 0x02)
	{
		alarm = 1;
		ILR = ILR | 0x02;
	}
	VICVectAddr = 0;
}

typedef struct
{
	unsigned int seconds;
	unsigned int minutes;
	unsigned int hours;
	unsigned int day_of_month;
	unsigned int day_of_week;
	unsigned int day_of_year;
	unsigned int month;
	unsigned int year;
}RTC_Time;

void RTC_Set_Time( RTC_Time Set_Time)
{
	SEC = Set_Time.seconds;
	MIN = Set_Time.minutes;
	HOUR = Set_Time.hours;
	DOM = Set_Time.day_of_month;
	DOW = Set_Time.day_of_week;
	DOY = Set_Time.day_of_year;
	MONTH = Set_Time.month;
	YEAR = Set_Time.year;
}

void RTC_Set_Alarm_Time( RTC_Time Alarm_Time)
{
	ALSEC = Alarm_Time.seconds;
	ALMIN = Alarm_Time.minutes;
	ALHOUR = Alarm_Time.hours;
	ALDOM = Alarm_Time.day_of_month;
	ALDOW = Alarm_Time.day_of_week;
	ALDOY = Alarm_Time.day_of_year;
	ALMON = Alarm_Time.month;
	ALYEAR = Alarm_Time.year;
}

RTC_Time RTC_Get_Time(void)
{
	RTC_Time time;
	time.seconds = SEC;
	time.minutes = MIN;
	time.hours = HOUR;
	time.day_of_month = DOM;
	time.day_of_week = DOW;
	time.day_of_year = DOY;
	time.month = MONTH;
	time.year = YEAR;
	return time;
}

int main(void)
{

	unsigned int i;

		RTC_Time set_time, alarm_time, current_time;
	char timestr[30], datestr[30];
	alarm = 0;
	flag = 0;
	IO0DIR = 0x00000010; /* P0.4 as output pin for LED */
	UART0_init();
	PCONP = (PCONP | (1<<9)); /* PCRTC = 1 */
	/* The RTC registers cannot be written to unless we make PCRTC = 1 */
	ILR = 0x0; /* No RTC interrupts */
	CCR = 0x12; /* 32.768kHz clock and Reset Clock Tick Counter */
	CCR = 0x10; 
	CIIR = 0x00; /* No interrupts */
	AMR = 0x00; /* Alarm registers not masked */
	VICVectAddr0 = (unsigned) RTC_ISR;
	VICVectCntl0 = 0x0000002D;
	VICIntEnable = 0x00002000;
	VICIntSelect = 0x00000000;
	set_time.seconds = 00;
	set_time.minutes = 25;
	set_time.hours = 11;
	set_time.day_of_month = 6;
	set_time.day_of_week = 5;
	set_time.day_of_year = 279;
	set_time.month = 10;
	set_time.year = 2017;
	RTC_Set_Time(set_time);
	CIIR = 0x01; /* Seconds value increment interrupt */
	alarm_time.seconds = 15;
	alarm_time.minutes = 25;
	alarm_time.hours = 11;
	alarm_time.day_of_month = 6;
	alarm_time.day_of_week = 5;
	alarm_time.day_of_year = 279;
	alarm_time.month = 10;
	alarm_time.year = 2017;
	RTC_Set_Alarm_Time(alarm_time);	
	CCR = 0x11; /* 32.768kHz clock and clock Enable */
	ILR = 0x03; /* RTC interrupts enabled */
	IO0CLR = 0x00000010;	

	while(1)
	{
		if(alarm == 1)
		{
			current_time = RTC_Get_Time();
			sprintf(timestr,"Alarm!!!: %d:%d:%d \r\n",current_time.hours,current_time.minutes,current_time.seconds);
			UART0_SendString(timestr);
			for(i=0;i<10;i++)
			{
				IO0SET = 0x00000010;
				delay_ms(300);
				IO0CLR = 0x00000010;
				delay_ms(300);
			}
			alarm = 0;
		}
		if (flag == 1)
		{
			current_time = RTC_Get_Time();
			sprintf(timestr,"Time: %d:%d:%d ",current_time.hours,current_time.minutes,current_time.seconds);
			sprintf(datestr,"Date: %d/%d/%d \r\n",current_time.day_of_month,current_time.month,current_time.year);
			UART0_SendString(timestr);
			UART0_SendString(datestr);
			flag = 0;
		}
	}
}
