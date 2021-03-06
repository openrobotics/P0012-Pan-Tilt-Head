/*****************************************************************************

Filename    :   main.cpp
Content     :   Simple minimal VR demo
Created     :   December 1, 2014
Author      :   Tom Heath
Copyright   :   Copyright 2012 Oculus, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

/*****************************************************************************/

// Include opencv files

//#include "../../OculusRoomTiny_Advanced/Common/Win32_GLAppUtil.h"
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <iostream>
#include <headers\display.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "C:\Users\User\OneDrive\op_bots\P0012-Pan-Tilt-Head\src\oculus/Win32_GLAppUtil_test.h"

#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#endif

using namespace OVR;
using namespace std;

struct Scene
{
	int     numModels;
	Model * Models[1];

	void    Add(Model * n)
	{
		Models[numModels++] = n;
	}

	void Render(Matrix4f view, Matrix4f proj)
	{
		static const GLchar* VertexShaderSrc =
			"#version 150\n"
			"uniform mat4 matWVP;\n"
			"in      vec4 Position;\n"
			"in      vec4 Color;\n"
			"in      vec2 TexCoord;\n"
			"out     vec2 oTexCoord;\n"
			"out     vec4 oColor;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = (matWVP * Position);\n"
			"   oTexCoord   = TexCoord;\n"
			"   oColor.rgb  = pow(Color.rgb, vec3(2.2));\n"   // convert from sRGB to linear
			"   oColor.a    = Color.a;\n"
			"}\n";

		static const char* FragmentShaderSrc =
			"#version 150\n"
			"uniform sampler2D Texture0;\n"
			"in      vec4      oColor;\n"
			"in      vec2      oTexCoord;\n"
			"out     vec4      FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = oColor * texture2D(Texture0, oTexCoord);\n"
			"}\n";

		GLuint    vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
		GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

		// Make textures
		ShaderFill * grid_material[1];
		
		cv::Mat frame;
		cv::VideoCapture capture(CV_CAP_ANY);
		/*
		if (!capture.isOpened())
		{
			cout << "Can not open a capture object." << endl;
			exit(-1);
		}

		if (!capture.grab())
		{
			cout << "Could not grab camera... Skipping frame." << endl;
		}

		capture.retrieve(frame, CV_CAP_OPENNI_BGR_IMAGE);

		if (!frame.empty())
		{
			cv::flip(frame, frame, 0);
		}
		
		TextureBuffer * generated_texture = new TextureBuffer(nullptr, false, false, Sizei(640, 480), 4, (unsigned char *)frame.ptr(), 1);
		grid_material[0] = new ShaderFill(vshader, fshader, generated_texture);
		*/

		glDeleteShader(vshader);
		glDeleteShader(fshader);
		for (int i = 0; i < numModels; ++i){
			Models[i]->Render(view, proj);
			//Models[i]->Fill = grid_material[0];
		}
	}

	GLuint CreateShader(GLenum type, const GLchar* src)
	{
		GLuint shader = glCreateShader(type);

		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);

		GLint r;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
		if (!r)
		{
			GLchar msg[1024];
			glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
			if (msg[0]) {
				OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
			}
			return 0;
		}

