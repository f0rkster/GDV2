#pragma once

#include "yoshix.h"

#include "SVSConstantsMandelbrot.h"
#include "SPSConstantsMandelbrot.h"


class CApplication : public gfx::IApplication
{
public:

    CApplication();
    virtual ~CApplication();

private:
    int             m_ticks;
    int             m_MaxIteration;

private:
    float           m_Position[3];
    float           m_FieldOfViewY;             // Vertical view angle of the camera
    float           m_ViewMatrix[16];
    float           m_ProjectionMatrix[16];
    gfx::BHandle    m_pVSPerFrameConstants;
    gfx::BHandle    m_pVSPerObjectConstants;
    gfx::BHandle    m_pPSPerObjectConstants;
    gfx::BHandle    m_pVertexShader;
    gfx::BHandle    m_pPixelShader;
    gfx::BHandle    m_pMaterial;
    gfx::BHandle    m_pMesh;

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