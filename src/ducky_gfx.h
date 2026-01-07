#ifndef DUCKY_GFX_H
#define DUCKY_GFX_H

#ifndef DUCKY_CORE_H
#include "ducky_core.h"
#endif

#ifdef DUCKY_GLAD_IMPL
#include DUCKY_GLAD_IMPL
#endif
#include "glad/glad.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define VERTEX_SHADER_BUFFER_SIZE 1024
#define FRAGMENT_SHADER_BUFFER_SIZE 1024

#pragma region Types

/**
 * @brief Each color compoent (`r`, `g`, `b`, `a`) is represented as a `0.0f` to
 * `1.0f` float value
 *
 */
typedef union d_Color {
  struct {
    float r;
    float g;
    float b;
    float a;
  };
  float data[4];
} Color, d_Color;

typedef enum d_FaceCullingType {
  DUCKY_CULL_BACK,
  DUCKY_CULL_FRONT,
  DUCKY_CULL_NONE
} FaceCullingType,
    d_FaceCullingType;

typedef struct d_Renderer {
  unsigned int max_directional_lights;
  unsigned int max_point_lights;
  unsigned int max_spot_lights;

  d_Color ambient_color;

  d_FaceCullingType face_culling;
  bool depth_testing;
  bool blending;
  bool line_smoothing;

  unsigned int shadow_map_size_w;
  unsigned int shadow_map_size_h;
} Renderer, d_Renderer;

typedef struct d_VAO {
  unsigned int id;
  bool bound;
} VAO, d_VAO;

typedef struct d_VBO {
  unsigned int id;
  bool bound;
} VBO, d_VBO;

typedef struct d_EBO {
  unsigned int id;
  bool bound;
} EBO, d_EBO;

typedef struct d_Shader {
  unsigned int id;
} Shader, d_Shader;

#pragma endregion

#pragma region Color Functions
d_Color d_color(const float r, const float g, const float b, const float a);
#pragma endregion

#pragma region Renderer Functions
/**
 * @brief Creates new renderer.
 *
 * @param max_directional_lights Maximum number of directional lights. (default:
 * 1)
 * @param max_point_lights Maximum number of point lights. (default: 8)
 * @param max_spot_lights Maximum number of spot lights. (default: 8)
 * @return d_Renderer*
 */
d_Renderer *d_renderer_create();
void d_renderer_destroy(d_Renderer *renderer);
void d_renderer_set_ambient_color(d_Renderer *renderer, const d_Color color);
void d_renderer_set_max_lights(d_Renderer *renderer,
                               const unsigned int max_directional_lights,
                               const unsigned int max_point_lights,
                               const unsigned int max_spot_lights);
void d_renderer_set_face_culling(d_Renderer *renderer,
                                 const d_FaceCullingType type);
void d_renderer_set_blending(d_Renderer *renderer, const bool enabled);
void d_renderer_set_depth_testing(d_Renderer *renderer, const bool enabled);
void d_renderer_set_line_smoothing(d_Renderer *renderer, const bool enabled);

void d_renderer_clear(const d_Color color);
#pragma endregion

#pragma region VAO Functions
/*
  Create a new Vertex Array Object (VAO).
  #### Returns:
  - A pointer to the created VAO.
*/
d_VAO *d_vao_create();
/*
  Destroy the specified VAO and free its resources.
  #### Parameters:
  - `vao`: The VAO to destroy.
  #### Throws:
  - `DUCKY_NULL_REFERENCE`: If the `vao` argument is NULL.
*/
void d_vao_destroy(d_VAO *vao);
/*
  Bind the specified VAO for use.
  #### Parameters:
  - `vao`: The VAO to bind.
  #### Throws:
  - `DUCKY_NULL_REFERENCE`: If the `vao` argument is NULL.
*/
void d_vao_bind(d_VAO *vao);
/*
  Unbind the specified VAO.
  #### Parameters:
  - `vao`: The VAO to unbind.
  #### Throws:
  - `DUCKY_NULL_REFERENCE`: If the `vao` argument is NULL.
*/
void d_vao_unbind(d_VAO *vao);
/*
  Link a vertex attribute to the VAO.
  #### Parameters:
  - `vao`: The VAO to link the attribute to.
  - `vbo`: The VBO containing the vertex data.
  - `layout`: The layout location of the attribute in the shader.
  - `size`: The number of components in the attribute (e.g. 3 for vec3).
  - `stride`: The byte offset between consecutive attributes.
  - `offset`: The byte offset of the first component of the attribute.
  #### Throws:
  - `DUCKY_NULL_REFERENCE`: If the `vao` or `vbo` argument is NULL.
*/
void d_vao_link_attrib(const d_VAO *vao, const d_VBO *vbo,
                       const unsigned int layout, const unsigned int size,
                       const unsigned int type, const unsigned int stride,
                       const void *offset);
