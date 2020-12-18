//========================================================================
// GLFW 3.3 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2016 Google Inc.
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "internal.h"
#include <string.h>


static int createNativeWindow(_GLFWwindow* window,
                              const _GLFWwndconfig* wndconfig)
{
    window->screen.width = wndconfig->width;
    window->screen.height = wndconfig->height;
    int rc=screen_create_window(&window->screen.screen_win,_glfw.screen.screen_ctx);
    if(rc)
    {
        printf("fail to screen_create_window\n");
        return GLFW_FALSE;
    }
    const char *display =wndconfig->display_name;	/* the display to create our window on */
    if(strlen(display)>0){
        screen_display_t *screen_disp;	/* native handle for our display */
        int screen_ndisplays;		/* number of displays */
        int i=0;
        int screen_val;
        rc = screen_get_context_property_iv(_glfw.screen.screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, 
        	&screen_ndisplays);
        if (rc) {
        	printf("screen_get_context_property_iv(SCREEN_PROPERTY_DISPLAY_COUNT)");
        	return GLFW_FALSE;
        }

        screen_disp = calloc(screen_ndisplays, sizeof(*screen_disp));
        if (screen_disp == NULL) {
        	fprintf(stderr, "could not allocate memory for display list\n");
        	return GLFW_FALSE;
        }

        rc = screen_get_context_property_pv(_glfw.screen.screen_ctx, SCREEN_PROPERTY_DISPLAYS, 
        	(void **)screen_disp);
        if (rc) {
        	printf("fail to screen_get_context_property_ptr(SCREEN_PROPERTY_DISPLAYS)\n");
        	free(screen_disp);
        	return GLFW_FALSE;
        }

        if (isdigit(*display)) {
        	int want_id = atoi(display);
        	for (i = 0; i < screen_ndisplays; ++i) {
        		int actual_id = 0;  // invalid
        		(void)screen_get_display_property_iv(screen_disp[i],
        				SCREEN_PROPERTY_ID, &actual_id);
        		if (want_id == actual_id) {
        			break;
        		}
        	}
        } else {
        	int type = -1;
        	if (strcmp(display, "internal") == 0) {
        		type = SCREEN_DISPLAY_TYPE_INTERNAL;
        	} else if (strcmp(display, "composite") == 0) {
        		type = SCREEN_DISPLAY_TYPE_COMPOSITE;
        	} else if (strcmp(display, "svideo") == 0) {
        		type = SCREEN_DISPLAY_TYPE_SVIDEO;
        	} else if (strcmp(display, "YPbPr") == 0) {
        		type = SCREEN_DISPLAY_TYPE_COMPONENT_YPbPr;
        	} else if (strcmp(display, "rgb") == 0) {
        		type = SCREEN_DISPLAY_TYPE_COMPONENT_RGB;
        	} else if (strcmp(display, "rgbhv") == 0) {
        		type = SCREEN_DISPLAY_TYPE_COMPONENT_RGBHV;
        	} else if (strcmp(display, "dvi") == 0) {
        		type = SCREEN_DISPLAY_TYPE_DVI;
        	} else if (strcmp(display, "hdmi") == 0) {
        		type = SCREEN_DISPLAY_TYPE_HDMI;
        	} else if (strcmp(display, "other") == 0) {
        		type = SCREEN_DISPLAY_TYPE_OTHER;
        	} else {
        		fprintf(stderr, "unknown display type %s\n", display);
        		free(screen_disp);
        		return GLFW_FALSE;
        	}
        	for (i = 0; i < screen_ndisplays; i++) {
        		screen_get_display_property_iv(screen_disp[i], 
        			SCREEN_PROPERTY_TYPE, &screen_val);
        		if (screen_val == type) {
        			break;
        		}
        	}
        }
        if (i >= screen_ndisplays) {
        	fprintf(stderr, "couldn't find display %s\n", display);
        	free(screen_disp);
        	return GLFW_FALSE;
        }

        rc = screen_set_window_property_pv(window->screen.screen_win, SCREEN_PROPERTY_DISPLAY, 
        (void **)&screen_disp[i]);
        	if (rc) {
        	printf("fail to screen_set_window_property_ptr(SCREEN_PROPERTY_DISPLAY)\n");
        	free(screen_disp);
        	return GLFW_FALSE;
        }

        free(screen_disp);
     }
    window->screen.zorder=wndconfig->zorder;
    int zorder=window->screen.zorder;
    rc = screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_ZORDER, &zorder);
    if (rc) 
    {
        printf("fail to screen_set_window_property_iv zorder=%d\n",zorder);
        return GLFW_FALSE;
    }
   // printf("_glfw.egl.display=%d,window->context.egl.config=%d\n",_glfw.egl.display, window->context.egl.config);
    
    int usage = SCREEN_USAGE_OPENGL_ES2;
    rc = screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_USAGE, &usage);
    if(rc)
     {
        printf("fail to screen_set_window_property_iv usage=%d\n",usage);
        return GLFW_FALSE;   
     }
    int size[2] = { wndconfig->width, wndconfig->height };
    printf("window width=%d,height=%d\n",size[0],size[1]);
    rc = screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_SIZE, size);
    if(rc)
     {
        printf("fail to screen_set_window_property_iv size=%d,%d\n",size[0],size[1]);
        return GLFW_FALSE;   
     }
    int transp = SCREEN_TRANSPARENCY_NONE;
    rc = screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_TRANSPARENCY, &transp);
     if(rc)
     {
        printf("fail to screen_set_window_property_iv transp=%d\n",transp);
        return GLFW_FALSE;   
     }
     EGLint interval = 1;
     rc = screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_SWAP_INTERVAL, &interval);
     if(rc)
     {
        printf("fail to screen_set_window_property_iv interval=%d\n",interval);
        return GLFW_FALSE;   
     }
     int pipeline = -1;
     if (pipeline != -1) {
		rc = screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_PIPELINE, &pipeline);
		 if(rc)
                {
                    printf("fail to screen_set_window_property_iv pipeline=%d\n",pipeline);
                    return GLFW_FALSE;   
                }
	}
     int nbuffers = 2;
     rc = screen_create_window_buffers(window->screen.screen_win, nbuffers);
     //printf("%s:%d\n",__FILE__,__LINE__);	  
