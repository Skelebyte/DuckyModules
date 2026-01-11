#ifndef DUCKY_CORE_H
#define DUCKY_CORE_H

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
const d_Error DUCKY_MEMORY_FAILURE;
const d_Error DUCKY_SDL_INIT_FAILURE;
const d_Error DUCKY_WINDOW_CREATION_FAILURE;
d_FullError *d_last_error;
#pragma endregion

void d_throw_error(const d_Error *error, const char *message, const char *file,
                   const char *function);

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

#endif

#ifdef DUCKY_CORE_IMPL

#pragma region Error Handling
const d_Error DUCKY_SUCCESS = {0, "DUCKY_SUCCESS"};
const d_Error DUCKY_FAILURE = {10, "DUCKY_FAILURE"};
const d_Error DUCKY_WARNING = {11, "DUCKY_WARNING"};
const d_Error DUCKY_NULL_REFERENCE = {12, "DUCKY_NULL_REFERENCE"};
const d_Error DUCKY_MEMORY_FAILURE = {20, "DUCKY_MEMORY_FAILURE"};
const d_Error DUCKY_SDL_INIT_FAILURE = {70, "DUCKY_SDL_INIT_FAILURE"};
const d_Error DUCKY_WINDOW_CREATION_FAILURE = {71,
                                               "DUCKY_WINDOW_CREATION_FAILURE"};
const d_Error DUCKY_CRITICAL = {90, "DUCKY_CRITICAL"};

d_FullError *d_last_error = NULL;

void d_throw_error(const d_Error *error, const char *message, const char *file,
                   const char *function) {
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
    d_throw_error(&DUCKY_MEMORY_FAILURE, "Failed to allocate memory for array.",
                  __FILE__, __FUNCTION__);
    return NULL;
  }

  array->data = malloc(element_size * initial_capacity);
  if (array->data == NULL) {
    free(array);
    d_throw_error(&DUCKY_MEMORY_FAILURE,
                  "Failed to allocate memory for array data.", __FILE__,
                  __FUNCTION__);
    return NULL;
  }

  array->element_size = element_size;
  array->length = 0;
  array->capacity = initial_capacity;

  return array;
}

void d_array_add(d_Array *array, void *element) {
  if (array == NULL) {
    d_throw_error(&DUCKY_NULL_REFERENCE, "array is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  if (array->length >= array->capacity) {
    size_t new_capacity = array->capacity * 2;
    void *new_data = realloc(array->data, array->element_size * new_capacity);
    if (new_data == NULL) {
      d_throw_error(&DUCKY_MEMORY_FAILURE,
                    "Failed to reallocate memory for array data.", __FILE__,
                    __FUNCTION__);
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
    d_throw_error(&DUCKY_NULL_REFERENCE, "array is NULL.", __FILE__,
                  __FUNCTION__);
    return NULL;
  }

  if (index >= array->length) {
    d_throw_error(&DUCKY_FAILURE, "Index out of bounds.", __FILE__,
                  __FUNCTION__);
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
    d_throw_error(&DUCKY_NULL_REFERENCE, "array is NULL.", __FILE__,
                  __FUNCTION__);
  }
}

#pragma endregion

#pragma region Event System
d_EventSystem *d_event_system = NULL;

d_Event *d_event_create(const char *name) {
  d_Event *event = malloc(sizeof(d_Event));
  if (event == NULL) {
    d_throw_error(&DUCKY_MEMORY_FAILURE, "Failed to allocate memory for event.",
                  __FILE__, __FUNCTION__);
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
    d_throw_error(&DUCKY_NULL_REFERENCE, "event is NULL.", __FILE__,
                  __FUNCTION__);
  }
}

void d_event_add_listener(d_Event *event, d_EventListener listener) {
  if (event == NULL) {
    d_throw_error(&DUCKY_NULL_REFERENCE, "event is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  if (listener == NULL) {
    d_throw_error(&DUCKY_NULL_REFERENCE, "listener is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  } else {
    d_array_add(event->listeners, &listener);
  }
}

void d_event_invoke(d_Event *event) {
  if (event == NULL) {
    d_throw_error(&DUCKY_NULL_REFERENCE, "event is NULL.", __FILE__,
                  __FUNCTION__);
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
    d_throw_error(&DUCKY_CRITICAL,
                  "Failed to allocate memory for event system.", __FILE__,
                  __FUNCTION__);
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
    d_throw_error(&DUCKY_NULL_REFERENCE, "event_system is NULL.", __FILE__,
                  __FUNCTION__);
  }
}

d_Event *d_event_system_get_event(d_EventSystem *event_system,
                                  const char *name) {
  if (event_system == NULL) {
    d_throw_error(&DUCKY_NULL_REFERENCE, "event_system is NULL.", __FILE__,
                  __FUNCTION__);
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
    d_throw_error(&DUCKY_NULL_REFERENCE, "event_system is NULL.", __FILE__,
                  __FUNCTION__);
    return;
  }

  d_Event *existing_event = d_event_system_get_event(event_system, name);
  if (existing_event != NULL) {
    d_throw_error(&DUCKY_WARNING, "Event with the same name already exists.",
                  __FILE__, __FUNCTION__);
    return;
  }

  d_Event *new_event = d_event_create(name);
  if (new_event == NULL) {
    d_throw_error(&DUCKY_FAILURE, "Failed to create event.", __FILE__,
                  __FUNCTION__);
    return;
  }

  d_array_add(event_system->events, &new_event);
}

#pragma endregion

#pragma region Core
void d_core_init() {
  d_event_system = d_event_system_create();
  if (d_event_system == NULL) {
    d_throw_error(&DUCKY_CRITICAL, "Failed to create event system.", __FILE__,
                  __FUNCTION__);
  }

  d_last_error = malloc(sizeof(d_FullError));

  d_event_system_add_event(d_event_system, "on_throw_error");
}

void d_core_shutdown() {
  d_event_system_destroy(d_event_system);
  free(d_last_error);
}
#pragma endregion

#endif
