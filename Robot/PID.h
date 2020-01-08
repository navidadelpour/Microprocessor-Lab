class PID {
    public:

    float KP;
    float KI;
    float KD;

    float last_proportional, integral;

    PID (float KP, float KI, float KD) {
        this->KP = KP;
        this->KD = KD;
        this->KI = KI;
    }

    int calculate_speed_difference(int proportional) {
        float derivative = proportional - last_proportional;
        integral = integral + proportional;
        last_proportional = proportional;

        return proportional * KP + integral * KI + derivative * KD;
    }


};