/*	struct {
		EGLint render_buffer[2];
		EGLint none;
	} egl_surf_attr = {
		.render_buffer = { EGL_RENDER_BUFFER, EGL_BACK_BUFFER },
		.none = EGL_NONE
	};
    
     _glfw.screen.egl_surf = eglCreateWindowSurface(_glfw.screen.egl_disp, _glfw.screen.egl_conf,
		window->screen.screen_win, (EGLint*)&egl_surf_attr);
	if (_glfw.screen.egl_surf  == EGL_NO_SURFACE) {
		printf("fail to eglCreateWindowSurface\n");
		return GLFW_FALSE;
	}

	rc = eglMakeCurrent(_glfw.screen.egl_disp, _glfw.screen.egl_surf , _glfw.screen.egl_surf , _glfw.screen.egl_ctx);
	if (rc != EGL_TRUE) {
		printf("fail to eglMakeCurrent\n");
		return GLFW_FALSE;   
	}

	rc = eglSwapInterval(_glfw.screen.egl_disp, interval);
	if (rc != EGL_TRUE) {
		printf("fail to eglSwapInterval\n");
		return GLFW_FALSE;   
	}*/
    return GLFW_TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformCreateWindow(_GLFWwindow* window,
                              const _GLFWwndconfig* wndconfig,
                              const _GLFWctxconfig* ctxconfig,
                              const _GLFWfbconfig* fbconfig)
{
    if (!createNativeWindow(window, wndconfig))
        return GLFW_FALSE;
    if (!_glfwCreateContextEGL(window, ctxconfig, fbconfig))
                return GLFW_FALSE;
   /* int format=choose_format( _glfw.egl.display, window->context.egl.config);
    printf("format=%d\n",format);
    int rc = screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_FORMAT, &format);
    if (rc) {
	printf("fail to screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)\n");
	return GLFW_FALSE;
    }
    
    if (ctxconfig->client != GLFW_NO_API)
    {
        if (ctxconfig->source == GLFW_NATIVE_CONTEXT_API ||
            ctxconfig->source == GLFW_OSMESA_CONTEXT_API)
        {
            if (!_glfwInitOSMesa())
                return GLFW_FALSE;
            if (!_glfwCreateContextOSMesa(window, ctxconfig, fbconfig))
                return GLFW_FALSE;
        }
        else
        {
            _glfwInputError(GLFW_API_UNAVAILABLE, "Null: EGL not available");
            return GLFW_FALSE;
        }
    }*/
    // printf("%s:%d\n",__FILE__,__LINE__);	  

    return GLFW_TRUE;
}

void _glfwPlatformDestroyWindow(_GLFWwindow* window)
{
    if (window->context.destroy)
        window->context.destroy(window);
}

void _glfwPlatformSetWindowTitle(_GLFWwindow* window, const char* title)
{
}

void _glfwPlatformSetWindowIcon(_GLFWwindow* window, int count,
                                const GLFWimage* images)
{
}

void _glfwPlatformSetWindowMonitor(_GLFWwindow* window,
                                   _GLFWmonitor* monitor,
                                   int xpos, int ypos,
                                   int width, int height,
                                   int refreshRate)
{
}

void _glfwPlatformGetWindowPos(_GLFWwindow* window, int* xpos, int* ypos)
{
}

void _glfwPlatformSetWindowPos(_GLFWwindow* window, int xpos, int ypos)
{
}

void _glfwPlatformGetWindowSize(_GLFWwindow* window, int* width, int* height)
{
    if (width)
        *width = window->screen.width;
    if (height)
        *height = window->screen.height;
}

void _glfwPlatformSetWindowSize(_GLFWwindow* window, int width, int height)
{
    window->screen.width = width;
    window->screen.height = height;
}

