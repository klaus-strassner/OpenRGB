#include "RGBController_Cherry.h"

RGBController_Cherry::RGBController_Cherry(CherryController* controller)
: controller{controller}
{
    name = "Cherry Favonius Keyboard";
    vendor = "Cherry";
    type = DEVICE_TYPE_KEYBOARD;
    description = "Cherry Keyboard";


    modes.push_back([]{
        mode m;

        return m;
    }());
}

RGBController_Cherry::~RGBController_Cherry() {
    delete controller;
}

void RGBController_Cherry::SetupZones() {
    zone newZone;

    newZone.name = "Keyboard";
}

void RGBController_Cherry::ResizeZone(int zone, int new_size) {

}

void RGBController_Cherry::DeviceUpdateLEDs() {

}

void RGBController_Cherry::UpdateZoneLEDs(int zone) {

}

void RGBController_Cherry::UpdateSingleLED(int led) {

}

void RGBController_Cherry::DeviceUpdateMode() {

}
