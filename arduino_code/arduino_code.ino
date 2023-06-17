// Pin assignments for IR sensors
const int sensor_1_pin = 2; // Sensor 1 input pin
const int sensor_2_pin = 3; // Sensor 2 input pin

// Variables to store people count
int peopleCount = 0;

// Variables to track sensor states
bool sensor1State = false;
bool sensor2State = false;

// Previous state variables to debounce the sensors
bool prevSensor1State = false;
bool prevSensor2State = false;

const int RESET_TIME_THRESHOLD = 5000;
const int READ_SENSOR_TIME_INTERVAL = 100;
unsigned long read_sensor_time = 0;

// State variable to track system state
enum State
{
    IDLE,
    ENTRY_IN_PROGRESS,
    EXIT_IN_PROGRESS,
    RESET
};
State currentState = IDLE;

unsigned long stateStartTime = 0; // Variable to store the time when a state started
void people_count(void);
void setup()
{
    Serial.begin(9600);

    pinMode(sensor_1_pin, INPUT);
    pinMode(sensor_2_pin, INPUT);
}

void loop()
{
    if(millis() - read_sensor_time >= READ_SENSOR_TIME_INTERVAL)
    {
        read_sensor_time = millis();
        people_count();
    }
}

void people_count(void)
{
    // Read sensor states
    sensor1State = digitalRead(sensor_1_pin);
    sensor2State = digitalRead(sensor_2_pin);

    // Handle system state transitions
    switch (currentState)
    {
    case IDLE:
        if (sensor1State && !prevSensor1State)
        {
            currentState = ENTRY_IN_PROGRESS;
            stateStartTime = millis(); // Record the start time of the state
        }
        else if (sensor2State && !prevSensor2State)
        {
            currentState = EXIT_IN_PROGRESS;
            stateStartTime = millis(); // Record the start time of the state
        }
        break;

    case ENTRY_IN_PROGRESS:
        if (sensor2State && !prevSensor2State)
        {
            peopleCount++;
            // Print current people count
            Serial.print("People Count: ");
            Serial.println(peopleCount);
            Serial.println("Someone entered the room");
            currentState = IDLE;
        }
        else if (millis() - stateStartTime >= RESET_TIME_THRESHOLD)
        { // If 5 seconds have passed
            currentState = RESET;
        }
        break;

    case EXIT_IN_PROGRESS:
        if (sensor1State && !prevSensor1State)
        {
            peopleCount--;
            // Print current people count
            Serial.print("People Count: ");
            Serial.println(peopleCount);
            Serial.println("Someone exited the room");
            currentState = IDLE;
        }
        else if (millis() - stateStartTime >= RESET_TIME_THRESHOLD)
        { // If 5 seconds have passed
            currentState = RESET;
        }
        break;

    case RESET:
        Serial.println("Resetting state...");
        currentState = IDLE;
        break;
    }

    // Update previous sensor states
    prevSensor1State = sensor1State;
    prevSensor2State = sensor2State;
}