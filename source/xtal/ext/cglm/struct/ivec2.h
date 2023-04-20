/*!
 * @brief add a vector to b vector store result in dest
 *
 * @param[in]  a    vector1
 * @param[in]  b    vector2
 * @returns         destination vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_add(ivec2s a, ivec2s b) {
	ivec2s r;
	glm_ivec2_add(a.raw, b.raw, r.raw);
	return r;
}

/*!
 * @brief add scalar to v vector store result in dest (d = v + s)
 *
 * @param[in]  a    vector
 * @param[in]  s    scalar
 * @returns         destination vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_adds(ivec2s a, float s) {
	ivec2s r;
	glm_ivec2_adds(a.raw, s, r.raw);
	return r;
}

/*!
 * @brief subtract b vector from a vector store result in dest
 *
 * @param[in]  a    vector1
 * @param[in]  b    vector2
 * @returns         destination vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_sub(ivec2s a, ivec2s b) {
	ivec2s r;
	glm_ivec2_sub(a.raw, b.raw, r.raw);
	return r;
}

/*!
 * @brief subtract scalar from v vector store result in dest (d = v - s)
 *
 * @param[in]  a    vector
 * @param[in]  s    scalar
 * @returns         destination vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_subs(ivec2s a, float s) {
	ivec2s r;
	glm_ivec2_subs(a.raw, s, r.raw);
	return r;
}

/*!
 * @brief multiply two vector (component-wise multiplication)
 *
 * @param     a     vector1
 * @param     b     vector2
 * @returns         v3 = (a[0] * b[0], a[1] * b[1], a[2] * b[2])
 */
CGLM_INLINE
ivec2s
glms_ivec2_mul(ivec2s a, ivec2s b) {
	ivec2s r;
	glm_ivec2_mul(a.raw, b.raw, r.raw);
	return r;
}

/*!
 * @brief multiply/scale ivec2 vector with scalar: result = v * s
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @returns         destination vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_scale(ivec2s v, float s) {
	ivec2s r;
	glm_ivec2_scale(v.raw, s, r.raw);
	return r;
}

/**
 * @brief distance between two vectors
 *
 * @param[in] a vector1
 * @param[in] b vector2
 * @return      distance
 */
CGLM_INLINE
float
glms_ivec2_distance(ivec2s a, ivec2s b) {
	return glm_ivec2_distance(a.raw, b.raw);
}