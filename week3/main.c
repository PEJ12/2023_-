#include "stm32f10x.h"
#include <stdio.h>

// GPIOD에 clock 인가
#define RCC_APB2ENR (*(volatile unsigned int*) 0x40021018) // 0x4002 1000 + 0x18

// Port D : configuration register low
#define GPIOD_CRL (*(volatile unsigned int *) 0x40011400) // D (2, 3, 4, 7 => CRL)

// Port D  : ouput -> BRR : reset, BSRR : reset or set
#define GPIOD_BRR (*(volatile unsigned int *) 0x40011414) // 0x40011400 + 0x14
#define GPIOD_BSRR (*(volatile unsigned int *) 0x40011410) // 0x40011400 + 0x10

void delay(__IO uint32_t nCount) {
	for(; nCount != 0; nCount --);
}

int main(void) {
    // PORT D ENABLE
    RCC_APB2ENR |= 0x20; //0x0000 0000 0010 0000

    // PORT D : General purpose output push-pull
    GPIOD_CRL &= ~0xF00FFF00; //2,3,4,7번째 비트에 해당하는 부분을 0으로 초기화
    GPIOD_CRL |= 0x30033300; //0011(=3)을 2,3,4,7번째 비트에 넣기

    // 제일 처음에 모든 LED 끄기
    GPIOD_BSRR &= 0x00000000;
    GPIOD_BSRR |= 0x9C;

    uint8_t flag = 0; // For PD2, PD3, PD4, PD7

    while(1) {
        if (flag) { //압력센서로 수중 펌프 모터 제어가 잘돼서 앰플생성을 성공했다면
            GPIOD_BSRR |= 0x80; // PD7 ON
            delay(5000000);
            GPIOD_BRR |= 0x80; // PD7 OFF

            flag = 0;
        }
    }
}
