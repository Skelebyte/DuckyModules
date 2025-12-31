#ifndef DUCKY_CORE_H
#define DUCKY_CORE_H

#include <stdlib.h>
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
const d_Error DUCKY_NULL_REFERENCE;
const d_Error DUCKY_MALLOC_FAILURE;
const d_Error DUCKY_SDL_INIT_FAILURE;
const d_Error DUCKY_WINDOW_CREATION_FAILURE;
#pragma endregion

void d_throw_error(d_Error error, const char *message, const char *file,
                   const char *function);

#pragma endregion

#endif

#ifdef DUCKY_CORE_IMPL
const d_Error DUCKY_SUCCESS = {0, "DUCKY_SUCCESS"};
const d_Error DUCKY_FAILURE = {10, "DUCKY_FAILURE"};
const d_Error DUCKY_NULL_REFERENCE = {11, "DUCKY_NULL_REFERENCE"};
const d_Error DUCKY_MALLOC_FAILURE = {20, "DUCKY_MALLOC_FAILURE"};
const d_Error DUCKY_SDL_INIT_FAILURE = {70, "DUCKY_SDL_INIT_FAILURE"};
const d_Error DUCKY_WINDOW_CREATION_FAILURE = {71,
                                               "DUCKY_WINDOW_CREATION_FAILURE"};

const d_Error DUCKY_CRITICAL = {90, "DUCKY_CRITICAL"};

void d_throw_error(d_Error error, const char *message, const char *file,
                   const char *function) {
  printf("[%d]: (%s, %s) %s\n", error, file, function, message);

  if (error.code == DUCKY_CRITICAL.code) {
    exit(DUCKY_CRITICAL.code);
  }
}

#endif
