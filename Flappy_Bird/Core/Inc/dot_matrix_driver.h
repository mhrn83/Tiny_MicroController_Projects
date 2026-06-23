#ifndef __DOT_MATRIX_DRIVER_H
#define __DOT_MATRIX_DRIVER_H

#include <stdbool.h>

static const int LED_MATRIX_DIM = 8;

void set_pixel(int x, int y);
void clear_matrix(void);
void display_matrix(void);

#endif /* __DOT_MATRIX_DRIVER_H */