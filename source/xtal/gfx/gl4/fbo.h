typedef struct XGL_FBO {
    u32 handle;
} XGL_FBO;

internal XGL_FBO XGL_FBONew(void);
internal void    XGL_FBODestroy(XGL_FBO fbo);
internal void    XGL_FBOBind(XGL_FBO fbo);
internal void    XGL_FBOUnbind(void);
internal void    XGL_FBOTexture(XGL_FBO fbo, XGL_Tex texture, GLenum attachment);
internal void    XGL_FBOAttachVBO(XGL_FBO fbo, XGL_VBO vbo, GLint stride);
internal void    XGL_FBOAttachEBO(XGL_FBO fbo, XGL_EBO ebo);
