
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
    float4 c_PSConstant0;
    uint4  c_PSConstant1;
}

#define c_PSColor        c_PSConstant0.xyz
#define c_PSMaxIteration c_PSConstant1.x

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

/*  Explanation of how the Mandelbrot algorithm works
    The Mandelbrot set is the set of complex numbers.
    Complex numbers consist of a real part and an imaginary part.
        Z = a + bi
    1I equals the square root of -1.
    The complex number is represented here in a float2.
    The first value represents the real part and the second value the imaginary part.

                     a
            |-------------------|
            |                c  |       C is a specific coordinate.
         bi |                   |       The Pixel consits of c = a + bi
            |                   |
            |-------------------|

    The Mandelbrot set is defined by the following iterative rule.
        z[0] = 0
        z[n+1] = z[n]^2+c
        c = a + bi

    For Example:
        Z[0] =     0
        Z[1] =     0 + C
        Z[2] =   C^2 + C
        Z[3] =  (C^2 + C)^2 + C
        Z[4] = ((C^2 + C)^2 + C)^2 +C
        Z[5] = ....

    All pixels that are in the Mandelbrot set are drawn in black, all others are colored.
    
    In each pass, the algorithm approaches the real set of complex numbers. Because of this, the shape gets sharper with each pass.

*/

// Source: https://arukiap.github.io/fractals/2019/06/02/rendering-the-mandelbrot-set-with-shaders.html
float2 squareImaginary(float2 _number)
{
    return float2(_number[0] * _number[0] - _number[1] * _number[1]
    , 2*_number[0]*_number[1]);
}

float iterateMandelbrot(float2 _coord, int _maxIterations)
{
    float2 z = float2(0, 0);
    for (int i = 0; i < _maxIterations; i++)
    {
        z = squareImaginary(z) + _coord;
        if (length(z) > 2)
            return 1.0f;
    }
    return 0.0f;
}


// -----------------------------------------------------------------------------
// Pixel Shader
// -----------------------------------------------------------------------------
float4 PSMain(PSInput _Input) : SV_TARGET
{
    return float4(iterateMandelbrot(_Input.m_PositionNormed, (int) c_PSMaxIteration) * c_PSColor, 1.0f);
}
