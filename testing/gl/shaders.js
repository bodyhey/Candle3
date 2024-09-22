
    const vertexShaderSource = `

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform vec3 eye;

attribute vec4 a_position;
attribute vec4 a_color;
attribute vec4 a_start;
attribute float a_alpha;

varying vec4 v_color;
varying vec2 v_position;
varying vec2 v_start;
varying vec2 v_texture;
varying vec3 vWorldPosition;

bool isNan(float val)
{
    return (val > 65535.0);
}

void main()
{
    // Calculate interpolated vertex position & line start point
    // if (isNan(a_position.x))  {
    //     v_position = (mv_matrix * vec4(eye, 0.0)).xy;
    // } else {
    //     v_position = (mv_matrix * a_position).xy;
    // }

    v_position = (mv_matrix * a_position).xy;
    vWorldPosition = a_position.xyz;

    if (!isNan(a_start.x) && !isNan(a_start.y)) {
        v_start = (mv_matrix * a_start).xy;
        v_texture = vec2(65536.0, 0);
    } else {
        // v_start.x should be Nan to draw solid lines
        v_start = a_start.xy;

        // set texture coord
        v_texture = a_start.yz;

        // set point size
        if (isNan(a_start.y) && !isNan(a_start.z)) gl_PointSize = a_start.z;
    }

    // if (isNan(a_position.x)) {
    //     gl_Position = vec4(-110, -10, 0, 1) * mvp_matrix;
    // } else {
    // }
        // Calculate vertex position in screen space
        gl_Position = mvp_matrix * a_position;

    // float d = distance(a_position.xyz, eye.xyz) / 10000.0;
    // float alpha = getFogFactor(d);

    float distance = (length(a_position.xyz - eye) / 150.0);

    float fogDensity = 0.05;
    float fogFactor = exp(-distance * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // if (distance < 10.0) {
    //     fogFactor = 1.0;
    // } else {
    //     fogFactor = 0.0;
    // }

    v_color = a_color;
    //v_color = vec4(d, d, d, d2);

    // vec4 fogColor = vec4(1.0, 0.0, 0.0, 1.0); // Kolor mgły
    // vec4 objectColor = vec4(0.0, 1.0, 0.0, 1.0); // Kolor obiektu

    // v_color = mix(fogColor, objectColor, fogFactor); // Mieszanie kolorów w zależności od odległości

    //v_color.a = d;
}

    `;

    const fragmentShaderSource = `

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

//Dash grid (px) = factor * pi;
const float factor = 2.0;

varying vec4 v_color;
varying vec2 v_position;
varying vec2 v_start;
varying vec2 v_texture;

uniform sampler2D texture;
uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform vec3 eye;
varying vec3 vWorldPosition;   // Pozycja punktu w przestrzeni świata

bool isNan(float val)
{
    return (val > 65535.0);
}

void main()
{
    // Draw dash lines
    if (!isNan(v_start.x)) {
        vec2 sub = v_position - v_start;
        float coord = length(sub.x) > length(sub.y) ? gl_FragCoord.x : gl_FragCoord.y;
        if (cos(coord / factor) > 0.0) discard;
    }

    // Set fragment color
    if (!isNan(v_texture.x)) {
        gl_FragColor = texture2D(texture, v_texture);
    } else
    {
        vec3 eye2 = (mvp_matrix * vec4(eye, 1.0)).xyz;

        float distance = length(vWorldPosition - eye2) * 0.01; // Obliczenie odległości

        // Efekt mgły
        float fogDensity = 0.3;
        float fogFactor = exp(-distance * fogDensity);
        fogFactor = clamp(fogFactor, 0.0, 1.0);

        vec4 fogColor = vec4(1.0, 1.0, 1.0, 1.0); // Kolor mgły
        // vec4 objectColor = vec4(1.0, 1.0, 1.0, 1.0); // Kolor obiektu

        gl_FragColor = mix(fogColor, v_color, fogFactor); // Mieszanie ko

        // gl_FragColor = v_color;
    }
}

    `;
