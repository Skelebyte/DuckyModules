#ifndef DUCKY_OBJS_H
#define DUCKY_OBJS_H

#ifndef DUCKY_CORE_H
#include "ducky_core.h"
#endif

#ifndef DUCKY_MATH_H
#include "ducky_math.h"
#endif

#ifndef DUCKY_GFX_H
#include "ducky_gfx.h"
#endif

#include "ufbx/ufbx.h"

#pragma region ObjectManager

#pragma endregion

#pragma region Transform
typedef struct d_Transform {
  Vec3 position;
  Vec3 rotation;
  Vec3 scale;

  d_Array *children;

  d_uint id;
} Transform, d_Transform;

d_Transform *d_transform_create();
void d_transform_destroy(d_Transform **target);
void d_transform_add_child(d_Transform *target, d_Transform *child);
void d_transform_remove_child(d_Transform *target, d_Transform *child);
void d_transform_update(d_Transform *target);

#pragma endregion

#pragma region Mesh

typedef struct d_Vertex {
  d_Vec3 position;
  d_Vec3 normal;
  d_Vec2 uv;
  d_uint index;
} Vertex, d_Vertex;

d_Vertex d_vertex_create(d_Vec3 pos, d_Vec3 norm, d_Vec2 UV, d_uint idx);

typedef struct d_Mesh {
  const char *path;

  d_Array *vertices;
  d_Array *indices;

  d_uint vertex_count;
  d_uint edge_count;
  d_uint face_count;
} Mesh, d_Mesh;

d_Mesh *d_mesh_load(const char *path);
void d_mesh_destroy(d_Mesh **mesh);

#pragma endregion

#pragma region Object

typedef struct d_Object {
  char *name;
  d_Transform *transform;
} Object, d_Object;

d_Object *d_object_create(char *name);
void d_object_destroy(d_Object **target);

#pragma endregion

#pragma region MeshRenderer

typedef struct d_MeshRenderer {
  d_Transform *transform;
  d_Mesh *mesh;

  d_Material *material;

  d_VAO *vao;
  d_VBO *vbo;
  d_EBO *ebo;
} MeshRenderer, d_MeshRenderer;

d_MeshRenderer *d_mesh_renderer_create(const char *mesh_path);
void d_mesh_renderer_destroy(d_MeshRenderer **mesh_renderer);
void d_mesh_renderer_update(d_MeshRenderer *mesh_renderer);

#pragma endregion

#pragma region Camera

typedef struct d_Camera {
  d_Transform *transform;

  float field_of_view;
  float near_plane;
  float far_plane;

  d_Mat4 projection;
  d_Mat4 view;

} Camera, d_Camera;

d_Camera *d_camera_create(float fov, float near, float far);
void d_camera_destroy(d_Camera **camera);

#pragma endregion

#endif

#ifdef DUCKY_OBJS_IMPL

#pragma region Transform

d_Transform *d_transform_create() {
  d_Transform *transform = malloc(sizeof(d_Transform));

  transform->position = d_vec3(0.0f, 0.0f, 0.0f);
  transform->rotation = d_vec3(0.0f, 0.0f, 0.0f);
  transform->scale = d_vec3(1.0f, 1.0f, 1.0f);

  transform->children = d_array_create(d_Transform, 1);

  return transform;
}

void d_transform_destroy(d_Transform **target) {
  if (target == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "target (d_Transform **) is NULL.");
    return;
  }
  if (*target == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "target is NULL.");
    return;
  }

  free(*target);
  *target = NULL;
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

#pragma region Mesh

d_Vertex d_vertex_create(d_Vec3 pos, d_Vec3 norm, d_Vec2 UV, d_uint idx) {
  d_Vertex vertex = {pos, norm, UV, idx};

  return vertex;
}

