#include "lcd.h"

void lcd_init(struct lcd *lcd) {
  lcd->control = lcd->status = lcd->y_coordinate = lcd->compare = 0;
}
