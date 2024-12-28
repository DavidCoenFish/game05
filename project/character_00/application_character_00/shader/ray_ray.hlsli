// https://en.wikipedia.org/wiki/Skew_lines#Distance%5B/url%5D
float3 RayRayClosest(float3 in_p1, float3 in_d1, float3 in_p2, float3 in_d2)
{
    float3 n = cross(in_d1, in_d2);
    float3 n1 = float3(0.0, 0.0, 0.0);
    float3 n2 = float3(0.0, 0.0, 0.0);
    float t1 = 0.0f;
    float t2 = 0.0f;
    float3 result = float3(0.0, 0.0, 0.0);

    // rays are not that close to parrallel
    if (0.000001 < dot(n, n))
    {
        n1 = cross(in_d1, n);
        n2 = cross(in_d2, n);

        float denom1 = dot(in_d1, n2);
        float denom2 = dot(in_d2, n1);
        if ((0.000001 < abs(denom1)) &&
            (0.000001 < abs(denom2)))
        {
            t1 = dot(in_p2 - in_p1, n2) / denom1;
            t2 = dot(in_p1 - in_p2, n1) / denom2;
            result = float3(1.0, t1, t2);
        }
    }

    return result;
}

float2 CalculateDistance(float in_t1, float in_t2, float3 in_p1, float3 in_d1, float in_l1, float3 in_p2, float3 in_d2, float in_l2)
{
    float3 c1 = float3(0.0, 0.0, 0.0);
    float3 c2 = float3(0.0, 0.0, 0.0);
    float d = 0.0;
    float2 result = float2(0.0, 0.0);

    if ((in_t1 < 0.0) ||
        (in_l1 < in_t1) ||
        (in_t2 < 0.0) ||
        (in_l2 < in_t2))
    {
        // Nop
    }
    else
    {
        c1 = in_p1 + (in_d1 * in_t1);
        c2 = in_p2 + (in_d2 * in_t2);
        d = length(c1 - c2);
        result = float2(1.0, d);
    }

    return result;
}
