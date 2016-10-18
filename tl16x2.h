#include <LiquidCrystal.h>

#define COLS 16
#define ROWS 2

//LiquidCrystal lcd(PD3, PD4, PD5, PD6, PD7, PB0);	// RS, Enable, D4, D5, D6, D7
// Using the port numbers don't work for some reason... Use Arduino pins instead.
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);			// RS, Enable, D4, D5, D6, D7

char frameBuffer[ROWS][COLS];	// [Rows][Columns]
uint8_t cursorPos[2];		// {x, y}

const void moveCursorY(bool up = false) {
	if (cursorPos[1] == 0) {
		if (up) {
			// Consider adding code to scroll (add a blank line and shift line down)
		} else {
			++cursorPos[1];
		}
	} else if (cursorPos[1] == ROWS - 1) {
		if (up) {
			cursorPos[1] = 0;
		} else {
			// Consider adding code to scroll (add a blank line and shift line up)
		}
	}
}

const void moveCursorX(bool left = false) {
	if (left) {
		if (cursorPos[0] == 0) {
			// Just do nothing I guess?
		} else {
			--cursorPos[0];
		}
	} else {
		if (cursorPos[0] == COLS - 1) {
			// Consider adding code to go to next line
		} else {
			++cursorPos[0];
		}
	}
}

const void writeFrameBuffer(void) {
	for (uint8_t y = 0; y < ROWS; ++y) {
		lcd.setCursor(0, y);
		for (uint8_t x = 0; x < COLS; ++x) {
			lcd.write(frameBuffer[y][x]);
		}
	}
	lcd.setCursor(cursorPos[0], cursorPos[1]);
}

const void clearScreen(void) {
	for (uint8_t y = 0; y < ROWS; ++y) {
		for (uint8_t x = 0; x < COLS; ++x) {
			frameBuffer[y][x] = ' ';
		}
	}
	cursorPos[0] = 0;
	cursorPos[1] = 0;
	lcd.clear();
	lcd.clear();	// Sometimes it doesn't work the first time.
}

const void writeToFrameBuffer(char c) {
	switch (c) {
		case '\n':
			break;
		case '\r':
			moveCursorY(false);
			cursorPos[0] = 0;
			break;
		case '\x7F':
			moveCursorX(true);
			frameBuffer[cursorPos[0]][cursorPos[1]] = ' ';
			break;
		default:
			frameBuffer[cursorPos[0]][cursorPos[1]] = c;
			moveCursorX(false);
			break;
	}
}

const void initScreen(void) {
	lcd.begin(COLS, ROWS);		// Columns, rows.
	lcd.noCursor();
	lcd.noBlink();
	lcd.display();
	lcd.noAutoscroll();
	lcd.leftToRight();
	clearScreen();
}
