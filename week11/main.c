#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"
#include "stm32f10x_tim.h"

int color[12] = {WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY};
uint16_t flag = 0;

void RCCInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
}

void GpioInit() {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure_LED;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PIN B0 = ADC_Channel_8
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ; //or OD
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure_LED.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure_LED.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure_LED.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure_LED);
}

void NvicInit() {
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

void TIM_configure() {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
   
  TIM_TimeBaseStructure.TIM_Period = 10000;
  TIM_TimeBaseStructure.TIM_Prescaler = 7200;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  ////////////////////////////////////////////////
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure_2;
  TIM_OCInitTypeDef TIM_OCInitStructure_2;

  TIM_TimeBaseStructure_2.TIM_Period = 20000;
  TIM_TimeBaseStructure_2.TIM_Prescaler = 72;                //set 50Hz
  TIM_TimeBaseStructure_2.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure_2.TIM_CounterMode = TIM_CounterMode_Down;

  TIM_OCInitStructure_2.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure_2.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure_2.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure_2.TIM_Pulse = 1500;                       // set move 0
  TIM_OC3Init(TIM3, &TIM_OCInitStructure_2);

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure_2);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
  TIM_ARRPreloadConfig(TIM3,ENABLE);
  TIM_Cmd(TIM3,ENABLE);
  
}

void changeServoM(int percent){  //(percent,move) 3.5 (-90),  7.5(0), 10(90)

  int myPulse = percent;

  TIM_OCInitTypeDef TIM_OCInitStructure_2;

  TIM_OCInitStructure_2.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure_2.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure_2.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure_2.TIM_Pulse = myPulse;
  TIM_OC3Init(TIM3, &TIM_OCInitStructure_2);

}

int t1 = 0;
int index=0;
int arr[17] = {700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,2000,2100,2200,2300};
void TIM2_IRQHandler(void) {
  t1++;
  if(t1==0xffff) t1 = 1;
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  if(flag==1){
    index++;
    changeServoM(arr[index%17]);
  }
  else{
    index--;
    if(index == -1){
      index = 16;
    }
    changeServoM(arr[index%17]);
  }
}


uint16_t x, y;
uint16_t lcd_x, lcd_y;
int main() {
  SystemInit();
  RCCInit();
  GpioInit();
  TIM_configure();
  NvicInit();
//---------------------------------
  LCD_Init();
  Touch_Configuration();
  Touch_Adjust();
  LCD_Clear(WHITE);

  while(1){
    LCD_ShowString(50, 30, "Team 02", BLACK, WHITE);
    
    LCD_DrawRectangle(100, 100, 150, 150);
    LCD_ShowString(125, 125, "BUT", BLACK, WHITE);
    
    Touch_GetXY(&x, &y, 0);
    Convert_Pos(x, y, &lcd_x, &lcd_y);
    
    
    if (flag ^ (100 < lcd_x && lcd_x < 150 && 100 < lcd_y && lcd_y < 150)) {
      flag = 1;
    }
    else { flag = 0; }
    
    if (flag == 1) {
      LCD_ShowString(50, 50, "ON  ", BLACK, WHITE);
      if (t1 % 2) {
        GPIO_SetBits(GPIOD, GPIO_Pin_2);
      }
      else {
        GPIO_ResetBits(GPIOD, GPIO_Pin_2);
      }
      if (t1 % 10 < 5) {
        GPIO_SetBits(GPIOD, GPIO_Pin_3);
      }
      else {
        GPIO_ResetBits(GPIOD, GPIO_Pin_3);
      }
    }
    else {
      GPIO_ResetBits(GPIOD, GPIO_Pin_2);
      GPIO_ResetBits(GPIOD, GPIO_Pin_3);
      LCD_ShowString(50, 50, "OFF", BLACK, WHITE);
    }

  }
}
