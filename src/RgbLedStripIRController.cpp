#include <Arduino.h>
#include <IRremote.h>

#pragma region Globals

// Define pins to easier assign values
#define redPin 5
#define greenPin 6
#define bluePin 3

// Define ir receiver
const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

// Other global values
int mode = 0;
int speed = 1000; // 1 sec
int fadeValue = 0; 

bool redEnabled = false;
bool greenEnabled = false;
bool blueEnabled = false;

// Values of each color
int redValue = 255;
int greenValue = 255;
int blueValue = 255;

// -1. Start value to set led's beginning values
// 0. decrease red led, increase green led
// 1. decrease green led, increase blue led
// 3. decrease blue led, increase red led
int rainbowMode = 0;

#pragma endregion

#pragma region Functions Declaration

void ModeSelect(decode_results results);
void SetRGBColor();
void SetRGBColor(int red, int green, int blue);
void Rainbow();
void LedOn();
void BlinkDebug(int howMany);

#pragma endregion


// Initialization
void setup()
{
    // Select pin modes
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    Serial.begin(9600);

    // Initialize IR receiver
    irrecv.enableIRIn();
    // irrecv.blink13(true);
}

// Main loop
void loop()
{
    if (irrecv.decode(&results))
    {
        ModeSelect(results);
        Serial.println(results.value, HEX);
    }
    Serial.println(results.value, HEX);
    BlinkDebug(mode);
}

// Changes mode value to set other light mode
void ModeSelect(decode_results results)
{
    switch (results.value)
    {
    case 0xEF1010EF:
        mode = 0;
        redEnabled = !redEnabled; 
        break;
    case 0xEF108877:
        mode = 1;
        greenEnabled = !greenEnabled;
        break;
    case 0xEF1048B7:
        mode = 2;
        blueEnabled = !blueEnabled;
        break;
    default:
        Serial.println("Undefined");
        break;
    }

    irrecv.resume();
}

void SetRGBColor()
{
    analogWrite(redPin, redValue * redEnabled);
    analogWrite(greenPin, greenValue * redEnabled);
    analogWrite(bluePin, blueValue * redEnabled);
}

// Set values for led strip
void SetRGBColor(int red, int green, int blue)
{
    redValue = red,
    greenValue = green;
    blueValue = blue;

    analogWrite(redPin, redValue * redEnabled);
    analogWrite(greenPin, greenValue * redEnabled);
    analogWrite(bluePin, blueValue * redEnabled);
}

// Play selected mode
void Play()
{
    
}
// Reset to default settings when reveived signal, except mode
bool Reset()
{
    if (irrecv.decode(&results))
    {
        SetRGBColor(255,255,255);

        redEnabled = false;
        greenEnabled = false;
        blueEnabled = false;

        rainbowMode = -1;
        speed = 1000;
        fadeValue = 0;

        ModeSelect(results);
        return true;
    }
    else
    {
        return false;
    }
}

// ONLY FOR DEBUGING
void BlinkDebug(int howMany)
{
    if(howMany == 0) return;
    digitalWrite(13, HIGH);
    delay(1000/howMany);
    digitalWrite(13, LOW);
    delay(1000/howMany);
}

// Leds will be always on
void LedOn()
{
    SetRGBColor(255,255,255);
}
// 0. decrease red led, increase green led
// 1. decrease green led, increase blue led
// 3. decrease blue led, increase red led
void Rainbow()
{
    // if irreceiver receive any signal signal
    if(Reset())
    {
        return;
    }
    // Initiates beginning of animation
    else if (rainbowMode == -1)
    {
        redEnabled = true;
        greenEnabled = true;
        blueEnabled = true;

        SetRGBColor(255,0,0);
        rainbowMode = 0;
    }
    else if (rainbowMode == 0)
    {
        redValue -= 5;
        greenValue += 5;
        SetRGBColor();
        if (greenValue >= 255)
        {
            rainbowMode = 1;
        }
    }
    else if(rainbowMode == 1)
    {
        greenValue -= 5;
        blueValue += 5;
        SetRGBColor();
        if (blueValue >= 255)
        {
            rainbowMode = 2;
        }
    }
    else if(rainbowMode == 2)
    {
        blueValue -= 5;
        redValue += 5;
        SetRGBColor();
        if(blueValue >= 255)
        {
            rainbowMode = 0;
        }
    }
}