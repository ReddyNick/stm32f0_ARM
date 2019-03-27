/*
 * It is just simple and pure template project
 * It does absolutely nothing and indicates that toolchain
 * is installed correctly.
 */

#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_gpio.h"

//#define FLASH_0LAT_DELAY0LAT

static void rcc_config(void)
{
    /* Set FLASH latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

    /* Enable HSI and wait for activation*/
    LL_RCC_HSI_Enable();
    while (LL_RCC_HSI_IsReady() != 1);

    /* Main PLL configuration and activation */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2,
                                LL_RCC_PLL_MUL_12);

    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1);

    /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    /* Set APB1 prescaler */
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    /* Update CMSIS variable (which can be updated also
     * through SystemCoreClockUpdate function) */
    SystemCoreClock = 48000000;
}
#define a_pin				LL_GPIO_PIN_0
#define b_pin				LL_GPIO_PIN_1
#define c_pin				LL_GPIO_PIN_2
#define d_pin				LL_GPIO_PIN_3
#define e_pin				LL_GPIO_PIN_4
#define f_pin				LL_GPIO_PIN_5
#define g_pin				LL_GPIO_PIN_6


static void gpio_config(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);	//LED

	//indicators
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);


	//button
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT); 

	// B-Port
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
	
}

void delay(void)
{
	for (int i = 0; i < 500; i++);
}


__attribute__((naked)) static void delay_2(void)
{
    asm ("push {r7, lr}");
    asm ("ldr r6, [pc, #8]");
    asm ("delay_loop:");
    asm ("sub r6, #1");
    asm ("cmp r6, #0");
    asm ("bne delay_loop");
    asm ("pop {r7, pc}");
    
    asm (".word 0x4e20"); //10ms
}

void show_digit(uint8_t digit)
{
	/*
     * Put all pins for indicator together (for segments only)
     */
    static uint32_t mask = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | \
                           LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | \
                           LL_GPIO_PIN_6;

    static const uint32_t decoder[] = {
        LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
        LL_GPIO_PIN_4 | LL_GPIO_PIN_5, 										// 0 	
        LL_GPIO_PIN_1 | LL_GPIO_PIN_2, 										// 1
        LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_6 | LL_GPIO_PIN_4 | \
        LL_GPIO_PIN_3, 														// 2
        LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_6 | LL_GPIO_PIN_2 | \
        LL_GPIO_PIN_3, 														// 3
        LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2, 		// 4
        a_pin | c_pin | d_pin | f_pin | g_pin, 								// 5
        a_pin | c_pin | d_pin | e_pin | f_pin | g_pin, 						// 6
        a_pin | b_pin | c_pin,												// 7
        a_pin | b_pin | c_pin | d_pin | e_pin | f_pin | g_pin,    			// 8
        a_pin | b_pin | c_pin | d_pin | f_pin | g_pin,						// 9

        a_pin | b_pin | c_pin | e_pin | f_pin | g_pin,    					// A
        c_pin | d_pin | e_pin | f_pin | g_pin,   						 	// B
        a_pin | d_pin | e_pin | f_pin,    									// C
        b_pin | c_pin | d_pin | e_pin | g_pin, 							   	// D
        a_pin | e_pin | f_pin | g_pin | d_pin,   							// E
        a_pin | e_pin | f_pin | g_pin    									// F
    };

    const uint8_t max_num = sizeof(decoder) / sizeof(uint32_t);

    /*
     * Read current state and do not change pins that are not related to
     * indicator (that is done by using masking)
     */
    uint32_t port_state = LL_GPIO_ReadOutputPort(GPIOB);

    port_state = (port_state & ~mask) | decoder[digit % max_num];
    LL_GPIO_WriteOutputPort(GPIOB, port_state);
}

void dyn_display(uint16_t number)
{
	
	static uint8_t digit_num = 0;


    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_0);
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_1);
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_2);
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_3);

    switch (digit_num) {
    case 0:
        /* Включите анод/катод для первой цифры на индикаторе */
        /* number & 0x000F это маска, чтобы получить 4 младший бита, 
           хранящих первую цифру */
    	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0);
        show_digit(number & 0x000F);
        break;
    case 1:
        /* Включите анод/катод для второй цифры на индикаторе */
    	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_1);
        show_digit((number & 0x00F0) >> 4);
        break;
    case 2:
    	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_2);
        show_digit((number & 0x0F00) >> 8);
        break;
    case 3:
    	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_3);
    	show_digit((number & 0xF000) >> 12);
        break;
    default:
        break;
    }

     /* Инкрементируйте переменную с номером текущего индикатора */
    digit_num = (digit_num + 1) % 4;
    //show_digit(out);

    return ;

}


int main(void)
{
	rcc_config();
	gpio_config();

	// uint8_t status_cur = 0;
	// uint8_t status_prev = 0;
	//uint8_t light_on = 0;

	uint8_t number = 0;

	// turn on indicator

	// LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0);
	// LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_1);
	// LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_2);
	// LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_3);

	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_0);
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_1);
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_2);
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_3);

	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0);
	show_digit(number);


	uint8_t counter = 0;
	uint8_t but_pressed = 0;

    while (1)
    {
    	dyn_display(number);
    	delay_2();
    	
    	//delay();
		if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0))
		{
			but_pressed = 1;
			counter = 0;
		}

		if (but_pressed)
		{
			counter++;
			delay_2();
		}


		if (counter >= 5)
		{
			LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);
            number++;
            
            //show_digit(number);
            but_pressed = 0;
            counter = 0;
		}
			
	}
	



    return 0;
}

		// 	delay_2();

		// 	if (!light_on)
		// 	{
		// 		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8);
		// 		light_on = 1;
  //   			delay();
		// 	}
		// 	else
		// 	{
		// 		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);
		// 		light_on = 0;
  //   			delay();
		// 	}

		// 	status_prev = 1;
		// }
		// else
		// if (!status_cur)
		// 	status_prev = 0;