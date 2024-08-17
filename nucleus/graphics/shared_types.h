#ifndef NU_GRAPHICS_SHARED_TYPES_H
#define NU_GRAPHICS_SHARED_TYPES_H

typedef enum
{
    NU_RENDERPASS_UNLIT,
    NU_RENDERPASS_FLAT,
    NU_RENDERPASS_TRANSPARENT,
    NU_RENDERPASS_WIREFRAME,
    NU_RENDERPASS_SKYBOX,
    NU_RENDERPASS_CANVAS,
} nu_renderpass_type_t;

typedef enum
{
    NU_MATERIAL_MESH,
    NU_MATERIAL_CANVAS,
} nu_material_type_t;

typedef enum
{
    NU_TEXTURE_WRAP_CLAMP,
    NU_TEXTURE_WRAP_REPEAT,
    NU_TEXTURE_WRAP_MIRROR
} nu_texture_wrap_mode_t;

#endif
