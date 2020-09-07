uniform sampler2D texture;
uniform vec4 a_color;
varying vec4 v_texcoord;

void main(void)
{
    //gl_FragColor = texture2D(texture, v_texcoord.st)+a_color;
    gl_FragColor = a_color;
}
