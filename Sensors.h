// #include <Arduino.h>

class Sensors {
    public:

    int threshold;
    int sensors_count;
    int *sensor_pins;

    Sensors (int threshold, int sensors_count, int *sensor_pins) {
        this->threshold = threshold;
        this->sensor_pins = sensor_pins;
        this->sensors_count = sensors_count;
    }

    int calculate_error() {
        int error;
        switch (get_line_position()) {
            case 0b00001: error =  4; break;
            case 0b00011: error =  3; break;
            case 0b00010: error =  2; break;
            case 0b00110: error =  1; break;
            case 0b00100: error =  0; break;
            case 0b01100: error = -1; break;
            case 0b01000: error = -2; break;
            case 0b11000: error = -3; break;
            case 0b10000: error = -4; break;
            default:      error =  0; break;
        }
        return error;
    }

    bool on_line(int sensor_pin) {
        int sensor_value = analogRead(sensor_pin);
        return sensor_value < THRESHOLD;
    }

    int get_line_position() {
        int line_position = 0;
         for(int i = 0; i < sensors_count; i++) {
            if(on_line(sensor_pins[i])) {
                line_position += (1 << i);
            }
        }
        return line_position;
    }
    
};