#pragma endregion

#pragma region VBO Functions
d_VBO *d_vbo_create(const float *vertices, const size_t size);
void d_vbo_destroy(d_VBO *vbo);
void d_vbo_bind(d_VBO *vbo);
void d_vbo_unbind(d_VBO *vbo);
#pragma endregion

#pragma region EBO Functions
d_EBO *d_ebo_create(const unsigned int *indices, const size_t size);
void d_ebo_destroy(d_EBO *ebo);
void d_ebo_bind(d_EBO *ebo);
void d_ebo_unbind(d_EBO *ebo);
#pragma endregion

#pragma region Shader Functions
d_Shader *d_shader_create(const char *vertex_src, const char *fragment_src);
void d_shader_destroy(d_Shader *shader);
void d_shader_activate(d_Shader *shader);
#pragma endregion

#endif

#ifdef DUCKY_GFX_IMPL

#pragma region Color Functions
d_Color d_color(const float r, const float g, const float b, const float a) {
  d_Color color;
  if (r < 0.0f || r > 1.0f) {
    d_throw_error(DUCKY_WARNING, "Red component out of range (0.0f - 1.0f).",
                  __FILE__, __FUNCTION__);
    color.r = 0.0f;
  } else {
    color.r = r;
  }

  if (g < 0.0f || g > 1.0f) {
    d_throw_error(DUCKY_WARNING, "Green component out of range (0.0f - 1.0f).",
                  __FILE__, __FUNCTION__);
    color.g = 0.0f;
  } else {
    color.g = g;
  }

  if (b < 0.0f || b > 1.0f) {
    d_throw_error(DUCKY_WARNING, "Blue component out of range (0.0f - 1.0f).",
                  __FILE__, __FUNCTION__);
    color.b = 0.0f;
  } else {
    color.b = b;
  }

  if (a < 0.0f || a > 1.0f) {
    d_throw_error(DUCKY_WARNING, "Alpha component out of range (0.0f - 1.0f).",
                  __FILE__, __FUNCTION__);
    color.a = 1.0f;
  } else {
    color.a = a;
  }

  return color;
}
#pragma endregion

#pragma region Renderer Functions
d_Renderer *d_renderer_create() {
  d_Renderer *renderer = malloc(sizeof(d_Renderer));
  if (renderer == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.", __FILE__,
                  __FUNCTION__);
    return NULL;
  }

  renderer->max_directional_lights = 1;
  renderer->max_point_lights = 8;
  renderer->max_spot_lights = 8;

  d_renderer_set_ambient_color(renderer, d_color(0.1f, 0.1f, 0.1f, 1.0f));
  d_renderer_set_face_culling(renderer, DUCKY_CULL_BACK);
  d_renderer_set_blending(renderer, true);
  d_renderer_set_depth_testing(renderer, true);
  d_renderer_set_line_smoothing(renderer, true);

  return renderer;
}

void d_renderer_destroy(d_Renderer *renderer) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  free(renderer);
}

void d_renderer_set_max_lights(d_Renderer *renderer,
                               const unsigned int max_directional_lights,
                               const unsigned int max_point_lights,
                               const unsigned int max_spot_lights) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  renderer->max_directional_lights = max_directional_lights;
  renderer->max_point_lights = max_point_lights;
  renderer->max_spot_lights = max_spot_lights;
}

void d_renderer_set_ambient_color(d_Renderer *renderer, const d_Color color) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  renderer->ambient_color = color;
}

void d_renderer_set_face_culling(d_Renderer *renderer,
                                 const d_FaceCullingType type) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  renderer->face_culling = type;
  switch (type) {
  case DUCKY_CULL_BACK:
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    break;
  case DUCKY_CULL_FRONT:
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    break;
  case DUCKY_CULL_NONE:
    glDisable(GL_CULL_FACE);
    break;
  default:
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    break;
  }
}

void d_renderer_set_blending(d_Renderer *renderer, bool enabled) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  renderer->blending = enabled;
  if (enabled) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } else {
    glDisable(GL_BLEND);
  }
}

void d_renderer_set_depth_testing(d_Renderer *renderer, const bool enabled) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  renderer->depth_testing = enabled;
  if (enabled) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
}

