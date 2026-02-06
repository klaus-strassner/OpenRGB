#ifndef CHERRY_CONTROLLER_H
#define CHERRY_CONTROLLER_H

#include <string>
#include <vector>
#include "serial_port.h"
#include "studio.pb.h"

#define LED_BEHAVIOR_ID         29
#define START_DELIMITER         0xAB
#define END_DELIMITER           0xAD
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
    zmk::studio::Response SendRecv(zmk::studio::Request request);
    size_t NewId();

public:
    CherryController(const std::string &portName);
    ~CherryController();
    std::string GetLocation();

    zmk::core::GetDeviceInfoResponse GetDeviceInfo();
    zmk::led_settings::LedEffectsNodesInfo GetEffects();
    zmk::led_settings::LedSettingsNodesInfo GetSettings();
    zmk::led_settings::LedSettingsValues GetSettingsValues(uint32_t studioId);
    zmk::behaviors::GetBehaviorDetailsResponse GetBehaviorDetail(uint32_t behaviorId);
    zmk::keymap::PhysicalLayouts GetPhysicalLayouts();
    zmk::keymap::Keymap GetKeymap();
};

#endif