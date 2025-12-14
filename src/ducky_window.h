#ifndef DUCKY_WINDOW_H
#define DUCKY_WINDOW_H

#ifndef DUCKY_CORE_H
#include "ducky_core.h"
#endif

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DUCKY_GLAD_IMPL
#include DUCKY_GLAD_IMPL
#endif

#pragma region Types

typedef enum d_ViewportState {
  DUCKY_LETTERBOXED,
  DUCKY_PILLARBOXED
} ViewportState,
    d_ViewportState;

typedef struct d_Viewport {
  int target_aspect_w;
  int target_aspect_h;

  int viewport_x;
  int viewport_y;
  int viewport_w;
  int viewport_h;

  d_ViewportState state;
} Viewport, d_Viewport;

typedef struct d_Window {
  const char *title;
  int width;
  int height;
  bool resizable;
  bool fullscreen;
  void *native_window;
  void *gl_context;
  d_Viewport *viewport;
  bool running;
} Window, d_Window;
#pragma endregion

#pragma region Viewport Functions
d_Viewport *d_viewport_create(const int target_aspect_w,
                              const int target_aspect_h);
void d_viewport_destroy(d_Viewport *viewport);
#pragma endregion

#pragma region Window Functions

/*
  Create a window with the specified parameters.
  If `title` is NULL, a default value will be used.
  #### Parameters:
  - `title`: The title of the window. (default: `"Ducky Window"`)
  - `width`: The width of the window.
  - `height`: The height of the window.
  - `resizable`: Whether the window is resizable.
  - `fullscreen`: Whether the window is fullscreen.
  #### Throws:
  - `DUCKY_CRITICAL`: If SDL or GLAD fails to initialize, create the window, or
  create the OpenGL context.
*/
d_Window *d_window_create(const char *title, const int width, const int height,
                          const bool resizable, const bool fullscreen);
/*
  Destroy the specified window and free its resources.
  #### Parameters:
  - `window`: The window to destroy.
  #### Throws:
  - `DUCKY_NULL_REFERENCE`: If the `window` argument is NULL.
*/
void d_window_destroy(d_Window *window);
/*
  Update the specified window, processing events.
  #### Parameters:
  - `window`: The window to update.
  #### Throws:
  - `DUCKY_NULL_REFERENCE`: If the `window` argument is NULL.
*/
void d_window_update(d_Window *window);
/*
  Get the current dimensions of the specified window.
  #### Parameters:
  - `window`: The window to get the dimensions of.
  - `width`: Pointer to an integer to store the width.
  - `height`: Pointer to an integer to store the height.
  #### Throws:
  - `DUCKY_NULL_REFERENCE`: If the `window` argument is NULL.
*/
void d_window_get_dimensions(d_Window *window, int *width, int *height);

void d_window_swap_buffers(d_Window *window);
#pragma endregion

#endif

#ifdef DUCKY_WINDOW_IMPL
d_Viewport *d_viewport_create(const int target_aspect_w,
                              const int target_aspect_h) {
  d_Viewport *viewport = (d_Viewport *)malloc(sizeof(d_Viewport));
  viewport->target_aspect_w = target_aspect_w;
  viewport->target_aspect_h = target_aspect_h;
  return viewport;
}

void d_viewport_destroy(d_Viewport *viewport) {
  if (viewport != NULL) {
    free(viewport);
  } else {
    d_throw_error(DUCKY_NULL_REFERENCE,
                  "(d_viewport_destroy) viewport is NULL.");
  }
}

d_Window *d_window_create(const char *title, const int width, const int height,
                          const bool resizable, const bool fullscreen) {

  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    char message[256] = "Failed to initialize SDL: ";
    strcat(message, SDL_GetError());
    d_throw_error(DUCKY_CRITICAL, message);
    return NULL;
  }

  SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL;
  if (resizable) {
    window_flags |= SDL_WINDOW_RESIZABLE;
  }
  if (fullscreen) {
    window_flags |= SDL_WINDOW_FULLSCREEN;
  }

  SDL_Window *sdl_window = SDL_CreateWindow(title, width, height, window_flags);

  if (!sdl_window) {
    char message[256] = "Failed to create SDL window: ";
    strcat(message, SDL_GetError());
    SDL_Quit();
    d_throw_error(DUCKY_CRITICAL, message);
    return NULL;
  }
  SDL_ShowWindow(sdl_window);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

  SDL_GLContext gl_context = SDL_GL_CreateContext(sdl_window);
  if (!gl_context) {
    char message[256] = "Failed to create OpenGL context: ";
    strcat(message, SDL_GetError());
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

    d_throw_error(DUCKY_CRITICAL, message);

    return NULL;
  }

#ifdef DUCKY_GLAD_IMPL
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
    d_throw_error(DUCKY_CRITICAL, "Failed to initialize GLAD");
    SDL_GL_DestroyContext(gl_context);

    return NULL;
  }
#endif

  d_Window *window = (d_Window *)malloc(sizeof(d_Window));
  window->title = title;
  window->width = width;
  window->height = height;
  window->resizable = resizable;
  window->fullscreen = fullscreen;
  window->native_window = (void *)sdl_window;
  window->gl_context = (void *)gl_context;
  window->viewport = d_viewport_create(1920, 1080);
  window->running = true;

  return window;
}

void d_window_destroy(d_Window *window) {
  if (window == NULL) {
    return;
  }

  SDL_DestroyWindow((SDL_Window *)window->native_window);
  SDL_GL_DestroyContext((SDL_GLContext)window->gl_context);
  d_viewport_destroy(window->viewport);
  SDL_Quit();
}

void d_window_update(d_Window *window) {
  if (window == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE, "(d_window_update) window is NULL.");
    return;
  }

  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT)
      window->running = false;
  }

  d_window_get_dimensions(window, &window->width, &window->height);

  float window_aspect = (float)window->width / window->height;
  float game_aspect = (float)window->viewport->target_aspect_w /
                      window->viewport->target_aspect_h;

  if (window_aspect > game_aspect) {
    window->viewport->viewport_w = (int)(window->height * game_aspect);
    window->viewport->viewport_h = window->height;

    window->viewport->viewport_x =
        (window->width - window->viewport->viewport_w) / 2;
    window->viewport->viewport_y = 0;
    window->viewport->state = DUCKY_PILLARBOXED;
  } else {
    window->viewport->viewport_w = window->width;
    window->viewport->viewport_h = (int)(window->width / game_aspect);

    window->viewport->viewport_x = 0;
    window->viewport->viewport_y =
        (window->height - window->viewport->viewport_h) / 2;
    window->viewport->state = DUCKY_LETTERBOXED;
  }

  glViewport(window->viewport->viewport_x, window->viewport->viewport_y,
             window->viewport->viewport_w, window->viewport->viewport_h);
}

void d_window_get_dimensions(d_Window *window, int *width, int *height) {
  if (window == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE,
                  "(d_window_get_dimensions) window is NULL.");
    return;
  }

  SDL_GetWindowSize((SDL_Window *)window->native_window, width, height);
}

void d_window_swap_buffers(d_Window *window) {
  if (window == NULL) {
    d_throw_error(DUCKY_NULL_REFERENCE,
                  "(d_window_swap_buffers) window is NULL.");
    return;
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SDL_GL_SwapWindow((SDL_Window *)window->native_window);
}
#endif
