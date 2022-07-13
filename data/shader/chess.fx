
cbuffer VSPerFrameConstants : register(b0)
{
    float4x4 c_VSViewProjectionMatrix;
}

cbuffer VSPerObjectConstants : register(b1)
{
    float4x4 c_VSWorldMatrix;
}

cbuffer PSPerObjectConstants : register(b0)
{
    float3 c_PSColorA;
    float3 c_PSColorB;
    uint2  c_PSSizeInCells;
}

struct VSInput
{
    float3 m_Position : OSPOSITION;
    float2 m_PositionNormed : POSITION_NORMED;
};

struct PSInput
{
    float4 m_Position : SV_POSITION;
    float2 m_PositionNormed : TEXCOORD0;
};

PSInput VSMain(VSInput _Input)
{
    float4 WSPosition = mul(float4(_Input.m_Position, 1.0f), c_VSWorldMatrix);
    float4 CSPosition = mul(WSPosition, c_VSViewProjectionMatrix);

    PSInput Result;

    Result.m_Position = CSPosition;
    Result.m_PositionNormed = _Input.m_PositionNormed;

    return Result;
}

float4 PSMain(PSInput _Input) : SV_TARGET
{
    //            x,y = 0..1                    8, 8
    uint2 Cell = _Input.m_PositionNormed * float2(c_PSSizeInCells);

    float3 Color;

    if (Cell.x % 2 == 0)
    {
        if (Cell.y % 2 == 0)
        {
            Color = c_PSColorA;
        }
        else
        {
            Color = c_PSColorB;
        }
    }
    else
    {
        if (Cell.y % 2 == 0)
        {
            Color = c_PSColorB;
        }
        else
        {
            Color = c_PSColorA;
        }
    }

    return float4(Color, 1.0f);
}
