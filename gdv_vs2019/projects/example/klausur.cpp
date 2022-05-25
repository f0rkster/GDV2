
#include "yoshix.h"

using namespace gfx;

class CApplication : public IApplication
{
public:

    CApplication();
    virtual ~CApplication();

private:

    struct VSPerFrameConstants
    {
        float m_VSViewProjectionMatrix[16];
    };

    struct VSPerObjectConstants
    {
        float m_VSWorldMatrix[16];
    };

    struct PSPerObjectConstants
    {
        float m_PSColor[4];
        float m_PSDepth;
        float m_Padding0[3];
        float m_PSValue1[2];
        float m_Padding1[2];
        float m_PSValue2[3];
        float m_Padding2[1];
    };

private:

    float   m_FieldOfViewY;             // Vertical view angle of the camera

    BHandle m_pVertexShader;
    BHandle m_pPixelShader;
    BHandle m_pVSPerFrameConstantBuffer;
    BHandle m_pVSPerObjectConstantBuffer;
    BHandle m_pPSPerObjectConstantBuffer;
    BHandle m_pMaterial;
    BHandle m_pMesh;

private:

    virtual bool InternOnCreateConstantBuffers();
    virtual bool InternOnReleaseConstantBuffers();
    virtual bool InternOnCreateShader();
    virtual bool InternOnReleaseShader();
    virtual bool InternOnCreateMaterials();
    virtual bool InternOnReleaseMaterials();
    virtual bool InternOnCreateMeshes();
    virtual bool InternOnReleaseMeshes();
    virtual bool InternOnResize(int _Width, int _Height);
    virtual bool InternOnUpdate();
    virtual bool InternOnFrame();
};

// -----------------------------------------------------------------------------

CApplication::CApplication()
    : m_FieldOfViewY(60.0f)        // Set the vertical view angle of the camera to 60 degrees.
{
}

// -----------------------------------------------------------------------------

CApplication::~CApplication()
{
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateConstantBuffers()
{
    CreateConstantBuffer(sizeof(VSPerFrameConstants), &m_pVSPerFrameConstantBuffer);
    CreateConstantBuffer(sizeof(VSPerObjectConstants), &m_pVSPerObjectConstantBuffer);
    CreateConstantBuffer(sizeof(PSPerObjectConstants), &m_pPSPerObjectConstantBuffer);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseConstantBuffers()
{
    ReleaseConstantBuffer(m_pVSPerFrameConstantBuffer);
    ReleaseConstantBuffer(m_pVSPerObjectConstantBuffer);
    ReleaseConstantBuffer(m_pPSPerObjectConstantBuffer);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateShader()
{
    CreateVertexShader("..\\data\\shader\\simple.hlsl", "VSMain", &m_pVertexShader);
    CreatePixelShader("..\\data\\shader\\simple.hlsl", "PSMain", &m_pPixelShader);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseShader()
{
    ReleaseVertexShader(m_pVertexShader);
    ReleasePixelShader(m_pPixelShader);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMaterials()
{
    SMaterialInfo Info;

    Info.m_NumberOfTextures = 0;
    Info.m_NumberOfVertexConstantBuffers = 2;
    Info.m_pVertexConstantBuffers[0] = m_pVSPerFrameConstantBuffer;
    Info.m_pVertexConstantBuffers[1] = m_pVSPerObjectConstantBuffer;
    Info.m_NumberOfPixelConstantBuffers = 1;
    Info.m_pPixelConstantBuffers[0] = m_pPSPerObjectConstantBuffer;
    Info.m_pVertexShader = m_pVertexShader;
    Info.m_pPixelShader = m_pPixelShader;
    Info.m_NumberOfInputElements = 2;
    Info.m_InputElements[0].m_Type = SInputElement::Float3;
    Info.m_InputElements[0].m_pName = "OSPOSITION";
    Info.m_InputElements[1].m_Type = SInputElement::Float4;
    Info.m_InputElements[1].m_pName = "COLOR";

    CreateMaterial(Info, &m_pMaterial);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMaterials()
{
    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMeshes()
{
    float Vertices[][3 + 4] =
    {
        // X      Y     Z     R     G     B     A 
        { -2.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
        {  2.0f, -2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        {  0.0f,  2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
    };

    int Indices[][3] =
    {
        { 0, 1, 2 }
    };

    SMeshInfo Info;

    Info.m_pVertices = &Vertices[0][0];
    Info.m_NumberOfVertices = 3;
    Info.m_pIndices = &Indices[0][0];
    Info.m_NumberOfIndices = 3;
    Info.m_pMaterial = m_pMaterial;

    CreateMesh(Info, &m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMeshes()
{
    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnResize(int _Width, int _Height)
{
    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnUpdate()
{
    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnFrame()
{
    return true;
}

// -----------------------------------------------------------------------------

void main()
{
    CApplication Application;

    RunApplication(800, 600, "YoshiX Example", &Application);
}