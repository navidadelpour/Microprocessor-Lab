// ============================================== SETTINGS /////////////////
const int SENSORS_COUNT = 5;

const int SENSOR_PINS[SENSORS_COUNT] = {A0, A1, A2, A3, A4};

const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;

const int ENB = 10;
const int IN3 = 8;
const int IN4 = 9;

const int THRESHOLD = 512;

const int DELAY_TIME = 0000;

const int MAX_SPEED = 255;
const int NORMAL_SPEED = 200;
const int MIN_SPEED = 100;

const int GOAL_POSITION = 0;

const float KP = 25;
const float KI = 1;
const float KD = 1;

// ============================================== VARIABLES /////////////////

int sensor_values[SENSORS_COUNT];

int line_position = 0;

int current_position;
int speed_difference;
float proportional, last_proportional, derivative, integral;

int speed = NORMAL_SPEED;

int pid() {    
    proportional = GOAL_POSITION - current_position;
    derivative = proportional - last_proportional;
    integral = proportional != 0 ? integral + proportional : 0;

    last_proportional = proportional;

    speed_difference = proportional * KP + integral * KI + derivative * KD;
}

void set_motor_speed() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, speed - speed_difference);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, speed + speed_difference);
}

bool on_line(int sensor_pin) {
    int sensor_value = analogRead(sensor_pin);
    return sensor_value > THRESHOLD ? true : false;
}

void calculate_line_position() {
    line_position = 0;
    for(int i = 0; i < SENSORS_COUNT; i++) {
        if(on_line(SENSOR_PINS[i])) {
            line_position += (0x1 << i);
        }
    }
}

bool race_is_done() {
    return line_position == 0b00000 || line_position == 0b00000;
}


void calculate_current_position() {
    switch (line_position) {
        case 0b00010:
        case 0b00110: current_position = 1; break;

        case 0b00001:
        case 0b00011: current_position = 2; break;

        case 0b00100: current_position = GOAL_POSITION; break;

        case 0b01000:
        case 0b01100: current_position = -1; break;
        
        case 0b10000:
        case 0b11000: current_position = -2; break;
        
        default: current_position = GOAL_POSITION; break;
    }
}

void stop() {
  speed = 0;
}

void make_all_motor_pins_output() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void log() {
    //Serial.print("line_position: ");
    //Serial.println(line_position);

    Serial.print("current position: ");
    Serial.println(current_position);

    Serial.print("proportional: ");
    Serial.println(proportional);
    
    Serial.print("derivative: ");
    Serial.println(derivative);
    
    Serial.print("integral: ");
    Serial.println(integral);
    
    Serial.print("speed_difference: ");
    Serial.println(speed_difference);
    
    Serial.println("--------------------------------------");
}

void setup() {
    Serial.begin(9600);
    make_all_motor_pins_output();
}

void loop() {
     calculate_line_position();
     if(race_is_done() && false) {
         stop();
     } else {
         calculate_current_position();
         log();
         delay(DELAY_TIME);
         pid();
         set_motor_speed();
     }    
}
