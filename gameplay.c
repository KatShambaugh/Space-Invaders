#include "spritesheet.h"
#include "myLib.h"
#include "gameplay.h"
#include <stdlib.h>

//Initializes the player, enemies, and bullets
void initializeGame() {
    player.row = 140;
    player.col = 130;
    player.rdel = 1;
    player.cdel = 1;
    player.width = 16;
    player.height = 16;

    //Sets attributes for player sprite 
    shadowOAM[0].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | player.row;
    shadowOAM[0].attr1 = ATTR1_SMALL | player.col;
    shadowOAM[0].attr2 = ATTR2_TILEID(0, 0);

    for(int i = 0; i < ENEMYCOUNT; i++) {
        enemies[i].row = 5;
        enemies[i].col = 10;
        enemies[i].rdel = 1;
        enemies[i].cdel = 1;
        enemies[i].active = 1;
        enemies[i].width = 8;
        enemies[i].height = 8;
    }

    //Sets attributes for enemy sprites
    int rowOffset = 15;
    int idOffset = 0;
    for (int i = 1; i < ENEMYCOUNT; i++) {
        shadowOAM[i].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | (5 + rowOffset);
        shadowOAM[i].attr1 = ATTR1_SMALL | 5 * i;
        shadowOAM[i].attr2 = ATTR2_TILEID(0, 8 + idOffset);
        if ((i == 9) || (i == 19) || (i == 29) || (i == 39) || (i == 49)) {
            rowOffset = rowOffset + 15;
            idOffset = idOffset + 8;
        }
    }

    for(int i = 0; i < BULLETCOUNT; i++) {
        bullets[i].row = 150;
        bullets[i].col = 70;
        bullets[i].speed = 2;
        bullets[i].active = 0;
        bullets[i].width = 16;
        bullets[i].height = 16;
    }

    enemiesLeft = ENEMYCOUNT - 1;
    livesLeft = 3;

    //Sets attributes for life sprites
    for (int i = 0; i < livesLeft; i++) {
        shadowOAM[ENEMYCOUNT + 2 + i].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | 0;
        shadowOAM[ENEMYCOUNT + 2 + i].attr1 = ATTR1_SMALL | 17 * i;
        shadowOAM[ENEMYCOUNT + 2 + i].attr2 = ATTR2_TILEID(0, 48);
    }
}

//Updates the game every cycle
void updateGame() {
    updatePlayer();
    updateEnemies(); 
    updateBullets();
	waitForVBlank();
}

//Handles player controls
void updatePlayer() {
	if ((BUTTON_HELD(BUTTON_LEFT)) && (player.col >= player.cdel)) {
        player.col--;
        shadowOAM[0].attr1 = ATTR1_SMALL | player.col;
    } else if ((BUTTON_HELD(BUTTON_RIGHT)) && (player.col + player.width - 1 < SCREENWIDTH - player.cdel)) {
        player.col++;
        shadowOAM[0].attr1 = ATTR1_SMALL | player.col;
    } else if (BUTTON_HELD(BUTTON_A)) {
        fireBullet();
    }
}

//Updates all of the enemies, controlling their movement
void updateEnemies() {
	//Moves enemies down if they hit the wall
    if((enemies[9].col > 70) | (enemies[0].col < 10)) {
        for(int i = 0; i < ENEMYCOUNT; i++) {
            enemies[i].cdel = -enemies[i].cdel;
            enemies[i].row += enemies[i].rdel;
        }
    }
    
    //Moves the enemies back and forth
    for(int i = 0; i < ENEMYCOUNT; i++) {
        enemies[i].col += enemies[i].cdel;
    }

    //Runs very simple animation of second row of enemy sprites
    int randNumber = rand() % 8;
    if (randNumber == 0) {
    	for (int i = 9; i < 19; i++) {
    		shadowOAM[i + 1].attr2 = ATTR2_TILEID(0, 16);
    	}
    } else {
    	for (int i = 9; i < 19; i++) {
    		shadowOAM[i + 1].attr2 = ATTR2_TILEID(0, 32);
    	}
    }

    //Triggers losing condition
    for(int i = 0; i < ENEMYCOUNT; i++) {
        if(enemies[i].row > 140) {
            enemiesLeft = -1;
        }
    }

    //Handles movement of sprites
    int rowOffset = 15;
    for (int i = 1; i < ENEMYCOUNT; i++) {
    	if (enemies[i].active == 1) {
    		shadowOAM[i].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | (enemies[i].row + rowOffset);
        	shadowOAM[i].attr1 = ATTR1_SMALL | (20 * (i % 9) + enemies[i].col);
    	}
        if ((i == 9) || (i == 19) || (i == 29) || (i == 39) || (i == 49)) {
            rowOffset = rowOffset + 15;
        }
    }

    int randTiming = rand() % 300;
    int enemyOffset = 40;
    if (randTiming == 0) {
        //Initializes enemy bullet
        int randEnemy = rand() % 10;
        if ((enemies[enemyOffset + randEnemy].active == 0) && (enemyOffset != 0)) {
            enemyOffset = enemyOffset - 10;
        }
        bullets[1].active = 1;
        bullets[1].row = enemies[enemyOffset + randEnemy].row + rowOffset;
        bullets[1].col = rand() % 120 + 10;
    
        shadowOAM[126].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | bullets[1].row;
        shadowOAM[126].attr1 = ATTR1_SMALL | bullets[1].col;
        shadowOAM[126].attr2 = ATTR2_TILEID(0, 16);
    }
}

