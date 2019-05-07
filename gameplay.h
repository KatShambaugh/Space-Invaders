//Constants
#define ENEMYCOUNT 50
#define BULLETCOUNT 2

OBJ_ATTR shadowOAM[128];

int enemiesLeft;
int livesLeft;

//Enemy Struct
typedef struct {
    int row;
    int col;
    int rdel;
    int cdel;
    int active;
    int height;
    int width;
} ENEMY;

//Player struct
typedef struct {
    int row;
    int col;
    int rdel;
    int cdel;
    int width;
    int height;
} PLAYER;

//Bullet struct
typedef struct {
    int row;
    int col;
    int speed;
    int active;
    int height;
    int width;
} BULLET;

PLAYER player;
ENEMY enemies[ENEMYCOUNT];
BULLET bullets[BULLETCOUNT];

//Prototypes
void initializeGame();
void updateGame();
void updatePlayer();
void fireBullet();
void updateBullets();
void updateEnemies();