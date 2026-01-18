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

#pragma region Debug
int d_check_shader_compile(GLuint shader, const char *type);

int d_check_shader_link(GLuint shader_id);
#pragma endregion

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
 * @brief Creates new renderer with the default settings.
 *
 * Defaults:
 *
 * `max_directional_lights` `1`
 *
 * `max_point_lights` - `8`
 *
 * `max_spot_lights` - `8`
 *
 * `ambient_color` - `0.1f, 0.1f, 0.1f, 1.0f`
 *
 * `face_culling` - `DUCKY_CULL_BACK`
 *
 * `depth_testing` - `true`
 *
 * `blending` - `true`
 *
 * `line_smoothing` - `true`
 */
d_Renderer *d_renderer_create();
void d_renderer_destroy(d_Renderer **renderer);
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
void d_vao_destroy(d_VAO **vao);
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
void d_vbo_destroy(d_VBO **vbo);
void d_vbo_bind(d_VBO *vbo);
void d_vbo_unbind(d_VBO *vbo);
#pragma endregion

#pragma region EBO Functions
d_EBO *d_ebo_create(const unsigned int *indices, const size_t size);
void d_ebo_destroy(d_EBO **ebo);
void d_ebo_bind(d_EBO *ebo);
void d_ebo_unbind(d_EBO *ebo);
#pragma endregion

#pragma region Shader Functions
d_Shader *d_shader_create(d_Renderer *renderer, const char *vertex_file_path,
                          const char *fragment_file_path);
void d_shader_destroy(d_Shader **shader);
void d_shader_activate(d_Shader *shader);
#pragma endregion

#endif

#ifdef DUCKY_GFX_IMPL

#pragma region Debug
int d_check_shader_compile(GLuint shader, const char *type) {
  GLint success;
  GLchar info[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 1024, NULL, info);
    char *message = "(SHADER) compilation failed: ";
    message = d_str_replace(message, "(SHADER)", type);
    message = d_str_append(message, info);
    d_throw_error(DUCKY_SHADER_COMPILE_FAILURE, message);
    free(message);
    return -1;
  }
  return 0;
}

int d_check_shader_link(GLuint shader_id) {
  GLint success;
  GLchar info[1024];
  glGetProgramiv(shader_id, GL_LINK_STATUS, &success);

  GLint log_length;

  glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
  if (!success) {
    glGetProgramInfoLog(shader_id, 1024, NULL, info);
    char *message = d_str_append("Shader program link failed: ", info);
    d_throw_error(DUCKY_SHADER_PROGRAM_LINK_FAILURE, message);
    free(message);
    return -1;
  }

  return 0;
}

#pragma endregion

#pragma region Color Functions
d_Color d_color(const float r, const float g, const float b, const float a) {
  d_Color color;
  if (r < 0.0f || r > 1.0f) {
    d_throw_error(DUCKY_WARNING, "Red component out of range (0.0f - 1.0f).");
    color.r = 0.0f;
  } else {
    color.r = r;
  }

  if (g < 0.0f || g > 1.0f) {
    d_throw_error(DUCKY_WARNING, "Green component out of range (0.0f - 1.0f).");
    color.g = 0.0f;
  } else {
    color.g = g;
  }

  if (b < 0.0f || b > 1.0f) {
    d_throw_error(DUCKY_WARNING, "Blue component out of range (0.0f - 1.0f).");
    color.b = 0.0f;
  } else {
    color.b = b;
  }

  if (a < 0.0f || a > 1.0f) {
    d_throw_error(DUCKY_WARNING, "Alpha component out of range (0.0f - 1.0f).");
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
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.");
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

void d_renderer_destroy(d_Renderer **renderer) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer (d_Renderer **) is NULL.");
    return;
  }
  if (*renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer (d_Renderer *) is NULL.");
    return;
  }

  free(*renderer);
  *renderer = NULL;
}

void d_renderer_set_max_lights(d_Renderer *renderer,
                               const unsigned int max_directional_lights,
                               const unsigned int max_point_lights,
                               const unsigned int max_spot_lights) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.");
    return;
  }

  renderer->max_directional_lights = max_directional_lights;
  renderer->max_point_lights = max_point_lights;
  renderer->max_spot_lights = max_spot_lights;
}

void d_renderer_set_ambient_color(d_Renderer *renderer, const d_Color color) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.");
    return;
  }

  renderer->ambient_color = color;
}

void d_renderer_set_face_culling(d_Renderer *renderer,
                                 const d_FaceCullingType type) {
  if (renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.");
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
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.");
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
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.");
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
    d_throw_error(DUCKY_NULL_REFERENCE, "renderer is NULL.");
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
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.");
    return NULL;
  }
  glGenVertexArrays(1, &vao->id);
  return vao;
}

void d_vao_destroy(d_VAO **vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vao (d_VAO **) is NULL.");
    return;
  }
  if (*vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vao (d_VAO *) is NULL.");
    return;
  }
  glDeleteVertexArrays(1, &(*vao)->id);
  free(*vao);
  *vao = NULL;
}

void d_vao_bind(d_VAO *vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vao is NULL.");
    return;
  }
  glBindVertexArray(vao->id);
  vao->bound = true;
}

void d_vao_unbind(d_VAO *vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vao is NULL.");
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
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_vao_link_attrib) vao is NULL.");
    return;
  }
  if (vbo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_vao_link_attrib) vbo is NULL.");
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
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.");
    return NULL;
  }
  glGenBuffers(1, &vbo->id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  return vbo;
}

