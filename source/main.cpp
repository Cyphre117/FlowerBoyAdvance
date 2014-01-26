#include <stdint.h>
#include <stdlib.h>
#include "gba.h"

#define KEY_MASK 0x03FF
#define KEYS_LEFTRIGHT	(3 << 4) // 0b0000000000110000
#define KEYS_DOWNUP		(3 << 6) // 0b0000000011000000

uint16_t keyCurrent, keyPrevious;

	// Get the current and previous key states
inline void keyPoll() {
	keyPrevious = keyCurrent;
	keyCurrent = REG_KEYINPUT;
}

void flower(uint8_t reference);
void cursorUpdate();
void cursorDraw();
void updateFlowers();
void drawGrass(uint8_t colour);
void testPalette(uint8_t line, uint8_t start, uint8_t length);

uint8_t something[16];

uint8_t xCursor = SCREEN_WIDTH/2;
uint8_t yCursor = SCREEN_HEIGHT/2;

uint8_t drawnFlowers	= 1;
uint8_t range			= 4;

uint8_t xTrails[12];
uint8_t yTrails[12];

const uint8_t bg		= 0;
const uint8_t red		= 1;
const uint8_t green		= 2;
const uint8_t pink		= 3;
const uint8_t white		= 4;
const uint8_t yellow	= 5;
const uint8_t blue		= 6;
const uint8_t grass 	= 7;

int main() { // game launch

    // Put the display into bitmap mode 4, and enable background 2.
	REG_DISPCNT = MODE4 | BG2_ENABLE;

	SetPaletteBG(blue,	RGB( 5, 5,31));
	SetPaletteBG(red, 	RGB(31, 0, 0));
	SetPaletteBG(green, RGB( 0,31, 0));
	SetPaletteBG(pink, 	RGB(20, 0,31));
	SetPaletteBG(white, RGB(31,31,31));
	SetPaletteBG(yellow,RGB(21,21, 0));
	SetPaletteBG(bg, 	RGB( 0, 4, 0));
	SetPaletteBG(grass, RGB( 0, 7, 0));

	// visual burst around flower, flower fades, petal remains

	while (true) { // splash screen, waiting for A button
		keyPoll();
		SetPaletteBG(bg, RGB(0,0,0)); // make the background black

		ClearScreen8(bg); 
		PlotPixel8(xCursor,yCursor,white);

		WaitVSync();
		FlipBuffers();

		//draw a white flower

		if ((keyCurrent & KEY_A) == 0) break;
	}


	for (uint8_t i = 0; i <= 7; ++i) { // game startup animation
		
			// draw to screen
		ClearScreen8(bg);
		drawGrass(grass);
		PlotPixel8(xCursor,yCursor,white);

			// flip buffers
		WaitVSync();
		WaitVSync();
		WaitVSync();
		WaitVSync();
		WaitVSync();
		WaitVSync();
		FlipBuffers();

			// increment colours
		SetPaletteBG(grass, RGB(0,i,0)); // grass fades in
		if (i > 4) continue;
		SetPaletteBG(bg, RGB(0,i,0)); // background fades in
	}

	while (true) { // main game loop

		keyPoll();			// Get the keys!
		ClearScreen8(bg);	// Clear the screen
		drawGrass(grass);	// draw random dots that are supposed to represent grass

		// draw flowers + update flowers
		srand(12);
		flower(1);
		flower(2);
		flower(3);
		flower(4);
		flower(5);
		flower(6);
		flower(7);
		flower(8);
		flower(9);
		flower(10);
		flower(11);
		flower(12);

		// cursor drawing and updating
		cursorUpdate();
		cursorDraw();

		WaitVSync(); 				// Wait till verticle blanking period
		FlipBuffers(); // Tell the screen to draw from the back buffer then flip the buffers
	}

	return 0;
}

void drawGrass(uint8_t colour) {
	srand(0);					// starts with the same seed, regenerates the coords each cycle (because i couldn't get arrays working)
	for (uint8_t i = 0; i < 30; ++i) {
		PlotPixel8(rand()%SCREEN_WIDTH,rand()%SCREEN_HEIGHT,colour);
	}
}

void flower(uint8_t reference) {

	if (reference > drawnFlowers) return; // the flower has not yet grown

	uint8_t x = rand() % SCREEN_WIDTH;
	uint8_t y = rand() % SCREEN_HEIGHT;

	if (reference == drawnFlowers) { // it's the newest flower (still a bud)

		PlotPixel8(x ,y ,green );

		if 	(
					(range * range)  > (abs(xCursor - x) * abs(xCursor - x)) + (abs(yCursor - y) * abs(yCursor - y))
			)	{
			++drawnFlowers;
		}
	}

	if (reference < drawnFlowers) { // the flower has bloomed

		// draw petals
		PlotPixel8(x	,y-1	,reference%6+1);
		PlotPixel8(x	,y+1	,reference%6+1);
		PlotPixel8(x-1	,y		,reference%6+1);
		PlotPixel8(x+1	,y		,reference%6+1);

		// draw centre
		PlotPixel8(x	,y		,yellow  );
	}
}

void cursorUpdate() {

	for (int8_t i = drawnFlowers-2; i > 0; --i) {
		xTrails[i] = xTrails[i-1];
		yTrails[i] = yTrails[i-1];
	}

		xTrails[0] = xCursor;
		yTrails[0] = yCursor;

	if ( (keyCurrent & KEY_RIGHT) == 0 ) {
		xCursor += 1;
	}

	if ( (keyCurrent & KEY_LEFT) == 0 ) {
		xCursor -= 1;
	}

	if ( (keyCurrent & KEY_UP) == 0 ) {
		yCursor -= 1;
	}

	if ( (keyCurrent & KEY_DOWN) == 0 ) {
		yCursor += 1;
	}

	if (xCursor >= SCREEN_WIDTH) {
		if (xTrails[0] > 100) {
			xCursor = 0;
		} else {
			xCursor = SCREEN_WIDTH-1;
		}
	}

	if (yCursor >= SCREEN_HEIGHT-1) {
		if (yTrails[0] > 100) {
			yCursor = 0;
		} else {
			yCursor = SCREEN_HEIGHT-2;
		}
	}

}

void cursorDraw() {
	for (int8_t i = drawnFlowers-2; i >= 0; --i) {
		PlotPixel8(xTrails[i],yTrails[i], ((i+1)%6)+1);
	}
	PlotPixel8(xCursor,yCursor,white);
}

