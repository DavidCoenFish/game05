#include "bezier_interpolant.hlsli"

/*
C:\development\prototype\unity\test00\Assets\Resources\material

https://www.shadertoy.com/view/MtS3Dy

http://research.microsoft.com/en-us/um/people/hoppe/ravg.pdf
    inline float det(float2 a, float2 b) { return a.x*b.y-b.x*a.y; }
    // Find vector ???? given pixel ??=(0,0) and Bézier points ??0,??1,??2.
    float2 get_distance_vector(float2 b0, float2 b1, float2 b2) {
        float a=det(b0,b2), b=2*det(b1,b0), d=2*det(b2,b1); // ??,??,??(??)
        float f=b*d-a*a; // ??(??)
        float d21=b2-b1, d10=b1-b0, d20=b2-b0;
        float2 gf=2*(b*d21+d*d10+a*d20);
        gf=float2(gf.y,-gf.x); // ???(??)
        float2 pp=-f*gf/dot(gf,gf); // ???
        float2 d0p=b0-pp; // ??? to origin
        float ap=det(d0p,d20), bp=2*det(d10,d0p); // ??,??(???)
        // (note that 2*ap+bp+dp=2*a+b+d=4*area(b0,b1,b2))
        float t=clamp((ap+bp)/(2*a+b+d), 0,1); // ???
        return lerp(lerp(b0,b1,t),lerp(b1,b2,t),t); // ???? = ??(???
        )
    }

http://research.microsoft.com/en-us/um/people/cloop/LoopBlinn05.pdf
http://www.pouet.net/topic.php?which=9119&page=1
http://blog.gludion.com/2009/08/distance-to-quadratic-bezier-curve.html

The top link is where the get_distance_vector comes from.

Thanks also to other bezier curve shadertoys:
https://www.shadertoy.com/view/XsX3zf
https://www.shadertoy.com/view/lts3Df
*/
struct Pixel
{
    float4 _color : SV_TARGET0;
};

float distanceFunction(float input_t, float2 p0, float2 p1, float2 p2, float2 sample_point, float2 dim_scale, float2 thickness) 
{
    float t = clamp(input_t, 0.0, 1.0);
    float a = (1.0 - t) * (1.0 - t);
    float b = 2.0 * t * (1.0 - t);
    float c = t * t;
    float2 p = (a * p0) + (b * p1) + (c * p2);
    float2 offset = (p - sample_point) * dim_scale;
    float distance = length(offset);

    //float coverage = 1.0 - saturate(_width_height_thickness.z - sd);
    float current_thickness = ((1.0 - t) * thickness.x) + (t * thickness.y);
    float coverage = saturate(current_thickness - distance);

    return coverage;
}

static const float PI = 3.1415926535897932384626433832795;

