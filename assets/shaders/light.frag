#version 450 core

struct Material {
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

uniform sampler2DArray u_TexturesArray[32];
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

float CalculateShadowsDirLight(vec4 fragPosLS, vec3 lightDir) {
    vec3 proj_coords = fragPosLS.xyz / fragPosLS.w;

    proj_coords = proj_coords * 0.5 + 0.5;
    float closest_depth = texture(u_TexturesArray[31], vec3(proj_coords.xy, 0.0)).r;
    float current_depth = proj_coords.z;

    vec3 normal = normalize(v_Normals);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_TexturesArray[31], 0).xy;

    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcf_dpt = texture(u_TexturesArray[31], vec3(proj_coords.xy + vec2(x, y) * texelSize, 0.0)).r;
            shadow += current_depth - bias > pcf_dpt ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if(proj_coords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

vec4 CalculateDirLight(DirLight light, vec3 normal, vec3 fragment_pos, vec3 view_pos) {
    vec3 lightDir = normalize(-light.mDirection);

    // simple dynamic light 
    float dynamic_light = max(dot(normalize(abs(fragment_pos)), lightDir), 0.0);
    //float dynamic_light = 1.0;
    float diff = max(dot(normalize(normal * dynamic_light), lightDir), 0.0);

    //vec3 reflectDir = reflect(-lightDir, normalize(normal));
    vec3 viewDir = normalize(view_pos - fragment_pos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), int(u_Material.mShininess));
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), int(u_Material.mShininess));

    vec3 ambient = light.mAmbient;
    vec4 diffuse = vec4(light.mDiffuse.xyz, 1.0) * vec4(diff, diff, diff, 1.0);
    vec3 specular = light.mSpecular * spec;

    int tex_ID = int(v_TexID);
    if(tex_ID < 31 && tex_ID >= 0) {
        ambient = light.mAmbient * texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 0.0)).xyz;
        diffuse = vec4(light.mDiffuse.xyz, 1.0) * pow(texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 0.0)), vec4(u_Gamma, u_Gamma, u_Gamma, 1.0)) * vec4(diff, diff, diff, 1.0);
        specular = light.mSpecular * spec * texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 1.0)).xyz;
    }

    //vec3 lighting = (ambient + (1.0 - CalculateShadowsDirLight(vec4(fragment_pos, 100.0), lightDir)) * (diffuse.xyz + specular));

    //return vec4(lighting, diffuse.w);
    return vec4(ambient.xyz, 0.0) + diffuse + vec4(specular.xyz, 0.0);
}

vec4 CalculatePointLight(PointLight light, vec3 normal, vec3 fragment_pos, vec3 view_pos) {
    vec3 lightDir = normalize(-light.mPosition - fragment_pos);

    float diff = max(dot(normal, lightDir), 0.0);
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(view_pos - fragment_pos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), int(u_Material.mShininess));

    float dist = length(light.mPosition - fragment_pos);
    //float attenuation = 1.0 / (light.mContrast + light.mLinear * dist + light.mQuadratic + (dist * dist));
    float attenuation = 1.0 / (dist);

    vec3 ambient = light.mAmbient;
    vec4 diffuse = vec4(light.mDiffuse.xyz, 1.0) * vec4(diff, diff, diff, 1.0);
    vec3 specular = light.mSpecular * spec;

    int tex_ID = int(v_TexID);
    if(tex_ID < 31 && tex_ID >= 0) {
        ambient = light.mAmbient * texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 0.0)).xyz;
        diffuse = diffuse * pow(texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 0.0)), vec4(u_Gamma, u_Gamma, u_Gamma, 1.0));
        specular = specular * texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 1.0)).xyz;
    }

    ambient = ambient * attenuation;
    diffuse = diffuse * attenuation;
    specular = specular * attenuation;

    return vec4(ambient.xyz, 0.0) + diffuse + vec4(specular.xyz, 0.0);
}

vec4 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragment_pos, vec3 view_pos) {
    vec3 lightDir = normalize(light.mPosition - fragment_pos);
    float theta = dot(lightDir, normalize(-light.mDirection));
    float epsilon = light.mInnerCutoff - light.mOuterCutoff;
    float intensity = clamp((theta - light.mOuterCutoff) / epsilon, 0.0, 1.0);

    vec4 color = vec4(light.mAmbient.xyz, 0.0);

    if(theta > light.mOuterCutoff) {
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = normalize(view_pos - fragment_pos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        //float spec = pow(max(dot(viewDir, halfwayDir), 0.0), int(u_Material.mShininess));
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), int(u_Material.mShininess));

        vec4 diffuse = vec4(light.mDiffuse.xyz, 1.0) * vec4(diff, diff, diff, 1.0);
        vec3 specular = light.mSpecular * spec;

        int tex_ID = int(v_TexID);
        if(tex_ID < 31 && tex_ID >= 0) {
            color = vec4(light.mAmbient, 1.0) * texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 0.0));
            diffuse = diffuse * pow(texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 0.0)), vec4(u_Gamma, u_Gamma, u_Gamma, 1.0));
            specular = specular * texture(u_TexturesArray[tex_ID], vec3(v_TexCoords, 1.0)).xyz;
        }

        diffuse *= intensity * intensity;
        specular *= intensity * intensity;

        float dist = length(light.mPosition - fragment_pos);
        float attenuation = 1.0 / (dist);

        color = color * attenuation;
        diffuse = diffuse * attenuation;
        specular = specular * attenuation;

        color += diffuse + vec4(specular.xyz, 0.0);
    }

    return color;
}

void main() {
    vec4 Pos = v_Model * v_Position;
    vec3 norm = normalize(mat3(transpose(inverse(v_Model))) * v_Normals);
    vec3 viewDir = normalize(u_ViewPosition - Pos.xyz);

    vec4 result = v_Color;

    if(result.w < 0.01) discard;

    vec4 light_result = CalculateDirLight(u_DirLight, norm, Pos.xyz, viewDir);

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