		return shader;
	}

	void Init(int includeIntensiveGPUobject)
	{
		static const GLchar* VertexShaderSrc =
			"#version 150\n"
			"uniform mat4 matWVP;\n"
			"in      vec4 Position;\n"
			"in      vec4 Color;\n"
			"in      vec2 TexCoord;\n"
			"out     vec2 oTexCoord;\n"
			"out     vec4 oColor;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = (matWVP * Position);\n"
			"   oTexCoord   = TexCoord;\n"
			"   oColor.rgb  = pow(Color.rgb, vec3(2.2));\n"   // convert from sRGB to linear
			"   oColor.a    = Color.a;\n"
			"}\n";

		static const char* FragmentShaderSrc =
			"#version 150\n"
			"uniform sampler2D Texture0;\n"
			"in      vec4      oColor;\n"
			"in      vec2      oTexCoord;\n"
			"out     vec4      FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = oColor * texture2D(Texture0, oTexCoord);\n"
			"}\n";

		GLuint    vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
		GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

		// Make textures
		ShaderFill * grid_material[1];

		cv::Mat frame;
		cv::VideoCapture capture(CV_CAP_ANY);

		if (!capture.isOpened())
		{	
			exit(-1);
		}

		capture.retrieve(frame, CV_CAP_OPENNI_BGR_IMAGE);

		if (!frame.empty())
		{
			cv::flip(frame, frame, 0);
			Sleep(250);
		}
		
		TextureBuffer * generated_texture = new TextureBuffer(nullptr, false, false, Sizei(640, 480), 4, (unsigned char *)frame.ptr(), 1);
		grid_material[0] = new ShaderFill(vshader, fshader, generated_texture);

		glDeleteShader(vshader);
		glDeleteShader(fshader);

		// Construct geometry
		Model * m = new Model(Vector3f(0, 0, 0), grid_material[0]);  // Screen that we display webcam data on
		m->AddSolidColorBox(-0.5f, 0.5f, -4.0f, 0.5f, 1.5f, -4.0f, 0x00808080);
		// x controls how left/right it is. more negative = more right (relative to user)
		// y controls how the height of the wall
		// z controls how far it reaches away from us. more negative = closer to user

		m->AllocateBuffers();
		Add(m);
	}

	Scene() : numModels(0) {}
	Scene(bool includeIntensiveGPUobject) :
		numModels(0)
	{
		Init(includeIntensiveGPUobject);
	}
	void Release()
	{
		while (numModels-- > 0)
			delete Models[numModels];
	}
	~Scene()
	{
		Release();
	}
};

static ovrGraphicsLuid GetDefaultAdapterLuid()
{
	ovrGraphicsLuid luid = ovrGraphicsLuid();

#if defined(_WIN32)
	IDXGIFactory* factory = nullptr;

	if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
	{
		IDXGIAdapter* adapter = nullptr;
		if (SUCCEEDED(factory->EnumAdapters(0, &adapter)))
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);
			memcpy(&luid, &desc.AdapterLuid, sizeof(luid));
			adapter->Release();
		}
		factory->Release();
	}
#endif
	return luid;
}

static int Compare(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(ovrGraphicsLuid));
}

