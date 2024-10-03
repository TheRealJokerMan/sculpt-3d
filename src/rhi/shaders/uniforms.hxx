//
// Copyright (c) 2024 Jamie Kenyon. All Rights Reserved.
//

#if defined(__cplusplus)
#    include <glm/mat4x4.hpp>
#    include <glm/vec2.hpp>
#    include <glm/vec3.hpp>
#    include <glm/vec4.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using uint = uint32_t;
#endif

#define GROUP_SIZE 256

/// A camera.
struct CameraUniform
{
    mat4 projection; ///< The view-projection matrix.
    vec4 eye;        ///< Eye position.
};

/// A model.
struct ModelUniform
{
    mat4 model; ///< The model's transform.
};

/// A brush.
struct BrushUniform
{
    vec3  p;    ///< Position.
    float pad0; ///< Padding.

    vec3  n;    ///< Normal.
    float pad1; ///< Padding.

    vec3  colour; ///< Colour.
    float amount; ///< Amount.

    float r;      ///< Radius.
    float r_sqrd; ///< Radius squared.
    float scale;  ///< Scale.
    float offset; ///< Offset.
};
