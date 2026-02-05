#ifndef CHERRY_CONTROLLER_H
#define CHERRY_CONTROLLER_H

#include <string>
#include <vector>
#include "serial_port.h"
#include "studio.pb.h"

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
    zmk::led_settings::LedSettingsNodeInfo GetSettings();
};

#endif