// return true to retry later (e.g. after display lost)
static bool MainLoop(bool retryCreate)
{
	TextureBuffer * eyeRenderTexture[2] = { nullptr, nullptr };
	DepthBuffer   * eyeDepthBuffer[2] = { nullptr, nullptr };
	ovrMirrorTexture mirrorTexture = nullptr;
	GLuint          mirrorFBO = 0;
	Scene         * roomScene = nullptr;
	long long frameIndex = 0;

	ovrSession session;
	ovrGraphicsLuid luid;
	ovrResult result = ovr_Create(&session, &luid);

	if (!OVR_SUCCESS(result))
		return retryCreate;

	if (Compare(luid, GetDefaultAdapterLuid())) // If luid that the Rift is on is not the default adapter LUID...
	{
		VALIDATE(false, "OpenGL supports only the default graphics adapter.");
	}

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);

	// Setup Window and Graphics
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
	ovrSizei windowSize = { hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 };
	if (!Platform.InitDevice(windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)))
		goto Done;

	// Make eye render buffers
	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(session, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
		eyeRenderTexture[eye] = new TextureBuffer(session, true, true, idealTextureSize, 1, NULL, 1);
		eyeDepthBuffer[eye] = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);

		if (!eyeRenderTexture[eye]->TextureChain)
		{
			if (retryCreate) goto Done;
			VALIDATE(false, "Failed to create texture.");
		}
	}

	ovrMirrorTextureDesc desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = windowSize.w;
	desc.Height = windowSize.h;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	result = ovr_CreateMirrorTextureGL(session, &desc, &mirrorTexture);
	if (!OVR_SUCCESS(result))
	{
		if (retryCreate) goto Done;
		VALIDATE(false, "Failed to create mirror texture.");
	}

	// Configure the mirror read buffer
	GLuint texId;
	ovr_GetMirrorTextureBufferGL(session, mirrorTexture, &texId);

	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Turn off vsync to let the compositor do its magic
	wglSwapIntervalEXT(0);

	// Make scene - can simplify further if needed
	roomScene = new Scene(false);

	// FloorLevel will give tracking poses where the floor height is 0
	ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);

	// Main loop
	while (Platform.HandleMessages())
	{
		ovrSessionStatus sessionStatus;
		ovr_GetSessionStatus(session, &sessionStatus);
		if (sessionStatus.ShouldQuit)
		{
			// Because the application is requested to quit, should not request retry
			retryCreate = false;
			break;
		}
		if (sessionStatus.ShouldRecenter)
			ovr_RecenterTrackingOrigin(session);

		if (sessionStatus.IsVisible)
		{
			// Keyboard inputs to adjust player orientation
			static float Yaw(3.141592f);
			//if (Platform.Key[VK_LEFT])  Yaw += 0.02f;
			//if (Platform.Key[VK_RIGHT]) Yaw -= 0.02f;

			// Keyboard inputs to adjust player position
			static Vector3f Pos2(0.0f, 0.0f, -5.0f); // this is our user origin
			//if (Platform.Key['W'] || Platform.Key[VK_UP])     Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, -0.05f));
			//if (Platform.Key['S'] || Platform.Key[VK_DOWN])   Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, +0.05f));
			//if (Platform.Key['D'])                            Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(+0.05f, 0, 0));
			//if (Platform.Key['A'])                            Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(-0.05f, 0, 0));

			// Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyeOffset) may change at runtime.
			ovrEyeRenderDesc eyeRenderDesc[2];
			eyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
			eyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

			// Get eye poses, feeding in correct IPD offset
			ovrPosef                  EyeRenderPose[2];
			ovrVector3f               HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset,
				eyeRenderDesc[1].HmdToEyeOffset };

			double sensorSampleTime;    // sensorSampleTime is fed into the layer later
			ovr_GetEyePoses(session, frameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

			// Render Scene to Eye Buffers
			for (int eye = 0; eye < 2; ++eye)
			{
				// Switch to eye render target
				eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);

				// Get view and projection matrices
				Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
				Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f(EyeRenderPose[eye].Orientation);
				Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
				Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
				Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(EyeRenderPose[eye].Position);

				Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
				Matrix4f proj = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_None);

				// Render world
				roomScene->Render(view, proj);

				// Avoids an error when calling SetAndClearRenderSurface during next iteration.
				// Without this, during the next while loop iteration SetAndClearRenderSurface
				// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
				// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
				eyeRenderTexture[eye]->UnsetRenderSurface();

				// Commit changes to the textures so they get picked up frame
				eyeRenderTexture[eye]->Commit();
			}

			// Do distortion rendering, Present and flush/sync

			ovrLayerEyeFov ld;
			ld.Header.Type = ovrLayerType_EyeFov;
			ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

			for (int eye = 0; eye < 2; ++eye)
			{
				ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureChain;
				ld.Viewport[eye] = Recti(eyeRenderTexture[eye]->GetSize());
				ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
				ld.RenderPose[eye] = EyeRenderPose[eye];
				ld.SensorSampleTime = sensorSampleTime;
			}

			ovrLayerHeader* layers = &ld.Header;
			result = ovr_SubmitFrame(session, frameIndex, nullptr, &layers, 1);
			// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
			if (!OVR_SUCCESS(result))
				goto Done;
			frameIndex++;
		}

		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = windowSize.w;
		GLint h = windowSize.h;
		glBlitFramebuffer(0, h, w, 0,
			0, 0, w, h,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		SwapBuffers(Platform.hDC);
	}

Done:
	delete roomScene;
	if (mirrorFBO) glDeleteFramebuffers(1, &mirrorFBO);
	if (mirrorTexture) ovr_DestroyMirrorTexture(session, mirrorTexture);
	for (int eye = 0; eye < 2; ++eye)
	{
		delete eyeRenderTexture[eye];
		delete eyeDepthBuffer[eye];
	}
	Platform.ReleaseDevice();
	ovr_Destroy(session);

	// Retry on ovrError_DisplayLost
	return retryCreate || (result == ovrError_DisplayLost);
}

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	// Initializes LibOVR, and the Rift
	ovrInitParams initParams = { ovrInit_RequestVersion, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	VALIDATE(Platform.InitWindow(hinst, L"Oculus Room Tiny (GL)"), "Failed to open window.");

	Platform.Run(MainLoop);

	ovr_Shutdown();

	return(0);
}