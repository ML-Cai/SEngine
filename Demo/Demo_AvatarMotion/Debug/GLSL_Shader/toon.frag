varying vec2 vTexCoord;
varying vec3 vVertex;
varying vec3 vNormal;
varying vec3 vLightVector_0;
varying vec3 vCameraVector;
uniform vec4 vAmbientColor;
uniform vec4 vDiffuseColor;
uniform vec4 vSpecularColor;
uniform sampler2D uTexture_1;
void main()
{
    vec4 edgeColor 	 = vec4(0.1, 0.1, 0.1, 1.0);
    float diffuseDot = dot(vLightVector_0, vNormal);
	float viewDot 	 = dot(vCameraVector, vNormal);
    float toonCoefficient;
    vec4 surfaceColor;

	/* Toon texture processing */
    if (diffuseDot > 0.8)
        toonCoefficient = 1.0;
    else if (diffuseDot > 0.5)
        toonCoefficient = 0.8;
    else
        toonCoefficient = 0.4;
    surfaceColor = texture2D(uTexture_1, vTexCoord );

	/* Outline process, check the angle between view and normal to rendering outline. */
    if(abs(viewDot) < mix(0.1, 0.3, max(0.0, diffuseDot)))	//the abs() operator is used to hold the reverse noraml with view.
        surfaceColor = edgeColor;
    
    gl_FragColor = toonCoefficient * vAmbientColor * surfaceColor +
                   vDiffuseColor * vec4(toonCoefficient * surfaceColor.rgb, surfaceColor.a);
}
