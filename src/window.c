#include "GLFW/glfw3.h"
#include "cimgui.h"
#include "client.h"
#include "entity.h"
#include "utils.h"
#include "utils.h"
#include "window.h"
#include <stdbool.h>

static void apply_fallback_resolution(unsigned int *width, unsigned int *height) {
	GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
	if (!primary_monitor) {
		return;
	}

	const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);
	if (!video_mode) {
		return;
	}

	if (*width == 0) {
		*width = video_mode->width;
	}

	if (*height == 0) {
		*height = video_mode->height;
	}
}

static void recalculate_cursor_ray(void) {
	// Normalize the cursor position to the [-1, 1] range (normalized device space).
	double normalized_x = -1.0 + 2.0 * client.window.cursor_pos_x / (double) client.window.width;
	double normalized_y = 1.0 - 2.0 * client.window.cursor_pos_y / (double) client.window.height;
	// double normalized_y = -(1.0 - 2.0 * client.window.cursor_pos_y / (double) client.window.height);
	vec2 n = {normalized_x, normalized_y};

	// Generate the inverse view-projection matrix, to convert from normalized device space to world space.
	mat4 view_projection_matrix, view_projection_inverse;
	glm_mat4_mul(client.renderer.projection_matrix, client.camera.view_matrix, view_projection_matrix);
	glm_mat4_inv(view_projection_matrix, view_projection_inverse);

	vec4 ray_start, ray_end;
	glm_mat4_mulv(view_projection_inverse, (vec4) { n[0], n[1], 0.f, 1.f}, ray_start);
	glm_vec4_scale(ray_start, 1.f / ray_start[3], ray_start);
	glm_mat4_mulv(view_projection_inverse, (vec4) { n[0], n[1], 1.f, 1.f}, ray_end);
	glm_vec4_scale(ray_end, 1.f / ray_end[3], ray_end);

	glm_vec4_copy(ray_start, client.window.cursor_ray_origin);
	glm_vec4_sub(ray_end, ray_start, client.window.cursor_ray_direction);
	glm_normalize(client.window.cursor_ray_direction);

	/*
	   if (tracking) {
	        for (size_t i = 0; i < client.scene.entity_count; i++) {
	                struct entity *entity = client.scene.entities[i];
	                if (client.ui.selected_entity_id == entity->id) {
	                                        float distance = tmp;

	                                        vec3 position, direction_further;
	                                        glm_vec3_mul(direction, (vec3) { distance, distance, distance}, direction_further);
	                                        glm_vec3_copy(origin, position);
	                                        glm_vec3_add(position, direction_further, position);

	                                        // printf("-> %f %f %f\n", position[0], position[1], position[2]);
	                                        glm_vec3_copy(position, entity->translation);
	                }
	        }
	   }
	 */
}

static void scroll_callback(GLFWwindow *glfw_window, double xoffset, double yoffset) {
	UNUSED(glfw_window);
	UNUSED(xoffset);
	UNUSED(yoffset);

	client.camera.eye_distance += -yoffset * 0.2f;
	if (client.camera.eye_distance <= 0.00001) {
		client.camera.eye_distance = 0.00001;
	}

	camera_update(&client.camera);
}

static void cursor_pos_callback(GLFWwindow *glfw_window, double x, double y) {
	UNUSED(glfw_window);

	double delta_x = x - client.window.cursor_pos_x;
	double delta_y = y - client.window.cursor_pos_y;

	client.window.cursor_pos_x = x;
	client.window.cursor_pos_y = y;

	recalculate_cursor_ray();

	if (client.ui.ig_io->WantCaptureMouse) {
		return;
	}

	if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		client.camera.center_rotation += -delta_x * 0.005f;

		client.camera.eye_above += delta_y * 0.005f;
		if (client.camera.eye_above >= M_PI_2) {
			client.camera.eye_above = M_PI_2 - 0.00001f;
		}

		if (client.camera.eye_above < -M_PI_2) {
			client.camera.eye_above = -M_PI_2 + 0.00001f;
		}

		camera_update(&client.camera);
	}
}

static void mouse_button_callback(GLFWwindow *glfw_window, int button, int action, int mods) {
	UNUSED(glfw_window);
	UNUSED(mods);

	// Mouse picking, as long as the UI doesn't intercept it.
	if (button == GLFW_MOUSE_BUTTON_1 && !client.ui.ig_io->WantCaptureMouse) {
		static uint32_t saved = 0;

		if (action == GLFW_PRESS) {
			saved = client.renderer.mousepicking_entity_id;
		} else if (action == GLFW_RELEASE && saved == client.renderer.mousepicking_entity_id) {
			client.ui.selected_entity_id = saved;
		}
	}
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	UNUSED(window);
	UNUSED(scancode);
	UNUSED(mods);

	if (client.ui.ig_io->WantTextInput) {
		return;
	}

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		client.ui.show = !client.ui.show;

		/*
		   if (client.ui.show) {
		        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		   } else {
		        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		   }
		 */
	}

	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			client.moving |= FORWARD;
		} else if (action == GLFW_RELEASE) {
			client.moving &= ~FORWARD;
		}
	} else if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) {
			client.moving |= LEFT;
		} else if (action == GLFW_RELEASE) {
			client.moving &= ~LEFT;
		}
	} else if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS) {
			client.moving |= BACKWARD;
		} else if (action == GLFW_RELEASE) {
			client.moving &= ~BACKWARD;
		}
	} else if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS) {
			client.moving |= RIGHT;
		} else if (action == GLFW_RELEASE) {
			client.moving &= ~RIGHT;
		}
	}
}

