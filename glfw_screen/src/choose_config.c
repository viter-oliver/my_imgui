#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include "choose_config.h"
struct {
	EGLint red_size;
	EGLint green_size;
	EGLint blue_size;
	EGLint alpha_size;
	EGLint samples;
	EGLint config_id;
} egl_conf_attr = {
	.red_size = EGL_DONT_CARE,
	.green_size = EGL_DONT_CARE,
	.blue_size = EGL_DONT_CARE,
	.alpha_size = EGL_DONT_CARE,
	.samples = EGL_DONT_CARE,
	.config_id = EGL_DONT_CARE,
};

EGLConfig choose_config(EGLDisplay egl_disp, const char* str)
{
	EGLConfig egl_conf = (EGLConfig)0;
	EGLConfig *egl_configs;
	EGLint egl_num_configs;
	EGLint val;
	EGLBoolean rc;
	const char *tok;
	EGLint i;

	if (str != NULL) {
		tok = str;
		while (*tok == ' ' || *tok == ',') {
			tok++;
		}

		while (*tok != '\0') {
			if (strncmp(tok, "rgba8888", strlen("rgba8888")) == 0) {
				egl_conf_attr.red_size = 8;
				egl_conf_attr.green_size = 8;
				egl_conf_attr.blue_size = 8;
				egl_conf_attr.alpha_size = 8;
				tok += strlen("rgba8888");
			} else if (strncmp(tok, "rgba5551", strlen("rgba5551")) == 0) {
				egl_conf_attr.red_size = 5;
				egl_conf_attr.green_size = 5;
				egl_conf_attr.blue_size = 5;
				egl_conf_attr.alpha_size = 1;
				tok += strlen("rgba5551");
			} else if (strncmp(tok, "rgba4444", strlen("rgba4444")) == 0) {
				egl_conf_attr.red_size = 4;
				egl_conf_attr.green_size = 4;
				egl_conf_attr.blue_size = 4;
				egl_conf_attr.alpha_size = 4;
				tok += strlen("rgba4444");
			} else if (strncmp(tok, "rgb565", strlen("rgb565")) == 0) {
				egl_conf_attr.red_size = 5;
				egl_conf_attr.green_size = 6;
				egl_conf_attr.blue_size = 5;
				egl_conf_attr.alpha_size = 0;
				tok += strlen("rgb565");
                	} else if (isdigit(*tok)) {
				val = atoi(tok);
				while (isdigit(*(++tok)));
				if (*tok == 'x') {
					egl_conf_attr.samples = val;
					tok++;
				} else {
					egl_conf_attr.config_id = val;
				}
			} else {
				fprintf(stderr, "invalid configuration specifier: ");
				while (*tok != ' ' && *tok != ',' && *tok != '\0') {
					fputc(*tok++, stderr);
				}
				fputc('\n', stderr);
			}

			while (*tok == ' ' || *tok == ',') {
				tok++;
			}
		}
	}

	rc = eglGetConfigs(egl_disp, NULL, 0, &egl_num_configs);
	if (rc != EGL_TRUE) {
		fprintf(stderr,"eglGetConfigs");
		return egl_conf;
	}
	if (egl_num_configs == 0) {
		fprintf(stderr, "eglGetConfigs: could not find a configuration\n");
		return egl_conf;
	}

	egl_configs = malloc(egl_num_configs * sizeof(*egl_configs));
	if (egl_configs == NULL) {
		fprintf(stderr, "could not allocate memory for %d EGL configs\n", egl_num_configs);
		return egl_conf;
	}

	rc = eglGetConfigs(egl_disp, egl_configs,
		egl_num_configs, &egl_num_configs);
	if (rc != EGL_TRUE) {
		free(egl_configs);
		return egl_conf;
	}

	for (i = 0; i < egl_num_configs; i++) {
		if (egl_conf_attr.config_id != EGL_DONT_CARE) {
			eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_CONFIG_ID, &val);
			if (val == egl_conf_attr.config_id) {
				egl_conf = egl_configs[i];
				break;
			} else {
				continue;
			}
		}

		eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_SURFACE_TYPE, &val);
		if (!(val & EGL_WINDOW_BIT)) {
			continue;
		}

		eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_RENDERABLE_TYPE, &val);
		if (!(val & EGL_OPENGL_ES2_BIT)) {
			continue;
		}

		eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_DEPTH_SIZE, &val);
		if (val == 0) {
			continue;
		}

		if (egl_conf_attr.red_size != EGL_DONT_CARE) {
			eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_RED_SIZE, &val);
			if (val != egl_conf_attr.red_size) {
				continue;
			}
		}
		if (egl_conf_attr.green_size != EGL_DONT_CARE) {
			eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_GREEN_SIZE, &val);
			if (val != egl_conf_attr.green_size) {
				continue;
			}
		}
		if (egl_conf_attr.blue_size != EGL_DONT_CARE) {
			eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_BLUE_SIZE, &val);
			if (val != egl_conf_attr.blue_size) {
				continue;
			}
		}
		if (egl_conf_attr.alpha_size != EGL_DONT_CARE) {
			eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_ALPHA_SIZE, &val);
			if (val != egl_conf_attr.alpha_size) {
				continue;
			}
		}
		if (egl_conf_attr.samples != EGL_DONT_CARE) {
			eglGetConfigAttrib(egl_disp, egl_configs[i], EGL_SAMPLES, &val);
			if (val != egl_conf_attr.samples) {
				continue;
			}
		}

		egl_conf = egl_configs[i];
		break;
	}

	free(egl_configs);

	if (egl_conf == (EGLConfig)0) {
		fprintf(stderr, "eglChooseConfig: could not find a matching configuration\n");
	}

	return egl_conf;
}


   // Get configs