//Handles bullet collision and moves the bullets
void updateBullets() {
    if (bullets[0].active == 1) {
        //Move the player bullet up
        bullets[0].row -= bullets[0].speed;
        shadowOAM[127].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | bullets[0].row;
        shadowOAM[127].attr1 = ATTR1_SMALL | player.col;
        shadowOAM[127].attr2 = ATTR2_TILEID(0, 56);

        //Check for bullet-enemy collisions
        int rowOffset = 10;
        for(int j = 1; j < ENEMYCOUNT; j++) {
            if (enemies[j].active == 1) {
                if (collision(bullets[0].row, player.col, 16, 16, enemies[j].row + rowOffset, (17 * (j % 9) + enemies[j].col), 16, 16)) {
                    enemies[j].active = 0;
                    bullets[0].active = 0;
                    shadowOAM[127].attr0 = ATTR0_HIDE;
                    shadowOAM[j].attr0 = ATTR0_HIDE;
                    enemiesLeft--;
                }
                if ((j == 9) || (j == 19) || (j == 29) || (j == 39) || (j == 49)) {
                    rowOffset = rowOffset + 10;
                }
            }
        }

        //Hide bullets that go off the screen
        if (bullets[0].row < 2) {
            bullets[0].row = 2;
            bullets[0].active = 0;
            shadowOAM[127].attr0 = ATTR0_HIDE;
        }
    }

    if (bullets[1].active == 1) {
        //Move the enemy bullet down
        bullets[1].row += bullets[1].speed;
        shadowOAM[126].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | bullets[1].row;
    	shadowOAM[126].attr1 = ATTR1_SMALL | bullets[1].col;
    	shadowOAM[126].attr2 = ATTR2_TILEID(0, 56);

        //Check for bullet-player collisions
        if ((bullets[1].active == 1) && (collision(bullets[1].row, bullets[1].col, bullets[1].height, bullets[1].width, player.row, player.col, player.height, player.width))) {
            bullets[1].active = 0;
            livesLeft--;
            shadowOAM[126].attr0 = ATTR0_HIDE | 0;
            if (livesLeft == 0) {
                shadowOAM[0].attr0 = ATTR0_HIDE;
            }
            shadowOAM[ENEMYCOUNT + 2 + livesLeft].attr0 = ATTR0_HIDE;
        }

        //Hide bullets that go off the screen
        if(bullets[1].row > 150) {
            bullets[1].row = 160;
            bullets[1].active = 0;
            shadowOAM[126].attr0 = ATTR0_HIDE;
        }
    }
    DMANow(3, shadowOAM, OAM, 512);
}

//Handles bullet firing (initializes bullets to be fired)
void fireBullet() {
	//Initializes player bullet
    bullets[0].active = 1;
    bullets[0].row = 140;
    bullets[0].col = player.col + 6;
    
    shadowOAM[127].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | bullets[0].row;
    shadowOAM[127].attr1 = ATTR1_SMALL | bullets[0].col;
    shadowOAM[127].attr2 = ATTR2_TILEID(0, 16);

    /*//Initializes enemy bullet
    int randEnemy = rand() % enemiesLeft;
    bullets[1].active = 1;
    bullets[1].row = enemies[20].row;
    bullets[1].col = 0;
    
    shadowOAM[126].attr0 = ATTR0_REGULAR | ATTR0_8BPP | ATTR0_SQUARE | bullets[1].row;
    shadowOAM[126].attr1 = ATTR1_SMALL | bullets[1].col;
    shadowOAM[126].attr2 = ATTR2_TILEID(0, 16);
    */
}
