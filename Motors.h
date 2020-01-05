#include <Arduino.h>

class Motors {
    public:

    int normal_speed;
    int min_speed;
    int max_speed;

    int left_in1;
    int left_in2;
    int left_en;
    
    int right_in1;
    int right_in2;
    int right_en;

    Motors (int normal_speed, int min_speed, int max_speed, int left_in1, int left_in2, int left_en, int right_in1, int right_in2, int right_en) {
        this->normal_speed = normal_speed;
        this->min_speed = min_speed;
        this->max_speed = max_speed;

        this->left_in1 = left_in1;
        this->left_in2 = left_in2;
        this->left_en = left_en;
        
        this->right_in1 = right_in1;
        this->right_in2 = right_in2;
        this->right_en = right_en;
    }

    void initialize() {
        pinMode(left_en, OUTPUT);
        pinMode(left_in1, OUTPUT);
        pinMode(left_in2, OUTPUT);

        pinMode(right_en, OUTPUT);
        pinMode(right_in1, OUTPUT);
        pinMode(right_in2, OUTPUT);

        digitalWrite(left_in1, LOW);
        digitalWrite(left_in2, HIGH);

        digitalWrite(right_in1, HIGH);
        digitalWrite(right_in2, LOW);
    }

    void set_motor(char *motor, char *direction, int speed) {
        if(motor == "left") {
            if(direction == "forward") {
                digitalWrite(left_in1, LOW);
                digitalWrite(left_in2, HIGH);
            } else if (direction == "backward") {
                digitalWrite(left_in1, HIGH);
                digitalWrite(left_in2, LOW);
            }
            analogWrite(left_en, speed);
        } else if(motor == "right") {
            if(direction == "forward") {
                digitalWrite(right_in1, HIGH);
                digitalWrite(right_in2, LOW);
            } else if (direction == "backward") {
                digitalWrite(right_in1, LOW);
                digitalWrite(right_in2, HIGH);
            }
            analogWrite(right_en, speed);
        }
    }

    void drive(int speed_difference, bool debug) {
        int left_speed = normal_speed + speed_difference;
        int right_speed = normal_speed - speed_difference;

        if(!debug) {
            if(left_speed > 0) {
                set_motor("left", "forward", constrain(left_speed, min_speed, max_speed));
            } else {
                set_motor("left", "backward", constrain(-left_speed, min_speed, max_speed));
            }

            if(right_speed > 0) {
                set_motor("right", "forward", constrain(right_speed, min_speed, max_speed));
            } else {
                set_motor("right", "backward", constrain(-right_speed, min_speed, max_speed));
            }
        }

        Serial.print("left_speed: ");
        Serial.println(left_speed);

        Serial.print("right_speed: ");
        Serial.println(right_speed);
    }

};
