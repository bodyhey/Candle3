
const vertexShaderSource2 = `

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform vec3 u_light_position;
uniform vec3 u_eye;

attribute vec4 a_position;
attribute vec4 a_color;
attribute vec4 a_normal;
attribute float a_alpha;

varying vec4 v_color;
varying vec3 v_position;
varying vec2 v_start;
varying vec3 v_normal;
varying vec2 v_texture;
varying float v_z;
varying vec3 v_light_direction;

// bool isNan(float val)
// {
//     return (val > 65535.0);
// }

void main()
{
    v_z = a_position.z;

//    vec4 vertex_position = mv_matrix * vec4(a_position.xyz, 1.0);
    vec4 vertex_position = vec4(a_position.xyz, 1.0);
//    v_normal = (mv_matrix * a_normal).xyz;
    v_normal = a_normal.xyz;

    //vec3 light_position_ = (mv_matrix * vec4(u_light_position, 1.0)).xyz;
    vec3 light_position_ = vec4(u_light_position, 1.0).xyz;
    v_light_direction = normalize(light_position_ - vertex_position.xyz);

    gl_Position = mvp_matrix * vertex_position;
    v_position = vertex_position.xyz;

    float y = a_position.z + 25.0;
    // y = -25 to 25
    y += 20.0;
    y /= 40.0;
    
    v_color = mix(vec4(1.0, 0.0, 0.0, 0.5), vec4(0.0, 1.0, 0.0, 0.5), y);
    // a_color;
}

    `;

const fragmentShaderSource2 = `

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

//Dash grid (px) = factor * pi;
// const float factor = 2.0;

varying vec4 v_color;
varying vec3 v_position;
varying vec3 v_normal;
// varying vec2 v_texture;
varying vec3 v_light_direction;
varying float v_z;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform vec3 eye;
varying vec3 v_world_pos;   // Pozycja punktu w przestrzeni świata
uniform vec3 u_light_color;
uniform vec3 u_object_color;

uniform vec3 light_color;
uniform vec3 object_color;

void main()
{
    //vec4 green = vec4(v_color.xyz, 1);
    vec4 green = vec4(0.6, 0.6, 0.1, 0.7);

    gl_FragColor = v_color;
}
    `;
