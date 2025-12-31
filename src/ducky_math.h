#ifndef DUCKY_MATH_H
#define DUCKY_MATH_H

#ifndef DUCKY_CORE_H
#include "ducky_core.h"
#endif

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define D_PI 3.14159265358979323846f

#pragma region Types

// 3D vector with float components
typedef union d_Vec3 {
  struct {
    float x;
    float y;
    float z;
  };
  float data[3];
} Vec3, Vector3, d_Vec3;

// 3D vector with int components
typedef union d_Vec3i {
  struct {
    int x;
    int y;
    int z;
  };
  int data[3];
} Vec3i, Vector3i, d_Vec3i;

// 2D vector with float components
typedef union d_Vec2 {
  struct {
    float x;
    float y;
  };
  float data[2];
} Vec2, Vector2, d_Vec2;

// 2D vector with int components
typedef union d_Vec2i {
  struct {
    int x;
    int y;
  };
  int data[2];
} Vec2i, Vector2i, d_Vec2i;

// 4D vector with float components
typedef union d_Vec4 {
  struct {
    float x;
    float y;
    float z;
    float w;
  };
  float data[4];
} Vec4, Vector4, d_Vec4;

// 4x4 matrix with float components
typedef struct d_Mat4 {
  float data[16];
} Mat4, Matrix4, d_Mat4;
#pragma endregion

#pragma region Math Functions
float d_to_radians(const float degrees);
float d_to_degrees(const float radians);
float d_lerpf(const float a, const float b, const float t);
#pragma endregion

#pragma region Vec3 Functions
d_Vec3 d_vec3(const float x, const float y, const float z);
d_Vec3 d_vec3_add(const d_Vec3 *a, const d_Vec3 *b);
d_Vec3 d_vec3_sub(const d_Vec3 *a, const d_Vec3 *b);
float d_vec3_dot(const d_Vec3 *a, const d_Vec3 *b);
float d_vec3_len(const d_Vec3 *a);
d_Vec3 d_vec3_normalized(const d_Vec3 *a);
d_Vec3 d_vec3_cross(const d_Vec3 *a, const d_Vec3 *b);
#pragma endregion

#pragma region Vec3i Functions
d_Vec3i d_vec3i(const int x, const int y, const int z);
d_Vec3i d_vec3i_add(const d_Vec3i *a, const d_Vec3i *b);
d_Vec3i d_vec3i_sub(const d_Vec3i *a, const d_Vec3i *b);
float d_vec3i_dot(const d_Vec3i *a, const d_Vec3i *b);
float d_vec3i_len(const d_Vec3i *a);
#pragma endregion

#pragma region Vec2 Functions
d_Vec2 d_vec2(const float x, const float y);
d_Vec2 d_vec2_add(const d_Vec2 *a, const d_Vec2 *b);
d_Vec2 d_vec2_sub(const d_Vec2 *a, const d_Vec2 *b);
float d_vec2_dot(const d_Vec2 *a, const d_Vec2 *b);
float d_vec2_len(const d_Vec2 *a);
d_Vec2 d_vec2_normalized(const d_Vec2 *a);
#pragma endregion

#pragma region Vec2i Functions
d_Vec2i d_vec2i(const int x, const int y);
d_Vec2i d_vec2i_add(const d_Vec2i *a, const d_Vec2i *b);
d_Vec2i d_vec2i_sub(const d_Vec2i *a, const d_Vec2i *b);
float d_vec2i_dot(const d_Vec2i *a, const d_Vec2i *b);
float d_vec2i_len(const d_Vec2i *a);
#pragma endregion

#pragma region Vec4 Functions
d_Vec4 d_vec4(const float x, const float y, const float z, const float w);
#pragma endregion

#pragma region Mat4 Functions
void d_mat4(d_Mat4 *a, bool identity);
void d_mat4_translate(d_Mat4 *a, const d_Vec3 *v);
void d_mat4_rotate(d_Mat4 *a, const d_Vec3 *v);
void d_mat4_scale(d_Mat4 *a, const d_Vec3 *v);
d_Mat4 d_mat4_multiply(const d_Mat4 *a, const d_Mat4 *b);
void d_mat4_perspective(d_Mat4 *a, float fov_degrees, float aspect_ratio,
                        float near, float far);
void d_mat4_orthogonal(d_Mat4 *a, float left, float right, float bottom,
                       float top, float near, float far);
