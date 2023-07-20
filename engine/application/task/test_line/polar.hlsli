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
    float temp = cos(in_polar_coord.y);
    float x = temp * cos(in_polar_coord.x);
    float y = temp * sin(in_polar_coord.x);
    float z = sin(in_polar_coord.y);

    return float3(x,y,z);
}

float3 MakeWorldEyeRay(float2 in_uv)
{
    float2 screen_radian = float2((in_uv.x * 0.5) * _camera_pos_fov_horizontal[3], (in_uv.y * 0.5) * _camera_at_fov_vertical[3]);
    float3 screen_eye_ray = MakeScreenEyeRay(screen_radian);

    // we are using y up, x to the right and z into page, which is left handed but croos is right, invert order to get right handed
    float3 camera_right = cross(_camera_up_camera_far.xyz, _camera_at_fov_vertical.xyz);
    float3 result = float3(
        dot(_camera_at_fov_vertical.xyz, screen_eye_ray),
        dot(camera_right, screen_eye_ray),
        dot(_camera_up_camera_far.xyz, screen_eye_ray)
        );

    return result;
}
