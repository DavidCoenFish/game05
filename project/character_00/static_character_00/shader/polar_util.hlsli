float3 PolarToVector(float2 in_polar_coord) 
{
    // We want +z out of page, anti-clockwise around up axis of y
    float temp = cos(in_polar_coord.y);
    float x = temp * cos(in_polar_coord.x);
    float y = sin(in_polar_coord.y);
    float z = temp * sin(in_polar_coord.x);

    return float3(x,y,z);
}
