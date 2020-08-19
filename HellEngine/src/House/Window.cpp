#include "hellpch.h"
#include "Window.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"
#include "Core/LevelEditor.h"

namespace HellEngine
{
	Window::Window(glm::vec3 position, Axis axis)
	{
		m_transform.position = position;
		m_axis = axis;
		Reconfigure();
		CreateCollisionObject();
	}

	Window::Window(const Window& cpy)
	{
		m_collisionObject = cpy.m_collisionObject;
		m_transform = cpy.m_transform;
		m_axis = cpy.m_axis;

		EntityData* entityData = (EntityData*)m_collisionObject->getUserPointer();
		entityData->ptr = this;
	}

	Window& Window::operator = (const Window& input) {
		new (this) Window(input);
		return *this;
	}

	void Window::DrawFrameAndSashes(Shader* shader)
	{
		// Draw frame and sashes
		LevelEditor::SetHighlightColorIfSelected(shader, this);
		static Model* model = AssetManager::GetModelByName("Window");		
		model->DrawMesh(shader, 0, m_transform.to_mat4());
		model->DrawMesh(shader, 1, m_transform.to_mat4());
		model->DrawMesh(shader, 3, m_transform.to_mat4());
		model->DrawMesh(shader, 5, m_transform.to_mat4());
		shader->setVec3("ColorAdd", glm::vec3(0, 0, 0));
	}

	void Window::DrawGlass(Shader* shader)
	{
		// Draw glass
		LevelEditor::SetHighlightColorIfSelected(shader, this);
		static Model* model = AssetManager::GetModelByName("WindowGlass");
	//	glDisable(GL_CULL_FACE);
		model->Draw(shader, m_transform.to_mat4());
	//	model->DrawMesh(shader, 2, m_transform.to_mat4());
	//	model->DrawMesh(shader, 4, m_transform.to_mat4());
	//	glEnable(GL_CULL_FACE);
		shader->setVec3("ColorAdd", glm::vec3(0, 0, 0));
	}

	void Window::Reconfigure()
	{
		m_transform.rotation = Util::SetRotationByAxis(m_axis);
	}

	void Window::CreateCollisionObject()
	{
		glm::vec3 position = m_transform.position;
		position.y += WINDOW_HEIGHT_SINGLE / 2;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Util::glmVec3_to_btVec3(position));
		transform.setRotation(Util::glmVec3_to_btQuat(m_transform.rotation));

		float friction = 0.5f;
		int collisionGroup = CollisionGroups::HOUSE;
		int collisionMask = CollisionGroups::EDITOR_ONLY;
		btCollisionShape* collisionShape = Physics::s_windowShape;
		PhysicsObjectType objectType = PhysicsObjectType::WINDOW;

		m_collisionObject = Physics::CreateCollisionObject(transform, collisionShape, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_DOOR, this);
	}

	void Window::RemoveCollisionObject()
	{
		Physics::s_dynamicsWorld->removeCollisionObject(m_collisionObject);
	}
}