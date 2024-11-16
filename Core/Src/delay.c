#include "delay.h"

void HAL_Delay_us(uint32_t us)
{
    uint32_t start_val, tickn, delays, wait;

    start_val = SysTick->VAL;
    tickn = HAL_GetTick();
    delays = us * 72;
    if (delays > start_val) {
        while (HAL_GetTick() == tickn)
            ;
        wait = 72000 + start_val - delays;
        while (wait < SysTick->VAL)
            ;
    }
    else {
        wait = start_val - delays;
        while (wait < SysTick->VAL && HAL_GetTick() == tickn)
            ;
    }
}

void HAL_Delay_us2(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*9; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}