#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 u_mvp_matrix;
uniform vec3 u_light_position;
uniform bool u_shadow;
uniform vec2 u_offset;

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec3 a_start;

varying vec3 v_position;
varying vec4 v_color;
varying vec2 v_start;
varying vec3 v_normal;
varying float v_z;
varying vec3 v_light_direction;

void main()
{
    vec4 vertex_position = vec4(a_position, 1.0);
    v_normal = (u_mvp_matrix * vec4(a_start, 1.0)).xyz;

    // vec3 u_light_position = vec3(0.0, -250.0, 111.0);

    // vec3 light_position_ = vec3(0.0, -250.0, 111.0);
    // light_position_ = vec3(
    //     light_position_.x * cos(u_light_rot) - light_position_.z * sin(u_light_rot),
    //     light_position_.y,
    //     light_position_.x * sin(u_light_rot) + light_position_.z * cos(u_light_rot)
    // );

    v_light_direction = vec3(5, -10, -4);

    gl_Position = u_mvp_matrix * vertex_position;

    if (a_position.x == 100.0 && a_position.y == 100.0) {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    };

    v_position = vertex_position.xyz;
    v_color = vec4(a_color, 1.0);
    if (u_shadow) {
        gl_Position += vec4(u_offset.x, u_offset.y, 0.0001, 0.0);
        //v_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
}
