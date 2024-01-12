cbuffer Cbuf : register(b0)
{
    matrix viewProj;
};

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    output.PosH = mul(float4(input.PosL, 1.0f), viewProj);
    return output;
}