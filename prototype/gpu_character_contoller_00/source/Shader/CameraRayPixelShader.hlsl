cbuffer Constants : register(b0)
{
    float2 m_fovHorizontalVerticalRadian;
};

struct Interpolants
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

float2 makePolareCoords(float2 in_uv){
	float2 result = float2((in_uv.x - 0.5) * m_fovHorizontalVerticalRadian.x, (in_uv.y - 0.5) * m_fovHorizontalVerticalRadian.y);
	return result; 
}

float3 makeScreenEyeRay(float2 in_polarCoords) {
	float polar_a_radian = atan2(in_polarCoords.y, in_polarCoords.x);
	float polar_r_radian = length(in_polarCoords);

	float z = cos(polar_r_radian);
	float temp = sqrt(1.0 - (z * z));
	float x = temp * cos(polar_a_radian);
	float y = temp * sin(polar_a_radian);
	return float3(x, y, z);
}


Pixel main( Interpolants In )
{
	float2 polarCoords = makePolareCoords(In.uv);
	float3 screenEyeRay = makeScreenEyeRay(polarCoords);

	Pixel Out;
	Out.color = float4(screenEyeRay, 1.0);

	return Out;
}