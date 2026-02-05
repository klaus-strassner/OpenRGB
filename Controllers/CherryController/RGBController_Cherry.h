#ifndef RGBCONTROLLER_CHERRY_H
#define RGBCONTROLLER_CHERRY_H

#include "RGBController.h"
#include "CherryController.h"

class RGBController_Cherry : public RGBController {
private:
    CherryController* controller;

public:
    RGBController_Cherry(CherryController* controller);
    ~RGBController_Cherry();

    void SetupZones();
    void ResizeZone(int zone, int new_size);
    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int zone);
    void UpdateSingleLED(int led);
    void DeviceUpdateMode();
};

#endif