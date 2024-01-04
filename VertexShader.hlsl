cbuffer CBuf
{
    matrix transform;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 Color : COLOR;
    float3 Normal : NORMAL;
};

float4 main(VertexIn vin) : SV_Position
{
    return mul(float4(vin.PosL, 1.0f), transform);
}
