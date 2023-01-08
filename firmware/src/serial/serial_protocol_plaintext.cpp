#include "../proto_gen/smartknob.pb.h"

#include "serial_protocol_plaintext.h"
#include "display_task.h"
#include "interface_task.h"
#include "led_handler.h"
#include <string>
#include <vector>
#include "config_state.h"
#include <esp_task_wdt.h>

LedHandler ledHandler = LedHandler();

static bool LAST_VLM_CHANGE_BY_PC = false;


void SerialProtocolPlaintext::handleState(const PB_SmartKnobState& state) {
    bool substantial_change = (latest_state_.current_position != state.current_position)
        || (latest_state_.config.detent_strength_unit != state.config.detent_strength_unit)
        || (latest_state_.config.endstop_strength_unit != state.config.endstop_strength_unit)
        || (latest_state_.config.num_positions != state.config.num_positions);
    latest_state_ = state;

    if (substantial_change) {
        //vlm
        if(!LAST_VLM_CHANGE_BY_PC){
            byte msg[3];
            msg[0] = 0x01;
            msg[1] = state.current_position;
            stream_.write(msg, 2);
        }
        LAST_VLM_CHANGE_BY_PC = false;
    }
}

void SerialProtocolPlaintext::log(const char* msg) {
    stream_.print("LOG: ");
    stream_.println(msg);
}

void SerialProtocolPlaintext::loop() {
    while (stream_.available() > 0) {
        int b = stream_.read();
        //stream_.write(b);
        if (b == 0) {
            if (protocol_change_callback_) {
                protocol_change_callback_(SERIAL_PROTOCOL_PROTO);
            }
            break;
        }
        if (b == ' ') {
            if (demo_config_change_callback_) {
                demo_config_change_callback_();
            }
        } else if (b == 'C') {
            #if SK_MOTOR
                motor_task_.runCalibration();
            #endif
        }
        else if (b == 0x02){
            uint8_t buf[24];
            if(stream_.available() > 0){
                int length = stream_.readBytes(buf, 24);
                if (length == 24){
                    ledHandler.changeLed(buf);
                }
            }
        }
        else if (b == 0x03){
            uint8_t buf[288];
            configState.imageLoading = true;
            esp_task_wdt_reset();
            vTaskDelay(1);
            for(int count = 0; count < 115200; count = count + 288){
                esp_task_wdt_reset();
                vTaskDelay(1);
                if(stream_.available() > 0){
                    int length = stream_.readBytes(buf, 288);
                    if (length == 288){
                        #if SK_DISPLAY
                        memcpy(&configState.image[count], buf, 288);
                        #endif
                    }
                    else {
                        stream_.write("WRONG PIXEL COUNT: ");
                        stream_.printf("%d", length);

                        #if SK_DISPLAY
                        memcpy(&configState.image[count], buf, length);
                        #endif
                        
                        int missing_count = 288 - length;
                        uint8_t buf_missing[missing_count];
                        if(stream_.available() > 0) {
                            int length_missing_read = stream_.readBytes(buf, 288);
                            if (length_missing_read == missing_count) {
                                #if SK_DISPLAY
                                memcpy(&configState.image[count + length], buf_missing, missing_count);
                                #endif
                            }
                            else {
                                memset(buf_missing, 0, missing_count);
                                #if SK_DISPLAY
                                memcpy(&configState.image[count + length], buf_missing, missing_count);
                                #endif
                            }
                        }
                    }
                }
                esp_task_wdt_reset();
                vTaskDelay(1);
            }
            configState.imageLoading = false;
        }
        else if (b == 0x04){
            if(stream_.available() > 0){
                    const int BUFFER_SIZE = 20;
                    char buf[BUFFER_SIZE];
                    int responseLen = stream_.readBytesUntil('\n', buf, BUFFER_SIZE);
                    buf[responseLen] = 0;
                    if(responseLen == 20){
                        buf[18] = '.';
                        buf[17] = '.';
                        buf[16] = '.';
                    }
                    #if SK_DISPLAY
                    configState.title = buf;
                    #endif
            }
        }
        else if (b == 0x06) {
            if(stream_.available() > 0) {
                char buf[0];
                stream_.readBytes(buf, 1);
                ledHandler.changeBrightness((uint8_t) buf[0]);
                //stream_.write(buf[0]);
            }
        }
        else if (b == 0x05) {
            if(stream_.available() > 0) {
                char buf[0];
                stream_.readBytes(buf, 1);
                motor_task_.setValue((int)buf[0]);
                LAST_VLM_CHANGE_BY_PC = true;
                //stream_.write(buf[0]);
            }
        }
    }
}

void SerialProtocolPlaintext::init(DemoConfigChangeCallback cb) {
    demo_config_change_callback_ = cb;
    ledHandler.run();
    stream_.println("SmartKnob starting!\n\nSerial mode: plaintext\nPress 'C' at any time to calibrate.\nPress <Space> to change haptic modes.");
}
