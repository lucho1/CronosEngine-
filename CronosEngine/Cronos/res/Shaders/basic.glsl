#type vertex
#version 430 core

//Vertex Layouts
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

//Uniform Buffer Object for camera data
layout(std140, binding = 0) uniform u_CameraData
{
	mat4 u_View;
	mat4 u_Proj;
	vec3 u_CameraPosition;
};

//Uniforms
uniform mat4 u_Model;

//Core Varying for fragment shader
out CoreVaryings
{
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
	vec3 CamPos;
} v_VertexCoreVars;


void main()
{
	v_VertexCoreVars.TexCoords	= a_TexCoords;
	v_VertexCoreVars.CamPos		= u_CameraPosition;
	v_VertexCoreVars.FragPos	= vec3(u_Model * vec4(a_Position, 1.0));
	v_VertexCoreVars.Normal		= mat3(transpose(inverse(u_Model))) * a_Normal;

	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
}


#type fragment
#version 430 core

// SHADER ASSERTION -----------------------------------------------------------------------------------------------------------------------------
#define CNSH_ASSERT(condition) while(!condition) { ++m_AssertTimes; break; }
#define COLOROUTPUT_EXIT() while(m_AssertTimes != 0) { color = AssertColorOutput(); break; }

uniform float u_ShaderPlaybackTime;
int m_AssertTimes = 0;

float randomNum(float num) { return fract(sin(dot(vec2(num), vec2(12.9898,78.233))) * 43758.5453); }
vec4 AssertColorOutput()
{
	//This is basically to set a blinking output color to denote an assertion on shader run time.
	vec4 blinkColor = vec4(1.0);
	blinkColor = vec4(randomNum(u_ShaderPlaybackTime), randomNum(u_ShaderPlaybackTime/2), randomNum(u_ShaderPlaybackTime/3), 1.0);
	return (mod(u_ShaderPlaybackTime, 1.0) > 0.5) ? blinkColor : vec4(1.0);
}
// ----------------------------------------------------------------------------------------------------------------------------------------------


// SHADER ITSELF --------------------------------------------------------------------------------------------------------------------------------
//Input variables
in CoreVaryings
{
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
	vec3 CamPos;
} v_CoreVars;


//Material Stuff
uniform vec4 u_AmbientColor;
uniform float u_Shininess = 32.0;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
//uniform sampler2D u_NormalMap;

uniform bool u_TextureEmpty = true;

uniform float u_GammaCorrectionValue = 2.2;

//Lights Structures
struct DirLight 
{
	vec4 LightDir;
	vec4 LightColor;
	float LightIntensity;
};

struct PointLight
{
	vec4 LightPos;
	vec4 LightColor;
	float LightIntensity;

	float LightAtt_K, LightAtt_L, LightAtt_Q; //Attenuation Factors (K constant, L linear, Q quadratic)
};

struct SpotLight
{
	vec4 LightPos;
	vec4 LightColor;
	vec4 LightDir;
	float LightIntensity;

	float LightAtt_K, LightAtt_L, LightAtt_Q; //Attenuation Factors (K constant, L linear, Q quadratic)
	float innerCutoffAngleCos, outerCutoffAngleCos; //Inner/Outer Cutoff values for light effect
};

//Lights Shader Storage Buffers
layout(std430, binding = 0) buffer SSBOData
{
	int u_CurrentLightsNum[3];
	PointLight u_PointLightsArray[];
};

layout(std430, binding = 1) buffer SSBOData2
{
	DirLight u_DirLightsArray[];
};

layout(std430, binding = 2) buffer SSBOData3
{	
	SpotLight u_SPLightsArray[];
};


//Light Calculations Functions ---------------------------------------------------------------------------------------
vec3 CalculateLightResult(vec3 LColor, vec3 LDir, vec3 normal, vec3 viewDir)
{
	//Normalize light direction
	vec3 lightDir = normalize(LDir);
	
	//Diffuse Component
	float diffImpact = max(dot(normal, lightDir), 0.0);
	
	//Specular component
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specImpact = pow(max(dot(normal, halfwayDir), 0.0), u_Shininess);

	//Calculate light result
	vec3 diffuse = LColor * diffImpact;
	vec3 specular = LColor * specImpact;

	//If we have textures, apply them
	if(!u_TextureEmpty)
	{
		diffuse *= texture(u_DiffuseTexture, v_CoreVars.TexCoords).rgb;
		specular *= texture(u_SpecularTexture, v_CoreVars.TexCoords).rgb;
	}	

	return (diffuse + specular);
}

