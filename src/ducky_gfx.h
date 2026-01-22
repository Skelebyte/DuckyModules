#ifndef DUCKY_GFX_H
#define DUCKY_GFX_H

#ifndef DUCKY_CORE_H
#include "ducky_core.h"
#endif

#ifdef DUCKY_GLAD_IMPL
#include DUCKY_GLAD_IMPL
#elif
#include "glad/glad.h"
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "stb/stb_image.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#pragma region Debug
int d_check_shader_compile(GLuint shader, const char *type);

int d_check_shader_link(GLuint shader_id);

/**
 * @brief Checks if there is an OpenGL error.
 *
 * @param message Extra context for the error.
 * @param function Function this function is called in.
 * @param file The file this function is called in.
 * @return `true` When there is an error.
 * @return `false` When there is no error.
 */
bool d_gl_error_internal(const char *message, const char *function,
                         const char *file);

/**
 * @brief Checks if there is an OpenGL error.
 *
 * @param message Extra context for the error.
 * @param function Function this function is called in.
 * @param file The file this function is called in.
 * @return `true` When there is an error.
 * @return `false` When there is no error.
 */
#define d_gl_error(message) d_gl_error_internal(message, __FUNCTION__, __FILE__)

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
  d_uint max_directional_lights;
  d_uint max_point_lights;
  d_uint max_spot_lights;

  d_Color ambient_color;

  d_FaceCullingType face_culling;
  bool depth_testing;
  bool blending;
  bool line_smoothing;

  d_uint shadow_map_size_w;
  d_uint shadow_map_size_h;
} Renderer, d_Renderer;

typedef struct d_VAO {
  d_uint id;
  bool bound;
} VAO, d_VAO;

typedef struct d_VBO {
  d_uint id;
  bool bound;
} VBO, d_VBO;

typedef struct d_EBO {
  d_uint id;
  bool bound;
} EBO, d_EBO;

typedef struct d_Shader {
  d_uint id;
} Shader, d_Shader;

typedef enum d_TextureBlendMode {
  NEAREST = 0,
  LINEAR = 1
} TextureBlendMode,
    d_TextureBlendMode;

typedef struct d_Texture {
  d_uint id;
  d_TextureBlendMode blend_mode;
} Texture, d_Texture;

typedef struct d_Material {
  d_Texture *diffuse;
  d_Texture *specular;
  d_Color color;
  float specular_strength;
  bool unlit;

  d_uint diffuse_uniform;
  d_uint specular_uniform;
  d_uint specular_strength_uniform;
  d_uint color_uniform;
  GLboolean unlit_uniform;
} Material, d_Material;

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
                               const d_uint max_directional_lights,
                               const d_uint max_point_lights,
                               const d_uint max_spot_lights);
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
void d_vao_link_attrib(const d_VAO *vao, const d_VBO *vbo, const d_uint layout,
                       const d_uint size, const d_uint type,
                       const d_uint stride, const void *offset);
#pragma endregion

#pragma region VBO Functions
d_VBO *d_vbo_create(const float *vertices, const size_t size);
void d_vbo_destroy(d_VBO **vbo);
void d_vbo_bind(d_VBO *vbo);
void d_vbo_unbind(d_VBO *vbo);
#pragma endregion

#pragma region EBO Functions
d_EBO *d_ebo_create(const d_uint *indices, const size_t size);
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

#pragma region Texture Functions

unsigned char *d_texture_custom_data(d_uint width, d_uint height,
                                     d_Color color_main,
                                     d_Color color_secondary);

d_Texture *d_texture_create(const char *path, d_TextureBlendMode blend_mode);
void d_texture_destroy(d_Texture **texture);
void d_texture_bind(d_Texture *texture);
void d_texture_unbind(d_Texture *texture);

#pragma endregion

#pragma region Material Functions

d_Material *d_material_create(const char *diffuse_path,
                              const char *specular_path, d_Color color);
void d_material_destroy(d_Material **material);
void d_material_get_uniforms(d_Material *material, d_Shader *shader);
void d_material_bind(d_Material *material);
void d_material_unbind(d_Material *material);

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

