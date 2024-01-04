struct VertexIn
{
    float3 PosL : POSITION;
    float3 Color : COLOR;
    float3 Normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 Color : COLOR;
    float3 Normal : NORMAL;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = float4(vin.PosL, 1.0f);
    vout.Color = vin.Color;
    vout.Normal = vin.Normal;
    return vout;
}