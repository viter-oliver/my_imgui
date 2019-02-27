#include <GLFW/glfw3.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
int get_resolution(int* width,int* height)
{
	struct fb_var_screeninfo sc_info;
	int fd=open("/dev/fb0",O_RDWR);
	ioctl(fd,FBIOGET_VSCREENINFO,&sc_info);
	printf("%d*%d\n",sc_info.xres,sc_info.yres);
	*width=sc_info.xres;
	*height=sc_info.yres;
	close(fd);
	return 0;
}

int main()
{
	glfwSetErrorCallback(error_callback);
	glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    int iwidth=0,iheight=0;
    get_resolution(&iwidth,&iheight);
	GLFWwindow* window = glfwCreateWindow(iwidth, iheight, "OpenGL", NULL, NULL); // Windowed
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	while(!glfwWindowShouldClose(window))
	{
	    glfwSwapBuffers(window);
	    glfwPollEvents();
	}

	glfwTerminate();
}
