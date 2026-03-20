#version 460 core
out vec4 aColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular2;
    int diffuseCount;
    int specularCount;
    float shininess;
};
uniform Material material;

struct DirLight {
    vec3 targetPos;
    vec3 lightPos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight[1];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 direction  = normalize(light.targetPos - light.lightPos);
    vec3 lightDir   = normalize(-direction);
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir   = normalize(light.position - fragPos);
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

struct SpotLight {
  float innerCutoff;
  float outerCutoff;
  vec3 cameraDir;
  vec3 cameraPos;

  float constant;
  float linear;
  float quadratic;
};

uniform SpotLight spotLight;

vec3 CalcSpotLight(SpotLight spotLight, vec3 viewDir, vec3 normal, vec3 fragPos) {
  float theta = (dot(normalize(-spotLight.cameraDir), viewDir));
  float intensity = clamp((theta - spotLight.outerCutoff) / (spotLight.innerCutoff - spotLight.outerCutoff),0.1,1);
  float diff = max(dot(normal, viewDir), 0.0);
  vec3 ambient;
  if (material.diffuseCount > 1) {
    ambient = 0.2 * vec3(texture(material.texture_diffuse1, TexCoords)) * vec3(texture(material.texture_diffuse2, TexCoords));
  } else {
    ambient = 0.2 * vec3(texture(material.texture_diffuse1, TexCoords));
  }
  vec3 diffuse;
  if (material.specularCount > 1) {
    diffuse  = diff * vec3(texture(material.texture_diffuse1, TexCoords)) * vec3(texture(material.texture_diffuse2, TexCoords));
  } else {
    diffuse  = diff * vec3(texture(material.texture_diffuse1, TexCoords));
  }

  float distance    = length(spotLight.cameraPos - fragPos);
  float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));

  ambient *= (intensity * attenuation);
  diffuse *= (intensity * attenuation);
  return (ambient + diffuse);
}

float LinearizeDepth(float depth, float near, float far) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < 1; i++) {
        result += CalcDirLight(dirLight[i], norm, viewDir);
    }
    // for (int i = 0; i < NR_POINT_LIGHTS; i++) {
    //     result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // }

    // result += CalcSpotLight(spotLight, viewDir, norm, FragPos);
    // float depth = LinearizeDepth(gl_FragCoord.z, 0.1, 10) / 10;
    // result += pow(depth, 32);
    aColor = vec4(result, 1.0f);
}
