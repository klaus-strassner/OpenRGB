#ifndef CHERRY_CONTROLLER_H
#define CHERRY_CONTROLLER_H

#include <string>
#include <vector>
#include "serial_port.h"

extern "C" {
    #include "studio.pb.h"
}

class CherryController {
private:
    std::string portName;
    serial_port serialPort;
    size_t requestId;
    size_t NewId();

public:
    CherryController(const std::string &portName);
    ~CherryController();

    zmk_studio_Response GetDeviceInfo();
};

#endif