#ifndef DUCKY_CORE_H
#define DUCKY_CORE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifdef DUCKY_CORE_PRINT_ERRORS
#include <stdio.h>
#endif

#pragma region Core

void d_core_init();
void d_core_shutdown();

#pragma endregion

#pragma region Error Handling
/*
  Codes:
  - `0`: Success
  - `10`-`19`:  Generic errors
  - `20`-`29`:  Memory errors
  - `30`-`39`:  File errors

  - `50`-`59`:  Audio errors
  - `60`-`69`:  Input errors
  - `70`-`79`:  Window errors
  - `80`-`89`:  Graphics/OpenGL errors
  - `90`-`99`:  Critical errors

*/
typedef struct d_Error {
  const int code;
  const char *name;
} Error, d_Error;

typedef struct d_FullError {
  const d_Error *error;
  const char *message;
  const char *file;
  const char *function;
} FullError, d_FullError;

#pragma region Predefined Errors
const d_Error DUCKY_SUCCESS;
const d_Error DUCKY_FAILURE;
const d_Error DUCKY_WARNING;
const d_Error DUCKY_NULL_REFERENCE;
const d_Error DUCKY_EMPTY_REFERENCE;
const d_Error DUCKY_INDEX_OUT_OF_BOUNDS;
const d_Error DUCKY_MEMORY_FAILURE;
const d_Error DUCKY_SDL_INIT_FAILURE;
const d_Error DUCKY_WINDOW_CREATION_FAILURE;
const d_Error DUCKY_SHADER_COMPILE_FAILURE;
const d_Error DUCKY_SHADER_PROGRAM_LINK_FAILURE;
d_FullError *d_last_error;
#pragma endregion

void d_throw_error_internal(const d_Error *error, const char *message,
                            const char *file, const char *function);

#define d_throw_error(error, message)                                          \
  d_throw_error_internal(&error, message, __FILE__, __FUNCTION__)

#pragma endregion

#pragma region Dynamic Array

typedef struct d_Array {
  void *data;
  size_t element_size;
  size_t length;
  size_t capacity;
} Array, d_Array;

d_Array *d_array_create_internal(size_t element_size, size_t initial_capacity);
#define d_array_create(type, initial_capacity)                                 \
  d_array_create_internal(sizeof(type), initial_capacity)

void d_array_add(d_Array *array, void *element);

#define d_array_get(array, type, index)                                        \
  (*(type *)d_array_get_internal(array, index))

void *d_array_get_internal(d_Array *array, unsigned int index);
void d_array_destroy(d_Array *array);

#pragma endregion

#pragma region Event System

typedef void (*d_EventListener)();
typedef d_EventListener EventListener;

typedef struct d_Event {
  const char *name;
  d_Array *listeners;
} Event, d_Event;

d_Event *d_event_create(const char *name);
void d_event_destroy(d_Event *event);
void d_event_add_listener(d_Event *event, d_EventListener listener);
void d_event_invoke(d_Event *event);

typedef struct d_EventSystem {
  d_Array *events;
} EventSystem, d_EventSystem;

d_EventSystem *d_event_system;

d_EventSystem *d_event_system_create();
void d_event_system_destroy(d_EventSystem *event_system);
d_Event *d_event_system_get_event(d_EventSystem *event_system,
                                  const char *name);
void d_event_system_add_event(d_EventSystem *event_system, const char *name);

#pragma endregion

#pragma region File

typedef struct d_File {
  char *data;
  const char *path;
  size_t size;
} File, d_File;

d_File *d_file_read(const char *path);
void d_file_destroy(d_File *file);
void d_file_edit(d_File *file, const char *data);
void d_file_save(d_File *file);

#pragma endregion

#pragma region Utilities

/**
 * @brief Finds the first occurrence of target substring in the given string.
 *
 * @param str string to search.
 * @param target target substring to find.
 * @param index_offset offset to add to the returned index. `0` if finding from
 * the start of the string.
 * @return The position of the first character of `target` in the string, and
 * `-1` and no match was found.
 * @throws DUCKY_NULL_REFERENCE if `str` or `target` is NULL.
 * @throws DUCKY_EMPTY_REFERENCE if `str` or `target` is an empty string.
 *
 */
