#include "ducky.h"

int main(int argc, char **argv) {
  d_core_init();

  Window *window = d_window_create("Ducky Window", 800, 600, true, false);
  Renderer *renderer = d_renderer_create();
  d_renderer_set_max_lights(renderer, 1, 16, 16);

  d_Shader *shader = d_shader_create(renderer, "assets/shaders/vertex.glsl",
                                     "assets/shaders/fragment.glsl");

  d_shader_activate(shader);

  d_MeshRenderer *mesh = d_mesh_renderer_create("assets/models/cube.fbx");
  mesh->material = d_material_create("assets/textures/demo_diffuse.png",
                                     "assets/textures/demo_specular.png",
                                     d_color(1.0f, 1.0f, 1.0f, 1.0f));

  while (d_window_running(window)) {
    d_window_update(window);

    d_renderer_clear(d_color(0.2f, 0.3f, 0.3f, 1.0f));

    d_mesh_renderer_update(mesh);

    d_window_swap_buffers(window);
  }

  d_mesh_renderer_destroy(&mesh);

  d_shader_destroy(&shader);
  d_renderer_destroy(&renderer);
  d_window_destroy(&window);

  d_core_shutdown();

  return 0;
}