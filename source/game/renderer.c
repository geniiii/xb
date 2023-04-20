internal void UpdateTransformMatrices(void) {
    glViewport(0, 0, xtal_os->window_size.width, xtal_os->window_size.height);
    renderer.ortho_projection = glms_ortho(0, (f32) xtal_os->window_size.width, (f32) xtal_os->window_size.height, 0, -1.0f, 1.0f);
}
