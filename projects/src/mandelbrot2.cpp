
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
        float        m_PSColor[3];
        float        m_Padding0[1];
        unsigned int m_PSMaxIteration;
        unsigned int m_Padding[3];
    };

private:
    int m_ticks;
    int m_MaxIteration;

private:

    float   m_Position[3];
    float   m_FieldOfViewY;             // Vertical view angle of the camera
    float   m_ViewMatrix[16];
    float   m_ProjectionMatrix[16];
    BHandle m_pVSPerFrameConstants;
    BHandle m_pVSPerObjectConstants;
    BHandle m_pPSPerObjectConstants;
    BHandle m_pVertexShader;
    BHandle m_pPixelShader;
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
    : m_Position{ 0.0f, 0.0f, 0.0f }
    , m_FieldOfViewY(60.0f)        // Set the vertical view angle of the camera to 60 degrees.
    , m_ticks(0)
    , m_MaxIteration(1)
{
}

// -----------------------------------------------------------------------------

CApplication::~CApplication()
{
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateConstantBuffers()
{
    CreateConstantBuffer(sizeof(VSPerFrameConstants), &m_pVSPerFrameConstants);
    CreateConstantBuffer(sizeof(VSPerObjectConstants), &m_pVSPerObjectConstants);
    CreateConstantBuffer(sizeof(PSPerObjectConstants), &m_pPSPerObjectConstants);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseConstantBuffers()
{
    ReleaseConstantBuffer(m_pVSPerFrameConstants);
    ReleaseConstantBuffer(m_pVSPerObjectConstants);
    ReleaseConstantBuffer(m_pPSPerObjectConstants);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateShader()
{
    CreateVertexShader("..\\data\\shader\\mandelbrot2.fx", "VSMain", &m_pVertexShader);
    CreatePixelShader("..\\data\\shader\\mandelbrot2.fx", "PSMain", &m_pPixelShader);

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
    Info.m_pVertexConstantBuffers[0] = m_pVSPerFrameConstants;
    Info.m_pVertexConstantBuffers[1] = m_pVSPerObjectConstants;
    Info.m_NumberOfPixelConstantBuffers = 1;
    Info.m_pPixelConstantBuffers[0] = m_pPSPerObjectConstants;
    Info.m_pVertexShader = m_pVertexShader;
    Info.m_pPixelShader = m_pPixelShader;
    Info.m_NumberOfInputElements = 2;
    Info.m_InputElements[0].m_Type = SInputElement::Float3;
    Info.m_InputElements[0].m_pName = "OSPOSITION";
    Info.m_InputElements[1].m_Type = SInputElement::Float2;
    Info.m_InputElements[1].m_pName = "POSITION_NORMED";

    CreateMaterial(Info, &m_pMaterial);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMaterials()
{
    ReleaseMaterial(m_pMaterial);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMeshes()
{
    float Vertices[][3 + 2] =
    {
        // X      Y     Z         U      V
        { -16.0f, -16.0f, 0.0f,  -4.0f, -4.0f },
        {  16.0f, -16.0f, 0.0f,   4.0f, -4.0f },
        {  16.0f,  16.0f, 0.0f,   4.0f,  4.0f },
        { -16.0f,  16.0f, 0.0f,  -4.0f,  4.0f },
    };

    int Indices[][3] =
    {
        { 0, 1, 2 }, // Triangle 0
        { 0, 2, 3 }, // Triangle 1
    };

    SMeshInfo Info;

    Info.m_pVertices = &Vertices[0][0];
    Info.m_NumberOfVertices = 4;
    Info.m_pIndices = &Indices[0][0];
    Info.m_NumberOfIndices = 6;
    Info.m_pMaterial = m_pMaterial;

    CreateMesh(Info, &m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMeshes()
{
    ReleaseMesh(m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnResize(int _Width, int _Height)
{
    float AspectRatio = static_cast<float>(_Width) / static_cast<float>(_Height);

    GetProjectionMatrix(60.0f, AspectRatio, 0.01f, 1000.0f, m_ProjectionMatrix);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnUpdate()
{
    float Eye[] = { 0.0f, 0.0f, -10.0f };
    float At[] = { 0.0f, 0.0f,   0.0f };
    float Up[] = { 0.0f, 1.0f,   0.0f };

    GetViewMatrix(Eye, At, Up, m_ViewMatrix);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnFrame()
{
    // Once per frame
    VSPerFrameConstants PerFrameConstantsVS;

    MulMatrix(m_ViewMatrix, m_ProjectionMatrix, PerFrameConstantsVS.m_VSViewProjectionMatrix);

    UploadConstantBuffer(&PerFrameConstantsVS, m_pVSPerFrameConstants);

    VSPerObjectConstants PerObjectConstantsVS;

    GetTranslationMatrix(m_Position[0], m_Position[1], m_Position[2], PerObjectConstantsVS.m_VSWorldMatrix);

    UploadConstantBuffer(&PerObjectConstantsVS, m_pVSPerObjectConstants);

    PSPerObjectConstants PerObjectConstantsPS;

    //switch (m_MaxIteration%4)
    //{
    //case 0:
    //    PerObjectConstantsPS.m_PSColor[0] = 0.25f; //R
    //    PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
    //    PerObjectConstantsPS.m_PSColor[2] = 0.0;   //B
    //    break;    
    //case 1:
    //    PerObjectConstantsPS.m_PSColor[0] = 0.55f; //R
    //    PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
    //    PerObjectConstantsPS.m_PSColor[2] = 0.0;   //B
    //    break;    
    //case 2:
    //    PerObjectConstantsPS.m_PSColor[0] = 0.75f; //R
    //    PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
    //    PerObjectConstantsPS.m_PSColor[2] = 0.0;   //B
    //    break;
    //case 3:
    //    PerObjectConstantsPS.m_PSColor[0] = 0.95f; //R
    //    PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
    //    PerObjectConstantsPS.m_PSColor[2] = 0.0;   //B
    //    break;
    //default:
    //    PerObjectConstantsPS.m_PSColor[0] = m_MaxIteration; //R
    //    PerObjectConstantsPS.m_PSColor[1] = m_MaxIteration; //G
    //    PerObjectConstantsPS.m_PSColor[2] = m_MaxIteration; //B
    //    break;
    //}

    PerObjectConstantsPS.m_PSColor[0] = m_MaxIteration; //R
    PerObjectConstantsPS.m_PSColor[1] = m_MaxIteration; //G
    PerObjectConstantsPS.m_PSColor[2] = m_MaxIteration; //B

    m_ticks++;
    if (m_ticks == 10)
    {
        m_ticks = 0;
        m_MaxIteration++;
    }

    PerObjectConstantsPS.m_PSMaxIteration = m_MaxIteration;

    UploadConstantBuffer(&PerObjectConstantsPS, m_pPSPerObjectConstants);

    DrawMesh(m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

void main()
{
    CApplication Application;

    RunApplication(800, 600, "YoshiX Example", &Application);
}