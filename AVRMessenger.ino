/***************************************************************************|
|   AVR Messenger - Encrypted Messenger and Games.                          |
|   Copyright (C) 2016  TransportLayer                                      |
|                                                                           |
|   This program is free software: you can redistribute it and/or modify    |
|   it under the terms of the GNU General Public License as published by    |
|   the Free Software Foundation, either version 3 of the License, or       |
|   (at your option) any later version.                                     |
|                                                                           |
|   This program is distributed in the hope that it will be useful,         |
|   but WITHOUT ANY WARRANTY; without even the implied warranty of          |
|   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
|   GNU General Public License for more details.                            |
|                                                                           |
|   You should have received a copy of the GNU General Public License       |
|   along with this program.  If not, see <http://www.gnu.org/licenses/>.   |
|***************************************************************************/

// NOTE TO DEVS: The nRF24L01 has a *REALLY* weird problem with addresses,
// where if the address has an unbalanced number of binary 1s and 0s then ACK
// packets will fail to send. Having no more than 60% of either seems to work.
// The function isGoodAddress() should help prevent errors.

#include "tl16x2.h"
#include "RF24.h"
#include "Chatpad.h"
#include <SoftwareSerial.h>
#include <extEEPROM.h>
#include <AESLib.h>

const char FWVERSION[] = "1.0.0";

// Dynamically updated settings.
/*
bool useEEPROM = false;		// User-modified.
bool useExtMem = false;		// User-modified.
bool extMemConnected = false;	// Updated in firmware.
*/

/*
const uint8_t vibratorPin = 14;		// Probably should use a transistor/MOSFET.
const uint8_t speakerPin = 15;		// Probably should use a transistor/MOSFET.
const uint8_t ledPin = 16;
*/

// nRF24L01+ on SPI bus and PB1 & SS (CE, CS).
RF24 radio(PB1, SS);

// Xbox 360-compatible ChatPad on PC2 & PC3.
// !!!USE A LEVEL SHIFTER FOR 5V -> 3.3V LOGIC!!!
/*
Chatpad pad;
SoftwareSerial padSerial(PC2, PC3);	// RX, TX
*/

// 256Kb EEPROM on I2C bus.
/*
extEEPROM extMem(kbits_256, 1, 64);
*/

/*
const bool isMemFormatted(bool external = false) {
	if (external) {
		char id[13];
		extMem.read(0, id, 12);
		return id == "AVRMessenger";
	} else {
		
	}
}
*/

/*
const uint8_t testExtMem(void) {
	const uint8_t error = extMem.begin(twiClock400kHz);
	switch (error) {
		case 0:
			// No error - The EEPROM is working and ready.
			extMemConnected = true;
			break;
		case 1:
			// Data is too long.
			// This should never happen.
			break;
		case 2:
			// We got a NACK on address transmission.
			// This usually means that the EEPROM is not connected.
			extMemConnected = false;
			break;
		case 3:
			// We got a NACK on data transmission.
			// This should never happen.
			break;
		case 4:
			// Unknown error.
			break;
	}
	return error;
}
*/

const bool isGoodAddress(const uint64_t address) {
	uint8_t numOnes = 0;
	uint64_t maxMask = 0b1000000000000000000000000000000000000000;	// 1<<40, but we can't do that because max shift 0-31.
	for (uint64_t mask = 1; mask <= maxMask; mask <<= 1) {
		if ((mask & address) == mask) {++numOnes;}
	}
	return ((40 - numOnes) >= 16) && ((40 - numOnes) <= 24);
}

