#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec2 v_texcoord;
uniform sampler2D u_texture;

void main()
{
    gl_FragColor = texture2D(u_texture, v_texcoord);
}
