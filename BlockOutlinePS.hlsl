struct VertexOut
{
    float4 PosH : SV_POSITION;
};

float4 main() : SV_TARGET
{
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}