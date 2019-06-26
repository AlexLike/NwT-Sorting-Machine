// SORTING MACHINE - Main
// A project by Simon Müller, Simeon Schwarzkopf & Alexander Zank
// Copyright (C) 2019 Alexander Zank
// Available under the MIT License, see README.md
// Last update on 20.05.2019


// MARK: - Dependencies

#include <Servo.h>


// MARK: - Global Definitions

#define sensorPin A0            // The reflex optocoupler's pin
#define horizontalServoPin 10   // The big servo's connector pin
#define verticalServoPin 9      // The small servo's connector pin
#define resetButtonPin 7        // The reset button's input pin
#define blackAngle 58           // The angle between the reset position and the white deposit
#define whiteAngle 38           // The angle between the reset position and the black deposit
#define servoSpeedFactor 30     // The delay (ms) between writing the next integer angle to any servo


// MARK: - Global Properties

Servo horizontalServo = Servo();    // The big servo object, initialized at the beginning
Servo verticalServo = Servo();      // The small servo object, initialized at the beginning
int sensorNorm;                     // The value returned by the sensor at daylight, calibrated regularly


// MARK: - Controller Lifecycle

/// A synchronous main function called once after the controller has booted up.
/// Parameters: *None*
/// Returns: *Nothing*
void setup() {
    // Attach the servo objects to their physical pin counterparts
    horizontalServo.attach(horizontalServoPin);
    verticalServo.attach(verticalServoPin);
    // Configure the reset button's pin
    pinMode(resetButtonPin, INPUT_PULLUP);
    // Establish Serial connection for debugging
    Serial.begin(57600);
}

/// A synchronous main function called repeatedly after void setup().
/// Parameters: *None*
/// Returns: *Nothing*
void loop() {
    calibrateSensor();
    enrollBall();
    if (isBallWhite()) {
        depositWhite();
    } else {
        depositBlack();
    }
    reset();
}


// MARK: - Helper Methods

/// A synchronous function that moves the vertical servo, and with it the arm construction, down onto the ball
/// Parameters: *None*
/// Returns: *Nothing*
void enrollBall() {
    Serial.println("Enrolling ball...");
    // Move vertically
    moveToAngleSlowly(verticalServo, 68);
    // Delay further execution
    sleepAndCheckButtonStatus(1000);
}

/// A synchronous function that checks the optocoupler's value against a known threshold
/// Parameters: *None*
/// Returns: A boolean value indicating whether the ball is white (true) or black (false)
boolean isBallWhite() {
    // Compare the current sensor value to the calibrated threshold
    return analogRead(sensorPin) > sensorNorm;
}

/// A synchronous function that moves the horizontal servo, and with it the arm construction, to the left deposit hole
/// Parameters: *None*
/// Returns: *Nothing*
void depositBlack() {
    Serial.println("Depositing black ball...");
    // Move horizontally
    horizontalServo.write(90 + blackAngle);
    // Delay further execution
    sleepAndCheckButtonStatus(500);
}

/// A synchronous function that moves the horizontal servo, and with it the arm construction, to the right deposit hole
/// Parameters: *None*
/// Returns: *Nothing*
void depositWhite() {
    Serial.println("Depositing white ball...");
    // Move horizontally
    horizontalServo.write(90 - whiteAngle);
    // Delay further execution
    sleepAndCheckButtonStatus(500);
}

/// A synchronous function that moves both servos to their initial position
/// Parameters: *None*
/// Returns: *Nothing*
void reset() {
    Serial.println("Resetting...");
    // Move horizontally
    horizontalServo.write(90);
    // Move vertically
    moveToAngleSlowly(verticalServo, 110);
    // Delay further execution (without checking the reset button)
    delay(2000);
}

/// A synchronous function that reads the optocoupler's current value and stores it for later reference as a threshold
/// Parameters: *None*
/// Returns: *Nothing*
void calibrateSensor() {
    // Update the daylight value
    sensorNorm = analogRead(sensorPin) - 20;
    Serial.print("Sensor threshold set to ");
    Serial.println(sensorNorm);
}

/// A synchronous function that attempts to emulate asynchronous behaviour by not actually idling but rather continuously checking the reset button's status
/// Parameters: *None*
/// Returns: *Nothing*
void sleepAndCheckButtonStatus(int delay) {
    // Store the current Arduino run time
    const int initialRunTime = millis();
    int currentRunTime = initialRunTime;
    // Continuously check whether a reset is needed; abort execution when the delay is reached
    while (currentRunTime < initialRunTime + delay) {
        // Detect whether the button is pressed and reset if this is the case
        resetIfNeeded();
        // Update current run time
        currentRunTime = millis();
    }
}

/// A synchronous function that checks whether the reset button was pressed and resets the device if this is the case
/// Parameters: *None*
/// Returns: *Nothing*
void resetIfNeeded() {
    // Check whether the button is pressed
    if (!digitalRead(resetButtonPin)) {
        Serial.println("Button pressed!");
        // Go back to initial position
        reset();
    }
}

/// A synchronous function that makes a given servo move more slowly than it's default speed by continuously mutating angle values after short time intervals
/// Parameters: - Servo servo: The servo object that should be written to
///             - int angle: The desired final angle
/// Returns: *Nothing*
void moveToAngleSlowly(Servo servo, int angle) {
    // Store the current servo angle as a constant
    const int initialAngle = servo.read();
    // Define increment sign based on direction of rotation
    int increment;
    if (initialAngle < angle) {
        increment = 1;
    } else {
        increment = -1;
    }
    // Iterate through all integer angles until the desired angle is met
    for (int currentAngle = initialAngle; currentAngle != angle; currentAngle += increment) {
        servo.write(currentAngle);
        delay(6);
    }
}