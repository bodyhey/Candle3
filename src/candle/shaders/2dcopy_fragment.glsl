#version 130

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

vec4 cubic(float v)
{
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) * (1.0/6.0);
}

vec4 textureBicubic(sampler2D sampler, vec2 texCoords)
{
   vec2 texSize = textureSize(sampler, 0);
   vec2 invTexSize = 1.0 / texSize;

   texCoords = texCoords * texSize - 0.5;


    vec2 fxy = fract(texCoords);
    texCoords -= fxy;

    vec4 xcubic = cubic(fxy.x);
    vec4 ycubic = cubic(fxy.y);

    vec4 c = texCoords.xxyy + vec2 (-0.5, +1.5).xyxy;

    vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4 (xcubic.yw, ycubic.yw) / s;

    offset *= invTexSize.xxyy;

    vec4 sample0 = texture(sampler, offset.xz);
    vec4 sample1 = texture(sampler, offset.yz);
    vec4 sample2 = texture(sampler, offset.xw);
    vec4 sample3 = texture(sampler, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(
       mix(sample3, sample2, sx), mix(sample1, sample0, sx)
    , sy);
}

void main()
{
    if (!u_gcodeBgMode) {
        fragColor = textureBicubic(u_texture, v_texcoord);
        // fragColor = texture(u_texture, v_texcoord);
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
    fragColor.b = 1.0;

    gl_FragDepth = depthValue + 0.0001;
}

// #ifdef GL_ES
// // Set default precision to medium
// precision mediump int;
// precision mediump float;
// #endif

// varying vec2 v_texcoord;
// uniform sampler2D u_texture;

// vec4 cubic(float v)
// {
//     vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
//     vec4 s = n * n * n;
//     float x = s.x;
//     float y = s.y - 4.0 * s.x;
//     float z = s.z - 4.0 * s.y + 6.0 * s.x;
//     float w = 6.0 - x - y - z;
//     return vec4(x, y, z, w) * (1.0/6.0);
// }

// vec4 textureBicubic(sampler2D sampler, vec2 texCoords)
// {
//    vec2 texSize = textureSize(sampler, 0);
//    vec2 invTexSize = 1.0 / texSize;

//    texCoords = texCoords * texSize - 0.5;


//     vec2 fxy = fract(texCoords);
//     texCoords -= fxy;

//     vec4 xcubic = cubic(fxy.x);
//     vec4 ycubic = cubic(fxy.y);

//     vec4 c = texCoords.xxyy + vec2 (-0.5, +1.5).xyxy;

//     vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
//     vec4 offset = c + vec4 (xcubic.yw, ycubic.yw) / s;

//     offset *= invTexSize.xxyy;

//     vec4 sample0 = texture(sampler, offset.xz);
//     vec4 sample1 = texture(sampler, offset.yz);
//     vec4 sample2 = texture(sampler, offset.xw);
//     vec4 sample3 = texture(sampler, offset.yw);

//     float sx = s.x / (s.x + s.y);
//     float sy = s.z / (s.z + s.w);

//     return mix(
//        mix(sample3, sample2, sx), mix(sample1, sample0, sx)
//     , sy);
// }


// void main()
// {
//     gl_FragColor = textureBicubic(u_texture, v_texcoord); //texture2D(u_texture, v_texcoord);
// }
