#version 130

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 u_mvp_matrix;
uniform mat4 u_mv_matrix;
uniform vec3 u_light_position;
//uniform bool u_shadow;
uniform vec3 u_eye;
uniform sampler2D u_palette;
uniform float u_near;
uniform float u_far;

in vec3 a_position;
in float a_color;
in vec3 a_start;

out vec4 v_color;
out vec2 v_start;
out vec3 v_normal;
out vec3 v_light_direction;
out vec3 v_eye;
out float v_zpos;
out float v_log_depth;

// bool isNan(float val)
// {
//     return (val > 65535.0);
// }

void main()
{
    vec4 vertex_position = vec4(a_position, 1.0);
    v_normal = a_start;

    vec3 light_position_ = vec4(u_light_position, 1.0).xyz;
    v_light_direction = normalize(light_position_ - vertex_position.xyz);

    // our world is rotated so we use Y axis as depth
    v_zpos = (u_mv_matrix * vertex_position).z;

    vec4 pos = u_mvp_matrix * vertex_position;

    float logDepth = log(pos.z / u_near + 1.0) / log(u_far / u_near + 1.0);

    // Zapis do gl_Position
    gl_Position = pos;
//    gl_Position.z = (2.0 * logDepth - 1.0) * gl_Position.w;

    v_color = texture2D(u_palette, vec2(a_color * (1.0 / 25.0) + (1.0 / 50.0), 0.0));
    v_eye = (vec4(u_eye, 1.0) * u_mvp_matrix).xyz;
}
