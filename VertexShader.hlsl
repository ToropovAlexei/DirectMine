cbuffer CBuf : register(b0)
{
    float3 worldPos;
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

float4x4 ConvertFloat3ToMatrix(float3 position)
{
    matrix mat;
    
    mat[0] = float4(1.0f, 0.0f, 0.0f, 0.0f);
    mat[1] = float4(0.0f, 1.0f, 0.0f, 0.0f);
    mat[2] = float4(0.0f, 0.0f, 1.0f, 0.0f);
    mat[3] = float4(position, 1.0f);
    
    return mat;
}

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = mul(float4(vin.PosL, 1.0f), mul(ConvertFloat3ToMatrix(worldPos), viewProj));
    vout.Normal = vin.Normal;
    vout.TexC = vin.TexC;
    return vout;
}
