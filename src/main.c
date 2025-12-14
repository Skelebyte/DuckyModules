#define DUCKY_CORE_IMPL
#define DUCKY_CORE_PRINT_ERRORS
#include "ducky_core.h"
#define DUCKY_MATH_IMPL
#include "ducky_math.h"
#define DUCKY_WINDOW_IMPL
#define DUCKY_GLAD_IMPL "glad/glad.h"
#include "ducky_window.h"
#include <stdio.h>

void main() {
  Window *window = d_window_create("Ducky Window", 800, 600, true, false);

  if (window == NULL) {
    d_get_error();
    return;
  }

  while (window->running) {
    d_window_update(window);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    d_window_swap_buffers(window);
  }

  d_window_destroy(window);
}