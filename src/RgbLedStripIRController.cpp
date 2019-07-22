#include <Arduino.h>
#include <IRremote.h>

#pragma region Globals

//define pins to easier assign values
#define redPin 5
#define greenPin 6
#define bluePin 3

//pilot buttons
#define nextMode 0xFD609F
#define previousMode 0xFD20DF
#define brightUp 0xFD906F
#define brightDown 0xFD807F
#define redUp 0xFD08F7
#define redDown 0xFD28D7
#define greenUp 0xFD8877
#define greenDown 0xFDA857
#define blueUp 0xFD48B7
#define blueDown 0xFD6897
#define onOff 0xFD00FF

//define ir receiver
const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

//other global values
int mode = 0;
int delaySpeed = 1000; // 1 sec
int fadeValue = 0;
int fadeIterator = 13;
double brightnessLimit = 1;

bool isOn = 1;

//values of each color
int redValue = 255;
int greenValue = 255;
int blueValue = 255;

double redPower = 1;
double greenPower = 1;
double bluePower = 1;

/** 
 *  -1. Start value to set led's beginning values
 *  0. decrease red led, increase green led
 *  1. decrease green led, increase blue led
 *  3. decrease blue led, increase red led
*/
int rainbowMode = -1;

#pragma endregion

#pragma region Functions Declaration

bool Menu(decode_results results);
void SetRGBColor();
void SetRGBColor(int red, int green, int blue);
void Rainbow();
void StaticLight();
void LedOn();
void Fade();
void ChangeLedPower(decode_results results);
void ChangeBrightnessLimit(double changedBrightness);
void ChangeMode(int changedMode);

#pragma endregion

//initialization
void setup() {
    //select pin modes
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    Serial.begin(9600);

    Reset();

    //initialize IR receiver
    irrecv.enableIRIn();
    // irrecv.blink13(true);
}

//main loop
void loop() {
    if (irrecv.decode(&results)){
        if (Menu(results)) {
            Reset();
        };
        Serial.println(results.value, HEX);
        irrecv.resume();
    }
    Play();
}

/**
 * Changes light mode
 * 
 * @param {decode_results} results - Value get form remote.
 */
bool Menu(decode_results results) {
    switch (results.value){
        //mode changing
        case onOff:
            isOn = !isOn;
            return false;

        case previousMode:
            ChangeMode(mode - 1);
            return true;

        case nextMode:
            ChangeMode(mode + 1);
            return true;

        //brightness changing
        case brightDown:
            ChangeBrightness(brightnessLimit - 0.1);
            return false;

        case brightUp:
            ChangeBrightness(brightnessLimit + 0.1);
            return false;

        default:
            ChangeLedPower(results);
            return false;
    }
}

/**
 * Mode is changed by arrows.
 * To prevent switching to non-exist mode.
 * 
 * @param changedMode - mode + value form arrow press.
 */
void ChangeMode(int changedMode) {
    if(changedMode > 2){
        mode = 0;
        Serial.println("mode changed to first");
        Serial.print("Current mode: ");
        Serial.println(mode);
    } else if(changedMode < 0) {
        mode = 2;
        Serial.println("mode changed to last");
        Serial.print("Current mode: ");
        Serial.println(mode);
    } else {
        mode = changedMode;
        Serial.print("Current mode: ");
        Serial.println(mode);
    }
}

void ChangeBrightness(double changedBrightness) {
    if(changedBrightness < 1.05 && changedBrightness > -0.05){
        brightnessLimit = changedBrightness;
        Serial.println("brightness limit changed");
        Serial.println(brightnessLimit);
    }
}

void SetRGBColor() {
    analogWrite(redPin, redValue * brightnessLimit * isOn);
    analogWrite(greenPin, greenValue * brightnessLimit * isOn);
    analogWrite(bluePin, blueValue * brightnessLimit * isOn);
}

//set values for led strip
void SetRGBColor(int red, int green, int blue) {
    redValue = red,
    greenValue = green;
    blueValue = blue;

    analogWrite(redPin, redValue * brightnessLimit * isOn);
    analogWrite(greenPin, greenValue * brightnessLimit * isOn);
    analogWrite(bluePin, blueValue * brightnessLimit * isOn);
}

//play selected mode
void Play() {
    switch (mode) {
        case 0:
            Rainbow();
            break;

        case 1:
            Fade();
            break;

        case 2:
            StaticLight();
            break;
        
        default:
            break;
    }
}

/**
 * Set default values when mode switched controller powered on.
 * 
 * @author Konrad Szerwiński,
 * @version 1.0,
 * @return void().
 */
bool Reset() {
        SetRGBColor(255, 255, 255);

        brightnessLimit = 1;
        isOn = 1;

        rainbowMode = -1;
        brightnessLimit = 1;
        fadeIterator = 13;
        fadeValue = 0;

        redPower = 1;
        greenPower = 1;
        bluePower = 1;

        return true;
}

//led's will be always on
void LedOn() {
    SetRGBColor(255, 255, 255);
}

void LedOf() {
    SetRGBColor(0,0,0);
}

// 0. decrease red led, increase green led
// 1. decrease green led, increase blue led
// 3. decrease blue led, increase red led
void Rainbow() {
    //if IR Receiver receive any signal signal
    delay(250);
    switch (rainbowMode) {
        case -1:
            SetRGBColor(255, 0, 0);
            rainbowMode = 0;
            break;

        case 0:
            redValue -= 5;
            greenValue += 5;
            SetRGBColor();
            if (greenValue >= 255)
            {
                rainbowMode = 1;
            }
            break;

        case 1:
            greenValue -= 5;
            blueValue += 5;
            SetRGBColor();
            if (blueValue >= 255) 
            {
                rainbowMode = 2;
            } 
            break;

        case 2:
            blueValue -= 5;
            redValue += 5;
            SetRGBColor();
            if (redValue >= 255)
            {
                rainbowMode = 0;
            }
            break;
    }
}

void ChangeLedPower(decode_results results){
    switch (results.value)
    {
    case redUp:
        if (redPower < 1)
            redPower += 0.1;
        break;
    case redDown:
        if (redPower > 0)
            redPower -= 0.1;
        break;
    case greenUp:
        if (greenPower < 1)
            greenPower += 0.1;
        break;
    case greenDown: 
        if (greenPower > 0)
            greenPower -= 0.1;
        break;
    case blueUp:
        if (bluePower < 1)
            bluePower += 0.1;
        break;
    case blueDown: 
        if (bluePower > 0)
            bluePower -= 0.1;
        break;
    default:
        break;
    }
}

void StaticLight() {
    SetRGBColor(255  * redPower, 255* greenPower, 255 * bluePower);

}

void Fade() {
    delay(50);
    if(fadeValue + fadeIterator > 255) {
        fadeValue = 255;
        fadeIterator *= -1;
    } else if (fadeValue + fadeIterator < 0) {
        fadeValue = 0;
        fadeIterator *= -1;
    }
    else {
        fadeValue += fadeIterator;
    }
    SetRGBColor((255 - fadeValue) * redPower, (255 - fadeValue) * greenPower, (255 - fadeValue) * bluePower);
}