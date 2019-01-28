#pragma once

#include <vector>
#include <map>
#include <fstream>
#include <functional>
#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>


#define MAX_CONTROLLERS 2


class InputManager;

enum NESControllerType
{
	NESCONTROLLER_STANDARD = 0
};

enum InputControllerType
{
	INPUTTYPE_KEYBOARD = 0,
	INPUTTYPE_CONTROLLER = 1
};

enum NESInputs
{
	DPAD_L,
	DPAD_R,
	DPAD_U,
	DPAD_D,

	BUT_A,
	BUT_B,
	BUT_START,
	BUT_SELECT
};

struct ControllerProfile
{
	std::string name;

	NESControllerType nesControllerType;
	InputControllerType inputControllerType;

	std::map<NESInputs, int> bindings;

	template<class Archive>
	void serialize(Archive & archive)
	{
		// serialize things by passing them to the archive
		archive(name, nesControllerType, inputControllerType, bindings);
	}
};

struct ControllerSettings
{
	int controllerProfileIndex = -1;
	int physicalControllerIndex = 0;

	template<class Archive>
	void serialize(Archive & archive)
	{
		// serialize things by passing them to the archive
		archive(controllerProfileIndex, physicalControllerIndex);
	}
};



class Settings
{
public:
	static void Init();

	void Save();
	void Load();

	template<class Archive>
	void serialize(Archive& archive);

	void AddSettingsLoadedListener(std::function<void()> callback);

	void AddNewControllerProfile(ControllerProfile profile);
	void DeleteControllerProfile(int index);

	static Settings* instance;

	static std::map<NESControllerType, std::string> nesControllerTypeNames;
	static std::map<InputControllerType, std::string> inputControllerTypeNames;

	static std::map<NESInputs, std::string> buttonNames;
	static std::map<NESControllerType, std::vector<NESInputs>> controllerButtons;

	std::vector<ControllerProfile> controllerProfiles;
	ControllerSettings controllerSettings[MAX_CONTROLLERS];

private:
	void SettingsLoaded();

	std::vector<std::function<void()>> settingsLoadCallbacks;
};

