struct VS_In
{
    float2 position : POSITION;
    float4 color : COLOR;
};

struct VS_Out
{
    float4 position : SV_Position;
    float4 color : COLOR;
};
