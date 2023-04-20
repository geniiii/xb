typedef struct {
    u32 handle;
} XGL_VBO;

typedef XGL_VBO XGL_EBO;

internal XGL_VBO XGL_VBONew(void) {
    XGL_VBO vbo = {0};
    glCreateBuffers(1, &vbo.handle);
    return vbo;
}
#define XGL_EBONew XGL_VBONew

internal void XGL_VBOStorageFlags(XGL_VBO vbo, const void* data, GLuint size, GLenum flags) {
    glNamedBufferStorage(vbo.handle, size, data, flags);
}
#define XGL_EBOStorageFlags XGL_VBOStorageFlags
internal void XGL_VBOStorage(XGL_VBO vbo, const void* data, GLuint size) {
    XGL_VBOStorageFlags(vbo, data, size, GL_DYNAMIC_STORAGE_BIT);
}
#define XGL_EBOStorage XGL_VBOStorage

internal void XGL_VBODestroy(XGL_VBO vbo) {
    glDeleteBuffers(1, &vbo.handle);
}
#define XGL_EBODestroy XGL_VBODestroy

internal void* XGL_VBOMapRange(XGL_VBO vbo, u32 start, u32 size, GLenum flags) {
    return glMapNamedBufferRange(vbo.handle, start, size, flags);
}
#define XGL_EBOMapRange XGL_VBOMapRange

internal void XGL_VBOUnmap(XGL_VBO vbo) {
    glUnmapNamedBuffer(vbo.handle);
}
#define XGL_EBOUnmap XGL_VBOUnmap

internal void XGL_VBOBind(XGL_VBO vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo.handle);
}
internal void XGL_EBOBind(XGL_VBO vbo) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.handle);
}

internal void XGL_VBOUnbind(void) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
internal void XGL_EBOUnbind(void) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
