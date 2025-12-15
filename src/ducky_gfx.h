#ifndef DUCKY_GFX_H
#define DUCKY_GFX_H

#ifdef DUCKY_GLAD_IMPL
#include DUCKY_GLAD_IMPL
#endif

#pragma region Types
typedef struct d_VAO
{
    unsigned int id;
} VAO, d_VAO;

typedef struct d_VBO
{
    unsigned int id;
} VBO, d_VBO;

typedef struct d_EBO
{
    unsigned int id;
} EBO, d_EBO;
#pragma endregion

#endif

#ifdef DUCKY_GFX_IMPL

#endif