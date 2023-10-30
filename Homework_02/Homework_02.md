# Homework 2

This assignment focuses on controlling each channel (Red, Green, and Blue) of an RGB LED using individual potentiometers.

## Components

 * 1x RGB LED
 * 3x 50K potentiometer
 * 2x 220Ω resistor
 * 1x 330Ω resistor

## Setup

![schematic of my setup](/Homework_02/schematic.png)

![picture of my setup](/Homework_02/setup.jpg)

## Video

[Demonstration](https://youtu.be/eDtiuODo5ew)

## Notes

* The light intensity of each color of the RGB LED in my kit didn't really match the one in my datasheet, therefore I set the color correction for each color by trial and error.
* Due to the noise on the analog inputs there is a small threshold for the LED to light up to prevent the flickering visible at low intensities.
* I put a piece of translucent tape over the LED to make the colors blend in a little better.