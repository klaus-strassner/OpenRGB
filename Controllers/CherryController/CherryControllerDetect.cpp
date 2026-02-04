#include "Detector.h"
#include "RGBController.h"
#include "find_usb_serial_port.h"
#include "CherryController.h"
#include <iostream>

extern "C" {
    #include "studio.pb.h"
}


#define CHERRY_VID      0x046A
#define FAVONIUS_PID   0xFFFD

void DetectCherryController() {
    std::vector<std::string *> ports = find_usb_serial_port(CHERRY_VID, FAVONIUS_PID);

    for(size_t device = 0; device < ports.size(); ++device) {
        CherryController *controller = new CherryController(*ports[device]);

        zmk_studio_Response response = controller->GetDeviceInfo();

        if(response.type.request_response.subsystem.core.which_response_type == zmk_core_Response_get_device_info_tag &&
            response.which_type == zmk_studio_Response_request_response_tag &&
            response.type.request_response.which_subsystem == zmk_studio_RequestResponse_core_tag &&
            response.type.request_response.subsystem.core.which_response_type == zmk_core_Response_get_device_info_tag) {
            std::cout << "success!" << std::endl;
        }
        else {
            delete controller;
        }

        delete ports[device];
    }
}

REGISTER_DETECTOR("Cherry Controller", DetectCherryController);