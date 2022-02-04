// FRAGMENT SHADER

#version 330

in vec4 color;
out vec4 outColor;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

//Textures 
uniform sampler2D texture0;
in vec2 texCoord0;

uniform mat4 matrixView;

in vec4 position;
in vec3 normal;

//PointLight
struct POINT
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};
uniform POINT lightPoint, lightPoint2;

vec4 PointLight(POINT light)
{
	// Calculate Point Light
	vec4 outColor = vec4(0, 0, 0, 0);
	// calculation code to be inserted in place of this comment
	vec3 L = normalize(matrixView * vec4(light.position, 1) - position).xyz;
	float NdotL = dot(normal, L);
	if (NdotL > 0)
		outColor += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	//Specular - Shininess
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);
	if (NdotL > 0 && RdotV > 0)
	    outColor += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	return outColor;

}


void main(void) 
{
  outColor = color;
  outColor += PointLight(lightPoint);
  outColor += PointLight(lightPoint2);

  outColor *= texture(texture0, texCoord0);
}
