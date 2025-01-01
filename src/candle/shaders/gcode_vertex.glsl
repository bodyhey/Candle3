#version 150

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 u_mvp_matrix;
uniform mat4 u_mv_matrix;
uniform vec3 u_light_position;
uniform bool u_shadow;
uniform vec3 u_eye;

in vec3 a_position;
in vec3 a_color;
in vec3 a_normal;
in float a_alpha;

out vec4 v_color;
out vec3 v_position;
out vec2 v_start;
out vec3 v_normal;
out vec2 v_texture;
out vec3 v_light_direction;
out vec3 v_eye;

// bool isNan(float val)
// {
//     return (val > 65535.0);
// }

void main()
{
    vec4 vertex_position = vec4(a_position, 1.0);
    v_normal = a_normal;

    vec3 light_position_ = vec4(u_light_position, 1.0).xyz;
    v_light_direction = normalize(light_position_ - vertex_position.xyz);

    gl_Position = u_mvp_matrix * vertex_position;

    v_position = vertex_position.xyz;

    v_color = vec4(a_color, 1.0);
    v_eye = (vec4(u_eye, 1.0) * u_mvp_matrix).xyz;
}
