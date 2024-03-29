# Sorting Machine
**An educational project by Simeon Schwarzkopf, Simon Müller and Alexander Zank realized at Hohenstaufen-Gymnasium Bad Wimpfen during June 2019.**

## Introduction
The goal of project Sorting Machine is the implementation of a mechanism that sorts table tennis balls according to their color on a radial playfield into two depots: one for black and one for white balls.
Programatically this is achieved by using an Arduino Genuino Uno and the code contained in this repository.

## Construction Outline
Two servo motors serve the horizontal and vertical axis and operate a 3D-printed cover-up claw freely with an optocoupler sensor installed on its inside pointing to the ball’s surface for brightness identification.
Additionally, a pulled-up button is installed onto the machine’s body that functions as a position reset button.

![Construction Outline](https://raw.githubusercontent.com/AlexLike/NwT-Sorting-Machine/Documentation-Assets/Contruction%20Outline.jpeg?token=AHIOBSZSEB554PH7ZZQBFA25EDWX2)

## Coding Principles
This project's code was written in native C++ with the Arduino's base and servo libraries. A key design decision was emphasizing the modularity and clarity of the code and keeping it as lightweight as possible so that the Arduino runs with a low latency, allowing us to simulate asynchronous behaviour regarding the reset button.

The code is clearly structured in the following sections, allowing any contributer to more easily navigate the small project:
```C++
// MARK: - Dependencies
// MARK: - Global Definitions
// MARK: - Global Properties
// MARK: - Controller Lifecycle
// MARK: - Helper Methods
```

As an added challange, instead of just running synchronous code, a reset button was conceptioned that would allow us to troubleshoot and calibrate our code more easily in the testing phase while adding some complexity to it. The function
`void sleepAndCheckButtonStatus(int delay)` is called as an alternative to the native `void delay(int delay)` function and simulates said asynchronous behaviour:

```C++
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
```

By constantly keeping track of the Arduino's runtime, `void resetIfNeeded()` can be called as long as the delay hasn't exceeded.

While testing, we also noticed that the vertical servo motor was moving too quickly, resulting in a forceful punch onto the playfield's surface. To mitigate this issue, we implemented `void moveToAngleSlowly(Servo servo, int angle)`, a function that only updates servo values after a small delay, resulting in a slower movement without giving up much fluency:

```C++
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
        delay(servoSpeedDelay);
    }
}
```

All these advanced functions, especially the servo one require a low latency so that no stuttering can become emminent. One measure to save dynamic storage, that impacts latency, while preserving the ease of use in the adjustment and calibration process (e.g. trying different `blackAngle`s) was the use of the C++-Keyword `#define` for any constant value in the code, as a declaration using said keyword isn't saved to the dynamic storage at runtime but rather filled in at compiletime. The Arduino has way more static storage for code than dynamic storage for changing variables, so using the latter allows for a better use of dynamic storage, e.g. the `sensorNorm` value that changes constantly.

## License
>Copyright (c) 2019 Alexander Zank
>
>Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
>
>The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
>
>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
