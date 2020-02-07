#include "toolkit.h"
#include "camera.h"
#include "renderer.h"
#include "direction.h"
#include "entity.h"

#include <SDL.h>

// TODO: Switch to SDL2 instead of GLFW?
// Yes. GLFW uses callbacks, impossible to poll for mouse wheel, callbacks aren't unique per window and dont allow
// custom parameters.

struct window {
    SDL_Window *sdl_window;
    SDL_GLContext sdl_gl_context;
    SDL_Event sdl_event;
    uint64_t previous_frame_time;
    bool should_close;
};

atomic_uint window_refcount;

void window_refcount_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&window_refcount, 1);

    if (previous_count == 0) {
        SDL_Init(SDL_INIT_VIDEO);
    }
}

void window_refcount_decrement(void) {
    unsigned int previous_count = atomic_fetch_sub(&window_refcount, 1);

    if (previous_count == 1) {
        SDL_Quit();
    }
}

struct window *window_create(int width, int height, const char *title) {
    struct window *window = malloc(sizeof *window);
    if (!window) {
        return NULL;
    }

    window_refcount_increment();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetSwapInterval(1);

    window->sdl_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (!window->sdl_window) {
        free(window);
        window_refcount_decrement();
        return NULL;
    }

    window->sdl_gl_context = SDL_GL_CreateContext(window->sdl_window);
    if (!window->sdl_gl_context) {
        SDL_DestroyWindow(window->sdl_window);
        window_refcount_decrement();
        return NULL;
    }

    window->should_close = false;
    window->previous_frame_time = SDL_GetPerformanceCounter();

    return window;
}

void window_use(struct window *window) {
    SDL_GL_MakeCurrent(window->sdl_window, window->sdl_gl_context);
}

void window_unuse(struct window *window) {
    TK_UNUSED(window);
}

void window_destroy(struct window *window) {
    SDL_GL_DeleteContext(window->sdl_gl_context);
    SDL_DestroyWindow(window->sdl_window);
    free(window);
    window_refcount_decrement();
}

tk_result window_poll_event(struct window *window) {
    int r = SDL_PollEvent(&window->sdl_event);

    if (r == 1) {
        if (window->sdl_event.type == SDL_QUIT) {
            window->should_close = true;
        }

        return TK_RESULT_SUCCESS;
    } else {
        return TK_RESULT_FAILURE;
    }
}

bool window_event_key_pressed(struct window *window, char key) {
    return window->sdl_event.type == SDL_KEYDOWN && !window->sdl_event.key.repeat && window->sdl_event.key.keysym.sym == key;
}

bool window_event_key_released(struct window *window, char key) {
    return window->sdl_event.type == SDL_KEYUP && !window->sdl_event.key.repeat && window->sdl_event.key.keysym.sym == key;
}

bool window_event_mouse_button_pressed(struct window *window, int button) {
    return window->sdl_event.type == SDL_MOUSEBUTTONDOWN && button == window->sdl_event.button.button;
}

bool window_event_mouse_button_released(struct window *window, int button) {
    return window->sdl_event.type == SDL_MOUSEBUTTONUP && button == window->sdl_event.button.button;
}

bool window_event_mouse_motion_relative(struct window *window, int *delta_x, int *delta_y) {
    if (window->sdl_event.type == SDL_MOUSEMOTION) {
        *delta_x = window->sdl_event.motion.xrel;
        *delta_y = window->sdl_event.motion.yrel;
        return true;
    }

    return false;
}

bool window_event_mouse_wheel(struct window *window, int *delta_x, int *delta_y) {
    if (window->sdl_event.type == SDL_MOUSEWHEEL) {
        *delta_x = window->sdl_event.wheel.x;
        *delta_y = window->sdl_event.wheel.y;
        return true;
    }

    return false;
}

float window_elapsed_seconds(struct window *window) {
    TK_UNUSED(window);

    uint64_t current_frame_time = SDL_GetPerformanceCounter();
    return (current_frame_time - window->previous_frame_time) / (float) SDL_GetPerformanceFrequency();
}

void window_framebuffer_size(struct window *window, int *width, int *height) {
    SDL_GL_GetDrawableSize(window->sdl_window, width, height);
}

void window_refresh(struct window *window) {
    window->previous_frame_time = SDL_GetPerformanceCounter();
    SDL_GL_SwapWindow(window->sdl_window);
}

bool window_should_close(struct window *window) {
    return window->should_close;
}

void window_close(struct window *window) {
    window->should_close = true;
}
