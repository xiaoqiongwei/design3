EESchema Schematic File Version 2  date 2013-01-30 23:12:01
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 43  0
EELAYER END
$Descr User 11000 8500
encoding utf-8
Sheet 1 2
Title "Robot"
Date "31 jan 2013"
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3850 4800 1450 4800
Wire Wire Line
	1550 4550 2350 4550
Wire Wire Line
	1450 4800 1450 4850
$Comp
L FUSE F1
U 1 1 5109EBC2
P 3600 4550
F 0 "F1" H 3700 4600 40  0000 C CNN
F 1 "5A" H 3500 4500 40  0000 C CNN
	1    3600 4550
	1    0    0    -1  
$EndComp
$Comp
L SPST SW1
U 1 1 5109EBA6
P 2850 4550
F 0 "SW1" H 2850 4650 70  0000 C CNN
F 1 "SPST" H 2850 4450 70  0000 C CNN
	1    2850 4550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5109E1C7
P 1450 4850
F 0 "#PWR?" H 1450 4850 30  0001 C CNN
F 1 "GND" H 1450 4780 30  0001 C CNN
	1    1450 4850
	1    0    0    -1  
$EndComp
$Comp
L VCC 11.1V
U 1 1 5109E132
P 1550 4550
F 0 "11.1V" H 1550 4650 30  0000 C CNN
F 1 "VCC" V 3100 9200 30  0000 C CNN
	1    1550 4550
	0    -1   -1   0   
$EndComp
$Sheet
S 3850 4300 1050 900 
U 5109E03D
F0 "Alimentation" 60
F1 "alimentation.sch" 60
F2 "Vin" I L 3850 4550 60 
F3 "GND" I L 3850 4800 60 
F4 "Vout5V" O R 4900 4550 60 
F5 "Vout11.1V" O R 4900 4800 60 
F6 "Vout24V" O R 4900 5050 60 
$EndSheet
$EndSCHEMATC