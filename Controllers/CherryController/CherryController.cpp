#include "CherryController.h"
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include "studio.pb.h"
#include <iomanip>

CherryController::CherryController(const std::string &portName)
: requestId{0}, portName{portName}, serialPort(portName.c_str(), 0)
{
    serialPort.serial_set_dtr(false);
}

CherryController::~CherryController() {
    serialPort.serial_set_dtr(true);
}

std::vector<uint8_t> CherryController::Encode(std::string payload) {
    std::vector<uint8_t> txBuffer;
    txBuffer.reserve(payload.size() + 2);

    txBuffer.push_back(START_DELIMITER);

    // insert escape byte
    for(const uint8_t byte : payload) {
        if(byte == START_DELIMITER || byte == ESCAPE_BYTE || byte == END_DELIMITER) {
            txBuffer.push_back(ESCAPE_BYTE);
            txBuffer.push_back(byte);
        }
        else {
            txBuffer.push_back(byte);
        }
    }

    txBuffer.push_back(END_DELIMITER);

    return txBuffer;
}

std::vector<uint8_t> CherryController::Decode(uint8_t rxBuffer[], size_t bytesRead) {
    std::vector<uint8_t> result;
    result.reserve(bytesRead - 2);

    // check for escape byte
    for(size_t byte = 1; byte < bytesRead - 1; byte++) {
        if(rxBuffer[byte] == ESCAPE_BYTE) {
            result.push_back(rxBuffer[++byte]);
        }
        else {
            result.push_back(rxBuffer[byte]);
        }
    }

    return result;
}

zmk::studio::Response CherryController::SendRecv(zmk::studio::Request request) {
    std::string payload;
    size_t request_id = NewId();

    request.set_request_id(request_id);
    request.SerializeToString(&payload);
    auto txBuffer = Encode(payload);

    serialPort.serial_flush_tx();
    serialPort.serial_write(reinterpret_cast<char*>(txBuffer.data()), txBuffer.size());

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    size_t bytesRead = 0;
    uint8_t rxBuffer[4096];
    zmk::studio::Response response;

    bytesRead = serialPort.serial_read(reinterpret_cast<char*>(rxBuffer), sizeof(rxBuffer));

    // bool read = true;
    // while(read) {
    //     bytesRead += serialPort.serial_read(reinterpret_cast<char*>(rxBuffer + bytesRead), sizeof(rxBuffer) - bytesRead);
    //     if(bytesRead > 0) 
    //         if(rxBuffer[bytesRead - 1] == 0xAD)
    //             read = false;
    // }
    
    std::cout << "Bytes Read: " << std::dec << bytesRead << std::endl;

    for (size_t i = 0; i < bytesRead; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
            << static_cast<int>(static_cast<unsigned char>(rxBuffer[i])) << " ";
    }
    std::cout << std::endl;
    
    std::vector<uint8_t> result = Decode(rxBuffer, bytesRead);
    
    if(response.ParseFromArray(result.data(), result.size())) {
        std::cout << "success parsing" << std::endl;
    }
    else {
        std::cout << "error parsing " << response.InitializationErrorString() << std::endl;
    }
    
    // todo ?
    // check request id

    // response.PrintDebugString();

    return response;
}

zmk::core::GetDeviceInfoResponse CherryController::GetDeviceInfo() {
    zmk::studio::Request request;
    request.mutable_core()
        ->set_get_device_info(true);

    zmk::studio::Response response = SendRecv(request);
    return response.request_response()
        .core()
        .get_device_info();
}

zmk::led_settings::LedEffectsNodesInfo CherryController::GetEffects() {
    zmk::studio::Request request;
    request.mutable_led_settings()
        ->mutable_led_settings_get_info()
        ->set_effects(true);

    zmk::studio::Response response = SendRecv(request);
    return response.request_response()
        .led_settings()
        .led_settings_get_info()
        .effects_data();
}

