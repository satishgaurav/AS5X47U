/** @file AS5247Spi.cpp
 *
 * @brief A library for Arduino boards that reads angles from AS5047 and AS5147 sensors.
 * 		  Also support configuration of the sensor parameters.
 *
 * @par
 * COPYRIGHT NOTICE: MIT License
 *
 * 	Copyright (c) 2020 Adrien Legrand <contact@adrien-legrand.com>
 *
 * 	Permission is hereby granted, free of charge, to any person obtaining a copy
 * 	of this software and associated documentation files (the "Software"), to deal
 * 	in the Software without restriction, including without limitation the rights
 * 	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * 	copies of the Software, and to permit persons to whom the Software is
 * 	furnished to do so, subject to the following conditions:
 *
 * 	The above copyright notice and this permission notice shall be included in all
 * 	copies or substantial portions of the Software.
 *
 * 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * 	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * 	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * 	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * 	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * 	SOFTWARE.
 *
*/

#include "AS5247Spi.h"

AS5247Spi::AS5247Spi(uint8_t _chipSelectPin) {
	// Initialize SPI Communication
	chipSelectPin = _chipSelectPin;
	pinMode(chipSelectPin, OUTPUT);
	digitalWrite(chipSelectPin, HIGH);
	SPI.begin();
}


// void AS5247Spi::writeData(uint32_t command, uint32_t value) {
// 	// @todo Expose the SPI Maximum Frequency in library interface.
// 	SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE1));
// 	// Send command
// 	digitalWrite(chipSelectPin, LOW);
// 	SPI.transfer(command);
// 	digitalWrite(chipSelectPin, HIGH);
// 	delayMicroseconds(1);
// 	// Read data
// 	digitalWrite(chipSelectPin, LOW);
// 	SPI.transfer(value);
// 	digitalWrite(chipSelectPin, HIGH);
// 	SPI.endTransaction();
// 	delayMicroseconds(1);

// }

// uint32_t AS5247Spi::readData(uint32_t command, uint32_t nopCommand) {
// 	SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE1));

// 	// Send Read Command
// 	digitalWrite(chipSelectPin, LOW);
// 	SPI.transfer(command);

// 	Serial.print("command inside: "); 
// 	Serial.println(command, HEX);

// 	digitalWrite(chipSelectPin, HIGH);
// 	delayMicroseconds(1);
// 	// Send Nop Command while receiving data
// 	digitalWrite(chipSelectPin, LOW);
// 	uint32_t receivedData = SPI.transfer(nopCommand);
// 	digitalWrite(chipSelectPin, HIGH);
// 	SPI.endTransaction();
// 	delayMicroseconds(1);
// 	return receivedData;
// }


uint32_t AS5247Spi::readData(uint32_t command, uint32_t nopCommand) {
	SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE1));

	// Send Read Command
	digitalWrite(chipSelectPin, LOW);

	// send 16 bits data two times 
	SPI.transfer16(command >> 16);
	SPI.transfer16(command); 

	digitalWrite(chipSelectPin, HIGH);
	delayMicroseconds(1);

	// Send Nop Command while receiving data
	digitalWrite(chipSelectPin, LOW);

	uint32_t receivedData1 = SPI.transfer16(0x0000);
	uint32_t receivedData2 = SPI.transfer16(0x0000);

	Serial.print("receivedData1: "); 
	Serial.println(receivedData1, BIN); 

	Serial.print("receivedData2: "); 
	Serial.println(receivedData2, BIN); 

	// merge data for two times 
	uint32_t receivedData = (receivedData1 << 16) + receivedData2; 

	digitalWrite(chipSelectPin, HIGH);
	SPI.endTransaction();
	delayMicroseconds(1);

	byte bytes[4]; 
	bytes[0] = (receivedData >> 0)  & 0xFF;
	bytes[1] = (receivedData >> 8)  & 0xFF;
	bytes[2] = (receivedData >> 16) & 0xFF;
	bytes[3] = (receivedData >> 24) & 0xFF;

	for (int i = sizeof(bytes)-1; i >= 0; i--){
		for(int j=7; j >= 0; j--){
            Serial.print(bitRead(bytes[i], j)); 
        }
        Serial.println();
    } 
    Serial.println(); 
	return receivedData;
}

uint16_t AS5247Spi::readData16(uint16_t command, uint16_t nopCommand) {
	SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE1));

	// for testing only 
	// command = 0x7FF5; 

	// Send Read Command
	digitalWrite(chipSelectPin, LOW);
	// SPI.transfer16(command);

	uint8_t a = command >> 8; 
	uint8_t b = command;

	Serial.print("a: "); 
	Serial.println(a, HEX);
	Serial.print("b: "); 
	Serial.println(b, HEX);


	SPI.transfer(a);
	SPI.transfer(b);

	Serial.print("command inside: "); 
	Serial.println(command, HEX);

	digitalWrite(chipSelectPin, HIGH);
	delayMicroseconds(1);
	// Send Nop Command while receiving data
	digitalWrite(chipSelectPin, LOW);


	// uint16_t receivedData = SPI.transfer16(0x00);

	// uint8_t receivedData1 = SPI.transfer16(0x0000);
	// uint8_t receivedData2 = SPI.transfer16(0x0000);

	uint8_t receivedData1 = SPI.transfer(0x00);
	uint8_t receivedData2 = SPI.transfer(0x00);

	uint16_t receivedData = (receivedData1 << 8)  + receivedData2; 

	Serial.print("receivedData1: "); 
	Serial.println(receivedData1, BIN); 
	Serial.print("receivedData2: "); 
	Serial.println(receivedData2, BIN); 

	// uint16_t receivedData = (receivedData2 << 8)  + receivedData1; 

	digitalWrite(chipSelectPin, HIGH);
	SPI.endTransaction();
	delayMicroseconds(1);

	byte bytes[2]; 
	bytes[0] = (receivedData >> 0)  & 0xFF;
	bytes[1] = (receivedData >> 8)  & 0xFF;

	// for (int i = 0; i < sizeof(bytes); i++){
	for (int i = sizeof(bytes)-1; i >= 0; i--){
        // Serial.print(commandFramePacket.CommandFramePacket[i], BIN);
        // for(int j=0; j < 8; j++){
        //     Serial.print(bitRead(bytes[i], j)); 
        // }

		for(int j=7; j >= 0; j--){
            Serial.print(bitRead(bytes[i], j)); 
        }
        // Serial.println();
    } 
    Serial.println(); 

	return receivedData;
}

uint8_t AS5247Spi::CRC8(byte byteArray[], int size){


    for(int i = 0; i < size; i++){
        Serial.println(byteArray[i], HEX); 
    }

    // Polynomial 
    const byte polynomial = 0x1D;
    // initial value 
    byte crc = 0xC4;  
    
    // calculate crc 
    for(int i = 0; i < size; i++){
        crc ^= byteArray[i]; 
        for(int j = 0; j < 8; j++){
            if((crc & 0x80) != 0){
                crc = (byte)((crc << 1) ^ polynomial); 
            } else {
                crc <<= 1; 
            }
        }
    }
    // final XOR  
    crc &= 0xFF;
    crc ^= 0xFF; 
    return crc; 

}