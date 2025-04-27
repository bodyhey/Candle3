#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
uniform sampler2D u_palette;

varying vec4 v_color;

void main()
{
    gl_FragColor = v_color;
}
