EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "12V Battery Supply and RS232/TTL Conveter Shield"
Date "2021-08-19"
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ROAM:22uF,25V C3
U 1 1 611BA262
P 2300 1900
F 0 "C3" H 2415 1946 50  0000 L CNN
F 1 "22uF,25V" H 2415 1855 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.33x2.70mm_HandSolder" H 2338 1750 50  0001 C CNN
F 3 "https://search.murata.co.jp/Ceramy/image/img/A01X/G101/ENG/GRM32ER71E226ME15-01.pdf" H 2300 1900 50  0001 C CNN
	1    2300 1900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 611C99EA
P 7200 1900
F 0 "C5" V 6948 1900 50  0000 C CNN
F 1 "1uF" V 7039 1900 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7238 1750 50  0001 C CNN
F 3 "~" H 7200 1900 50  0001 C CNN
	1    7200 1900
	0    1    1    0   
$EndComp
$Comp
L Device:C C7
U 1 1 611C9F9C
P 7600 800
F 0 "C7" V 7348 800 50  0000 C CNN
F 1 "1uF" V 7439 800 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7638 650 50  0001 C CNN
F 3 "~" H 7600 800 50  0001 C CNN
	1    7600 800 
	0    1    1    0   
$EndComp
Text Label 7050 2500 0    50   ~ 0
R1IN
Text Label 7050 2700 0    50   ~ 0
R2IN
Text Label 5450 2700 2    50   ~ 0
R2OUT
Text Label 5450 2500 2    50   ~ 0
R1OUT
Text Label 5450 2300 2    50   ~ 0
T2IN
Text Label 5450 2100 2    50   ~ 0
T1IN
Text Label 7050 2100 0    50   ~ 0
T1OUT
Text Label 7050 2300 0    50   ~ 0
T2OUT
Text Label 5650 3650 0    50   ~ 0
R1IN
Text Label 5650 3550 0    50   ~ 0
T1OUT
$Comp
L power:GND #PWR0101
U 1 1 611CE9FA
P 5950 3450
F 0 "#PWR0101" H 5950 3200 50  0001 C CNN
F 1 "GND" H 5955 3277 50  0000 C CNN
F 2 "" H 5950 3450 50  0001 C CNN
F 3 "" H 5950 3450 50  0001 C CNN
	1    5950 3450
	1    0    0    -1  
$EndComp
Text Label 5150 3450 2    50   ~ 0
R2IN
Text Label 5150 3550 2    50   ~ 0
T2OUT
$Comp
L power:GND #PWR0102
U 1 1 611D02B5
P 5150 3650
F 0 "#PWR0102" H 5150 3400 50  0001 C CNN
F 1 "GND" H 5155 3477 50  0000 C CNN
F 2 "" H 5150 3650 50  0001 C CNN
F 3 "" H 5150 3650 50  0001 C CNN
	1    5150 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 611D2E12
P 700 1550
F 0 "#PWR0104" H 700 1300 50  0001 C CNN
F 1 "GND" H 705 1377 50  0000 C CNN
F 2 "" H 700 1550 50  0001 C CNN
F 3 "" H 700 1550 50  0001 C CNN
	1    700  1550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 611D3443
P 2150 2350
F 0 "#PWR0105" H 2150 2100 50  0001 C CNN
F 1 "GND" H 2155 2177 50  0000 C CNN
F 2 "" H 2150 2350 50  0001 C CNN
F 3 "" H 2150 2350 50  0001 C CNN
	1    2150 2350
	1    0    0    -1  
$EndComp
$Comp
L ROAM:K7803-500R3 U1
U 1 1 611B1418
P 2150 1400
F 0 "U1" H 2528 1438 50  0000 L CNN
F 1 "K7803-500R3" H 2528 1347 50  0000 L CNN
F 2 "Remote Ocean Acidification Monitor:K7803-500R3" H 2150 1700 50  0001 C CNN
F 3 "https://www.mornsun-power.com/html/pdf/K7803-500R3.html" H 2150 1700 50  0001 C CNN
	1    2150 1400
	1    0    0    -1  
