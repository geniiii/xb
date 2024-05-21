internal void _XGL_ShaderFatal(GLuint handle, String8 adverb, String8 name,
                               void (*log_fn)(GLuint, GLint, GLint*, char*),
                               void (*iv_fn)(GLuint, GLuint, GLint*)) {
    if (iv_fn && log_fn) {
        i32 log_size;
        iv_fn(handle, GL_INFO_LOG_LENGTH, &log_size);

        char* log_text = Xtal_MArenaPush(&xtal_os->frame_arena, log_size);
        log_fn(handle, log_size, NULL, log_text);

        xtal_os->OutputErrorMessage("Fatal Error", "Error %S shader \"%S\":\n%.*s", adverb, name, log_size, log_text);
    } else {
        xtal_os->OutputErrorMessage("Fatal Error", "Error %S shader \"%S\"", adverb, name);
    }
}

internal b32 XGL_ShaderCompileFromData(String8 data, GLenum type, GLuint* handle_ptr) {
    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar* const*) &data.s, (const GLint*) &data.size);
    glCompileShader(handle);
    *handle_ptr = handle;

    GLint result;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
    return result;
}

internal XGL_Shader XGL_ShaderNew(XGL_SType type) {
    XGL_Shader shader = {
        .type   = type,
        .handle = glCreateProgram(),
        .info   = &xgl_generated_shaders[type],
    };

    GLuint vs, fs;
    if (!XGL_ShaderCompileFromData(shader.info->vertex, GL_VERTEX_SHADER, &vs)) {
        _XGL_ShaderFatal(shader.handle, S8Lit("compiling vertex"), shader.info->name, glGetShaderInfoLog, glGetShaderiv);
    }
    if (!XGL_ShaderCompileFromData(shader.info->fragment, GL_FRAGMENT_SHADER, &fs)) {
        _XGL_ShaderFatal(shader.handle, S8Lit("compiling fragment"), shader.info->name, glGetShaderInfoLog, glGetShaderiv);
    }

    glAttachShader(shader.handle, vs);
    glAttachShader(shader.handle, fs);

    // NOTE(geni): Only necessary for old OpenGL versions
#if 0
	for (u32 i = 0; i < shader.input_count; ++i) {
		glBindAttribLocation(shader.handle, shader.inputs[i].loc, shader.inputs[i].name);
	}

	for (u32 i = 0; i < shader.output_count; ++i) {
		glBindAttribLocation(shader.handle, shader.outputs[i].loc, shader.outputs[i].name);
	}
#endif

    glLinkProgram(shader.handle);

    GLint success;
    glGetProgramiv(shader.handle, GL_LINK_STATUS, &success);
    if (!success) {
        _XGL_ShaderFatal(shader.handle, S8Lit("linking"), shader.info->name, glGetProgramInfoLog, glGetProgramiv);
    }

    glDeleteShader(vs);
    glDetachShader(shader.handle, vs);
    glDeleteShader(fs);
    glDetachShader(shader.handle, fs);

    glValidateProgram(shader.handle);

    // NOTE(geni): Set uniform locations
    for (u32 i = 0; i < shader.info->uniform_count; ++i) {
        shader.info->uniforms[i].loc = glGetUniformLocation(shader.handle, shader.info->uniforms[i].name.cstr);
        if (shader.info->uniforms[i].loc == -1) {
            _XGL_ShaderFatal(shader.handle, S8_PushF(&xtal_os->frame_arena, "getting location of uniform \"%S\" for", shader.info->uniforms[i].name), shader.info->name, NULL, NULL);
        }
    }

    return shader;
}

internal void XGL_ShaderBind(XGL_Shader shader) {
    glUseProgram(shader.handle);
}
internal void XGL_ShaderUnbind(void) {
    glUseProgram(0);
}

internal void XGL_ShaderDestroy(XGL_Shader shader) {
    glDeleteProgram(shader.handle);
}

internal void XGL_ShaderUniformF32(XGL_Shader shader, i32 id, f32 f) {
    glUniform1f(shader.info->uniforms[id].loc, f);
}
internal void XGL_ShaderUniformI32(XGL_Shader shader, i32 id, i32 i) {
    glUniform1i(shader.info->uniforms[id].loc, i);
}
internal void XGL_ShaderUniformU32(XGL_Shader shader, i32 id, u32 i) {
    glUniform1ui(shader.info->uniforms[id].loc, i);
}

internal void XGL_ShaderUniformVec2(XGL_Shader shader, i32 id, vec2s vec) {
    glUniform2f(shader.info->uniforms[id].loc, vec.x, vec.y);
}
internal void XGL_ShaderUniformVec3(XGL_Shader shader, i32 id, vec3s vec) {
    glUniform3f(shader.info->uniforms[id].loc, vec.x, vec.y, vec.z);
}
internal void XGL_ShaderUniformVec4(XGL_Shader shader, i32 id, vec4s vec) {
    glUniform4f(shader.info->uniforms[id].loc, vec.x, vec.y, vec.z, vec.w);
}

internal void XGL_ShaderUniformVec2i(XGL_Shader shader, i32 id, ivec2s vec) {
    glUniform2i(shader.info->uniforms[id].loc, vec.x, vec.y);
}
internal void XGL_ShaderUniformVec3i(XGL_Shader shader, i32 id, ivec3s vec) {
    glUniform3i(shader.info->uniforms[id].loc, vec.x, vec.y, vec.z);
}
internal void XGL_ShaderUniformVec4i(XGL_Shader shader, i32 id, ivec4s vec) {
    glUniform4i(shader.info->uniforms[id].loc, vec.x, vec.y, vec.z, vec.w);
}

internal void XGL_ShaderUniformMat4(XGL_Shader shader, i32 id, mat4s* mat) {
    glUniformMatrix4fv(shader.info->uniforms[id].loc, 1, 0, (const GLfloat*) mat);
}
