#pragma once


//Simple OGL --------------

//=============================================================================
// Summary :
//
// simple_ogl is small library written in C++ that provides a quick and simple way to create an 
// OpenGL ready Window.
//
// It is a single file header that should be very easy to drop-in and start using in any pre-existing project.
//
//
//=============================================================================
// Revision History :
//
//  
//
//===============================================================================  
// Todo List :
// 
//  - Support for differnt OS
//
//===============================================================================  
//  You MUST define SIMPLE_OGL_IMPLEMENTATION
//
//  in exactly *one* C or C++ file that includes this header, BEFORE the include
//  like so:
//
//  #define SIMPLE_OGL_IMPLEMENTATION
//  #include "simple_ogl.h"
//
//  All other files should just include "simple_ogl.h" without the define
//=============================================================================
// Basic Usage / Quick Start :
//
// 
//
// After defining SIMPLE_OGL_IMPLEMENTATION
//
// To create a default window:
//
// 1) Declare a SGLWindow 
// 2) Call sgl_window
//
// To specify some different options for your window:
//  1)
//      - You can either directly set the values of the SGLWindow struct
//      - Or call sgl_platform_window_setup with your desired values
//  2)
//      - Call sgl_window
// 
// For more information check the [DEFAULT EXAMPLE] section for a running example that is ready to use.
// 
//===============================================================================
// Declaring and Loading OpenGL funtions
// 
// The section [OpenGL Function Declarations] contains an IMCOMPLETE (very imcomplete) and basic
// list of OpenGL functions. 
// 
// Most definetely you will need to add and load new functions if you use this library.
// To add a new OpenGL function you do two things :
//
// 1) Add the function declaration of the GL function you want in the [DECLARE NEW GL FUNCTION] section,
//    using the DECLARE_GL_FUNC_PTR macro. All you need to know is the function signature.
//    In that section you can see plenty of other examples of how to declare these. 
//
// 2) Load the function in sgl_load_gl_functions() .
//    At the end of sgl_load_gl_functions() there is a section [LOAD NEW GL FUNCTION] ,
//    this is where you use the GET_GL_FUNC_SAFE macro to load the function you declared in step 1.
//
//===============================================================================
//
// API  : API reference can be found further down.
//        In the following sections (search for) :
//          [Win32 Create an OpenGL ready window]  -> Win32 Window Creation API
//          [DECLARE NEW GL FUNCTION]              -> Declare new GL function that you need
//          [LOAD NEW GL FUNCTION]                 -> Load new GL function that you need (must declare first)
//          [DEFAULT EXAMPLE]                      -> Ready to run examples.
//===============================================================================  

// ***NOTE TO ANY USER OF THIS LIBRARY***
//
//  On Win32 this library uses #pragma comment to link to the following static libraries :
//    - OpenGL32.lib
//    - Gdi32.lib
//  
//  If using the default example:
//    -User32.lib



#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/gl.h>

#pragma comment(lib, "OpenGL32.lib")

#endif //_WIN32



// [INTERNAL] Types --------------
#include <stdint.h>
//Unsigned
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t  uint8;
//Signed
typedef int32_t int32;
//Bool
typedef int32 bool32;

// [INTERNAL] Static Declarations
#define internal        static
#define global_variable static
#define local_persist   static

// [INTERNAL] Utility Macros
#define Bytes(n)  (n)
#define KiloBytes(n)  (Bytes(n)*1024)
#define InvalidCodePath SGL_Assert(!"InvalidCodePath")
#define InvalidDefaultCase default: {InvalidCodePath;} break
#define SGL_Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

#ifdef _WIN32
//[INTERNAL] Function Pointers Typedefs
#define WIN32_WINDOW_CALLBACK(name) LRESULT CALLBACK name(HWND Window,UINT Message,WPARAM WParam,LPARAM LParam)
typedef WIN32_WINDOW_CALLBACK(win32_window_callback);
#endif //_WIN32

//[INTERNAL] OpenGL Enums and Types not found in gl.h

//@NOTE: gl.h can be found in almost all win32 systems while the other headers are not usually present.
typedef char			GLchar;
typedef ptrdiff_t		GLsizeiptr;
typedef uint64_t    	GLuint64;