void d_mat4_look_at(d_Mat4 *a, const d_Vec3 *position,
                    const d_Vec3 *target_position, const d_Vec3 *forward);
void d_mat4_inverse(d_Mat4 *a);
#pragma endregion
#endif

#ifdef DUCKY_MATH_IMPL

#pragma region Math Functions
float d_to_radians(const float degrees) { return degrees * D_PI / 180.0f; }

float d_to_degrees(const float radians) { return radians / D_PI * 180.0f; }

float d_lerpf(const float a, const float b, const float t) {
  return a + t * (b - a);
}
#pragma endregion

#pragma region Vec3 Functions
d_Vec3 d_vec3(const float x, const float y, const float z) {
  d_Vec3 result;
  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}

d_Vec3 d_vec3_add(const d_Vec3 *a, const d_Vec3 *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return d_vec3(0.0f, 0.0f, 0.0f);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return d_vec3(0.0f, 0.0f, 0.0f);
  }

  d_Vec3 result;
  result.x = a->x + b->x;
  result.y = a->y + b->y;
  result.z = a->z + b->z;
  return result;
}

d_Vec3 d_vec3_sub(const d_Vec3 *a, const d_Vec3 *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return d_vec3(0.0f, 0.0f, 0.0f);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return d_vec3(0.0f, 0.0f, 0.0f);
  }

  d_Vec3 result;
  result.x = a->x - b->x;
  result.y = a->y - b->y;
  result.z = a->z - b->z;
  return result;
}

float d_vec3_dot(const d_Vec3 *a, const d_Vec3 *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }

  return a->x * b->x + a->y * b->y + a->z * b->z;
}

float d_vec3_len(const d_Vec3 *a) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }

  return sqrt(pow(a->x, 2) + pow(a->y, 2) + pow(a->z, 2));
}

d_Vec3 d_vec3_normalized(const d_Vec3 *a) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return d_vec3(0.0f, 0.0f, 0.0f);
  }

  float len = d_vec3_len(a);
  if (len == 0) {
    d_throw_error(DUCKY_FAILURE, "Cannot normalize zero-length vector",
                  __FILE__, __FUNCTION__);
    return *a;
  }
  d_Vec3 result;
  result.x = a->x / len;
  result.y = a->y / len;
  result.z = a->z / len;
  return result;
}

d_Vec3 d_vec3_cross(const d_Vec3 *a, const d_Vec3 *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return d_vec3(0.0f, 0.0f, 0.0f);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return d_vec3(0.0f, 0.0f, 0.0f);
  }

  return d_vec3(a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z,
                a->x * b->y - a->y * b->x);
}
#pragma endregion

#pragma region Vec3i Functions
d_Vec3i d_vec3i(const int x, const int y, const int z) {
  d_Vec3i result;
  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}

d_Vec3i d_vec3i_add(const d_Vec3i *a, const d_Vec3i *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return d_vec3i(0, 0, 0);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return d_vec3i(0, 0, 0);
  }

  d_Vec3i result;
  result.x = a->x + b->x;
  result.y = a->y + b->y;
  result.z = a->z + b->z;
  return result;
}

d_Vec3i d_vec3i_sub(const d_Vec3i *a, const d_Vec3i *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return d_vec3i(0, 0, 0);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return d_vec3i(0, 0, 0);
  }

  d_Vec3i result;
  result.x = a->x - b->x;
  result.y = a->y - b->y;
  result.z = a->z - b->z;
  return result;
}

float d_vec3i_dot(const d_Vec3i *a, const d_Vec3i *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }

  return a->x * b->x + a->y * b->y + a->z * b->z;
}

float d_vec3i_len(const d_Vec3i *a) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }

  return sqrt(pow(a->x, 2) + pow(a->y, 2) + pow(a->z, 2));
}
#pragma endregion

#pragma region Vec2 Functions
d_Vec2 d_vec2(const float x, const float y) {
  d_Vec2 result;
  result.x = x;
  result.y = y;
  return result;
}

d_Vec2 d_vec2_add(const d_Vec2 *a, const d_Vec2 *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return d_vec2(0.0f, 0.0f);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return d_vec2(0.0f, 0.0f);
  }

  d_Vec2 result;
  result.x = a->x + b->x;
  result.y = a->y + b->y;
  return result;
}

d_Vec2 d_vec2_sub(const d_Vec2 *a, const d_Vec2 *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return d_vec2(0.0f, 0.0f);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return d_vec2(0.0f, 0.0f);
  }

  d_Vec2 result;
  result.x = a->x - b->x;
  result.y = a->y - b->y;
  return result;
}