int d_str_find(const char *str, const char *target, unsigned int index_offset);

char *d_str_replace(char *str, const char *target, const char *replacement);

char *d_str_append(const char *destination, const char *target);

#pragma endregion

#endif

#ifdef DUCKY_CORE_IMPL

#pragma region Error Handling
const d_Error DUCKY_SUCCESS = {0, "DUCKY_SUCCESS"};
const d_Error DUCKY_FAILURE = {10, "DUCKY_FAILURE"};
const d_Error DUCKY_WARNING = {11, "DUCKY_WARNING"};
const d_Error DUCKY_NULL_REFERENCE = {12, "DUCKY_NULL_REFERENCE"};
const d_Error DUCKY_EMPTY_REFERENCE = {13, "DUCKY_EMPTY_REFERENCE"};
const d_Error DUCKY_INDEX_OUT_OF_BOUNDS = {14, "DUCKY_INDEX_OUT_OF_BOUNDS"};
const d_Error DUCKY_MEMORY_FAILURE = {20, "DUCKY_MEMORY_FAILURE"};
const d_Error DUCKY_SDL_INIT_FAILURE = {70, "DUCKY_SDL_INIT_FAILURE"};
const d_Error DUCKY_WINDOW_CREATION_FAILURE = {71,
                                               "DUCKY_WINDOW_CREATION_FAILURE"};
const d_Error DUCKY_SHADER_COMPILE_FAILURE = {81,
                                              "DUCKY_SHADER_COMPILE_FAILURE"};
const d_Error DUCKY_SHADER_PROGRAM_LINK_FAILURE = {
    82, "DUCKY_SHADER_PROGRAM_LINK_FAILURE"};
const d_Error DUCKY_CRITICAL = {90, "DUCKY_CRITICAL"};

d_FullError *d_last_error = NULL;

void d_throw_error_internal(const d_Error *error, const char *message,
                            const char *file, const char *function) {
  d_last_error->error = error;
  d_last_error->message = message;
  d_last_error->file = file;
  d_last_error->function = function;
  d_event_invoke(d_event_system_get_event(d_event_system, "on_throw_error"));

#ifdef DUCKY_CORE_PRINT_ERRORS
  printf("[%d]: (%s, %s) %s\n", error->code, file, function, message);
#endif

  if (error->code >= DUCKY_CRITICAL.code && error->code < 100) {
    exit(DUCKY_CRITICAL.code);
  }
}

#pragma endregion

#pragma region Dynamic Array

d_Array *d_array_create_internal(size_t element_size, size_t initial_capacity) {
  d_Array *array = malloc(sizeof(d_Array));
  if (array == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to allocate memory for array.");
    return NULL;
  }

  array->data = malloc(element_size * initial_capacity);
  if (array->data == NULL) {
    free(array);
    d_throw_error(DUCKY_MEMORY_FAILURE,
                  "Failed to allocate memory for array data.");
    return NULL;
  }

  array->element_size = element_size;
  array->length = 0;
  array->capacity = initial_capacity;

  return array;
}

void d_array_add(d_Array *array, void *element) {
  if (array == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "array is NULL.");
    return;
  }

  if (array->length >= array->capacity) {
    size_t new_capacity = array->capacity * 2;
    void *new_data = realloc(array->data, array->element_size * new_capacity);
    if (new_data == NULL) {
      d_throw_error(DUCKY_MEMORY_FAILURE,
                    "Failed to reallocate memory for array data.");
      return;
    }
    array->data = new_data;
    array->capacity = new_capacity;
  }

  memcpy((char *)array->data + (array->length * array->element_size), element,
         array->element_size);
  array->length++;
}

void *d_array_get_internal(d_Array *array, unsigned int index) {
  if (array == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "array is NULL.");
    return NULL;
  }

  if (index >= array->length) {
    d_throw_error(DUCKY_FAILURE, "Index out of bounds.");
    return NULL;
  }

  return (char *)array->data + (index * array->element_size);
}

void d_array_destroy(d_Array *array) {
  if (array != NULL) {
    if (array->data != NULL) {
      free(array->data);
    }
    free(array);
  } else {
    d_throw_error(DUCKY_NULL_REFERENCE, "array is NULL.");
  }
}