d_Mesh *d_mesh_load(const char *path) {
  if (path == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "path is NULL.");
    return NULL;
  }
  if (path == "") {
    d_throw_error(DUCKY_EMPTY_REFERENCE, "path is empty.");
    return NULL;
  }

  d_Mesh *mesh = malloc(sizeof(d_Mesh));
  if (mesh == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to malloc mesh.");
    return NULL;
  }

  mesh->vertices = d_array_create(d_Vertex, 1);
  mesh->indices = d_array_create(d_uint, 1);

  ufbx_error error;
  ufbx_scene *scene = ufbx_load_file(path, NULL, &error);
  if (scene == NULL) {
    char *message = "Failed to load model (path: ";
    message = d_str_append(message, path);
    message = d_str_append(message, "). ");
    message = d_str_append(message, error.description.data);

    d_throw_error(DUCKY_FAILURE, message);
    free(message);
    free(mesh);
    return NULL;
  }

  for (size_t i = 0; i < scene->nodes.count; i++) {
    ufbx_node *node = scene->nodes.data[i];
    if (node->is_root)
      continue;

    printf("object: %s\n", node->name.data);
    if (node->mesh) {
      ufbx_mesh *fbx_mesh = node->mesh;

      for (size_t j = 0; j < fbx_mesh->num_vertices; j++) {
        size_t index = j;
        ufbx_vec3 fbx_pos =
            ufbx_get_vertex_vec3(&fbx_mesh->vertex_position, index);
        ufbx_vec3 fbx_norm =
            ufbx_get_vertex_vec3(&fbx_mesh->vertex_normal, index);
        ufbx_vec2 fbx_uv = ufbx_get_vertex_vec2(&fbx_mesh->vertex_uv, index);

        d_Vec3 pos = d_vec3(fbx_pos.x, fbx_pos.y, fbx_pos.z);
        d_Vec3 norm = d_vec3(fbx_norm.x, fbx_norm.y, fbx_norm.z);
        d_Vec2 uv = d_vec2(fbx_uv.x, fbx_uv.y);

        d_Vertex vertex = d_vertex_create(pos, norm, uv, index);

        d_array_add(mesh->vertices, (void *)&vertex);
      }

      for (size_t j = 0; j < fbx_mesh->vertex_indices.count; j++) {

        d_uint index = fbx_mesh->vertex_indices.data[j];

        d_array_add(mesh->indices, (void *)&index);
      }

      mesh->vertex_count = fbx_mesh->vertices.count;
      mesh->edge_count = fbx_mesh->edges.count;
      mesh->face_count = fbx_mesh->faces.count;
    }
  }

  ufbx_free_scene(scene);
  return mesh;
}

void d_mesh_destroy(d_Mesh **mesh) {
  if (mesh == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "mesh (d_Mesh **) is NULL.");
    return;
  }
  if (*mesh == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "mesh (d_Mesh *) is NULL.");
    return;
  }

  free(*mesh);
  *mesh = NULL;
}

#pragma endregion

#pragma region Object

d_Object *d_object_create(char *name) {
  if (name == NULL) {
    name = "new_object";
  }

  d_Object *new_object = malloc(sizeof(d_Object));
  if (new_object == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to malloc new_object.");
    return NULL;
  }

  new_object->name = name;
  new_object->transform = d_transform_create();

  return new_object;
}

void d_object_destroy(d_Object **target) {
  if (target == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "target (d_Object **) is NULL.");
    return;
  }
  if (*target == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "target (d_Object *) is NULL");
    return;
  }

  d_transform_destroy(&(*target)->transform);
  free(*target);
  *target = NULL;
}

#pragma endregion

#pragma region MeshRenderer

d_MeshRenderer *d_mesh_renderer_create(const char *mesh_path) {

  d_MeshRenderer *mesh_renderer = malloc(sizeof(d_MeshRenderer));

  mesh_renderer->transform = d_transform_create();
  mesh_renderer->mesh = d_mesh_load(mesh_path);
  mesh_renderer->material = NULL;

  return mesh_renderer;
}

void d_mesh_renderer_destroy(d_MeshRenderer **mesh_renderer) {
  if (mesh_renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE,
                  "mesh_renderer (d_MeshRenderer **) is NULL.");
    return;
  }
  if (*mesh_renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE,
                  "mesh_renderer (d_MeshRenderer *) is NULL.");
    return;
  }

  d_transform_destroy(&(*mesh_renderer)->transform);
  d_mesh_destroy(&(*mesh_renderer)->mesh);

  d_material_destroy(&(*mesh_renderer)->material);

  free(*mesh_renderer);
  *mesh_renderer = NULL;
}

void d_mesh_renderer_update(d_MeshRenderer *mesh_renderer) {
  if (mesh_renderer == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "mesh_renderer is NULL");
    return;
  }

  d_transform_update(mesh_renderer->transform);

  if (mesh_renderer->material == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "material must not be NULL!");
    return;
  }
}

#pragma endregion

#pragma region Camera

d_Camera *d_camera_create(float fov, float near, float far) {
  if (fov <= 0.0f) {
    fov = 60.0f;
  }

  if (near <= 0.0f) {
    near = 0.01f;
  }

  if (far <= 0.0f) {
    far = 100.0f;
  }

  d_Camera *camera = malloc(sizeof(d_Camera));

  camera->field_of_view = fov;
  camera->near_plane = near;
  camera->far_plane = far;

  camera->projection = d_mat4(true);
  // setting camera->projection to the actual d_mat4_projection function will be
  // done in the d_camera_update function

  camera->view = d_mat4(true);
}

#pragma endregion

#endif