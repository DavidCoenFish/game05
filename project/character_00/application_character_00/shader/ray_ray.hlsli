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


/*
	https://stackoverflow.com/questions/2316490/the-algorithm-to-find-the-point-of-intersection-of-two-3d-line-segment/10288710#10288710

	result[0] != 0.0 if result valid
	result[1] distance along ray A of the nearest point between rays
	result[2] distance along ray B of the nearest point between rays
*/
//float3 IntersectRayRay(float3 in_a_pos, float3 in_a_norm, float3 in_b_pos, float3 in_b_norm)
//{
//	float3 result = float3(0.0, 0.0, 0.0);
//	float3 c = float3(in_b_pos.x - in_a_pos.x, in_b_pos.y - in_a_pos.y, in_b_pos.z - in_a_pos.z);
//	//~a.~b	~b.~c	~a.~c	~a.~a	~b.~b
//	float ab = dot(in_a_norm, in_b_norm);
//	float bc = dot(in_b_norm, c);
//	float ac = dot(in_a_norm, c);
//	float aa = dot(in_a_norm, in_a_norm);
//	float bb = dot(in_b_norm, in_b_norm);
//
//	//D=A + ~a * ((-(~a.~b)(~b.~c) + (~a.~c)(~b.~b))/((~a.~a)(~b.~b)-(~a.~b)(~a.~b))
//	//E=B + ~b((~a.~b)(~a.~c)-(~b.~c)(~a.~a))/((~a.~a)(~b.~b)-(~a.~b)(~a.~b))
//	float denominator = (aa * bb) - (ab * ab); 
//	if (0.0 == denominator)
//	{
//		return result;
//	}
//
//	result[0] = 1.0;
//	result[1] = ((ac * bb) - (ab * bc)) / denominator;
//	result[2] = ((ab * ac) - (bc * aa)) / denominator;
//
//	return result;
//}


