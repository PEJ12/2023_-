#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "core_cm3.h"
#include "stm32f10x_adc.h"
#include "misc.h"

void RCC_Configure(void);
void GPIO_Configure(void);
void USART1_Init(void);
void USART2_Init(void);
void NVIC_Configure(void);

uint16_t waterValue1, waterValue2, waterValue3, waterValue4; // 수위센서 측정 값 저장하는 변수
//---------------------------------------------------------------------------------------------------
typedef struct {
    uint8_t relay_number;
    bool isTriggered;
} RelayConfig;

RelayConfig relay_configs[4];

uint16_t pinList[] = { GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11 };

uint16_t pressure_sensor_value; //압력센서 값을 저장할 글로벌 변수입니다.



void RCC_Configure(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

// PC2, PC3, PC4, PC5
// channel 12, 13, 14, 15
void GPIO_Configure(void) {
    /********* PART2 **********/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /********* PART3 **********/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void ADC_Configure(void) {
    ADC_InitTypeDef ADC_12;
    ADC_12.ADC_ContinuousConvMode = ENABLE;
    ADC_12.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_12.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_12.ADC_Mode = ADC_Mode_Independent;
    ADC_12.ADC_NbrOfChannel = 1;
    ADC_12.ADC_ScanConvMode = DISABLE;

    ADC_Init(ADC1, &ADC_12);

    // 수정
    //void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4, ADC_SampleTime_239Cycles5);

    // 무게 센서 (PC1)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 5, ADC_SampleTime_239Cycles5);

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void NVIC_Configure(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // need modify

    // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_Init(&NVIC_InitStructure);

}

void ADC1_2_IRQHandler() {
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);

        // 각 채널에 대한 변환 수행
        for (uint8_t channel = ADC_Channel_11; channel <= ADC_Channel_15; channel++) {
            ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);

            // 변환이 완료될 때까지 기다림
            while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

            // 각 채널의 값을 읽음
            switch (channel) {
            case ADC_Channel_11: pressure_sensor_value = ADC_GetConversionValue(ADC1); break;
            case ADC_Channel_12: waterValue1 = ADC_GetConversionValue(ADC1); break;
            case ADC_Channel_13: waterValue2 = ADC_GetConversionValue(ADC1); break;
            case ADC_Channel_14: waterValue3 = ADC_GetConversionValue(ADC1); break;
            case ADC_Channel_15: waterValue4 = ADC_GetConversionValue(ADC1); break;
            }
        }
    }

}

// 원료 부족한지 확인
bool checkIsEnough() {
    uint16_t threshold = 1500; // 임계값 수정 필요
    if (waterValue1 <= threshold && waterValue2 <= threshold && waterValue3 <= threshold && waterValue4 <= threshold) {
        //USART_SendData(USART2, "lack");
        return false;
    }
    // 동작 안되면 else 빼기
    else {
        return true;
    }
}

/************************** part4 ***********************************/

/*릴레이 관련 초기화 함수*/
void InitializeGPIOForRelays(void);
void InitializeRelayConfigs(void);

void InitializeGPIOForRelays(void) { // Relay용 GPIO핀들을 Push-Pull 출력 모드로 설정합니다.
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 릴레이용 GPIO: 일단은 GPIOE로 설정해둘게요...*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //GPIOE 사용하였습니다.

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11; //8,9,10,11
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void InitializeRelayConfigs(void) {
    relay_configs[0] = (RelayConfig){ .relay_number = 1, .isTriggered = false };
    relay_configs[1] = (RelayConfig){ .relay_number = 2, .isTriggered = false };
    relay_configs[2] = (RelayConfig){ .relay_number = 3, .isTriggered = false };
    relay_configs[3] = (RelayConfig){ .relay_number = 4, .isTriggered = false };
}

//void Delay(uint8_t delayTime) {
 // uint8_t i;

//  for (i = 0; i < delayTime * 10; i++) {}
//  printf("%d", delayTime);
// }

void Delay(uint32_t  nTime) {
    printf("%d\n", nTime);
    for (; nTime > 0; nTime--);
}

void OpenRelayWithWaterPump(uint8_t relay_number, uint8_t delayTime) { //수중 펌프를 동작시키는 함수입니다.
    uint8_t idx = relay_number;
    //if (!relay_configs[idx].isTriggered) {
    GPIOE->BRR = pinList[idx]; // PE2 ON (릴레이 i 열림) -> 수중펌프 i번 동작
   // relay_configs[idx].isTriggered = true;
    printf("before DELAY\n");
    Delay(delayTime * 10);
    printf("before BRR\n");
    GPIOE->BSRR = pinList[idx]; // PE2 ON (릴레이 i close) -> 수중펌프 i번 동작중지
   // relay_configs[idx].isTriggered = false;

    printf("after BRR\n");
    //}
}

/************************** part3 ***********************************/
void ActivatePressureSensor(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /********* PART5 **********/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 노란색 LED(B4) 켜기, 붉은색 LED(A2) 끄기
    GPIOB->BRR = GPIO_Pin_4;
    GPIOA->BSRR = GPIO_Pin_2;

    // 압력 센서 활성화
    //GPIO_SetBits(GPIOC, GPIO_Pin_1);
    GPIOC->BSRR = GPIO_Pin_1;

    // 메인 컨트롤러에 신호 보내기
    //GPIO_SetBits(GPIOx, SIGNAL_PIN);
}


int main(void) {
    SystemInit();

    RCC_Configure();

    GPIO_Configure();

    ADC_Configure();

    NVIC_Configure();

    while (1) {
        //printf("%d\n", waterValue1);
        //printf("%d\n", waterValue2);
        //printf("%d\n", waterValue3);
        //printf("%d\n", waterValue4);


        bool isEnough = checkIsEnough();

        printf("isEnough: %d\n", isEnough);

        if (isEnough) {
            ActivatePressureSensor(); /************* part3 **************/

            InitializeGPIOForRelays(); // 릴레이용 GPIO 초기화

            InitializeRelayConfigs(); // 각각 수중 펌프 모터에 연결될 릴레이 설정(번호 및 isTriggered)

            uint8_t ampleCombination[] = { 20, 30, 10, 10 }; // 블루투스로 들어온 앰플 조합 (예시입니다)


            GPIOE->BRR = pinList[0]; // PE2 ON (릴레이 i 열림) -> 수중펌프 i번 동작
          // relay_configs[idx].isTriggered = true;
            printf("before DELAY\n");
            Delay(ampleCombination[0] * 1000000);
            printf("before BRR\n");
            GPIOE->BSRR = pinList[0]; // PE2 ON (릴레이 i close) -> 수중펌프 i번 동작중지
            //for (int i = 0; i < sizeof(ampleCombination); i++){
            //  OpenRelayWithWaterPump(0, ampleCombination[0] * 100000);
            //}

            break;
        }
    }


    /*
    while (1) {
      printf("value1:%d", waterValue1);
      printf("value2:%d\n", waterValue2);
      printf("value3:%d\n", waterValue3);
    }
    */

    return 0;
}
