#include "Settings.h"

Settings* Settings::instance;


std::map<NESControllerType, std::string> Settings::nesControllerTypeNames = {
	{NESCONTROLLER_STANDARD, "Standard Controller"}
};

std::map<InputControllerType, std::string> Settings::inputControllerTypeNames = {
	{INPUTTYPE_KEYBOARD, "Keyboard"},
	{INPUTTYPE_CONTROLLER, "Gamepad"}
};

std::map<NESInputs, std::string> Settings::buttonNames = {
	{DPAD_L, "DPad Left"},
	{DPAD_R, "DPad Right"},
	{DPAD_U, "DPad Up"},
	{DPAD_D, "DPad Down"},
	{BUT_A, "A"},
	{BUT_B, "B"},
	{BUT_START, "Start"},
	{BUT_SELECT, "Select"}
};

std::map<NESControllerType, std::vector<NESInputs>> Settings::controllerButtons = {
	{NESCONTROLLER_STANDARD, std::vector<NESInputs> {
		DPAD_D,
		DPAD_U,
		DPAD_L,
		DPAD_R,
		BUT_A,
		BUT_B,
		BUT_START,
		BUT_SELECT
	}}
};



void Settings::Init()
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
	instance = new Settings();
	instance->Load();
}

void Settings::Load()
{
	std::ifstream input("settings.bin", std::ios::binary);
	if (!input.fail())
	{
		/*json j;

		input >> j;

		for (json &i : j["controllerprofiles"])
		{
			ControllerProfile profile;
			profile.name = i["name"];
			profile.nesControllerType = i["nesControllerType"];
			profile.inputControllerType = i["inputControllerType"];
			profile.bindings = i["bindings"];
			controllerProfiles.push_back(profile);
		}*/

		/* Deserialize settings here */

		cereal::PortableBinaryInputArchive archive(input);
		archive(*Settings::instance);

		SettingsLoaded();
	}
	else
	{
		// Save with default settings
		Save();
	}
}

void Settings::Save()
{
	/*json j;

	j["controllerprofiles"] = json::array();
	for (int i = 0; i < controllerProfiles.size(); i++)
	{
		j["controllerprofiles"].push_back({	{"name", controllerProfiles[i].name },
											{"nesControllerType", controllerProfiles[i].nesControllerType},
											{"inputControllerType", controllerProfiles[i].inputControllerType},
											{"bindings", controllerProfiles[i].bindings} });
	}

	std::string jsonString = j.dump(4);*/
	std::ofstream output("settings.bin", std::ios::binary);
	cereal::PortableBinaryOutputArchive archive(output);
	archive(*Settings::instance);

	output.flush();
	output.close();

	SettingsLoaded();
}

template<class Archive>
void Settings::serialize(Archive& archive)
{
	archive(controllerProfiles, controllerSettings);
}

void Settings::AddSettingsLoadedListener(std::function<void()> callback)
{
	settingsLoadCallbacks.push_back(callback);
}

void Settings::DeleteControllerProfile(int index)
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (controllerSettings[i].controllerProfileIndex > index)
		{
			controllerSettings[i].controllerProfileIndex--;
		}
		else if (controllerSettings[i].controllerProfileIndex == index)
		{
			controllerSettings[i].controllerProfileIndex = -1;
		}
	}

	controllerProfiles.erase(controllerProfiles.begin() + index);

	Save();
}

void Settings::SettingsLoaded()
{
	for (auto callback : settingsLoadCallbacks)
	{
		callback();
	}
}

void Settings::AddNewControllerProfile(ControllerProfile profile)
{
	instance->controllerProfiles.push_back(profile);
}
