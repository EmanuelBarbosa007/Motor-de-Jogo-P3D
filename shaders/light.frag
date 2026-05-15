#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model

uniform sampler2D TexSampler;

// Estrutura da fonte de luz ambiente global
struct AmbientLight {
	vec3 ambient;	// Componente de luz ambiente global
};

uniform AmbientLight ambientLight;

// Estrutura de uma fonte de luz direcional
struct DirectionalLight	{
	vec3 direction;		// Direção da luz, espaço do mundo
	
	vec3 ambient;		// Componente de luz ambiente
	vec3 diffuse;		// Componente de luz difusa
	vec3 specular;		// Componente de luz especular
};

uniform DirectionalLight directionalLight; // Fonte de luz direcional

// Estrutura de uma fonte de luz pontual
struct PointLight	{
	vec3 position;		// Posição do ponto de luz, espaço do mundo
	
	vec3 ambient;		// Componente de luz ambiente
	vec3 diffuse;		// Componente de luz difusa
	vec3 specular;		// Componente de luz especular
	
	float constant;		// Coeficiente de atenuação constante
	float linear;		// Coeficiente de atenuação linear
	float quadratic;	// Coeficiente de atenuação quadrática
};

uniform PointLight pointLight[2]; // Duas fontes de luz pontual

// Estrutura de uma fonte de luz cónica
struct SpotLight {
	vec3 position;		// Posição do foco de luz, espaço do mundo
	
	vec3 ambient;		// Componente de luz ambiente
	vec3 diffuse;		// Componente de luz difusa
	vec3 specular;		// Componente de luz especular
	
	float constant;		// Coeficiente de atenuação constante
	float linear;		// Coeficiente de atenuação linear
	float quadratic;	// Coeficiente de atenuação quadrática

	float spotCutoff, spotExponent;
	vec3 spotDirection;
};

uniform SpotLight spotLight; // Fonte de luz cónica

struct Material{
	vec3 emissive;
	vec3 ambient;		// Ka
	vec3 diffuse;		// kd
	vec3 specular;		// ke
	float shininess;
};

uniform Material material;
uniform int hasTexture; // A NOSSA VARIÁVEL NOVA QUE VEM DO C++!

vec3 diffuseColor;

in vec3 vPositionEyeSpace;
in vec3 vNormalEyeSpace;
in vec2 textureCoord;

layout (location = 0) out vec4 fColor; // Cor final do fragmento

vec4 calcAmbientLight(AmbientLight light);
vec4 calcDirectionalLight(DirectionalLight light, out vec4 ambient);
vec4 calcPointLight(PointLight light, out vec4 ambient);
vec4 calcSpotLight(SpotLight light, out vec4 ambient);

void main()
{
	// Se tiver textura (Bolas), usa a imagem. Se não tiver textura (Mesa), usa apenas a cor do material!
	if (hasTexture == 1) {
		diffuseColor = texture(TexSampler, textureCoord).rgb;
	} else {
		diffuseColor = material.diffuse;
	}

	// Cálculo da componente emissiva do material.
	vec4 emissive = vec4(material.emissive, 1.0);

	// Luz Ambiente Global
	vec4 ambient;

	// Cálculo do efeito da iluminação no fragmento.
	vec4 light[4];
	vec4 ambientTmp;

	// Contribuição da fonte de luz ambiente
	ambient = calcAmbientLight(ambientLight);
	
	// Contribuição da fonte de luz direcional
	light[0] = calcDirectionalLight(directionalLight, ambientTmp);
	ambient += ambientTmp;
	
	// Contribuição de cada fonte de luz Pontual
	for(int i=0; i<2; i++) {
		light[i+1] = calcPointLight(pointLight[i], ambientTmp);
		ambient += ambientTmp;
	}
	
	// Contribuição da fonte de luz cónica
	light[3] = calcSpotLight(spotLight, ambientTmp);
	ambient += ambientTmp;

	// Afetar a componente ambiente pela cor do material
	ambient *= vec4(diffuseColor,1.0);

	// Cálculo da cor final do fragmento.
	// ----------------------------------------------------------------
	// Combinar luzes de forma mais realista
	vec4 directLighting = vec4(0.0);
	float totalWeight = 0.0;

	// Somar contribuições com pesos baseados na distância/intensidade
	for(int i = 0; i < 4; i++) {
		if(length(light[i].rgb) > 0.001) { // Apenas se a luz contribui significativamente
			float weight = 1.0 / (1.0 + length(light[i].rgb)); // Peso inversamente proporcional à intensidade
			directLighting += light[i] * weight;
			totalWeight += weight;
		}
	}

	// Normalizar pela soma dos pesos
	if(totalWeight > 0.0) {
		directLighting /= totalWeight;
	}

	// Combinar com ambiente de forma mais suave
	vec4 finalAmbient = ambient * 0.2; // Reduzir ainda mais a contribuição ambiente
	fColor = emissive + finalAmbient + directLighting;
	fColor = min(fColor, 1.0);
	// ----------------------------------------------------------------
}

vec4 calcAmbientLight(AmbientLight light) {
	// Cálculo da contribuição da fonte de luz ambiente global, para a cor do objeto.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);
	return ambient;
}

vec4 calcDirectionalLight(DirectionalLight light, out vec4 ambient) {
	// Cálculo da reflexão da componente da luz ambiente.
	ambient = vec4(material.ambient * light.ambient, 1.0);

	// Cálculo da reflexão da componente da luz difusa.
	vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
	vec3 L = normalize(-lightDirectionEyeSpace); // Direção inversa à da direção luz.
	vec3 N = normalize(vNormalEyeSpace);
	
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(diffuseColor * light.diffuse, 1.0) * NdotL;
	
	// Cálculo da reflexão da componente da luz especular.
	vec3 V = normalize(-vPositionEyeSpace);
	
	// Modelo Blinn-Phong
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	// Cálculo da contribuição da fonte de luz direcional para a cor final do fragmento.
	return (diffuse + specular);
}

vec4 calcPointLight(PointLight light, out vec4 ambient) {
	// Cálculo da reflexão da componente da luz ambiente.
	ambient = vec4(material.ambient * light.ambient, 1.0);

	// Cálculo da reflexão da componente da luz difusa.
	vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(diffuseColor * light.diffuse, 1.0) * NdotL;
	
	// Cálculo da reflexão da componente da luz especular.
	vec3 V = normalize(-vPositionEyeSpace);
	
	// Modelo Blinn-Phong
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	// attenuation
	float dist = length(lightPositionEyeSpace - vPositionEyeSpace);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	
	return (attenuation * (diffuse + specular));
}

vec4 calcSpotLight(SpotLight light, out vec4 ambient) 
{
	// Cálculo da reflexão da componente da luz ambiente.
	ambient = vec4(material.ambient * light.ambient, 1.0);

	// Cálculo da reflexão da componente da luz difusa.
	vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(diffuseColor * light.diffuse, 1.0) * NdotL;
	
	// Cálculo da reflexão da componente da luz especular.
	vec3 V = normalize(-vPositionEyeSpace);
	
	// Modelo Blinn-Phong
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	float spotIntensity;
	float spotCos = dot(L, normalize((View * vec4(-light.spotDirection, 0.0)).xyz)); 
	
	if(acos(spotCos) > radians(light.spotCutoff)) {
		spotIntensity = 0.0f;
	}
	else {
		spotIntensity = pow(spotCos, light.spotExponent);
	}
	diffuse *= spotIntensity;
	specular *= spotIntensity;
	
	// attenuation
	float dist = length(lightPositionEyeSpace - vPositionEyeSpace);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	
	return (attenuation * (diffuse + specular));
}