struct VS_In
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VS_Out
{
    float4 position : SV_Position;
    float3 color : COLOR;
};


cbuffer Constants : register(b0)
{
    struct
    {
        matrix translation;
    } constants;
}