#pragma endregion

#pragma region Event System
d_EventSystem *d_event_system = NULL;

d_Event *d_event_create(const char *name) {
  d_Event *event = malloc(sizeof(d_Event));
  if (event == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to allocate memory for event.");
    return NULL;
  }

  event->name = name;
  event->listeners = d_array_create(d_EventListener, 1);
  if (event->listeners == NULL) {
    free(event);
    return NULL;
  }

  return event;
}

void d_event_destroy(d_Event *event) {
  if (event != NULL) {
    d_array_destroy(event->listeners);
    free(event);
  } else {
    d_throw_error(DUCKY_NULL_REFERENCE, "event is NULL.");
  }
}

void d_event_add_listener(d_Event *event, d_EventListener listener) {
  if (event == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "event is NULL.");
    return;
  }

  if (listener == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "listener is NULL.");
    return;
  } else {
    d_array_add(event->listeners, &listener);
  }
}

void d_event_invoke(d_Event *event) {
  if (event == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "event is NULL.");
    return;
  }

  for (size_t i = 0; i < event->listeners->length; i++) {
    d_EventListener listener =
        d_array_get(event->listeners, d_EventListener, i);
    if (listener != NULL) {
      listener();
    }
  }
}

d_EventSystem *d_event_system_create() {
  d_EventSystem *event_system = malloc(sizeof(d_EventSystem));
  if (event_system == NULL) {
    d_throw_error(DUCKY_CRITICAL,
                  "Failed to allocate memory for event system.");
    return NULL;
  }

  event_system->events = d_array_create(d_Event, 1);
  if (event_system->events == NULL) {
    free(event_system);
    return NULL;
  }

  return event_system;
}

void d_event_system_destroy(d_EventSystem *event_system) {
  if (event_system != NULL) {
    for (size_t i = 0; i < event_system->events->length; i++) {
      d_Event *event = d_array_get(event_system->events, d_Event *, i);
      d_event_destroy(event);
    }
    d_array_destroy(event_system->events);
    free(event_system);
  } else {
    d_throw_error(DUCKY_NULL_REFERENCE, "event_system is NULL.");
  }
}

d_Event *d_event_system_get_event(d_EventSystem *event_system,
                                  const char *name) {
  if (event_system == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "event_system is NULL.");
    return NULL;
  }

  for (size_t i = 0; i < event_system->events->length; i++) {
    d_Event *event = d_array_get(event_system->events, d_Event *, i);
    if (strcmp(event->name, name) == 0) {
      return event;
    }
  }

  return NULL;
}

void d_event_system_add_event(d_EventSystem *event_system, const char *name) {
  if (event_system == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "event_system is NULL.");
    return;
  }

  d_Event *existing_event = d_event_system_get_event(event_system, name);
  if (existing_event != NULL) {
    d_throw_error(DUCKY_WARNING, "Event with the same name already exists.");
    return;
  }

  d_Event *new_event = d_event_create(name);
  if (new_event == NULL) {
    d_throw_error(DUCKY_FAILURE, "Failed to create event.");
    return;
  }

  d_array_add(event_system->events, &new_event);
}

#pragma endregion

#pragma region Core
void d_core_init() {
  d_last_error = malloc(sizeof(d_FullError));
  if (d_last_error == NULL) {
    // throw critical with no window
  }

  d_event_system = d_event_system_create();
  if (d_event_system == NULL) {
    d_throw_error(DUCKY_CRITICAL, "Failed to create event system.");
  }

  d_event_system_add_event(d_event_system, "on_throw_error");
}

void d_core_shutdown() {
  d_event_system_destroy(d_event_system);
  free(d_last_error);
}
#pragma endregion

#pragma region File

d_File *d_file_read(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    d_throw_error(DUCKY_FAILURE, "Failed to open file.");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = malloc(length + 1);
  if (buffer == NULL) {
    fclose(file);
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to allocate memory for file.");
    return NULL;
  }

  fread(buffer, length, 1, file);
  fclose(file);

  buffer[length] = 0;

  d_File *d_file = malloc(sizeof(d_File));
  if (d_file == NULL) {
    free(buffer);
    d_throw_error(DUCKY_MEMORY_FAILURE,
                  "Failed to allocate memory for d_File.");
    return NULL;
  }

  d_file->data = buffer;

  return d_file;
}

