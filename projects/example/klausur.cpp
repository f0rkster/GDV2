
#include "yoshix.h"

using namespace gfx;

// -----------------------------------------------------------------------------
// Defines a constant buffer on the CPU. Note that this struct matches exactly
// the 'VSBuffer' in the 'textured.fx' file. For easy communication between CPU
// and GPU it is always a good idea to rebuild the GPU struct on the CPU side.
// Node that the size of a constant buffer on bytes has to be a multiple of 16.
// For example the following is not possible:
// 
//     struct SVertexBuffer
//     {
//         float m_ViewProjectionMatrix[16];
//         float m_WorldMatrix[16];
//         float m_Scalar;
//     };
//
// The problem is the final member 'm_Scalar'. The two matrices at the begin
// require 2 * 16 * 4 = 128 bytes, which is dividable by 16. Adding the four
// bytes of 'm_Scalar' results in 132 bytes, which cannot be divided by 16.
// Instead you have to use a 4D vector, even if that implies a waste of memory.
// 
//     struct SVertexBuffer
//     {
//         float m_ViewProjectionMatrix[16];
//         float m_WorldMatrix[16];
//         float m_Vector[4];           => store 'm_Scalar' in the first component of the 4D vector and waste the other three ones.
//     };
//     
// -----------------------------------------------------------------------------
struct SVertexBuffer
{
	float m_ViewProjectionMatrix[16];       // Result of view matrix * projection matrix.
	float m_WorldMatrix[16];                // The world matrix to transform a mesh from local space to world space.
};

// -----------------------------------------------------------------------------

struct SPixelBuffer
{
	float m_Color[16];                      // The color the triangle is filled with in the pixel shader.
};

// -----------------------------------------------------------------------------


class CMandelbrot : public IApplication
{
	public:

		CMandelbrot();
		virtual ~CMandelbrot();

	private:

		float m_FieldOfViewY;               // Vertical view angle of the camera
		float   m_ViewMatrix[16];           // The view matrix to transform a mesh from world space into view space.
		float   m_ProjectionMatrix[16];     // The projection matrix to transform a mesh from view space into clip space.
		BHandle m_pVertexConstantBuffer;    // A pointer to a YoshiX constant buffer, which defines global data for a vertex shader.
		BHandle m_pPixelConstantBuffer;     // A pointer to a YoshiX constant buffer, which defines global data for a pixel shader.
		BHandle m_pVertexShader;            // A pointer to a YoshiX vertex shader, which processes each single vertex of the mesh.
		BHandle m_pPixelShader;             // A pointer to a YoshiX pixel shader, which computes the color of each pixel visible of the mesh on the screen.
		BHandle m_pMaterial;                // A pointer to a YoshiX material, spawning the surface of the mesh.
		BHandle m_pMesh;                    // A pointer to a YoshiX mesh, which represents a single triangle.


	private:

