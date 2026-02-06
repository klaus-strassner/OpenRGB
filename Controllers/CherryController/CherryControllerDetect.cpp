#include "Detector.h"
#include "RGBController.h"
#include "find_usb_serial_port.h"
#include "CherryController.h"
#include "RGBController_Cherry.h"
#include <iostream>
#include "studio.pb.h"

#define CHERRY_VID      0x046A
#define FAVONIUS_PID    0xFFFD


void DetectCherryController() {
    std::vector<std::string *> ports = find_usb_serial_port(CHERRY_VID, FAVONIUS_PID);

    for(size_t device = 0; device < ports.size(); ++device) {
        CherryController *controller = new CherryController(*ports[device]);

        zmk::core::GetDeviceInfoResponse response = controller->GetDeviceInfo();

        if (response.serial_number().empty()) {
            std::cout << "Device detected on " << *ports[device] 
                << " but failed to respond to GetDeviceInfo." << std::endl;
            delete controller;
            delete ports[device];
            continue;
        }
        std::cout << "device detected on " << *ports[device] << ", name: " << response.name() << ", serial number: ";
        for (unsigned char c : response.serial_number()) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        std::cout << std::endl;
    
        // auto physicalLayout = controller->GetPhysicalLayouts().layouts(0);
        // std::cout << std::dec << "keys: " << physicalLayout.keys_size() << std::endl;

        RGBController_Cherry *rgbController = new RGBController_Cherry(controller);
        ResourceManager::get()->RegisterRGBController(rgbController);

        delete ports[device];

        controller->~CherryController();
        rgbController->~RGBController_Cherry();
    }
}

REGISTER_DETECTOR("Cherry Favonius Keyboard", DetectCherryController);