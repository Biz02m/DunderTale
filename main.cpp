#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <ctime>
#include <cstdlib>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   560  //default 480. here is screen height + menu height
#define CAM_BOUNDS 200   // if player touches a bound, the whole screen moves
#define LEVEL_WIDTH             1920
#define LEVEL_HEIGHT    1080
#define NUMBER_OF_IMAGES 15
#define NUMBER_OF_PATTERNS 14
#define NUMBER_OF_PLAYER_PATTERNS 4  
#define NUMBER_OF_ENEMY_TYPES 4
#define SLOW_BULLET_SPEED 0.25 //bullet speed defines how fast each bullet moves
#define MEDIUM_BULLET_SPEED 0.5
#define FAST_BULLET_SPEED 2
#define SHOTING_KNOCKBACK 5  //this defines how far the player is set back when a bullet is shot
#define BULLET_DISTANCE 0.8  //this defines the spiral, zigzag, eightshaped patterns distance from center
#define PLAYER_INVINCIBILITY_TIME 2 // this defines for how long player doesnt take dmg after being shot
#define PLAYER_MOVING_SPEED 0.4
#define MENU_BAR_HEIGHT 80 // upper screen menu bar height
#define PLAYER_AMMO 5 //selfexplanatory bruv
#define PLAYER_BULLET_LIFETIME 4 //this defines how long each bullet has untill its erased
#define TIME_BETWEEN_SHOOTING 1 // this defines the time between each player shot, so he cant mash the space bar 
#define NO_AMMO_PENALTY 5 //when player runs out of bullets, he gets a penality. this value has to be greater than player_bullet_lifetime
#define PLAYER_BULLET_DMG 10
#define ENEMY_BULLET_DMG 10
enum directions {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

enum screenType {
    MENU,
    DEATHSCREEN,
    WINSCREEN,
};

enum menuinputoutput { //all sorts of inputs and outputs of interface 
    quitgame,
    startgame,
    nextlevel,
    YES,
    NO,
    standBy,
    chooselevel,
    none,
    gotomenu
};

enum patterns {
    verticalUp, // the position of each pattern is essential!!
    verticalDown,
    horizontalRight,
    horizontalLeft,
    diagonalUpRight,
    diagonalUpLeft,
    diagonalDownRight,
    diagonalDownLeft,
    spiral,
    eight_horizontaly,
    eight_verticaly,
    zigzag,
    zigzag2,
    circle,
    specialBomb,
    specialBigBullet,

};

enum images {
    eti,
    charset,
    heart_up,
    heart_down,
    heart_left,
    heart_right,
    flowey,
    bullet,
    doggo,
    Bullet_up,
    Bullet_right,
    Bullet_down,
    Bullet_left,
    sans,
    dundertaleLogo,
};

enum objectTypes {
    floweyObject,
    sansObject,
    annoyingDogObject,
    playerObject,
    bulletObject,
};


//ALL THE NUMBERS DECLARED BELOW ARE TEMPORARY AND DEFAULT, THEY ARE ALL CHANGED THROUGHOUT RUNNING
//the reason why these are set to some values is to make vs shut up with warnings
struct eachBullet {
    float x = LEVEL_WIDTH / 2;
    float y = LEVEL_HEIGHT / 2;
    int hasBeenShot = NO;
    double lifeTime = 0;
    int posession = bulletObject;
};

struct BulletObject {
    int pattern = verticalUp;
    int numberOfBullets = 0;
    int numberOfShotBullets = 0;
    int objWidth = 40; //40 is the width in pixels of the player heart 
    int objHeight = 40;
    eachBullet* Bullet;
};

struct ObjectOnScreen {
    float x = LEVEL_WIDTH / 2;
    float y = LEVEL_HEIGHT / 2;
    double health = 100;
    double invincibilityTime = 0;
    double timeBetweenShooting = 0;
    double noAmmoTime = 0;
    int fullscreen = NO;
    int score = 0;
    int noAmmo = NO;
    int shotTaken = NO;
    int wasHit = NO;
    int wasHitTimes = 0;
    int direction = UP;
    int numberOfBullets = 0;
    int numberOfShotBullets = 0;
    int objWidth = 40; //40 is the width in pixels of the player heart 
    int objHeight = 40;
    int chosenLevel = floweyObject; //by default

    //enemy additional info
    int lastBulletPattern = verticalUp;
    double shootSpecialPatternTime = 0;
    int numberOfPatterns = 0;
    int name = 0; //by default
    int movable = NO;
    float cooldown = 0; //cooldown represents the frequency between bullets released by boss

    BulletObject playerBullets[NUMBER_OF_PLAYER_PATTERNS];
};

struct view {
    //the center of screen position on level
    int Xcenter = LEVEL_WIDTH / 2;
    int Ycenter = LEVEL_HEIGHT / 2;
    //the position of player boundaries, if player touches a boundary, the screen moves
    int x1 = Xcenter - SCREEN_WIDTH / 2 + CAM_BOUNDS;
    int y1 = Ycenter - (SCREEN_HEIGHT + MENU_BAR_HEIGHT) / 2 + CAM_BOUNDS;
    int x2 = Xcenter + SCREEN_WIDTH / 2 - CAM_BOUNDS;
    int y2 = Ycenter + (SCREEN_HEIGHT + MENU_BAR_HEIGHT) / 2 - CAM_BOUNDS;

};
struct colors {
    int black;
    int green;
    int red;
    int blue;
    int white;
};
struct camera {
    int x;
    int y;
};
/*--------------------SDL-FUNCTIONS--------------------*/
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset);
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
void increaseTime(int* t1, int* t2, double* delta, double* worldTime, double* shootTimer, double* fpsTimer, double* fps, int* frames);

void FreeSdlObjects(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer);
void DestroyAllImages(SDL_Surface** images);
bool CheckIfAllPicturesAreLoaded(SDL_Surface** images);
void deleteBullets(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level);

void CheckIfPlayerWentOffVisibleSpace(ObjectOnScreen* player, view* pView, camera* camera);
void UpdatePView(view* pView, camera camera);
void handleEvents(SDL_Event* event, int* quit, int* newgame, ObjectOnScreen* player, int* shoot);

void DrawToScreen(colors color, double worldTime, SDL_Surface* screen, SDL_Surface** images, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, BulletObject* bullets, camera camera, double fps);
int showScreen(SDL_Renderer** renderer, SDL_Surface** screen, colors color, SDL_Texture** scrtex, SDL_Surface** images, int* level, int screenType);
void menu(SDL_Renderer** renderer, SDL_Surface** screen, colors color, SDL_Texture** scrtex, SDL_Surface** images, int* status, int level);
void deathScreen(SDL_Surface** screen, SDL_Surface** images, int* status);
void levelScreen(SDL_Surface** screen, SDL_Surface** images, int* status, int* level);
void winScreen(SDL_Surface** screen, SDL_Surface** images, int* status, int* level);
void DrawUi(SDL_Surface* screen, colors color, SDL_Surface** images, double worldTime, double fps, ObjectOnScreen* player, ObjectOnScreen* enemy, camera camera, int level);
void DrawEnemies(SDL_Surface* screen, SDL_Surface** images, camera camera, ObjectOnScreen* enemy, int level);
void DrawPlayer(SDL_Surface* screen, SDL_Surface** images, camera camera, ObjectOnScreen* player);
void DrawBullets(SDL_Surface* screen, SDL_Surface** images, camera camera, BulletObject* bullets, ObjectOnScreen* enemy, int level, ObjectOnScreen player);

void updateVariables(int* t1, int* frames, double* fpsTimer, double* fps, int* quit, int* newgame,
    double* worldTime, double* shootTimer, ObjectOnScreen* player, ObjectOnScreen* enemy,
    BulletObject* bullets, int level, camera* camera, view* pView);

void initBullets(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level);
void initEnemies(ObjectOnScreen* enemy);

