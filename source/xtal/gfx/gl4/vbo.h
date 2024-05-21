typedef struct {
    u32 handle;
} XGL_VBO;

typedef XGL_VBO XGL_EBO;

internal XGL_VBO XGL_VBONew(void);
internal void    XGL_VBOStorageFlags(XGL_VBO vbo, const void* data, GLuint size, GLenum flags);
internal void    XGL_VBOStorage(XGL_VBO vbo, const void* data, GLuint size);
internal void    XGL_VBODestroy(XGL_VBO vbo);
internal void*   XGL_VBOMapRange(XGL_VBO vbo, u32 start, u32 size, GLenum flags);
internal void    XGL_VBOUnmap(XGL_VBO vbo);
internal void    XGL_VBOBind(XGL_VBO vbo);
internal void    XGL_VBOUnbind(void);

internal void XGL_EBOBind(XGL_VBO vbo);
internal void XGL_EBOUnbind(void);
#define XGL_EBONew          XGL_VBONew
#define XGL_EBOStorageFlags XGL_VBOStorageFlags
#define XGL_EBOStorage      XGL_VBOStorage
#define XGL_EBODestroy      XGL_VBODestroy
#define XGL_EBOMapRange     XGL_VBOMapRange
#define XGL_EBOUnmap        XGL_VBOUnmap
