cbuffer ConstantBuffer0 : register(b0)
{
    float4 _camera_pos_fov_horizontal;
    float4 _camera_at_fov_vertical;
    float4 _camera_up_camera_far;
    float _camera_unit_pixel_size;
};

float3 GetCameraPos()
{
    return _camera_pos_fov_horizontal.xyz;
}

float3 GetCameraAt()
{
    return _camera_at_fov_vertical.xyz;
}

float3 GetCameraUp()
{
    return _camera_up_camera_far.xyz;
}

float GetCameraFar()
{
    return _camera_up_camera_far.w;
}

float GetCameraUnitPixelSize()
{
    return _camera_unit_pixel_size;
}

float3 MakeScreenEyeRay(float2 in_polar_coord) 
{
    // We want +z out of page, but clockwise around up axis of y

#if 0
    float temp = cos(in_polar_coord.y);
    float x = temp * cos(in_polar_coord.x);
    float y = sin(in_polar_coord.y);
    float z = -(temp * sin(in_polar_coord.x));
#else
    float temp = cos(in_polar_coord.y);
    float x = temp * cos(in_polar_coord.x);
    float y = sin(in_polar_coord.y);
    float z = -(temp * sin(in_polar_coord.x));
#endif

    return float3(x,y,z);
}

float3 MakeWorldEyeRay(float2 in_uv)
{
#if 0
    //negative uv.x maps to positive polar
    float2 screen_radian = float2((-in_uv.x * 0.5) * _camera_pos_fov_horizontal[3], (in_uv.y * 0.5) * _camera_at_fov_vertical[3]);
    float3 screen_eye_ray = MakeScreenEyeRay(screen_radian);

    // we are using y up, x to the right and z into page, which is left handed but croos is right, invert order to get right handed
    float3 camera_right = cross(_camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz);
    float3 result = float3(
        dot(_camera_at_fov_vertical.xyz, screen_eye_ray),
        dot(_camera_up_camera_far.xyz, screen_eye_ray),
        dot(camera_right, screen_eye_ray)
        );
#else
    //negative uv.x maps to positive polar
    float2 screen_radian = float2((in_uv.x * -0.5) * _camera_pos_fov_horizontal[3], (in_uv.y * 0.5) * _camera_at_fov_vertical[3]);
    float3 screen_eye_ray = MakeScreenEyeRay(screen_radian);

    // we are using y up, x to the right and z into page, which is left handed but croos is right, invert order to get right handed
    float3 camera_right = cross(_camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz);
    float3 tx = float3(_camera_at_fov_vertical.x, _camera_up_camera_far.x, camera_right.x);
    float3 ty = float3(_camera_at_fov_vertical.y, _camera_up_camera_far.y, camera_right.y);
    float3 tz = float3(_camera_at_fov_vertical.z, _camera_up_camera_far.z, camera_right.z);
    float3 result = float3(
        dot(screen_eye_ray, tx),
        dot(screen_eye_ray, ty),
        dot(screen_eye_ray, tz)
        );
#endif

    return result;
}
