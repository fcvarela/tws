#ifdef _VERTEX_

varying vec4 v_vertex;
varying vec4 orig_vertex;

uniform vec3 v3CameraPos;
uniform vec3 v3LightPos;
uniform vec3 v3InvWavelength;
uniform float fCameraHeight;
uniform float fCameraHeight2;
uniform float fOuterRadius;
uniform float fOuterRadius2;
uniform float fInnerRadius;
uniform float fInnerRadius2;
uniform float fKrESun;
uniform float fKmESun;
uniform float fKr4PI;
uniform float fKm4PI;
uniform float fScale;
uniform float fScaleDepth;
uniform float fScaleOverScaleDepth;

uniform int nSamples;
uniform float fSamples;

uniform sampler2D heightTexture;
uniform int skirts;

float scale(float fCos);
float getNearIntersection(vec3 pos, vec3 ray, float distance2, float radius2);

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

void main(void) {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    if (length(gl_Vertex.xyz) > 1000.0) {
        vec4 heightmap = texture2D(heightTexture, gl_TexCoord[0].st);
        float height = heightmap.a * 65536.0 - 32768.0;
        height = floor(height);
        orig_vertex = vec4(normalize(gl_Vertex.xyz) * (height + fInnerRadius), 1.0);
    } else {
        orig_vertex = gl_Vertex;
    }

    v_vertex = gl_ModelViewMatrix * orig_vertex;
    gl_Position = gl_ModelViewProjectionMatrix * orig_vertex;

    // Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
    vec3 v3Pos = orig_vertex.xyz;
    vec3 v3Ray = v3Pos - v3CameraPos;
    float fFar = length(v3Ray);
    v3Ray /= fFar;

    float fNear, fDepth;
    vec3 v3Start;

    if (length(v3CameraPos) > fOuterRadius) {
        fNear = getNearIntersection(v3CameraPos, v3Ray, fCameraHeight2, fOuterRadius2);
        v3Start = v3CameraPos + v3Ray * fNear;
        fFar -= fNear;
        fDepth = exp((fInnerRadius - fOuterRadius) / fScaleDepth);
    } else {
        v3Start = v3CameraPos;
        fDepth = exp((fInnerRadius - fCameraHeight) / fScaleDepth);
    }

    float fCameraAngle = dot(-v3Ray, v3Pos) / length(v3Pos);
    float fLightAngle = dot(v3LightPos, v3Pos) / length(v3Pos);
    float fCameraScale = scale(fCameraAngle);
    float fLightScale = scale(fLightAngle);
    float fCameraOffset = fDepth*fCameraScale;
    float fTemp = (fLightScale + fCameraScale);

    float fSampleLength = fFar / fSamples;
    float fScaledLength = fSampleLength * fScale;
    vec3 v3SampleRay = v3Ray * fSampleLength;
    vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

    // Now loop through the sample rays
    vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
    vec3 v3Attenuate;
    for (int i=0; i<nSamples; i++) {
        float fHeight = length(v3SamplePoint);
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        float fScatter = fDepth*fTemp - fCameraOffset;
        v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        v3SamplePoint += v3SampleRay;
    }
    gl_FrontColor.rgb = v3FrontColor * (v3InvWavelength * fKrESun + fKmESun);
    gl_FrontSecondaryColor.rgb = v3Attenuate;
}

#endif

#ifdef _FRAGMENT_
uniform int lod;
uniform sampler2D terrainTexture;
uniform sampler2D normalTexture;
uniform sampler2D terrainDetailTexture;
uniform sampler2D heightTexture;
uniform float fInnerRadius;
varying vec4 v_vertex;
uniform vec3 v3CameraPos;
varying vec4 orig_vertex;

