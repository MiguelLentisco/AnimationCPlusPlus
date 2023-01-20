#include "Application/TrackTestApp.h"

#include <cstdarg>

#include "Animation/Interpolation.h"
#include "Animation/Frame.h"
#include "Core/Mat4.h"
#include "Core/Quat.h"
#include "Renderer/DebugDrawer.h"

// ---------------------------------------------------------------------------------------------------------------------

void TrackTestApp::Initialize()
{
    Application::Initialize();

    m_ScalarTracks.push_back(MakeScalarTrack(Interpolation::Linear, 2, MakeFrame(0.0f, 0.0f), MakeFrame(1.0f, 1.0f)));
	m_ScalarTracksLooping.push_back(false);

	m_ScalarTracks.push_back(MakeScalarTrack(Interpolation::Linear, 2, MakeFrame(0.0f, 0.0f), MakeFrame(0.5f, 1.0f)));
	m_ScalarTracksLooping.push_back(false);

	m_ScalarTracks.push_back(MakeScalarTrack(Interpolation::Linear, 3, MakeFrame(0.25f, 0.0f), MakeFrame(0.5f, 1.0f),
		MakeFrame(0.75f, 0.0f)));
	m_ScalarTracksLooping.push_back(true);

	m_ScalarTracks.push_back(MakeScalarTrack(Interpolation::Linear, 3, MakeFrame(0.25f, 0.0f), MakeFrame(0.5f, 1.0f),
		MakeFrame(0.75f, 0.0f)));
	m_ScalarTracksLooping.push_back(false);

	ScalarTrack stepTrack;
	stepTrack.Resize(11);
	stepTrack.SetInterpolation(Interpolation::Constant);
	
	for (int i = 0; i < 11; ++i)
	{
		const float time = static_cast<float>(i) * 0.05f + 0.25f;
		stepTrack[i] = MakeFrame(time, i % 2 == 0 ? 0.0f : 1.0f);
	}
	
	m_ScalarTracks.push_back(stepTrack);
	m_ScalarTracks.push_back(stepTrack);
	m_ScalarTracksLooping.push_back(true);
	m_ScalarTracksLooping.push_back(false);

	m_ScalarTracks.push_back(MakeScalarTrack(Interpolation::Cubic, 2, MakeFrame(0.25f, 0.676221f, 0.0f, 0.676221f),
		MakeFrame(0.75f, 4.043837f, 1.0f, 4.043837f)));
	m_ScalarTracksLooping.push_back(false);

	m_ScalarTracks.push_back(MakeScalarTrack(Interpolation::Cubic, 3, MakeFrame(0.25f, 0, 0, 0),
		MakeFrame(0.5f, 0, 1, 0), MakeFrame(0.75f, 0, 0, 0)));
	m_ScalarTracksLooping.push_back(true);

	m_ScalarTracks.push_back(MakeScalarTrack(Interpolation::Cubic, 5, MakeFrame(0.25f, 0, 0, 0),
		MakeFrame(0.3833333f, -10.11282f, 0.5499259f, -10.11282f), MakeFrame(0.5f, 25.82528f, 1, 25.82528f),
		MakeFrame(0.6333333f, 7.925411f, 0.4500741f, 7.925411f), MakeFrame(0.75f, 0, 0, 0)));
	m_ScalarTracksLooping.push_back(true);

	m_ScalarTracks.push_back(MakeScalarTrack(Interpolation::Cubic, 5, MakeFrame(0.25f, 0, 0, 0),
		MakeFrame(0.3833333f, 13.25147f, 0.5499259f, -10.11282f), MakeFrame(0.5f, 10.2405f, 1, -5.545671f),
		MakeFrame(0.6333333f, 7.925411f, 0.4500741f, -11.40672f), MakeFrame(0.75f, 0, 0, 0)));
	m_ScalarTracksLooping.push_back(true);

	m_ScalarTrackLines = new DebugDrawer();
	m_HandlePoints = new DebugDrawer();
	m_HandleLines = new DebugDrawer();
	m_ReferenceLines = new DebugDrawer();

	// Draw graphics
	for (unsigned int i = 0; i < 10; ++i)
	{
		const float yPosition = static_cast<float>(i) * 2.2f + 0.1f;
		
		static constexpr float HEIGHT = 1.8f;
		static constexpr float LEFT = 1.0f;
		static constexpr float RIGHT = 14.0;

		m_ReferenceLines->Push(Vec3(LEFT, yPosition, 0));
		m_ReferenceLines->Push(Vec3(LEFT, yPosition + HEIGHT, 0));

		m_ReferenceLines->Push(Vec3(LEFT, yPosition, 0));
		m_ReferenceLines->Push(Vec3(RIGHT, yPosition, 0));
	}
	m_ReferenceLines->UpdateOpenGLBuffers();

	// Draw each plot
	for (unsigned int i = 0; i < m_ScalarTracks.size(); ++i)
	{
		const float yPosition = static_cast<float>(9 - i) * 2.2f + 0.1f;
		
		static constexpr float HEIGHT = 1.8f;
		static constexpr float LEFT = 1.0f;
		static constexpr float RIGHT = 14.0;
		static constexpr float X_RANGE = RIGHT - LEFT;

		// Sample each graph
		for (unsigned int j = 1; j < 150; ++j)
		{
			const float thisJNorm = static_cast<float>(j - 1) / 149.0f;
			const float nextJNorm = static_cast<float>(j) / 149.0f;

			const float thisX = LEFT + thisJNorm * X_RANGE;
			const float nextX = LEFT + nextJNorm * X_RANGE;

			float thisY = m_ScalarTracks[i].Sample(thisJNorm, m_ScalarTracksLooping[i]);
			float nextY = m_ScalarTracks[i].Sample(nextJNorm, m_ScalarTracksLooping[i]);

			thisY = yPosition + thisY * HEIGHT;
			nextY = yPosition + nextY * HEIGHT;

			m_ScalarTrackLines->Push(Vec3(thisX, thisY, 0.1f));
			m_ScalarTrackLines->Push(Vec3(nextX, nextY, 0.1f));
		}

		// Plot each frame point and slope directions
		const unsigned int numFrames = m_ScalarTracks[i].GetSize();
		for (unsigned int j = 0; j < numFrames; ++j)
		{
			const float thisTime = m_ScalarTracks[i][j].m_Time;
			const float thisY = yPosition + m_ScalarTracks[i].Sample(thisTime, m_ScalarTracksLooping[i]) * HEIGHT;
			const float thisX = LEFT + thisTime * X_RANGE;
			
			m_HandlePoints->Push(Vec3(thisX, thisY, 0.9f));

			if (thisTime > 0.f && (j > 0 || m_ScalarTracksLooping[i]))
			{
				const float prevY = yPosition + m_ScalarTracks[i].Sample(thisTime - 0.0005f, m_ScalarTracksLooping[i]) * HEIGHT;
				const float prevX = LEFT + (thisTime - 0.0005f) * X_RANGE;

				Vec3 thisVec = Vec3(thisX, thisY, 0.6f);
				Vec3 prevVec = Vec3(prevX, prevY, 0.6f);
				Vec3 handleVec = thisVec + (prevVec - thisVec).Normalized() * 0.75f;

				m_HandleLines->Push(thisVec);
				m_HandleLines->Push(handleVec);
			}

			if (thisTime < 1.f && (j < numFrames - 1 || m_ScalarTracksLooping[i]) &&
				m_ScalarTracks[i].GetInterpolation() != Interpolation::Constant)
			{
				const float nextY = yPosition + m_ScalarTracks[i].Sample(thisTime + 0.0005f, m_ScalarTracksLooping[i]) * HEIGHT;
				const float nextX = LEFT + (thisTime + 0.0005f) * X_RANGE;

				const Vec3 thisVec = Vec3(thisX, thisY, 0.6f);
				const Vec3 nextVec = Vec3(nextX, nextY, 0.6f);
				const Vec3 handleVec = thisVec + (nextVec - thisVec).Normalized() * 0.75f;

				m_HandleLines->Push(thisVec);
				m_HandleLines->Push(handleVec);
			}
		}
	}
	
	m_ScalarTrackLines->UpdateOpenGLBuffers();
	m_HandleLines->UpdateOpenGLBuffers();
	m_HandlePoints->UpdateOpenGLBuffers();
	
} // Initialize

