cbuffer CBuf : register(b0)
{
    matrix world;
};

cbuffer CBuf2 : register(b1)
{
    matrix viewProj;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = mul(float4(vin.PosL, 1.0f), mul(world, viewProj));
    vout.Normal = vin.Normal;
    vout.TexC = vin.TexC;
    return vout;
}
