#include "libdaisy.h"
#include <stm32h7xx_hal.h>

// Jump related stuff

#define u32 uint32_t 
#define vu32 volatile uint32_t
#define SET_REG(addr,val) do { *(vu32*)(addr)=val; } while(0)
#define GET_REG(addr)     (*(vu32*)(addr))

#define RCC_CR      RCC
#define RCC_CFGR    (RCC + 0x04)
#define RCC_CIR     (RCC + 0x08)
#define RCC_AHBENR  (RCC + 0x14)
#define RCC_APB2ENR (RCC + 0x18)
#define RCC_APB1ENR (RCC + 0x1C)

#define SCS      0xE000E000
#define STK      (SCS+0x10)
#define STK_CTRL (STK+0x00)
#define RCC_CR      RCC

typedef struct {
	vu32 ISER[2];
	u32  RESERVED0[30];
	vu32 ICER[2];
	u32  RSERVED1[30];
	vu32 ISPR[2];
	u32  RESERVED2[30];
	vu32 ICPR[2];
	u32  RESERVED3[30];
	vu32 IABR[2];
	u32  RESERVED4[62];
	vu32 IPR[15];
} NVIC_TypeDef;

__attribute__((always_inline)) static inline void __JUMPTOQSPI()
{
	__asm("LDR R1, =0xE000ED00;"); // SCB
	__asm("LDR R0, =0x90000000;"); // APP BASE
	__asm("STR R0, [R1, #8]"); // VTOR
	__asm("LDR SP, [R0, #0]"); // SP @ +0
	__asm("LDR R0, [R0, #4]"); // PC @ +4
	__asm("BX R0");
}

typedef void(*EntryPoint)(void);

// Static Function Declaration
static void SystemClock_Config(void);
static void Error_Handler(void);

void dsy_system_init(uint8_t board)
{
	// For now we won't use the board parameter since all three supported boards use the same 16MHz HSE.
	// That said, 2hp Audio BB had a bit different setup, and actually a more accurate sample rate for audio iirc.
	SystemClock_Config();
}

void dsy_system_jumpto(uint32_t addr)
{
	//		NVIC_TypeDef *rNVIC = (NVIC_TypeDef *)NVIC_BASE;
	//		rNVIC->ICER[0] = 0xFFFFFFFF;
	//		rNVIC->ICER[1] = 0xFFFFFFFF;
	//		rNVIC->ICPR[0] = 0xFFFFFFFF;
	//		rNVIC->ICPR[1] = 0xFFFFFFFF;
	//		SET_REG(STK_CTRL, 0x04);
	//	
	//		// System reset.
	//		SET_REG(RCC_CR, GET_REG(RCC_CR)     | 0x00000001);
	//		SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) & 0xF8FF0000);
	//		SET_REG(RCC_CR, GET_REG(RCC_CR)     & 0xFEF6FFFF);
	//		SET_REG(RCC_CR, GET_REG(RCC_CR)     & 0xFFFBFFFF);
	//		SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) & 0xFF80FFFF);
	//		SET_REG(RCC_CIR, 0x00000000);
		
	//	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
	//	__disable_irq();
	//	uint32_t application_address = addr + 4;
	//	EntryPoint application = (EntryPoint)(application_address);
	//	HAL_RCC_DeInit();
	//	SysTick->CTRL = 0;
	//	SysTick->LOAD = 0;
	//	SysTick->VAL = 0;
	//	__set_MSP(*(__IO uint32_t*)addr);
	//	SCB->VTOR = addr;
	//	HAL_DeInit();
	//	application();
	// Broken for now. . . 
	while (1)
	{
		 
	}
}
void dsy_system_jumptoqspi()
{
	__JUMPTOQSPI();
	while (1)
	{
		 
	}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable 
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source 
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_USART6
                              |RCC_PERIPHCLK_SPI1|RCC_PERIPHCLK_SAI2
                              |RCC_PERIPHCLK_SAI1|RCC_PERIPHCLK_SDMMC
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_ADC
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_QSPI
                              |RCC_PERIPHCLK_FMC | RCC_PERIPHCLK_I2C4;
  PeriphClkInitStruct.PLL2.PLL2M = 4;
  PeriphClkInitStruct.PLL2.PLL2N = 100;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 6;
  PeriphClkInitStruct.PLL3.PLL3N = 295;
  PeriphClkInitStruct.PLL3.PLL3P = 64;
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 4;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_PLL;
	//PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;
	PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;
  PeriphClkInitStruct.Sai23ClockSelection = RCC_SAI23CLKSOURCE_PLL3;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL2;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_PLL3;
	PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PLL3;
	//PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_D3PCLK1;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable USB Voltage detector 
  */
  HAL_PWREx_EnableUSBVoltageDetector();
}

static void SystemClock_Config_old(void);
// Static Function Definition
static void SystemClock_Config_old(void)
{

	
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	/*!< Supply configuration update enable */
	MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

	/** Supply configuration update enable 
	*/
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	/** Configure the main internal regulator output voltage 
	*/
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
	/** Macro to configure the PLL clock source 
	*/
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 200; //400MHZ Sys Clk
	//RCC_OscInitStruct.PLL.PLLN = 240; //400MHZ Sys Clk
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 3; // was 16
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
	                            | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_USART6
	                            | RCC_PERIPHCLK_SPI1 | RCC_PERIPHCLK_SAI2
	                            | RCC_PERIPHCLK_SAI1 | RCC_PERIPHCLK_SDMMC
	                            | RCC_PERIPHCLK_I2C2 | RCC_PERIPHCLK_ADC
	                            | RCC_PERIPHCLK_USB | RCC_PERIPHCLK_QSPI
	                            | RCC_PERIPHCLK_FMC;
	PeriphClkInitStruct.PLL2.PLL2M = 4;
	PeriphClkInitStruct.PLL2.PLL2N = 100;
	PeriphClkInitStruct.PLL2.PLL2P = 2;
	PeriphClkInitStruct.PLL2.PLL2Q = 2;
	PeriphClkInitStruct.PLL2.PLL2R = 2; // was 4
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2; // was 0
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.PLL3.PLL3M = 15;
	PeriphClkInitStruct.PLL3.PLL3N = 368;
	PeriphClkInitStruct.PLL3.PLL3P = 16;
	PeriphClkInitStruct.PLL3.PLL3Q = 4; 
	PeriphClkInitStruct.PLL3.PLL3R = 16; 
	PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0; // was 1
	PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
	PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
	//PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
	PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_PLL;
	PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
	//PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2; // Updated to use PLL2 @ 120MHz
	PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
	PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;
	PeriphClkInitStruct.Sai23ClockSelection = RCC_SAI23CLKSOURCE_PLL3;
	PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
	PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
	PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
	PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_PLL3;
	PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
	PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Enable USB Voltage detector 
	*/
	HAL_PWREx_EnableUSBVoltageDetector();
}

static void Error_Handler()
{
	// Insert code to handle errors here.	
	while(1)
	{
	}
}