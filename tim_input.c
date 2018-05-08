//tim_input.c
/*26.04.2017 настройки ТИМ на вход для измерения частот.
 *27.04.2017 настройки ТИМ на вход для измерения парам ШИМ.
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
//Измерение периода следования импульсов.
//Эта функция настраивает ТИМ14_СН1 на захват входного сигнала по его фронтам
//Расчет периода производится в прерывании.
void tim_input_init()
{
//---конфигурируем выводы CHy TIMx как алтернативная ф-ция	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//PA7 - вх ТИМ14 & MCO (Alternate function - AF0)
	GPIOA->MODER |= GPIO_MODER_MODER7_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	//GPIOA->AFR[0] - младший регистр (0-7 биты)
	//GPIOA->AFR[1] - старший регистр (8-15 биты)
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL7 & 0x40000000);//в рег альтерн функций (AFRL) для пина РА7 (GPIO_AFRL_AFRH7)выставить АФ-4 (0x40000000)
	//PA6 - (output)
	GPIOA->MODER |= GPIO_MODER_MODER6_0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	GPIOA->MODER |= GPIO_MODER_MODER2_0;
	GPIOA->MODER |= GPIO_MODER_MODER3_0;
	//--------------------
//--Настройки ТIМ14 в режим захвата входного сигнала:
    //разрешаем тактирование ТИМ14
	TIM14_EN;

	//общие настройки таймера
	TIM14->PSC = TimerTick_1ms;//предделитель ставим что бы таймер тикал каждую 1мс(тактировался)

	//настраиваем режим работы канала на вход(установить нужное значение в рег TIMx_CCMR1_CC1S[1:0]):
	//01 - захват сигнала TI1
	//10 - захват сигнала TI2
	//01 - захват сигнала TRC
	TIM14->CCMR1 |= TIM_CCMR1_CC1S_0;//этим вкл канал на вход

    //настраиваем фильтр вх сигналов (частоту выборки)
    //TIM4->CCMR1 |= TIM_CCMR1_IC1F_0;
    TIM14->CCMR1 &= ~TIM_CCMR1_IC1F;//фильтр выкл

    //настраиваем предделитель вх сигналов
    //TIM4->CCMR1 |= TIM_CCMR1_IC1PSC_0;
    TIM14->CCMR1 &= ~TIM_CCMR1_IC1PSC;//предделитель выкл

    //захват по фронту или срезу сигнала TIxFP1
	TIM14->CCER &= ~TIM_CCER_CC1P;//по фронту

    //вкл канала захвата
	TIM14->CCER |= TIM_CCER_CC1E;
    //----------------------------------

	/* Разрешаем таймеру генерировать прерывание по захвату */
	TIM14->DIER |= TIM_DIER_CC1IE; //разрешаем прерывание от таймера
	/* Включаем таймер */
	TIM14->CR1 |= TIM_CR1_CEN;
	/* Разрешаем прерывания таймера TIM */
	NVIC_EnableIRQ(TIM14_IRQn);
	//---------------------------------
}
//***********************************************************************
//***********************************************************************
//ф-ци настраивает ТИМ14_СН1 на захват входного сигнала по его фронтам
//регистрация измерений производится в прерывании
void tim_pwm_input_init()
{
    // конфигурируем вывод CH1 TIM3 как алтернативная ф-ция

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//PA6 - вх ТИМ3_CH1 (Alternate function - AF1)
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	//GPIOA->AFR[0] - младший регистр (0-7 биты)
	//GPIOA->AFR[1] - старший регистр (8-15 биты)
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL6 & 0x01000000);//в рег альтерн функций (AFRL) для пина РА6 (GPIO_AFRL_AFRL6)выставить АФ-1 (0x01000000)
	//PA7 - (output)
	GPIOA->MODER |= GPIO_MODER_MODER7_0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	GPIOA->MODER |= GPIO_MODER_MODER2_0;
	GPIOA->MODER |= GPIO_MODER_MODER3_0;
	//--------------------
    //Настройки ТIМ3 в режиме захвата входного сигнала PWM:
    //разрешаем тактирование ТИМ3
	TIM3_EN;

	//общие настройки таймера
	TIM3->PSC = TimerTick_1ms;

	//настраиваем режим работы канала CH1 & CH2 на вход:
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;//01 - захват сигнала TI1
	TIM3->CCMR1 |= TIM_CCMR1_CC2S_1;//10 - захват сигнала TI1

    //настраиваем фильтр вх сигналов (частоту выборки)
    //TIM4->CCMR1 |= TIM_CCMR1_IC1F_0;
    TIM3->CCMR1 &= ~TIM_CCMR1_IC1F;//фильтр выкл
    TIM3->CCMR1 &= ~TIM_CCMR1_IC2F;//фильтр выкл

    //настраиваем предделитель вх сигналов
    //TIM4->CCMR1 |= TIM_CCMR1_IC1PSC_0;
    TIM3->CCMR1 &= ~TIM_CCMR1_IC1PSC;//предделитель выкл
    TIM3->CCMR1 &= ~TIM_CCMR1_IC2PSC;//предделитель выкл

    //захват по фронту или срезу сигнала TIxFP1
	TIM3->CCER &= ~TIM_CCER_CC1P;//по фронту
	TIM3->CCER |= TIM_CCER_CC2P;//по срезу

	//??? TIxFP1
	TIM3->SMCR |= TIM_SMCR_SMS_2;//SMS = 100
	TIM3->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0;//TS = 101

    //вкл канала захвата
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CCER |= TIM_CCER_CC2E;
    //----------------------------------

	/* Разрешаем таймеру генерировать прерывание по захвату */
	TIM3->DIER |= TIM_DIER_CC1IE; //разрешаем прерывание от таймера
	/* Включаем таймер */
	TIM3->CR1 |= TIM_CR1_CEN;
	/* Разрешаем прерывания таймера TIM */
	NVIC_EnableIRQ(TIM3_IRQn);
	//---------------------------------
}
//***********************************************************************
