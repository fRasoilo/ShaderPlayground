
//@TODO: Try to make this work in win32_get_file_timestamp
//#include <sys/stat.h> //__stat64 and _stat64


#define SIMPLE_OGL_IMPLEMENTATION
#define SGL_DEFAULT_EXAMPLE

#include "simple_ogl.h"



char* vert_shader_file_name = "test_tex.vert";
char* frag_shader_file_name = "test_tex.frag" ;

GLfloat quad_vertex_positions[] = {
    //Pos               //UV
   -0.5f, -0.5f, 0.0f,  0.0, 0.0,
   0.5f, -0.5f, 0.0f,   1.0, 0.0,
   0.5f,  0.5f, 0.0f,   1.0, 1.0,

   -0.5f,  0.5f, 0.0f,  0.0, 1.0,
   -0.5f, -0.5f, 0.0f,  0.0, 0.0,
   0.5f,  0.5f, 0.0f,   1.0, 1.0,


};

typedef uint64_t uint64;

//Helper
void string_copy(char* from, char* to)
{
    while(*from)
    {
        *to++ = *from++;
    }
    *to = '\0';
}


//Global Variables
global_variable SGLWindow main_window = {};

struct gl_renderer
{    
    GLuint index_buffer;
    GLuint vertex_buffer;
    
    //Texture
    GLuint texture_id;
    void* texture = 0;
    int32 texture_width = 0;
    int32 texture_height = 0;

    //Programs
    GLuint program_default; 
};
gl_renderer ogl_renderer = {};

//Internal shader representaiton
#define MAX_SHADERS 100
#define MAX_PROGRAMS 100

GLuint
setup_texture(void* texture, int32 width, int32 height)
{
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    #define GL_CLAMP_TO_EDGE                  0x812F
    #define GL_CLAMP_TO_BORDER                0x812D

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width,height, 0,GL_RGBA, GL_UNSIGNED_BYTE, texture);
    
    return(tex_id);
}

struct Program;

struct Shader
{
    char file_name[MAX_PATH];
    GLenum type;
    GLuint handle;
    uint64 hash;

    //Win32
    FILETIME timestamp;

    //Program that use this shader
    Program *programs[MAX_PROGRAMS];
    uint32 num_programs;

};


//Forward Declares
internal LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM WParam, LPARAM LParam);
internal void process_msgs(void);

struct loaded_file
{
    void* contents;
    int size;
};

bool32 win32_load_file_data(char *file_name, loaded_file *loaded_file, bool32 debug = 0);
void win32_free_loaded_file(loaded_file *loaded_file);
FILETIME win32_get_file_timestamp(char* file_name);
bool32 win32_check_shader_and_update(Shader *shader);


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

