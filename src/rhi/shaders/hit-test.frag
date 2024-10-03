//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#version 450
#extension GL_EXT_scalar_block_layout : require

#include "uniforms.hxx"

layout (location = 0) in vec3 in_world;

layout (location = 0) out uint out_normal;

layout (set = 0, binding = 0, std430) uniform Camera {
    CameraUniform u_camera;
};

layout (set = 0, binding = 1, std430) uniform Model {
    ModelUniform u_model;
};

float normal_sign(float x)
{
    return (x < 0.0) ? -1.0 : 1.0;
}

uint compress_normal(vec3 n)
{
    float inv = 1.0 / (abs(n.x) + abs(n.y) + abs(n.z));

    float fx = 0.0;
    float fy = 0.0;
    if (n.z < 0.0)
    {
        fx = (1.0 - abs(n.y * inv)) * normal_sign(n.x);
        fy = (1.0 - abs(n.x * inv)) * normal_sign(n.y);
    }
    else
    {
        fx = n.x * inv;
        fy = n.y * inv;
    }

    uint ux = uint(clamp(0.5 * (fx + 1.0), 0.0, 1.0) * 65535.0);
    uint uy = uint(clamp(0.5 * (fy + 1.0), 0.0, 1.0) * 65535.0);

    return (ux << 16) | uy;
}

void main()
{
    vec3 pos_world = in_world;

    vec3 dx = dFdx(pos_world);
    vec3 dy = dFdy(pos_world);
    vec3 normal_world = normalize(cross(dy, dx));
    
    out_normal = compress_normal(normal_world);
}