void CalcPositionOfBullets(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, double delta, double* shootTimer); // this calculates the next position of bullet
void CalcPositionOfEnemy(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, double worldTime, double delta); // this calculates the next position of enemy
void CalcPositionOfPlayerBullets(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, double delta); // this calculates the next position of playerBullet
void checkStateOfBullet(BulletObject* bullets, int bulletType, int ithBullet, ObjectOnScreen* enemy, int level, double* shootTimer); // this checks if bullet went off level
void checkHitBox(BulletObject* bullets, ObjectOnScreen* player, int bulletType, int ithBullet, double delta, ObjectOnScreen* enemy, int level, int posession);
void getObjectPositions(BulletObject* bullets, ObjectOnScreen* player, int bulletType, int ithBullet, ObjectOnScreen* enemy, int level, int posession, int* Bx1, int* Bx2, int* By1, int* By2, int* Px1, int* Px2, int* Py1, int* Py2);
void shootBullet(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, double* shootTimer, int* shoot, double delta);
void fillInBulletsPosition(BulletObject* bullets, ObjectOnScreen* enemy, int level);

// main
#ifdef __cplusplus
extern "C"
#endif
/*---------------------------------------------------------------------------------------MAIN-----------------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char** argv) {
    //initialize all thing and setup all things 
    int t1, t2, quit, newgame, frames, rc, play, shoot, level = floweyObject;
    double delta, worldTime, shootTimer, fpsTimer, fps;  //world time counts time since start of game, shoot time remembers last shooting
    ObjectOnScreen player;
    ObjectOnScreen enemy[NUMBER_OF_ENEMY_TYPES];
    initEnemies(enemy);
    BulletObject bullets[NUMBER_OF_PATTERNS];
    initBullets(bullets, &player, enemy, sansObject); //sansObject is here temporarily because he has all the bullets, he is then replaced with desired boss
    view pView;
    colors color;

    SDL_Surface* images[NUMBER_OF_IMAGES];
    SDL_Event event;
    SDL_Surface* screen;
    SDL_Texture* scrtex;
    SDL_Window* window;
    SDL_Renderer* renderer;
    camera camera = { LEVEL_WIDTH - SCREEN_WIDTH / 2, LEVEL_HEIGHT - SCREEN_HEIGHT / 2 };

    srand(time(NULL)); //unused feature, can be turned on manually

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    // fullscreen mode, must be set manually
    if (player.fullscreen == YES) {
        rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
    }
    else {
        rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    }
    if (rc != 0) {
        SDL_Quit();
        printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
        return 1;
    };

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");
    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_ShowCursor(SDL_DISABLE);

    // load images, check if all pictures were loaded
    images[charset] = SDL_LoadBMP("./cs8x8.bmp");
    images[eti] = SDL_LoadBMP("./eti.bmp");
    images[heart_up] = SDL_LoadBMP("./hearts/heart_up.bmp");
    images[heart_down] = SDL_LoadBMP("./hearts/heart_down.bmp");
    images[heart_right] = SDL_LoadBMP("./hearts/heart_right.bmp");
    images[heart_left] = SDL_LoadBMP("./hearts/heart_left.bmp");
    images[flowey] = SDL_LoadBMP("./flowey.bmp");
    images[bullet] = SDL_LoadBMP("./bullet.bmp");
    images[doggo] = SDL_LoadBMP("./doggo.bmp");
    images[Bullet_up] = SDL_LoadBMP("./Bullet_up.bmp");
    images[Bullet_right] = SDL_LoadBMP("./Bullet_right.bmp");
    images[Bullet_down] = SDL_LoadBMP("./Bullet_down.bmp");
    images[Bullet_left] = SDL_LoadBMP("./Bullet_left.bmp");
    images[sans] = SDL_LoadBMP("./sans.bmp");
    images[dundertaleLogo] = SDL_LoadBMP("./dundertaleLogo.bmp");
    if (CheckIfAllPicturesAreLoaded(images)) {
        FreeSdlObjects(screen, scrtex, window, renderer);
        DestroyAllImages(images);
        SDL_Quit();
        return 1;
    }

    SDL_SetColorKey(images[charset], true, 0x000000);
    color.black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
    color.green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
    color.red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
    color.blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
    color.white = SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);

    //Setup ends here, i cant squeze it in another function so it stays here

    //set up variables
    updateVariables(&t1, &frames, &fpsTimer, &fps, &quit, &newgame, &worldTime, &shootTimer, &player, enemy, bullets, level, &camera, &pView);

    play = showScreen(&renderer, &screen, color, &scrtex, images, &level, MENU);
    newgame = 0; // zero means dont start new game and 1 means start new game
    shoot = NO; //if shoot is set to YES it means a player wants to pew pew
    if (play == startgame) {
        while (!quit) {
            //reset all variables, delete bullets and reinit them
            updateVariables(&t1, &frames, &fpsTimer, &fps, &quit, &newgame, &worldTime, &shootTimer, &player, enemy, bullets, level, &camera, &pView);
            deleteBullets(bullets, &player, enemy, level);
            initBullets(bullets, &player, enemy, level);
            while (!quit && !newgame) {
                increaseTime(&t1, &t2, &delta, &worldTime, &shootTimer, &fpsTimer, &fps, &frames);

                SDL_RenderClear(renderer);
                //start putting things on screen
                SDL_FillRect(screen, NULL, color.black);
                DrawToScreen(color, worldTime, screen, images, &player, enemy, level, bullets, camera, fps);
                SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
                SDL_RenderCopy(renderer, scrtex, NULL, NULL);
                SDL_RenderPresent(renderer);

                // handling of events (if there were any)
                handleEvents(&event, &quit, &newgame, &player, &shoot); //and also execute them

                //calculate position of bullets, enemies etc
                shootBullet(bullets, &player, enemy, level, &shootTimer, &shoot, delta);
                CheckIfPlayerWentOffVisibleSpace(&player, &pView, &camera);
                CalcPositionOfBullets(bullets, &player, enemy, level, delta, &shootTimer);
                CalcPositionOfEnemy(bullets, &player, enemy, level, worldTime, delta);
                CalcPositionOfPlayerBullets(bullets, &player, enemy, level, delta);
                UpdatePView(&pView, camera);


                //check if player won or lost or wants to go to menu after losing or winning
                if (player.health <= 0 && play != gotomenu) {
                    play = showScreen(&renderer, &screen, color, &scrtex, images, &level, DEATHSCREEN);
                    if (play == startgame) {
                        newgame = 1;
                    }
                    else if (play == quitgame) {
                        quit = 1;
                    }
                }
                if (enemy[level].health <= 0 && play != gotomenu) {
                    play = showScreen(&renderer, &screen, color, &scrtex, images, &level, WINSCREEN);
                    if (play == startgame) {
                        newgame = 1;
                    }
                    else if (play == quitgame) {
                        quit = 1;
                    }
                }
                if (play == gotomenu) {
                    play = showScreen(&renderer, &screen, color, &scrtex, images, &level, MENU);
                    if (play == startgame) {
                        newgame = 1;
                    }
                    else if (play == quitgame) {
                        quit = 1;
                    }
                }

                frames++;
                printf("enemy hitboxes coord %d %d %d %d enemy coords %d %d player cords %d %d\r", enemy[level].x - enemy[level].objWidth / 2, enemy[level].y - enemy[level].objHeight / 2, enemy[level].x + enemy[level].objWidth / 2, enemy[level].y + enemy[level].objHeight, enemy[level].x, enemy[level].y, player.x, player.y);
            };
        }
    }

    //freeing all surfaces and arrays, cleaning up
    FreeSdlObjects(screen, scrtex, window, renderer);
    deleteBullets(bullets, &player, enemy, level);
    DestroyAllImages(images);
    SDL_Quit();
    return 0;
};
/*-------------------------------------------------------------------END OF MAIN-----------------------------------------------------------------------*/

