#include <stdlib.h>

#include "game_play.h"
#include "dot_matrix_driver.h"
#include "stm32f4xx_hal.h"
#include "main.h"

const int INIT_BIRD_HEIGHT = 3;
const int BIRD_X_POS = 4;
const int HOLE_SPACE = 3;
const int MAX_RISE = 2;
const int MIN_HOLE_HEIGHT = 1;
const int MAX_HOLE_HEIGHT = 4;

volatile bool falling = true;
volatile bool start_game = false;


static inline void draw_bird(int x, int y)
{
	set_pixel(x, y);
}

static void draw_obstacle(int obstacle_x_pos, int hole_pos)
{
	int height = 0;
	
	while (height != hole_pos) {
		set_pixel(obstacle_x_pos, height);
		height++;
	}
	
	height += HOLE_SPACE;
	
	while (height != LED_MATRIX_DIM) {
		set_pixel(obstacle_x_pos, height);
		height++;
	}
}

int static random(unsigned start_bound, unsigned end_bound)
{
	if (end_bound <= start_bound)
		return end_bound;
	
	return (rand() % (end_bound - start_bound + 1)) + start_bound;
}

static void game_over(void)
{  
	// start game over sound
	HAL_GPIO_WritePin(GPIOA, buzzer_Pin, GPIO_PIN_SET);

  // draw an X on matrix
	clear_matrix();
  for (int i = 0; i < LED_MATRIX_DIM; i++) {
		set_pixel(i, i); // diagonal from top-left to bottom-right
    set_pixel(i, LED_MATRIX_DIM - 1 - i); // diagonal from top-right to bottom-left
  }
  display_matrix();
	
	// stop game over sound
	HAL_GPIO_WritePin(GPIOA, buzzer_Pin, GPIO_PIN_RESET);
}

static inline void game_setup(void)
{
	srand(HAL_GetTick());
  start_game = false;
	falling = true;
	draw_bird(BIRD_X_POS, INIT_BIRD_HEIGHT);
}

void game_loop(void)
{
	int bird_height = INIT_BIRD_HEIGHT; 
	int obstacle_x_pos = LED_MATRIX_DIM - 1;
	int rise_count = 0;
	int hole_pos = random(MIN_HOLE_HEIGHT, MAX_HOLE_HEIGHT);
	
	game_setup();
	while (!start_game)
		;
	
	while (1) {
		clear_matrix();
		
		if (falling) {
			if (bird_height < LED_MATRIX_DIM - 1)
				bird_height++;
		} else {
			if (bird_height >= 0)
				bird_height--;
			rise_count++;
		}
		
		if (!falling && rise_count == MAX_RISE) {
				rise_count = 0;
				falling = true;
		}
				
		draw_bird(BIRD_X_POS, bird_height);
		draw_obstacle(obstacle_x_pos, hole_pos);
		display_matrix();
		
		if ((obstacle_x_pos == BIRD_X_POS && 
			  (bird_height < hole_pos || bird_height >= hole_pos + HOLE_SPACE)) ||
				(bird_height == LED_MATRIX_DIM - 1)) {
			game_over();
			break;
		}
				
		obstacle_x_pos--;
		if (obstacle_x_pos < 0) {
			obstacle_x_pos = LED_MATRIX_DIM - 1;
			hole_pos = random(MIN_HOLE_HEIGHT, MAX_HOLE_HEIGHT);
		}
	}
}

void jump_bird(void)
{
	// handle button debounce
	static uint32_t last_interrupt_time = 0;
  uint32_t interrupt_time = HAL_GetTick();
  // if interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200){
      if(!start_game)
        start_game = true;
      falling = false;  
  }
  last_interrupt_time = interrupt_time;
}
