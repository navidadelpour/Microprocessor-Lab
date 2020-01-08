class Sensors {
    public:

    int threshold;
    int sensors_count;
    int *sensor_pins;
    int *errors;
    int previous_error;

    Sensors (int threshold, int sensors_count, int *sensor_pins, int *errors) {
        this->threshold = threshold;
        this->sensor_pins = sensor_pins;
        this->sensors_count = sensors_count;
        this->errors = errors;
    }

    int calculate_error() {
        int error;
        switch (get_line_position()) {
            case 0b01111:
            case 0b00111:
            case 0b00011: error =  errors[3]; break;

            case 0b00001: error =  errors[2]; break;
            
            case 0b00110:
            case 0b00010: error =  errors[1]; break;

            case 0b11111:
            case 0b00100: error =  errors[0]; break;

            case 0b01100:
            case 0b01000: error = -errors[1]; break;

            case 0b10000: error = -errors[2]; break;
            
            case 0b11110:
            case 0b11100:
            case 0b11000: error = -errors[3]; break;
            
            default:      error =  previous_error; break;
        }
        previous_error = error;
        return error;
    }

    bool on_line(int sensor_pin) {
        int sensor_value = analogRead(sensor_pin);
        return sensor_value < threshold;
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
