uniform mat4 mv_matrix;
uniform mat4 p_matrix;

attribute vec4 a_position;
attribute vec4 a_texcoord;
varying vec4 v_texcoord;
varying float originToVertexZ;
varying float z;

void main(void)
{
    z = a_position.z;
    vec4 esVert = mv_matrix * a_position;
    vec4 esObjOrigin = mv_matrix * vec4(0,0,0,1);
    originToVertexZ = esVert.z-esObjOrigin.z;
    gl_Position = p_matrix * esVert;
    v_texcoord = a_texcoord;
}
