
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#include "misc.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void Delay(void);
void sendDataUART1(uint16_t data);


//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
	// TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
	/* LED port clock enable */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	// TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'

    /* LED pin setting*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

}




void Delay(void) {
	int i;

	for (i = 0; i < 2000000; i++) {}
}


void turnOnLed(int index){
    uint16_t pinList[] = {GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_7};

    GPIO_SetBits(GPIOD, pinList[index]);
    Delay();
    GPIO_ResetBits(GPIOD, pinList[index]);
}


int main(void)
{

    SystemInit();

    RCC_Configure();

    GPIO_Configure();

 
    int flag = 0;

    while (1) {
    	// TODO: implement 
        GPIO_ResetBits(GPIOD, GPIO_Pin_2);
        GPIO_ResetBits(GPIOD, GPIO_Pin_3);
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        GPIO_ResetBits(GPIOD, GPIO_Pin_7);

        
        if (flag) {
            turnOnLed(3);
            flag = 0;
        }

    	Delay();
    }
    return 0;
}

