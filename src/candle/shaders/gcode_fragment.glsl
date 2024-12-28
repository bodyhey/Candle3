#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec3 v_position;
varying vec3 v_normal;
varying vec3 v_light_direction;
varying vec4 v_color;
uniform bool u_shadow;
uniform vec3 u_eye;

void main()
{
    if (u_shadow) {
        gl_FragColor = vec4(0.2, 0.2, 0.2, 1.0);
        return;
    }

    vec4 object_color = v_color;// vec3(0.7, 0.2, 0.8);
    vec4 light_color = vec4(1.0, 1.0, 0.0, 1.0);
    // if (object_color.x == 0.0 && object_color.y == 0.0 && object_color.z == 0.0) {
    //     gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    //     return;
    // }

    vec3 viewDir = normalize(v_position);// - u_eye);
    vec3 normal = normalize(cross(viewDir, v_normal));
    vec3 lightDir = normalize(v_light_direction);
    float diff = abs(dot(normal, lightDir)) * 0.5;
    vec4 color = mix(object_color, light_color, diff);

    gl_FragColor = color;
}
