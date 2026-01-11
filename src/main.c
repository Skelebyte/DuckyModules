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

int main(int argc, char **argv) {
  d_core_init();

  Window *window = d_window_create("Ducky Window", 800, 600, true, false);
  Renderer *renderer = d_renderer_create();

  d_throw_error(&DUCKY_WARNING, "This is a test warning.", __FILE__,
                __FUNCTION__);

  bool err_thrown = false;

  while (window->running) {
    d_window_update(window);

    d_window_swap_buffers(window);

    if (!err_thrown) {
      d_throw_error(&DUCKY_FAILURE, "This is a test failure.", __FILE__,
                    __FUNCTION__);
      err_thrown = true;
    }
  }

  d_renderer_destroy(renderer);
  d_window_destroy(window);

  d_core_shutdown();

  return 0;
}