$EndComp
$Comp
L power:+4V #PWR0107
U 1 1 611D5D80
P 2600 1750
F 0 "#PWR0107" H 2600 1600 50  0001 C CNN
F 1 "+4V" H 2615 1923 50  0000 C CNN
F 2 "" H 2600 1750 50  0001 C CNN
F 3 "" H 2600 1750 50  0001 C CNN
	1    2600 1750
	1    0    0    -1  
$EndComp
$Comp
L power:+4V #PWR0108
U 1 1 611D747D
P 4300 750
F 0 "#PWR0108" H 4300 600 50  0001 C CNN
F 1 "+4V" H 4315 923 50  0000 C CNN
F 2 "" H 4300 750 50  0001 C CNN
F 3 "" H 4300 750 50  0001 C CNN
	1    4300 750 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 1750 2000 1650
Wire Wire Line
	2000 2350 2150 2350
Wire Wire Line
	2300 2350 2300 2050
Connection ~ 2150 2350
Wire Wire Line
	2150 1650 2150 2350
Wire Wire Line
	2300 1750 2300 1650
$Comp
L ROAM:22uF,25V C1
U 1 1 611B866A
P 2000 1900
F 0 "C1" H 2115 1946 50  0000 L CNN
F 1 "22uF,25V" H 2115 1855 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.33x2.70mm_HandSolder" H 2038 1750 50  0001 C CNN
F 3 "https://search.murata.co.jp/Ceramy/image/img/A01X/G101/ENG/GRM32ER71E226ME15-01.pdf" H 2000 1900 50  0001 C CNN
	1    2000 1900
	-1   0    0    1   
$EndComp
Connection ~ 2000 1750
Wire Wire Line
	2150 2350 2300 2350
Wire Wire Line
	1600 1750 2000 1750
Connection ~ 2300 1750
Wire Wire Line
	2300 1750 2600 1750
$Comp
L ROAM:22uF,25V C2
U 1 1 611B9500
P 2000 2200
F 0 "C2" H 1885 2246 50  0000 R CNN
F 1 "22uF,25V" H 1885 2155 50  0000 R CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.33x2.70mm_HandSolder" H 2038 2050 50  0001 C CNN
F 3 "https://search.murata.co.jp/Ceramy/image/img/A01X/G101/ENG/GRM32ER71E226ME15-01.pdf" H 2000 2200 50  0001 C CNN
	1    2000 2200
	-1   0    0    1   
$EndComp
$Comp
L Device:C C4
U 1 1 611C6521
P 5250 1250
F 0 "C4" H 5365 1296 50  0000 L CNN
F 1 "1uF" H 5365 1205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5288 1100 50  0001 C CNN
F 3 "~" H 5250 1250 50  0001 C CNN
	1    5250 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 1100 5450 1100
Wire Wire Line
	5450 1400 5250 1400
Wire Wire Line
	6250 800  6550 800 
Wire Wire Line
	7750 1900 7350 1900
$Comp
L power:GND #PWR0109
U 1 1 611EA0D8
P 7750 1900
F 0 "#PWR0109" H 7750 1650 50  0001 C CNN
F 1 "GND" H 7755 1727 50  0000 C CNN
F 2 "" H 7750 1900 50  0001 C CNN
F 3 "" H 7750 1900 50  0001 C CNN
	1    7750 1900
	1    0    0    -1  
$EndComp
Connection ~ 7750 1900
$Comp
L Interface_UART:MAX3232 U2
U 1 1 611BBB25
P 6250 2000
F 0 "U2" H 6250 3381 50  0000 C CNN
F 1 "MAX3232" H 6250 3290 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 6300 950 50  0001 L CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX3222-MAX3241.pdf" H 6250 2100 50  0001 C CNN
	1    6250 2000
	1    0    0    -1  
