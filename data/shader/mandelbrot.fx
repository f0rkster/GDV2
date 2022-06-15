
// -----------------------------------------------------------------------------
// Define the constant buffers.
// -----------------------------------------------------------------------------

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
    uint   c_PSMaxIteration;
}

// -----------------------------------------------------------------------------
// Define input and output data of the vertex shader.
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Vertex Shader
// -----------------------------------------------------------------------------
PSInput VSMain(VSInput _Input)
{
    float4 WSPosition = mul(float4(_Input.m_Position, 1.0f), c_VSWorldMatrix);
    float4 CSPosition = mul(WSPosition, c_VSViewProjectionMatrix);

    PSInput Result;

    Result.m_Position = CSPosition;
    Result.m_PositionNormed = _Input.m_PositionNormed;

    return Result;
}

// -----------------------------------------------------------------------------
// Util Functions
// -----------------------------------------------------------------------------

float2 squareImganiary(float2 _number)
{
    return float2(_number[0] * _number[0] - _number[1] * _number[1]
    , 2*_number[0]*_number[1]);
}

float iterateMandelbrot(float2 _coord, int _maxIterations)
{
    float2 z = float2(0, 0);
    for (int i = 0; i < _maxIterations; i++)
    {
        z = squareImganiary(z) + _coord;
        if (length(z) > 2)
            return i / _maxIterations;

    }
    return _maxIterations;

}


// -----------------------------------------------------------------------------
// Pixel Shader
// -----------------------------------------------------------------------------
float4 PSMain(PSInput _Input) : SV_TARGET
{
        return iterateMandelbrot(_Input.m_PositionNormed, 100);

}
