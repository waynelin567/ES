// Virtual timers
//
// Uses a single hardware timer to create an unlimited supply of virtual
//  software timers
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"

#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"
#include "buckler.h"
#include "virtual_timer.h"
void led0_toggle() {
    nrf_gpio_pin_toggle(BUCKLER_LED0);
    printf("LED toggled!\n");

}
void led1_toggle() {
    nrf_gpio_pin_toggle(BUCKLER_LED1);
}
void led2_toggle() {
    nrf_gpio_pin_toggle(BUCKLER_LED2);
    printf("LED toggled!\n");
}
void turn_off_LEDS()
{
    led0_toggle();
    led1_toggle();
    led2_toggle();
}
int main(void) {
  ret_code_t error_code = NRF_SUCCESS;
  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Board initialized!\n");
  // You can use the NRF GPIO library to test your timers
  nrf_gpio_pin_dir_set(BUCKLER_LED0, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(BUCKLER_LED1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(BUCKLER_LED2, NRF_GPIO_PIN_DIR_OUTPUT);

  //turn_off_LEDS();

  virtual_timer_init();
  //nrf_delay_ms(3000);
  uint32_t stop_time = read_timer() + 5000000;
  //checkoff 6.2.6
  uint32_t start_toggleID = virtual_timer_start_repeated(1000000, led0_toggle);
  
  
  //uint32_t toggle0 = virtual_timer_start_repeated(1000000, led0_toggle);
  //uint32_t toggle1 = virtual_timer_start_repeated(2000000, led1_toggle);
  //list_print();
  //nrf_delay_ms(3000);
  //irrelevant = virtual_timer_start(2000000, led2_toggle);
  //timer_list->timer_value =
  // Setup some timers and see what happens
  //virtual_timer_start_repeated(1000000, led0_toggle);
  //virtual_timer_start_repeated(2000000, led1_toggle);

  // loop forever
  bool done = false;
  while (1) {
    uint32_t timer_counter = read_timer();
    printf("Timer value: %ld\n", timer_counter);
    nrf_delay_ms(1000);
    // checkoff 6.2.6
    if (timer_counter >= stop_time && !done)
    {
      virtual_timer_cancel(start_toggleID);
    }
  }
}