float d_vec2_dot(const d_Vec2 *a, const d_Vec2 *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }

  return a->x * b->x + a->y * b->y;
}

float d_vec2_len(const d_Vec2 *a) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL", __FILE__, __FUNCTION__);
    return 0.0f;
  }
  return sqrt(pow(a->x, 2) + pow(a->y, 2));
}

d_Vec2 d_vec2_normalized(const d_Vec2 *a) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return d_vec2(0.0f, 0.0f);
  }

  float len = d_vec2_len(a);
  if (len == 0) {
    d_throw_error(DUCKY_FAILURE, "Cannot normalize zero-length vector",
                  __FILE__, __FUNCTION__);
    return *a;
  }
  d_Vec2 result;
  result.x = a->x / len;
  result.y = a->y / len;
  return result;
}
#pragma endregion

#pragma region Vec2i Functions
d_Vec2i d_vec2i(const int x, const int y) {
  d_Vec2i result;
  result.x = x;
  result.y = y;
  return result;
}

d_Vec2i d_vec2i_add(const d_Vec2i *a, const d_Vec2i *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return d_vec2i(0, 0);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL.", __FILE__, __FUNCTION__);
    return d_vec2i(0, 0);
  }

  d_Vec2i result;
  result.x = a->x + b->x;
  result.y = a->y + b->y;
  return result;
}

d_Vec2i d_vec2i_sub(const d_Vec2i *a, const d_Vec2i *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return d_vec2i(0, 0);
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL.", __FILE__, __FUNCTION__);
    return d_vec2i(0, 0);
  }

  d_Vec2i result;
  result.x = a->x - b->x;
  result.y = a->y - b->y;
  return result;
}

float d_vec2i_dot(const d_Vec2i *a, const d_Vec2i *b) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return 0.0f;
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL.", __FILE__, __FUNCTION__);
    return 0.0f;
  }

  return a->x * b->x + a->y * b->y;
}

float d_vec2i_len(const d_Vec2i *a) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return 0.0f;
  }
  return sqrt(pow(a->x, 2) + pow(a->y, 2));
}
#pragma endregion

#pragma region Vec4 Functions
d_Vec4 d_vec4(const float x, const float y, const float z, const float w) {
  d_Vec4 result;
  result.x = x;
  result.y = y;
  result.z = z;
  result.w = w;
  return result;
}
#pragma endregion

#pragma region Mat4 Functions

void d_mat4(d_Mat4 *a, bool identity) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return;
  }

  if (identity == true) {
    for (int i = 0; i < 16; i++) {
      a->data[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
  } else {
    for (int i = 0; i < 16; i++) {
      a->data[i] = 0.0f;
    }
  }
}

void d_mat4_translate(d_Mat4 *a, const d_Vec3 *pos) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  if (pos == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "pos is NULL.", __FILE__, __FUNCTION__);
    return;
  }

  a->data[12] = pos->x;
  a->data[13] = pos->y;
  a->data[14] = pos->z;
}

void d_mat4_rotate(d_Mat4 *a, const d_Vec3 *rot) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  if (rot == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "rot is NULL.", __FILE__, __FUNCTION__);
    return;
  }

  d_Mat4 x_rot;
  d_mat4(&x_rot, true);
  d_Mat4 y_rot;
  d_mat4(&y_rot, true);
  d_Mat4 z_rot;
  d_mat4(&z_rot, true);

  d_Vec3 radians =
      d_vec3(d_to_radians(rot->x), d_to_radians(rot->y), d_to_radians(rot->z));

  x_rot.data[5] = cos(radians.x);
  x_rot.data[6] = -sin(radians.x);
  x_rot.data[9] = sin(radians.x);
  x_rot.data[10] = cos(radians.x);
  y_rot.data[0] = cos(radians.y);
  y_rot.data[2] = sin(radians.y);
  y_rot.data[8] = -sin(radians.y);
  y_rot.data[10] = cos(radians.y);
  z_rot.data[0] = cos(radians.z);
  z_rot.data[1] = sin(radians.z);
  z_rot.data[4] = -sin(radians.z);
  z_rot.data[5] = cos(radians.z);

  d_Mat4 xy_rot = d_mat4_multiply(&x_rot, &y_rot);
  d_Mat4 xyz_rot = d_mat4_multiply(&xy_rot, &z_rot);
  *a = d_mat4_multiply(a, &xyz_rot);
}

