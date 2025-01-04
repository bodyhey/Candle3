
const vertexShaderSource2d = `#version 300 es

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

in vec2 a_position;
in vec2 a_texcoord;

out vec3 v_position;
out vec2 v_texcoord;

void main()
{
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_texcoord = a_texcoord;
}

    `;

const fragmentShaderSource2d = `#version 300 es

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

in vec2 v_texcoord;
uniform sampler2D u_texture;
uniform sampler2D u_depthTexture;
uniform bool u_gcodeBgMode;
out vec4 fragColor;

void main()
{
    if (!u_gcodeBgMode) {
        fragColor = texture(u_texture, v_texcoord);
        return;
    }

    vec4 color = vec4(0.0);
    int steps = 6; // Sample count around the current pixel
    float thickness = 1.8; // "Grubość" linii (w jednostkach tekstury)
    float stepSize = thickness / float(steps); // Odległość pomiędzy próbkami
    float depthValue = 1.0;
    vec2 texelSize = vec2(1.0 / 800.0, 1.0 / 600.0); // Rozmiar jednego piksela tekstury
    float max = 0.6;

    for (int i = -steps; i <= steps; i++) {
        for (int j = -steps; j <= steps; j++) {
            vec2 offset = vec2(float(i), float(j)) * texelSize * stepSize;
            color += texture(u_texture, v_texcoord + offset);
            depthValue = min(depthValue, texture(u_depthTexture, v_texcoord + offset).r);
        }
    }

    color /= pow(float(2 * steps + 1), 2.0);
    fragColor = min(vec4(vec3(0.0), (1.0-color.r) * 9.0), vec4(max));

    gl_FragDepth = depthValue + 0.0001;
}
    `;
