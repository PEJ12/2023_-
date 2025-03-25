#include "stm32f10x.h"
#include <stdio.h>

// GPIO, B, C, D에 clock 인가
#define RCC_APB2ENR (*(volatile unsigned int*) 0x40021018) // 0x4002 1000 + 0x18

// Port  B, C : configuration register low 푸쉬버튼 스위치 주소 선언
#define GPIOB_CRH (*(volatile unsigned int *) 0x40010C04) // B (10 => CRH) Key2
#define GPIOC_CRL (*(volatile unsigned int *) 0x40011000) // C (4 => CRL) KEY1
#define GPIOC_CRH (*(volatile unsigned int *) 0x40011004) // C (13 => CRH) KEY13
#define GPIOD_CRL (*(volatile unsigned int *) 0x40011400) // D (2, 3, 4, 7 => CRL)

// Port B, C : input data register
#define GPIOB_IDR (*(volatile unsigned int *) 0x40010C08) // B 0x40010C00 + 0x08
#define GPIOC_IDR (*(volatile unsigned int *) 0x40011008) // C 0x40011000 + 0x08

// Port D  : ouput -> BRR : reset, BSRR : reset or set
#define GPIOD_BRR (*(volatile unsigned int *) 0x40011414) // 0x40011400 + 0x14
#define GPIOD_BSRR (*(volatile unsigned int *) 0x40011410) // 0x40011400 + 0x10

void delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--);
}

//1번모터를 PD2, 2번모터를 PD3로하자

int main(void) {
    // PORT D ENABLE
    RCC_APB2ENR |= 0x20; //0x0000 0000 0010 0000

    // PORT C ENABLE
    RCC_APB2ENR |= 0x10; //0x0000 0000 0001 0000

    // PORT B ENABLE
    RCC_APB2ENR |= 0x8; //0x0000 0000 0000 1000

    // PORT B : Input pull up, pull down
    GPIOB_CRH &= ~0x00000F00;  //10번째 비트에 해당하는 부분을 0으로 초기화
    GPIOB_CRH |= 0x00000800; //1000(=8)을 10번째 비트에 넣기

    // PORT C : Input pull up, pull down
    GPIOC_CRL &= ~0x000F0000; //4번째 비트에 해당하는 부분을 0으로 초기화
    GPIOC_CRL |= 0x00080000; //1000(=8)을 4번째 비트에 넣기
    GPIOC_CRH &= ~0x00F00000; //13번째 비트에 해당하는 부분을 0으로 초기화
    GPIOC_CRH |= 0x00800000; //1000(=8)을 13번째 비트에 넣기

    // PORT D : General purpose output push-pull
    GPIOD_CRL &= ~0x0000FF00; //2,3번째 비트에 해당하는 부분을 0으로 초기화
    GPIOD_CRL |= 0x00003300; //0011(=3)을 2,3번째 비트에 넣기

    while (1) {
        if (~GPIOC_IDR & (0x1 << 4)) { // KEY1

            GPIOD_BSRR |= 0x04; // PD2 ON
            GPIOD_BSRR |= 0x08; // PD3 ON

            delay(10000000);

            GPIOD_BRR |= 0x04; // PD2 OFF
            GPIOD_BRR |= 0x08; // PD3 OFF
        }

        if (~GPIOB_IDR & (0x1 << 10)) { // KEY2
            GPIOD_BSRR |= 0x04; // PD2 ON

            delay(10000000);

            GPIOD_BRR |= 0x04; // PD2 OFF
        }

        if (~GPIOC_IDR & (0x1 << 13)) { // KEY3
            GPIOD_BSRR |= 0x08; // PD3 ON

            delay(10000000);

            GPIOD_BRR |= 0x08; // PD3 OFF
        }
    }
}
