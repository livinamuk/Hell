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
		m_EditorCollisionObject = cpy.m_EditorCollisionObject;
		m_GlassCollisionObject = cpy.m_GlassCollisionObject;
		m_transform = cpy.m_transform;
		m_axis = cpy.m_axis;

		EntityData* entityData = (EntityData*)m_EditorCollisionObject->getUserPointer();
		entityData->ptr = this;
		EntityData* entityData2 = (EntityData*)m_GlassCollisionObject->getUserPointer();
		entityData2->ptr = this;
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
		// For the invisible editor picker object
		glm::vec3 position = m_transform.position;
		position.y += WINDOW_HEIGHT_SINGLE / 2;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Util::glmVec3_to_btVec3(position));
		transform.setRotation(Util::glmVec3_to_btQuat(m_transform.rotation));

		float friction = 0.5f;
		int collisionGroup = CollisionGroups::NONE;
		int collisionMask = CollisionGroups::EDITOR_ONLY;
		btCollisionShape* collisionShape = Physics::s_windowShape;
		PhysicsObjectType objectType = PhysicsObjectType::EDITOR_WINDOW;

		m_EditorCollisionObject = Physics::CreateCollisionObject(transform, collisionShape, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_DOOR, this);
	

		// For the glass itself
		m_triangleMesh = new btTriangleMesh();

		Model* glassModel = AssetManager::GetModelByName("WindowGlass");

		for (int i = 0; i < glassModel->m_meshes[0]->indices.size(); i += 3)
		{
			// upper glass
			std::vector<Vertex>* vertices = &glassModel->m_meshes[0]->vertices;
			std::vector<unsigned int>* indices = &glassModel->m_meshes[0]->indices;
			glm::vec3 scale = m_transform.scale;
			btVector3 vertA = Util::glmVec3_to_btVec3(vertices->at(indices->at(i)).Position);
			btVector3 vertB = Util::glmVec3_to_btVec3(vertices->at(indices->at(i + 1)).Position);
			btVector3 vertC = Util::glmVec3_to_btVec3(vertices->at(indices->at(i + 2)).Position);
			m_triangleMesh->addTriangle(vertA, vertB, vertC);
		}	
		for (int i = 0; i < glassModel->m_meshes[1]->indices.size(); i += 3)
		{
			// lower glass
			std::vector<Vertex>* vertices = &glassModel->m_meshes[1]->vertices;
			std::vector<unsigned int>* indices = &glassModel->m_meshes[1]->indices;
			glm::vec3 scale = m_transform.scale;
			btVector3 vertA = Util::glmVec3_to_btVec3(vertices->at(indices->at(i)).Position);
			btVector3 vertB = Util::glmVec3_to_btVec3(vertices->at(indices->at(i + 1)).Position);
			btVector3 vertC = Util::glmVec3_to_btVec3(vertices->at(indices->at(i + 2)).Position);
			m_triangleMesh->addTriangle(vertA, vertB, vertC);
		}

		m_triangleMeshShape = new btBvhTriangleMeshShape(m_triangleMesh, true, true);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(Util::glmVec3_to_btVec3(m_transform.position));
		meshTransform.setRotation(Util::glmVec3_to_btQuat(m_transform.rotation));

		m_GlassCollisionObject = new btCollisionObject();
		m_GlassCollisionObject->setCollisionShape(m_triangleMeshShape);
		m_GlassCollisionObject->setWorldTransform(meshTransform);
		m_GlassCollisionObject->setFriction(0.5);
		m_GlassCollisionObject->setCustomDebugColor(btVector3(1, 0, 0));

		EntityData* entityData = new EntityData();
		entityData->type = PhysicsObjectType::GLASS;
		entityData->ptr = this;

		m_GlassCollisionObject->setUserPointer(entityData);
		m_GlassCollisionObject->getCollisionShape()->setLocalScaling(Util::glmVec3_to_btVec3(m_transform.scale));

		int group = CollisionGroups::HOUSE;
		int mask = CollisionGroups::ENTITY | CollisionGroups::ENEMY | CollisionGroups::PROJECTILES;

		Physics::s_dynamicsWorld->addCollisionObject(m_GlassCollisionObject, group, mask);
		//Physics::CreateCollisionObject(transform, collisionShape, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_DOOR, this);

		m_GlassCollisionObject->setCollisionFlags(m_GlassCollisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}

	void Window::RemoveCollisionObject()
	{
		Physics::s_dynamicsWorld->removeCollisionObject(m_EditorCollisionObject);
	}
}