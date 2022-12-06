#include "../proto_gen/smartknob.pb.h"

#include "serial_protocol_plaintext.h"
#include "display_task.h"
#include "interface_task.h"
#include "led_handler.h"
#include <string>
#include <vector>
#include "music_state.h"

LedHandler ledHandler = LedHandler();


void SerialProtocolPlaintext::handleState(const PB_SmartKnobState& state) {
    bool substantial_change = (latest_state_.current_position != state.current_position)
        || (latest_state_.config.detent_strength_unit != state.config.detent_strength_unit)
        || (latest_state_.config.endstop_strength_unit != state.config.endstop_strength_unit)
        || (latest_state_.config.num_positions != state.config.num_positions);
    latest_state_ = state;

    if (substantial_change) {
        //stream_.printf("STATE: %d/%d  (detent strength: %0.2f, width: %0.0f deg, endstop strength: %0.2f)\n", state.current_position, state.config.num_positions - 1, state.config.detent_strength_unit, degrees(state.config.position_width_radians), state.config.endstop_strength_unit);
        //stream_.printf("!vlm_%c\n", state.current_position);
        byte msg[3];
        msg[0] = 0x01;
        msg[1] = state.current_position;
        msg[2] = 0x0A;
        stream_.write(msg, 3);
        //stream_.printf("!vlm_\n");
    }
}

void SerialProtocolPlaintext::log(const char* msg) {
    stream_.print("LOG: ");
    stream_.println(msg);
}

void SerialProtocolPlaintext::loop() {
    while (stream_.available() > 0) {
        int b = stream_.read();
        stream_.write(b);
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
            stream_.printf("pla_\n");
        } else if (b == 'C') {
            motor_task_.runCalibration();
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
            //uint8_t buf[115200];
            //if(stream_.available() > 0){
            //    int length = stream_.readBytes(musicState.image, 115200);
            //    if (length == 115200){
            //        stream_.write("IMAGE RECIEVED");
            //    }
            //}
        }
        else if (b == '!'){
            //stream_.write("Command detected\n");
            if(stream_.available() > 0){
                String a = stream_.readStringUntil('_');
                if(a == "amb"){
                    //stream_.write("Command amb detected\n");
                    const int BUFFER_SIZE = 3*8;
                    char buf[BUFFER_SIZE];
                    int responseLen = stream_.readBytesUntil('\n', buf, BUFFER_SIZE);
                    //stream_.write(buf);
                }
                else if(a == "tit"){
                    //stream_.write("Command title detected\n");
                    const int BUFFER_SIZE = 20;
                    char buf[BUFFER_SIZE];
                    int responseLen = stream_.readBytesUntil('\n', buf, BUFFER_SIZE);
                    buf[responseLen] = 0;
                    if(responseLen == 20){
                        buf[18] = '.';
                        buf[17] = '.';
                        buf[16] = '.';
                    }
                    //stream_.write(buf);
                    //musicState.title = buf;
                }
            }
        }
    }
}

void SerialProtocolPlaintext::init(DemoConfigChangeCallback cb) {
    demo_config_change_callback_ = cb;
    ledHandler.run();
    stream_.println("SmartKnob starting!\n\nSerial mode: plaintext\nPress 'C' at any time to calibrate.\nPress <Space> to change haptic modes.");
}
