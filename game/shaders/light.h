#ifndef LIGHT_HDR
#define LIGHT_HDR

uniform vec3 gEyeWorldPosition;

#ifdef USE_SHADOWMAP
uniform sampler2DShadow gShadowMap;
#endif

struct baseLight {
    vec3 color;
    float ambient;
    float diffuse;
};

struct pointLight {
    baseLight base;
    vec3 position;
    float radius;
};

struct spotLight {
    pointLight base;
    vec3 direction;
    float cutOff;
};

struct directionalLight {
    baseLight base;
    vec3 direction;
};

#ifdef USE_SHADOWMAP
uniform mat4 gLightWVP;

float calcShadowFactor(vec3 shadowCoord) {
    vec2 scale = 1.0f / textureSize(gShadowMap, 0);
    shadowCoord.xy *= textureSize(gShadowMap, 0);
    vec2 offset = fract(shadowCoord.xy - 0.5f);
    shadowCoord.xy -= offset*0.5f;
    vec4 size = vec4(offset + 1.0f, 2.0f - offset);
    return (1.0f / 9.0f) * dot(size.zxzx*size.wwyy,
        vec4(texture(gShadowMap, vec3(scale * (shadowCoord.xy + vec2(-0.5f, -0.5f)), shadowCoord.z)),
             texture(gShadowMap, vec3(scale * (shadowCoord.xy + vec2(1.0f, -0.5f)), shadowCoord.z)),
             texture(gShadowMap, vec3(scale * (shadowCoord.xy + vec2(-0.5f, 1.0f)), shadowCoord.z)),
             texture(gShadowMap, vec3(scale * (shadowCoord.xy + vec2(1.0f, 1.0f)), shadowCoord.z))));
}
#endif

float calcLightFactor(float facing,
                      vec3 lightDirection,
                      vec3 worldPosition,
                      vec3 normal,
                      float diffuse,
                      vec2 spec)
{
    float attenuation = diffuse * facing;

    vec3 vertexToEye = gEyeWorldPosition - worldPosition;
    vec3 lightReflect = lightDirection + 2.0f * facing * normal;
    float specularFactor = dot(vertexToEye, lightReflect);
    if (specularFactor > 0.0f) {
        specularFactor *= inversesqrt(dot(vertexToEye, vertexToEye));
        attenuation += spec.x * pow(specularFactor, spec.y);
    }

    return attenuation;
}

vec4 calcDirectionalLight(directionalLight light,
                          vec3 worldPosition,
                          vec3 normal,
                          vec2 spec)
{
    float attenuation = light.base.ambient;
    float facing = dot(normal, -light.direction);
    if (facing > 0.0f) {
      attenuation += calcLightFactor(facing,
                                     light.direction,
                                     worldPosition,
                                     normal,
                                     light.base.diffuse,
                                     spec);
    }
    return vec4(light.base.color, 1.0f) * attenuation;
}

vec4 calcPointLight(pointLight light,
                    vec3 worldPosition,
                    vec3 normal,
                    vec2 spec)
{
    vec4 color = vec4(0.0f);

    vec3 lightDirection = worldPosition - light.position;
    float distanceSq = dot(lightDirection, lightDirection);
    if (distanceSq < light.radius * light.radius) {
        float attenuation = 0.0f;
        float facing = -dot(normal, lightDirection);
        if (facing > 0.0f) {
            float invDistance = inversesqrt(distanceSq);
            float attenuation = 1.0f - clamp(distanceSq * invDistance / light.radius, 0.0f, 1.0f);
#ifdef USE_SHADOWMAP
            vec3 absDir = abs(lightDirection);
            vec4 project =
                max(absDir.x, absDir.y) > absDir.z ?
                    (absDir.x > absDir.y ?
                        vec4(lightDirection.zyx, 0.0f) :
                        vec4(lightDirection.xzy, 1.0f / 3.0f)) :
                    vec4(lightDirection, 2.0f / 3.0f);
            vec4 shadowCoord = gLightWVP * vec4(project.xy, abs(project.z), 1.0f);
            attenuation *= calcShadowFactor(shadowCoord.xyz / shadowCoord.w + vec3(project.w, 0.5f * step(0.0f, project.z), 0.0f));
#endif
            attenuation *= calcLightFactor(facing * invDistance,
                                           lightDirection * invDistance,
                                           worldPosition,
                                           normal,
                                           light.base.diffuse,
                                           spec);
            color = vec4(light.base.color, 1.0f) * attenuation;
        }
    }

    return color;
}

vec4 calcSpotLight(spotLight light,
                   vec3 worldPosition,
                   vec3 normal,
                   vec2 spec)
{
    vec4 color = vec4(0.0f);

    vec3 lightDirection = worldPosition - light.base.position;
    float distanceSq = dot(lightDirection, lightDirection);
    if (distanceSq < light.base.radius * light.base.radius) {
        float facing = -dot(normal, lightDirection);
        if (facing > 0.0f) {
            float invDistance = inversesqrt(distanceSq);
            float spotFactor = dot(lightDirection, light.direction) * invDistance;
            if (spotFactor > light.cutOff) {
                float attenuation = 1.0f - clamp(distanceSq * invDistance / light.base.radius, 0.0f, 1.0f);
                attenuation *= 1.0f - (1.0f - spotFactor) / (1.0f - light.cutOff);
#ifdef USE_SHADOWMAP
                vec4 shadowCoord = gLightWVP * vec4(worldPosition, 1.0f);
                attenuation *= calcShadowFactor(shadowCoord.xyz / shadowCoord.w);
#endif
                attenuation *= calcLightFactor(facing * invDistance,
                                               lightDirection * invDistance,
                                               worldPosition,
                                               normal,
                                               light.base.base.diffuse,
                                               spec);
                color = vec4(light.base.base.color, 1.0f) * attenuation;
            }
        }
    }

    return color;
}

#endif