//Enums in glcorearb.h from https://www.khronos.org/registry/OpenGL/index_gl.php#headers
    #define GL_TEXTURE0								0x84C0
    #define GL_TEXTURE1								0x84C1
    #define GL_TEXTURE_RECTANGLE					0x84F5
    #define GL_RGBA_INTEGER							0x8D99
    #define GL_HALF_FLOAT							0x140B
    #define GL_RGBA16UI								0x8D76
    #define GL_RGBA16F								0x881A
    #define GL_VERTEX_SHADER						0x8B31
    #define GL_FRAGMENT_SHADER						0x8B30
    #define GL_ARRAY_BUFFER							0x8892
    #define GL_ELEMENT_ARRAY_BUFFER					0x8893
    #define GL_STATIC_DRAW							0x88E4
    #define GL_WRITE_ONLY							0x88B9
    #define GL_QUERY_RESULT							0x8866
    #define GL_TIME_ELAPSED							0x88BF
    #define GL_TIMESTAMP							0x8E28
    #define GL_FRAMEBUFFER_SRGB						0x8DB9
    #define GL_COMPILE_STATUS                       0x8B81
    #define GL_INFO_LOG_LENGTH                      0x8B84
    #define GL_GEOMETRY_SHADER                      0x8DD9
    #define GL_LINK_STATUS                          0x8B82




// WGL Enums -- in wglext.h from https://www.khronos.org/registry/OpenGL/index_gl.php#headers
#ifdef _WIN32

    #define WGL_CONTEXT_MAJOR_VERSION_ARB			0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB			0x2092
    #define WGL_CONTEXT_PROFILE_MASK_ARB			0x9126
    #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB		0x00000001
    #define WGL_DRAW_TO_WINDOW_ARB					0x2001
    #define WGL_ACCELERATION_ARB					0x2003
    #define WGL_SUPPORT_OPENGL_ARB					0x2010
    #define WGL_DOUBLE_BUFFER_ARB					0x2011
    #define WGL_PIXEL_TYPE_ARB						0x2013
    #define WGL_COLOR_BITS_ARB						0x2014
    #define WGL_ALPHA_BITS_ARB						0x201B
    #define WGL_FULL_ACCELERATION_ARB				0x2027
    #define WGL_TYPE_RGBA_ARB						0x202B
    #define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB		0x20A9
    #define WGL_DEPTH_BITS_ARB                      0x2022
    #define WGL_STENCIL_BITS_ARB                    0x2023

#endif //_WIN32


//=============================================================================
// API - [Win32 Create an OpenGL ready window]
//
//=============================================================================

struct SGLWindow {
    //platorm independent part
    bool32 initialized;
    int32 width;
    int32 height;
    char* title;

    bool32 fullscreen;
    bool32 running;

    union 
    {
        //Win32
        struct {
            HWND  handle;
            WINDOWPLACEMENT window_position;
            HDC device_context;
            HGLRC rendering_context;    
        };
        //@TODO: Other OS
    };
};

// This is used to setup the window before creating it.
// You can also set these values directly in the struct if you prefer.
//
// window           - pointer to the SGLWindow struct
// title            - the window title
// width and height - window widht and height.
// full_screen      - whether we want to create a fullscreen window or not. 
void  sgl_win32_window_setup(SGLWindow* window,char* title = "SimpleOGL Window", int32 width = 800, int32 height = 600, bool32 full_screen = 0);


// This function can be called without setting any paramaters of the SGLWindow struct,
// if this is the case and the SGLWindow is not initialized we will get a default window.
// In Win32 OS the default can be seen as the default params in the sgl_win32_window_setup function.
// 
// window          - pointer to a SGLWindow struct
// instance        - the HINSTANCE Instance arg passed in the WinMain function.
// window_callback - a pointer to a Windows Window Callback function.
//                   Windows needs a function like this to handle window events.
//                   the function has the following signature :
//                     LRESULT CALLBACKWindowCallback(HWND Window,UINT Message, WPARAM WParam, LPARAM LParam);
//
//You can see an example of a callback function in the [Win32 Default Example] Section further below
//where the function  sgl_default_win32_wnd_callback is defined
void sgl_win32_window_create(SGLWindow* window,HINSTANCE instance,win32_window_callback* window_callback);

//This function sets up an OpenGL modern context to work with our window.
//All we need is to pass a pointer to a SGLWindow struct, optionally we can specify a major and minor version
//of the OpenGL context we would like to get.
//For example if we specify major = 3 and minor = 0 we will be requesting a Opengl version 3.0 context.
//If we don't specify major and minor then we will get the lastest supported version in our system.
//
//window        - pointer to SGLWindow struct
//major_version - desired major version of OpenGL
//minor_version - desired minor version of OpenGL
bool32 sgl_win32_window_ogl_setup(SGLWindow* window, int32 major_version = 0, int32 minor_version = 0);