void increaseTime(int* t1, int* t2, double* delta, double* worldTime, double* shootTimer, double* fpsTimer, double* fps, int* frames) {
    *t2 = SDL_GetTicks();
    *delta = (*t2 - *t1) * 0.001;
    *t1 = *t2;
    (*worldTime) += (*delta);
    (*shootTimer) += (*delta);
    (*fpsTimer) += (*delta);
    if ((*fpsTimer) > 0.5) {
        (*fps) = (*frames) * 2;
        (*frames) = 0;
        (*fpsTimer) -= 0.5;
    };
}

void initEnemies(ObjectOnScreen* enemy) {
    for (int enemyType = 0; enemyType < NUMBER_OF_ENEMY_TYPES; enemyType++) {
        switch (enemyType) {
        case floweyObject:
            enemy[enemyType].x = LEVEL_WIDTH / 2;
            enemy[enemyType].y = LEVEL_HEIGHT / 2;
            enemy[enemyType].numberOfPatterns = 11; //all the diagonal and x,y axis bullets including spiral, check pattern enum
            enemy[enemyType].objWidth = 40;
            enemy[enemyType].objHeight = 40;
            enemy[enemyType].cooldown = 0.25;
            break;
        case sansObject:
            enemy[enemyType].x = LEVEL_WIDTH / 2;
            enemy[enemyType].y = LEVEL_HEIGHT / 2;
            enemy[enemyType].numberOfPatterns = NUMBER_OF_PATTERNS; //all in baby all patterns
            enemy[enemyType].objWidth = 75;
            enemy[enemyType].objHeight = 80;
            enemy[enemyType].cooldown = 0.5;
            break;
        case annoyingDogObject:
            enemy[enemyType].x = LEVEL_WIDTH / 2;
            enemy[enemyType].y = LEVEL_HEIGHT / 2;
            enemy[enemyType].numberOfPatterns = NUMBER_OF_PATTERNS; //and also he is going to be the bullet
            enemy[enemyType].objWidth = 310;
            enemy[enemyType].objHeight = 330;
            enemy[enemyType].cooldown = 0.25;
            break;
        }
    }
}


void deleteBullets(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level) {
    int bossBulletPatterns = enemy[level].numberOfPatterns;
    for (int bulletType = 0; bulletType < bossBulletPatterns; bulletType++) {
        delete[] bullets[bulletType].Bullet; // necessary for cleanup, boss bullets
        bullets[bulletType].numberOfShotBullets = 0; //unnecessary for cleanup but needed for restarting the game
    }
    //player bullets
    for (int bulletType = 0; bulletType < NUMBER_OF_PLAYER_PATTERNS; bulletType++) {
        delete[] player->playerBullets[bulletType].Bullet;
    }

}

void initBullets(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level) {
    //init boss bullets
    int bossBulletPatterns = enemy[level].numberOfPatterns;
    for (int bulletType = 0; bulletType < bossBulletPatterns; bulletType++) {
        bullets[bulletType].pattern = bulletType;
        switch (bulletType) {
        case verticalUp:
            bullets[bulletType].numberOfBullets = 20; //these values set how many bullet can be sent by a boss, all circular have the same value BUT CAN BE CHANGED, change them all you like
            break;
        case verticalDown:
            bullets[bulletType].numberOfBullets = 20;
            break;
        case horizontalRight:
            bullets[bulletType].numberOfBullets = 20;
            break;
        case horizontalLeft:
            bullets[bulletType].numberOfBullets = 20;
            break;
        case diagonalUpRight:
            bullets[bulletType].numberOfBullets = 20;
            break;
        case diagonalUpLeft:
            bullets[bulletType].numberOfBullets = 20;
            break;
        case diagonalDownRight:
            bullets[bulletType].numberOfBullets = 20;
            break;
        case diagonalDownLeft:
            bullets[bulletType].numberOfBullets = 20;
            break;
        case spiral:
            bullets[bulletType].numberOfBullets = 2;
            if (level == sansObject) {
                bullets[bulletType].numberOfBullets = 15;
            }
            else if (level == annoyingDogObject) {
                bullets[bulletType].numberOfBullets = 25;
            }
            break;
        case zigzag:
            bullets[bulletType].numberOfBullets = 1;
            if (level == sansObject) {
                bullets[bulletType].numberOfBullets = 15;
            }
            else if (level == annoyingDogObject) {
                bullets[bulletType].numberOfBullets = 25;
            }
            break;
        case zigzag2:
            bullets[bulletType].numberOfBullets = 1;
            if (level == sansObject) {
                bullets[bulletType].numberOfBullets = 15;
            }
            else if (level == annoyingDogObject) {
                bullets[bulletType].numberOfBullets = 25;
            }
            break;
        case eight_horizontaly:
            bullets[bulletType].numberOfBullets = 2;
            if (level == sansObject) {
                bullets[bulletType].numberOfBullets = 15;
            }
            else if (level == annoyingDogObject) {
                bullets[bulletType].numberOfBullets = 25;
            }
            break;
        case eight_verticaly:
            bullets[bulletType].numberOfBullets = 2;
            if (level == sansObject) {
                bullets[bulletType].numberOfBullets = 15;
            }
            else if (level == annoyingDogObject) {
                bullets[bulletType].numberOfBullets = 25;
            }
            break;
        case circle:
            bullets[bulletType].numberOfBullets = 2;
            if (level == sansObject) {
                bullets[bulletType].numberOfBullets = 15;
            }
            else if (level == annoyingDogObject) {
                bullets[bulletType].numberOfBullets = 25;
            }
            break;
        }
        bullets[bulletType].Bullet = new eachBullet[bullets[bulletType].numberOfBullets];
    }

    fillInBulletsPosition(bullets, enemy, level); // stupid i know but for some reason the first bullet spawns where it shouldnt

    //initialize player bullets
    for (int bulletType = 0; bulletType < NUMBER_OF_PLAYER_PATTERNS; bulletType++) {
        player->playerBullets[bulletType].pattern = bulletType;
        switch (bulletType) {
        case verticalUp:
            player->playerBullets[bulletType].numberOfBullets = 10;
            break;
        case verticalDown:
            player->playerBullets[bulletType].numberOfBullets = 10;
            break;
        case horizontalRight:
            player->playerBullets[bulletType].numberOfBullets = 10;
            break;
        case horizontalLeft:
            player->playerBullets[bulletType].numberOfBullets = 10;
            break;
        }
        player->playerBullets[bulletType].Bullet = new eachBullet[player->playerBullets[bulletType].numberOfBullets];
    }

}

void fillInBulletsPosition(BulletObject* bullets, ObjectOnScreen* enemy, int level) {
    int bossBulletPatterns = enemy[level].numberOfPatterns;
    for (int bulletType = 0; bulletType < bossBulletPatterns; bulletType++) {
        for (int ithBullet = 0; ithBullet < bullets[bulletType].numberOfBullets; ithBullet++) {
            bullets[bulletType].Bullet[ithBullet].x = LEVEL_WIDTH / 2;
            bullets[bulletType].Bullet[ithBullet].y = LEVEL_HEIGHT / 2;
            bullets[bulletType].Bullet[ithBullet].hasBeenShot = NO;
        }
    }
}


// draw a text txt on surface screen, starting from the point (x, y) charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) {
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8;
    d.h = 8;
    while (*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_BlitSurface(charset, &s, screen, &d);
        x += 8;
        text++;
    };
};

// draw a surface sprite on a surface screen in point (x, y), (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
    SDL_Rect dest;
    dest.x = x - sprite->w / 2;
    dest.y = y - sprite->h / 2;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite, NULL, screen, &dest);
};

// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32*)p = color;
};

// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    for (int i = 0; i < l; i++) {
        DrawPixel(screen, x, y, color);
        x += dx;
        y += dy;
    };
};

// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
    Uint32 outlineColor, Uint32 fillColor) {
    int i;
    DrawLine(screen, x, y, k, 0, 1, outlineColor);
    DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
    DrawLine(screen, x, y, l, 1, 0, outlineColor);
    DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
    for (i = y + 1; i < y + k - 1; i++)
        DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

void FreeSdlObjects(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer) {
    SDL_FreeSurface(screen);
    SDL_DestroyTexture(scrtex);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    printf("\nCleanup Complete, nice job!!\n");
}

void DestroyAllImages(SDL_Surface** images) {
    for (int i = 0; i < NUMBER_OF_IMAGES; i++) {
        SDL_FreeSurface(images[i]);
        printf("picture %d deleted succesfuly", i);
    }
}

bool CheckIfAllPicturesAreLoaded(SDL_Surface** images) {
    for (int i = 0; i < NUMBER_OF_IMAGES; i++) {
        if (images[i] == NULL) {
            printf("waring, picture %d not loaded", i);
            return 1;
        }
        printf("picture %d loaded correctly \n", i);
    }
    return 0;
}

void CheckIfPlayerWentOffVisibleSpace(ObjectOnScreen* player, view* pView, camera* camera) {
    if (player->x < pView->x1) {
        pView->Xcenter--;
        if (pView->Xcenter - (SCREEN_WIDTH / 2) < 0) {
            pView->Xcenter = 0 + (SCREEN_WIDTH / 2);
        }
    }
    if (player->x > pView->x2) {
        pView->Xcenter++;
        if (pView->Xcenter + (SCREEN_WIDTH / 2) > LEVEL_WIDTH) {
            pView->Xcenter = LEVEL_WIDTH - (SCREEN_WIDTH / 2);
        }
    }
    if (player->y < pView->y1) {
        pView->Ycenter--;
        if (pView->Ycenter - (SCREEN_HEIGHT / 2) < 0) {
            pView->Ycenter = 0 + (SCREEN_HEIGHT / 2);
        }
    }
    if (player->y > pView->y2) {
        pView->Ycenter++;
        if (pView->Ycenter + (SCREEN_HEIGHT / 2) > LEVEL_HEIGHT) {
            pView->Ycenter = LEVEL_HEIGHT - (SCREEN_HEIGHT / 2);
        }
    }
    camera->x = (pView->Xcenter - SCREEN_WIDTH / 2);
    camera->y = (pView->Ycenter - SCREEN_HEIGHT / 2);
    if (camera->x < 0) {
        camera->x = 0;
    }
    if (camera->y < 0) {
        camera->y = 0;
    }
    if ((camera->x + SCREEN_WIDTH) > LEVEL_WIDTH) {
        camera->x = LEVEL_WIDTH - SCREEN_WIDTH;
    }
    if ((camera->y + SCREEN_HEIGHT) > LEVEL_HEIGHT) {
        camera->y = LEVEL_HEIGHT - SCREEN_HEIGHT;
    }
}

void UpdatePView(view* pView, camera camera) {
    pView->x1 = pView->Xcenter - SCREEN_WIDTH / 2 + CAM_BOUNDS;
    pView->y1 = pView->Ycenter - SCREEN_HEIGHT / 2 + CAM_BOUNDS;
    pView->x2 = camera.x + SCREEN_WIDTH - CAM_BOUNDS;
    pView->y2 = camera.y + SCREEN_HEIGHT - CAM_BOUNDS;
}



void handleEvents(SDL_Event* event, int* quit, int* newgame, ObjectOnScreen* player, int* shoot) {
    while (SDL_PollEvent(&(*event))) {
        switch (event->type) {
        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_ESCAPE) *quit = 1;
            if (event->key.keysym.sym == SDLK_n) *newgame = 1;
            if (event->key.keysym.sym == SDLK_SPACE && player->shotTaken == NO && (player->numberOfShotBullets < PLAYER_AMMO)) *shoot = YES; //player can either shoot with space or with numpads
            if (event->key.keysym.sym == SDLK_UP && player->shotTaken == NO && (player->numberOfShotBullets < PLAYER_AMMO)) *shoot = YES;
            if (event->key.keysym.sym == SDLK_LEFT && player->shotTaken == NO && (player->numberOfShotBullets < PLAYER_AMMO)) *shoot = YES;
            if (event->key.keysym.sym == SDLK_DOWN && player->shotTaken == NO && (player->numberOfShotBullets < PLAYER_AMMO)) *shoot = YES;
            if (event->key.keysym.sym == SDLK_RIGHT && player->shotTaken == NO && (player->numberOfShotBullets < PLAYER_AMMO)) *shoot = YES;
            break;
        case SDL_KEYUP:
            break;
        case SDL_QUIT:
            *quit = 1;
            break;
        };
    };

    //this could've been done differently but it works which matters the most
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_N]) {
        *newgame = 1;
    }
    if (state[SDL_SCANCODE_A]) {
        player->x -= PLAYER_MOVING_SPEED;
        player->direction = LEFT;
        if (player->x < 0) {
            player->x = 0;
        }
    }
    if (state[SDL_SCANCODE_D]) {
        player->x += PLAYER_MOVING_SPEED;
        player->direction = RIGHT;
        if (player->x > LEVEL_WIDTH) {
            player->x = LEVEL_WIDTH;
        }
    }
    if (state[SDL_SCANCODE_W]) {
        player->y -= PLAYER_MOVING_SPEED;
        player->direction = UP;
        if (player->y < MENU_BAR_HEIGHT + player->objWidth / 2) {
            player->y = MENU_BAR_HEIGHT + player->objHeight / 2;
        }
    }
    if (state[SDL_SCANCODE_S]) {
        player->y += PLAYER_MOVING_SPEED;
        player->direction = DOWN;
        if (player->y > LEVEL_HEIGHT) {
            player->y = LEVEL_HEIGHT;
        }
    }
    if (state[SDL_SCANCODE_UP]) {
        player->direction = UP;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        player->direction = RIGHT;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        player->direction = DOWN;
    }
    if (state[SDL_SCANCODE_LEFT]) {
        player->direction = LEFT;
    }
}

int showScreen(SDL_Renderer** renderer, SDL_Surface** screen, colors color, SDL_Texture** scrtex, SDL_Surface** images, int* level, int screenType) {
    int  status = standBy;
    while (status != quitgame && status != startgame && status != gotomenu) {
        SDL_RenderClear(*renderer);
        //start putting things on screen
        SDL_FillRect(*screen, NULL, color.black);
        DrawRectangle(*screen, 4, 4, SCREEN_WIDTH - 8, SCREEN_HEIGHT - 8, color.red, color.black);

        if (status == chooselevel) {
            levelScreen(screen, images, &status, level);
        }
        else {
            switch (screenType) {
            case MENU:
                menu(renderer, screen, color, scrtex, images, &status, *level);
                break;
            case DEATHSCREEN:
                deathScreen(screen, images, &status);
                break;
            case WINSCREEN:
                winScreen(screen, images, &status, level);
                break;
            }
        }
        SDL_UpdateTexture(*scrtex, NULL, (*screen)->pixels, (*screen)->pitch);
        SDL_RenderCopy(*renderer, *scrtex, NULL, NULL);
        SDL_RenderPresent(*renderer);
    };
    return status;
};

void menu(SDL_Renderer** renderer, SDL_Surface** screen, colors color, SDL_Texture** scrtex, SDL_Surface** images, int* status, int level) {
    char text[128];
    //draw logo
    DrawSurface(*screen, images[dundertaleLogo], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    //draw interface
    sprintf(text, "PRESS N TO PLAY THE GAME");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 240, text, images[charset]);
    switch (level) {
    case floweyObject:
        sprintf(text, "FLOWEY SELECTED");
        DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 260, text, images[charset]);
        break;
    case sansObject:
        sprintf(text, "SANS SELECTED");
        DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 260, text, images[charset]);
        break;
    case annoyingDogObject:
        sprintf(text, "&*#* SELECTED");
        DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 260, text, images[charset]);
        break;
    }
    sprintf(text, "PRESS S TO SELECT LEVEL");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 280, text, images[charset]);
    sprintf(text, "PRESS ESC TO EXIT");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 300, text, images[charset]);

    SDL_Event event;
    while (SDL_PollEvent(&(event))) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) *status = quitgame;
            if (event.key.keysym.sym == SDLK_n) *status = startgame;
            if (event.key.keysym.sym == SDLK_s) *status = chooselevel;

            break;
        case SDL_KEYUP:
            break;
        case SDL_QUIT: *status = quitgame;
            break;
        };
    };
}