static void window_size_callback(GLFWwindow *window, int width, int height) {
	UNUSED(window);

	client.window.width = width;
	client.window.height = height;
}

static void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
	UNUSED(window);

	client.renderer.viewport_width = width;
	client.renderer.viewport_height = height;
	glViewport(0, 0, width, height);
}

// TODO: Would be nice to use this functionality for something.
static void drop_callback(GLFWwindow *window, int nb, const char *paths[]) {
	UNUSED(window);

	for (int i = 0; i < nb; i++) {
		printf("--> %s\n", paths[i]);
	}
}

void window_fullscreen(struct window *window, bool fullscreen) {
	static int original_width, original_height, original_x, original_y;

	if (fullscreen) {
		glfwGetWindowSize(window->glfw_window, &original_width, &original_height);
		glfwGetWindowPos(window->glfw_window, &original_x, &original_y);

		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window->glfw_window, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
	} else {
		glfwSetWindowMonitor(window->glfw_window, NULL, original_x, original_y, original_width, original_height, 0);
	}

	window->fullscreen = fullscreen;
}

bool window_init(struct window *window, unsigned int width, unsigned int height, const char *title, bool fullscreen) {
	window->width = width;
	window->height = height;
	window->samples = 4; // TODO: Support changing the number of samples. This requires recreating the window and sharing the context.
	window->fullscreen = fullscreen;
	window->title = strdup(title);

	window->last_time = glfwGetTime();
	window->now_time = glfwGetTime();

	if (!window->title) {
		return false;
	}

	// Automatically initializes the GLFW library for the first window created.
	if (glfwInit() == GLFW_FALSE) {
		free(window->title);
		return false;
	}

	// Use the primary monitor's resolution when dimensions weren't specified.
	apply_fallback_resolution(&window->width, &window->height);

	// GLFW window hints.
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_SAMPLES, window->samples); // Multisampling.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Mac only supports OpenGL 3.2 or 4.1.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Modern rendering pipeline.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true); // Mac OS X requires forward compatibility.

	// Fullscreen mode always uses the primary monitor for now.
	GLFWmonitor *monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;

	// Create the actual window using the GLFW library.
	window->glfw_window = glfwCreateWindow(window->width, window->height, title, monitor, NULL);
	if (!window->glfw_window) {
		glfwTerminate();
		free(window->title);
		return false;
	}

	// Make the current thread use the new window's OpenGL context so that we can initialize OpenGL for it.
	glfwMakeContextCurrent(window->glfw_window);

	// Initialize OpenGL.
	if (!gladLoadGL()) {
		glfwDestroyWindow(window->glfw_window);
		glfwTerminate();
		free(window->title);
		return false;
	}

	// Minimum number of monitor refreshes the driver should wait after the call to glfwSwapBuffers before actually swapping the buffers on the display.
	// Essentially, 0 = V-Sync off, 1 = V-Sync on. Leaving this on avoids ugly tearing artifacts.
	// It requires the OpenGL context to be effective on Windows.
	glfwSwapInterval(1);

	// Initial cursor position.
	glfwGetCursorPos(window->glfw_window, &window->cursor_pos_x, &window->cursor_pos_y);

	// Configure callbacks.
	glfwSetCursorPosCallback(window->glfw_window, cursor_pos_callback);
	glfwSetFramebufferSizeCallback(window->glfw_window, framebuffer_resize_callback);
	glfwSetWindowSizeCallback(window->glfw_window, window_size_callback);
	glfwSetKeyCallback(window->glfw_window, key_callback);
	glfwSetMouseButtonCallback(window->glfw_window, mouse_button_callback);
	glfwSetScrollCallback(window->glfw_window, scroll_callback);
	glfwSetDropCallback(window->glfw_window, drop_callback);

	return true;
}

double window_elapsed(const struct window *window) {
	return window->now_time - window->last_time;
}

void window_fini(struct window *window) {
	free(window->title);
	glfwDestroyWindow(window->glfw_window);
	glfwTerminate();
}

void window_update_title(struct window *window, const char *title) {
	char *title_copy = strdup(title);
	if (!title_copy) {
		return;
	}

	free(window->title);
	window->title = title_copy;
	glfwSetWindowTitle(window->glfw_window, title_copy);
}

void window_close(struct window *window, int force) {
	glfwSetWindowShouldClose(window->glfw_window, 1);

	if (force) {
		glfwDestroyWindow(window->glfw_window);
		window->glfw_window = NULL;
	}
}

bool window_closed(const struct window *window) {
	if (window->glfw_window == NULL) {
		return true;
	}

	return glfwWindowShouldClose(window->glfw_window) == 1;
}

void window_poll_events(struct window *window) {
	UNUSED(window);
	glfwPollEvents();
	window->now_time = glfwGetTime();
}

void window_framebuffer_size(const struct window *window, int *width, int *height) {
	glfwGetFramebufferSize(window->glfw_window, width, height);
}

bool window_extension_supported(const char *name) {
	return glfwExtensionSupported(name) == GLFW_TRUE;
}

void window_refresh(struct window *window) {
	glfwSwapBuffers(window->glfw_window);
	window->last_time = window->now_time;
}
