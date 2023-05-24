bool
CreateShader(const char ** VertexShaderSource, const char ** FragmentShaderSource)
{
    char Log[1024];
    int Success;
    GLint VS;
    GLint FS;
    //Vertex shader
    VS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VS, 1, VertexShaderSource, NULL);
    glCompileShader(VS);
    glGetShaderiv(VS, GL_COMPILE_STATUS, &Success);
    //Check if the vertex shader compiled
    if (!Success)
    {
        glGetShaderInfoLog(VS, 1024, NULL, Log);
        DEBUG_LOG(Log);
        return false;
    }
    //Fragment shader
    FS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FS, 1, FragmentShaderSource, NULL);
    glCompileShader(FS);
    glGetShaderiv(FS, GL_COMPILE_STATUS, &Success);
    //Check if the fragment shader compiled successfully
    if (!Success)
    {
        glGetShaderInfoLog(FS, 1024, NULL, Log);
        DEBUG_LOG(Log);
        return false;
    }
    GLint Program;
    Program = glCreateProgram();
    glAttachShader(Program, VS);
    glAttachShader(Program, FS);
    glLinkProgram(Program);
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    //Delete the shaders
    glDeleteShader(VS);
    glDeleteShader(FS);
    if (!Success)
    {
        glGetProgramInfoLog(Program, 1024, NULL, Log);
        DEBUG_LOG(Log);
        return false;
    }
    return true;
#if 0    
    // Get all the info about uniforms
    GLint i;
    GLint count;
    
    GLint size; // size of the variable
    GLsizei length;
    glGetProgramiv(shader->program, GL_ACTIVE_UNIFORMS, &count);
    if (GL_SHADER_MAX_UNIFORMS < count)
    {
        DEBUG_LOG("ERROR %s at line %d: More uniforms in shader then max allowed", __FILE__,__LINE__);
    }
    for (i = 0; i < (count <= GL_SHADER_MAX_UNIFORMS ? count : GL_SHADER_MAX_UNIFORMS); i++)
    {
        glGetActiveUniform(shader->program,
                           (GLuint)i,
                           GL_SHADER_UNIFORM_NAME_MAX_LENGTH,
                           &length,
                           &size,
                           &shader->uniforms[shader->number_of_uniforms].type,
                           shader->uniforms[shader->number_of_uniforms].name);
        shader->uniforms[shader->number_of_uniforms].location = glGetUniformLocation(shader->program, shader->uniforms[shader->number_of_uniforms].name);
        shader->number_of_uniforms++;
    }
#endif
}