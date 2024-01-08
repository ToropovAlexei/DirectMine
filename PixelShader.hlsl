Texture2D tex : register(t0);
SamplerState splr : register(s0);

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
};

float4 main(VertexOut pin) : SV_Target
{
    return tex.Sample(splr, pin.TexC);
}