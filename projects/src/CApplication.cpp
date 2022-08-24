#include "CApplication.h"

#include <iostream>

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
    gfx::CreateConstantBuffer(sizeof(VSPerFrameConstants), &m_pVSPerFrameConstants);
    gfx::CreateConstantBuffer(sizeof(VSPerObjectConstants), &m_pVSPerObjectConstants);
    gfx::CreateConstantBuffer(sizeof(PSPerObjectConstants), &m_pPSPerObjectConstants);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseConstantBuffers()
{
    gfx::ReleaseConstantBuffer(m_pVSPerFrameConstants);
    gfx::ReleaseConstantBuffer(m_pVSPerObjectConstants);
    gfx::ReleaseConstantBuffer(m_pPSPerObjectConstants);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateShader()
{
    gfx::CreateVertexShader("..\\data\\shader\\mandelbrot.fx", "VSMain", &m_pVertexShader);
    gfx::CreatePixelShader("..\\data\\shader\\mandelbrot.fx", "PSMain", &m_pPixelShader);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseShader()
{
    gfx::ReleaseVertexShader(m_pVertexShader);
    gfx::ReleasePixelShader(m_pPixelShader);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMaterials()
{
    gfx::SMaterialInfo Info;

    Info.m_NumberOfTextures = 0;
    Info.m_NumberOfVertexConstantBuffers = 2;
    Info.m_pVertexConstantBuffers[0] = m_pVSPerFrameConstants;
    Info.m_pVertexConstantBuffers[1] = m_pVSPerObjectConstants;
    Info.m_NumberOfPixelConstantBuffers = 1;
    Info.m_pPixelConstantBuffers[0] = m_pPSPerObjectConstants;
    Info.m_pVertexShader = m_pVertexShader;
    Info.m_pPixelShader = m_pPixelShader;
    Info.m_NumberOfInputElements = 2;
    Info.m_InputElements[0].m_Type = gfx::SInputElement::Float3;
    Info.m_InputElements[0].m_pName = "OSPOSITION";
    Info.m_InputElements[1].m_Type = gfx::SInputElement::Float2;
    Info.m_InputElements[1].m_pName = "POSITION_NORMED";

    CreateMaterial(Info, &m_pMaterial);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMaterials()
{
    gfx::ReleaseMaterial(m_pMaterial);

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

    gfx::SMeshInfo Info;

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
    gfx::ReleaseMesh(m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnResize(int _Width, int _Height)
{
    float AspectRatio = static_cast<float>(_Width) / static_cast<float>(_Height);

    gfx::GetProjectionMatrix(60.0f, AspectRatio, 0.01f, 1000.0f, m_ProjectionMatrix);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnUpdate()
{
    float Eye[] = { 0.0f, 0.0f, -10.0f };
    float At[] = { 0.0f, 0.0f,   0.0f };
    float Up[] = { 0.0f, 1.0f,   0.0f };

    gfx::GetViewMatrix(Eye, At, Up, m_ViewMatrix);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnFrame()
{
    // Once per frame
    VSPerFrameConstants PerFrameConstantsVS;

    gfx::MulMatrix(m_ViewMatrix, m_ProjectionMatrix, PerFrameConstantsVS.m_VSViewProjectionMatrix);

    gfx::UploadConstantBuffer(&PerFrameConstantsVS, m_pVSPerFrameConstants);

    VSPerObjectConstants PerObjectConstantsVS;

    gfx::GetTranslationMatrix(m_Position[0], m_Position[1], m_Position[2], PerObjectConstantsVS.m_VSWorldMatrix);

    gfx::UploadConstantBuffer(&PerObjectConstantsVS, m_pVSPerObjectConstants);

    PSPerObjectConstants PerObjectConstantsPS;

    switch (m_MaxIteration % 4)
    {
    case 0:
        PerObjectConstantsPS.m_PSColor[0] = 0.25f; //R
        PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
        PerObjectConstantsPS.m_PSColor[2] = 0.0;   //B
        break;
    case 1:
        PerObjectConstantsPS.m_PSColor[0] = 0.55f; //R
        PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
        PerObjectConstantsPS.m_PSColor[2] = 0.0;   //B
        break;
    case 2:
        PerObjectConstantsPS.m_PSColor[0] = 0.75f; //R
        PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
        PerObjectConstantsPS.m_PSColor[2] = 0.0;   //B
        break;
    case 3:
        PerObjectConstantsPS.m_PSColor[0] = 0.95f; //R
        PerObjectConstantsPS.m_PSColor[1] = 0.25f; //G
        PerObjectConstantsPS.m_PSColor[2] = 0.0;   //B
        break;
    default:
        PerObjectConstantsPS.m_PSColor[0] = m_MaxIteration; //R
        PerObjectConstantsPS.m_PSColor[1] = m_MaxIteration; //G
        PerObjectConstantsPS.m_PSColor[2] = m_MaxIteration; //B
        break;
    }

    m_ticks++;
    if (m_ticks == 10)
    {
        m_ticks = 0;
        m_MaxIteration++;
    }

    PerObjectConstantsPS.m_PSMaxIteration = m_MaxIteration;

    gfx::UploadConstantBuffer(&PerObjectConstantsPS, m_pPSPerObjectConstants);

    gfx::DrawMesh(m_pMesh);

    return true;
}