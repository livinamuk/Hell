#include "hellpch.h"
#include "CoreImGui.h"
#include "ImGuizmo/ImGuizmo.h"
#include "CoreGL.h"
#include "Helpers/AssetManager.h"
#include "Helpers/Util.h"
#include "Config.h"
#include "File.h"
#include "LevelEditor.h"
#include "Core/GameData.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

namespace HellEngine
{
	bool CoreImGui::s_Show = false;

	void CoreImGui::InitImGui()
	{
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(CoreGL::s_window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
		ImGui::StyleColorsDark();
	}

	void CoreImGui::Render(Game* game)
	{
		//if (!Input::s_showCursor)
		//	return;


		ImGuizmo::AnimationCurve demoCurve(ImVec2(0, 0), ImVec2(1, 1), 0xFF00FFFF, ImCurveEdit::CurveSmooth);
		ImGuizmo::AnimationCurve linearCurve(ImVec2(0, 0), ImVec2(1, 1), 0xFF00FFFF, ImCurveEdit::CurveLinear);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		ImGui::ShowDemoWindow();

		ImGuiIO* io = &(ImGui::GetIO());
		int fps = std::round(io->Framerate);
		ImGui::Text("\nWELCOME TO HELL\nFPS: %d\n", fps);

	
		/*
		if (ImGui::BeginCombo("##MapList", currentAnimation.c_str()))
		{
			for (auto const& animation : importer.m_animations)
			{
				std::string animationName = animation.first;
				bool is_selected = (currentAnimation == animationName);

				if (ImGui::Selectable(animationName.c_str(), is_selected))
				{
					currentAnimation = animationName;
				}
			}

			ImGui::EndCombo();
		}
		*/

		/*
		ImGui::Text("Mouse stored: %d, %d", HellEngine::Input::StoredMouseX, HellEngine::Input::StoredMouseY);
		ImGui::Text("Mouse editor: %d, %d", HellEngine::Input::MouseX_Editor, HellEngine::Input::MouseY_Editor);
		ImGui::Text("Last mouse: %d, %d", camera.m_oldX, camera.m_oldY);

		fbxsdk::FbxTime  m_startTime;
		fbxsdk::FbxTime  m_endTime;
		fbxsdk::FbxTime  m_currentTime;

		ImGui::Text("Current time: %s", importer.m_animations.at(currentAnimation).m_currentTime.GetTimeString());
		ImGui::Text("Start time:   %s", importer.m_animations.at(currentAnimation).m_startTime.GetTimeString());
		ImGui::Text("End time:     %s", importer.m_animations.at(currentAnimation).m_endTime.GetTimeString());
		*/

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		ImGui::BeginTabBar("MASTER_TAB_BAR", tab_bar_flags);
		ImGui::Text("\n");

		if (ImGui::BeginTabItem("Ragdoll")) {
			ShowRagdollMenu(game);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Scene")) {
			ShowSceneMenu(game);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Map")) {
			ShowMapMenu(game);
			ImGui::EndTabItem(); 
		}
		if (ImGui::BeginTabItem("Player")) {
			ShowPlayerMenu(game);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Shaders")) {
			ShowShaderMenu(game);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Misc")) {
			ShowOtherMenu(game);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Volumetric Blood")) {
			VolumetricBloodMenu(game);
			ImGui::EndTabItem();
		}
		
		ImGui::EndTabBar();
	
		///////////////////////////////////////////////////////////
		// new stuff //////////////////////////////////////////

		LevelEditor::RenderGizmo(io, game);

		// end new stuff //////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void CoreImGui::ShowMapMenu(Game* game)
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		float comboWidth = 200;
		ImGui::Indent(4.0f);

		if (ImGui::Button("Load Map"))
		{
			//Physics::RemoveHouse(&game->house);
			//game->house = File::LoadMap("Map.txt");
			Physics::RemoveHouse(GameData::p_house);
			GameData::LoadHouse("Map.txt");
		}
		ImGui::SameLine(); 
		if (ImGui::Button("Save Map")) {
			File::SaveMap("Map.txt", GameData::p_house);
		}
		ImGui::Text(" ");

		ImGui::BeginTabBar("ROOM_TAB_BAR", tab_bar_flags);
		for (int i = 0; i < GameData::p_house->m_rooms.size(); i++)
		{
			if (ImGui::BeginTabItem(("Room " + std::to_string(i)).c_str()))
			{
			//	ImGui::Text("Size: %.1f, %.1f", GameData::p_house->m_rooms[i].m_size.x, GameData::p_house->m_rooms[i].m_size.y);
			//	ImGui::Text("Pos:  %.1f, %.1f", GameData::p_house->m_rooms[i].m_position.x, GameData::p_house->m_rooms[i].m_position.y);

				// Size
				ImGui::Text("North Wall");
				ImGui::SameLine(); if (ImGui::Button("<##A")) {
					GameData::p_house->m_rooms[i].m_position.x -= 0.05f;
					GameData::p_house->m_rooms[i].m_size.x += 0.1f;
					GameData::p_house->RebuildAll();
				}
				ImGui::SameLine(); if (ImGui::Button(">##B")) {
					GameData::p_house->m_rooms[i].m_position.x += 0.05f;
					GameData::p_house->m_rooms[i].m_size.x -= 0.1f;
					GameData::p_house->RebuildAll();
				}
				ImGui::Text("South Wall");
				ImGui::SameLine(); if (ImGui::Button("<##C")) {
					GameData::p_house->m_rooms[i].m_position.x += 0.05f;
					GameData::p_house->m_rooms[i].m_size.x += 0.1f;
					GameData::p_house->RebuildAll();
				}
				ImGui::SameLine(); if (ImGui::Button(">##D")) {
					GameData::p_house->m_rooms[i].m_position.x -= 0.05f;
					GameData::p_house->m_rooms[i].m_size.x -= 0.1f;
					GameData::p_house->RebuildAll();
				}
				ImGui::Text("West Wall");
				ImGui::SameLine(); if (ImGui::Button("<##F")) {
					GameData::p_house->m_rooms[i].m_position.y -= 0.05f;
					GameData::p_house->m_rooms[i].m_size.y += 0.1f;
					GameData::p_house->RebuildAll();
				}
				ImGui::SameLine(); if (ImGui::Button(">##G")) {
					GameData::p_house->m_rooms[i].m_position.y += 0.05f;
					GameData::p_house->m_rooms[i].m_size.y -= 0.1f;
					GameData::p_house->RebuildAll();
				}
				ImGui::Text("East Wall");
				ImGui::SameLine(); if (ImGui::Button("<##H")) {
					GameData::p_house->m_rooms[i].m_position.y += 0.05f;
					GameData::p_house->m_rooms[i].m_size.y += 0.1f;
					GameData::p_house->RebuildAll();
				}
				ImGui::SameLine(); if (ImGui::Button(">##I")) {
					GameData::p_house->m_rooms[i].m_position.y -= 0.05f;
					GameData::p_house->m_rooms[i].m_size.y -= 0.1f;
					GameData::p_house->RebuildAll();
				}
				ImGui::Text("Story");
				ImGui::SameLine(); if (ImGui::Button("<##Asdf")) {
					GameData::p_house->m_rooms[i].m_story -= 1;
					GameData::p_house->RebuildAll();
				}
				ImGui::SameLine(); if (ImGui::Button(">##Bsdf")) {
					GameData::p_house->m_rooms[i].m_story += 1;
					GameData::p_house->RebuildAll();
				}
				if (ImGui::Button("Delete Room")) {
					GameData::p_house->m_rooms.erase(GameData::p_house->m_rooms.begin() + i);
					GameData::p_house->RebuildAll();
				}
				ImGui::SameLine(); if (ImGui::Button("New Room")) {
					GameData::p_house->m_rooms.push_back(Room(glm::vec2(0, 0), glm::vec2(4, 4), 0, GameData::p_house));
					GameData::p_house->RebuildAll();
				}
				ImGui::Text("Pos X, Z:  %.3f %.3f", GameData::p_house->m_rooms[i].m_position.x, GameData::p_house->m_rooms[i].m_position.y);
				ImGui::Text("Size X, Z: %.3f %.3f", GameData::p_house->m_rooms[i].m_size.x, GameData::p_house->m_rooms[i].m_size.y);
				ImGui::Text("Lower X, Lower Z: %.3f %.3f", GameData::p_house->m_rooms[i].m_lowerX, GameData::p_house->m_rooms[i].m_lowerZ);
				ImGui::Text("Upper X, Upper Z: %.3f %.3f", GameData::p_house->m_rooms[i].m_upperX, GameData::p_house->m_rooms[i].m_upperZ);

				ImGui::Text(" ");
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();

		ImGui::BeginTabBar("LIGHT_TAB_BAR", tab_bar_flags);
		for (int i = 0; i < GameData::p_house->m_lights.size(); i++)
		{
			if (ImGui::BeginTabItem(("Light " + std::to_string(i)).c_str()))
			{
				if (ImGui::InputFloat3("Position", glm::value_ptr(GameData::p_house->m_lights[i].m_position)))
					GameData::p_house->m_lights[i].Init();

				ImGui::InputInt("Type", &(GameData::p_house->m_lights[i].m_modelType));

				ImGui::InputFloat3("ModelPos", glm::value_ptr(GameData::p_house->m_lights[i].m_modelTransform.position));
				ImGui::InputFloat3("ModelRot", glm::value_ptr(GameData::p_house->m_lights[i].m_modelTransform.rotation));
				ImGui::InputFloat3("ModelSca", glm::value_ptr(GameData::p_house->m_lights[i].m_modelTransform.scale));

				ImGui::InputFloat("Radius   ", &GameData::p_house->m_lights[i].m_radius, 0.1f, 1.0f, "%0.5f");
				ImGui::InputFloat("Magic    ", &GameData::p_house->m_lights[i].m_magic, 0.1f, 1.0f, "%0.5f");
				ImGui::InputFloat("Strength ", &GameData::p_house->m_lights[i].m_strength, 0.1f, 1.0f, "%0.5f");
				ImGui::ColorEdit3("Color", glm::value_ptr(GameData::p_house->m_lights[i].m_color));
				ImGui::Text("RoomID: %d", GameData::p_house->m_lights[i].m_roomID);

				if (ImGui::Button("Delete Light")) {
					GameData::p_house->m_lights.erase(GameData::p_house->m_lights.begin() + i);
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button("New Light")) {
					GameData::p_house->m_lights.push_back(Light(glm::vec3(0, 1, 0)));
					game->RebuildMap();
				}
				
				ImGui::EndTabItem();
			}

		}
		ImGui::EndTabBar();
		ImGui::Text(" ");


		///////////
		// DOORS //
		/////////// 

		ImGui::BeginTabBar("DOOR_TAB_BAR", tab_bar_flags);

		std::vector<std::string> currentDoorAxis;
		for (int i = 0; i < GameData::p_house->m_doors.size(); i++)
			currentDoorAxis.push_back(Util::AxisToString(GameData::p_house->m_doors[i].m_axis));

		for (int i = 0; i < GameData::p_house->m_doors.size(); i++)
		{
			if (ImGui::BeginTabItem(("Door " + std::to_string(i)).c_str()))
			{
				if (ImGui::InputFloat3("Position", glm::value_ptr(GameData::p_house->m_doors[i].m_rootTransform.position)))
					game->RebuildMap(); 
				
				ImGui::Text("Story");
				ImGui::SameLine(); if (ImGui::Button("<##A")) {
					GameData::p_house->m_doors[i].m_story -= 1;
					GameData::p_house->m_doors[i].Reconfigure();
				}
				ImGui::SameLine(); if (ImGui::Button(">##B")) {
					GameData::p_house->m_doors[i].m_story += 1;
					GameData::p_house->m_doors[i].Reconfigure();
				}
				ImGui::Text("Rotate floor texture"); ImGui::SameLine(); ImGui::Checkbox("##checkBox3", &GameData::p_house->m_doors[i].m_floor.m_rotateTexture);

				ImGui::Text("Rotation Axis ");
				const char* axisList[4] = { "POS_X", "NEG_X", "POS_Z", "NEG_Z" };
				ImGui::SameLine();
				ImGui::PushItemWidth(100);
				if (ImGui::BeginCombo("##combo7", currentDoorAxis[i].c_str()))
				{
					for (int n = 0; n < 4; n++)
					{
						bool is_selected = (currentDoorAxis[i] == axisList[n]);
						if (ImGui::Selectable(axisList[n], is_selected)) {
							GameData::p_house->m_doors[i].m_axis = Util::StringToAxis(axisList[n]);
							GameData::p_house->m_doors[i].Reconfigure();
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Delete Door")) {					
					Door* door = &GameData::p_house->m_doors[i];
					GameData::p_house->DeleteDoor(door);
				}

				if (ImGui::Button("New Door")) {
					GameData::p_house->m_doors.push_back(Door(glm::vec2(0, 0), 0, Axis::POS_X, true));
				}

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		ImGui::Text(" ");


		/////////////
		// WINDOWS //
		///////////// 

		ImGui::BeginTabBar("WINDOWS_TAB_BAR", tab_bar_flags);

		std::vector<std::string> currentWindowAxis;
		for (int i = 0; i < GameData::p_house->m_windows.size(); i++)
			currentWindowAxis.push_back(Util::AxisToString(GameData::p_house->m_windows[i].m_axis));

		for (int i = 0; i < GameData::p_house->m_windows.size(); i++)
		{
			if (ImGui::BeginTabItem(("Window " + std::to_string(i)).c_str()))
			{
				if (ImGui::InputFloat("X Pos", &GameData::p_house->m_windows[i].m_transform.position.x)) {
					game->RebuildMap();
					GameData::p_house->m_windows[i].Reconfigure();
				}
				if (ImGui::InputFloat("Z Pos", &GameData::p_house->m_windows[i].m_transform.position.z)) {
					game->RebuildMap();
					GameData::p_house->m_windows[i].Reconfigure();
				}
				/*if (ImGui::InputFloat("height", &GameData::p_house->m_windows[i].m_startHeight)) {
					game->RebuildMap();
					GameData::p_house->m_windows[i].Reconfigure();
				}
				if (ImGui::InputInt("Story", &GameData::p_house->m_windows[i].m_story)) {
					game->RebuildMap(); 
					GameData::p_house->m_windows[i].Reconfigure();
				}*/

			/*	ImGui::Text("Story");
				ImGui::SameLine(); if (ImGui::Button("<##A")) {
					GameData::p_house->m_windows[i].m_story -= 1;
					GameData::p_house->m_windows[i].Reconfigure();
				}
				ImGui::SameLine(); if (ImGui::Button(">##B")) {
					GameData::p_house->m_windows[i].m_story += 1;
					GameData::p_house->m_windows[i].Reconfigure();
				}*/
			
				ImGui::Text("Rotation Axis ");
				const char* axisList[4] = { "POS_X", "NEG_X", "POS_Z", "NEG_Z" };
				ImGui::SameLine();
				ImGui::PushItemWidth(100);
				if (ImGui::BeginCombo("##combo7", currentWindowAxis[i].c_str()))
				{
					for (int n = 0; n < 4; n++)
					{
						bool is_selected = (currentWindowAxis[i] == axisList[n]);
						if (ImGui::Selectable(axisList[n], is_selected)) {
							GameData::p_house->m_windows[i].m_axis = Util::StringToAxis(axisList[n]);
							GameData::p_house->m_windows[i].Reconfigure();
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Delete Window")) {
					GameData::p_house->m_windows.erase(GameData::p_house->m_windows.begin() + i);
					game->RebuildMap();
				}
				ImGui::EndTabItem();
			}
		}
		if (ImGui::Button("New Window")) {
			GameData::p_house->m_windows.push_back(Window(glm::vec3(0,0,0), Axis::POS_X));
			game->RebuildMap();
		}
		ImGui::EndTabBar();
		ImGui::Text(" ");
		
		////////////////
		// STAIRCASES //
		//////////////// 

		ImGui::BeginTabBar("STAIRCASE_TAB_BAR", tab_bar_flags);

		std::vector<std::string> currentStaircaseAxis;
		for (int i = 0; i < GameData::p_house->m_staircases.size(); i++)
			currentStaircaseAxis.push_back(Util::AxisToString(GameData::p_house->m_staircases[i].m_axis));

		for (int i = 0; i < GameData::p_house->m_staircases.size(); i++)
		{
			if (ImGui::BeginTabItem(("Staircase " + std::to_string(i)).c_str()))
			{
				if (ImGui::InputFloat3("Position", glm::value_ptr(GameData::p_house->m_staircases[i].m_rootTransform.position)))
					game->RebuildMap();

				ImGui::Text("Story");
				ImGui::SameLine(); if (ImGui::Button("<##A")) {
					GameData::p_house->m_staircases[i].m_story -= 1;
					GameData::p_house->m_staircases[i].Reconfigure();
				}
				ImGui::SameLine(); if (ImGui::Button(">##B")) {
					GameData::p_house->m_staircases[i].m_story += 1;
					GameData::p_house->m_staircases[i].Reconfigure();
				}
				ImGui::Text("Rotate bottom floor tex"); ImGui::SameLine(); ImGui::Checkbox("##checkBox8", &GameData::p_house->m_staircases[i].m_bottomDoorwayFloor.m_rotateTexture);
				ImGui::Text("Rotate top floor tex"); ImGui::SameLine(); ImGui::Checkbox("##checkBox9", &GameData::p_house->m_staircases[i].m_topDoorwayFloor.m_rotateTexture);
				ImGui::Text("Rotate landing floor tex"); ImGui::SameLine(); ImGui::Checkbox("##checkBox10", &GameData::p_house->m_staircases[i].m_landingFloor.m_rotateTexture);

				ImGui::Text("Rotation Axis ");
				const char* axisList[4] = { "POS_X", "NEG_X", "POS_Z", "NEG_Z" };
				//std::string currentDoorAxis
				ImGui::SameLine();
				ImGui::PushItemWidth(100);
				if (ImGui::BeginCombo("##combo11", currentStaircaseAxis[i].c_str()))
				{
					for (int n = 0; n < 4; n++)
					{
						bool is_selected = (currentStaircaseAxis[i] == axisList[n]);
						if (ImGui::Selectable(axisList[n], is_selected)) {
							GameData::p_house->m_staircases[i].m_axis = Util::StringToAxis(axisList[n]);
							GameData::p_house->m_staircases[i].Reconfigure();
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Delete Stiarcase")) {
					GameData::p_house->m_staircases.erase(GameData::p_house->m_staircases.begin() + i);
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button("New Staircase")) {
					GameData::p_house->m_staircases.push_back(Staircase(glm::vec2(0, 0), Axis::POS_X, 0, 18, false, false, false, false));
					game->RebuildMap();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		ImGui::Text(" ");
	}

	void CoreImGui::ShowSceneMenu(Game* game)
	{
	
		static int selected = -1;



		if (ImGui::TreeNode("Scene"))
		{
			for (int n = 0; n < GameData::p_house->m_entities.size(); n++)
			{
				Entity* entity = &GameData::p_house->m_entities[n];
				char buf[32];
				sprintf(buf, entity->m_tag, n);
				if (ImGui::Selectable(buf, selected == n))
					selected = n;
			}
			ImGui::TreePop();
		}

		// Properties
		ImGui::Text("\n");
		ImGui::Separator();
		ImGui::Text("\n");
		ImGui::Text("Properties");

		if (selected != -1)
		{
			Entity* entity = &GameData::p_house->m_entities[selected];
			ImGui::InputText("Tag", entity->m_tag, 64);

			// Build models list
			std::vector<std::string> modelList;
			for (Model& model : AssetManager::models)
				modelList.push_back(model.name);

			// Build materials list
			std::vector<std::string> materialList;
			for (Material& material : AssetManager::materials)
				materialList.push_back(material.name);

			std::vector<std::string> currentModelName;
			for (int i = 0; i < GameData::p_house->m_entities.size(); i++)
				currentModelName.push_back(AssetManager::GetModelNameByID(GameData::p_house->m_entities[i].m_materialID));

			std::vector<std::string> currentMaterialName;
			for (int i = 0; i < GameData::p_house->m_entities.size(); i++)
				currentMaterialName.push_back(AssetManager::GetMaterialNameByID(GameData::p_house->m_entities[i].m_materialID));

			// Show models list
			if (ImGui::BeginCombo("Model", currentModelName[selected].c_str()))
			{
				for (int n = 0; n < modelList.size(); n++)
				{
					bool is_selected = (currentModelName[selected] == modelList[n]);
					if (ImGui::Selectable(modelList[n].c_str(), is_selected)) {
						entity->SetModelID(AssetManager::GetModelIDByName(modelList[n]));
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			// Show materials list
			if (ImGui::BeginCombo("Material", currentMaterialName[selected].c_str()))
			{
				for (int n = 0; n < materialList.size(); n++)
				{
					bool is_selected = (currentMaterialName[selected] == materialList[n]);
					if (ImGui::Selectable(materialList[n].c_str(), is_selected)) {
						entity->m_materialID = AssetManager::GetMaterialIDByName(materialList[n]);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

		

			ImGui::PushItemWidth(200);
			ImGui::InputFloat3("Position", glm::value_ptr(entity->m_transform.position));
			ImGui::InputFloat3("Rotation", glm::value_ptr(entity->m_transform.rotation));
			ImGui::InputFloat3("Scale   ", glm::value_ptr(entity->m_transform.scale));
		}
	}

	void CoreImGui::VolumetricBloodMenu(Game* game)
	{
		/*static int selected = -1;

		// Properties
		ImGui::Text("gAlbedo Blending");

		if (selected != -1)
		{
			Entity* entity = &GameData::p_house->m_entities[selected];
			ImGui::InputText("Tag", entity->m_tag, 64);

			// Build models list
			std::vector<std::string> modelList;
			for (Model& model : AssetManager::models)
				modelList.push_back(model.name);

			// Build materials list
			std::vector<std::string> materialList;
			for (Material& material : AssetManager::materials)
				materialList.push_back(material.name);

			std::vector<std::string> gAlbedoSource;
			gAlbedoSource.push_back()

			//for (int i = 0; i < GameData::p_house->m_entities.size(); i++)
			//	blendingModes.push_back(AssetManager::GetModelNameByID(GameData::p_house->m_entities[i].m_materialID));

			std::vector<std::string> currentMaterialName;
			for (int i = 0; i < GameData::p_house->m_entities.size(); i++)
				currentMaterialName.push_back(AssetManager::GetMaterialNameByID(GameData::p_house->m_entities[i].m_materialID));

			// Show models list
			if (ImGui::BeginCombo("Model", blendingModes[selected].c_str()))
			{
				for (int n = 0; n < modelList.size(); n++)
				{
					bool is_selected = (blendingModes[selected] == modelList[n]);
					if (ImGui::Selectable(modelList[n].c_str(), is_selected)) {
						entity->SetModelID(AssetManager::GetModelIDByName(modelList[n]));
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			// Show materials list
			if (ImGui::BeginCombo("Material", currentMaterialName[selected].c_str()))
			{
				for (int n = 0; n < materialList.size(); n++)
				{
					bool is_selected = (currentMaterialName[selected] == materialList[n]);
					if (ImGui::Selectable(materialList[n].c_str(), is_selected)) {
						entity->m_materialID = AssetManager::GetMaterialIDByName(materialList[n]);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}



			ImGui::PushItemWidth(200);
			ImGui::InputFloat3("Position", glm::value_ptr(entity->m_transform.position));
			ImGui::InputFloat3("Rotation", glm::value_ptr(entity->m_transform.rotation));
			ImGui::InputFloat3("Scale   ", glm::value_ptr(entity->m_transform.scale));
		}*/
	}

	void CoreImGui::ShowPlayerMenu(Game* game)
	{
		ImGui::Text("Cam rot: %.3f, %.3f, %.3f", game->camera.m_transform.rotation.x, game->camera.m_transform.rotation.y, game->camera.m_transform.rotation.z);
		ImGui::Text("Cam pos: %.3f, %.3f, %.3f", game->camera.m_transform.position.x, game->camera.m_transform.position.y, game->camera.m_transform.position.z);
		ImGui::Text("Mouse: %d, %d", HellEngine::Input::s_mouseX, HellEngine::Input::s_mouseY);
		ImGui::Text(" ");

		if (game->m_player.m_movementState == PlayerMovementState::WALKING)
			ImGui::Text("Movement State: WALKING");
		else if (game->m_player.m_movementState == PlayerMovementState::STOPPED)
			ImGui::Text("Movement State: STOPPED");
		else
			ImGui::Text("Movement State: ERROR");


		ImGui::PushItemWidth(100);

		//ImGui::InputFloat("Scale", &Staircase::trimScale, 0.1f, 1.0f, "%0.5f");
		//ImGui::InputFloat("Offset", &Staircase::yOffset, 0.1f, 1.0f, "%0.5f");

		ImGui::InputFloat("Walk Speed", &game->m_player.m_characterController.m_movementSpeedWalking, 0.1f, 1.0f, "%0.2f");
		ImGui::InputFloat("Run Speed", &game->m_player.m_characterController.m_movementSpeedRunning, 0.1f, 1.0f, "%0.2f");
		ImGui::InputFloat("Crouch Speed", &game->m_player.m_characterController.m_movementSpeedCrouching, 0.1f, 1.0f, "%0.2f");
		ImGui::InputFloat("Jump Strength", &game->m_player.m_characterController.m_jumpStrength, 0.1f, 1.0f, "%0.2f");
		ImGui::InputFloat("Standing height", &game->m_player.m_characterController.m_viewHeightStanding, 0.1f, 1.0f, "%0.2f");
		ImGui::InputFloat("Crouching height", &game->m_player.m_characterController.m_viewHeightCrouching, 0.1f, 1.0f, "%0.2f");
		ImGui::InputFloat("Crouch Down Speed", &game->m_player.m_characterController.m_crouchDownSpeed, 0.1f, 1.0f, "%0.2f");
	}

	void CoreImGui::ShowOtherMenu(Game* game)
	{
		ImGui::Text("\nCAMERA");
		ImGui::Text("Ray cast: %s", Util::PhysicsObjectEnumToString(game->m_cameraRaycast.m_objectType));
		ImGui::Text("Ray dist: %f", game->m_cameraRaycast.m_distance);
		ImGui::Text("View pos: (%f, %f, %f)", game->camera.m_viewPos.x, game->camera.m_viewPos.y, game->camera.m_viewPos.z);
		ImGui::Text("Front:    (%f, %f, %f)", game->camera.m_Front.x, game->camera.m_Front.y, game->camera.m_Front.z);


		ImGui::Text("\n");
		ImGui::InputFloat("##inspectfloaat", &Config::INSPECT_FOV, 0.0f, 9.0f, 10.0f);


		ImGui::Text("\n");
		ImGui::Text("TEST_QUAT");
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("w", &Config::TEST_QUAT.w, 0.0f, 9.0f, 10.0f);
		ImGui::SameLine(); ImGui::InputFloat("x", &Config::TEST_QUAT.x, 0.0f, 9.0f, 10.0f);
		ImGui::SameLine(); ImGui::InputFloat("y", &Config::TEST_QUAT.y, 0.0f, 9.0f, 10.0f);
		ImGui::SameLine(); ImGui::InputFloat("z", &Config::TEST_QUAT.z, 0.0f, 9.0f, 10.0f);
		ImGui::Text("(%f, %f, %f, %f)", Config::TEST_QUAT.w, Config::TEST_QUAT.x, Config::TEST_QUAT.y, Config::TEST_QUAT.z);
		
		ImGui::Text("TEST_QUAT2");
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("w_", &Config::TEST_QUAT2.w, 0.0f, 9.0f, 10.0f);
		ImGui::SameLine(); ImGui::InputFloat("x_", &Config::TEST_QUAT2.x, 0.0f, 9.0f, 10.0f);
		ImGui::SameLine(); ImGui::InputFloat("y_", &Config::TEST_QUAT2.y, 0.0f, 9.0f, 10.0f);
		ImGui::SameLine(); ImGui::InputFloat("z_", &Config::TEST_QUAT2.z, 0.0f, 9.0f, 10.0f);
		ImGui::Text("(%f, %f, %f, %f)", Config::TEST_QUAT2.w, Config::TEST_QUAT2.x, Config::TEST_QUAT2.y, Config::TEST_QUAT2.z);

		glm::quat q = Config::TEST_QUAT * Config::TEST_QUAT2;
		ImGui::Text("(%f, %f, %f, %f)", q.w, q.x, q.y, q.z);
		
		ImGui::PushItemWidth(100);

		ImGui::Text("\n");
		ImGui::Text("TEST_FLOAT"); ImGui::SameLine();
		ImGui::InputFloat("##Gasdasd", &Config::TEST_FLOAT, 0.0f, 9.0f, 10.0f);
		ImGui::Text("TEST_FLOAT2"); ImGui::SameLine();
		ImGui::InputFloat("##Gadfsdasd", &Config::TEST_FLOAT2, 0.0f, 9.0f, 10.0f);
		ImGui::Text("TEST_FLOAT3"); ImGui::SameLine();
		ImGui::InputFloat("##Gasdasdfsd", &Config::TEST_FLOAT3, 0.0f, 9.0f, 10.0f);
		ImGui::Text("TEST_FLOAT4"); ImGui::SameLine();
		ImGui::InputFloat("##GasdDFasdfsd", &Config::TEST_FLOAT4, 0.0f, 9.0f, 10.0f);
		ImGui::Text("TEST_FLOAT5"); ImGui::SameLine();
		ImGui::InputFloat("##Gasfghdasdfsd", &Config::TEST_FLOAT5, 0.0f, 9.0f, 10.0f);
		ImGui::Text("TEST_FLOAT6"); ImGui::SameLine();
		ImGui::InputFloat("##GasdDfghFasdfsd", &Config::TEST_FLOAT6, 0.0f, 9.0f, 10.0f);

		ImGui::Text("\n");
		ImGui::Text("sway amount"); ImGui::SameLine();
		ImGui::InputFloat("##G", &Config::SWAY_AMOUNT, 0.0f, 9.0f, 10.0f);
		ImGui::Text("smooth amount"); ImGui::SameLine();
		ImGui::InputFloat("##Gf", &Config::SMOOTH_AMOUNT, 0.0f, 9.0f, 10.0f);
		ImGui::Text("min X"); ImGui::SameLine();
		ImGui::InputFloat("##fGf", &Config::SWAY_MIN_X, 0.0f, 9.0f, 10.0f);
		ImGui::Text("man X"); ImGui::SameLine();
		ImGui::InputFloat("##ffGf", &Config::SWAY_MAX_X, 0.0f, 9.0f, 10.0f);
		ImGui::Text("min Y"); ImGui::SameLine();
		ImGui::InputFloat("##fgGf", &Config::SWAY_MIN_Y, 0.0f, 9.0f, 10.0f);
		ImGui::Text("man Y"); ImGui::SameLine();
		ImGui::InputFloat("##fqwefGf", &Config::SWAY_MAX_Y, 0.0f, 9.0f, 10.0f);

		ImGui::Text("\n");

		ImGui::InputFloat("glPolygonOffset factor: ", &Renderer::s_polygonFactor, 0.1f, 1.0f, "%0.5f");
		ImGui::InputFloat("glPolygonOffset units:  ", &Renderer::s_polygonUnits, 0.1f, 1.0f, "%0.5f");
		;

		/*ImGui::Text("\nSHOTTY");
		ImGui::InputFloat3("Pos", glm::value_ptr(game->m_shotgunTransform.position));
		ImGui::InputFloat3("Rot", glm::value_ptr(game->m_shotgunTransform.rotation));
		ImGui::InputFloat3("Scl", glm::value_ptr(game->m_shotgunTransform.scale));
		*/

		ImGui::Text("\nDEBUG TRANSFORM");
		ImGui::InputFloat3("Poso", glm::value_ptr(Renderer::s_DebugTransform.position));
		ImGui::InputFloat3("Rota", glm::value_ptr(Renderer::s_DebugTransform.rotation));
		ImGui::InputFloat3("Scla", glm::value_ptr(Renderer::s_DebugTransform.scale));
		ImGui::Text("\nDEBUG TRANSFORM2");
		ImGui::InputFloat3("Poso2", glm::value_ptr(Renderer::s_DebugTransform2.position));
		ImGui::InputFloat3("Rota2", glm::value_ptr(Renderer::s_DebugTransform2.rotation));
		ImGui::InputFloat3("Scla2", glm::value_ptr(Renderer::s_DebugTransform2.scale));
		ImGui::Text("\nDEBUG TRANSFORM3");
		ImGui::InputFloat3("Poso3", glm::value_ptr(Renderer::s_DebugTransform3.position));
		ImGui::InputFloat3("Rota3", glm::value_ptr(Renderer::s_DebugTransform3.rotation));
		ImGui::InputFloat3("Scla3", glm::value_ptr(Renderer::s_DebugTransform3.scale));

		ImGui::Text("WIP"); ImGui::SameLine(); ImGui::Checkbox("##checkBoxq9", &Renderer::s_demo);
		ImGui::Text("Render Door Way Volumes"); ImGui::SameLine(); ImGui::Checkbox("##checkBo1123xq9", &Renderer::b_renderDoorWayVolumes);
		


		/*if (game->m_player.m_gunState == GunState::FIRING)
			ImGui::Text("fireState: FIRING");
		else if (game->m_player.m_gunState == GunState::IDLE)
			ImGui::Text("fireState: IDLE");
		else if (game->m_player.m_gunState == GunState::RELOADING)
			ImGui::Text("fireState: RELOADING");
		else
			ImGui::Text("fireState: ERROR");
		*/

		//ImGui::Text("Mouse down: %i", Input::s_leftMouseDown);
		//ImGui::Text("Mouse pressed: %i", Input::s_leftMousePressed);
		//ImGui::Text("Mouse down last frame: %i", Input::s_leftMouseDownLastFrame);

		//if (game->m_HUDshotgun.m_currentAnimation != NULL)
		//	ImGui::Text("\nHUD shotgun: %s, %s, %s", game->m_HUDshotgun.m_currentAnimationName, game->m_HUDshotgun.m_currentAnimationTime.GetTimeString(), game->m_HUDshotgun.m_currentAnimation->m_endTime.GetTimeString());

		ImGui::Text("\nRaycast:  %s", Util::PhysicsObjectEnumToString(game->m_cameraRaycast.m_objectType));
		ImGui::Text("Distance: %f", game->m_cameraRaycast.m_distance);
	}

	void CoreImGui::ShowShaderMenu(Game* game)
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		ImGui::BeginTabBar("SHADER_TAB_BAR", tab_bar_flags);
		ImGui::Text("\n");

		if (ImGui::BeginTabItem("DOF")) {
			ImGui::SameLine(); ImGui::Checkbox("Show Focus##showFocus", &Config::DOF_showFocus);
			ImGui::SameLine(); ImGui::Checkbox("Vignetting##vignetting", &Config::DOF_vignetting);
			ImGui::SliderFloat("Vignette Out", &Config::DOF_vignout, 0.0f, 5.0f);
			ImGui::SliderFloat("Vignette In", &Config::DOF_vignin, 0.0f, 5.0f);
			ImGui::SliderFloat("Vignette Fade", &Config::DOF_vignfade, 0.0f, 200.0f);
			ImGui::SliderFloat("CoC", &Config::DOF_CoC, 0.0f, 5.0f);
			ImGui::SliderFloat("Max Blur", &Config::DOF_maxblur, 0.0f, 5.0f);
			ImGui::SliderInt("Samples", &Config::DOF_samples, 0, 10);
			ImGui::SliderInt("Rings", &Config::DOF_rings, 0, 10);
			ImGui::SliderFloat("Threshold", &Config::DOF_threshold, 0.0f, 5.0f);
			//ImGui::SliderFloat("Gain", &Config::DOF_gain, 0.0f, 5.0f);
			ImGui::SliderFloat("Bias", &Config::DOF_bias, 0.0f, 5.0f);
			ImGui::SliderFloat("Fringe", &Config::DOF_fringe, 0.0f, 5.0f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	void CoreImGui::ShowRagdollMenu(Game* game)
	{
		return;

		
	}
}