void d_renderer_set_line_smoothing(d_Renderer *renderer, const bool enabled) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  renderer->line_smoothing = enabled;
  if (enabled) {
    glEnable(GL_LINE_SMOOTH);
  } else {
    glDisable(GL_LINE_SMOOTH);
  }
}

void d_renderer_clear(const d_Color color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
#pragma endregion

#pragma region VAO Functions
d_VAO *d_vao_create() {
  d_VAO *vao = malloc(sizeof(d_VAO));
  if (vao == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.", __FILE__,
                  __FUNCTION__);
    return NULL;
  }
  glGenVertexArrays(1, &vao->id);
  return vao;
}

void d_vao_destroy(d_VAO *vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vao is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  glDeleteVertexArrays(1, &vao->id);
  free(vao);
}

void d_vao_bind(d_VAO *vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vao is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  glBindVertexArray(vao->id);
  vao->bound = true;
}

void d_vao_unbind(d_VAO *vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vao is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  glBindVertexArray(0);
  vao->bound = false;
}

void d_vao_link_attrib(const d_VAO *vao, const d_VBO *vbo,
                       const unsigned int layout, const unsigned int size,
                       const unsigned int type, const unsigned int stride,
                       const void *offset) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_vao_link_attrib) vao is NULL.",
                  __FILE__, __FUNCTION__);
    return;
  }
  if (vbo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_vao_link_attrib) vbo is NULL.",
                  __FILE__, __FUNCTION__);
  }
  // bind vbo
  glVertexAttribPointer(layout, size, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
  // unbind vbo
}
#pragma endregion

#pragma region VBO Functions
d_VBO *d_vbo_create(const float *vertices, const size_t size) {
  d_VBO *vbo = malloc(sizeof(d_VBO));
  if (vbo == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.", __FILE__,
                  __FUNCTION__);
    return NULL;
  }
  glGenBuffers(1, &vbo->id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  return vbo;
}

void d_vbo_destroy(d_VBO *vbo) {
  if (vbo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vbo is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  glDeleteBuffers(1, &vbo->id);
  free(vbo);
}

void d_vbo_bind(d_VBO *vbo) {}
void d_vbo_unbind(d_VBO *vbo);
#pragma endregion

#pragma region EBO Functions
d_EBO *d_ebo_create(const unsigned int *indices, const size_t size) {
  d_EBO *ebo = malloc(sizeof(d_EBO));
  if (ebo == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.", __FILE__,
                  __FUNCTION__);
    return NULL;
  }
  glGenBuffers(1, &ebo->id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  return ebo;
}

void d_ebo_destroy(d_EBO *ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "ebo is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  glDeleteBuffers(1, &ebo->id);
  free(ebo);
}

void d_ebo_bind(d_EBO *ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "ebo is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
  ebo->bound = true;
}

void d_ebo_unbind(d_EBO *ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "ebo is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  ebo->bound = false;
}

#pragma endregion

#pragma region Shader Functions
d_Shader *d_shader_create(const char *vertex_src, const char *fragment_src) {
  FILE *vertex_file = fopen(vertex_src, "r");
  if (vertex_file == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vertex shader file not found.",
                  __FILE__, __FUNCTION__);
    return NULL;
  }

  if (fseek(vertex_file, 0, SEEK_END) != 0) {
    fclose(vertex_file);
    d_throw_error(DUCKY_FAILURE, "fseek failed.", __FILE__, __FUNCTION__);
    return NULL;
  }

  unsigned long vertex_file_size = ftell(vertex_file);

  if (vertex_file_size == -1L) {
    fclose(vertex_file);
    d_throw_error(DUCKY_FAILURE, "ftell failed.", __FILE__, __FUNCTION__);
    return NULL;
  }
  rewind(vertex_file);

  char *vertex_file_buffer = malloc(sizeof(char) * (vertex_file_size + 1));

  if (vertex_file_buffer == NULL) {
    fclose(vertex_file);
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.", __FILE__,
                  __FUNCTION__);
    return NULL;
  }

  fclose(vertex_file);

  //  max light stuff, compile shaders...

  FILE *fragment_file = fopen(fragment_src, "r");
  if (fragment_file == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "fragment shader file not found.",
                  __FILE__, __FUNCTION__);
    return NULL;
  }
}
void d_shader_destroy(d_Shader *shader) {}
void d_shader_activate(d_Shader *shader) {}
#pragma endregion

#endif