// based on Olivier Besson (http://www.gludion.com)
static const float zeroMax = 0.0; //0.0000001;
float thirdDegreeEquationMin(float a, float b, float c, float d, float2 p0, float2 p1, float2 p2, float2 sample_point, float2 dim_scale, float2 thickness) 
{
    if (zeroMax < abs(a))
    {
        // let's adopt form: x3 + ax2 + bx + d = 0
        float z = a;
        a = b / z;
        b = c / z;
        c = d / z;
        // we solve using Cardan formula: http://fr.wikipedia.org/wiki/M%C3%A9thode_de_Cardan
        float p = b - ((a * a) / 3.0);
        float q = (a * ((2.0 * a * a) - (9.0 * b)) / 27.0) + c;
        float p3 = p * p * p;
        float D = (q * q) + ((4.0 * p3) / 27.0);
        float offset = -a / 3.0;
        if (zeroMax < D)
        {
            // D positive
            z = sqrt(D);
            float u = (-q + z) / 2.0;
            float v = (-q - z) / 2.0;
            if (0.0 <= u) 
            {
                u = pow(u, 1.0 / 3.0);
            }
            else 
            {
                u = -pow(-u, 1.0 / 3.0);
            }
            if (0.0 <= v) 
            {
                v = pow(v, 1.0 / 3.0);
            }
            else 
            {
                v = -pow(-v, 1.0 / 3.0);
            }
            float result = distanceFunction(u + v + offset, p0, p1, p2, sample_point, dim_scale, thickness);
            return result;
        }
        else if (D < -zeroMax)
        {
            // D negative
            float u = 2.0 * sqrt(-p / 3.0);
            float v = acos(-sqrt(-27.0 / p3) * q / 2.0) / 3.0;
            float result1 = distanceFunction(u * cos(v) + offset, p0, p1, p2, sample_point, dim_scale, thickness);
            float result2 = distanceFunction(u * cos(v + 2.0 * PI / 3.0) + offset, p0, p1, p2, sample_point, dim_scale, thickness);
            float result3 = distanceFunction(u * cos(v + 4.0 * PI / 3.0) + offset, p0, p1, p2, sample_point, dim_scale, thickness);
            float result = max(result1, max(result2, result3));
            return result;
        }
        else
        {
            // D zero
            float u = 0.0;
            if (q < 0.0) 
            {
                u = pow(-q / 2.0, 1.0 / 3.0);
            }
            else 
            {
                u = -pow(q / 2.0, 1.0 / 3.0);
            }
            float result1 = distanceFunction((2.0 * u) + offset, p0, p1, p2, sample_point, dim_scale, thickness);
            float result2 = distanceFunction(-u + offset, p0, p1, p2, sample_point, dim_scale, thickness);
            float result = max(result1, result2);
            return result;
        }
    }
    //else
    //{
    //    // a = 0, then actually a 2nd degree equation:
    //    // form : ax2 + bx + c = 0;
    //    a = b;
    //    b = c;
    //    c = d;
    //    if (abs(a) <= zeroMax)
    //    {
    //        if (abs(b) <= zeroMax) {
    //            return 0.0; //nullDistance;
    //        }
    //        else
    //        {
    //            float result = distanceFunction(-c / b, p0, p1, p2, sample_point, dim_scale, thickness);
    //            return result;
    //        }
    //    }
    //    float D = (b * b) - (4.0 * a * c);
    //    if (D <= -zeroMax) {
    //        return 0.0; //nullDistance;
    //    }
    //    if (zeroMax < D) {
    //        // D positive
    //        D = sqrt(D);
    //        float result1 = distanceFunction((-b - D) / (2.0 * a), p0, p1, p2, sample_point, dim_scale, thickness);
    //        float result2 = distanceFunction((-b + D) / (2.0 * a), p0, p1, p2, sample_point, dim_scale, thickness);
    //        float result = max(result1, result2);
    //        return result;
    //    }
    //    else if (D < -zeroMax) {
    //        // D negative
    //        return 0.0; //nullDistance;
    //    }
    //    else {
    //        // D zero
    //        float result = distanceFunction(-b / (2.0 * a), p0, p1, p2, sample_point, dim_scale, thickness);
    //        return result;
    //    }
    //}
    return 0.0;
}

// based on Olivier Besson (http://www.gludion.com)
float sdBezier(float2 sample_point, float2 p0, float2 p1, float2 p2, float2 dim_scale, float2 thickness) 
{
    float2 A = p1 - p0;
    float2 B = p0 - (2.0 * p1) + p2;

    float2 sampleRelative = p0 - sample_point;

    float a = (B.x * B.x) + (B.y * B.y);
    float b = 3.0 * ((A.x * B.x) + (A.y * B.y));
    float c = 2.0 * ((A.x * A.x) + (A.y * A.y)) + (sampleRelative.x * B.x) + (sampleRelative.y * B.y);
    float d = (sampleRelative.x * A.x) + (sampleRelative.y * A.y);
    float result = thirdDegreeEquationMin(a, b, c, d, p0, p1, p2, sample_point, dim_scale, thickness);

    return result;
}

float sdFunction(float2 sample_point, float2 p0, float2 p1, float2 p2, float2 dim_scale, float2 thickness)
{
    float result = sdBezier(sample_point, p0, p1, p2, dim_scale, thickness);
    return result;
}

Pixel main( Interpolant in_input )
{
    Pixel result;

    float coverage = sdFunction(in_input._uv, in_input._p0, in_input._p1, in_input._p2, in_input._dim, in_input._thickness);
    result._color = float4(0.0, 0.0, 0.0, coverage);

    return result;
}

