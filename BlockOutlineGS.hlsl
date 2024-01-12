//#pragma warning (disable:3206)
//#pragma warning (disable:3554)

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

static const float PI = 3.1415926f;
static const float fRatio = 2.0f;
static float fThickness = 0.01f;

void addHalfCircle(inout TriangleStream<PS_INPUT> triangleStream, int nCountTriangles, float4 linePointToConnect, float fPointWComponent, float fAngle)
{
    PS_INPUT output = (PS_INPUT) 0;
    for (int nI = 0; nI < nCountTriangles; ++nI)
    {
        output.Pos.x = cos(fAngle + (PI / nCountTriangles * nI)) * fThickness / fRatio;
        output.Pos.y = sin(fAngle + (PI / nCountTriangles * nI)) * fThickness;
        output.Pos.z = 0.0f;
        output.Pos.w = 0.0f;
        output.Pos += linePointToConnect;
        output.Pos *= fPointWComponent;
        triangleStream.Append(output);

        output.Pos = linePointToConnect * fPointWComponent;
        triangleStream.Append(output);

        output.Pos.x = cos(fAngle + (PI / nCountTriangles * (nI + 1))) * fThickness / fRatio;
        output.Pos.y = sin(fAngle + (PI / nCountTriangles * (nI + 1))) * fThickness;
        output.Pos.z = 0.0f;
        output.Pos.w = 0.0f;
        output.Pos += linePointToConnect;
        output.Pos *= fPointWComponent;
        triangleStream.Append(output);

        triangleStream.RestartStrip();
    }
}

[maxvertexcount(42)]
void main(line PS_INPUT input[2], inout TriangleStream<PS_INPUT> triangleStream)
{
    PS_INPUT output = (PS_INPUT) 0;

    int nCountTriangles = 6;

    float4 positionPoint0Transformed = input[0].Pos;
    float4 positionPoint1Transformed = input[1].Pos;

    float fPoint0w = positionPoint0Transformed.w;
    float fPoint1w = positionPoint1Transformed.w;

    //calculate out the W parameter, because of usage of perspective rendering
    positionPoint0Transformed.xyz = positionPoint0Transformed.xyz / positionPoint0Transformed.w;
    positionPoint0Transformed.w = 1.0f;
    positionPoint1Transformed.xyz = positionPoint1Transformed.xyz / positionPoint1Transformed.w;
    positionPoint1Transformed.w = 1.0f;

    //calculate the angle between the 2 points on the screen
    float3 positionDifference = positionPoint0Transformed.xyz - positionPoint1Transformed.xyz;
    float3 coordinateSysten = float3(1.0f, 0.0f, 0.0f);

    positionDifference.z = 0.0f;
    coordinateSysten.z = 0.0f;

    float fAngle = acos(dot(positionDifference.xy, coordinateSysten.xy) / (length(positionDifference.xy) * length(coordinateSysten.xy)));

    if (cross(positionDifference, coordinateSysten).z < 0.0f)
    {
        fAngle = 2.0f * PI - fAngle;
    }

    fAngle *= -1.0f;
    fAngle -= PI * 0.5f;

    //first half circle of the line
    addHalfCircle(triangleStream, nCountTriangles, positionPoint0Transformed, fPoint0w, fAngle);
    addHalfCircle(triangleStream, nCountTriangles, positionPoint1Transformed, fPoint1w, fAngle + PI);

    //connection between the two circles
    //triangle1
    output.Pos.x = cos(fAngle) * fThickness / fRatio;
    output.Pos.y = sin(fAngle) * fThickness;
    output.Pos.z = 0.0f;
    output.Pos.w = 0.0f;
    output.Pos += positionPoint0Transformed;
    output.Pos *= fPoint0w; //undo calculate out the W parameter, because of usage of perspective rendering
    triangleStream.Append(output);

    output.Pos.x = cos(fAngle + (PI / nCountTriangles * (nCountTriangles))) * fThickness / fRatio;
    output.Pos.y = sin(fAngle + (PI / nCountTriangles * (nCountTriangles))) * fThickness;
    output.Pos.z = 0.0f;
    output.Pos.w = 0.0f;
    output.Pos += positionPoint0Transformed;
    output.Pos *= fPoint0w;
    triangleStream.Append(output);

    output.Pos.x = cos(fAngle + (PI / nCountTriangles * (nCountTriangles))) * fThickness / fRatio;
    output.Pos.y = sin(fAngle + (PI / nCountTriangles * (nCountTriangles))) * fThickness;
    output.Pos.z = 0.0f;
    output.Pos.w = 0.0f;
    output.Pos += positionPoint1Transformed;
    output.Pos *= fPoint1w;
    triangleStream.Append(output);

    //triangle2
    output.Pos.x = cos(fAngle) * fThickness / fRatio;
    output.Pos.y = sin(fAngle) * fThickness;
    output.Pos.z = 0.0f;
    output.Pos.w = 0.0f;
    output.Pos += positionPoint0Transformed;
    output.Pos *= fPoint0w;
    triangleStream.Append(output);

    output.Pos.x = cos(fAngle) * fThickness / fRatio;
    output.Pos.y = sin(fAngle) * fThickness;
    output.Pos.z = 0.0f;
    output.Pos.w = 0.0f;
    output.Pos += positionPoint1Transformed;
    output.Pos *= fPoint1w;
    triangleStream.Append(output);

    output.Pos.x = cos(fAngle + (PI / nCountTriangles * (nCountTriangles))) * fThickness / fRatio;
    output.Pos.y = sin(fAngle + (PI / nCountTriangles * (nCountTriangles))) * fThickness;
    output.Pos.z = 0.0f;
    output.Pos.w = 0.0f;
    output.Pos += positionPoint1Transformed;
    output.Pos *= fPoint1w;
    triangleStream.Append(output);
}