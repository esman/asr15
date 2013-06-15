#include "krnl/generic.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

void RCC_Configuration(void)
{
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
  /* ADCCLK = PCLK2/2 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div2);
#else
  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div4);
#endif
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 clock */
  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable ADC1 and GPIOC clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 | RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
}

int main()
{
	BKP_DeInit();
	BKP_TamperPinCmd(DISABLE);
	RCC_Configuration();
	
	// NVIC configuration
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
	 
	//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	 
	//set subpriority to lowest
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	 
	//enable IRQ channel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 
	//update NVIC registers
	NVIC_Init(&NVIC_InitStructure);
	
	//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
	 
	//update NVIC registers
	NVIC_Init(&NVIC_InitStructure);

	Init();
	
    for(;;) MainLoop();
}
