# Homework 5

The task was to implement a stopwatch that counts in tenths of a second with 3 buttons: Start/Pause, Reset and Lap. 

## Components

* 1x 74HC595 IC
* 1x 4-digit 7-segment display
* 8x 330Ω resistor
* 4x pushbutton

## Setup

![schematic of my setup](/Homework_05/schematic.png)

![picture of my setup](/Homework_05/setup.jpg)

## Video

[Demonstration](https://youtu.be/tpJcqW-vfzU)

## Notes
* To prevent flickering, whenever certain actions are triggered (save lap, reset lap) they will wait one cycle to be executed. This is to make sure there is no noticeable delay in the display writing.