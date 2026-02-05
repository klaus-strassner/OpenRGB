#include "CherryController.h"
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include "studio.pb.h"

const unsigned char header[] { 0xAB };
const unsigned char footer[] { 0xAD };

CherryController::CherryController(const std::string &portName)
: requestId{0}, portName{portName}, serialPort(portName.c_str(), 0)
{
    serialPort.serial_set_dtr(false);
}

CherryController::~CherryController() {
    serialPort.serial_set_dtr(true);
}

size_t CherryController::NewId() {
    return ++requestId;
}

zmk::studio::Response CherryController::SendRecv(zmk::studio::Request request) {
    std::string payload;
    size_t request_id = NewId();
    request.set_request_id(request_id);

    request.SerializeToString(&payload);

    std::vector<uint8_t> txBuffer;
    txBuffer.reserve(payload.size() + 2);
    txBuffer.push_back(0xAB);
    txBuffer.insert(txBuffer.end(), payload.begin(), payload.end());
    txBuffer.push_back(0xAD);

    serialPort.serial_write(reinterpret_cast<char*>(txBuffer.data()), txBuffer.size());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    uint8_t rxBuffer[8192];
    size_t bytesRead = serialPort.serial_read(reinterpret_cast<char*>(rxBuffer), sizeof(rxBuffer));

    // todo ?
    // check request id
    
    zmk::studio::Response response;
    response.ParseFromArray(&rxBuffer[1], bytesRead - 2);

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