void d_vbo_destroy(d_VBO **vbo) {
  if (vbo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vbo (d_VBO **) is NULL.");
    return;
  }
  if (*vbo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "vbo (d_VBO *) is NULL.");
    return;
  }
  glDeleteBuffers(1, &(*vbo)->id);
  free(*vbo);
  *vbo = NULL;
}

void d_vbo_bind(d_VBO *vbo) {}
void d_vbo_unbind(d_VBO *vbo);
#pragma endregion

#pragma region EBO Functions
d_EBO *d_ebo_create(const unsigned int *indices, const size_t size) {
  d_EBO *ebo = malloc(sizeof(d_EBO));
  if (ebo == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.");
    return NULL;
  }
  glGenBuffers(1, &ebo->id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  return ebo;
}

void d_ebo_destroy(d_EBO **ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "ebo (d_EBO **) is NULL.");
    return;
  }
  if (*ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "ebo (d_EBO *) is NULL.");
    return;
  }

  glDeleteBuffers(1, &(*ebo)->id);
  free(*ebo);
  *ebo = NULL;
}

void d_ebo_bind(d_EBO *ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "ebo is NULL.");
    return;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
  ebo->bound = true;
}

void d_ebo_unbind(d_EBO *ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "ebo is NULL.");
    return;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  ebo->bound = false;
}

#pragma endregion

#pragma region Shader Functions
d_Shader *d_shader_create(d_Renderer *renderer, const char *vertex_file_path,
                          const char *fragment_file_path) {
  d_Shader *shader = malloc(sizeof(d_Shader));

  if (shader == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.");
    return NULL;
  }

  d_File *fragment_shader = d_file_read(fragment_file_path);
  if (fragment_shader == NULL) {
    d_throw_error(DUCKY_FAILURE, "Failed to read fragment shader.");

    d_core_shutdown();
  }

  d_File *vertex_shader = d_file_read(vertex_file_path);
  if (vertex_shader == NULL) {
    d_throw_error(DUCKY_FAILURE, "Failed to read vertex shader.");

    d_core_shutdown();
  }

  char *number = malloc(sizeof(char) * 4);
  if (number == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to malloc number.");
  }

  sprintf(number, "%d", renderer->max_point_lights);
  char *res = d_str_append("#define MAX_POINT_LIGHTS ", number);
  fragment_shader->data =
      d_str_replace(fragment_shader->data, "#define MAX_POINT_LIGHTS 8", res);

  sprintf(number, "%d", renderer->max_spot_lights);
  res = d_str_append("#define MAX_SPOT_LIGHTS ", number);
  fragment_shader->data =
      d_str_replace(fragment_shader->data, "#define MAX_SPOT_LIGHTS 8", res);

  sprintf(number, "%d", renderer->max_directional_lights);
  res = d_str_append("#define MAX_DIRECTIONAL_LIGHTS ", number);
  fragment_shader->data = d_str_replace(
      fragment_shader->data, "#define MAX_DIRECTIONAL_LIGHTS 1", res);

  free(number);
  free(res);

  GLuint vert = glCreateShader(GL_VERTEX_SHADER);
  const char *vert_src = vertex_shader->data;
  glShaderSource(vert, 1, &vert_src, NULL);
  glCompileShader(vert);
  if (d_check_shader_compile(vert, "VERTEX_SHADER") == -1) {
    d_throw_error(DUCKY_FAILURE, "Failed to compile vertex shader.");
    free(shader);
    d_file_destroy(&vertex_shader);
    d_file_destroy(&fragment_shader);
    return NULL;
  }

  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
  const char *frag_src = fragment_shader->data;
  glShaderSource(frag, 1, &frag_src, NULL);
  glCompileShader(frag);
  if (d_check_shader_compile(frag, "FRAGMENT_SHADER") == -1) {
    d_throw_error(DUCKY_FAILURE, "Failed to compile fragment shader.");
    free(shader);
    d_file_destroy(&vertex_shader);
    d_file_destroy(&fragment_shader);
    return NULL;
  }
  d_file_destroy(&vertex_shader);
  d_file_destroy(&fragment_shader);

  shader->id = glCreateProgram();

  glAttachShader(shader->id, vert);
  glDeleteShader(vert);

  glAttachShader(shader->id, frag);
  glDeleteShader(frag);

  glLinkProgram(shader->id);
  if (d_check_shader_link(shader->id) == -1) {
    d_throw_error(DUCKY_FAILURE, "Failed to link shader program.");
    free(shader);
    return NULL;
  }

  if (glIsProgram(shader->id) == GL_FALSE) {
    d_throw_error(DUCKY_FAILURE, "Shader progam is NOT valid!");
    free(shader);
    return NULL;
  }

  return shader;
}
void d_shader_destroy(d_Shader **shader) {
  if (shader == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "shader (d_Shader **) is NULL.");
    return;
  }
  if (*shader == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "shader (d_Shader *) is NULL.");
    return;
  }

  glDeleteProgram((*shader)->id);

  free(*shader);
  *shader = NULL;
}
void d_shader_activate(d_Shader *shader) {
  if (shader == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "shader is NULL.");
    return;
  }

  glUseProgram(shader->id);

  GLenum error = glGetError();
  char *error_str = d_str_from_int(error);
  if (error != GL_NO_ERROR) {
    char *message =
        d_str_append("Failed to use shader program. OpenGL Error: ", error_str);
    d_throw_error(DUCKY_FAILURE, message);
  }
  free(error_str);
}
#pragma endregion

#endif