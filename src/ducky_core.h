#ifndef DUCKY_CORE_H
#define DUCKY_CORE_H

#include <stdlib.h>
#ifdef DUCKY_CORE_PRINT_ERRORS
#include <stdio.h>
#endif

#pragma region Error Handling

typedef enum d_ErrorCode {
  DUCKY_UNKNOWN = -1,
  DUCKY_SUCCESS = 0,
  DUCKY_FAILURE = 1,
  DUCKY_CRITICAL = 2,
  DUCKY_NULL_REFERENCE = 3,
} ErrorCode,
    d_ErrorCode;

typedef struct d_Error {
  d_ErrorCode code;
  const char *message;
} Error, d_Error;

extern d_Error current_error;

void d_throw_error(d_ErrorCode code, const char *message);
d_Error *d_get_error();

#pragma endregion

#endif

#ifdef DUCKY_CORE_IMPL

d_Error current_error = {(d_ErrorCode)-1, ""};

void d_throw_error(d_ErrorCode code, const char *message) {
#ifdef DUCKY_CORE_PRINT_ERRORS
  printf("[%d]: %s\n", code, message);
#endif
  current_error.code = code;
  current_error.message = message;

  if (code == DUCKY_CRITICAL) {
    exit(DUCKY_CRITICAL);
  }
}

d_Error *d_get_error() { return &current_error; }

#endif
