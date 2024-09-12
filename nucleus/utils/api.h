#ifndef NU_UTILS_API_H
#define NU_UTILS_API_H

#include <nucleus/graphics/api.h>

//////////////////////////////////////////////////////////////////////////
//////                               UI                             //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_UI_BUTTON,
    NU_UI_CHECKBOX,
    NU_UI_CURSOR,
} nu_ui_element_t;

typedef struct
{
    nu_u32_t top;
    nu_u32_t bottom;
    nu_u32_t left;
    nu_u32_t right;
} nu_ui_margin_t;

typedef struct
{
    nu_material_t  material;
    nu_rect_t      extent;
    nu_ui_margin_t margin;
} nu_ui_image_style_t;

typedef struct
{
    nu_ui_element_t type;
    union
    {
        struct
        {
            nu_ui_image_style_t pressed;
            nu_ui_image_style_t released;
            nu_ui_image_style_t hovered;
        } button;
        struct
        {
            nu_ui_image_style_t checked;
            nu_ui_image_style_t unchecked;
        } checkbox;
        struct
        {
            nu_ui_image_style_t image;
        } cursor;
    };
} nu_ui_style_t;

typedef nu_vec(nu_ui_style_t) nu_ui_style_vec_t;

typedef enum
{
    NU_UI_CONTROLLER_DISABLED,
    NU_UI_CONTROLLER_SELECTION,
    NU_UI_CONTROLLER_CURSOR,
} nu_ui_controller_mode_t;

NU_DEFINE_HANDLE(nu_ui_t);

NU_API void nu_blit_sliced(nu_renderpass_t pass,
                           nu_material_t   handle,
                           nu_rect_t       extent,
                           nu_rect_t       tex_extent,
                           nu_ui_margin_t  margin);

NU_API nu_ui_t nu_ui_create(void);
NU_API void    nu_ui_delete(nu_ui_t ui);

NU_API void nu_ui_set_cursor(nu_ui_t ui, nu_u32_t controller, nu_ivec2_t pos);
NU_API void nu_ui_set_pressed(nu_ui_t   ui,
                              nu_u32_t  controller,
                              nu_bool_t pressed);

NU_API void nu_ui_begin(nu_ui_t ui);
NU_API void nu_ui_end(nu_ui_t ui);
NU_API void nu_ui_submit_renderpasses(nu_ui_t                       ui,
                                      const nu_renderpass_submit_t *info);

NU_API void nu_ui_push_style(nu_ui_t ui, nu_ui_style_t *style);
NU_API void nu_ui_pop_style(nu_ui_t ui);

NU_API nu_u32_t nu_ui_controller(nu_ui_t ui);

NU_API nu_bool_t nu_ui_button(nu_ui_t ui, nu_rect_t extent);
NU_API nu_bool_t nu_ui_checkbox(nu_ui_t ui, nu_rect_t extent, nu_bool_t *state);

//////////////////////////////////////////////////////////////////////////
//////                       Controller                             //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_CONTROLLER_FREEFLY_ALIGNED,
    NU_CONTROLLER_FREEFLY,
    NU_CONTROLLER_CHARACTER
} nu_controller_mode_t;

typedef struct
{
    nu_input_t view_pitch_neg;
    nu_input_t view_pitch_pos;
    nu_input_t view_yaw_neg;
    nu_input_t view_yaw_pos;
    nu_input_t view_roll_neg;
    nu_input_t view_roll_pos;
    nu_input_t move_up;
    nu_input_t move_down;
    nu_input_t move_left;
    nu_input_t move_right;
    nu_input_t move_forward;
    nu_input_t move_backward;
    nu_input_t switch_mode;
} nu_controller_info_t;

NU_DEFINE_HANDLE(nu_controller_t);

NU_API nu_controller_t nu_controller_create(const nu_controller_info_t *info);
NU_API void            nu_controller_delete(nu_controller_t controller);
NU_API void            nu_controller_update(nu_controller_t   controller,
                                            float             dt,
                                            nu_camera_info_t *info);

//////////////////////////////////////////////////////////////////////////
//////                        Geometry                              //////
//////////////////////////////////////////////////////////////////////////

#define NU_CUBE_VERTEX_COUNT  36
#define NU_PLANE_VERTEX_COUNT 6

NU_DEFINE_HANDLE(nu_geometry_t);

NU_API nu_geometry_t nu_geometry_create(nu_size_t capacity);
NU_API void          nu_geometry_delete(nu_geometry_t geometry);

NU_API void nu_geometry_cube(nu_geometry_t geometry, float unit);
NU_API void nu_geometry_plane(nu_geometry_t geometry,
                              float         width,
                              float         height);
NU_API void nu_geometry_grid(nu_geometry_t geometry,
                             nu_u32_t      width,
                             nu_u32_t      height,
                             float         unit,
                             float         uv_scale);
NU_API void nu_geometry_transform(nu_geometry_t geometry, nu_mat4_t m);
NU_API void nu_geometry_append(nu_geometry_t dst, nu_geometry_t src);

NU_API nu_mesh_t nu_mesh_create_geometry(nu_geometry_t geometry);

#endif
