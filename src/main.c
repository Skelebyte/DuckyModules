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

  d_File *frag_shader = d_file_read("assets/shaders/fragment.glsl");
  if (frag_shader == NULL) {
    d_throw_error(&DUCKY_FAILURE, "Failed to read fragment shader.", __FILE__,
                  __FUNCTION__);
    d_renderer_destroy(renderer);
    d_window_destroy(window);

    d_core_shutdown();
  }

  d_File *vertex_shader = d_file_read("assets/shaders/vertex.glsl");
  if (vertex_shader == NULL) {
    d_throw_error(&DUCKY_FAILURE, "Failed to read vertex shader.", __FILE__,
                  __FUNCTION__);
    d_renderer_destroy(renderer);
    d_window_destroy(window);

    d_core_shutdown();
  }

  size_t pos = d_str_find(frag_shader->data, "#define MAX_POINT_LIGHTS 8", 0);
  size_t eol = d_str_find(frag_shader->data, "\n", pos);
  printf("Found at %zu to %zu\n", pos, eol);

  while (window->running) {
    d_window_update(window);

    d_renderer_clear(d_color(0.2f, 0.3f, 0.3f, 1.0f));

    d_window_swap_buffers(window);
  }

  d_renderer_destroy(renderer);
  d_window_destroy(window);

  d_core_shutdown();

  return 0;
}