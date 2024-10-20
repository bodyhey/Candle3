
    const vertexShaderSource = `

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform vec3 u_light_position;

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

    v_color = a_color;
}

    `;

    const fragmentShaderSource = `

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

uniform sampler2D texture;
uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform vec3 eye;
varying vec3 v_world_pos;   // Pozycja punktu w przestrzeni świata
uniform vec3 u_light_color;
uniform vec3 u_object_color;

uniform vec3 light_color;
uniform vec3 object_color;

// bool isNan(float val)
// {
//     return (val > 65535.0);
// }

void main()
{
    // Draw dash lines
    // if (!isNan(v_start.x)) {
    //     vec2 sub = v_position - v_start;
    //     float coord = length(sub.x) > length(sub.y) ? gl_FragCoord.x : gl_FragCoord.y;
    //     if (cos(coord / factor) > 0.0) discard;
    // }

    // Set fragment color
//     if (!isNan(v_texture.x)) {
//         gl_FragColor = texture2D(texture, v_texture);
//     } else
//     {
//         //vec3 eye2 = (mvp_matrix * vec4(eye, 1.0)).xyz;
//         vec4 p_view = (mv_matrix * vec4(v_world_pos, 1.0)); // Pozycja punktu w przestrzeni widoku

//         //float distance = length(v_world_pos - eye2) * 0.1; // Obliczenie odległości
// //        float distance = abs(p_view.z); // Obliczenie odległości
//         ///float distance = abs(p_view) * 0.04; // Obliczenie odległości

//         // Efekt mgły
//         // float fogDensity = 0.0011;
//         // float fogFactor = gl_FragCoord.z * 1.0;// exp(-distance * 0.001);
//         // //float fogFactor = distance * fogDensity;
//         //fogFactor = clamp(fogFactor, 0.0, 1.0);

//         vec4 fogColor = vec4(0.0, 1.0, 1.0, 1.0); // Kolor mgły
//         //gl_FragColor = mix(fogColor, v_color, fogFactor); // Mieszanie ko

//         gl_FragColor = v_color;
//     }


    // v2

    // // Normalizacja wektorów
    // vec3 normal = normalize(v_normal);
    // vec3 light_dir = normalize(v_light_direction);

    // // // Światło rozproszone (diffuse)
    // float diff = max(dot(normal, light_dir), 0.0);

    // // // Końcowy kolor: ambient + diffuse
    // vec3 ambient = 0.1 * u_object_color;
    // vec3 diffuse = diff * u_light_color * u_object_color;

    // vec3 final_color = ambient + diffuse;
    // gl_FragColor = vec4(final_color, 1.0);

    // v3

    vec3 viewDir = normalize(v_position - eye);

    // Wektor normalny (iloczyn wektorowy wektora widzenia i kierunku linii)
    vec3 normal = normalize(cross(viewDir, v_normal));

    // Cieniowanie z użyciem normalnego wektora
    // Możesz tu dodać obliczenia oświetlenia np. Phonga
    // ...

    vec4 c1 = vec4(0.0, 0.0, 1.0, 1.0);
    vec4 c2 = vec4(1.0, 0.0, 0.0, 1.0);

    vec3 lightDir = vec3(0.0, 0.0, 1.0);
    vec4 baseColor = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 green = vec4(0.0, 1.0, 1.0, 1.0);

    //float brightness = max(dot(viewDir, v_normal) + 0.1, 0.0) * 1.0;
    //float brightness = mix(c1, c2, v_z);

    gl_FragColor = mix(baseColor, green, (v_z + 15.0 ) / 30.0);
    //vec4(baseColor * brightness, 1.0);
}
    `;