mat3 fromToRotation(vec3 from, vec3 to) {
    float EPSILON = 0.000001;
    mat3 result;

    vec3 v = cross(from, to);
    float e = dot(from, to);
    float f = (e < 0.0) ? -e : e;
    if (f > 1.0-EPSILON) {
        vec3 u, v, x;
        float c1, c2, c3;
        int i,j;
        x.x = from.x > 0.0 ? from.x : -from.x;
        x.x = from.y > 0.0 ? from.y : -from.y;
        x.x = from.z > 0.0 ? from.z : -from.z;

        if (x.x < x.y) {
            if (x.x < x.z) {x.x = 1.0; x.y = x.z = 0.0;}
            else {x.z = 1.0; x.x = x.y = 0.0;}
        } else {
            if (x.y < x.z) {x.y = 1.0; x.x = x.z = 0.0;}
            else {x.z = 1.0; x.x = x.y = 0.0;}
        }

        u = x - from;
        v = x - to;

        c1 = 2.0 / dot(u, u);
        c2 = 2.0 / dot(v, v);
        c3 = c1 * c2 * dot(u, v);

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++)
                result[j][i] = -c1 * u[i] * u[j] - c2 * v[i] * v[j] + c3 * v[i] * u[j];
            result[i][i] += 1.0;
        }
    } else {
        float hvx, hvz, hvxy, hvxz, hvyz;
        float h = 1.0/(1.0 + e);
        hvx = h * v.x;
        hvz = h * v.z;
        hvxy = hvx * v.y;
        hvxz = hvx * v.z;
        hvyz = hvz * v.y;
        result[0][0] = e + hvx * v.x;
        result[0][1] = hvxy - v.z;
        result[0][2] = hvxz + v.y;

        result[1][0] = hvxy + v.z;
        result[1][1] = e + h * v.y * v.y;
        result[1][2] = hvyz - v.x;

        result[2][0] = hvxz - v.y;
        result[2][1] = hvyz + v.x;
        result[2][2] = e + hvz * v.z;
    }
    return result;
}

void main (void) {
    vec4 heightmap = texture2D(heightTexture, gl_TexCoord[0].st);
    float real_height = heightmap.a * 65536.0 - 32768.0;
    float height = floor(real_height);

    // texture
    vec4 terrain = texture2D(terrainTexture, gl_TexCoord[0].st);
    vec4 detail = vec4(1.0);
    if (lod < 1) detail = texture2D(terrainDetailTexture, gl_TexCoord[0].st*16.0) * 2.0;

    vec3 normal = texture2D(normalTexture, gl_TexCoord[0].st).xyz;
    normal = (normal * 2.0) - 1.0;

    float slope = abs(dot(vec3(0.0, 0.0, 1.0), normal.rbg));

    // compute specular
    float coeff = 0.0;
    if (terrain.rgba != vec4(0.0)) {
        if (terrain.a == 0.0)
            coeff = 0.4;
    }

    // rotate normal accordingly
    normal *= vec3(1.0, 1.0, 1.0);
    mat3 rot_matrix = fromToRotation(vec3(0.0, 0.0, 1.0), normalize(orig_vertex.xyz));
    normal = normalize(normal * rot_matrix);
    normal = normalize(gl_NormalMatrix * normal);

    vec3 light = normalize(vec3(gl_LightSource[0].position - v_vertex));
    vec3 halfV = reflect(-light, normal);

    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);

    // compute diffuse
    float nxDir = max(0.0, dot(normal, light));
    if (nxDir > 0.0) {
        diffuse = gl_LightSource[0].diffuse * nxDir;
        float NdotHV = clamp(dot(halfV, vec3(normalize(-v_vertex))), 0.0, 1.0);
        specular = gl_LightSource[0].specular * pow(NdotHV, 32.0) * coeff;
    }

    terrain.a = 1.0;
    terrain = terrain * detail * diffuse + specular;
    gl_FragColor = terrain + (gl_Color + 0.25 * gl_SecondaryColor);
    gl_FragColor.a = 1.0;
}

#endif
