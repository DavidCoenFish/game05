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
