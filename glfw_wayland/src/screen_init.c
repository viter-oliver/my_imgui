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
#include "choose_config.h"

//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformInit(void)
{
	struct {
		EGLint client_version[2];
		EGLint none;
	} egl_ctx_attr = {
		.client_version = { EGL_CONTEXT_CLIENT_VERSION, 2 },
		.none = EGL_NONE
	};
    _glfw.screen.display=EGL_DEFAULT_DISPLAY;//by default
    if (!_glfwInitEGL())
        return GLFW_FALSE;
   int rc;  
   /* int disp_id= EGL_DEFAULT_DISPLAY;
    
   _glfw.screen.egl_disp = eglGetDisplay(disp_id);
    if(_glfw.screen.egl_disp==EGL_NO_DISPLAY)
    {
        printf("fail to get display\n");
        return GLFW_FALSE;
    }    
    printf("%s:%d\n",__FILE__,__LINE__);
   
    rc = eglInitialize(_glfw.screen.egl_disp, NULL, NULL);
    if(rc!= EGL_TRUE){
        printf("fail to eglInitialize\n");
        return GLFW_FALSE;
    }
    
    const char *conf_str = NULL;
    
    
    _glfw.screen.egl_conf= choose_config(_glfw.screen.egl_disp, conf_str);
    if (_glfw.screen.egl_conf == (EGLConfig)0) {
		printf("fail to choost config\n");
            return GLFW_FALSE;
	}

     _glfw.screen.egl_ctx = eglCreateContext(_glfw.screen.egl_disp,  _glfw.screen.egl_conf,
		EGL_NO_CONTEXT, (EGLint*)&egl_ctx_attr);
	if ( _glfw.screen.egl_ctx == EGL_NO_CONTEXT) {
		printf("fail to eglCreateContext\n");
            return GLFW_FALSE;
	}    */
    rc = screen_create_context(&_glfw.screen.screen_ctx, 0);
    if(rc)
    {
        printf("fail to create screen context\n");
        return GLFW_FALSE;
    }
    rc = screen_create_event(&_glfw.screen.screen_ev);
    if(rc)
    {
        printf("fail to create event\n");
        return GLFW_FALSE;
    }

    /*
    rc = screen_get_context_property_iv(_glfw.screen.screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &_glfw.screen.screen_ndisplays);
    _glfw.screen.screen_disp = calloc(_glfw.screen.screen_ndisplays, sizeof(*_glfw.screen.screen_disp));
    rc = screen_get_context_property_pv(_glfw.screen.screen_ctx, SCREEN_PROPERTY_DISPLAYS,(void **)_glfw.screen.screen_disp);
    */
    _glfwInitTimerPOSIX();
    return GLFW_TRUE;
}

void _glfwPlatformTerminate(void)
{
    screen_destroy_context(_glfw.screen.screen_ctx);

}

const char* _glfwPlatformGetVersionString(void)
{
    return _GLFW_VERSION_NUMBER " null OSMesa";
}

