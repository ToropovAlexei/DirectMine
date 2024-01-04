struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 Color : COLOR;
    float3 Normal : NORMAL;
};

float4 main(VertexOut pin) : SV_TARGET
{
    return float4(pin.Color, 1.0f);
}