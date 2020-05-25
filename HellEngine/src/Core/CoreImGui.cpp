#include "hellpch.h"
#include "CoreImGui.h"
#include "CoreGL.h"
#include "Helpers/AssetManager.h"
#include "Config.h"
#include "File.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

namespace HellEngine
{
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
		
		ImGui::EndTabBar();
	


		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void CoreImGui::ShowMapMenu(Game* game)
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		float comboWidth = 200;
		ImGui::Indent(4.0f);

		if (ImGui::Button("Load Map")) {
			game->house = File::LoadMap("Map.txt"); 
			game->RebuildMap();
		}
		ImGui::SameLine(); 
		if (ImGui::Button("Save Map")) {
			File::SaveMap("Map.txt", &game->house);
		}
		ImGui::Text(" ");

		ImGui::BeginTabBar("ROOM_TAB_BAR", tab_bar_flags);
		for (int i = 0; i < game->house.m_rooms.size(); i++)
		{
			if (ImGui::BeginTabItem(("Room " + std::to_string(i)).c_str()))
			{
			//	ImGui::Text("Size: %.1f, %.1f", game->house.m_rooms[i].m_size.x, game->house.m_rooms[i].m_size.y);
			//	ImGui::Text("Pos:  %.1f, %.1f", game->house.m_rooms[i].m_position.x, game->house.m_rooms[i].m_position.y);

				// Size
				ImGui::Text("North Wall");
				ImGui::SameLine(); if (ImGui::Button("<##A")) {
					game->house.m_rooms[i].m_position.x -= 0.05f;
					game->house.m_rooms[i].m_size.x += 0.1f;
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button(">##B")) {
					game->house.m_rooms[i].m_position.x += 0.05f;
					game->house.m_rooms[i].m_size.x -= 0.1f;
					game->RebuildMap();
				}
				ImGui::Text("South Wall");
				ImGui::SameLine(); if (ImGui::Button("<##C")) {
					game->house.m_rooms[i].m_position.x += 0.05f;
					game->house.m_rooms[i].m_size.x += 0.1f;
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button(">##D")) {
					game->house.m_rooms[i].m_position.x -= 0.05f;
					game->house.m_rooms[i].m_size.x -= 0.1f;
					game->RebuildMap();
				}
				ImGui::Text("West Wall");
				ImGui::SameLine(); if (ImGui::Button("<##F")) {
					game->house.m_rooms[i].m_position.y -= 0.05f;
					game->house.m_rooms[i].m_size.y += 0.1f;
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button(">##G")) {
					game->house.m_rooms[i].m_position.y += 0.05f;
					game->house.m_rooms[i].m_size.y -= 0.1f;
					game->RebuildMap();
				}
				ImGui::Text("East Wall");
				ImGui::SameLine(); if (ImGui::Button("<##H")) {
					game->house.m_rooms[i].m_position.y += 0.05f;
					game->house.m_rooms[i].m_size.y += 0.1f;
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button(">##I")) {
					game->house.m_rooms[i].m_position.y -= 0.05f;
					game->house.m_rooms[i].m_size.y -= 0.1f;
					game->RebuildMap();
				}
				ImGui::Text("Story");
				ImGui::SameLine(); if (ImGui::Button("<##Asdf")) {
					game->house.m_rooms[i].m_story -= 1;
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button(">##Bsdf")) {
					game->house.m_rooms[i].m_story += 1;
					game->RebuildMap();
				}
				if (ImGui::Button("Delete Room")) {
					game->house.m_rooms.erase(game->house.m_rooms.begin() + i);
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button("New Room")) {
					game->house.m_rooms.push_back(Room(glm::vec2(0, 0), glm::vec2(4, 4), 0));
					game->RebuildMap();
				}
				ImGui::Text("Pos X, Z:  %.3f %.3f", game->house.m_rooms[i].m_position.x, game->house.m_rooms[i].m_position.y);
				ImGui::Text("Size X, Z: %.3f %.3f", game->house.m_rooms[i].m_size.x, game->house.m_rooms[i].m_size.y);
				ImGui::Text("Lower X, Lower Z: %.3f %.3f", game->house.m_rooms[i].m_lowerX, game->house.m_rooms[i].m_lowerZ);
				ImGui::Text("Upper X, Upper Z: %.3f %.3f", game->house.m_rooms[i].m_upperX, game->house.m_rooms[i].m_upperZ);

				ImGui::Text(" ");
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();

		ImGui::BeginTabBar("LIGHT_TAB_BAR", tab_bar_flags);
		for (int i = 0; i < game->house.m_lights.size(); i++)
		{
			if (ImGui::BeginTabItem(("Light " + std::to_string(i)).c_str()))
			{
				if (ImGui::InputFloat3("Position", glm::value_ptr(game->house.m_lights[i].m_position)))
					game->house.m_lights[i].Init();

				ImGui::InputFloat3("ModelPos", glm::value_ptr(game->house.m_lights[i].m_modelTransform.position));
				ImGui::InputFloat3("ModelRot", glm::value_ptr(game->house.m_lights[i].m_modelTransform.rotation));
				ImGui::InputFloat3("ModelSca", glm::value_ptr(game->house.m_lights[i].m_modelTransform.scale));

				ImGui::InputFloat("Radius   ", &game->house.m_lights[i].m_radius, 0.1f, 1.0f, "%0.5f");
				ImGui::InputFloat("Magic    ", &game->house.m_lights[i].m_magic, 0.1f, 1.0f, "%0.5f");
				ImGui::InputFloat("Strength ", &game->house.m_lights[i].m_strength, 0.1f, 1.0f, "%0.5f");
				ImGui::ColorEdit3("Color", glm::value_ptr(game->house.m_lights[i].m_color));
				ImGui::Text("RoomID: %d", game->house.m_lights[i].m_roomID);

				if (ImGui::Button("Delete Light")) {
					game->house.m_lights.erase(game->house.m_lights.begin() + i);
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button("New Light")) {
					game->house.m_lights.push_back(Light(glm::vec3(0, 1, 0)));
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
		for (int i = 0; i < game->house.m_doors.size(); i++)
			currentDoorAxis.push_back(Util::AxisToString(game->house.m_doors[i].m_axis));

		for (int i = 0; i < game->house.m_doors.size(); i++)
		{
			if (ImGui::BeginTabItem(("Door " + std::to_string(i)).c_str()))
			{
				if (ImGui::InputFloat3("Position", glm::value_ptr(game->house.m_doors[i].m_rootTransform.position)))
					game->RebuildMap(); 
				
				ImGui::Text("Story");
				ImGui::SameLine(); if (ImGui::Button("<##A")) {
					game->house.m_doors[i].m_story -= 1;
					game->house.m_doors[i].Reconfigure();
				}
				ImGui::SameLine(); if (ImGui::Button(">##B")) {
					game->house.m_doors[i].m_story += 1;
					game->house.m_doors[i].Reconfigure();
				}
				ImGui::Text("Rotate floor texture"); ImGui::SameLine(); ImGui::Checkbox("##checkBox3", &game->house.m_doors[i].m_floor.m_rotateTexture);

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
							game->house.m_doors[i].m_axis = Util::StringToAxis(axisList[n]);
							game->house.m_doors[i].Reconfigure();
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Delete Door")) {
					game->house.m_doors.erase(game->house.m_doors.begin() + i);
					game->RebuildMap();
				}
				if (ImGui::Button("New Door")) {
					game->house.m_doors.push_back(Door(glm::vec2(0, 0), 0, Axis::POS_X, true));
					game->RebuildMap();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		ImGui::Text(" ");
		
		////////////////
		// STAIRCASES //
		//////////////// 

		ImGui::BeginTabBar("STAIRCASE_TAB_BAR", tab_bar_flags);

		std::vector<std::string> currentStaircaseAxis;
		for (int i = 0; i < game->house.m_staircases.size(); i++)
			currentStaircaseAxis.push_back(Util::AxisToString(game->house.m_staircases[i].m_axis));

		for (int i = 0; i < game->house.m_staircases.size(); i++)
		{
			if (ImGui::BeginTabItem(("Staircase " + std::to_string(i)).c_str()))
			{
				if (ImGui::InputFloat3("Position", glm::value_ptr(game->house.m_staircases[i].m_rootTransform.position)))
					game->RebuildMap();

				ImGui::Text("Story");
				ImGui::SameLine(); if (ImGui::Button("<##A")) {
					game->house.m_staircases[i].m_story -= 1;
					game->house.m_staircases[i].Reconfigure();
				}
				ImGui::SameLine(); if (ImGui::Button(">##B")) {
					game->house.m_staircases[i].m_story += 1;
					game->house.m_staircases[i].Reconfigure();
				}
				ImGui::Text("Rotate bottom floor tex"); ImGui::SameLine(); ImGui::Checkbox("##checkBox8", &game->house.m_staircases[i].m_bottomDoorwayFloor.m_rotateTexture);
				ImGui::Text("Rotate top floor tex"); ImGui::SameLine(); ImGui::Checkbox("##checkBox9", &game->house.m_staircases[i].m_topDoorwayFloor.m_rotateTexture);
				ImGui::Text("Rotate landing floor tex"); ImGui::SameLine(); ImGui::Checkbox("##checkBox10", &game->house.m_staircases[i].m_landingFloor.m_rotateTexture);

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
							game->house.m_staircases[i].m_axis = Util::StringToAxis(axisList[n]);
							game->house.m_staircases[i].Reconfigure();
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Delete Stiarcase")) {
					game->house.m_staircases.erase(game->house.m_staircases.begin() + i);
					game->RebuildMap();
				}
				ImGui::SameLine(); if (ImGui::Button("New Staircase")) {
					game->house.m_staircases.push_back(Staircase(glm::vec2(0, 0), Axis::POS_X, 0, 18, false, false, false, false));
					game->RebuildMap();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		ImGui::Text(" ");
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

		ImGui::InputFloat("Scale", &Staircase::trimScale, 0.1f, 1.0f, "%0.5f");
		ImGui::InputFloat("Offset", &Staircase::yOffset, 0.1f, 1.0f, "%0.5f");
		ImGui::InputFloat("Walking Speed", &game->m_player.m_characterController.m_walkingSpeed, 0.1f, 1.0f, "%0.5f");
		ImGui::InputFloat("View height", &game->camera.m_viewHeight, 0.1f, 1.0f, "%0.5f");
	}

	void CoreImGui::ShowOtherMenu(Game* game)
	{
		ImGui::Text("\nCAMERA");
		ImGui::Text("Ray cast: %s", game->m_cameraRaycast.m_name);
		ImGui::Text("Ray dist: %f", game->m_cameraRaycast.m_distance);
		ImGui::Text("View pos: (%f, %f, %f)", game->camera.m_viewPos.x, game->camera.m_viewPos.y, game->camera.m_viewPos.z);
		ImGui::Text("Front:    (%f, %f, %f)", game->camera.m_Front.x, game->camera.m_Front.y, game->camera.m_Front.z);


		ImGui::InputFloat("glPolygonOffset factor: ", &Renderer::s_polygonFactor, 0.1f, 1.0f, "%0.5f");
		ImGui::InputFloat("glPolygonOffset units:  ", &Renderer::s_polygonUnits, 0.1f, 1.0f, "%0.5f");
		;

		ImGui::Text("\nSHOTTY");
		ImGui::InputFloat3("Pos", glm::value_ptr(game->m_shotgunTransform.position));
		ImGui::InputFloat3("Rot", glm::value_ptr(game->m_shotgunTransform.rotation));
		ImGui::InputFloat3("Scl", glm::value_ptr(game->m_shotgunTransform.scale));


		ImGui::Text("\nDEBUG TRANSFORM");
		ImGui::InputFloat3("Poso", glm::value_ptr(Renderer::s_DebugTransform.position));
		ImGui::InputFloat3("Rota", glm::value_ptr(Renderer::s_DebugTransform.rotation));
		ImGui::InputFloat3("Scla", glm::value_ptr(Renderer::s_DebugTransform.scale));
		ImGui::Text("\nDEBUG TRANSFORM2");
		ImGui::InputFloat3("Poso2", glm::value_ptr(Renderer::s_DebugTransform2.position));
		ImGui::InputFloat3("Rota2", glm::value_ptr(Renderer::s_DebugTransform2.rotation));
		ImGui::InputFloat3("Scla2", glm::value_ptr(Renderer::s_DebugTransform2.scale));
		
		ImGui::Text("WIP"); ImGui::SameLine(); ImGui::Checkbox("##checkBoxq9", &Renderer::s_demo);



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

		if (game->m_HUDshotgun.m_currentAnimation != NULL)
			ImGui::Text("\nHUD shotgun: %s, %s, %s", game->m_HUDshotgun.m_currentAnimationName, game->m_HUDshotgun.m_currentAnimationTime.GetTimeString(), game->m_HUDshotgun.m_currentAnimation->m_endTime.GetTimeString());

		//ImGui::Text("\nRaycast:  %s", game->m_cameraRaycast.m_name);
		ImGui::Text("Distance: %f", game->m_cameraRaycast.m_distance);

		/*ImGui::Text("\nCHARACTER\nVelocity: %f, %f, %f",
			game->m_player.m_characterController.GetVelocity().x,
			game->m_player.m_characterController.GetVelocity().y,
			game->m_player.m_characterController.GetVelocity().z);*/
		ImGui::Text("Manual Velocity: %f, %f, %f",
			game->m_player.m_characterController.m_manualVelocity.x,
			game->m_player.m_characterController.m_manualVelocity.y,
			game->m_player.m_characterController.m_manualVelocity.z);
		ImGui::Text("Deceleration: %f", game->m_player.m_characterController.m_deceleration);
		ImGui::Text("IsOnGround(): %i", game->m_player.m_characterController.IsOnGround());

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
}