$EndComp
Connection ~ 7750 1600
Wire Wire Line
	7750 1600 7750 1900
Wire Wire Line
	7750 1600 7750 800 
Wire Wire Line
	7050 1600 7450 1600
$Comp
L Device:C C8
U 1 1 611C8388
P 7600 1600
F 0 "C8" V 7348 1600 50  0000 C CNN
F 1 "1uF" V 7439 1600 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7638 1450 50  0001 C CNN
F 3 "~" H 7600 1600 50  0001 C CNN
	1    7600 1600
	0    1    1    0   
$EndComp
Wire Wire Line
	7050 1100 7250 1100
Wire Wire Line
	7050 1400 7250 1400
$Comp
L Device:C C6
U 1 1 611C7CF2
P 7250 1250
F 0 "C6" H 7365 1296 50  0000 L CNN
F 1 "1uF" H 7365 1205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7288 1100 50  0001 C CNN
F 3 "~" H 7250 1250 50  0001 C CNN
	1    7250 1250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 611EC054
P 6250 3200
F 0 "#PWR0110" H 6250 2950 50  0001 C CNN
F 1 "GND" H 6255 3027 50  0000 C CNN
F 2 "" H 6250 3200 50  0001 C CNN
F 3 "" H 6250 3200 50  0001 C CNN
	1    6250 3200
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR0111
U 1 1 611EC4E3
P 4100 1150
F 0 "#PWR0111" H 4100 1000 50  0001 C CNN
F 1 "+3V3" H 4115 1323 50  0000 C CNN
F 2 "" H 4100 1150 50  0001 C CNN
F 3 "" H 4100 1150 50  0001 C CNN
	1    4100 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 1150 4200 1150
$Comp
L power:+3V3 #PWR0112
U 1 1 611EDF9F
P 6550 800
F 0 "#PWR0112" H 6550 650 50  0001 C CNN
F 1 "+3V3" H 6565 973 50  0000 C CNN
F 2 "" H 6550 800 50  0001 C CNN
F 3 "" H 6550 800 50  0001 C CNN
	1    6550 800 
	1    0    0    -1  
$EndComp
Connection ~ 6550 800 
Text Label 3550 2850 2    50   ~ 0
R1OUT
Text Label 3550 2950 2    50   ~ 0
T1IN
Text Label 3550 1850 2    50   ~ 0
T2IN
Text Label 3550 1950 2    50   ~ 0
R2OUT
Wire Wire Line
	3550 1850 3600 1850
Wire Wire Line
	3600 1950 3550 1950
Wire Wire Line
	3550 2850 3600 2850
Wire Wire Line
	3600 2950 3550 2950
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 611F7F53
P 900 1450
F 0 "J1" H 980 1442 50  0000 L CNN
F 1 "Conn_01x02" H 980 1351 50  0000 L CNN
F 2 "Connector_Molex:Molex_Nano-Fit_105313-xx02_1x02_P2.50mm_Horizontal" H 900 1450 50  0001 C CNN
F 3 "~" H 900 1450 50  0001 C CNN
	1    900  1450
	1    0    0    -1  