//@TODO: Erorr logging if shader_file is blank
GLuint create_shader_inline(const char* shader_file, GLenum shader_type, GLuint prev_shader = 0)
{
    GLuint shader = prev_shader;
    if(!shader)
    {
        shader = glCreateShader(shader_type);
    }
    
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

//@TODO: Error logging if file not found.
GLuint create_shader_from_file(char* file_name, GLenum shader_type)
{
    loaded_file file_to_load = {};
    win32_load_file_data(file_name, &file_to_load);
    GLuint shader = create_shader_inline((char*)file_to_load.contents, shader_type);
    return(shader);
}

GLuint recompile_shader_from_file(char* file_name, GLenum shader_type, GLuint prev_shader)
{
    loaded_file file_to_load = {};
    bool32 loaded = win32_load_file_data(file_name, &file_to_load, 1);
    while(!loaded)
    {
        loaded = win32_load_file_data(file_name, &file_to_load, 1);
    }
    GLuint shader = create_shader_inline((char*)file_to_load.contents, shader_type, prev_shader);
    return(shader);
}

bool32 link_program(Shader* shader_list, uint32 size, GLuint program, char* debug_name)
{
    bool32 success = true;

    for(size_t index = 0; index < size; ++index)
        {
            glAttachShader(program, shader_list[index].handle);
            }
        glLinkProgram(program);

        GLint status;
        glGetProgramiv (program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            success = false;
            
            GLint info_log_length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

            GLchar *string_info_log = new GLchar[info_log_length + 1];
            glGetProgramInfoLog(program, info_log_length, NULL, string_info_log);
                
            fprintf(stderr, "Linker failure in Program [%s]: %s\n",debug_name,  string_info_log);

            delete(string_info_log);
        }
        for(size_t index = 0; index < size; ++index)
        {
            glDetachShader(program, shader_list[index].handle);
        }

    return(success);
}

GLuint create_program(Shader* shader_list, int size, char* debug_name)
{
    GLuint program = glCreateProgram();
    link_program(shader_list, size, program,  debug_name);
    return program;
}



//Convenience struct for creating programs.
struct FileShaderType
{
    char* file_name;
    GLenum type;
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
    char name[MAX_PATH];
    uint64 hash;
    GLuint handle;
    Shader *shader_list[NUM_SHADERS_IN_PROGRAM];
    uint32 shader_count;
    bool32 need_update;
};


struct ShaderSystem
{
    uint32 num_shaders;
    Shader shaders[MAX_SHADERS];

    uint32 num_programs;
    Program programs[MAX_PROGRAMS];

    //Can change at any time, and is cleared out after update_programs finishes
    uint32 num_to_update;
    Program *programs_to_update[MAX_PROGRAMS];

    void add_program(FileShaderType *file_type_pair, uint32 count, char* program_name)
    {
        Program new_program = {};
        string_copy(program_name,new_program.name);
        new_program.hash = HASH(program_name);
        new_program.shader_count = count;

        for(uint32 i = 0; i < count; ++i)
        {
            //@TODO: Check if this shader exists in the system
            // If yes then retrieve the handle 
            //      Add it to the program shader list
            //      Add this program to its program list
            //      Increment the number of programs that use it

            //If no then go here
            //New Shader to add to the system
            Shader shader = {};
            string_copy(file_type_pair[i].file_name, shader.file_name);
            shader.type =  file_type_pair[i].type;
            shader.handle = create_shader_from_file(shader.file_name,shader.type);
            shader.hash = HASH(file_type_pair[i].file_name);
            shader.timestamp = win32_get_file_timestamp(file_type_pair[i].file_name);
            shader.programs[shader.num_programs] = &programs[num_programs];
            shader.num_programs++;

            //Add the shader to the system
            shaders[num_shaders] = shader;
            //Add shader to program
            new_program.shader_list[i] = &shaders[num_shaders];
            ++num_shaders;
        }
        new_program.handle = create_program(new_program.shader_list[0], new_program.shader_count, program_name);
        programs[num_programs++] = new_program;
    }

#if 0
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
#endif

    GLuint * get_program(char* program_name)
    {
        GLuint *result = 0;
        uint64 lookup_hash = HASH(program_name);
        for(uint32 index = 0; index < num_programs; ++index)
        {
            if(programs[index].hash == lookup_hash)
            {
                result = &programs[index].handle;
                break;
            }
        }
        return(result);
    }

    void update_programs()
    {
        
        //Check if any shaders have been changed
        for(uint32 i = 0; i < num_shaders; ++i)
        {
            Shader *curr_shader = &shaders[i];

            bool32 modified = win32_check_shader_and_update(curr_shader);
            if(modified)
            {
                //
                printf("Shader at index: %d has changed", i);

                //Recompile shader
                curr_shader->handle = recompile_shader_from_file(curr_shader->file_name, curr_shader->type, curr_shader->handle);
                
                //Set all programs that use this shader to need_update and add them to the 
                //programs_to_update array
                for(uint32 j = 0; j < curr_shader->num_programs; ++j)
                {
                    curr_shader->programs[j]->need_update = true;
                    programs_to_update[num_to_update] = curr_shader->programs[j];
                    ++num_to_update; 
                }
            }
        }

        //Re-link programs in programs_to_update,Reset their need to update var
        for(uint32 i = 0; i < num_to_update; ++i)
        {
            Program *program_to_update = programs_to_update[i];
            //@TODO: What if the program was not relinked succesfully?? Deal with it.
            link_program(program_to_update->shader_list[0], program_to_update->shader_count,
                         program_to_update->handle, program_to_update->name);
            program_to_update->need_update = false;
        }
        //Reset programs to update and num_to_update
        num_to_update = 0;
    }
};

ShaderSystem shader_system = {};



internal void init_test_program()
{
    #if 1 
  
    FileShaderType shaders[2] = 
    {
        {vert_shader_file_name,GL_VERTEX_SHADER},
        {frag_shader_file_name,GL_FRAGMENT_SHADER}
    };

    shader_system.add_program(&shaders[0], 2, "test_program");
    ogl_renderer.program_default = *shader_system.get_program("test_program");
  #endif

   
}
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
    glGenBuffers(1, &ogl_renderer.index_buffer);
    //InitVertexBuffer(&sgl_default_ogl.VertexBuffer,triangle_vertex_positions,ArrayCount(triangle_vertex_positions));    
    //

    int32 length = sizeof(quad_vertex_positions) / sizeof(quad_vertex_positions[0]);
    int32 size = length*sizeof(quad_vertex_positions[0]);
    glGenBuffers(1, &ogl_renderer.vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, ogl_renderer.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 size,
                 quad_vertex_positions,
                 GL_STATIC_DRAW);    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    init_test_program();
    
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

static float global_dt = 0.0f;

void render()
{    
    glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    // -- Draw Commands --

    //Want to be able to choose program in two ways:
    //1) By passing in a program var that we store and manage, this is a ptr to GLuint that we initialize.
    //      glUseProgram(*ogl.program_default);
    //2) By choosing a program name and the shader shader_system gives us back the correct program ptr;
    //      glUseProgram(*get_program("default"));

    //ogl_renderer.program_default = *shader_system.get_program("test_program");
    glUseProgram(ogl_renderer.program_default);
    

    //NOTE: Quad Example
    #if 1
    GLuint time_uniform = glGetUniformLocation(ogl_renderer.program_default, "time_in");
    glUniform1f(time_uniform, global_dt);

    glBindBuffer(GL_ARRAY_BUFFER, ogl_renderer.vertex_buffer);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1); //UV

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, 0);    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(3* sizeof(float)) ); //UV
        
    glBindTexture(GL_TEXTURE_2D, ogl_renderer.texture_id);
    glDrawArrays(GL_TRIANGLES,0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    #endif
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
        shader_system.update_programs();
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
win32_load_file_data(char *file_name, loaded_file *loaded_file, bool32 debug)
{
    
    bool32 loaded  = false;
    HANDLE file_handle = CreateFileA(file_name,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING, 0, 0);

    if(debug)
    {
        int x = 0;
        DWORD error = GetLastError();
        int break_here = 0;
    }

    if(file_handle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        if(GetFileSizeEx(file_handle, &file_size))
        {
            unsigned int file_size_32 = (int)(file_size.QuadPart);
            //@TODO: Change to malloc and call free!!!
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
    else
    {
        int x = 5; //break here
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


FILETIME 
win32_get_file_timestamp(char* file_name)
{
    #if 0 
    struct __stat64 file_info = {};
    _stat64(file_name, &file_info);
    return(file_info.st_mtime);
    #endif

    HANDLE file_handle = CreateFile(file_name,0,0,0,OPEN_EXISTING,0,0);
    FILETIME last_write_time = {};
    GetFileTime(file_handle,0,0,&last_write_time);
    return(last_write_time);
}

bool32 
win32_check_shader_and_update(Shader* shader)
{
    //Check
    bool32 modified = false;
    HANDLE file_handle = CreateFile(shader->file_name,0,0,0,OPEN_EXISTING,0,0);
    FILETIME last_write_time = {};
    GetFileTime(file_handle,0,0,&last_write_time);
    modified = CompareFileTime(&shader->timestamp,&last_write_time);
    //Update
    if(modified)
    {
        shader->timestamp = last_write_time;
    }
    CloseHandle(file_handle);
    return(modified);
}

//
//
//
//