// Virtual timer implementation
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "nrf.h"
#include "virtual_timer.h"
#include "virtual_timer_linked_list.h"

// This is the interrupt handler that fires on a compare event
void TIMER4_IRQHandler(void) {
  // This should always be the first line of the interrupt handler!
  // It clears the event so that it doesn't happen again
  //NRF_TIMER4->EVENTS_COMPARE[0] = 0;
  NRF_TIMER4->EVENTS_COMPARE[2] = 0;
  //printf("Timer Fired!\n");
  node_t* timer_node = list_get_first();
  timer_node->cb();
  if (timer_node->repeated)
  {
    uint32_t next_interrupt_time = timer_node->timer_value + timer_node->microseconds;
    timer_node->timer_value = next_interrupt_time;
    //virtual_timer_start_repeated(next_interrupt_time, timer_node->cb);
    //list_remove(timer_node);
    //free(timer_node);
    NRF_TIMER4->CC[2] = timer_node->timer_value;
    //printf("now: %ld, head: %ld\n", read_timer(), list_get_first()->timer_value);
  }
  else 
  {
    list_remove(timer_node);
    free(timer_node);
    if (list_get_first()!=NULL)
      NRF_TIMER4->CC[2] = list_get_first()->timer_value;
  }
  //list_print();
  // Place your interrupt handler code here
}
// Read the current value of the timer counter
uint32_t read_timer(void) {
  NRF_TIMER4->TASKS_CAPTURE[1] = 1;
  uint32_t internal_counter = NRF_TIMER4->CC[1];
  // Should return the value of the internal counter for TIMER4
  return internal_counter;
}
void virtual_timer_init(void) {
  // Place your timer initialization code here
  NRF_TIMER4->MODE = 0;
  NRF_TIMER4->BITMODE = 3;
  NRF_TIMER4->PRESCALER = 4;
  NRF_TIMER4->TASKS_CLEAR = 1;
  NRF_TIMER4->TASKS_START = 1;
  NRF_TIMER4->INTENSET = 1<<18;
  NVIC_EnableIRQ(TIMER4_IRQn);
}

static uint32_t timer_start(uint32_t microseconds, virtual_timer_callback_t cb, bool repeated) {
  //__disable_irq();
  node_t* timer = malloc(sizeof(node_t));
  uint32_t current_time = read_timer();
  uint32_t next_interrupt_time = current_time + microseconds;
  timer->timer_value = next_interrupt_time;
  timer->cb = cb;
  timer->repeated = repeated;
  timer->microseconds = microseconds;
  list_insert_sorted(timer);

  node_t* timer_node = list_get_first();
 // if (current_time > NRF_TIMER4->CC[2]) {
  NRF_TIMER4->CC[2] = timer_node->timer_value;
 // }
  //__enable_irq();
  // Return a unique timer ID. (hint: What is guaranteed unique about the timer you have created?)
  return (uint32_t) timer_node;
}

// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start(uint32_t microseconds, virtual_timer_callback_t cb) {
  return timer_start(microseconds, cb, false);
}
// You do not need to modify this function
// Instead, implement timer_start
uint32_t virtual_timer_start_repeated(uint32_t microseconds, virtual_timer_callback_t cb) {
  return timer_start(microseconds, cb, true);
}
// Remove a timer by ID.
// Make sure you don't cause linked list consistency issues!
// Do not forget to free removed timers.
void virtual_timer_cancel(uint32_t timer_id) {
  node_t* n = list_get_first();
  while (n!=NULL)
  {
    if ((uint32_t)n==timer_id)
    {
      list_remove(n);
      free(n);
      break;
    }
    n = n->next;
  }

  node_t* new_head = list_get_first();
  if (new_head!=NULL)
    NRF_TIMER4->CC[2] = new_head->timer_value;
  else 
    NRF_TIMER4->CC[2] = 0;
}