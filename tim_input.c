//tim_input.c
/*26.04.2017 ��������� ��� �� ���� ��� ��������� ������.
 *27.04.2017 ��������� ��� �� ���� ��� ��������� ����� ���.
 */
#include <tim_input.h>

#define  TIM1_EN     (RCC->APB2ENR |= RCC_APB2ENR_TIM1EN)
#define  TIM2_EN     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define  TIM3_EN     (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
#define  TIM6_EN     (RCC->APB1ENR |= RCC_APB1ENR_TIM6EN)
#define  TIM14_EN    (RCC->APB1ENR |= RCC_APB1ENR_TIM14EN) 
#define  TIM15_EN    (RCC->APB2ENR |= RCC_APB2ENR_TIM15EN)
#define  TIM16_EN    (RCC->APB2ENR |= RCC_APB2ENR_TIM16EN)
#define  TIM17_EN    (RCC->APB2ENR |= RCC_APB2ENR_TIM17EN)

#define  TIM_IN		TIM3

#define TimerTick_1ms	(48000-1)
//***********************************************************************
//��������� ������� ���������� ���������.
//��� ������� ����������� ���14_��1 �� ������ �������� ������� �� ��� �������
//������ ������� ������������ � ����������.
void tim_input_init()
{
//---������������� ������ CHy TIMx ��� ������������� �-���	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//PA7 - �� ���14 & MCO (Alternate function - AF0)
	GPIOA->MODER |= GPIO_MODER_MODER7_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	//GPIOA->AFR[0] - ������� ������� (0-7 ����)
	//GPIOA->AFR[1] - ������� ������� (8-15 ����)
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL7 & 0x40000000);//� ��� ������� ������� (AFRL) ��� ���� ��7 (GPIO_AFRL_AFRH7)��������� ��-4 (0x40000000)
	//PA6 - (output)
	GPIOA->MODER |= GPIO_MODER_MODER6_0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	GPIOA->MODER |= GPIO_MODER_MODER2_0;
	GPIOA->MODER |= GPIO_MODER_MODER3_0;
	//--------------------
//--��������� �I�14 � ����� ������� �������� �������:
    //��������� ������������ ���14
	TIM14_EN;

	//����� ��������� �������
	TIM14->PSC = TimerTick_1ms;//������������ ������ ��� �� ������ ����� ������ 1��(������������)

	//����������� ����� ������ ������ �� ����(���������� ������ �������� � ��� TIMx_CCMR1_CC1S[1:0]):
	//01 - ������ ������� TI1
	//10 - ������ ������� TI2
	//01 - ������ ������� TRC
	TIM14->CCMR1 |= TIM_CCMR1_CC1S_0;//���� ��� ����� �� ����

    //����������� ������ �� �������� (������� �������)
    //TIM4->CCMR1 |= TIM_CCMR1_IC1F_0;
    TIM14->CCMR1 &= ~TIM_CCMR1_IC1F;//������ ����

    //����������� ������������ �� ��������
    //TIM4->CCMR1 |= TIM_CCMR1_IC1PSC_0;
    TIM14->CCMR1 &= ~TIM_CCMR1_IC1PSC;//������������ ����

    //������ �� ������ ��� ����� ������� TIxFP1
	TIM14->CCER &= ~TIM_CCER_CC1P;//�� ������

    //��� ������ �������
	TIM14->CCER |= TIM_CCER_CC1E;
    //----------------------------------

	/* ��������� ������� ������������ ���������� �� ������� */
	TIM14->DIER |= TIM_DIER_CC1IE; //��������� ���������� �� �������
	/* �������� ������ */
	TIM14->CR1 |= TIM_CR1_CEN;
	/* ��������� ���������� ������� TIM */
	NVIC_EnableIRQ(TIM14_IRQn);
	//---------------------------------
}
//***********************************************************************
//***********************************************************************
//�-�� ����������� ���14_��1 �� ������ �������� ������� �� ��� �������
//����������� ��������� ������������ � ����������
void tim_pwm_input_init()
{
    // ������������� ����� CH1 TIM3 ��� ������������� �-���

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//PA6 - �� ���3_CH1 (Alternate function - AF1)
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	//GPIOA->AFR[0] - ������� ������� (0-7 ����)
	//GPIOA->AFR[1] - ������� ������� (8-15 ����)
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL6 & 0x01000000);//� ��� ������� ������� (AFRL) ��� ���� ��6 (GPIO_AFRL_AFRL6)��������� ��-1 (0x01000000)
	//PA7 - (output)
	GPIOA->MODER |= GPIO_MODER_MODER7_0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	GPIOA->MODER |= GPIO_MODER_MODER2_0;
	GPIOA->MODER |= GPIO_MODER_MODER3_0;
	//--------------------
    //��������� �I�3 � ������ ������� �������� ������� PWM:
    //��������� ������������ ���3
	TIM3_EN;

	//����� ��������� �������
	TIM3->PSC = TimerTick_1ms;

	//����������� ����� ������ ������ CH1 & CH2 �� ����:
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;//01 - ������ ������� TI1
	TIM3->CCMR1 |= TIM_CCMR1_CC2S_1;//10 - ������ ������� TI1

    //����������� ������ �� �������� (������� �������)
    //TIM4->CCMR1 |= TIM_CCMR1_IC1F_0;
    TIM3->CCMR1 &= ~TIM_CCMR1_IC1F;//������ ����
    TIM3->CCMR1 &= ~TIM_CCMR1_IC2F;//������ ����

    //����������� ������������ �� ��������
    //TIM4->CCMR1 |= TIM_CCMR1_IC1PSC_0;
    TIM3->CCMR1 &= ~TIM_CCMR1_IC1PSC;//������������ ����
    TIM3->CCMR1 &= ~TIM_CCMR1_IC2PSC;//������������ ����

    //������ �� ������ ��� ����� ������� TIxFP1
	TIM3->CCER &= ~TIM_CCER_CC1P;//�� ������
	TIM3->CCER |= TIM_CCER_CC2P;//�� �����

	//??? TIxFP1
	TIM3->SMCR |= TIM_SMCR_SMS_2;//SMS = 100
	TIM3->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0;//TS = 101

    //��� ������ �������
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CCER |= TIM_CCER_CC2E;
    //----------------------------------

	/* ��������� ������� ������������ ���������� �� ������� */
	TIM3->DIER |= TIM_DIER_CC1IE; //��������� ���������� �� �������
	/* �������� ������ */
	TIM3->CR1 |= TIM_CR1_CEN;
	/* ��������� ���������� ������� TIM */
	NVIC_EnableIRQ(TIM3_IRQn);
	//---------------------------------
}
//***********************************************************************
