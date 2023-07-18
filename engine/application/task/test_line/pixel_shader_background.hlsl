struct Interpolant
{
    float4 _position : SV_Position;
    float2 _uv : TEXCOORD0;
};

struct Pixel
{
    float4 _color : SV_TARGET0;
};

cbuffer ConstantBuffer0 : register(b0)
{
    float4 _camera_pos_fov_horizontal;
    float4 _camera_at_fov_vertical;
    float4 _camera_up_camera_far;
};

float3 MakeScreenEyeRay(float2 in_polar_coord) 
{
    float temp = cos(in_polar_coord.y);
    float x = temp * sin(in_polar_coord.x);
    float y = sin(in_polar_coord.y);
    float z = temp * cos(in_polar_coord.x);

    return float3(x,y,z);
}

float3 MakeWorldEyeRay(float3 in_ray, float3 in_at, float3 in_up, float3 in_right)
{
    float x = dot(in_right, in_ray);
    float y = dot(in_up, in_ray);
    float z = dot(in_at, in_ray);

    return float3(x, y, z);
}

Pixel main( Interpolant in_input )
{
    Pixel result;

    float2 screen_radian = float2((in_input._uv.x * 0.5) * _camera_pos_fov_horizontal[3], (in_input._uv.y * 0.5) * _camera_at_fov_vertical[3]);
    float3 screen_eye_ray = MakeScreenEyeRay(screen_radian);

    float3 camera_right = cross(_camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz);
    float3 world_eye_ray = MakeWorldEyeRay(screen_eye_ray, _camera_at_fov_vertical.xyz, _camera_up_camera_far.xyz, camera_right);

    result._color = float4(world_eye_ray.x, world_eye_ray.y, world_eye_ray.z, 1.0);

    return result;
}