//This is the simplest call we can make to create a window (in any supported platform).
//This functions is basically the equivalent of calling
//  sgl_win32_window_create and sgl_win32_window_ogl_setup and it is all it is doing internally.
void sgl_window(SGLWindow* window,HINSTANCE instance,win32_window_callback* window_callback);

//This is a helper function that toggles between fullscreen and windowed window.
//window - pointer to SGLWindow struct 
void sgl_win32_window_toggle_fullscreen(SGLWindow* window);

//END API -------------------------------

//===============================================================================  
// [IMPLEMENTATION]
//=============================================================================== 
#ifdef SIMPLE_OGL_IMPLEMENTATION

//
//[OpenGL Functions] ---------------------

//@TODO: Explain this bit more
//Macro for Declaring OpenGL Function Pointers
#define DECLARE_GL_FUNC_PTR(return_type, func_name, params) typedef return_type func_name##_func_signature params; \
                                                            func_name##_func_signature *func_name = nullptr; 

#ifdef _WIN32
//Macro for getting the adress of the given OpenGL function and checking we have a valid address (not null).
#define GET_GL_FUNC_SAFE(name) name = (name##_func_signature *)wglGetProcAddress(#name); \
                               SGL_Assert(name);
#else
    //@TODO: Other OS
   #error No other OS defined!
#endif //_WIN32

//[OpenGL Function Declarations] --------------------------------------------------------------------------------

	DECLARE_GL_FUNC_PTR(void, glActiveTexture, (GLenum))
	DECLARE_GL_FUNC_PTR(void, glMultiDrawElements, (GLenum, const GLsizei *, GLenum, const void *const *, GLsizei))
	DECLARE_GL_FUNC_PTR(void, glEnableVertexAttribArray, (GLuint))
	DECLARE_GL_FUNC_PTR(void, glDisableVertexAttribArray, (GLuint))
	DECLARE_GL_FUNC_PTR(void, glVertexAttribPointer, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void *))
	DECLARE_GL_FUNC_PTR(void, glGenBuffers, (GLsizei, GLuint *))
	DECLARE_GL_FUNC_PTR(void, glDeleteBuffers, (GLsizei, const GLuint *))
	DECLARE_GL_FUNC_PTR(void, glBindBuffer, (GLenum, GLuint))
	DECLARE_GL_FUNC_PTR(void, glBufferData, (GLenum, GLsizeiptr, const void *, GLenum))
	DECLARE_GL_FUNC_PTR(void *, glMapBuffer, (GLenum, GLenum))
	DECLARE_GL_FUNC_PTR(GLboolean, glUnmapBuffer, (GLenum))
	DECLARE_GL_FUNC_PTR(GLuint, glCreateShader, (GLenum))
	DECLARE_GL_FUNC_PTR(void, glDeleteShader, (GLuint))
	DECLARE_GL_FUNC_PTR(void, glShaderSource, (GLuint, GLsizei, const GLchar **, const GLint *))
	DECLARE_GL_FUNC_PTR(void, glCompileShader, (GLuint))
	DECLARE_GL_FUNC_PTR(void, glAttachShader, (GLuint, GLuint))
	DECLARE_GL_FUNC_PTR(GLuint, glCreateProgram, (void))
	DECLARE_GL_FUNC_PTR(void, glDeleteProgram, (GLuint))
	DECLARE_GL_FUNC_PTR(void, glLinkProgram, (GLuint))
	DECLARE_GL_FUNC_PTR(void, glUseProgram, (GLuint))
	DECLARE_GL_FUNC_PTR(GLint, glGetUniformLocation, (GLuint, const GLchar *))
	DECLARE_GL_FUNC_PTR(void, glUniform1i, (GLint, GLint))
	DECLARE_GL_FUNC_PTR(void, glUniform4f, (GLint, GLfloat, GLfloat, GLfloat, GLfloat))
	DECLARE_GL_FUNC_PTR(void, glUniform4fv, (GLint, GLsizei, const GLfloat *))
	DECLARE_GL_FUNC_PTR(void, glGenQueries, (GLsizei, GLuint *))
	DECLARE_GL_FUNC_PTR(void, glDeleteQueries, (GLsizei, const GLuint *))
	DECLARE_GL_FUNC_PTR(void, glBeginQuery, (GLenum, GLuint))
	DECLARE_GL_FUNC_PTR(void, glEndQuery, (GLenum))
    DECLARE_GL_FUNC_PTR(void, glGenVertexArrays, (GLsizei, GLuint *))
    DECLARE_GL_FUNC_PTR(void, glDeleteVertexArrays, (GLsizei, const GLuint *))
    DECLARE_GL_FUNC_PTR(void, glBindVertexArray, (GLuint))
    DECLARE_GL_FUNC_PTR(void, glGetShaderiv, (GLuint, GLenum, GLint *))
    DECLARE_GL_FUNC_PTR(void, glGetShaderInfoLog, (GLuint, GLsizei, GLsizei, GLchar *))
    DECLARE_GL_FUNC_PTR(void, glGetProgramiv, (GLuint, GLenum, GLint *)) 
    DECLARE_GL_FUNC_PTR(void, glGetProgramInfoLog, (GLuint, GLsizei, GLsizei *, GLchar *))
    DECLARE_GL_FUNC_PTR(void, glDetachShader, (GLuint, GLuint))
    
    //[DECLARE NEW GL FUNCTION]
    //Declare any new functions here...

