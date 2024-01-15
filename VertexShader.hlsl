cbuffer CBuf2 : register(b0)
{
    matrix viewProj;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 TexC : TEXCOORD;
    float Depth : DEPTH;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = mul(float4(vin.PosL, 1.0f), viewProj);
    vout.TexC = vin.TexC;
    vout.Depth = vout.PosH.z;
    return vout;
}
