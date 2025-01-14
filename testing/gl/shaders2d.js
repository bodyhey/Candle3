
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
    v_texcoord = a_texcoord;// / vec2(2.0);
}

    `;

const fragmentShaderSource2d = `#version 300 es

    #ifdef GL_ES
    // Set default precision to medium
    precision mediump int;
    precision mediump float;
    #endif

    // FXAA algorithm from NVIDIA, C# implementation by Jasper Flick, GLSL port by Dave Hoskins
    // http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
    // https://catlikecoding.com/unity/tutorials/advanced-rendering/fxaa/

    uniform sampler2D u_texture;
    uniform sampler2D u_depthTexture;
    vec2 resolution = vec2(1.0 / 800.0, 1.0 / 600.0);
    in vec2 v_texcoord;
    uniform int u_mode;
    out vec4 fragColor;

    #define EDGE_STEP_COUNT 6
    #define EDGE_GUESS 8.0
    #define EDGE_STEPS 1.0, 1.5, 2.0, 2.0, 2.0, 4.0
    const float edgeSteps[EDGE_STEP_COUNT] = float[EDGE_STEP_COUNT]( EDGE_STEPS );

    float _ContrastThreshold = 0.0312;
    float _RelativeThreshold = 0.063;
    float _SubpixelBlending = 1.0;

    vec4 Sample( sampler2D  tex2D, vec2 uv ) {

        return texture( tex2D, uv );

    }

    float SampleLuminance( sampler2D tex2D, vec2 uv ) {

        return dot( Sample( tex2D, uv ).rgb, vec3( 0.3, 0.59, 0.11 ) );

    }

    float SampleLuminance( sampler2D tex2D, vec2 texSize, vec2 uv, float uOffset, float vOffset ) {

        uv += texSize * vec2(uOffset, vOffset);
        return SampleLuminance(tex2D, uv);

    }

    struct LuminanceData {

        float m, n, e, s, w;
        float ne, nw, se, sw;
        float highest, lowest, contrast;

    };

    LuminanceData SampleLuminanceNeighborhood( sampler2D tex2D, vec2 texSize, vec2 uv ) {

        LuminanceData l;
        l.m = SampleLuminance( tex2D, uv );
        l.n = SampleLuminance( tex2D, texSize, uv,  0.0,  1.0 );
        l.e = SampleLuminance( tex2D, texSize, uv,  1.0,  0.0 );
        l.s = SampleLuminance( tex2D, texSize, uv,  0.0, -1.0 );
        l.w = SampleLuminance( tex2D, texSize, uv, -1.0,  0.0 );

        l.ne = SampleLuminance( tex2D, texSize, uv,  1.0,  1.0 );
        l.nw = SampleLuminance( tex2D, texSize, uv, -1.0,  1.0 );
        l.se = SampleLuminance( tex2D, texSize, uv,  1.0, -1.0 );
        l.sw = SampleLuminance( tex2D, texSize, uv, -1.0, -1.0 );

        l.highest = max( max( max( max( l.n, l.e ), l.s ), l.w ), l.m );
        l.lowest = min( min( min( min( l.n, l.e ), l.s ), l.w ), l.m );
        l.contrast = l.highest - l.lowest;
        return l;

    }

    bool ShouldSkipPixel( LuminanceData l ) {

        float threshold = max( _ContrastThreshold, _RelativeThreshold * l.highest );
        return l.contrast < threshold;

    }

    float DeterminePixelBlendFactor( LuminanceData l ) {

        float f = 2.0 * ( l.n + l.e + l.s + l.w );
        f += l.ne + l.nw + l.se + l.sw;
        f *= 1.0 / 12.0;
        f = abs( f - l.m );
        f = clamp( f / l.contrast, 0.0, 1.0 );

        float blendFactor = smoothstep( 0.0, 1.0, f );
        return blendFactor * blendFactor * _SubpixelBlending;

    }

    struct EdgeData {

        bool isHorizontal;
        float pixelStep;
        float oppositeLuminance, gradient;

    };

    EdgeData DetermineEdge( vec2 texSize, LuminanceData l ) {

        EdgeData e;
        float horizontal =
            abs( l.n + l.s - 2.0 * l.m ) * 2.0 +
            abs( l.ne + l.se - 2.0 * l.e ) +
            abs( l.nw + l.sw - 2.0 * l.w );
        float vertical =
            abs( l.e + l.w - 2.0 * l.m ) * 2.0 +
            abs( l.ne + l.nw - 2.0 * l.n ) +
            abs( l.se + l.sw - 2.0 * l.s );
        e.isHorizontal = horizontal >= vertical;

        float pLuminance = e.isHorizontal ? l.n : l.e;
        float nLuminance = e.isHorizontal ? l.s : l.w;
        float pGradient = abs( pLuminance - l.m );
        float nGradient = abs( nLuminance - l.m );

        e.pixelStep = e.isHorizontal ? texSize.y : texSize.x;
        
        if (pGradient < nGradient) {

            e.pixelStep = -e.pixelStep;
            e.oppositeLuminance = nLuminance;
            e.gradient = nGradient;

        } else {

            e.oppositeLuminance = pLuminance;
            e.gradient = pGradient;

        }

        return e;

    }

    float DetermineEdgeBlendFactor( sampler2D  tex2D, vec2 texSize, LuminanceData l, EdgeData e, vec2 uv ) {

        vec2 uvEdge = uv;
        vec2 edgeStep;
        if (e.isHorizontal) {

            uvEdge.y += e.pixelStep * 0.5;
            edgeStep = vec2( texSize.x, 0.0 );

        } else {

            uvEdge.x += e.pixelStep * 0.5;
            edgeStep = vec2( 0.0, texSize.y );

        }

        float edgeLuminance = ( l.m + e.oppositeLuminance ) * 0.5;
        float gradientThreshold = e.gradient * 0.25;

        vec2 puv = uvEdge + edgeStep * edgeSteps[0];
        float pLuminanceDelta = SampleLuminance( tex2D, puv ) - edgeLuminance;
        bool pAtEnd = abs( pLuminanceDelta ) >= gradientThreshold;

        for ( int i = 1; i < EDGE_STEP_COUNT && !pAtEnd; i++ ) {

            puv += edgeStep * edgeSteps[i];
            pLuminanceDelta = SampleLuminance( tex2D, puv ) - edgeLuminance;
            pAtEnd = abs( pLuminanceDelta ) >= gradientThreshold;

        }

        if ( !pAtEnd ) {

            puv += edgeStep * EDGE_GUESS;

        }

        vec2 nuv = uvEdge - edgeStep * edgeSteps[0];
        float nLuminanceDelta = SampleLuminance( tex2D, nuv ) - edgeLuminance;
        bool nAtEnd = abs( nLuminanceDelta ) >= gradientThreshold;

        for ( int i = 1; i < EDGE_STEP_COUNT && !nAtEnd; i++ ) {

            nuv -= edgeStep * edgeSteps[i];
            nLuminanceDelta = SampleLuminance( tex2D, nuv ) - edgeLuminance;
            nAtEnd = abs( nLuminanceDelta ) >= gradientThreshold;

        }

        if ( !nAtEnd ) {

            nuv -= edgeStep * EDGE_GUESS;

        }

        float pDistance, nDistance;
        if ( e.isHorizontal ) {

            pDistance = puv.x - uv.x;
            nDistance = uv.x - nuv.x;

        } else {
            
            pDistance = puv.y - uv.y;
            nDistance = uv.y - nuv.y;

        }

        float shortestDistance;
        bool deltaSign;
        if ( pDistance <= nDistance ) {

            shortestDistance = pDistance;
            deltaSign = pLuminanceDelta >= 0.0;

        } else {

            shortestDistance = nDistance;
            deltaSign = nLuminanceDelta >= 0.0;

        }

        if ( deltaSign == ( l.m - edgeLuminance >= 0.0 ) ) {

            return 0.0;

        }

        return 0.5 - shortestDistance / ( pDistance + nDistance );

    }

    vec4 ApplyFXAA( sampler2D  tex2D, vec2 texSize, vec2 uv ) {

        LuminanceData luminance = SampleLuminanceNeighborhood( tex2D, texSize, uv );
        if ( ShouldSkipPixel( luminance ) ) {

            return Sample( tex2D, uv );

        }

        float pixelBlend = DeterminePixelBlendFactor( luminance );
        EdgeData edge = DetermineEdge( texSize, luminance );
        float edgeBlend = DetermineEdgeBlendFactor( tex2D, texSize, luminance, edge, uv );
        float finalBlend = max( pixelBlend, edgeBlend );

        if (edge.isHorizontal) {

            uv.y += edge.pixelStep * finalBlend;

        } else {

            uv.x += edge.pixelStep * finalBlend;

        }

        return Sample( tex2D, uv );

    }

    void main() {
        if (u_mode == 0) {
            fragColor = texture(u_texture, v_texcoord);
            gl_FragDepth = texture(u_depthTexture, v_texcoord).r;
        } else if (u_mode == 1) {
            fragColor = ApplyFXAA( u_texture, resolution.xy, v_texcoord );
            gl_FragDepth = texture(u_depthTexture, v_texcoord).r;
        } else {
            vec4 color = vec4(0.0);
            int steps = 6; // Sample count around the current pixel
            float thickness = 3.5; // "Grubość" linii (w jednostkach tekstury)
            float stepSize = thickness / float(steps); // Odległość pomiędzy próbkami
            float depthValue = 1.0;
            vec2 texelSize = vec2(1.0 / 800.0, 1.0 / 600.0); // Rozmiar jednego piksela tekstury
            float max = 0.75;

            for (int i = -steps; i <= steps; i++) {
                for (int j = -steps; j <= steps; j++) {
                    vec2 offset = vec2(float(i), float(j)) * texelSize * stepSize;
                    color += texture(u_texture, v_texcoord + offset);
                    depthValue = min(depthValue, texture(u_depthTexture, v_texcoord + offset).r);
                }
            }

            color /= pow(float(2 * steps + 1), 2.0);
            fragColor = min(vec4(vec3(0.0), (1.0-color.r) * 10.0), vec4(max));
            //fragColor.a = color.a;
            //fragColor = vec4(0.0, 0.0, 0.0, 1.0);
            gl_FragDepth = depthValue + 0.001;   
        }
    }