//[END OpenGL Function Delarations] -------------------------------------------------------------------------------------------------------

void sgl_load_gl_functions()
{
    GET_GL_FUNC_SAFE(glActiveTexture)
	GET_GL_FUNC_SAFE(glMultiDrawElements)
	GET_GL_FUNC_SAFE(glEnableVertexAttribArray)
	GET_GL_FUNC_SAFE(glDisableVertexAttribArray)
	GET_GL_FUNC_SAFE(glVertexAttribPointer)
	GET_GL_FUNC_SAFE(glGenBuffers)
	GET_GL_FUNC_SAFE(glDeleteBuffers)
	GET_GL_FUNC_SAFE(glBindBuffer)
	GET_GL_FUNC_SAFE(glBufferData)
	GET_GL_FUNC_SAFE(glMapBuffer)
	GET_GL_FUNC_SAFE(glUnmapBuffer)
	GET_GL_FUNC_SAFE(glCreateShader)
	GET_GL_FUNC_SAFE(glDeleteShader)
	GET_GL_FUNC_SAFE(glShaderSource)
	GET_GL_FUNC_SAFE(glCompileShader)
	GET_GL_FUNC_SAFE(glAttachShader)
	GET_GL_FUNC_SAFE(glCreateProgram)
	GET_GL_FUNC_SAFE(glDeleteProgram)
	GET_GL_FUNC_SAFE(glLinkProgram)
	GET_GL_FUNC_SAFE(glUseProgram)
	GET_GL_FUNC_SAFE(glGetUniformLocation)
	GET_GL_FUNC_SAFE(glUniform1i)
	GET_GL_FUNC_SAFE(glUniform4f)
	GET_GL_FUNC_SAFE(glUniform4fv)
	GET_GL_FUNC_SAFE(glGenQueries)
	GET_GL_FUNC_SAFE(glDeleteQueries)
	GET_GL_FUNC_SAFE(glBeginQuery)
	GET_GL_FUNC_SAFE(glEndQuery)
    GET_GL_FUNC_SAFE(glGenVertexArrays)
    GET_GL_FUNC_SAFE(glDeleteVertexArrays)
    GET_GL_FUNC_SAFE(glBindVertexArray)
    GET_GL_FUNC_SAFE(glGetShaderiv)
    GET_GL_FUNC_SAFE(glGetShaderInfoLog)
    GET_GL_FUNC_SAFE(glGetProgramiv)
    GET_GL_FUNC_SAFE(glGetProgramInfoLog)
    GET_GL_FUNC_SAFE(glDetachShader)

    //[LOAD NEW FUNCTION]
    // Load any other functions you might need here.
}
//
//[END OpenGL Functions] ---------------------

//
//[Win32] ---------------------


#ifdef _WIN32

//For SwapBuffers
#pragma comment(lib, "Gdi32.lib")

//[INTERNAL] - Declaring Win32 specific OpenGL function pointers.
DECLARE_GL_FUNC_PTR(BOOL ,wglChoosePixelFormatARB, (HDC , const int *, const FLOAT *, UINT , int *, UINT *))
DECLARE_GL_FUNC_PTR(HGLRC ,wglCreateContextAttribsARB, (HDC , HGLRC , const int *))