$EndComp
NoConn ~ 4600 1550
NoConn ~ 4600 1850
NoConn ~ 4600 1950
NoConn ~ 4600 2350
NoConn ~ 4600 2450
NoConn ~ 4600 2550
NoConn ~ 4600 2650
NoConn ~ 3600 1550
NoConn ~ 3600 1650
NoConn ~ 3600 1750
NoConn ~ 3600 2050
NoConn ~ 3600 2150
NoConn ~ 3600 2450
NoConn ~ 3600 2550
NoConn ~ 3600 2650
NoConn ~ 3600 3150
NoConn ~ 3600 3250
$Comp
L power:GND #PWR0116
U 1 1 611FF818
P 1150 750
F 0 "#PWR0116" H 1150 500 50  0001 C CNN
F 1 "GND" H 1155 577 50  0000 C CNN
F 2 "" H 1150 750 50  0001 C CNN
F 3 "" H 1150 750 50  0001 C CNN
	1    1150 750 
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 61200B7E
P 700 750
F 0 "#FLG0101" H 700 825 50  0001 C CNN
F 1 "PWR_FLAG" H 700 923 50  0000 C CNN
F 2 "" H 700 750 50  0001 C CNN
F 3 "~" H 700 750 50  0001 C CNN
	1    700  750 
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 61200F7E
P 1150 750
F 0 "#FLG0102" H 1150 825 50  0001 C CNN
F 1 "PWR_FLAG" H 1150 923 50  0000 C CNN
F 2 "" H 1150 750 50  0001 C CNN
F 3 "~" H 1150 750 50  0001 C CNN
	1    1150 750 
	1    0    0    -1  
$EndComp
NoConn ~ 4000 1150
$Comp
L power:GND #PWR0114
U 1 1 61205408
P 4700 3550
F 0 "#PWR0114" H 4700 3300 50  0001 C CNN
F 1 "GND" H 4705 3377 50  0000 C CNN
F 2 "" H 4700 3550 50  0001 C CNN
F 3 "" H 4700 3550 50  0001 C CNN
	1    4700 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 3550 4600 3550
Wire Wire Line
	4600 3550 4600 3650
Wire Wire Line
	4600 3650 4100 3650
$Comp
L power:PWR_FLAG #FLG0104
U 1 1 6120CE87
P 2050 750
F 0 "#FLG0104" H 2050 825 50  0001 C CNN
F 1 "PWR_FLAG" H 2050 923 50  0000 C CNN
F 2 "" H 2050 750 50  0001 C CNN
F 3 "~" H 2050 750 50  0001 C CNN
	1    2050 750 
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Counter_Clockwise J2
U 1 1 61215810
P 5350 3550
F 0 "J2" H 5400 3867 50  0000 C CNN
F 1 "Conn_02x03_Counter_Clockwise" H 5400 3776 50  0000 C CNN
F 2 "Connector_Molex:Molex_Nano-Fit_105314-xx06_2x03_P2.50mm_Horizontal" H 5350 3550 50  0001 C CNN
F 3 "~" H 5350 3550 50  0001 C CNN
	1    5350 3550
	1    0    0    -1  
$EndComp
$Comp
L MCU_Module_Adafruit:Adafruit_Feather_Generic A1
U 1 1 612168AF
P 4100 2350
F 0 "A1" H 4100 961 50  0000 C CNN
F 1 "Adafruit_Feather_Generic" H 4100 870 50  0000 C CNN
F 2 "MCU_Module_Adafruit:Adafruit_Feather_WithMountingHoles" H 4200 1000 50  0001 L CNN
F 3 "https://cdn-learn.adafruit.com/downloads/pdf/adafruit-feather.pdf" H 4100 1550 50  0001 C CNN
	1    4100 2350
	1    0    0    -1  
$EndComp
NoConn ~ 3600 2250
$Comp
L Diode:B140-E3 D1
U 1 1 6122BEAE
P 4300 900
F 0 "D1" V 4346 820 50  0000 R CNN
F 1 "B140-E3" V 4255 820 50  0000 R CNN
F 2 "Diode_SMD:D_SMA" H 4300 725 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88946/b120.pdf" H 4300 900 50  0001 C CNN
	1    4300 900 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5950 3450 5650 3450
