
// settings
#define DETECTED LOW
#define DEBUG 0
#define RESET_TIME_THRESHOLD 1000
#define READ_SENSOR_TIME_INTERVAL 10
#define PULSE_DURATION 200

// Pin assignments for IR sensors
const int sensor_1_pin = 4; // Sensor 1 input pin
const int sensor_2_pin = 5; // Sensor 2 input pin
const int led1Pin = 16;     // LED 1 pin
const int led2Pin = 17;     // LED 2 pin
// Variables to store people count
int people_count = 0;

// Variables to track sensor states
bool sensor_1_state = !DETECTED;
bool sensor_2_state = !DETECTED;

// Previous state variables to debounce the sensors
bool prevsensor_1_state = !DETECTED;
bool prevsensor_2_state = !DETECTED;

unsigned long read_sensor_time = 0;

// Variables for LED pulsing
bool ledPulseInProgress = false;
unsigned long ledPulseStartTime = 0;
unsigned long ledPulseDuration = 0;
byte ledPulsePin = 0;

#if DEBUG
#define PRINT(x) Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#else
#define PRINT(x)
#define PRINTLN(x)
#endif
#define PRINTLN_IMPORTANT(x) Serial.println(x)

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
void counter_function(void);
void updateLEDPulse();
void pulseLED(byte ledPin, unsigned long duration);
void setup()
{
    Serial.begin(9600);

    pinMode(sensor_1_pin, INPUT_PULLUP);
    pinMode(sensor_2_pin, INPUT_PULLUP);
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    // digitalWrite(led1Pin, HIGH); // test
    // digitalWrite(led2Pin, HIGH); // test
}

void loop()
{
    if (millis() - read_sensor_time >= READ_SENSOR_TIME_INTERVAL)
    {
        read_sensor_time = millis();
        counter_function();
    }
}

void counter_function(void)
{
    // Read sensor states
    sensor_1_state = digitalRead(sensor_1_pin) == DETECTED ? 1 : 0;
    sensor_2_state = digitalRead(sensor_2_pin) == DETECTED ? 1 : 0;

    // Handle system state transitions
    switch (currentState)
    {
    case IDLE:
        if (sensor_1_state && !prevsensor_1_state)
        {
            currentState = ENTRY_IN_PROGRESS;
            PRINTLN("ENTRY_IN_PROGRESS");
            stateStartTime = millis(); // Record the start time of the state
        }
        else if (sensor_2_state && !prevsensor_2_state)
        {
            currentState = EXIT_IN_PROGRESS;
            PRINTLN("EXIT_IN_PROGRESS");
            stateStartTime = millis(); // Record the start time of the state
        }
        break;

    case ENTRY_IN_PROGRESS:
        if (sensor_2_state && !prevsensor_2_state)
        {
            people_count++;
            // Print current people count
            PRINT("People Count: ");
            PRINTLN_IMPORTANT(people_count);
            PRINTLN("Someone entered the room");
            pulseLED(led1Pin, PULSE_DURATION); // Pulse LED1 for 500ms
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
            if (people_count > 0)
            {
                people_count--;
            }
            // Print current people count
            PRINT("People Count: ");
            PRINTLN_IMPORTANT(people_count);
            PRINTLN("Someone exited the room");
            pulseLED(led2Pin, PULSE_DURATION); // Pulse LED2 for 500ms
            currentState = IDLE;
        }
        else if (millis() - stateStartTime >= RESET_TIME_THRESHOLD)
        { // If 5 seconds have passed
            currentState = RESET;
        }
        break;

    case RESET:
        PRINTLN("Resetting state...");
        currentState = IDLE;
        break;
    }
    // Update LED pulsing
    updateLEDPulse();
    // Update previous sensor states
    prevsensor_1_state = sensor_1_state;
    prevsensor_2_state = sensor_2_state;
}

// Function to start pulsing the specified LED for the given duration in milliseconds
void pulseLED(byte ledPin, unsigned long duration)
{
    ledPulsePin = ledPin;
    ledPulseDuration = duration;
    ledPulseStartTime = millis();
    ledPulseInProgress = true;
    digitalWrite(ledPulsePin, HIGH); // Start the LED pulse
}

// Function to update the LED pulsing
void updateLEDPulse()
{
    if (ledPulseInProgress)
    {
        unsigned long elapsed = millis() - ledPulseStartTime;

        if (elapsed >= ledPulseDuration)
        {
            digitalWrite(ledPulsePin, LOW); // Stop the LED pulse
            ledPulseInProgress = false;
        }
    }
}