internal void 
sgl_win32_window_setup(SGLWindow* window,char* title, int32 width, int32 height, bool32 fullscreen)
{    
    window->initialized = true;
    window->width  = width;//1920
    window->height = height;//1080
    window->handle = 0;
    window->title = title;
    window->fullscreen = fullscreen;
    window->running = true;
    window->window_position = {};
}

//@TODO: Should this be the only function in the simplest API possible?
internal void
sgl_win32_window_create(SGLWindow* window,HINSTANCE instance,win32_window_callback* window_callback)
{

    if(!window->initialized){
        sgl_win32_window_setup(window);
    }
    //TODO: Make it so that we can make a fullscreen window.    
    WNDCLASSA window_class = {};
    window_class.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC; //TODO: Try CS_OWNDC
    window_class.lpfnWndProc = window_callback;
    window_class.hInstance = instance;
    window_class.hIcon = LoadIcon(instance, IDI_APPLICATION); //default icon
    window_class.lpszClassName = "SGL Window";
    window_class.hCursor = LoadCursor(0, IDC_ARROW);    //default cursor
    //ShowCursor(0);
    
    if(RegisterClassA(&window_class))
    {
        RECT window_rect = {0 ,0, window->width, window->height};
        DWORD style = WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
        AdjustWindowRect(&window_rect, style, 0);
        
        window->handle = CreateWindowExA( WS_EX_WINDOWEDGE/*|WS_EX_COMPOSITED*/, //WS_EX_TOPMOST|WS_EX_LAYERED, //TODO: Experiment with this 
                                                window_class.lpszClassName, 
                                                window->title,
                                                style, //TODO: Experiment with this 
                                                CW_USEDEFAULT,
                                                CW_USEDEFAULT,
                                                window_rect.right -window_rect.left,
                                                window_rect.bottom -window_rect.top ,
                                                0, 0, instance, 0);
    }
    else
    {
        //TODO: Some sort of error reporting to user.
    }
}

internal bool32
sgl_win32_window_ogl_setup(SGLWindow* window, int32 major_version, int32 minor_version)
{
    window->device_context = GetDC(window->handle);

     //Setup Pixel Format
    PIXELFORMATDESCRIPTOR desired_pixel_format = {};
    desired_pixel_format.nSize = sizeof(desired_pixel_format);
    desired_pixel_format.nVersion = 1;   
    desired_pixel_format.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
    desired_pixel_format.cColorBits = 32; //@TODO: Allow to be specified by user
    desired_pixel_format.cDepthBits = 8; //@TODO: Allow to be specified by user
    desired_pixel_format.cStencilBits = 8;//@TODO: Allow to be specified by user
    desired_pixel_format.cAlphaBits   = 8;//@TODO: Allow to be specified by user
    desired_pixel_format.iLayerType = PFD_MAIN_PLANE;

    int32 suggested_format_index = ChoosePixelFormat(window->device_context, &desired_pixel_format);

    PIXELFORMATDESCRIPTOR suggested_pixel_format;
    DescribePixelFormat(window->device_context, suggested_format_index, 
                        sizeof(suggested_pixel_format), &suggested_pixel_format);
    SetPixelFormat(window->device_context, suggested_format_index, &suggested_pixel_format);
    
    window->rendering_context = wglCreateContext(window->device_context);
    if(wglMakeCurrent(window->device_context, window->rendering_context))
    {
        //We can now create a modern OpenGL context.

        
        GET_GL_FUNC_SAFE(wglCreateContextAttribsARB)

        if(wglCreateContextAttribsARB)
        {
            const int context_attrib_list[] =
            {
                //WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                //WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0, 0 //End
            };
            
            HGLRC share_context = 0;
            HGLRC modern_gl_context = wglCreateContextAttribsARB(window->device_context,share_context,context_attrib_list );
        
            if(modern_gl_context)
            {
                if(wglMakeCurrent(window->device_context, modern_gl_context))
                {
                    wglDeleteContext(window->rendering_context);
                    window->rendering_context = modern_gl_context;
                }
            }
        }
        else
        {
            //@TODO: Could not create modern gl context, old version of OGL
        }

        //@TODO: Get OpenGL context info
        char* vendor     = (char *)glGetString(GL_VENDOR);
        char* renderer   = (char *)glGetString(GL_RENDERER);
        char* version    = (char *)glGetString(GL_VERSION);
        char* extensions = (char *)glGetString(GL_EXTENSIONS);
    }
    else
    {
        SGL_Assert(!"We should not be here!");
    }

    sgl_load_gl_functions();

    ReleaseDC(window->handle, window->device_context);

    return true;
}