// ---------------------------------------------------------------------------------------------------------------------

void TrackTestApp::Render(float inAspectRatio)
{
    Application::Render(inAspectRatio);

	const Mat4 model;
	const Mat4 view = Mat4::CreateLookAt(Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 1, 0));
	const Mat4 projection = Mat4::CreateOrtho(0, inAspectRatio * 22.0f, 0, 22, 0.001f, 10);
	const Mat4 mvp = projection * view * model;

	m_ReferenceLines->Draw(DebugDrawMode::Lines, Vec3(1, 1, 1), mvp);
	m_ScalarTrackLines->Draw(DebugDrawMode::Lines, Vec3(0, 1, 0), mvp);
	m_HandleLines->Draw(DebugDrawMode::Lines, Vec3(1, 0, 0), mvp);
	m_HandlePoints->Draw(DebugDrawMode::Points, Vec3(0, 0, 1), mvp);
	
} // Render

// ---------------------------------------------------------------------------------------------------------------------

void TrackTestApp::Shutdown()
{
	m_ScalarTracks.clear();
	m_ScalarTracksLooping.clear();

	delete m_ScalarTrackLines;
	delete m_HandlePoints;
	delete m_HandleLines;
	delete m_ReferenceLines;
	
    Application::Shutdown();
	
} // Shutdown

