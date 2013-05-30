
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifndef EGL_EXT_swap_buffers_with_damage
#define EGL_EXT_swap_buffers_with_damage 1
typedef EGLBoolean (EGLAPIENTRYP PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC)(EGLDisplay dpy, EGLSurface surface, EGLint *rects, EGLint n_rects);
#endif

struct pilot_egldisplay{
	EGLDisplay dpy;
	EGLContext ctx;
	EGLConfig conf;
	PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC swap_buffers_with_damage;
};

struct pilot_eglcanvas *
pilot_eglcanvas_create(struct pilot_window *window)
{
	struct pilot_eglcanvas *canvas;
	struct pilot_egldisplay *egldisplay;

	canvas = malloc(sizeof *canvas);
	memset(canvas, 0, sizeof(*canvas));
	canvas->window = window;
	if (window) {
		if (!window->display->egl)
			window->display->egl = pilot_egldisplay_create(window->display);
		if (!window->display->egl) {
			free(canvas);
			return NULL;
		}
	}

	egldisplay = window->display->egl;

	canvas->native =
		wl_egl_window_create(window->surface,
				     window->width, window->height);
	canvas->egl_surface =
		eglCreateWindowSurface(egldisplay->dpy,
				       egldisplay->conf,
				       canvas->native, NULL);

	ret = eglMakeCurrent(egldisplay->dpy, canvas->egl_surface,
			     canvas->egl_surface, egldisplay->ctx);
	if (ret != EGL_TRUE) {
		pilot_eglcanvas_destroy(canvas);
		return NULL;
	}

	return canvas;
}

void
pilot_eglcanvas_destroy(struct pilot_eglcanvas *canvas)
{
	struct pilot_egldisplay *egldisplay = canvas->window->display->egl;

	eglMakeCurrent(egldisplay->dpy, EGL_NO_SURFACE, EGL_NO_SURFACE,
		       EGL_NO_CONTEXT);

	eglDestroySurface(egldisplay->dpy, canvas->egl_surface);
	wl_egl_window_destroy(canvas->native);

	if (egldisplay)
		pilot_egldisplay_destroy(egldisplay);
	free(canvas);
}

int
pilot_eglcanvas_set_draw_handler(struct pilot_eglcanvas *canvas, f_draw_handler handler, void *data)
{
	
	return 0;
}

int
pilot_eglcanvas_resize(struct pilot_eglcanvas *canvas, int32_t width, int32_t height)
{
	wl_egl_window_resize(canvas->native, width, height, 0, 0);
}

static GLuint
_pilot_eglcanvas_createshader(struct pilot_eglcanvas *canvas, const char *source, GLenum shader_type)
{
	GLuint shader;
	GLint status;

	shader = glCreateShader(shader_type);
	if (!shader)
		return -1;

	glShaderSource(shader, 1, (const char **) &source, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		char log[1000];
		GLsizei len;
		glGetShaderInfoLog(shader, 1000, &len, log);
		fprintf(stderr, "Error: compiling %s: %*s\n",
			shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment",
			len, log);
		exit(1);
	}

	return shader;
}

/**
 * pilot_egldisplay
 **/
struct pilot_egldisplay *
pilot_egldisplay_create(struct pilot_display *display)
{
	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	const char *extensions;

	EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 1,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLint major, minor, n;
	EGLBoolean ret;

	struct pilot_egldisplay *egldisplay;

	egldisplay = malloc(sizeof *egldisplay);
	memset(egldisplay, 0, sizeof(*egldisplay));

	if (opaque)
		config_attribs[9] = 0;

	egldisplay->dpy = eglGetDisplay(display->display);
	if (!egldisplay->dpy) {
		free(egldisplay);
		return NULL;
	}

	ret = eglInitialize(egldisplay->dpy, &major, &minor);
	if (ret != EGL_TRUE) {
		free(egldisplay);
		return NULL;
	}
	ret = eglBindAPI(EGL_OPENGL_ES_API);
	if (ret != EGL_TRUE) {
		free(egldisplay);
		return NULL;
	}

	ret = eglChooseConfig(egldisplay->dpy, config_attribs,
			      &egldisplay->conf, 1, &n);
	if (ret != EGL_TRUE && n != 1) {
		free(egldisplay);
		return NULL;
	}

	egldisplay->ctx = eglCreateContext(egldisplay->dpy,
					    egldisplay->conf,
					    EGL_NO_CONTEXT, context_attribs);
	if (!egldisplay->ctx) {
		free(egldisplay);
		return NULL;
	}

	egldisplay->swap_buffers_with_damage = NULL;
	extensions = eglQueryString(egldisplay->dpy, EGL_EXTENSIONS);
	if (extensions &&
	    strstr(extensions, "EGL_EXT_swap_buffers_with_damage") &&
	    strstr(extensions, "EGL_EXT_buffer_age"))
		egldisplay->swap_buffers_with_damage =
			(PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC)
			eglGetProcAddress("eglSwapBuffersWithDamageEXT");

	if (egldisplay->swap_buffers_with_damage)
		printf("has EGL_EXT_buffer_age and EGL_EXT_swap_buffers_with_damage\n");

	return egldisplay;
}

void
pilot_egldisplay_destroy(struct pilot_egldisplay *egldisplay)
{
	eglTerminate(egldisplay->dpy);
	eglReleaseThread();
}