internal void
sgl_win32_window_toggle_fullscreen(SGLWindow* window)
{
    //@NOTE: Taken from Casey Muratori and Raymond Chen
    //http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx

    DWORD style = GetWindowLong(window->handle,GWL_STYLE);
    if(style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitor_info = {sizeof(monitor_info)};
        
        window->window_position = {sizeof(window->window_position)};
        if(GetWindowPlacement(window->handle, &window->window_position) &&
           GetMonitorInfo(MonitorFromWindow(window->handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
        {
            SetWindowLong(window->handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window->handle, HWND_TOP,
                         monitor_info.rcMonitor.left,    monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right -  monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(window->handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window->handle, &window->window_position);
        SetWindowPos(window->handle, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

//@NOTE: The simplest API call we can make to create a window, this will create a default window.
internal void
sgl_window(SGLWindow* window,HINSTANCE instance,win32_window_callback* window_callback)
{
    sgl_win32_window_create(window,instance,window_callback);
    sgl_win32_window_ogl_setup(window);
}

#endif //_WIN32


//[END Win32] ---------------------





//===============================================================================  
// [DEFAULT EXAMPLE]
//
// This is a ready to run simple example.
// 
// To use the example :
//
// 1) #define SGL_DEFAULT_EXAMPLE
// 
// 2) Copy the WinMain from [Win32 Example Program]
//
//=============================================================================== 



#ifdef SGL_DEFAULT_EXAMPLE

#include <stdio.h>

//[Win32 Default Example] ---------------------
#ifdef _WIN32 

//For the win32 windows callback
#pragma comment(lib, "User32.lib")

//@NOTE: Change this define to point to your own Win32 Window Callback Function.
#define sgl_win32_window_callback sgl_default_win32_wnd_callback 
//@NOTE: Change this define to point to your own function that processes Win32 Messages.
#define sgl_win32_process_msgs sgl_default_win32_process_msgs
//

global_variable SGLWindow default_main_window = {};

internal LRESULT CALLBACK
sgl_default_win32_wnd_callback(HWND window, UINT message, WPARAM WParam, LPARAM LParam)
{
    LRESULT result = 0;

    switch(message)
    {
        case WM_CREATE:
        {       
            return(0);
        }break;     
        case WM_CLOSE:
        {
            default_main_window.running = false;
        }break;
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);            
            RECT main_rect;
            GetClientRect(default_main_window.handle, &main_rect);
            EndPaint(window, &paint);                   
        }break;
        case WM_SIZE:
        {
        }break;
        case WM_MOUSEMOVE:
        {
        }break;
        case WM_KEYDOWN:
        {
            switch(WParam)
            {
            }
        }break;
        
        default:
        {
            //@NOTE: Default window handling.
            result = DefWindowProcA(window, message, WParam, LParam);
        }break;
    }
    return(result);
}

internal void
sgl_default_win32_process_msgs(void)
{
    MSG message;
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        //NOTE: For now lets just translate and dispatch all message types trough Windows.
        TranslateMessage(&message);
        //NOTE: This will pipe the messages to the Win32MainWindowCallback func
        DispatchMessageA(&message);               
    }
}

/*[Win32 Example Program]

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{

    HMODULE module = GetModuleHandle(0);
    sgl_window(&default_main_window,Instance,sgl_win32_window_callback);
    //Default Example
    sgl_init_default_state();
    //Loop
    while(default_main_window.running)
    {
        sgl_win32_process_msgs();
        sgl_default_render(&default_main_window);
    }   
    return 0;
}

*/

#else
    //@TODO: Other OS
   #error No other OS defined!
#endif //_WIN32 



//Example Triangle.
GLfloat triangle_vertex_positions[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

struct sgl_default_renderer{
    GLuint index_buffer;
    GLuint vertex_buffer;
    GLuint program_default; 
};

global_variable sgl_default_renderer sgl_default_ogl;

//Example Default Shaders
char* sgl_default_vertex_shader[] =
{"#version 330                                                      \n"
 "//Default.VERT                                                    \n"
 "layout (location = 0) in vec3 vertexPosition_modelspace;          \n"
 "void main()                                                       \n"
 "{                                                                 \n"
 "gl_Position.xyz = vertexPosition_modelspace;                      \n"
 "gl_Position.w = 1.0;                                              \n"
 "}                                                                 \n"
};

char* sgl_default_frag_shader[] =
{"#version 330                    \n"
 "//Default FRAG SHADER           \n"
 "out vec4 color;                 \n"
 "void main()                     \n"
 "{                               \n"                                             
 "    color =  vec4(0.5, 0, 0.5, 1);  \n"
 "}                               \n"
};

GLuint
sgl_internal_shader_create(GLenum shader_type,const char* shader_file)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1,&shader_file, NULL);
    glCompileShader(shader);
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint info_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

        //TODO(filipe): Get rid of this new...
        GLchar *info_log = new GLchar[info_log_length + 1];
        glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

        const char *string_shader_type = NULL;
        switch(shader_type)
        {
            case GL_VERTEX_SHADER:   string_shader_type = "vertex"; break;
            case GL_GEOMETRY_SHADER: string_shader_type = "geometry"; break;
            case GL_FRAGMENT_SHADER: string_shader_type = "fragment"; break;
        }
        fprintf(stderr, "Compile failure in %s shader:\n%s\n",
                string_shader_type, info_log);
        
        delete(info_log);
    }
    return shader;
}

GLuint
sgl_internal_program_create(GLuint* shader_list, int size, char* debug_name)
{
    GLuint program = glCreateProgram();
    
    for(size_t index = 0; index < size; ++index)
    {
        glAttachShader(program, shader_list[index]);
    }
    glLinkProgram(program);

    GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint info_log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar *string_info_log = new GLchar[info_log_length + 1];
        glGetProgramInfoLog(program, info_log_length, NULL, string_info_log);
            
        fprintf(stderr, "Linker failure in Program [%s]: %s\n",debug_name,  string_info_log);

        delete(string_info_log);
    }
    for(size_t index = 0; index < size; ++index)
    {
        glDetachShader(program, shader_list[index]);
    }
    return program;
}

internal void sgl_init_default_program()
{
    const int32 shader_list_size = 2;
    //NOTE(filipe): Triangle Example
    GLuint shader_list[shader_list_size] = 
    {
        sgl_internal_shader_create(GL_VERTEX_SHADER,sgl_default_vertex_shader[0]),
        sgl_internal_shader_create(GL_FRAGMENT_SHADER,sgl_default_frag_shader[0])
    };

    sgl_default_ogl.program_default = sgl_internal_program_create(shader_list, shader_list_size, "SGL Default Program");
    for(size_t index = 0; index < shader_list_size; ++index)
    {
        glDeleteShader(shader_list[index]);
    }
}


void sgl_init_default_state()
{
    //Init default buffers
    glGenBuffers(1, &sgl_default_ogl.index_buffer);
    //InitVertexBuffer(&sgl_default_ogl.VertexBuffer,triangle_vertex_positions,ArrayCount(triangle_vertex_positions));    
    //

    int32 length = sizeof(triangle_vertex_positions) / sizeof(triangle_vertex_positions[0]);
    int32 size = length*sizeof(triangle_vertex_positions[0]);
    glGenBuffers(1, &sgl_default_ogl.vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sgl_default_ogl.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 size,
                 triangle_vertex_positions,
                 GL_STATIC_DRAW);    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    sgl_init_default_program();
    
    //GL state 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);        
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void sgl_default_render(SGLWindow* window)
{    
    glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(sgl_default_ogl.program_default);
        
    //Draw Commands

    //NOTE: Triangle Example
    glBindBuffer(GL_ARRAY_BUFFER, sgl_default_ogl.vertex_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);    
    glDrawArrays(GL_TRIANGLES,0, 3);
    glDisableVertexAttribArray(0);

    //End Draw Commands
    glUseProgram(0);    

#ifdef _WIN32
    SwapBuffers(window->device_context); 
#else
    //@TODO: Other OS
   #error No other OS defined!
#endif //_WIN32 

}

#endif // SGL_DEFAULT_EXAMPLE

//[END Default Example] ---------------------

#endif // SIMPLE_OGL_IMPLEMENTATION





//

/*
The MIT License (MIT)

Copyright (c) 2018 Filipe Rasoilo www.rasoilo.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/