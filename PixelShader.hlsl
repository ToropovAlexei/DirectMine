Texture2D tex : register(t0);
SamplerState splr : register(s0);

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
    float Depth : DEPTH;
};

#define FOG_COLOR float4(0.8f, 0.8f, 0.8f, 1.0f)
#ifndef NDEBUG
    #define FOG_START 180.0f
    #define FOG_END 230.0f
#else
    #define FOG_START 100.0f
    #define FOG_END 150.0f
#endif

float CalculateFogFactor(float depth)
{
    float fogFactor = (depth - FOG_START) / (FOG_END - FOG_START);
    fogFactor = saturate(fogFactor);

    return fogFactor;
}

float4 main(VertexOut pin) : SV_Target
{
    float fogFactor = CalculateFogFactor(pin.Depth);
    float4 finalColor = lerp(tex.Sample(splr, pin.TexC), FOG_COLOR, fogFactor);
    return finalColor;
}