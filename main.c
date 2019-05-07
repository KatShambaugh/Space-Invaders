#include "myLib.h"
#include "start.h"
#include "game.h"
#include "pause.h"
#include "win.h"
#include "lose.h"
#include "gameplay.h"
#include "spritesheet.h"

//Prototypes
void initialize();
void goToStart();
void start();
void goToGame();
void game();
void goToPause();
void pause();
void goToWin();
void win();
void goToLose();
void lose();

enum {START, GAME, PAUSE, WIN, LOSE};
int state;

unsigned short buttons;
unsigned short oldButtons;

int main() {
	initialize();

	while(1) {
		//State machine
		switch(state) {
            case START:
                start();
                break;
            case GAME:
                game();
                break;
            case PAUSE:
                pause();
                break;
            case WIN:
                win();
                break;
            case LOSE:
                lose();
                break;
        }  

		oldButtons = buttons;
		buttons = BUTTONS;
	}
}

//Initializes states
void initialize() {
	//Load start background
	DMANow(3, startPal, PALETTE, 256);
	DMANow(3, startTiles, &CHARBLOCK[0], 11296);
	DMANow(3, startMap, &SCREENBLOCK[31], 1024);

	buttons = BUTTONS;

	//Load spritesheet
    DMANow(3, spritesheetPal, SPRITEPALETTE, 256);
    DMANow(3, spritesheetTiles, &CHARBLOCK[4], 16384);

    //Set up control register and background control
    REG_DISPCTL = MODE0 | BG1_ENABLE | SPRITE_ENABLE | SPRITE_MODE_1D;
    REG_BG1CNT = BG_SIZE_SMALL | BG_8BPP | BG_CHARBLOCK(0) | BG_SCREENBLOCK(31);
}

//Navigates to start state
void goToStart() {
    state = START;
}

//Sets up start screen
void start() {
	hideSprites();

	waitForVBlank();

	DMANow(3, shadowOAM, OAM, 512);

    if (BUTTON_PRESSED(BUTTON_START)) {
        initializeGame();
		goToGame();
    }
}

//Navigates to game state
void goToGame() {
    state = GAME;
}

//Run game state
void game() {
	//Load game background
	DMANow(3, gamePal, PALETTE, 256);
	DMANow(3, gameTiles, &CHARBLOCK[0], 32);
	DMANow(3, gameMap, &SCREENBLOCK[31], 1024);

    updateGame();

    if (BUTTON_PRESSED(BUTTON_START)) {
        goToPause();
    } else if (enemiesLeft == 0) {
        goToWin();
    } else if ((enemiesLeft == -1) | (livesLeft == 0)) {
        goToLose();
    }
}

//Navigates to pause state
void goToPause() {
    state = PAUSE;
}

//Runs pause state
void pause() {
	//Load pause background
	DMANow(3, pausePal, PALETTE, 256);
	DMANow(3, pauseTiles, &CHARBLOCK[0], 2080);
	DMANow(3, pauseMap, &SCREENBLOCK[31], 1024);
    
    waitForVBlank();

    if (BUTTON_PRESSED(BUTTON_START)) {
        goToGame();
    } else if (BUTTON_PRESSED(BUTTON_SELECT)) {
        goToStart();
    }
}

//Navigates to win state
void goToWin() {
    state = WIN;
}

//Runs win state
void win() {
	hideSprites();

	//Load win background
	DMANow(3, winPal, PALETTE, 256);
	DMANow(3, winTiles, &CHARBLOCK[0], 5632);
	DMANow(3, winMap, &SCREENBLOCK[31], 1024);

    waitForVBlank();

    DMANow(3, shadowOAM, OAM, 512);

    if (BUTTON_PRESSED(BUTTON_START)) {
        goToStart();
    }
}

//Navigates to lose state
void goToLose() {
    state = LOSE;
}

//Runs lose state
void lose() {
	hideSprites();

	//Load lose background
	DMANow(3, losePal, PALETTE, 256);
	DMANow(3, loseTiles, &CHARBLOCK[0], 17056);
	DMANow(3, loseMap, &SCREENBLOCK[31], 1024);

    waitForVBlank();

    DMANow(3, shadowOAM, OAM, 512);

    if (BUTTON_PRESSED(BUTTON_START)) {
        goToStart();
    }
}