		virtual bool InternOnCreateTextures();
		virtual bool InternOnReleaseTextures();
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

CMandelbrot::CMandelbrot()
	: m_FieldOfViewY(60.0f)        // Set the vertical view angle of the camera to 60 degrees.
	, m_pVertexConstantBuffer(nullptr)
	, m_pPixelConstantBuffer(nullptr)
	, m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pMaterial(nullptr)
{
}

// -----------------------------------------------------------------------------

CMandelbrot::~CMandelbrot()
{
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnCreateTextures()
{
	// -----------------------------------------------------------------------------
	// Path is "..\\data\\images\\texture.dds"
	// -----------------------------------------------------------------------------
	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnReleaseTextures()
{
	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnCreateConstantBuffers()
{
    // -----------------------------------------------------------------------------
	// Create a constant buffer with global data for the vertex shader. We use this
	// buffer to upload the data defined in the 'SVertexBuffer' struct. Note that it
	// is not possible to use the data of a constant buffer in vertex and pixel
	// shader. Constant buffers are specific to a certain shader stage. If a 
	// constant buffer is a vertex or a pixel buffer is defined in the material info
	// when creating the material.
	// -----------------------------------------------------------------------------
    CreateConstantBuffer(sizeof(SVertexBuffer), &m_pVertexConstantBuffer);
	CreateConstantBuffer(sizeof(SPixelBuffer), &m_pPixelConstantBuffer);

	return true; 
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnReleaseConstantBuffers()
{
	// -----------------------------------------------------------------------------
	// Important to release the buffers again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseConstantBuffer(m_pVertexConstantBuffer);
	ReleaseConstantBuffer(m_pPixelConstantBuffer);

	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnCreateShader()
{
	// -----------------------------------------------------------------------------
	// Load and compile the shader programs.
	// -----------------------------------------------------------------------------
	CreateVertexShader("..\\data\\shader\\mandelbrot.fx", "VSShader", &m_pVertexShader);
	CreatePixelShader("..\\data\\shader\\mandelbrot.fx", "PSShader", &m_pPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnReleaseShader()
{
	ReleaseVertexShader(m_pVertexShader);
	ReleasePixelShader(m_pPixelShader);
	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnCreateMaterials()
{
	// -----------------------------------------------------------------------------
	// Create a material spawning the mesh. Note that you can use the same material
	// for multiple meshes as long as the input layout of the vertex shader matches
	// the vertex layout of the mesh.
	// -----------------------------------------------------------------------------
	SMaterialInfo MaterialInfo;

	MaterialInfo.m_NumberOfTextures = 0;                           // The material does not need textures, because the pixel shader just returns a constant color.
	MaterialInfo.m_NumberOfVertexConstantBuffers = 1;                           // We need one vertex constant buffer to pass world matrix and view projection matrix to the vertex shader.
	MaterialInfo.m_pVertexConstantBuffers[0] = m_pVertexConstantBuffer;     // Pass the handle to the created vertex constant buffer.
	MaterialInfo.m_NumberOfPixelConstantBuffers = 1;                           // We need one pixel constant buffer to pass the color to the pixel shader.
	MaterialInfo.m_pPixelConstantBuffers[0] = m_pPixelConstantBuffer;      // Pass the handle to the created pixel constant buffer.
	MaterialInfo.m_pVertexShader = m_pVertexShader;             // The handle to the vertex shader.
	MaterialInfo.m_pPixelShader = m_pPixelShader;              // The handle to the pixel shader.
	MaterialInfo.m_NumberOfInputElements = 1;                           // The vertex shader requests the position as only argument.
	MaterialInfo.m_InputElements[0].m_pName = "POSITION";                  // The semantic name of the argument, which matches exactly the identifier in the 'VSInput' struct.
	MaterialInfo.m_InputElements[0].m_Type = SInputElement::Float3;       // The position is a 3D vector with floating points.

	CreateMaterial(MaterialInfo, &m_pMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnReleaseMaterials()
{
	// -----------------------------------------------------------------------------
	// Important to release the material again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseMaterial(m_pMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnCreateMeshes()
{
    // -----------------------------------------------------------------------------
	// Define the vertices of the mesh. Note that we define the complete data of one
	// vertex in one piece, so no separate arrays for position, normals, ... This
	// technique is called 'interleaved storage'. The vertex layout here consists of
	// two elements, namely the position (3D) and the texture coordinates (2D). This
	// matches the definition of the input elements in the material info and also
	// the members of the 'VSInput' struct in the 'textured.fx' file.
	// -----------------------------------------------------------------------------
    float QuadVertices[][3] =
    {
        { -4.0f, -4.0f, 0.0f, },
        {  4.0f, -4.0f, 0.0f, },
        {  4.0f,  4.0f, 0.0f, },
        { -4.0f,  4.0f, 0.0f, },
    };

    // -----------------------------------------------------------------------------
    // Define the topology of the mesh via indices. An index addresses a vertex from
    // the array above. Three indices represent one triangle. When defining the 
    // triangles of a mesh imagine that you are standing in front of the triangle 
    // and looking to the center of the triangle. If the mesh represents a closed
    // body such as a cube, your view position has to be outside of the body. Now
    // define the indices of the addressed vertices of the triangle in counter-
    // clockwise order.
    // -----------------------------------------------------------------------------
    int QuadIndices[][3] =
    {
        { 0, 1, 2, },
        { 0, 2, 3, },
    };

    // -----------------------------------------------------------------------------
    // Define the mesh and its material. The material defines the look of the 
    // surface covering the mesh. Note that you pass the number of indices and not
    // the number of triangles.
    // -----------------------------------------------------------------------------
    SMeshInfo MeshInfo;

    MeshInfo.m_pVertices = &QuadVertices[0][0];      // Pointer to the first float of the first vertex.
    MeshInfo.m_NumberOfVertices = 4;                 // The number of vertices.
    MeshInfo.m_pIndices = &QuadIndices[0][0];        // Pointer to the first index.
    MeshInfo.m_NumberOfIndices = 6;                  // The number of indices (has to be dividable by 3).
    MeshInfo.m_pMaterial = m_pMaterial;              // A handle to the material covering the mesh.

    CreateMesh(MeshInfo, &m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnReleaseMeshes()
{
    // -----------------------------------------------------------------------------
    // Important to release the mesh again when the application is shut down.
    // -----------------------------------------------------------------------------
    ReleaseMesh(m_pMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnResize(int _Width, int _Height)
{
	// -----------------------------------------------------------------------------
	// The projection matrix defines the size of the camera frustum. The YoshiX
	// camera has the shape of a pyramid with the eye position at the top of the
	// pyramid. The horizontal view angle is defined by the vertical view angle
	// and the ratio between window width and window height. Note that we do not
	// set the projection matrix to YoshiX. Instead we store the projection matrix
	// as a member and upload it in the 'InternOnFrame' method in a constant buffer.
	// -----------------------------------------------------------------------------
	GetProjectionMatrix(m_FieldOfViewY, static_cast<float>(_Width) / static_cast<float>(_Height), 0.1f, 100.0f, m_ProjectionMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnUpdate()
{
	float Eye[3];
	float At[3];
	float Up[3];

	// -----------------------------------------------------------------------------
	// Define position and orientation of the camera in the world. The result is
	// stored in the 'm_ViewMatrix' matrix and uploaded in the 'InternOnFrame'
	// method.
	// -----------------------------------------------------------------------------
	Eye[0] = 0.0f; At[0] = 0.0f; Up[0] = 0.0f;
	Eye[1] = 0.0f; At[1] = 0.0f; Up[1] = 1.0f;
	Eye[2] = -8.0f; At[2] = 0.0f; Up[2] = 0.0f;

	GetViewMatrix(Eye, At, Up, m_ViewMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool CMandelbrot::InternOnFrame()
{
	// -----------------------------------------------------------------------------
	// Upload the world matrix and the view projection matrix to the GPU. This has
	// to be done before drawing the mesh, though not necessarily in this method.
	// -----------------------------------------------------------------------------
	SVertexBuffer VertexBuffer;

	GetIdentityMatrix(VertexBuffer.m_WorldMatrix);

	MulMatrix(m_ViewMatrix, m_ProjectionMatrix, VertexBuffer.m_ViewProjectionMatrix);

	UploadConstantBuffer(&VertexBuffer, m_pVertexConstantBuffer);

	// -----------------------------------------------------------------------------
	// Upload the color to the GPU.
	// -----------------------------------------------------------------------------
	SPixelBuffer PixelBuffer;

	PixelBuffer.m_Color[0] = 1.0f;  // Red
	PixelBuffer.m_Color[1] = 0.0f;  // Green
	PixelBuffer.m_Color[2] = 0.0f;  // Blue
	PixelBuffer.m_Color[3] = 1.0f;  // Alpha

	UploadConstantBuffer(&PixelBuffer, m_pPixelConstantBuffer);

	// -----------------------------------------------------------------------------
	// Draw the mesh. This will activate the shader, constant buffers, and textures
	// of the material on the GPU and render the mesh to the current render targets.
	// -----------------------------------------------------------------------------
	DrawMesh(m_pMesh);


	return true;
}

// -----------------------------------------------------------------------------

void main()
{
	CMandelbrot Application;

	RunApplication(800, 600, "YoshiX Example", &Application);
}