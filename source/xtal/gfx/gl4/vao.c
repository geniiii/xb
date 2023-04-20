typedef struct XGL_VAO {
    u32 handle;
} XGL_VAO;

internal XGL_VAO XGL_VAONew(void) {
    XGL_VAO vao;
    glCreateVertexArrays(1, &vao.handle);
    return vao;
}
internal void XGL_VAODestroy(XGL_VAO vao) {
    glDeleteVertexArrays(1, &vao.handle);
}

internal void XGL_VAOBind(XGL_VAO vao) {
    glBindVertexArray(vao.handle);
}
internal void XGL_VAOUnbind(void) {
    glBindVertexArray(0);
}
internal void XGL_VAOAttachVBO(XGL_VAO vao, XGL_VBO vbo, GLint stride) {
    glVertexArrayVertexBuffer(vao.handle, 0, vbo.handle, 0, stride);
}
internal void XGL_VAOAttachEBO(XGL_VAO vao, XGL_EBO ebo) {
    glVertexArrayElementBuffer(vao.handle, ebo.handle);
}

internal void _XGL_VAOSetupAttrib(XGL_VAO vao, GLuint index) {
    glEnableVertexArrayAttrib(vao.handle, index);
    glVertexArrayAttribBinding(vao.handle, index, 0);
}
internal void XGL_VAOAttrInteger(XGL_VAO vao, GLuint index, GLint size, GLenum type, GLuint offset) {
    _XGL_VAOSetupAttrib(vao, index);
    glVertexArrayAttribIFormat(vao.handle, index, size, type, offset);
}
internal void XGL_VAOAttr(XGL_VAO vao, GLuint index, GLint size, GLenum type, GLuint offset) {
    _XGL_VAOSetupAttrib(vao, index);
    glVertexArrayAttribFormat(vao.handle, index, size, type, GL_FALSE, offset);
}
internal void XGL_VAOAttrNormalized(XGL_VAO vao, GLuint index, GLint size, GLenum type, GLuint offset) {
    _XGL_VAOSetupAttrib(vao, index);
    glVertexArrayAttribFormat(vao.handle, index, size, type, GL_TRUE, offset);
}
