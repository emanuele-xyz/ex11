#include <Default.hlsli>

float4 main(VS_Out input) : SV_Target
{
    return float4(input.color, 1.0f);
}
