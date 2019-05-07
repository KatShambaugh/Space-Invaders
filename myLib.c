#include "myLib.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;

DMA *dma = (DMA *)0x40000B0;

//Waits for VBlank
void waitForVBlank() {
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

//Handles DMA transfer
void DMANow(int channel, volatile const void *src, volatile void *dst, unsigned int cnt) {
    dma[channel].cnt = 0;
	dma[channel].src = src;
    dma[channel].dst = dst;
	dma[channel].cnt = cnt | DMA_ON;
}

//Returns true if collision
int collision(int rowA, int colA, int heightA, int widthA, int rowB, int colB, int heightB, int widthB) {
    return rowA < rowB + heightB - 1 && rowA + heightA - 1 > rowB
        && colA < colB + widthB - 1 && colA + widthA - 1 > colB;
}

//Hides all sprites in OAM
void hideSprites() {
    for (int i = 0; i < 128; i++) {
        shadowOAM[i].attr0 = ATTR0_HIDE;
    }   
}