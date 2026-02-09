#ifndef CHERRY_CONTROLLER_H
#define CHERRY_CONTROLLER_H

#include <string>
#include <vector>
#include "serial_port.h"
#include "studio.pb.h"

#define LED_BEHAVIOR_ID         29
#define START_DELIMITER         0xAB
#define END_DELIMITER           0xAD
#define ESCAPE_BYTE             0xAC
#define BEHVAIOR_VALUE_STRING   "Value"
#define BRIGHTNESS_SET_STRING   "Set Brightness"
#define SPEED_SET_STRING        "Set Speed"
#define HUE_SET_STRING          "Set Hue"
#define SATURATION_SET_STRING   "Set Saturation"


class CherryController {
private:
    size_t requestId;
    std::string portName;
    serial_port serialPort;
    int effectStudioIdOffset;
    std::vector<zmk::led_settings::LedEffectNodeInfo> ledEffects;

    size_t NewId() { return requestId++; }
    
    zmk::studio::Response SendRecv(zmk::studio::Request request);
    std::vector<uint8_t> Encode(std::string payload);
    std::vector<uint8_t> Decode(uint8_t rxBuffer[], size_t bytesRead);

public:
    CherryController(const std::string &portName);
    ~CherryController();

    std::string GetLocation() { return portName; }
    int GetEffectStudioIdOffset() { return effectStudioIdOffset; }
    std::vector<zmk::led_settings::LedEffectNodeInfo> GetLedEffects() { return ledEffects; }
    
    void SetEffectStudioIdOffset(int value) { effectStudioIdOffset = value; }
    void AddLedEffects(zmk::led_settings::LedEffectNodeInfo value) { ledEffects.push_back(value); }
    
    zmk::core::GetDeviceInfoResponse GetDeviceInfo();
    zmk::led_settings::LedEffectsNodesInfo GetEffects();
    zmk::led_settings::LedSettingsNodesInfo GetSettings();
    zmk::led_settings::LedSettingsValues GetSettingsValues(uint32_t studioId);
    zmk::behaviors::GetBehaviorDetailsResponse GetBehaviorDetail(uint32_t behaviorId);
    zmk::keymap::PhysicalLayouts GetPhysicalLayouts();
    zmk::keymap::Keymap GetKeymap();

    zmk::led_settings::LedSettingsSetResponse SetSetting(uint32_t studioId, std::vector<zmk::led_settings::Values> params);


    bool UpdateMode(int modeId);
    bool UpdateSpeed(int modeId, int speed);
    bool UpdateBrightness(int modeId, int brightness);
};

#endif