typedef struct XGL_VAO {
    u32 handle;
} XGL_VAO;

internal XGL_VAO XGL_VAONew(void);
internal void    XGL_VAODestroy(XGL_VAO vao);
internal void    XGL_VAOBind(XGL_VAO vao);
internal void    XGL_VAOUnbind(void);
internal void    XGL_VAOAttachVBO(XGL_VAO vao, XGL_VBO vbo, GLint stride);
internal void    XGL_VAOAttachEBO(XGL_VAO vao, XGL_EBO ebo);
internal void    XGL_VAOAttr(XGL_VAO vao, GLuint index, GLint size, GLenum type, GLuint offset);
internal void    XGL_VAOAttrNormalized(XGL_VAO vao, GLuint index, GLint size, GLenum type, GLuint offset);