void d_file_destroy(d_File *file) {
  if (file != NULL) {
    if (file->data != NULL) {
      free(file->data);
    }
    free(file);
  } else {
    d_throw_error(DUCKY_NULL_REFERENCE, "file is NULL.");
  }
}

void d_file_edit(d_File *file, const char *data) {
  if (file == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "file is NULL.");
    return;
  }

  size_t data_length = strlen(data);
  file->data = realloc(file->data, data_length + 1);
  if (file->data == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE,
                  "Failed to allocate memory for file data.");
    return;
  }

  strcpy(file->data, data);
  file->size = data_length;
}

#pragma endregion

#pragma region Utilities

// using an int to count how many characters match the target, and when the
// amount is equal to the length of the target, break.
int d_str_find(const char *str, const char *target, unsigned int index_offset) {
  if (str == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "str is NULL.");
    return -1;
  }

  if (str[0] == '\0') {
    d_throw_error(DUCKY_EMPTY_REFERENCE, "str is empty.");
    return -1;
  }

  if (target == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "target is NULL.");
    return -1;
  }

  if (target[0] == '\0') {
    d_throw_error(DUCKY_EMPTY_REFERENCE, "target is empty.");
    return -1;
  }

  size_t str_length = strlen(str);
  size_t target_length = strlen(target);

  if (index_offset >= str_length) {
    d_throw_error(DUCKY_INDEX_OUT_OF_BOUNDS, "index_offset is out of bounds.");
    return -1;
  }

  bool matches = false;
  int match_position = -1;

  for (int i = index_offset; i < str_length; i++) {
    if (str[i] == target[0]) {
      for (int j = 0; j < target_length; j++) {
        if (i + j > str_length) {
          matches = false;
          break;
        }
        if (str[i + j] == target[j]) {
          matches = true;
        } else {
          matches = false;
          break;
        }
      }
      if (matches) {
        match_position = i;
        break;
      } else {
        match_position = -1;
      }
    }
  }

  return match_position;
}

char *d_str_replace(char *str, const char *target, const char *replacement) {
  if (str == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "str is NULL");
    return NULL;
  }

  if (target == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "target is NULL");
    return NULL;
  }

  if (replacement == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "replacement is NULL");
    return NULL;
  }

  size_t str_length = strlen(str);
  size_t target_length = strlen(target);
  size_t replacement_length = strlen(replacement);

  size_t new_length = str_length - target_length + replacement_length;

  int target_start = d_str_find(str, target, 0);
  if (target_start == -1) {
    return NULL;
  }

  char *new_str = malloc((new_length + 1) * sizeof(char));
  if (new_str == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to malloc new_str.");
    return NULL;
  }

  strcpy(new_str, str);

  for (int i = target_start; i < new_length; i++) {
    if (i >= target_start + replacement_length) {
      if (i - (replacement_length - target_length) > str_length) {
        d_throw_error(DUCKY_INDEX_OUT_OF_BOUNDS, "");
      }
      new_str[i] = str[i - (replacement_length - target_length)];
      continue;
    }

    new_str[i] = replacement[i - target_start];
  }

  new_str[new_length] = '\0';
  return new_str;
}

char *d_str_append(const char *destination, const char *target) {
  if (destination == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "destination is NULL.");
    return NULL;
  }

  if (target == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "target is NULL.");
    return NULL;
  }

  size_t destination_length = strlen(destination);
  size_t target_length = strlen(target);
  size_t new_length = destination_length + target_length;

  char *new_str = malloc((new_length + 1) * sizeof(char));
  if (new_str == NULL) {
    d_throw_error(DUCKY_MEMORY_FAILURE, "Failed to malloc new_str.");
    return NULL;
  }

  strcpy(new_str, destination);

  for (int i = destination_length; i < new_length; i++) {
    new_str[i] = target[i - destination_length];
  }

  new_str[new_length] = '\0';
  return new_str;
}

#pragma endregion

#endif