$Comp
L Mechanical:Fiducial FID1
U 1 1 611E9950
P 2550 800
F 0 "FID1" H 2635 846 50  0000 L CNN
F 1 "Fiducial" H 2635 755 50  0000 L CNN
F 2 "Fiducial:Fiducial_0.5mm_Mask1.5mm" H 2550 800 50  0001 C CNN
F 3 "~" H 2550 800 50  0001 C CNN
	1    2550 800 
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R3
U 1 1 611EA0BE
P 1250 2300
F 0 "R3" H 1318 2346 50  0000 L CNN
F 1 "14.7k" H 1318 2255 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 1290 2290 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/productsearch/datasheet/rchip/PYu-RC_Group_51_RoHS_L_11.pdf" H 1250 2300 50  0001 C CNN
	1    1250 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R4
U 1 1 611EF075
P 1250 2700
F 0 "R4" H 1318 2746 50  0000 L CNN
F 1 "3.3k" H 1318 2655 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 1290 2690 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/productsearch/datasheet/rchip/PYu-RC_Group_51_RoHS_L_11.pdf" H 1250 2700 50  0001 C CNN
	1    1250 2700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 611F101A
P 1250 2850
F 0 "#PWR04" H 1250 2600 50  0001 C CNN
F 1 "GND" H 1255 2677 50  0000 C CNN
F 2 "" H 1250 2850 50  0001 C CNN
F 3 "" H 1250 2850 50  0001 C CNN
	1    1250 2850
	1    0    0    -1  
$EndComp
Connection ~ 1250 2500
Wire Wire Line
	1250 2500 1250 2550
Wire Wire Line
	1250 2450 1250 2500
Text Label 1350 2500 0    50   ~ 0
VBATT_LVL
Wire Wire Line
	1350 2500 1250 2500
$Comp
L Device:R_US R1
U 1 1 611F5542
P 700 2300
F 0 "R1" H 768 2346 50  0000 L CNN
F 1 "3.74k" H 768 2255 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 740 2290 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/productsearch/datasheet/rchip/PYu-RC_Group_51_RoHS_L_11.pdf" H 700 2300 50  0001 C CNN
	1    700  2300
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R2
U 1 1 611F5548
P 700 2700
F 0 "R2" H 768 2746 50  0000 L CNN
F 1 "3.3k" H 768 2655 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 740 2690 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/productsearch/datasheet/rchip/PYu-RC_Group_51_RoHS_L_11.pdf" H 700 2700 50  0001 C CNN
	1    700  2700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 611F554E
P 700 2850
F 0 "#PWR02" H 700 2600 50  0001 C CNN
F 1 "GND" H 705 2677 50  0000 C CNN
F 2 "" H 700 2850 50  0001 C CNN
F 3 "" H 700 2850 50  0001 C CNN
	1    700  2850
	1    0    0    -1  
$EndComp
Connection ~ 700  2500
Wire Wire Line
	700  2500 700  2550
Wire Wire Line
	700  2450 700  2500
Text Label 800  2500 0    50   ~ 0
VBUS_LVL
Wire Wire Line
	800  2500 700  2500
$Comp
L power:VBUS #PWR07
U 1 1 611FB806
P 4850 1100
F 0 "#PWR07" H 4850 950 50  0001 C CNN
F 1 "VBUS" H 4865 1273 50  0000 C CNN
F 2 "" H 4850 1100 50  0001 C CNN
F 3 "" H 4850 1100 50  0001 C CNN
	1    4850 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 1100 4300 1100
Wire Wire Line
	4300 1100 4300 1050
Wire Wire Line
	4300 1150 4300 1100
Connection ~ 4300 1100
$Comp
L power:+VDC #PWR0113
U 1 1 611FD98B
P 700 750
F 0 "#PWR0113" H 700 650 50  0001 C CNN
F 1 "+VDC" H 700 1025 50  0000 C CNN
F 2 "" H 700 750 50  0001 C CNN
F 3 "" H 700 750 50  0001 C CNN
	1    700  750 
	-1   0    0    1   
$EndComp
$Comp
L power:+VDC #PWR0103
U 1 1 611D2B13
P 700 1450
F 0 "#PWR0103" H 700 1350 50  0001 C CNN
F 1 "+VDC" H 700 1725 50  0000 C CNN
F 2 "" H 700 1450 50  0001 C CNN
F 3 "" H 700 1450 50  0001 C CNN
	1    700  1450
	1    0    0    -1  