//Dir Light Calculation
vec3 CalculateDirectionalLight(DirLight dLight, vec3 normal, vec3 viewDirection)
{
	//Result
	return CalculateLightResult(dLight.LightColor.xyz, dLight.LightDir.xyz, normal, viewDirection) * dLight.LightIntensity;
}

//Point Light Calculation
vec3 CalculatePointLight(PointLight pLight, vec3 normal, vec3 viewDirection)
{
	//Calculate light direction
	vec3 direction = pLight.LightPos.xyz - v_CoreVars.FragPos;

	//Attenuation Calculation
	float d = length(pLight.LightPos.xyz - v_CoreVars.FragPos);
	float lightAttenuation = 1.0/ (pLight.LightAtt_K + pLight.LightAtt_L * d + pLight.LightAtt_Q *(d * d));

	//Result
	return CalculateLightResult(pLight.LightColor.xyz, direction, normal, viewDirection) * lightAttenuation * pLight.LightIntensity;
}

//Spot Light Calculation
vec3 CalculateSpotLight(SpotLight spLight, vec3 normal, vec3 viewDirection)
{
	//Calculate light direction
	vec3 direction = spLight.LightPos.xyz - v_CoreVars.FragPos;

	//Attenuation Calculation
	float d = length(spLight.LightPos.xyz - v_CoreVars.FragPos);
	float lightAttenuation = 1.0/ (spLight.LightAtt_K + spLight.LightAtt_L * d + spLight.LightAtt_Q *(d * d));

	//Spotlight Calcs for Soft Edges
	float theta = dot(normalize(spLight.LightPos.xyz - v_CoreVars.FragPos), normalize(-spLight.LightDir.xyz)); //Light direction and light orientation
	float epsilon = spLight.innerCutoffAngleCos - spLight.outerCutoffAngleCos;
	
	float lightIntensity = clamp((theta - spLight.outerCutoffAngleCos) / epsilon, 0.0, 1.0) * spLight.LightIntensity;

	//Result
	return CalculateLightResult(spLight.LightColor.xyz, direction, normal, viewDirection) * lightAttenuation * lightIntensity;
}


//ZBuffer rendering -------------------------------------------------------------------------------------------------
uniform vec2 u_CamPlanes; //x for near plane, y for far plane
uniform bool u_drawZBuffer = false;

float LinearizeZ(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * u_CamPlanes.x * u_CamPlanes.y) / (u_CamPlanes.y + u_CamPlanes.x - z*(u_CamPlanes.y - u_CamPlanes.x));
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//Output variables
out vec4 color;

void main()
{
	//Drawing ZBuffer (if we must)
	if (u_drawZBuffer)
	{
		float depth = (LinearizeZ(gl_FragCoord.z) / u_CamPlanes.y);
		color = vec4(vec3(depth), 1.0);
	}
	else
	{
		//Initial Assertion Check
		CNSH_ASSERT((u_CurrentLightsNum[0] >= 0));
		CNSH_ASSERT((u_CurrentLightsNum[1] >= 0));
		CNSH_ASSERT((u_CurrentLightsNum[2] >= 0));

		//Generic Light Calculations
		vec3 normalVec = normalize(v_CoreVars.Normal);
		vec3 viewDirection = normalize(v_CoreVars.CamPos - v_CoreVars.FragPos);

		//Color Output
		vec3 colorOutput = vec3(0.0);

		//Lighting Calculations
		for(int i = 0; i < u_CurrentLightsNum[0]; ++i)
			colorOutput += CalculateDirectionalLight(u_DirLightsArray[i], normalVec, viewDirection);

		for(int i = 0; i < u_CurrentLightsNum[1]; ++i)
			colorOutput += CalculatePointLight(u_PointLightsArray[i], normalVec, viewDirection);

		for(int i = 0; i < u_CurrentLightsNum[2]; ++i)
			colorOutput += CalculateSpotLight(u_SPLightsArray[i], normalVec, viewDirection);
		
		//Texturing Calculations & Final Fragment Color
		if(!u_TextureEmpty)
			color = vec4(colorOutput + u_AmbientColor.rgb * texture(u_DiffuseTexture, v_CoreVars.TexCoords).rgb, 1.0);
		else
			color = vec4(colorOutput + u_AmbientColor.rgb, 1.0);

		//Gamma correction
		color.rgb = pow(color.rgb, vec3(1.0/u_GammaCorrectionValue));

		//Final Assertion Check
		COLOROUTPUT_EXIT();
	}
}		