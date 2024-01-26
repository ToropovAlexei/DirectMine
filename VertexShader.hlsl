cbuffer CBuf2 : register(b0)
{
    matrix viewProj;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 TexC : TEXCOORD;
    min16uint Light : LIGHT;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 TexC : TEXCOORD;
    float Depth : DEPTH;
    float4 Light : LIGHT;
};

float4 lightToChannels(min16uint light)
{
    float4 result;
    result.a = ((light >> 12) & 0xF) / 15.0f;
    result.r = ((light >> 8) & 0xF) / 15.0f;
    result.g = ((light >> 4) & 0xF) / 15.0f;
    result.b = (light & 0xF) / 15.0f;
    return result;
}

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = mul(float4(vin.PosL, 1.0f), viewProj);
    vout.TexC = vin.TexC;
    vout.Depth = vout.PosH.z;
    vout.Light = lightToChannels(vin.Light);
    return vout;
}
