#include "CherryController.h"
#include <thread>
#include <iostream>
#include <iomanip>

extern "C" {
    #include "pb_encode.h"
    #include "pb_decode.h"
    #include "studio.pb.h"
}

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

zmk_studio_Response CherryController::GetDeviceInfo() {
    zmk_studio_Request request = zmk_studio_Request_init_zero;

    uint8_t txBuffer[64];
    pb_ostream_t txStream = pb_ostream_from_buffer(&txBuffer[1], sizeof(txBuffer) - 2);
    txBuffer[0] = 0xAB;
    
    size_t request_id = NewId();
    request.request_id = request_id;
    request.which_subsystem = zmk_studio_Request_core_tag;
    request.subsystem.core.which_request_type = zmk_core_Request_get_device_info_tag;

    int count;
    if(pb_encode(&txStream, zmk_studio_Request_fields, &request)) {
        txBuffer[txStream.bytes_written + 1] = 0xAD;
        count = serialPort.serial_write(reinterpret_cast<char*>(txBuffer), txStream.bytes_written + 2);
    }

    // test output
    // expect:  AB 08 01 1A 02 08 01 AD
    // get:     AB 08 01 1A 02 08 00 AD
    // for (size_t i = 0; i < count; i++) {
    //     std::cout << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
    //         << (int)txBuffer[i] << " ";
    // }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    zmk_studio_Response response = zmk_studio_Response_init_zero;

    uint8_t rxBuffer[64];
    size_t bytesRead = serialPort.serial_read(reinterpret_cast<char*>(rxBuffer), sizeof(rxBuffer));
    pb_istream_t rxStream = pb_istream_from_buffer(&rxBuffer[1], bytesRead - 2);

    // test input
    // expect:  AB 0A 1A 08 01 1A 16 0A 14 0A 00 12 10 57 69 FF FC 5D 7D F4 A9 67 A6 F3 97 C2 23 98 9A AD
    // get:     AB 0A 1A 08 01 1A 16 0A 14 0A 00 12 10 57 69 FF FC 5D 7D F4 A9 67 A6 F3 97 C2 23 98 9A AD
    // for (size_t i = 0; i < bytesRead; i++) {
    //     std::cout << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
    //         << (int)rxBuffer[i] << " ";
    // }

    pb_decode(&rxStream, zmk_studio_Response_fields, &response);

    return response;
}


size_t CherryController::NewId() {
    return ++requestId;
}