
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
    float4 c_PSColor;
    float  c_PSDepth;   // => float4, float  c_PSDepth  + float3 Padding, 12 Leerbytes
    float2 c_PSValue1;  // => float4, float2 c_PSValue1 + float2 Padding,  8 Leerbytes
    float3 c_PSValue2;  // => float4, float3 c_PSValue2 + float  Padding,  4 Leerbytes
}

struct VSInput                          // Vertex Shader Input
{
    float3 m_Position : OSPOSITION;     // Object Space Position
    float4 m_Color    : COLOR;
};

struct PSInput                          // Pixel Shader Input
{
    float4 m_Position : SV_POSITION;    // Clip Space Position
    float4 m_Color    : TEXCOORD0;
};

PSInput VSMain(VSInput _Input)
{
    float4 WSPosition = mul(float4(_Input.m_Position, 1.0f), c_WorldMatrix);
    float4 CSPosition = mul(WSPosition, c_ViewProjectionMatrix);

    PSInput Result;

    Result.m_Position = CSPosition;
    Result.m_Color    = _Input.m_Color;

    return Result;
}

float4 PSMain(PSInput _Input) : SV_TARGET
{
    return _Input.m_Color;
}

// OS Object   Space
// WS World    Space
// ES View/Eye Space
// CS Clip     Space
// 
// VS          Vertex Shader
// PS          Pixel Shader

