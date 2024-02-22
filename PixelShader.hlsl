Texture2DArray tex : register(t0);
SamplerState splr : register(s0);

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 TexC : TEXCOORD;
    float Depth : DEPTH;
    float4 Light : LIGHT;
};

#define FOG_COLOR float4(0.8f, 0.8f, 0.8f, 1.0f)
#ifndef NDEBUG
    #define FOG_START 320.0f
    #define FOG_END 360.0f
#else
    #define FOG_START 250.0f
    #define FOG_END 290.0f
#endif

float CalculateFogFactor(float depth)
{
    float fogFactor = (depth - FOG_START) / (FOG_END - FOG_START);
    fogFactor = saturate(fogFactor);

    return fogFactor;
}

float4 main(VertexOut pin) : SV_Target
{
    float4 texColor = tex.Sample(splr, pin.TexC);
    clip(texColor.a < 0.5f ? -1 : 1);
    float fogFactor = CalculateFogFactor(pin.Depth);
    //float4 finalColor = lerp(tex.Sample(splr, pin.TexC), FOG_COLOR, fogFactor);
    float3 light = pin.Light.rgb;
    light.r = max(max(light.r, 0.1f), pin.Light.a);
    light.g = max(max(light.g, 0.1f), pin.Light.a);
    light.b = max(max(light.b, 0.1f), pin.Light.a);
    float4 finalColor = texColor * float4(light, 1.0f);
    return finalColor;
}