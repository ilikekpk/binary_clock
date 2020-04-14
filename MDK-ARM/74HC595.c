#include "74HC595.h"

void bin_disp_write(uint8_t a, uint8_t b, uint8_t c)
{
	LL_GPIO_ResetOutputPin(ENTER_GPIO_Port, ENTER_Pin);

	
	LL_SPI_TransmitData8(SPI1, a << 1);
	LL_SPI_TransmitData8(SPI1, b << 1);
	LL_SPI_TransmitData8(SPI1, c << 1);

  LL_mDelay(1);
	LL_GPIO_SetOutputPin(ENTER_GPIO_Port, ENTER_Pin);
}

void bin_disp_init(void)
{
	LL_SPI_Enable(SPI1);
	LL_GPIO_SetOutputPin(ENTER_GPIO_Port, ENTER_Pin);	
}
