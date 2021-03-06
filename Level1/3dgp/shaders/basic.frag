// FRAGMENT SHADER

#version 330
//inputs and outputs
in vec2 texCoord0;
in vec4 position;
in vec3 normal;
in vec4 color;

out vec4 outColor;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

//Textures 
uniform sampler2D texture0;

//PointLight
struct POINT
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	mat4 matrix; 
};
uniform POINT lightPoint, lightPoint2, lightPoint3;

vec4 PointLight(POINT light)
{
	// Calculate Point Light
	vec4 outColor = vec4(0, 0, 0, 0);
	// calculation code to be inserted in place of this comment
	vec3 L = normalize(light.matrix * vec4(light.position, 1) - position).xyz;  //uniform mat4 matrixView; changed by light.matrix
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

//SPOT LIGHT
struct SPOT
{
	float cutoff;
	float attenuation;
	mat4 matrix;
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};
uniform SPOT spotLight1;

vec4 SpotLight(SPOT light)
{
	// Calculate Point Light
	vec4 outColor = vec4(0, 0, 0, 0);
	// calculation code to be inserted in place of this comment
	vec3 L = normalize(light.matrix * vec4(light.position, 1) - position).xyz;    //matrixView changed by light.matrix
	float NdotL = dot(normal, L);
	if (NdotL > 0)
		outColor += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	//Specular - Shininess
	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);
	if (NdotL > 0 && RdotV > 0)
	    outColor += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	//////////////////////////////////////////////////////
	// HERE GOES THE NEW CODE TO DETERMINE THE SPOT FACTOR

	vec3 D = normalize(mat3(light.matrix) * light.direction);
	float s = dot(-L, D); 
	float alpha = acos(s);
                            
	if( alpha <= light.cutoff)
	{
		s = pow(s, light.attenuation);
	}
	else
	{
		s = 0;
	}

	return s * outColor;
}

void main(void) 
{

  outColor = color;
  outColor += PointLight(lightPoint);
  outColor += PointLight(lightPoint2);
  outColor += SpotLight(spotLight1);
  outColor *= texture(texture0, texCoord0);
}