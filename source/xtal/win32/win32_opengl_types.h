// NOTE(geni): Because I really don't feel like including gl.h, we define all the types here

typedef void         GLvoid;
typedef unsigned int GLenum;
#include "ext/KHR/khrplatform.h"
typedef khronos_float_t  GLfloat;
typedef int              GLint;
typedef int              GLsizei;
typedef unsigned int     GLbitfield;
typedef double           GLdouble;
typedef unsigned int     GLuint;
typedef unsigned char    GLboolean;
typedef khronos_uint8_t  GLubyte;
typedef char             GLchar;
typedef khronos_int16_t  GLshort;
typedef khronos_int8_t   GLbyte;
typedef khronos_uint16_t GLushort;
#define GL_FALSE 0
#define GL_TRUE  1
