
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

    enum Direction {FORWARD, BACKWARD};
    enum Motor {LEFT, RIGHT};

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
    }

    void set_motor(Motor motor, Direction direction, int speed) {
        if(motor == LEFT) {
            if(direction == FORWARD) {
                digitalWrite(left_in1, LOW);
                digitalWrite(left_in2, HIGH);
            } else if (direction == BACKWARD) {
                digitalWrite(left_in1, HIGH);
                digitalWrite(left_in2, LOW);
            }
            analogWrite(left_en, speed);
        } else if(motor == RIGHT) {
            if(direction == FORWARD) {
                digitalWrite(right_in1, HIGH);
                digitalWrite(right_in2, LOW);
            } else if (direction == BACKWARD) {
                digitalWrite(right_in1, LOW);
                digitalWrite(right_in2, HIGH);
            }
            analogWrite(right_en, speed);
        }
    }

    Direction get_direction(int speed) {
        return speed > 0 ? FORWARD : BACKWARD;
    }

    int normalize_speed(int speed) {
        return constrain(abs(speed), min_speed, max_speed);
    }

    void drive(int speed_difference, bool debug) {
        int left_speed = normal_speed + speed_difference;
        int right_speed = normal_speed - speed_difference;

        if(!debug) {
            set_motor(LEFT, get_direction(left_speed), normalize_speed(left_speed));
            set_motor(RIGHT, get_direction(right_speed), normalize_speed(right_speed));
        }

        Serial.print("left_speed: ");
        Serial.print(normalize_speed(left_speed));
        Serial.print(" - ");
        Serial.println(get_direction(left_speed) == FORWARD ? "F" : "B");

        Serial.print("right_speed: ");
        Serial.print(normalize_speed(right_speed));
        Serial.print(" - ");
        Serial.println(get_direction(right_speed) == FORWARD ? "F" : "B");
    }

};
