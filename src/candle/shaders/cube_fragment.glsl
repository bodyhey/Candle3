#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec4 v_color;

uniform sampler2D u_palette;

void main()
{
    gl_FragColor = v_color;
}
