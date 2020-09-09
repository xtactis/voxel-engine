uniform sampler2D texture;
uniform vec4 a_color;
varying vec4 v_texcoord;
varying float originToVertexZ;
varying float z;

float remap( float minval, float maxval, float curval )
{
    return ( curval - minval ) / ( maxval - minval );
}

const vec4 RED    = vec4( 1.0, 0.0, 0.0, 1.0 );
const vec4 PURPLE = vec4( 1.0, 0.0, 1.0, 1.0 );
const vec4 BLUE   = vec4( 0.0, 0.0, 1.0, 1.0 );
const vec4 TEAL   = vec4( 0.0, 1.0, 1.0, 1.0 );
const vec4 GREEN  = vec4( 0.0, 1.0, 0.0, 1.0 );
const vec4 YELLOW  = vec4( 1.0, 1.0, 0.0, 1.0 );


void main(void)
{
    float u = fract(z/4);
    if (u < 1./6) {
        gl_FragColor = mix(RED, PURPLE, remap(0.0, 1./6, u));
    } else if (u < 2./6) {
        gl_FragColor = mix(PURPLE, BLUE, remap(1./6, 2./6, u));
    } else if (u < 3./6) {
        gl_FragColor = mix(BLUE, TEAL, remap(2./6, 3./6, u));
    } else if (u < 4./6) {
        gl_FragColor = mix(TEAL, GREEN, remap(3./6, 4./6, u));
    } else if (u < 5./6) {
        gl_FragColor = mix(GREEN, YELLOW, remap(4./6, 5./6, u));
    } else {
        gl_FragColor = mix(YELLOW, RED, remap(5./6, 1.0, u));
    }
    gl_FragColor = gl_FragColor*0.8;
    //gl_FragColor = texture2D(texture, v_texcoord.st)+a_color;
    //gl_FragColor = a_color * 0.5*originToVertexZ/100+0.5;
}
