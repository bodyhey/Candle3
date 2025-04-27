
const vertexShaderSource1Cube = `

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform mat4 c_matrix;
uniform sampler2D u_palette;

attribute vec3 a_position;
attribute float a_color;
attribute vec3 a_normal;

varying vec4 v_color;

void main()
{
    gl_Position = mvp_matrix * c_matrix * vec4(a_position, 1.0);
//    v_color = vec4(a_color, 1.0);
    v_color = texture2D(u_palette, vec2(a_color * (1.0 / 10.0), 0.0));
}

    `;

const fragmentShaderSource1Cube = `

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec4 v_color;

void main()
{
    gl_FragColor = v_color;
}
    `;