void levelScreen(SDL_Surface** screen, SDL_Surface** images, int* status, int* level) {
    char text[128];

    sprintf(text, "PRESS B TO GO BACK");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 240, text, images[charset]);
    sprintf(text, "PRESS 1 TO SELECT FLOWEY - EASY");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 260, text, images[charset]);
    sprintf(text, "PRESS 2 TO SELECT SANS - MEDIUM");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 280, text, images[charset]);
    sprintf(text, "PRESS 3 TO SELECT $#@% - HARD");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 300, text, images[charset]);

    SDL_Event event;
    while (SDL_PollEvent(&(event))) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) { *status = quitgame; }
            if (event.key.keysym.sym == SDLK_b) { *status = standBy; }
            if (event.key.keysym.sym == SDLK_1) { *status = standBy; *level = floweyObject; }
            if (event.key.keysym.sym == SDLK_2) { *status = standBy; *level = sansObject; }
            if (event.key.keysym.sym == SDLK_3) { *status = standBy; *level = annoyingDogObject; }

            break;
        case SDL_KEYUP:
            break;
        case SDL_QUIT: *status = quitgame;
            break;
        };
    };
}

void deathScreen(SDL_Surface** screen, SDL_Surface** images, int* status) {
    char text[128];
    sprintf(text, "YOU HAVE LOST");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 220, text, images[charset]);
    sprintf(text, "WANT TO RETRY?");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 240, text, images[charset]);
    sprintf(text, "Y   N");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 260, text, images[charset]);
    sprintf(text, "WANT TO GO TO MENU PRESS M?");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 280, text, images[charset]);

    SDL_Event event;
    while (SDL_PollEvent(&(event))) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_n) *status = quitgame;
            if (event.key.keysym.sym == SDLK_y) *status = startgame;
            if (event.key.keysym.sym == SDLK_m) *status = gotomenu;

            break;
        case SDL_KEYUP:
            break;
        case SDL_QUIT: *status = quitgame;
            break;
        };
    };
}

void winScreen(SDL_Surface** screen, SDL_Surface** images, int* status, int* level) {
    char text[128];

    sprintf(text, "YOU HAVE WON");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 220, text, images[charset]);
    sprintf(text, "WANT TO GO TO NEXT LEVEL?");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 240, text, images[charset]);
    switch (*level) {
    case floweyObject:
        sprintf(text, "SANS - MEDIUM");
        DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 260, text, images[charset]);
        break;
    case sansObject:
        sprintf(text, "&*$# - HARD");
        DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 260, text, images[charset]);
        break;
    case annoyingDogObject:
        sprintf(text, "Oh, it seems there is no next level... i think. Nice...");
        DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 260, text, images[charset]);
        break;
    }
    if (*level != annoyingDogObject) {
        sprintf(text, "Y   N");
        DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 280, text, images[charset]);
    }
    sprintf(text, "WANT TO GO TO MENU? PRESS M");
    DrawString(*screen, (*screen)->w / 2 - strlen(text) * 8 / 2, 300, text, images[charset]);

    SDL_Event event;
    while (SDL_PollEvent(&(event))) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_n) *status = quitgame;
            if (event.key.keysym.sym == SDLK_y && *level != annoyingDogObject) { *status = startgame; (*level)++; }
            if (event.key.keysym.sym == SDLK_m) *status = gotomenu;

            break;
        case SDL_KEYUP:
            break;
        case SDL_QUIT: *status = quitgame;
            break;
        };
    };
}

void updateVariables(int* t1, int* frames, double* fpsTimer, double* fps, int* quit, int* newgame,
    double* worldTime, double* shootTimer, ObjectOnScreen* player, ObjectOnScreen* enemy,
    BulletObject* bullets, int level, camera* camera, view* pView) {
    *t1 = SDL_GetTicks();
    *frames = 0;
    *fpsTimer = 0;
    *fps = 0;
    *quit = 0;
    *newgame = 0;
    *worldTime = 0;
    *shootTimer = 0;
    player->health = 100;
    player->wasHit = NO;
    player->invincibilityTime = 0;
    player->numberOfShotBullets = 0;
    player->noAmmo = NO;
    player->noAmmoTime = 0;
    player->x = LEVEL_WIDTH / 2;
    player->y = LEVEL_HEIGHT / 2 + 100;
    enemy[level].health = 100;
    enemy[level].x = LEVEL_WIDTH / 2;
    enemy[level].y = LEVEL_HEIGHT / 2;
    pView->Xcenter = LEVEL_WIDTH / 2;
    pView->Ycenter = LEVEL_HEIGHT / 2 + 100;
    camera->x = pView->Xcenter - SCREEN_WIDTH / 2;
    camera->y = pView->Ycenter - SCREEN_HEIGHT / 2;
}

void DrawToScreen(colors color, double worldTime, SDL_Surface* screen, SDL_Surface** images, ObjectOnScreen* player,
    ObjectOnScreen* enemy, int level, BulletObject* bullets, camera camera, double fps) {
    //fill screen with black and put upper info bar
    DrawPlayer(screen, images, camera, player);
    DrawBullets(screen, images, camera, bullets, enemy, level, *player);
    DrawEnemies(screen, images, camera, enemy, level);
    DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, MENU_BAR_HEIGHT, color.red, color.black);
    DrawRectangle(screen, SCREEN_WIDTH - 188, 6, 150, 75, color.red, color.black);
    DrawUi(screen, color, images, worldTime, fps, player, enemy, camera, level);
}

