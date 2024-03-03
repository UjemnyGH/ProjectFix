#version 450 core

struct Material {
    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
    float mShininess;
};

struct DirLight {
    vec3 mDirection;
    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
};

struct PointLight {
    vec3 mPosition;
    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
    float mContrast;
    float mLinear;
    float mQuadratic;
};

struct SpotLight {
    vec3 mPosition;
    vec3 mDirection;
    float mInnerCutoff;
    float mOuterCutoff;
    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
};

uniform sampler2D u_Textures[32];
uniform Material u_Material;
uniform DirLight u_DirLight;
uniform PointLight u_PointLights[64];
uniform SpotLight u_SpotLights[64];
uniform float u_PointLightsCount;
uniform float u_SpotLightsCount;
uniform vec3 u_ViewPosition;

uniform float u_Gamma = 2.2;

in vec4 v_Color;
in vec2 v_TexCoords;
in vec3 v_Normals;
in float v_TexID;
in vec4 v_Position;
in mat4 v_Model;

out vec4 o_Color;

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 fragment_pos, vec3 view_pos) {
    vec3 lightDir = normalize(light.mDirection);

    // simple dynamic light 
    float dynamic_light = max(dot(normalize(abs(fragment_pos)), lightDir), 0.0);
    //float dynamic_light = 1.0;
    float diff = max(dot(normal * dynamic_light, lightDir), 0.0);

    //vec3 reflectDir = reflect(-lightDir, normalize(normal));
    vec3 viewDir = normalize(view_pos - fragment_pos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(view_pos, reflectDir), 0.0), int(u_Material.mShininess));
    float spec = pow(max(dot(view_pos, halfwayDir), 0.0), int(u_Material.mShininess));

    vec3 ambient = light.mAmbient * u_Material.mAmbient;
    vec3 diffuse = light.mDiffuse * diff * u_Material.mDiffuse;
    diffuse = pow(diffuse.rgb, vec3(u_Gamma));
    vec3 specular = light.mSpecular * spec * u_Material.mSpecular;

    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragment_pos, vec3 view_pos) {
    vec3 lightDir = normalize(-light.mPosition - fragment_pos);

    float diff = max(dot(normal, lightDir), 0.0);
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(view_pos - fragment_pos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(view_pos, halfwayDir), 0.0), int(u_Material.mShininess));

    float dist = length(light.mPosition - fragment_pos);
    //float attenuation = 1.0 / (light.mContrast + light.mLinear * dist + light.mQuadratic + (dist * dist));
    float attenuation = 1.0 / (dist * dist);

    vec3 ambient = light.mAmbient * u_Material.mAmbient;
    vec3 diffuse = light.mDiffuse * diff * u_Material.mDiffuse;
    diffuse = pow(diffuse.rgb, vec3(u_Gamma));
    vec3 specular = light.mSpecular * spec * u_Material.mSpecular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragment_pos, vec3 view_pos) {
    vec3 lightDir = normalize(-light.mPosition - fragment_pos);
    float theta = dot(lightDir, normalize(-light.mDirection));
    float epsilon = light.mInnerCutoff - light.mOuterCutoff;
    float intensity = clamp((theta - light.mOuterCutoff) / epsilon, 0.0, 1.0);

    vec3 color = light.mAmbient * u_Material.mAmbient;

    if(theta > light.mOuterCutoff) {
        float diff = max(dot(normal, lightDir), 0.0);
        //vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = normalize(view_pos - fragment_pos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(view_pos, halfwayDir), 0.0), int(u_Material.mShininess));

        vec3 diffuse = light.mDiffuse * diff * u_Material.mDiffuse;
        diffuse = pow(diffuse.rgb, vec3(u_Gamma));
        vec3 specular = light.mSpecular * spec * u_Material.mSpecular;

        diffuse *= intensity * intensity;
        specular *= intensity * intensity;

        color += diffuse + specular;
    }

    return color;
}

void main() {
    vec4 Pos = v_Model * v_Position;
    vec3 norm = normalize(mat3(transpose(inverse(v_Model))) * v_Normals);
    vec3 viewDir = normalize(u_ViewPosition - Pos.xyz);

    int tex_ID = int(v_TexID);

    vec4 result = v_Color;

    if(v_TexID < 32.0 && v_TexID >= 0.0) 
    {
        result *= texture(u_Textures[tex_ID], v_TexCoords);
    }

    if(result.w < 0.01) discard;

    vec3 light_result = CalculateDirLight(u_DirLight, norm, Pos.xyz, viewDir);

    for(int i = 0; i < int(u_PointLightsCount); i++) {
        light_result += CalculatePointLight(u_PointLights[i], norm, Pos.xyz, viewDir);
    }

    for(int i = 0; i < int(u_SpotLightsCount); i++) {
        light_result += CalculateSpotLight(u_SpotLights[i], norm, Pos.xyz, viewDir);
    }

    result *= vec4(light_result.xyz, 1.0);

    result.rgb = pow(result.rgb, vec3(1.0 / u_Gamma));

    o_Color = result;
}