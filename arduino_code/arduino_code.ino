// Pin assignments for IR sensors
const int sensor_1_pin = 2; // Sensor 1 input pin
const int sensor_2_pin = 3; // Sensor 2 input pin

// Variables to store people count
int people_count = 0;

// Variables to track sensor states
bool sensor_1_state = false;
bool sensor_2_state = false;

// Previous state variables to debounce the sensors
bool prevsensor_1_state = false;
bool prevsensor_2_state = false;

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
    sensor_1_state = digitalRead(sensor_1_pin);
    sensor_2_state = digitalRead(sensor_2_pin);

    // Handle system state transitions
    switch (currentState)
    {
    case IDLE:
        if (sensor_1_state && !prevsensor_1_state)
        {
            currentState = ENTRY_IN_PROGRESS;
            stateStartTime = millis(); // Record the start time of the state
        }
        else if (sensor_2_state && !prevsensor_2_state)
        {
            currentState = EXIT_IN_PROGRESS;
            stateStartTime = millis(); // Record the start time of the state
        }
        break;

    case ENTRY_IN_PROGRESS:
        if (sensor_2_state && !prevsensor_2_state)
        {
            people_count++;
            // Print current people count
            Serial.print("People Count: ");
            Serial.println(people_count);
            Serial.println("Someone entered the room");
            currentState = IDLE;
        }
        else if (millis() - stateStartTime >= RESET_TIME_THRESHOLD)
        { // If 5 seconds have passed
            currentState = RESET;
        }
        break;

    case EXIT_IN_PROGRESS:
        if (sensor_1_state && !prevsensor_1_state)
        {
            people_count--;
            // Print current people count
            Serial.print("People Count: ");
            Serial.println(people_count);
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
    prevsensor_1_state = sensor_1_state;
    prevsensor_2_state = sensor_2_state;
}