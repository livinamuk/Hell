#include "hellpch.h"
#include "Window.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"

namespace HellEngine
{
	Window::Window(float xPos, float zPos, int story, float height, Axis axis)
	{
		m_transform.position.x = xPos;
		m_transform.position.z = zPos;
		m_story = story;
		m_axis = axis;
		m_startHeight = height;
		Reconfigure();
	}

	void Window::Draw(Shader* shader)
	{		
		static int modelID = AssetManager::GetModelIDByName("Window");
		AssetManager::models[modelID].Draw(shader, m_transform.to_mat4());
	}

	void Window::Reconfigure()
	{
		m_transform.position.y = m_story * ROOM_HEIGHT + m_startHeight;
		m_transform.rotation = Util::SetRotationByAxis(m_axis);
	}
}