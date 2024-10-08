#include <Default.hlsli>

VS_Out main(VS_In input)
{
    VS_Out output;
    output.position = float4(input.position, 1.0);
    return output;
}
