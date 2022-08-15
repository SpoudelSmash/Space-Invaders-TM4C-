# Nyan Defender
Nyan Defender is a Space Invaders inspired game where you play as Nyan Cat and defend against Rat King's army.

It's played using a TM4C microcontroller and 
* An LCD: to display game in real time (Digital Output)
* Two Buttons: Fire/Select and Pause/Back buttons (Digital Input)
* A potentiometer: used for player movement (Analog Input)

***Hardware I/O connections***
* Slide pot pin 1 connected to ground
* Slide pot pin 2 connected to PD2/AIN5
* Slide pot pin 3 connected to +3.3V 
* fire button connected to PE0
* special weapon fire button connected to PE1
* 8*R resistor DAC bit 0 on PB0 (least significant bit)
* 4*R resistor DAC bit 1 on PB1
* 2*R resistor DAC bit 2 on PB2
* 1*R resistor DAC bit 3 on PB3 (most significant bit)
* LED on PB4
* LED on PB5

***LCD connections***
* Backlight (pin 10) connected to +3.3 V
* MISO (pin 9) unconnected
* SCK (pin 8) connected to PA2 (SSI0Clk)
* MOSI (pin 7) connected to PA5 (SSI0Tx)
* TFT_CS (pin 6) connected to PA3 (SSI0Fss)
* CARD_CS (pin 5) unconnected
* Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
* RESET (pin 3) connected to PA7 (GPIO)
* VCC (pin 2) connected to +3.3 V
* Gnd (pin 1) connected to ground
<br/>

![image](https://user-images.githubusercontent.com/55002142/184737334-4e787bc9-5382-4319-996c-38521cc3ce36.png)
*Demo of Nyan Defenders game in progress*
