#pragma once

struct VSPerFrameConstants
{
    float m_VSViewProjectionMatrix[16];
};

struct VSPerObjectConstants
{
    float m_VSWorldMatrix[16];
};