#ifndef DUCKY_CORE_H
#define DUCKY_CORE_H

#include <stdlib.h>
#include <string.h>
#ifdef DUCKY_CORE_PRINT_ERRORS
#include <stdio.h>
#endif

#pragma region Error Handling
/*
  Codes:
  - `0`: Success
  - `10`-`19`:  Generic errors
  - `20`-`29`:  Memory errors
  - `30`-`39`:  File errors
  - `40`-`49`:  Graphics errors
  - `50`-`59`:  Audio errors
  - `60`-`69`:  Input errors
  - `70`-`79`:  Window errors
  - `90`-`99`:  Critical errors

*/
typedef struct d_Error {
  const int code;
  const char *name;
} Error, d_Error;

#pragma region Predefined Errors
const d_Error DUCKY_SUCCESS;
const d_Error DUCKY_FAILURE;
const d_Error DUCKY_WARNING;
const d_Error DUCKY_NULL_REFERENCE;
const d_Error DUCKY_MEMORY_FAILURE;
const d_Error DUCKY_SDL_INIT_FAILURE;
const d_Error DUCKY_WINDOW_CREATION_FAILURE;
#pragma endregion

void d_throw_error(d_Error error, const char *message, const char *file,
                   const char *function);

#pragma endregion

#pragma region Dynamic Array

typedef struct d_DArray {
  void *data;
  unsigned int element_size;
  unsigned int length;
} DArray, d_DArray;

d_DArray *d_darray_create(const unsigned int element_size);
void d_darray_destroy(d_DArray *array);
void d_darray_add(d_DArray *array, const void *element);
void d_darray_remove(d_DArray *array, const unsigned int index);
void *d_darray_get(d_DArray *array, const unsigned int index);
void d_darray_clear(d_DArray *array);
void d_darray_length(d_DArray *array);

#pragma region Event System

#pragma endregion

#endif

#ifdef DUCKY_CORE_IMPL
const d_Error DUCKY_SUCCESS = {0, "DUCKY_SUCCESS"};
const d_Error DUCKY_FAILURE = {10, "DUCKY_FAILURE"};
const d_Error DUCKY_WARNING = {11, "DUCKY_WARNING"};
const d_Error DUCKY_NULL_REFERENCE = {12, "DUCKY_NULL_REFERENCE"};
const d_Error DUCKY_MEMORY_FAILURE = {20, "DUCKY_MEMORY_FAILURE"};
const d_Error DUCKY_SDL_INIT_FAILURE = {70, "DUCKY_SDL_INIT_FAILURE"};
const d_Error DUCKY_WINDOW_CREATION_FAILURE = {71,
                                               "DUCKY_WINDOW_CREATION_FAILURE"};

const d_Error DUCKY_CRITICAL = {90, "DUCKY_CRITICAL"};

void d_throw_error(d_Error error, const char *message, const char *file,
                   const char *function) {
  printf("[%d]: (%s, %s) %s\n", error, file, function, message);

  if (error.code <= DUCKY_CRITICAL.code && error.code < 100) {
    exit(DUCKY_CRITICAL.code);
  }
}

d_DArray *d_darray_create(const unsigned int element_size) {
  d_DArray *array = malloc(sizeof(d_DArray));
  if (array == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "malloc failed.", __FILE__,
                  __FUNCTION__);
    return NULL;
  }
  array->data = NULL;
  array->element_size = element_size;
  array->length = 0;
  return array;
}

void d_darray_destroy(d_DArray *array) {
  if (array == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "array is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }
  if (array->data != NULL) {
    free(array->data);
  }
  free(array);
}

void d_darray_add(d_DArray *array, const void *element) {
  if (array == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "array is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  void *temp = NULL;

  temp = realloc(array->data, array->element_size * (array->length + 1));
  if (temp == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "realloc failed.", __FILE__,
                  __FUNCTION__);
    return;
  }

  // if (array->data != NULL) {
  //   memcpy(temp, array->data, array->element_size * array->length);
  // }

  memcpy((char *)temp + array->element_size * array->length, element,
         array->element_size);

  array->data = temp;
  array->length++;
}

void d_darray_remove(d_DArray *array, const unsigned int index) {
  if (array == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "array is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  if (array->data == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "array data is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  if (index >= array->length) {
    d_throw_error(DUCKY_FAILURE, "index out of bounds.", __FILE__,
                  __FUNCTION__);
    return;
  }

  void *target = (char *)array->data + (array->element_size * index);
  void *last =
      (char *)array->data + (array->element_size * (array->length - 1));
  void *temp = malloc(array->element_size);

  memcpy(temp, last, array->element_size);

  memcpy((char *)array->data + (array->element_size * (array->length - 1)),
         target, array->element_size);

  memcpy((char *)array->data + (array->element_size * index), temp,
         array->element_size);

  void *resized =
      realloc(array->data, array->element_size * (array->length - 1));

  if (resized == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "realloc failed.", __FILE__,
                  __FUNCTION__);
    return;
  }

  array->data = resized;
  array->length--;
}

#endif
