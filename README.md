# egcp-2120-lightsaber
DIY lightsaber built from scratch with an ATmega328P, using the PlatformIO VSCode extension development environment and Fritzing to generate the wiring diagram. The overal goal for the code of this project is to write everything without the use of any helper libraries, rather only the base AVR libraries (for registers, interrupts, and delays). Any and all  helper libraries will be written based off of the AVR libraries.

## Schematic
![Wiring Diagram](schematics/lightsaber_schem.png)

## Datasheets
ATmega328P - [https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf) <br>
MPU6050 - [https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf) <br>
MCP4725 - [https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP4725-12-Bit-Digital-to-Analog-Converter-with-EEPROM-Memory-DS20002039.pdf](https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP4725-12-Bit-Digital-to-Analog-Converter-with-EEPROM-Memory-DS20002039.pdf)