`;

const fragmentShaderSource2d_ = `#version 300 es

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

float cubic(float x) {
    x = abs(x);
    if (x <= 1.0) {
        return 1.0 - 2.0 * x * x + x * x * x;
    } else if (x < 2.0) {
        return 4.0 - 8.0 * x + 5.0 * x * x - x * x * x;
    }
    return 0.0;
}

void main()
{
    if (!u_gcodeBgMode) {
        //fragColor = texture(u_texture, v_texcoord);

        vec2 textureSize = vec2(1600.0, 1200.0);
        vec2 texelSize = vec2(1.0 / 1600.0, 1.0 / 1200.0);

        // Indeks fragmentu w teksturze
        vec2 texPos = v_texcoord * textureSize;
        vec2 texFloor = floor(texPos);
        vec2 frac = texPos - texFloor;        

        vec4 result = vec4(0.0);
        for (int i = -1; i <= 2; ++i) {
            for (int j = -1; j <= 2; ++j) {
                vec2 offset = vec2(i, j);
                vec2 samplePos = (texFloor + offset) * texelSize;
                float weight = cubic(offset.x - frac.x) * cubic(offset.y - frac.y);
                result += texture(u_texture, samplePos) * weight;
            }
        }
        
        fragColor = vec4(result.a, result.a, result.a, 1.0);
        return;
    }

    vec4 color = vec4(0.0);
    int steps = 6; // Sample count around the current pixel
    float thickness = 1.5; // "Grubość" linii (w jednostkach tekstury)
    float stepSize = thickness / float(steps); // Odległość pomiędzy próbkami
    float depthValue = 1.0;
    vec2 texelSize = vec2(1.0 / 800.0, 1.0 / 600.0); // Rozmiar jednego piksela tekstury
    float max = 0.3;

    for (int i = -steps; i <= steps; i++) {
        for (int j = -steps; j <= steps; j++) {
            vec2 offset = vec2(float(i), float(j)) * texelSize * stepSize;
            color += texture(u_texture, v_texcoord + offset);
            depthValue = min(depthValue, texture(u_depthTexture, v_texcoord + offset).r);
        }
    }

    color /= pow(float(2 * steps + 1), 2.0);
    //color /= 5.0;
    //fragColor = min(vec4(vec3(0.0), (1.0-color.r) * 10.0), vec4(max));
    //fragColor.a = color.a;
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);

    gl_FragDepth = depthValue + 0.001;
}
    `;
