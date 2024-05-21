internal XGL_FBO XGL_FBONew(void) {
    XGL_FBO fbo;
    glCreateFramebuffers(1, &fbo.handle);
    return fbo;
}
internal void XGL_FBODestroy(XGL_FBO fbo) {
    glDeleteFramebuffers(1, &fbo.handle);
}

internal void XGL_FBOBind(XGL_FBO fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.handle);
}
internal void XGL_FBOUnbind(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

internal void XGL_FBOTexture(XGL_FBO fbo, XGL_Tex texture, GLenum attachment) {
    glNamedFramebufferTexture(fbo.handle, attachment, texture.handle, 0);
}

internal void XGL_FBOAttachVBO(XGL_FBO fbo, XGL_VBO vbo, GLint stride) {
    glVertexArrayVertexBuffer(fbo.handle, 0, vbo.handle, 0, stride);
}
internal void XGL_FBOAttachEBO(XGL_FBO fbo, XGL_EBO ebo) {
    glVertexArrayElementBuffer(fbo.handle, ebo.handle);
}
