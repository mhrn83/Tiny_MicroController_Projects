#include "dot_matrix_driver.h"
#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"


enum LED_COL_STATE { ON_STATE, OFF_STATE };
static const char ROW_SEL[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static bool matrix[8][8] = {};

void set_pixel(int x, int y)
{
	if (x < 0 || x >= LED_MATRIX_DIM || y < 0 || y >= LED_MATRIX_DIM)
		return;
	
	matrix[x][y] = ON_STATE;
}

void clear_matrix(void)
{
	for (int i = 0; i < LED_MATRIX_DIM; i++)
		for (int j = 0; j < LED_MATRIX_DIM; j++)
			matrix[i][j] = OFF_STATE;
}

void display_matrix(void)
{
	for (int c = 12; c > 0; c--) {
		for (int i = 0; i < LED_MATRIX_DIM; i++) {
			GPIOE->ODR = ROW_SEL[i];
			for (int j = 0; j < LED_MATRIX_DIM; j++) {
				if (matrix[j][i] == OFF_STATE)
					GPIOC->ODR |= (1 << j);
				else
					GPIOC->ODR &= ~(1 << j);
			}
			HAL_Delay(1);
		}
	}
	GPIOE->ODR = 0x00;
	GPIOC->ODR = 0x00;
}
