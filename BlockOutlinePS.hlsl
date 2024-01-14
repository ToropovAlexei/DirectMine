struct VertexOut
{
    float4 PosH : SV_POSITION;
};

float4 main() : SV_TARGET
{
	return float4(0.05f, 0.05f, 0.05f, 1.0f);
}