void d_mat4_scale(d_Mat4 *a, const d_Vec3 *scale) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  if (scale == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "scale is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  a->data[0] = scale->x;
  a->data[5] = scale->y;
  a->data[10] = scale->z;
}

void d_mat4_perspective(d_Mat4 *a, float fov_degrees, float aspect_ratio,
                        float near, float far) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return;
  }

  float fov_radians = d_to_radians(fov_degrees);

  a->data[0] = 1 / (aspect_ratio * tan(fov_radians / 2));
  a->data[5] = 1 / tan(fov_radians / 2);
  a->data[10] = -(far + near) / (far - near);
  a->data[11] = -1;
  a->data[14] = -(2 * far * near) / (far - near);
  a->data[15] = 0;
}

void d_mat4_orthogonal(d_Mat4 *a, float left, float right, float bottom,
                       float top, float near, float far) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  a->data[0] = 2 / (right - left);
  a->data[5] = 2 / (top - bottom);
  a->data[10] = -2 / (far - near);
  a->data[12] = -(right + left) / (right - left);
  a->data[13] = -(top + bottom) / (top - bottom);
  a->data[14] = -(far + near) / (far - near);
  a->data[15] = 1;
}

void d_mat4_look_at(d_Mat4 *a, const d_Vec3 *position,
                    const d_Vec3 *target_position, const d_Vec3 *forward) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return;
  }
  if (position == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "position is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  if (target_position == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "target_position is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  if (forward == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "forward is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  d_mat4(a, true);

  d_Vec3 temp_up = d_vec3(0.0f, 1.0f, 0.0f);
  d_Vec3 temp_forward = d_vec3_sub(position, target_position);

  d_Vec3 forward_ = d_vec3_normalized(&temp_forward);
  d_Vec3 right = d_vec3_cross(&temp_up, &forward_);
  d_Vec3 up = d_vec3_cross(&forward_, &right);

  a->data[0] = right.x;
  a->data[4] = right.y;
  a->data[8] = right.z;
  a->data[12] = -d_vec3_dot(&right, position);
  a->data[1] = up.x;
  a->data[5] = up.y;
  a->data[9] = up.z;
  a->data[13] = -d_vec3_dot(&up, position);
  a->data[2] = forward_.x;
  a->data[6] = forward_.y;
  a->data[10] = forward_.z;
  a->data[14] = -d_vec3_dot(&forward_, position);
}

void d_mat4_inverse(d_Mat4 *a) {
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return;
  }

  Mat4 inv;
  d_mat4(&inv, true);

  inv.data[0] = a->data[0];
  inv.data[1] = a->data[4];
  inv.data[2] = a->data[8];
  inv.data[3] = 0;
  inv.data[4] = a->data[1];
  inv.data[5] = a->data[5];
  inv.data[6] = a->data[9];
  inv.data[7] = 0;
  inv.data[8] = a->data[2];
  inv.data[9] = a->data[6];
  inv.data[10] = a->data[10];
  inv.data[11] = 0;
  inv.data[15] = 1;
  inv.data[12] = 0;
  inv.data[13] = 0;
  inv.data[14] = 0;

  d_Vec3 t = d_vec3(a->data[12], a->data[13], a->data[14]);
  d_Vec3 invT =
      d_vec3(-(inv.data[0] * t.x + inv.data[4] * t.y + inv.data[8] * t.z),
             -(inv.data[1] * t.x + inv.data[5] * t.y + inv.data[9] * t.z),
             -(inv.data[2] * t.x + inv.data[6] * t.y + inv.data[10] * t.z));

  inv.data[12] = invT.x;
  inv.data[13] = invT.y;
  inv.data[14] = invT.z;

  *a = inv;
}

d_Mat4 d_mat4_multiply(const d_Mat4 *a, const d_Mat4 *b) {
  d_Mat4 result;
  d_mat4(&result, true);
  if (a == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "a is NULL.", __FILE__, __FUNCTION__);
    return result;
  }
  if (b == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "b is NULL.", __FILE__, __FUNCTION__);
    return result;
  }

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      float sum = 0;
      for (int i = 0; i < 4; i++) {
        sum += a->data[i * 4 + row] * b->data[col * 4 + i];
      }
      result.data[col * 4 + row] = sum;
    }
  }

  return result;
}
#pragma endregion

#endif