
const vertexShaderSource1Cube = `

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform mat4 c_matrix;

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec3 a_normal;

varying vec4 v_color;

void main()
{
    gl_Position = mvp_matrix * c_matrix * vec4(a_position, 1.0);
    v_color = vec4(a_color, 1.0);
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
