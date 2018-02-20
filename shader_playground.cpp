#define SIMPLE_OGL_IMPLEMENTATION
#define SGL_DEFAULT_EXAMPLE

#include "simple_ogl.h"

typedef uint64_t uint64;

//Global Variables
global_variable SGLWindow main_window = {};

struct gl_renderer
{    
    GLuint index_buffer;
    GLuint vertex_buffer;
    
    //Programs
    GLuint program_default; 
};


//Forward Declares
internal LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM WParam, LPARAM LParam);
internal void process_msgs(void);

struct loaded_file
{
    void* contents;
    int size;
};

bool32 win32_load_file_data(char *file_name, loaded_file *loaded_file);
void win32_free_loaded_file(loaded_file *loaded_file);

#define HASH(str) djb2_hash(str)
unsigned long djb2_hash(char* str)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }    
    return (hash);
}

//Shader and Program Creation
struct Shader
{
    char file_name[MAX_PATH];
    GLuint handle;
    GLenum type;
    uint64 hash;
    uint64 time_stamp;
};

// VERTEX_SHADER
// FRAGMENT_SHADER
// GEOMETRY_SHADER
// TESS_CONTROL_SHADER
// TESS_EVALUATION_SHADER
// COMPUTE_SHADER
#define NUM_SHADERS_IN_PROGRAM 6
struct Program
{
    uint64 hash;
    GLuint handle;
    GLuint *shader_list[NUM_SHADERS_IN_PROGRAM];
    uint32 shader_count;
    bool32 need_update;
};


#define MAX_SHADERS 100
#define MAX_PROGRAMS 100

struct ShaderSystem
{
    uint32 num_shaders;
    Shader shaders[MAX_SHADERS];

    uint32 num_programs;
    Program programs[MAX_PROGRAMS];

    //Can change at any time, and is cleared out after update_programs finishes
    uint32 num_to_update
    Program programs_to_update[MAX_PROGRAMS];

    void create_program(Shader *shader_list, uint32 count, char* program_name)
    {
        Program new_program = {};
        new_program.hash = HASH(program_name);
        new_program.shader_count = count;

        for(uint32 index = 0; index < count; ++index)
        {
            shaders[num_shaders] = shader_list[index];
            new_program.shader_list[index] = &shaders[num_shaders].handle;
            ++num_shaders;
        }
        new_program.handle = create_program(new_program.shader_list, count, program_name);
    }

    GLuint * get_program(char* program_name)
    {
        uint64 lookup_hash = HASH(program_name);
        for(uint32 index = 0; index < num_programs; ++index)
        {
            if(programs[index].hash == lookup_hash)
            {
                return(&programs[index].handle);
            }
        }
    }
};




GLuint create_shader_inline(const char* shader_file, GLenum shader_type)
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

GLuint create_shader_from_file(char* file_name, GLenum shader_type)
{
    loaded_file file_to_load = {};
    win32_load_file_data(file_name, &file_to_load);
    GLuint shader = create_shader_inline((char*)file_to_load.contents, shader_type);
    return(shader);
}


GLuint
create_program(GLuint* shader_list, int size, char* debug_name)
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

#if 0
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
#endif
//
//

void
reshape(int32 width, int32 height)
{
    glViewport(0, 0, width, height);
}

void init_gl_state()
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

void render()
{    
    glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    // -- Draw Commands --

    //Want to be able to choose program in two ways:
    //1) By passing in a program var that we store and manage, this is a ptr to GLuint that we initialize.
    //      glUseProgram(*ogl.program_default);
    //2) By choosing a program name and the shader system gives us back the correct program ptr;
    //      glUseProgram(*get_program("default"));


    //NOTE: Triangle Example
    /*
    glBindBuffer(GL_ARRAY_BUFFER, sgl_default_ogl.vertex_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);    
    glDrawArrays(GL_TRIANGLES,0, 3);
    glDisableVertexAttribArray(0);
    */

    //-- End Draw Commands --


    glUseProgram(0);    

#ifdef _WIN32
    SwapBuffers(main_window.device_context); 
#else
    //@TODO: Other OS
   #error No other OS defined!
#endif //_WIN32 

}



int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{

    sgl_window(&main_window,Instance,window_callback);
    
    //Default Example
    init_gl_state();
    //Loop
    while(main_window.running)
    {
        process_msgs();
        render();
    }   
    return 0;
}








//
//
//
//Win32 Functions
internal LRESULT CALLBACK
window_callback(HWND window, UINT message, WPARAM WParam, LPARAM LParam)
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
            main_window.running = false;
        }break;
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);            
            RECT main_rect;
            GetClientRect(main_window.handle, &main_rect);
            EndPaint(window, &paint);                   
        }break;
        case WM_SIZE:
        {
            int32 width = (int) LOWORD(LParam);
            int32 height = (int) HIWORD(LParam);
            reshape(width, height);
        }break;
        case WM_MOUSEMOVE:
        {
        }break;
        case WM_KEYDOWN:
        {
            switch(WParam)
            {
                case 0x0D:
                {
                    sgl_win32_window_toggle_fullscreen(&main_window);
                }
                break;
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
process_msgs(void)
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


bool32
win32_load_file_data(char *file_name, loaded_file *loaded_file)
{
    bool32 loaded  = false;
    HANDLE file_handle = CreateFileA(file_name,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING, 0, 0);
    if(file_handle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        if(GetFileSizeEx(file_handle, &file_size))
        {
            unsigned int file_size_32 = (int)(file_size.QuadPart);
            loaded_file->contents = VirtualAlloc(0, file_size_32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(loaded_file->contents)
            {
                DWORD bytes_read;
                if(ReadFile(file_handle, loaded_file->contents, file_size_32, &bytes_read, 0) &&
                   (file_size_32 == bytes_read))
                {
                    loaded_file->size = file_size_32;
                    loaded = true;
                }        
            }
        }
        CloseHandle(file_handle);
    }
    return(loaded);
}

void
win32_free_loaded_file(loaded_file *loaded_file)
{
    if(loaded_file->contents)
    {
        VirtualFree(loaded_file->contents, 0, MEM_RELEASE);
    }
}

//
//
//
//