#define DUCKY_CORE_IMPL
#define DUCKY_CORE_PRINT_ERRORS
#include "ducky_core.h"
#define DUCKY_MATH_IMPL
#include "ducky_math.h"
#define DUCKY_WINDOW_IMPL
#define DUCKY_GLAD_IMPL "glad/glad.h"
#include "ducky_window.h"
#define DUCKY_GFX_IMPL
#include "ducky_gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*EventFunction)();

typedef struct d_Event {
  EventFunction function;
  EventFunction *listeners;
  unsigned int listener_count;
} Event, d_Event;

d_Event *d_event_new(void *function) {
  d_Event *event = malloc(sizeof(d_Event));
  if (event == NULL) {
    return NULL;
  }
  event->function = function;
  event->listeners = NULL;
  event->listener_count = 0;
  return event;
}

void d_event_send(d_Event *event) {
  if (event == NULL) {
    return;
  }

  event->function();

  for (int i = 0; i < event->listener_count; i++) {
    EventFunction listener = event->listeners[i];

    listener();
  }
}

void d_event_add_listener(d_Event *event, EventFunction listener) {
  if (event == NULL || listener == NULL) {
    return;
  }
  EventFunction *temp = malloc(sizeof(EventFunction) * (event->listener_count));
  memcpy(temp, event->listeners,
         sizeof(EventFunction) * (event->listener_count));
  temp = realloc(temp, sizeof(EventFunction) * (event->listener_count + 1));

  if (temp != NULL) {
    event->listeners = temp;
    event->listeners[event->listener_count] = listener;
    event->listener_count++;
  }
}

void d_event_remove_listener(d_Event *event, EventFunction listener) {
  if (event == NULL || listener == NULL) {
    return;
  }

  for (int i = 0; i < event->listener_count; i++) {
    if (event->listeners[i] == listener) {
      EventFunction target = event->listeners[i];
      EventFunction last = event->listeners[event->listener_count - 1];
      event->listeners[event->listener_count - 1] = target;
      event->listeners[i] = last;
      EventFunction *temp =
          malloc(sizeof(EventFunction) * (event->listener_count));
      memcpy(temp, event->listeners,
             sizeof(EventFunction) * (event->listener_count));
      temp = realloc(temp, sizeof(EventFunction) * (event->listener_count - 1));
      if (temp != NULL) {
        event->listeners = temp;
        event->listener_count--;
      }

      break;
    }
  }
}

void hello() { printf("Hello, "); }
void world() { printf("world!\n"); }
void hi_mum() { printf("Hi, Mum!\n"); }

void main() {

  // Window *window = d_window_create("Ducky Window", 800, 600, true, false);
  // Renderer *renderer = d_renderer_create();

  // while (window->running) {
  //   d_window_update(window);

  //   d_window_swap_buffers(window);
  // }

  // d_window_destroy(window);

  d_DArray *array = d_darray_create(sizeof(int));
  for (int i = 1; i < 11; i++) {
    d_darray_add(array, &i);
  }

  d_darray_remove(array, 4);

  for (int i = 0; i < 20; i++) {
    int value = *((int *)array->data + i);
    printf("Value at index %d: %d\n", i, value);
  }

  d_darray_destroy(array);
}