void _glfwPlatformSetWindowSizeLimits(_GLFWwindow* window,
                                      int minwidth, int minheight,
                                      int maxwidth, int maxheight)
{
}

void _glfwPlatformSetWindowAspectRatio(_GLFWwindow* window, int n, int d)
{
}

void _glfwPlatformGetFramebufferSize(_GLFWwindow* window, int* width, int* height)
{
    if (width)
        *width = window->screen.width;
    if (height)
        *height = window->screen.height;
}

void _glfwPlatformGetWindowFrameSize(_GLFWwindow* window,
                                     int* left, int* top,
                                     int* right, int* bottom)
{
}

void _glfwPlatformGetWindowContentScale(_GLFWwindow* window,
                                        float* xscale, float* yscale)
{
    if (xscale)
        *xscale = 1.f;
    if (yscale)
        *yscale = 1.f;
}

void _glfwPlatformIconifyWindow(_GLFWwindow* window)
{
}

void _glfwPlatformRestoreWindow(_GLFWwindow* window)
{
}

void _glfwPlatformMaximizeWindow(_GLFWwindow* window)
{
}

int _glfwPlatformWindowMaximized(_GLFWwindow* window)
{
    return GLFW_FALSE;
}

int _glfwPlatformWindowHovered(_GLFWwindow* window)
{
    return GLFW_FALSE;
}

int _glfwPlatformFramebufferTransparent(_GLFWwindow* window)
{
    return GLFW_FALSE;
}

void _glfwPlatformSetWindowResizable(_GLFWwindow* window, GLFWbool enabled)
{
}

void _glfwPlatformSetWindowDecorated(_GLFWwindow* window, GLFWbool enabled)
{
}

void _glfwPlatformSetWindowFloating(_GLFWwindow* window, GLFWbool enabled)
{
}

float _glfwPlatformGetWindowOpacity(_GLFWwindow* window)
{
    return 1.f;
}

void _glfwPlatformSetWindowOpacity(_GLFWwindow* window, float opacity)
{
}

void _glfwPlatformShowWindow(_GLFWwindow* window)
{
    int vis = 1;
    screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_VISIBLE, &vis);
}


void _glfwPlatformRequestWindowAttention(_GLFWwindow* window)
{
}

void _glfwPlatformUnhideWindow(_GLFWwindow* window)
{
}

void _glfwPlatformHideWindow(_GLFWwindow* window)
{
    int vis = 0;
    screen_set_window_property_iv(window->screen.screen_win, SCREEN_PROPERTY_VISIBLE, &vis);

}

void _glfwPlatformFocusWindow(_GLFWwindow* window)
{
}

int _glfwPlatformWindowFocused(_GLFWwindow* window)
{
    return GLFW_FALSE;
}

int _glfwPlatformWindowIconified(_GLFWwindow* window)
{
    return GLFW_FALSE;
}

int _glfwPlatformWindowVisible(_GLFWwindow* window)
{
    return GLFW_FALSE;
}

void _glfwPlatformPollEvents(void)
{
}

void _glfwPlatformWaitEvents(void)
{
}

void _glfwPlatformWaitEventsTimeout(double timeout)
{
}

void _glfwPlatformPostEmptyEvent(void)
{
}

void _glfwPlatformGetCursorPos(_GLFWwindow* window, double* xpos, double* ypos)
{
}

void _glfwPlatformSetCursorPos(_GLFWwindow* window, double x, double y)
{
}

void _glfwPlatformSetCursorMode(_GLFWwindow* window, int mode)
{
}

int _glfwPlatformCreateCursor(_GLFWcursor* cursor,
                              const GLFWimage* image,
                              int xhot, int yhot)
{
    return GLFW_TRUE;
}

int _glfwPlatformCreateStandardCursor(_GLFWcursor* cursor, int shape)
{
    return GLFW_TRUE;
}

void _glfwPlatformDestroyCursor(_GLFWcursor* cursor)
{
}

void _glfwPlatformSetCursor(_GLFWwindow* window, _GLFWcursor* cursor)
{
}

void _glfwPlatformSetClipboardString(const char* string)
{
}

const char* _glfwPlatformGetClipboardString(void)
{
    return NULL;
}

const char* _glfwPlatformGetScancodeName(int scancode)
{
    return "";
}

int _glfwPlatformGetKeyScancode(int key)
{
    return -1;
}

void _glfwPlatformGetRequiredInstanceExtensions(char** extensions)
{
}

int _glfwPlatformGetPhysicalDevicePresentationSupport(VkInstance instance,
                                                      VkPhysicalDevice device,
                                                      uint32_t queuefamily)
{
    return GLFW_FALSE;
}

VkResult _glfwPlatformCreateWindowSurface(VkInstance instance,
                                          _GLFWwindow* window,
                                          const VkAllocationCallbacks* allocator,
                                          VkSurfaceKHR* surface)
{
    // This seems like the most appropriate error to return here
    return VK_ERROR_INITIALIZATION_FAILED;
}