void setup(void) {
	// Get start time.
	const uint8_t bootTime = millis();

	// Start serial at 115200 baud for debugging.
	Serial.begin(115200);
	Serial.print("Initializing firmware ");
	Serial.print(FWVERSION);
	Serial.println("...");

	// We're open-source! This uses a lot of bytes...
	Serial.println("AVR Messenger  Copyright (C) 2016  TransportLayer");
	Serial.println("This firmware is licensed under the GNU General Public License as");
	Serial.println("published by the Free Software Foundation (version 3 or later).");

	// Setup LCD.
	Serial.println("Initializing LCD...");
	initScreen();
	frameBuffer[0][1] = 'A';
	frameBuffer[0][2] = 'V';
	frameBuffer[0][3] = 'R';
	frameBuffer[0][5] = 'M';
	frameBuffer[0][6] = 'e';
	frameBuffer[0][7] = 's';
	frameBuffer[0][8] = 's';
	frameBuffer[0][9] = 'e';
	frameBuffer[0][10] = 'n';
	frameBuffer[0][11] = 'g';
	frameBuffer[0][12] = 'e';
	frameBuffer[0][13] = 'r';
	frameBuffer[1][2] = 'v';
	for (uint8_t i = 0; i < sizeof(FWVERSION) - 1; ++i) {
		frameBuffer[1][i + 3] = FWVERSION[i];
	}
	writeFrameBuffer();

	// Setup radio.
	Serial.println("Initializing radio...");
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);				// Set power level (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm).
	radio.setDataRate(RF24_1MBPS);				// Set data rate (RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps).
	radio.setAutoAck(true);					// Turn on autoACK.
	radio.setRetries(0, 15);				// x+1*250us delay between retries, y number of retries (max 15).
	radio.setPayloadSize(32);				// Use a 32-byte payload.

	/*
	// Setup keyboard.
	Serial.println("Initializing keyboard...");
	// (define callback function and init pad)
	*/

	/*
	// Setup EEPROM.
	Serial.println("Initializing external EEPROM...");
	testExtMem();
	*/

	/*
	// Setup notifiers.
	Serial.println("Initializing notifiers...");
	pinMode(vibratorPin, OUTPUT);
	pinMode(speakerPin, OUTPUT);
	pinMode(ledPin, OUTPUT);
	digitalWrite(vibratorPin, LOW);
	digitalWrite(speakerPin, LOW);
	digitalWrite(ledPin, LOW);
	*/

	// Self-tests.
	Serial.println("Starting self-test...");
	// Radio.
	// (No idea how to test this)
	// Keyboard.
	// (Not yet implemented)
	// Notifiers.
	/*
	digitalWrite(vibratorPin, HIGH);
	tone(speakerPin, 880, 250);
	digitalWrite(ledPin, HIGH);
	delay(250);
	digitalWrite(vibratorPin, LOW);
	noTone(speakerPin);
	digitalWrite(speakerPin, LOW);
	digitalWrite(ledPin, LOW);
	*/
	// Encryption.
	Serial.print("Encryption: ");
	const uint8_t aesTestKey[32];
	const uint8_t aesTestDataTemplate[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t aesTestData[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	aes256_enc_single(aesTestKey, aesTestData);
	aes256_dec_single(aesTestKey, aesTestData);
	for (uint8_t i = 0; i < 32; ++i) {
		if (aesTestData[i] != aesTestDataTemplate[i]) {Serial.println("FAIL"); break;}
		if (i == 31) {Serial.println("PASS");}
	}
	// Address Test.
	Serial.print("Address Tester (OK): ");
	Serial.println(isGoodAddress(0x5555555555) ? "PASS" : "FAIL");
	Serial.print("Address Tester (LO): ");
	Serial.println(isGoodAddress(0x0000000000) ? "FAIL" : "PASS");
	Serial.print("Address Tester (HI): ");
	Serial.println(isGoodAddress(0xFFFFFFFFFF) ? "FAIL" : "PASS");

	// Keep splash screen up for a minimum of 1 second.
	if (millis() - bootTime < 1000) {
		const uint16_t splashDelay = (bootTime + 1000) - millis();
		Serial.print("Showing splash for ");
		Serial.print(splashDelay);
		Serial.println("ms longer.");
		delay(splashDelay);
	}

	// Done initializing.
	clearScreen();
	Serial.println("Done.");
}

void loop(void) {
	
}