void DrawUi(SDL_Surface* screen, colors color, SDL_Surface** images, double worldTime, double fps, ObjectOnScreen* player, ObjectOnScreen* enemy, camera camera, int level) {
    char text[128];

    //draw all the text 
    sprintf(text, "DUNDERTALE");
    DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 16, text, images[charset]);

    sprintf(text, "Survival time = %.1lf s", worldTime);
    DrawString(screen, 10, 16, text, images[charset]);

    sprintf(text, "fps count = %.0lf fps", fps);
    DrawString(screen, 10, 26, text, images[charset]);

    sprintf(text, "Buttons:");
    DrawString(screen, SCREEN_WIDTH - strlen(text) * 8 - 190, 36, text, images[charset]);

    sprintf(text, "exit:       Esc");
    DrawString(screen, SCREEN_WIDTH - 180, 16, text, images[charset]);

    sprintf(text, "new game:    n");
    DrawString(screen, SCREEN_WIDTH - 180, 26, text, images[charset]);

    sprintf(text, "shoot: space/num");
    DrawString(screen, SCREEN_WIDTH - 180, 36, text, images[charset]);

    sprintf(text, "motion: ");
    DrawString(screen, SCREEN_WIDTH - 180, 61, text, images[charset]);

    sprintf(text, "\030");
    DrawString(screen, SCREEN_WIDTH - 78, 51, text, images[charset]);

    sprintf(text, "\031");
    DrawString(screen, SCREEN_WIDTH - 78, 71, text, images[charset]);

    sprintf(text, "\032");
    DrawString(screen, SCREEN_WIDTH - 88, 61, text, images[charset]);

    sprintf(text, "\033");
    DrawString(screen, SCREEN_WIDTH - 68, 61, text, images[charset]);

    //draw the player and boss health
    sprintf(text, "Player Health");
    DrawString(screen, 16, 50, text, images[charset]);
    DrawRectangle(screen, 16, 60, 100, 20, color.red, color.black); //outer layer
    DrawRectangle(screen, 16, 60, player->health, 20, color.red, color.red); //the inner layer

    sprintf(text, "Boss Health");
    DrawString(screen, 130, 50, text, images[charset]);
    DrawRectangle(screen, 130, 60, 100, 20, color.red, color.black);
    DrawRectangle(screen, 130, 60, enemy[level].health, 20, color.red, color.red);

    //annoy player if he has been hit
    if (player->wasHit == YES) {
        int x = SCREEN_WIDTH / 2 + player->invincibilityTime * SLOW_BULLET_SPEED * cos(player->invincibilityTime) * SCREEN_HEIGHT / 3;
        int y = SCREEN_HEIGHT / 2 + player->invincibilityTime * SLOW_BULLET_SPEED * sin(player->invincibilityTime) * SCREEN_HEIGHT / 3;

        sprintf(text, "Invincibility %d", int(PLAYER_INVINCIBILITY_TIME - player->invincibilityTime));
        DrawString(screen, x, y, text, images[charset]);
    }


    //draw ammo count
    int positionx = player->x + 20 - camera.x;
    int positiony = player->y - 20 - camera.y;

    if (player->noAmmo == YES) {
        sprintf(text, "NO AMMO %d", int(NO_AMMO_PENALTY - player->noAmmoTime));
        DrawString(screen, positionx, positiony, text, images[charset]);
    }
    else {
        for (int bullets = PLAYER_AMMO; bullets - player->numberOfShotBullets > 0; bullets--) {
            DrawRectangle(screen, positionx + bullets * 5, positiony, 2, 2, color.white, color.white);
        }
    }
    //draw POW on screen

    for (int bulletType = 0; bulletType < NUMBER_OF_PLAYER_PATTERNS; bulletType++) {
        for (int ithBullet = 0; ithBullet < player->playerBullets[bulletType].numberOfShotBullets; ithBullet++) {
            if (player->playerBullets[bulletType].Bullet[ithBullet].hasBeenShot == NO) {
                continue;
            }
            int x = SCREEN_WIDTH / 2 + player->playerBullets[bulletType].Bullet[ithBullet].lifeTime * SLOW_BULLET_SPEED * cos(player->playerBullets[bulletType].Bullet[ithBullet].lifeTime) * SCREEN_HEIGHT / 3;
            int y = SCREEN_HEIGHT / 2 + player->playerBullets[bulletType].Bullet[ithBullet].lifeTime * SLOW_BULLET_SPEED * tan(player->playerBullets[bulletType].Bullet[ithBullet].lifeTime / 4) * SCREEN_HEIGHT / 3;
            if (y == SCREEN_HEIGHT / 2) { y = player->y; x = player->x; }
            if (ithBullet % 2 == 0) {
                sprintf(text, "POW");
                DrawString(screen, x, y, text, images[charset]);
            }
            else {
                sprintf(text, "PEW");
                DrawString(screen, SCREEN_WIDTH - x, y, text, images[charset]);
            }
        }
    }
}


void DrawEnemies(SDL_Surface* screen, SDL_Surface** images, camera camera, ObjectOnScreen* enemy, int level) {

    switch (level) {
    case floweyObject:
        DrawSurface(screen, images[flowey], enemy[level].x - camera.x, enemy[level].y - camera.y);
        break;
    case sansObject:
        DrawSurface(screen, images[sans], enemy[level].x - camera.x, enemy[level].y - camera.y);
        break;
    case annoyingDogObject:
        DrawSurface(screen, images[doggo], enemy[level].x - camera.x, enemy[level].y - camera.y);
        break;
    }
}

void DrawBullets(SDL_Surface* screen, SDL_Surface** images, camera camera, BulletObject* bullets, ObjectOnScreen* enemy, int level, ObjectOnScreen player) {
    //draw boss bullets
    int bossBulletPatterns = enemy[level].numberOfPatterns;
    for (int bulletType = 0; bulletType < bossBulletPatterns; bulletType++) {
        for (int ithBullet = 0; ithBullet < bullets[bulletType].numberOfShotBullets; ithBullet++) {
            if (bullets[bulletType].Bullet[ithBullet].hasBeenShot == NO) {
                continue;
            }
            int xpos = bullets[bulletType].Bullet[ithBullet].x;
            int ypos = bullets[bulletType].Bullet[ithBullet].y;
            DrawSurface(screen, images[bullet], xpos - camera.x, ypos - camera.y);
        }
    }
    //draw player bullets
    for (int bulletType = 0; bulletType < NUMBER_OF_PLAYER_PATTERNS; bulletType++) {
        for (int ithBullet = 0; ithBullet < player.playerBullets[bulletType].numberOfShotBullets; ithBullet++) {
            if (player.playerBullets[bulletType].Bullet[ithBullet].hasBeenShot == NO) {
                continue;
            }
            int xpos = player.playerBullets[bulletType].Bullet[ithBullet].x;
            int ypos = player.playerBullets[bulletType].Bullet[ithBullet].y;
            switch (player.playerBullets[bulletType].pattern) {
            case UP:
                DrawSurface(screen, images[Bullet_up], xpos - camera.x, ypos - camera.y);
                break;
            case DOWN:
                DrawSurface(screen, images[Bullet_down], xpos - camera.x, ypos - camera.y);
                break;
            case RIGHT:
                DrawSurface(screen, images[Bullet_right], xpos - camera.x, ypos - camera.y);
                break;
            case LEFT:
                DrawSurface(screen, images[Bullet_left], xpos - camera.x, ypos - camera.y);
                break;
            }
        }
    }

}

void DrawPlayer(SDL_Surface* screen, SDL_Surface** images, camera camera, ObjectOnScreen* player) {
    switch (player->direction) {
    case LEFT:
        DrawSurface(screen, images[heart_left], player->x - camera.x, player->y - camera.y);
        break;
    case RIGHT:
        DrawSurface(screen, images[heart_right], player->x - camera.x, player->y - camera.y);
        break;
    case UP:
        DrawSurface(screen, images[heart_up], player->x - camera.x, player->y - camera.y);
        break;
    case DOWN:
        DrawSurface(screen, images[heart_down], player->x - camera.x, player->y - camera.y);
        break;

    }
}

void shootBullet(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, double* shootTimer, int* shoot, double delta) {
    double freq = enemy[level].cooldown;
    int bulletType;
    int bossBulletPatterns = enemy[level].numberOfPatterns;
    if (*shootTimer >= freq) {
        int lastBullet = player->lastBulletPattern;
        /*
        bulletType = rand() % bossBulletPatterns;
        while (lastBullet == bulletType) {
                bulletType = rand() % bossBulletPatterns;
        }
        */
        bulletType = (lastBullet + 1) % bossBulletPatterns;
        player->lastBulletPattern = bulletType;
        int numberOfShotBullets = bullets[bulletType].numberOfShotBullets;
        if (numberOfShotBullets >= bullets[bulletType].numberOfBullets) {
            *shootTimer = 0;
            return;
        }
        bullets[bulletType].Bullet[numberOfShotBullets].hasBeenShot = YES;
        bullets[bulletType].numberOfShotBullets++;
        bullets[bulletType].Bullet[numberOfShotBullets].x = enemy[level].x;
        bullets[bulletType].Bullet[numberOfShotBullets].y = enemy[level].y;
        *shootTimer = 0;
    }


    //player part
    double timeBetweenShooting = player->timeBetweenShooting;
    if (player->shotTaken == YES) {
        player->timeBetweenShooting += delta;
        if (timeBetweenShooting >= TIME_BETWEEN_SHOOTING) {
            player->timeBetweenShooting = 0;
            player->shotTaken = NO;
        }
    }

    double noAmmoPenalty = player->noAmmoTime;
    if (player->numberOfShotBullets == PLAYER_AMMO) {
        player->noAmmo = YES;

    }
    if (player->noAmmo == YES) {
        player->noAmmoTime += delta;
        player->noAmmo = YES;
        if (noAmmoPenalty >= NO_AMMO_PENALTY) {
            player->noAmmoTime = 0;
            player->noAmmo = NO;
        }
    }


    if (*shoot == YES && player->shotTaken != YES && player->noAmmo != YES) {
        int direction = player->direction;
        int ithBullet = player->playerBullets[direction].numberOfShotBullets;
        int checkCountOfBullets = player->numberOfShotBullets + 1;
        //before doing anything, check if player can shoot
        if (checkCountOfBullets <= PLAYER_AMMO) {
            player->playerBullets[direction].numberOfShotBullets++;
            player->numberOfShotBullets++;
            player->playerBullets[direction].Bullet[ithBullet].hasBeenShot = YES;
            player->playerBullets[direction].Bullet[ithBullet].x = player->x;
            player->playerBullets[direction].Bullet[ithBullet].y = player->y;
            switch (player->direction) {
            case UP:
                player->y += SHOTING_KNOCKBACK;
                break;
            case DOWN:
                player->y -= SHOTING_KNOCKBACK;
                break;
            case RIGHT:
                player->x -= SHOTING_KNOCKBACK;
                break;
            case LEFT:
                player->x += SHOTING_KNOCKBACK;
                break;
            }
            player->shotTaken = YES;
        }
    }
    *shoot = NO;
}


