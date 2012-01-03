#ifdef _VERTEX_

varying vec3 v3Pos;

void main(void) {
    v3Pos = gl_Vertex.xyz;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

#endif

#ifdef _FRAGMENT_

uniform vec3 v3CameraPos;
uniform vec3 v3LightPos;
varying vec3 v3Pos;
uniform vec3 v3InvWavelength;
uniform float fCameraHeight;
uniform float fCameraHeight2;
uniform float fInnerRadius;
uniform float fInnerRadius2;
uniform float fOuterRadius;
uniform float fOuterRadius2;
uniform float fKrESun;
uniform float fKmESun;
uniform float fKr4PI;
uniform float fKm4PI;
uniform float fScale;
uniform float fScaleDepth;
uniform float fScaleOverScaleDepth;
uniform float fSamples;
uniform int nSamples;
const float g = -0.95;
const float g2 = g*g;

float scale(float fCos) {
    float x = 1.0 - fCos;
    return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

float getNearIntersection(vec3 pos, vec3 ray, float distance2, float radius2) {
    float B = 2.0 * dot(pos, ray);
    float C = distance2 - radius2;
    float det = max(0.0, B*B - 4.0 * C);
    return 0.5 * (-B - sqrt(det));
}

void main (void) {
    vec3 v3Ray = v3Pos - v3CameraPos;
    float fFar = length(v3Ray);
    v3Ray /= fFar;

    float fStartOffset;
    vec3 v3Start;

    if (length(v3CameraPos) > fOuterRadius) {
        float fNear = getNearIntersection(v3CameraPos, v3Ray, fCameraHeight2, fOuterRadius2);
        v3Start = v3CameraPos + v3Ray * fNear;
        fFar -= fNear;
        float fStartAngle = dot(v3Ray, v3Start) / fOuterRadius;
        float fStartDepth = exp(-1.0 / fScaleDepth);
        fStartOffset = fStartDepth * scale(fStartAngle);
    } else {
        v3Start = v3CameraPos;
        float fHeight = length(v3Start);
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
        float fStartAngle = dot(v3Ray, v3Start) / fHeight;
        fStartOffset = fDepth*scale(fStartAngle);
    }

    // Initialize the scattering loop variables
    float fSampleLength = fFar / fSamples;
    float fScaledLength = fSampleLength * fScale;
    vec3 v3SampleRay = v3Ray * fSampleLength;
    vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

    // Now loop through the sample rays
    vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
    for(int i=0; i<nSamples; i++) {
        float fHeight = length(v3SamplePoint);
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        float fLightAngle = dot(v3LightPos, v3SamplePoint) / fHeight;
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
        float fScatter = (fStartOffset + fDepth*(scale(fLightAngle) - scale(fCameraAngle)));
        vec3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        v3SamplePoint += v3SampleRay;
    }

    // Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
    vec4 secondaryColor = vec4(v3FrontColor * fKmESun, 1.0);
    vec4 primaryColor = vec4(v3FrontColor * (v3InvWavelength * fKrESun), 1.0);
    vec3 v3Direction = v3CameraPos - v3Pos;

    float fCos = dot(v3LightPos, v3Direction) / length(v3Direction);
    float fRayleighPhase = 0.75 * (1.0 + fCos*fCos);
    float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
    gl_FragColor = fRayleighPhase * primaryColor + fMiePhase * secondaryColor;
}

#endif
