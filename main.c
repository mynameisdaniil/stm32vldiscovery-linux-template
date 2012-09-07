#include <stm32f10x.h>
#include <system_stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

#include <stdio.h>
#include <stdlib.h>

/* Подключаем библиотеку и драйвер GPIO */
#include "hd44780.h"
#include "hd44780_stm32f10x.h"

void init_lcd(void);
void delay_microseconds(uint16_t us);
uint32_t uint32_time_diff(uint32_t now, uint32_t before);
void hd44780_assert_failure_handler(const char *filename, unsigned long line);

HD44780 lcd;
HD44780_STM32F10x_GPIO_Driver lcd_pindriver;
volatile uint32_t systick_ms = 0;

int main(void)
{
  SysTick_Config(SystemCoreClock / 1000);
  init_lcd();
  hd44780_autoscroll_off(&lcd);
  hd44780_blink_on(&lcd);
  hd44780_write_string(&lcd, "1 first row 1");
  hd44780_move_cursor(&lcd, 0, 1);
  hd44780_write_string(&lcd, "2 second row 2");
  hd44780_move_cursor(&lcd, 0, 2);
  hd44780_write_string(&lcd, "3 third row 3");
  hd44780_move_cursor(&lcd, 0, 3);
  //const size_t buf_size = lcd.columns_amount + 1;
  /* char buf[] = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзиклмнопрстуфхцчшщъыьэюя"; */
  //snprintf(buf, buf_size, "%d", (uint8_t)"Ф");
  hd44780_write_string(&lcd, "По-русски!");

  /* while (1) */
  /* { */
  /*   // Каждую секунду выводим счётчик на экран и увеличиваем его */

  /*   static uint32_t lcd_update_ms = 0; */

  /*   if (uint32_time_diff(systick_ms, lcd_update_ms) >= 1000) */
  /*   { */
  /*     lcd_update_ms = systick_ms; */

  /*     static unsigned counter = 0; */

  /*     const size_t buf_size = lcd.columns_amount + 1; */
  /*     char buf[buf_size]; */
  /*     snprintf(buf, buf_size, "%d", counter); */

  /*     ++counter; */

  /*     hd44780_clear(&lcd); */
  /*     hd44780_write_string(&lcd, buf); */
  /*   } */
  /* } */
}

void SysTick_Handler(void)
{
  ++systick_ms;
}

void init_lcd(void)
{
  /* Распиновка дисплея */
  const HD44780_STM32F10x_Pinout lcd_pinout =
  {
    {
      /* RS        */  { GPIOB, GPIO_Pin_15 },
      /* ENABLE    */  { GPIOB, GPIO_Pin_14 },
      /* RW        */  { NULL, 0 },
      /* Backlight */  { NULL, 0 },
      /* DP0       */  { NULL, 0 },
      /* DP1       */  { NULL, 0 },
      /* DP2       */  { NULL, 0 },
      /* DP3       */  { NULL, 0 },
      /* DP4       */  { GPIOB, GPIO_Pin_13 },
      /* DP5       */  { GPIOB, GPIO_Pin_12 },
      /* DP6       */  { GPIOB, GPIO_Pin_11 },
      /* DP7       */  { GPIOB, GPIO_Pin_10 },
    }
  };

  /* Настраиваем драйвер: указываем интерфейс драйвера (стандартный),
     указанную выше распиновку и обработчик ошибок GPIO (необязателен). */
  lcd_pindriver.interface = HD44780_STM32F10X_PINDRIVER_INTERFACE;
  /* Если вдруг захотите сами вручную настраивать GPIO для дисплея
     (зачем бы вдруг), напишите здесь ещё (библиотека учтёт это):
  lcd_pindriver.interface.configure = NULL; */
  lcd_pindriver.pinout = lcd_pinout;
  lcd_pindriver.assert_failure_handler = hd44780_assert_failure_handler;

  /* И, наконец, создаём конфигурацию дисплея: указываем наш драйвер,
     функцию задержки, обработчик ошибок дисплея (необязателен) и опции.
     На данный момент доступны две опции - использовать или нет
     вывод RW дисплея (в последнем случае его нужно прижать к GND),
     и то же для управления подсветкой. */
  const HD44780_Config lcd_config =
  {
    (HD44780_GPIO_Interface*)&lcd_pindriver,
    delay_microseconds,
    hd44780_assert_failure_handler
  };

  /* Ну, а теперь всё стандартно: подаём тактирование на GPIO,
     инициализируем дисплей: 16x2, 4-битный интерфейс, символы 5x8 точек. */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  hd44780_init(&lcd, HD44780_MODE_4BIT, &lcd_config, 16, 4, HD44780_CHARSIZE_5x8);
}

void delay_microseconds(uint16_t us)
{
  SysTick->VAL = SysTick->LOAD;
  const uint32_t systick_ms_start = systick_ms;

  while (1)
  {
    uint32_t diff = uint32_time_diff(systick_ms, systick_ms_start);

    if (diff >= ((uint32_t)us / 1000) + (us % 1000 ? 1 : 0))
      break;
  }
}

uint32_t uint32_time_diff(uint32_t now, uint32_t before)
{
  return (now >= before) ? (now - before) : (UINT32_MAX - before + now);
}

void hd44780_assert_failure_handler(const char *filename, unsigned long line)
{
  (void)filename; (void)line;
  do {} while (1);
}