bool d_gl_error_internal(const char *message, const char *function,
                         const char *file) {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    if (message == NULL || message == "")
      message = "OpenGL error ";

    char *error_code = d_str_from_int(error);
    char *error_message = d_str_append(message, " (");
    error_message = d_str_append(error_message, error_code);
    error_message = d_str_append(error_message, ")");
    d_throw_error_internal(&DUCKY_FAILURE, error_message, false, file,
                           function);
    free(error_code);
    free(error_message);
    return true;
  }

  return false;
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
                               const d_uint max_directional_lights,
                               const d_uint max_point_lights,
                               const d_uint max_spot_lights) {
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

void d_vao_link_attrib(const d_VAO *vao, const d_VBO *vbo, const d_uint layout,
                       const d_uint size, const d_uint type,
                       const d_uint stride, const void *offset) {
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
d_EBO *d_ebo_create(const d_uint *indices, const size_t size) {
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

#pragma region Texture Functions

unsigned char *d_texture_custom_data(d_uint width, d_uint height,
                                     d_Color color_main,
                                     d_Color color_secondary) {
  unsigned char *data = malloc(sizeof(unsigned char) * (width * height * 4));
  if (data == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to malloc data.");
    return NULL;
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      float t = (float)x / width;
      float s = (float)y / height;

      int index = (y * width + x) * 4;
      if (((int)(s * height) + (int)(t * width)) % 2 == 0) {
        data[index] = color_main.r;
        data[index + 1] = color_main.r;
        data[index + 2] = color_main.g;
        data[index + 3] = color_main.a;
      } else {
        data[index] = color_secondary.r;
        data[index + 1] = color_secondary.r;
        data[index + 2] = color_secondary.g;
        data[index + 3] = color_secondary.a;
      }
    }
  }

  return data;
}

d_Texture *d_texture_create(const char *path, d_TextureBlendMode blend_mode) {
  bool invalid_path = false;
  if (path == NULL) {
    invalid_path = true;
  }

  if (d_is_path_valid(path) == false) {
    d_throw_error_silent(DUCKY_WARNING, "Texture path is not valid!");
    invalid_path = true;
  }

  if (invalid_path == true) {
    printf("texture path is invalid\n");
    return NULL;
  }

  int width;
  int height;
  int channels;

  unsigned char *data;

  if (invalid_path == false) {
    data = stbi_load(path, &width, &height, &channels, 0);

    if (data == NULL) {
      char *message = d_str_append("Failed to load image from path: ", path);
      d_throw_error(DUCKY_FAILURE, message);
      free(message);
      return NULL;
    }
  }

  d_Texture *texture = malloc(sizeof(d_Texture));
  if (texture == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "Failed to malloc texture.");
    return NULL;
  }

  glGenTextures(1, &texture->id);
  if (d_gl_error("Failed to generate texture ") == true) {
    free(texture);
    return NULL;
  }

  glBindTexture(GL_TEXTURE_2D, texture->id);
  if (d_gl_error("Failed to bind texture ") == true) {
    d_texture_destroy(&texture);
    return NULL;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  if (d_gl_error("Failed to set texture parameter (GL_TEXTURE_WRAP_S) ") ==
      true) {
    d_texture_destroy(&texture);
    return NULL;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  if (d_gl_error("Failed to set texture parameter (GL_TEXTURE_WRAP_T) ") ==
      true) {
    d_texture_destroy(&texture);
    return NULL;
  }

  if (invalid_path == true) {
    blend_mode = NEAREST;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  blend_mode == LINEAR ? GL_LINEAR : GL_NEAREST);
  if (d_gl_error("Failed to set texture parameter (GL_TEXTURE_MIN_FILTER) ") ==
      true) {
    d_texture_destroy(&texture);
    return NULL;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  blend_mode == LINEAR ? GL_LINEAR : GL_NEAREST);
  if (d_gl_error("Failed to set texture parameter (GL_TEXTURE_MAG_FILTER) ") ==
      true) {
    d_texture_destroy(&texture);
    return NULL;
  }

  texture->blend_mode = blend_mode;

  if (invalid_path == false) {
    GLenum format = channels == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    if (d_gl_error("glTexImage2D failed ") == true) {
      d_texture_destroy(&texture);
      return NULL;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    if (d_gl_error("Failed to generate Mipmap ") == true) {
      d_texture_destroy(&texture);
      return NULL;
    }

    free(data);
  } else {
    data = d_texture_custom_data(4, 4, d_color(0.0f, 0.0f, 0.0f, 1.0f),
                                 d_color(1.0f, 0.0f, 1.0f, 1.0f));
    if (data == NULL) {
      d_throw_error(DUCKY_FAILURE, "Failed to create missing texture.");
      d_texture_destroy(&texture);
      return NULL;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    if (d_gl_error("glTexImage2D failed ") == true) {
      d_texture_destroy(&texture);
      return NULL;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    if (d_gl_error("Failed to generate Mipmap ") == true) {
      d_texture_destroy(&texture);
      return NULL;
    }

    free(data);
  }

  return texture;
}

void d_texture_destroy(d_Texture **texture) {
  if (texture == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "texture (d_Texture **) is NULL.");
    return;
  }
  if (*texture == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "texture (d_Texture *) is NULL.");
    return;
  }

  glDeleteTextures(1, &(*texture)->id);
  (*texture)->id = 0;
  free(*texture);
  *texture = NULL;
}

void d_texture_bind(d_Texture *texture) {
  if (texture == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "texture is NULL.");
    return;
  }

  glBindTexture(GL_TEXTURE_2D, texture->id);
  d_gl_error("Bind texture ");
}

void d_texture_unbind(d_Texture *texture) {
  if (texture == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "texture is NULL.");
    return;
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  d_gl_error("Unbind texture ");
}

#pragma endregion

#pragma region Material Functions

d_Material *d_material_create(const char *diffuse_path,
                              const char *specular_path, d_Color color) {
  d_Material *material = malloc(sizeof(d_Material));

  material->diffuse = d_texture_create(diffuse_path, NEAREST);
  material->specular = d_texture_create(specular_path, NEAREST);
  material->color = color;
  material->specular_strength = 0.5f;
  material->unlit = false;
}

void d_material_destroy(d_Material **material) {
  if (material == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "material (d_Material **) is NULL.");
    return;
  }
  if (*material == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "material (d_Material *) is NULL.");
    return;
  }

  d_texture_destroy(&(*material)->diffuse);

  d_texture_destroy(&(*material)->specular);

  free(*material);
  *material = NULL;
}

void d_material_get_uniforms(d_Material *material, d_Shader *shader) {
  if (material == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "material is NULL.");
    return;
  }

  if (shader == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "shader is NULL.");
    return;
  }

  material->diffuse_uniform =
      glGetUniformLocation(shader->id, "diffuse_texture");
  d_gl_error("Failed to get diffuse texture uniform ");

  material->specular_uniform =
      glGetUniformLocation(shader->id, "specular_texture");
  d_gl_error("Failed to get specular texture uniform ");

  material->color_uniform = glGetUniformLocation(shader->id, "color");
  d_gl_error("Failed to get color uniform ");

  material->specular_strength_uniform =
      glGetUniformLocation(shader->id, "specular_strength");
  d_gl_error("Failed to get specular strength uniform ");

  material->unlit_uniform = glGetUniformLocation(shader->id, "unlit");
  d_gl_error("Failed to get unlit uniform ");
}

void d_material_bind(d_Material *material) {
  if (material == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "material is NULL.");
    return;
  }

  glActiveTexture(GL_TEXTURE0);
  d_texture_bind(material->diffuse);

  glActiveTexture(GL_TEXTURE1);
  d_texture_bind(material->specular);
}

void d_material_unbind(d_Material *material) {
  if (material == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "material is NULL.");
    return;
  }

  d_texture_unbind(material->diffuse);

  d_texture_unbind(material->specular);
}

#pragma endregion

#endif