void CalcPositionOfEnemy(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, double worldTime, double delta) {
    if (level == annoyingDogObject) {
        int x2 = LEVEL_WIDTH / 2 + sin(worldTime) * LEVEL_HEIGHT / 3;
        int y2 = LEVEL_HEIGHT / 2 + sin(worldTime) * cos(worldTime) * LEVEL_HEIGHT / 3;
        enemy[level].x = x2;
        enemy[level].y = y2;
        checkHitBox(bullets, player, none, none, delta, enemy, level, annoyingDogObject);
    }
    if (level == sansObject) {
        int x2 = LEVEL_WIDTH / 2 + sin(worldTime) * LEVEL_HEIGHT / 3;
        int y2 = LEVEL_HEIGHT / 2;
        enemy[level].x = x2;
        enemy[level].y = y2;
        checkHitBox(bullets, player, none, none, delta, enemy, level, sansObject);
    }
}

void CalcPositionOfBullets(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, double delta, double* shootTimer) {
    float x2, y2; //new position of bullet
    double lifetime;
    int bossBulletPatterns = enemy[level].numberOfPatterns;
    for (int bulletType = 0; bulletType < bossBulletPatterns; bulletType++) {
        for (int ithBullet = 0; ithBullet < bullets[bulletType].numberOfBullets; ithBullet++) {
            if (bullets[bulletType].Bullet[ithBullet].hasBeenShot == NO) {
                continue;
            }
            x2 = bullets[bulletType].Bullet[ithBullet].x;
            y2 = bullets[bulletType].Bullet[ithBullet].y;

            switch (bulletType) {
            case verticalUp:
            case verticalDown:
                (bulletType == verticalUp ? y2 -= MEDIUM_BULLET_SPEED : y2 += MEDIUM_BULLET_SPEED);
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case horizontalRight:
            case horizontalLeft:
                (bulletType == horizontalRight ? x2 += MEDIUM_BULLET_SPEED : x2 -= MEDIUM_BULLET_SPEED);
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case diagonalUpRight:
            case diagonalUpLeft:
                (bulletType == diagonalUpRight ? x2 += MEDIUM_BULLET_SPEED : x2 -= MEDIUM_BULLET_SPEED);
                y2 -= SLOW_BULLET_SPEED;
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case diagonalDownRight:
            case diagonalDownLeft:
                (bulletType == diagonalDownRight ? x2 += MEDIUM_BULLET_SPEED : x2 -= MEDIUM_BULLET_SPEED);
                y2 += SLOW_BULLET_SPEED;
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case spiral:
                bullets[bulletType].Bullet[ithBullet].lifeTime += delta;
                lifetime = bullets[bulletType].Bullet[ithBullet].lifeTime;
                x2 = enemy[level].x + lifetime * SLOW_BULLET_SPEED * cos(lifetime) * enemy[level].y;
                y2 = enemy[level].y + lifetime * SLOW_BULLET_SPEED * sin(lifetime) * enemy[level].y;
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case zigzag:
                bullets[bulletType].Bullet[ithBullet].lifeTime += delta;
                lifetime = bullets[bulletType].Bullet[ithBullet].lifeTime;
                x2 = enemy[level].x + lifetime * cos(lifetime) * sin(lifetime) * enemy[level].y;
                y2 = enemy[level].y + cos(lifetime) * enemy[level].y;
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case zigzag2:
                bullets[bulletType].Bullet[ithBullet].lifeTime += delta;
                lifetime = bullets[bulletType].Bullet[ithBullet].lifeTime;
                x2 = enemy[level].x + sin(lifetime) * enemy[level].y;
                y2 = enemy[level].y + lifetime * sin(lifetime) * cos(lifetime) * enemy[level].y;
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case eight_horizontaly:
                bullets[bulletType].Bullet[ithBullet].lifeTime += delta;
                lifetime = bullets[bulletType].Bullet[ithBullet].lifeTime;
                x2 = enemy[level].x + sin(lifetime * MEDIUM_BULLET_SPEED) * enemy[level].y * BULLET_DISTANCE;
                y2 = enemy[level].y + sin(lifetime * MEDIUM_BULLET_SPEED) * cos(lifetime * MEDIUM_BULLET_SPEED) * enemy[level].y * BULLET_DISTANCE;
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case eight_verticaly:
                bullets[bulletType].Bullet[ithBullet].lifeTime += delta;
                lifetime = bullets[bulletType].Bullet[ithBullet].lifeTime;
                x2 = enemy[level].x + cos(lifetime * MEDIUM_BULLET_SPEED) * sin(lifetime * MEDIUM_BULLET_SPEED) * enemy[level].y * BULLET_DISTANCE;
                y2 = enemy[level].y + cos(lifetime * MEDIUM_BULLET_SPEED) * enemy[level].y * BULLET_DISTANCE;
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case circle:
                bullets[bulletType].Bullet[ithBullet].lifeTime += delta;
                lifetime = bullets[bulletType].Bullet[ithBullet].lifeTime;
                x2 = enemy[level].x + sin(lifetime) * enemy[level].y * SLOW_BULLET_SPEED;
                y2 = enemy[level].y + cos(lifetime) * enemy[level].y * SLOW_BULLET_SPEED;
                bullets[bulletType].Bullet[ithBullet].x = x2;
                bullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            }
            checkStateOfBullet(bullets, bulletType, ithBullet, enemy, level, shootTimer);
            checkHitBox(bullets, player, bulletType, ithBullet, delta, enemy, level, bulletObject);
        }

    }

}

void CalcPositionOfPlayerBullets(BulletObject* bullets, ObjectOnScreen* player, ObjectOnScreen* enemy, int level, double delta) {

    float x2, y2; //new position of bullet
    double lifetime;
    for (int bulletType = 0; bulletType < NUMBER_OF_PLAYER_PATTERNS; bulletType++) {
        for (int ithBullet = 0; ithBullet < player->playerBullets[bulletType].numberOfBullets; ithBullet++) {
            if (player->playerBullets[bulletType].Bullet[ithBullet].hasBeenShot == NO) {
                continue;
            }

            player->playerBullets[bulletType].Bullet[ithBullet].lifeTime += delta;
            lifetime = player->playerBullets[bulletType].Bullet[ithBullet].lifeTime;

            if (lifetime >= PLAYER_BULLET_LIFETIME) {
                player->playerBullets[bulletType].Bullet[ithBullet].hasBeenShot = NO;
                player->playerBullets[bulletType].Bullet[ithBullet].lifeTime = 0;
                player->numberOfShotBullets--;
                player->playerBullets[bulletType].numberOfShotBullets--;
                continue;
            }

            x2 = player->playerBullets[bulletType].Bullet[ithBullet].x;
            y2 = player->playerBullets[bulletType].Bullet[ithBullet].y;

            switch (bulletType) {
            case verticalUp:
            case verticalDown:
                (bulletType == verticalUp ? y2 -= FAST_BULLET_SPEED : y2 += FAST_BULLET_SPEED);
                player->playerBullets[bulletType].Bullet[ithBullet].x = x2;
                player->playerBullets[bulletType].Bullet[ithBullet].y = y2;
                break;

            case horizontalRight:
            case horizontalLeft:
                (bulletType == horizontalRight ? x2 += FAST_BULLET_SPEED : x2 -= FAST_BULLET_SPEED);
                player->playerBullets[bulletType].Bullet[ithBullet].x = x2;
                player->playerBullets[bulletType].Bullet[ithBullet].y = y2;
                break;
            }
            checkHitBox(bullets, player, bulletType, ithBullet, delta, enemy, level, playerObject);
        }

    }

}

