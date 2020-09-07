uniform mat4 mvp_matrix;

attribute vec4 a_position;
attribute vec4 a_texcoord;
varying vec4 v_texcoord;

void main(void)
{
    gl_Position = mvp_matrix * a_position;
    v_texcoord = a_texcoord;
}