zmk::led_settings::LedSettingsNodesInfo CherryController::GetSettings() {
    zmk::studio::Request request;
    request.mutable_led_settings()
        ->mutable_led_settings_get_info()
        ->set_settings(true);

    zmk::studio::Response response = SendRecv(request);
    return response.request_response()
        .led_settings()
        .led_settings_get_info()
        .settings_data();
}

zmk::led_settings::LedSettingsValues CherryController::GetSettingsValues(uint32_t studioId) {
    zmk::studio::Request request;
    request.mutable_led_settings()
        ->mutable_led_settings_get_values()
        ->set_settings_id(studioId);

    zmk::studio::Response response = SendRecv(request);
    return response.request_response()
        .led_settings()
        .led_settings_get_values()
        .values();
}

zmk::behaviors::GetBehaviorDetailsResponse CherryController::GetBehaviorDetail(uint32_t behaviorId) {
    zmk::studio::Request request;
    request.mutable_behaviors()
        ->mutable_get_behavior_details()
        ->set_behavior_id(behaviorId);

    zmk::studio::Response response = SendRecv(request);
    return response.request_response()
        .behaviors()
        .get_behavior_details();
}

zmk::keymap::PhysicalLayouts CherryController::GetPhysicalLayouts() {
    zmk::studio::Request request;
    request.mutable_keymap()
        ->set_get_physical_layouts(true);

    zmk::studio::Response response = SendRecv(request);
    return response.request_response()
        .keymap()
        .get_physical_layouts();
}


zmk::keymap::Keymap CherryController::GetKeymap() {
    zmk::studio::Request request;
    request.mutable_keymap()
        ->set_get_keymap(true);

    zmk::studio::Response response = SendRecv(request);
    return response.request_response()
        .keymap()
        .get_keymap();
}

zmk::led_settings::LedSettingsSetResponse CherryController::SetSetting(uint32_t studioId, std::vector<zmk::led_settings::Values> params) {
    zmk::studio::Request request;
    request.mutable_led_settings()
        ->mutable_led_settings_set()
        ->set_settings_id(studioId);
    
    for(const auto& param : params) {
        *request.mutable_led_settings()
            ->mutable_led_settings_set()
            ->add_values() = param;
    }

    zmk::studio::Response response = SendRecv(request);
    return response.request_response()
        .led_settings()
        .led_settings_set();
}

bool CherryController::UpdateMode(int modeId) {
    std::vector<zmk::led_settings::Values> params;
    zmk::led_settings::Values param;

    param.mutable_selected_effect()
        ->set_change_type(zmk::led_settings::ChangeTypeInt::SET_VALUE);
    param.mutable_selected_effect()
        ->set_value(modeId);
    params.push_back(param);

    SetSetting(2, params);

    return true;
}

bool CherryController::UpdateSpeed(int modeId, int speed) {
    std::vector<zmk::led_settings::Values> params;
    zmk::led_settings::Values param;

    param.mutable_speed_factor()
        ->set_change_type(zmk::led_settings::ChangeTypeInt::SET_VALUE);
    param.mutable_speed_factor()
        ->set_value(speed);
    params.push_back(param);

        for(const auto& effect : ledEffects) {
            if(effect.studio_id() == (uint32_t)modeId + effectStudioIdOffset) {
                SetSetting(effect.settings_studio_id(), params);
        }
    }

    return true;
}

bool CherryController::UpdateBrightness(int modeId, int brightness) {
    std::vector<zmk::led_settings::Values> params;
    zmk::led_settings::Values param;

    param.mutable_brightness()
        ->set_change_type(zmk::led_settings::ChangeTypeInt::SET_VALUE);
    param.mutable_brightness()
        ->set_value(brightness);
    params.push_back(param);

        for(const auto& effect : ledEffects) {
            if(effect.studio_id() == (uint32_t)modeId + effectStudioIdOffset) {
                SetSetting(effect.settings_studio_id(), params);
        }
    }

    return true;
}