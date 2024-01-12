struct VertexOut
{
    float4 PosH : SV_POSITION;
};

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}