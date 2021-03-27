#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

#include "GLFW/glfw3.h"

struct window {
	GLFWwindow *glfw_window;
	unsigned int width;
	unsigned int height;
	double start_time;
	int samples;
};

/**
 * @brief Mark a window for use.
 *
 * The window must be marked for use before OpenGL commands are issued.
 *
 * @remark Every use must be paired with an unuse once the commands are done.
 * @see window_unuse().
 */
void window_use(const struct window *window);
void window_unuse(const struct window *window);

/**
 * @brief Creates a new window.
 *
 * @param[in] width The width of the window.
 * @param[in] height The height of the window.
 * @param[in] title The title of the window.
 * @param[in] fullscreen Whether or not to be in fullscreen mode.
 *
 * @remark UTF-8 titles are supported.
 * @remark Widths and heights of 0 defaults to the primary monitor's current resolution.
 * @remark [Thread safety] This function must only be called from the main thread.
 *
 * @return A pointer to the newly created window or NULL on error.
 */
struct window *window_create(unsigned int width, unsigned int height, const char *title, bool fullscreen);

/**
 * @brief Destroys a window.
 *
 * @param[in] window A pointer to the window to destroy.
 *
 * @remark [Thread safety] This function must only be called from the main thread.
 * @remark [Reentrancy] This function must not be called from a callback.
 */
void window_destroy(struct window *window);

/**
 * @brief Closes a window.
 *
 * @param[in] window A pointer to the window to close.
 *
 * @remark Nothing happens if the window was already closed.
 * @remark Use in combination with window_closed().
 */
void window_close(const struct window *window);

/**
 * @brief A way to check whether a window has been closed.
 *
 * @param[in] window A pointer to the window.
 *
 * @remark Use in combination with window_close().
 *
 * @return true if the window has been closed or false otherwise.
 */
bool window_closed(const struct window *window);

/**
 * @brief Poll for input events.
 *
 * The events are handled by callbacks that must have been previously configured.
 * In case they weren't configured, dummy callbacks are used as a fallback to discard the events.
 *
 * @param[in] window A pointer to the window.
 */
void window_poll_events(const struct window *window);

/**
 * @brief Obtain the time elapsed since the window was created.
 *
 * @param[in] window A pointer to the window.
 *
 * @remark The highest resolution monotonic time source available on the platform is used.
 *         It should be in the order of a few micro or nano seconds.
 *
 * @return The time elapsed, in seconds.
 */
double window_elapsed(const struct window *window);

/**
 * @brief Obtain the size of the window's framebuffer.
 *
 * It's possible for window's framebuffers to have a larger width/height than the window itself.
 * This is common on high pixel density displays (mac retina).
 *
 * @param[in] window A pointer to the window.
 * @param[out] width The width of the window's framebuffer.
 * @param[out] height The height of the window's framebuffer.
 */
void window_framebuffer_size(const struct window *window, unsigned int *width, unsigned int *height);

#endif
