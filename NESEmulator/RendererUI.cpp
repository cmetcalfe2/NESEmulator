#include "Renderer.h"

void Renderer::RenderMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Open", NULL, &showOpenROMFileDialog);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Input"))
		{
			ImGui::MenuItem("Set Up Controllers", NULL, &showControllerSettingsDialog);
			ImGui::MenuItem("Controller Profiles", NULL, &showControllerProfilesDialog);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void Renderer::RenderFileDialog()
{
	if (showOpenROMFileDialog)
	{
		if (ImGuiFileDialog::Instance()->FileDialog("Choose ROM File", ".nes\0", ".", ""))
		{
			if (ImGuiFileDialog::Instance()->IsOk == true)
			{
				nes->LoadROM(ImGuiFileDialog::Instance()->GetFilepathName());
			}

			showOpenROMFileDialog = false;
		}
	}
}

void Renderer::RenderControllerSetupDialog()
{
	if (showControllerSettingsDialog)
	{
		ImGui::Begin("Controller Settings", &showControllerSettingsDialog);

		for (int i = 0; i < MAX_CONTROLLERS; i++)
		{
			ImGui::PushID(i);
			ImGui::Text("Controller %i", i + 1);

			std::string profileName = "";
			if (Settings::instance->controllerSettings[i].controllerProfileIndex != -1)
			{
				profileName = Settings::instance->controllerProfiles[Settings::instance->controllerSettings[i].controllerProfileIndex].name;
			}
			else
			{
				profileName = "No Profile Assigned";
			}

			if (ImGui::BeginCombo("Profile", profileName.c_str()))
			{
				for (int j = 0; j < Settings::instance->controllerProfiles.size(); j++)
				{
					if (ImGui::Selectable(Settings::instance->controllerProfiles[j].name.c_str()))
						Settings::instance->controllerSettings[i].controllerProfileIndex = j;
				}

				ImGui::EndCombo();
			}

			if (Settings::instance->controllerSettings[i].controllerProfileIndex != -1)
			{
				if (Settings::instance->controllerProfiles[Settings::instance->controllerSettings[i].controllerProfileIndex].inputControllerType == INPUTTYPE_CONTROLLER)
				{
					if (ImGui::Button("Set Physical Controller"))
					{
						Settings::instance->controllerSettings[i].physicalControllerIndex = SetPhysicalController();
					}
				}
			}

			ImGui::PopID();
		}

		if (ImGui::Button("Save"))
		{
			Settings::instance->Save();
			showControllerSettingsDialog = false;
		}

		ImGui::End();
	}
}

void Renderer::RenderControllerProfileDialog()
{
	if (showControllerProfilesDialog)
	{
		ImGui::Begin("Controller Profiles", &showControllerProfilesDialog);
		for (int i = 0; i < Settings::instance->controllerProfiles.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Text(Settings::instance->controllerProfiles[i].name.c_str());
			if (ImGui::Button("Edit"))
			{
				editControllerProfileIndex = i;
				curControllerProfile = Settings::instance->controllerProfiles[i];
				showEditControllerProfileDialog = true;
			}
			if (ImGui::Button("Delete"))
			{	
				Settings::instance->DeleteControllerProfile(i);
			}
			ImGui::PopID();
		}

		if (ImGui::Button("Create New"))
		{
			curControllerProfile = ControllerProfile();
			showNewControllerProfileDialog = true;
		}

		ImGui::End();
	}

	if (showNewControllerProfileDialog)
	{
		ImGui::Begin("New Controller Profile", &showNewControllerProfileDialog);
		ImGui::InputText("Name", &curControllerProfile.name);

		if (ImGui::BeginCombo("NES Controller Type", Settings::nesControllerTypeNames[curControllerProfile.nesControllerType].c_str()))
		{
			for (auto const& it : Settings::nesControllerTypeNames)
			{
				if (ImGui::Selectable(it.second.c_str()))
					curControllerProfile.nesControllerType = it.first;
			}
			
			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Input Controller Type", Settings::inputControllerTypeNames[curControllerProfile.inputControllerType].c_str()))
		{
			for (auto const& it : Settings::inputControllerTypeNames)
			{
				if (ImGui::Selectable(it.second.c_str()))
					curControllerProfile.inputControllerType = it.first;
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Save"))
		{
			Settings::instance->AddNewControllerProfile(curControllerProfile);
			Settings::instance->Save();
			showNewControllerProfileDialog = false;
		}

		ImGui::End();
	}
}

void Renderer::RenderEditControllerProfileDialog()
{
	if (showEditControllerProfileDialog)
	{
		ImGui::Begin("Edit Controller Profile", &showEditControllerProfileDialog);

		// Name
		ImGui::InputText("Name", &curControllerProfile.name);

		// NES Controller type
		if (ImGui::BeginCombo("NES Controller Type", Settings::nesControllerTypeNames[curControllerProfile.nesControllerType].c_str()))
		{
			for (auto const& it : Settings::nesControllerTypeNames)
			{
				if (ImGui::Selectable(it.second.c_str()))
					curControllerProfile.nesControllerType = it.first;
			}

			ImGui::EndCombo();
		}

		// Input controller type
		if (ImGui::BeginCombo("Input Controller Type", Settings::inputControllerTypeNames[curControllerProfile.inputControllerType].c_str()))
		{
			for (auto const& it : Settings::inputControllerTypeNames)
			{
				if (ImGui::Selectable(it.second.c_str()))
					curControllerProfile.inputControllerType = it.first;
			}
			ImGui::EndCombo();
		}

		ImGui::Spacing();

		// Button bindings
		for (auto &it : Settings::controllerButtons[curControllerProfile.nesControllerType])
		{
			ImGui::Text(Settings::buttonNames[it].c_str());
			//ImGui::SameLine();
			std::string buttonText = (curControllerProfile.bindings.count(it)) ? GetButtonName(curControllerProfile.bindings[it]) : "Unassigned";

			ImGui::PushID(it);
			if (ImGui::Button(buttonText.c_str()))
			{
				BindButton(it);
			}
			ImGui::PopID();
		}

		// Save
		if (ImGui::Button("Save"))
		{
			Settings::instance->controllerProfiles[editControllerProfileIndex] = curControllerProfile;
			Settings::instance->Save();
			showEditControllerProfileDialog = false;
		}

		ImGui::End();
	}
}

std::string Renderer::GetButtonName(int button)
{
	if (curControllerProfile.inputControllerType == INPUTTYPE_KEYBOARD)
	{
		return SDL_GetKeyName(button);
	}
	else
	{
		return SDL_GameControllerGetStringForButton((SDL_GameControllerButton)button);
	}
}

void Renderer::BindButton(NESInputs button)
{
	bool buttonBound = false;
	while (!buttonBound)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (curControllerProfile.inputControllerType == INPUTTYPE_KEYBOARD && e.type == SDL_KEYDOWN)
			{
				curControllerProfile.bindings[button] = e.key.keysym.sym;
				buttonBound = true;
			}
			else if (curControllerProfile.inputControllerType == INPUTTYPE_CONTROLLER && e.type == SDL_CONTROLLERBUTTONDOWN)
			{
				curControllerProfile.bindings[button] = e.cbutton.button;
				buttonBound = true;
			}
		}
	}
}

int Renderer::SetPhysicalController()
{
	while (true)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_CONTROLLERBUTTONDOWN)
			{
				return e.cbutton.which;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					return -1;
				}
			}
		}
	}
}
