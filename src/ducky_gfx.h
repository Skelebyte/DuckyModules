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
#include <stdlib.h>

#pragma region Types
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

#endif

#ifdef DUCKY_GFX_IMPL

#pragma region VAO Functions
d_VAO *d_vao_create() {
  d_VAO *vao = malloc(sizeof(d_VAO));
  if (vao == NULL) {
    d_throw_error(DUCKY_MALLOC_FAILURE, "(d_vao_create) malloc failed.",
                  __FILE__, __FUNCTION__);
    return NULL;
  }
  glGenVertexArrays(1, &vao->id);
  return vao;
}

void d_vao_destroy(d_VAO *vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_vao_destroy) vao is NULL.",
                  __FILE__, __FUNCTION__);
    return;
  }
  glDeleteVertexArrays(1, &vao->id);
  free(vao);
}

void d_vao_bind(d_VAO *vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_vao_bind) vao is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  glBindVertexArray(vao->id);
  vao->bound = true;
}

void d_vao_unbind(d_VAO *vao) {
  if (vao == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_vao_unbind) vao is NULL.", __FILE__,
                  __FUNCTION__);
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
    d_throw_error(DUCKY_MALLOC_FAILURE, "(d_vbo_create) malloc failed.",
                  __FILE__, __FUNCTION__);
    return NULL;
  }
  glGenBuffers(1, &vbo->id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  return vbo;
}

void d_vbo_destroy(d_VBO *vbo) {
  if (vbo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_vbo_destroy) vbo is NULL.",
                  __FILE__, __FUNCTION__);
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
    d_throw_error(DUCKY_MALLOC_FAILURE, "(d_ebo_create) malloc failed.",
                  __FILE__, __FUNCTION__);
    return NULL;
  }
  glGenBuffers(1, &ebo->id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  return ebo;
}

void d_ebo_destroy(d_EBO *ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_ebo_destroy) ebo is NULL.",
                  __FILE__, __FUNCTION__);
    return;
  }
  glDeleteBuffers(1, &ebo->id);
  free(ebo);
}

void d_ebo_bind(d_EBO *ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_ebo_bind) ebo is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
  ebo->bound = true;
}

void d_ebo_unbind(d_EBO *ebo) {
  if (ebo == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_ebo_unbind) ebo is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  ebo->bound = false;
}

#pragma endregion

#endif