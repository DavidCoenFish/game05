RWTexture2D<float4> g_output_texture : register(u0);

//https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/sv-groupthreadid
//SV_GroupID
//SV_GroupThreadID
//SV_DispatchThreadID
[numthreads(8, 8, 1)]
void main(uint3 in_dispatch_thread_id : SV_DispatchThreadID, uint3 in_group_id : SV_GroupID)
{
    float4 sampled_color = float4(0.0, 0.0, 0.0, 1.0);

    sampled_color.x = (float(in_dispatch_thread_id.x)) / 1920.0;
    sampled_color.y = (float(in_dispatch_thread_id.y)) / 1080.0;

    sampled_color.x = (float(in_group_id.x)) / 8.0;
    sampled_color.y = (float(in_group_id.y)) / 8.0;
    //sampled_color.z = step(0,
    //    (in_group_id.x + in_group_id.y + in_group_id.z) % 2
    //    );

    g_output_texture[in_dispatch_thread_id.xy] = sampled_color;
}