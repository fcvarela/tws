#ifdef _VERTEX_

void main() {
	vec3 normal, lightdir;
	float NdotL;

	normal = normalize(gl_NormalMatrix * gl_Normal);
	lightdir = normalize(vec3(gl_LightSource[0].position));

	NdotL = max(dot(normal, lightdir), 0.0);
	gl_FrontColor = vec4(vec3(1.0) * NdotL, 1.0);
	gl_Position = ftransform();

}

#endif

#ifdef _FRAGMENT_

void main() {
	gl_FragColor = gl_Color;
}

#endif
