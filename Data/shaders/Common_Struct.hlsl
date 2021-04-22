/*
Copyright(c) 2016-2021 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES =========
#include "Common.hlsl"
//====================

struct Surface
{
    // Properties
    float3  albedo;
    float   alpha;
    float   roughness;
    float   metallic;
    float   clearcoat;
    float   clearcoat_roughness;
    float   anisotropic;
    float   anisotropic_rotation;
    float   sheen;
    float   sheen_tint;
    float3  occlusion;
    float3  emissive;
    float3  F0;
    int     id;
    float2  uv;
    float   depth;
    float3  position;
    float3  normal;
    float3  camera_to_pixel;
    float   camera_to_pixel_length;
    
    // Activision GTAO paper: https://www.activision.com/cdn/research/s2016_pbs_activision_occlusion.pptx
    float3 multi_bounce_ao(float visibility, float3 albedo)
    {
        float3 a    = 2.0404 * albedo - 0.3324;
        float3 b    = -4.7951 * albedo + 0.6417;
        float3 c    = 2.7552 * albedo + 0.6903;
        float x     = visibility;
        return max(x, ((x * a + b) * x + c) * x);
    }
    
    void Build(uint2 position_screen, bool use_albedo = true)
    {
        // Sample render targets
        float4 sample_albedo    = tex_albedo[position_screen];
        float4 sample_normal    = tex_normal[position_screen];
        float4 sample_material  = tex_material[position_screen];
        float sample_depth      = tex_depth[position_screen].r;

        // Misc
        uv      = (position_screen + 0.5f) / g_resolution;
        depth   = sample_depth;
        id      = round(sample_normal.a * 65535);
        
        albedo      = use_albedo ? sample_albedo.rgb : 1.0f;
        alpha       = sample_albedo.a;
        roughness   = sample_material.r;
        metallic    = sample_material.g;
        emissive    = sample_material.b * sample_albedo.rgb * 50.0f;
        F0          = lerp(0.04f, albedo, metallic);
        
        clearcoat               = mat_clearcoat_clearcoatRough_aniso_anisoRot[id].x;
        clearcoat_roughness     = mat_clearcoat_clearcoatRough_aniso_anisoRot[id].y;
        anisotropic             = mat_clearcoat_clearcoatRough_aniso_anisoRot[id].z;
        anisotropic_rotation    = mat_clearcoat_clearcoatRough_aniso_anisoRot[id].w;
        sheen                   = mat_sheen_sheenTint_pad[id].x;
        sheen_tint              = mat_sheen_sheenTint_pad[id].y;

        // Occlusion
        float occlusion_ssao    = !g_is_transparent_pass ? tex_ssao.SampleLevel(sampler_point_clamp, uv, 0).r : 1.0f; // if ssao is disabled, the texture will be 1x1 white pixel, so we use a sampler
        float occlusion_tex     = sample_material.a;
        float _occlusion        = min(occlusion_tex, occlusion_ssao);
        occlusion               = multi_bounce_ao(_occlusion, sample_albedo.rgb);
        
        // Reconstruct position from depth
        float x             = uv.x * 2.0f - 1.0f;
        float y             = (1.0f - uv.y) * 2.0f - 1.0f;
        float4 pos_clip     = float4(x, y, depth, 1.0f);
        float4 pos_world    = mul(pos_clip, g_view_projection_inverted);
        position            = pos_world.xyz / pos_world.w;

        normal                  = normalize(sample_normal.xyz);
        camera_to_pixel         = position - g_camera_position.xyz;
        camera_to_pixel_length  = length(camera_to_pixel);
        camera_to_pixel         = normalize(camera_to_pixel);
    }

    bool is_sky()           { return id == 0; }
    bool is_transparent()   { return alpha != 1.0f; }
    bool is_opaque()        { return alpha == 1.0f; }
};

struct Light
{
    // Properties
    float3  color;
    float3  position;
    float3  direction;
    float   distance_to_pixel;
    float   angle;
    float   bias;
    float   normal_bias;
    float   near;
    float   far;
    float   attenuation;
    float   intensity;
    float3  radiance;
    float   n_dot_l;
    uint    array_size;

    // Attenuation over distance
    float compute_attenuation_distance(const float3 surface_position)
    {
        float distance_to_pixel = length(surface_position - position);
        float attenuation       = saturate(1.0f - distance_to_pixel / far);
        return attenuation * attenuation;
    }

    // Attenuation over angle (approaching the outer cone)
    float compute_attenuation_angle(const float3 direction)
    {
        float light_dot_pixel   = dot(direction, direction);
        float cutoffAngle       = 1.0f - angle;
        float epsilon           = cutoffAngle - cutoffAngle * 0.9f;
        float attenuation       = saturate((light_dot_pixel - cutoffAngle) / epsilon);
        return attenuation * attenuation;
    }

    // Final attenuation for all suported lights
    float compute_attenuation(const float3 surface_position)
    {
        float attenuation = 0.0f;
        
        #if DIRECTIONAL
        attenuation   = saturate(dot(-cb_light_direction.xyz, float3(0.0f, 1.0f, 0.0f)));
        #elif POINT
        attenuation   = compute_attenuation_distance(surface_position);
        #elif SPOT
        attenuation   = compute_attenuation_distance(surface_position) * compute_attenuation_angle(cb_light_direction.xyz);
        #endif
    
        return attenuation;
    }
    
    float3 compute_direction(float3 light_position, Surface surface)
    {
        float3 direction = 0.0f;
        
        #if DIRECTIONAL
        direction   = normalize(cb_light_direction.xyz);
        #elif POINT
        direction   = normalize(surface.position - light_position);
        #elif SPOT
        direction   = normalize(surface.position - light_position);
        #endif
    
        return direction;
    }
    
    void Build(Surface surface)
    {
        color             = cb_light_color.rgb;
        position          = cb_light_position.xyz;
        near              = 0.1f;
        intensity         = cb_light_intensity_range_angle_bias.x;
        far               = cb_light_intensity_range_angle_bias.y;
        angle             = cb_light_intensity_range_angle_bias.z;
        bias              = cb_light_intensity_range_angle_bias.w;
        normal_bias       = cb_light_normal_bias;
        distance_to_pixel = length(surface.position - position);
        direction         = compute_direction(position, surface);
        attenuation       = compute_attenuation(surface.position);
        n_dot_l           = saturate(dot(surface.normal, -direction)); // Pre-compute n_dot_l since it's used in many places
        radiance          = color * intensity * attenuation * n_dot_l * surface.occlusion;
        #if DIRECTIONAL
        array_size = 4;
        #else
        array_size = 1;
        #endif
    }
};