$EndComp
$Comp
L power:+VDC #PWR0106
U 1 1 611D5257
P 1600 1750
F 0 "#PWR0106" H 1600 1650 50  0001 C CNN
F 1 "+VDC" H 1600 2025 50  0000 C CNN
F 2 "" H 1600 1750 50  0001 C CNN
F 3 "" H 1600 1750 50  0001 C CNN
	1    1600 1750
	1    0    0    -1  
$EndComp
$Comp
L power:+VDC #PWR03
U 1 1 611FEA37
P 1250 2150
F 0 "#PWR03" H 1250 2050 50  0001 C CNN
F 1 "+VDC" H 1250 2425 50  0000 C CNN
F 2 "" H 1250 2150 50  0001 C CNN
F 3 "" H 1250 2150 50  0001 C CNN
	1    1250 2150
	1    0    0    -1  
$EndComp
$Comp
L power:VBUS #PWR01
U 1 1 611FFC2C
P 700 2150
F 0 "#PWR01" H 700 2000 50  0001 C CNN
F 1 "VBUS" H 715 2323 50  0000 C CNN
F 2 "" H 700 2150 50  0001 C CNN
F 3 "" H 700 2150 50  0001 C CNN
	1    700  2150
	1    0    0    -1  
$EndComp
$Comp
L power:VBUS #PWR05
U 1 1 6120040B
P 1600 750
F 0 "#PWR05" H 1600 600 50  0001 C CNN
F 1 "VBUS" H 1615 923 50  0000 C CNN
F 2 "" H 1600 750 50  0001 C CNN
F 3 "" H 1600 750 50  0001 C CNN
	1    1600 750 
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG01
U 1 1 61200A56
P 1600 750
F 0 "#FLG01" H 1600 825 50  0001 C CNN
F 1 "PWR_FLAG" H 1600 923 50  0000 C CNN
F 2 "" H 1600 750 50  0001 C CNN
F 3 "~" H 1600 750 50  0001 C CNN
	1    1600 750 
	1    0    0    -1  
$EndComp
Text Label 4600 2150 0    50   ~ 0
VBATT_LVL
Text Label 4600 2250 0    50   ~ 0
VBUS_LVL
Wire Wire Line
	6550 800  7450 800 
$Comp
L power:+3.3V #PWR06
U 1 1 61208B9A
P 2050 750
F 0 "#PWR06" H 2050 600 50  0001 C CNN
F 1 "+3.3V" H 2065 923 50  0000 C CNN
F 2 "" H 2050 750 50  0001 C CNN
F 3 "" H 2050 750 50  0001 C CNN
	1    2050 750 
	-1   0    0    1   
$EndComp
Wire Notes Line
	2300 550  2300 1100
Wire Notes Line
	550  1100 3200 1100
Wire Notes Line
	550  3100 3200 3100
Wire Notes Line
	3200 3950 7900 3950
Wire Notes Line
	7900 3950 7900 550 
Wire Notes Line
	3200 550  3200 3950
Text Notes 2450 3000 0    50   ~ 0
Power subsystem
Text Notes 6600 3850 0    50   ~ 0
Processor board interconnect\nand communications subsystem
Text Notes 6590 7480 0    50   ~ 0
1
Text Notes 6680 7480 0    50   ~ 0
1
Text Notes 4140 320  0    50   ~ 0
WARNING! Do not omit this diode! VBUS is connected to 5V USB power through the Feather, so this prevents power backflowing\n(essentially shorting) through the Buck converter when the USB is connected to the feather! Directly connecting the Buck converter to\nthe VBUS line can permanently damage your computer's USB hardware!
Wire Notes Line
	4600 350  4600 850 
Wire Notes Line
	4600 850  4500 850 
$EndSCHEMATC
