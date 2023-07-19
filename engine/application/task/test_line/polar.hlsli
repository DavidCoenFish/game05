cbuffer ConstantBuffer0 : register(b0)
{
    float4 _camera_pos_fov_horizontal;
    float4 _camera_at_fov_vertical;
    float4 _camera_up_camera_far;
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

float3 MakeScreenEyeRay(float2 in_polar_coord) 
{
    float temp = cos(in_polar_coord.y);
    float x = temp * sin(in_polar_coord.x);
    float y = sin(in_polar_coord.y);
    float z = temp * cos(in_polar_coord.x);

    return float3(x,y,z);
}

float3 MakeWorldEyeRay(float2 in_uv)
{
    float2 screen_radian = float2((in_uv.x * 0.5) * _camera_pos_fov_horizontal[3], (in_uv.y * 0.5) * _camera_at_fov_vertical[3]);
    float3 screen_eye_ray = MakeScreenEyeRay(screen_radian);

    float3 camera_right = cross(_camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz);
    float3 result = float3(
        dot(camera_right, screen_eye_ray),
        dot(_camera_up_camera_far.xyz, screen_eye_ray),
        dot(_camera_at_fov_vertical.xyz, screen_eye_ray)
        );

    return result;
}
