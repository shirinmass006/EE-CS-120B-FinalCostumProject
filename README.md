# Microncontrollers used: ATMEGA 1284, Quantity: 2
# For my final project: I designed a fan controller(3D printed fan connected to a DC motor) that changes the speed of a motor based upon a temperature sensor. I display the temperature measure in celcius on one LCD and use a second LCD to display the last ten temperatures. The user can use a joystick to go up or down in the array to display the desired stored temperature on the 2nd LCD. 
# Details: 
# Microcontroller 1: I use the 1st micro controller to only program my DC motor. The speed of the motor depends to how hot the temperature sensor gets. The hotted the temperature sensor gets, the faster the DC motor will rotate to compensenate for the hot temperature. 
# Microcontroller 2: I use the 1st micro controller to program both LCDs and Joystick.

# There are 5 state machines: 

# 1st SM: Light up the LEDs depending on how hot the temperature sensor gets
# 2nd SM: Displays the temperature on the 1st LCD using the ADC value 
# 3rd SM: ADC mux that collects which ADC value to pick between the temperature sensor and the joystick.
# 4th SM: Display last ten that displays the last ten temperature values on the 2nd LCD
# 5th SM: Joystick control that controlls which value in that array we chose to display depending on how they move the joystick (up or down)

# There are 4 functions:
# 1st function: PWM function that controlls the motor
# 2nd function: ADC init function that enables the ADC 
# 3rd function: LCD_Custom_Char that displays a special character
# 4th function: TemperatureArray that saves the last ten temperature values in an array

# PWM Motor Controller with LCD display / EE 120B