void checkStateOfBullet(BulletObject* bullets, int bulletType, int ithBullet, ObjectOnScreen* enemy, int level, double* shootTimer) {
    //check if the bullet hit a boundary, if it did reset its position
    if (bullets[bulletType].Bullet[ithBullet].x >= LEVEL_WIDTH || bullets[bulletType].Bullet[ithBullet].x <= 0 || bullets[bulletType].Bullet[ithBullet].y >= LEVEL_HEIGHT || bullets[bulletType].Bullet[ithBullet].y <= 0) {
        bullets[bulletType].Bullet[ithBullet].x = enemy[level].x;
        bullets[bulletType].Bullet[ithBullet].y = enemy[level].y;
        bullets[bulletType].Bullet[ithBullet].lifeTime = 0;
        bullets[bulletType].Bullet[ithBullet].hasBeenShot = NO;
        bullets[bulletType].numberOfShotBullets--;
        *shootTimer = 0;
    }
}

void checkHitBox(BulletObject* bullets, ObjectOnScreen* player, int bulletType, int ithBullet, double delta, ObjectOnScreen* enemy, int level, int posession) {

    int Bx1, Bx2, By1, By2; // bullet hitbox places
    int Px1, Px2, Py1, Py2; //player hitbox places
    int markHit = NO;
    double invincibility = PLAYER_INVINCIBILITY_TIME; //after being hit, player gains x seconds of invincibility
    getObjectPositions(bullets, player, bulletType, ithBullet, enemy, level, posession, &Bx1, &Bx2, &By1, &By2, &Px1, &Px2, &Py1, &Py2);
    if (posession == bulletObject || posession == annoyingDogObject || posession == sansObject) {
        if (player->wasHit == YES) {
            player->invincibilityTime += delta / 9; //because it is in a loop it has to be divided by a number otherwise it goes crazy fast, this number is random and is not particulary important
            if (player->invincibilityTime >= invincibility) {
                player->wasHit = NO;
                player->invincibilityTime = 0;
            }
        }
        else if (((Px1 >= Bx1 && Px1 <= Bx2) || (Px2 >= Bx1 && Px2 <= Bx2)) && ((Py1 >= By1 && Py1 <= By2) || (Py2 >= By1 && Py2 <= By2)) ||//check if objects are touching each other
            (Px1 <= Bx1 && Px1<Bx2 && Px2 >= Bx2 && Px2>Bx1 && Py1 <= By1 && Py1<By2 && Py2 >= By2 && Py2>By1)) {    //by chance, check if an object is inside another object
            player->health -= ENEMY_BULLET_DMG;
            player->wasHit = YES;
            player->wasHitTimes++;
            bullets[bulletType].Bullet[ithBullet].x = enemy[level].x;
            bullets[bulletType].Bullet[ithBullet].y = enemy[level].y;
        }
    }
    else if (posession == playerObject) {
        if (((Px1 >= Bx1 && Px1 <= Bx2) || (Px2 >= Bx1 && Px2 <= Bx2)) && ((Py1 >= By1 && Py1 <= By2) || (Py2 >= By1 && Py2 <= By2)) ||
            (Px1 <= Bx1 && Px1<Bx2 && Px2 >= Bx2 && Px2>Bx1 && Py1 <= By1 && Py1<By2 && Py2 >= By2 && Py2>By1)) {
            enemy[level].health -= PLAYER_BULLET_DMG;
            player->playerBullets[bulletType].Bullet[ithBullet].x = -10; //this sets the bullet to fly out of visible playerspace
            player->playerBullets[bulletType].Bullet[ithBullet].y = -10;
        }
    }



}

void getObjectPositions(BulletObject* bullets, ObjectOnScreen* player, int bulletType, int ithBullet, ObjectOnScreen* enemy, int level, int posession, int* Bx1, int* Bx2, int* By1, int* By2, int* Px1, int* Px2, int* Py1, int* Py2) {
    //get bullet params
    //this could have been shorter but i had limited budget...
    if (level != annoyingDogObject && level != sansObject) { //if player is playing annoying dog level or sans level, it becomes the bullet so this function gets his position
        if (posession == bulletObject) {
            *Bx1 = bullets[bulletType].Bullet[ithBullet].x - bullets[bulletType].objWidth / 2;
            *Bx2 = bullets[bulletType].Bullet[ithBullet].x + bullets[bulletType].objWidth / 2;
            *By1 = bullets[bulletType].Bullet[ithBullet].y - bullets[bulletType].objHeight / 2;
            *By2 = bullets[bulletType].Bullet[ithBullet].y + bullets[bulletType].objHeight / 2;
        }
        else {
            *Bx1 = player->playerBullets[bulletType].Bullet[ithBullet].x - player->playerBullets[bulletType].objWidth / 2;
            *Bx2 = player->playerBullets[bulletType].Bullet[ithBullet].x + player->playerBullets[bulletType].objWidth / 2;
            *By1 = player->playerBullets[bulletType].Bullet[ithBullet].y - player->playerBullets[bulletType].objHeight / 2;
            *By2 = player->playerBullets[bulletType].Bullet[ithBullet].y + player->playerBullets[bulletType].objHeight / 2;
        }
    }
    else {
        if (posession == bulletObject) {
            *Bx1 = bullets[bulletType].Bullet[ithBullet].x - bullets[bulletType].objWidth / 2;
            *Bx2 = bullets[bulletType].Bullet[ithBullet].x + bullets[bulletType].objWidth / 2;
            *By1 = bullets[bulletType].Bullet[ithBullet].y - bullets[bulletType].objHeight / 2;
            *By2 = bullets[bulletType].Bullet[ithBullet].y + bullets[bulletType].objHeight / 2;
        }
        else if (posession == playerObject) {
            *Bx1 = player->playerBullets[bulletType].Bullet[ithBullet].x - player->playerBullets[bulletType].objWidth / 2;
            *Bx2 = player->playerBullets[bulletType].Bullet[ithBullet].x + player->playerBullets[bulletType].objWidth / 2;
            *By1 = player->playerBullets[bulletType].Bullet[ithBullet].y - player->playerBullets[bulletType].objHeight / 2;
            *By2 = player->playerBullets[bulletType].Bullet[ithBullet].y + player->playerBullets[bulletType].objHeight / 2;
        }
        else {
            *Bx1 = enemy[level].x - enemy[level].objWidth / 2;
            *Bx2 = enemy[level].x + enemy[level].objWidth / 2;
            *By1 = enemy[level].y - enemy[level].objHeight / 2;
            *By2 = enemy[level].y + enemy[level].objHeight / 2;
        }
    }

    //get player or enemy params
    if (posession == bulletObject || posession == annoyingDogObject || posession == sansObject) {
        *Px1 = player->x - player->objWidth / 2;
        *Px2 = player->x + player->objWidth / 2;
        *Py1 = player->y - player->objHeight / 2;
        *Py2 = player->y + player->objHeight / 2;
    }
    else {
        *Px1 = enemy[level].x - enemy[level].objWidth / 2;
        *Px2 = enemy[level].x + enemy[level].objWidth / 2;
        *Py1 = enemy[level].y - enemy[level].objHeight / 2;
        *Py2 = enemy[level].y + enemy[level].objHeight / 2;
    }
}