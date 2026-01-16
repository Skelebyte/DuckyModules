#ifndef DUCKY_OBJS_H
#define DUCKY_OBJS_H

#ifndef DUCKY_CORE_H
#include "ducky_core.h"
#endif

#ifndef DUCKY_MATH_H
#include "ducky_math.h"
#endif

#pragma region ObjectManager

#pragma endregion

#pragma region Transform
typedef struct d_Transform {
  Vec3 position;
  Vec3 rotation;
  Vec3 scale;

  d_Transform *parent;
  d_Array *children;

  unsigned int id;
} Transform, d_Transform;

d_Transform *d_transform_create();
void d_transform_destroy(d_Transform *target);
void d_transform_add_child(d_Transform *target, d_Transform *child);
void d_transform_remove_child(d_Transform *target, d_Transform *child);
void d_transform_update(d_Transform *target);

#pragma endregion

#pragma region Object

typedef struct d_Object {
  char *name;
  d_Transform *transform;
} Object, d_Object;

// TODO:
// create and destroy (create: specifing name and creating the transform)
// (destroy: frees the object and destroys the transform) update (also updates
// the transform)

#pragma endregion

#endif

#ifdef DUCKY_OBJS_IMPL

#pragma region Transform

d_Transform *d_transform_create() {
  d_Transform *transform = malloc(sizeof(d_Transform));

  transform->position = d_vec3(0.0f, 0.0f, 0.0f);
  transform->rotation = d_vec3(0.0f, 0.0f, 0.0f);
  transform->scale = d_vec3(1.0f, 1.0f, 1.0f);

  transform->parent = NULL;
  transform->children = d_array_create(d_Transform, 1);

  return transform;
}

void d_transform_add_child(d_Transform *target, d_Transform *child) {
  d_array_add(target->children, child);
}

void d_transform_remove_child(d_Transform *target, d_Transform *child) {
  d_throw_error(DUCKY_WARNING, "d_Array currently does not support removing!");
}

void d_transform_update(d_Transform *target) {
  if (target->rotation.x > 360)
    target->rotation.x -= 360;
  if (target->rotation.x < -360)
    target->rotation.x += 360;

  if (target->rotation.y > 360)
    target->rotation.y -= 360;
  if (target->rotation.y < -360)
    target->rotation.y += 360;

  if (target->rotation.z > 360)
    target->rotation.z -= 360;
  if (target->rotation.z < -360)
    target->rotation.z += 360;
}

#pragma endregion

#endif