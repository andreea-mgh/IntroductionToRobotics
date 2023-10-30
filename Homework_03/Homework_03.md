# Homework 3

This assignment involves simulating a 3-floor elevator control system using LEDs, buttons, and a buzzer with Arduino.

## Components

* 3x yellow LED (floor indicators)
* 1x green LED (state indicator)
* 4x 220Ω resistor
* 1x passive buzzer
* 3x pushbutton

## Setup

![schematic of my setup](/Homework_03/schematic.png)

![picture of my setup](/Homework_03/setup.jpg)

## Video

[Demonstration](https://youtu.be/)

## Notes

* Since the floor states are stored in an array, the program can be scaled for multiple floors by just changing the `floors` variable and setting the appropriate pins.