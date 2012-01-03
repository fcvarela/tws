#ifdef _VERTEX_

void main() {
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}

#endif

#ifdef _FRAGMENT_

uniform sampler2D s2Test;
uniform float fExposure;

void main() {
    vec4 sum = vec4(0);
    vec2 texcoord = vec2(gl_TexCoord[0]);
    float samples[11];

    samples[0] = 0.0222244;
    samples[1] = 0.0378346;
    samples[2] = 0.0755906;
    samples[3] = 0.1309775;
    samples[4] = 0.1756663;
    samples[5] = 0.1974126;
    samples[6] = 0.1756663;
    samples[7] = 0.1309775;
    samples[8] = 0.0755906;
    samples[9] = 0.0378346;
    samples[10]= 0.0222244;

    for (int i=-5; i<5; i++) {
        for (int j=-5; j<5; j++) {
            sum += texture2D(s2Test, texcoord + vec2(j, i) * 0.004) * samples[j+5];
        }
    }

    gl_FragColor = clamp(sum*sum*0.04 + texture2D(s2Test, texcoord), 0.0, 1.0);
    gl_FragColor = mix(gl_FragColor, texture2D(s2Test, texcoord), 0.9);
    gl_FragColor = (1.0 - exp(gl_FragColor * -fExposure));
    gl_FragColor.a = 1.0;
}

#endif
