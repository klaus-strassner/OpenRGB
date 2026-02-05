#ifndef CHERRY_CONTROLLER_H
#define CHERRY_CONTROLLER_H

#include <string>
#include <vector>
#include "serial_port.h"
#include "studio.pb.h"

#define LED_BEHAVIOR_ID         29
#define BEHVAIOR_VALUE_STRING   "Value"
#define BRIGHTNESS_SET_STRING   "Set Brightness"
#define SPEED_SET_STRING        "Set Speed"
#define HUE_SET_STRING          "Set Hue"
#define SATURATION_SET_STRING   "Set Saturation"


class CherryController {
private:
    std::string portName;
    serial_port serialPort;
    size_t requestId;
    zmk::studio::Response SendRecv(zmk::studio::Request request);
    size_t NewId();

public:
    CherryController(const std::string &portName);
    ~CherryController();

    zmk::core::GetDeviceInfoResponse GetDeviceInfo();
    zmk::led_settings::LedEffectsNodesInfo GetEffects();
    zmk::led_settings::LedSettingsNodesInfo GetSettings();
    zmk::led_settings::LedSettingsValues GetSettingsValues(uint32_t studioId);
    zmk::behaviors::GetBehaviorDetailsResponse GetBehaviorDetail(uint32_t behaviorId);
};

#endif