// ---------------------------------------------------------------------------------------------------------------------

ScalarFrame TrackTestApp::MakeFrame(float time, float value)
{
	return MakeFrame(time, 0.0f, value, 0.0f);
	
} // MakeFrame

// ---------------------------------------------------------------------------------------------------------------------

ScalarFrame TrackTestApp::MakeFrame(float time, float in, float value, float out)
{
	ScalarFrame result;
	result.m_Time = time;
	result.m_In[0] = in;
	result.m_Value[0] = value;
	result.m_Out[0] = out;
	return result;
	
} // MakeFrame

// ---------------------------------------------------------------------------------------------------------------------

VectorFrame TrackTestApp::MakeFrame(float time, const Vec3& value)
{
	return MakeFrame(time, {}, value, {});
	
} // MakeFrame

// ---------------------------------------------------------------------------------------------------------------------

VectorFrame TrackTestApp::MakeFrame(float time, const Vec3& in, const Vec3& value, const Vec3& out)
{
	VectorFrame result;
	result.m_Time = time;
	result.m_In[0] = in.x;
	result.m_In[1] = in.y;
	result.m_In[2] = in.z;
	result.m_Value[0] = value.x;
	result.m_Value[1] = value.y;
	result.m_Value[2] = value.z;
	result.m_Out[0] = out.x;
	result.m_Out[1] = out.y;
	result.m_Out[2] = out.z;
	return result;
	
} // MakeFrame

// ---------------------------------------------------------------------------------------------------------------------

QuaternionFrame TrackTestApp::MakeFrame(float time, const Quat& value) 
{
	return MakeFrame(time, Quat(0, 0, 0, 0), value, Quat(0, 0, 0, 0));
	
} // MakeFrame

// ---------------------------------------------------------------------------------------------------------------------

QuaternionFrame TrackTestApp::MakeFrame(float time, const Quat& in, const Quat& out, const Quat& value)
{
	QuaternionFrame result;
	result.m_Time = time;
	result.m_In[0] = in.x;
	result.m_In[1] = in.y;
	result.m_In[2] = in.z;
	result.m_In[3] = in.w;
	result.m_Value[0] = value.x;
	result.m_Value[1] = value.y;
	result.m_Value[2] = value.z;
	result.m_Value[3] = value.w;
	result.m_Out[0] = out.x;
	result.m_Out[1] = out.y;
	result.m_Out[2] = out.z;
	result.m_Out[3] = out.w;
	return result;
	
} // MakeFrame

// ---------------------------------------------------------------------------------------------------------------------

ScalarTrack TrackTestApp::MakeScalarTrack(Interpolation interp, int numFrames, ...)
{
	ScalarTrack result;
	result.SetInterpolation(interp);
	result.Resize(numFrames);

	va_list args;
	va_start(args, numFrames);

	for (int i = 0; i < numFrames; ++i)
	{
		result[i] = va_arg(args, ScalarFrame);
	}

	va_end(args);

	return result;
	
} // MakeScalarTrack

// ---------------------------------------------------------------------------------------------------------------------

VectorTrack TrackTestApp::MakeVectorTrack(Interpolation interp, int numFrames, ...)
{
	VectorTrack result;
	result.SetInterpolation(interp);
	result.Resize(numFrames);

	va_list args;
	va_start(args, numFrames);

	for (int i = 0; i < numFrames; ++i)
	{
		result[i] = va_arg(args, VectorFrame);
	}

	va_end(args);

	return result;
	
} // MakeVectorTrack

// ---------------------------------------------------------------------------------------------------------------------

QuaternionTrack TrackTestApp::MakeQuaternionTrack(Interpolation interp, int numFrames, ...)
{
	QuaternionTrack result;
	result.SetInterpolation(interp);
	result.Resize(numFrames);

	va_list args;
	va_start(args, numFrames);

	for (int i = 0; i < numFrames; ++i)
	{
		result[i] = va_arg(args, QuaternionFrame);
	}

	va_end(args);

	return result;
	
} // MakeQuaternionTrack

// ---------------------------------------------------------------------------------------------------------------------
