/*
C-COM: Enemy Well Known
    This program aims to mimic the basic functionalities
        of a popular video game X-COM: Enemy Unknown. It's not particularly fun, but it's complete.
    The player can move, shoot, and throw grenades with their units.
    The enemy team can move and shoot.
Created by:
    Nicolas LaCognata
    11/16/16
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BOARD_Y 20
#define BOARD_X 40
#define SQUAD_SIZE 6
#define NUM_ENEMIES 10
typedef struct location{ //Represents a board space
    int y, x;
} loc;
typedef struct unit { //Represents a player unit
    char rank[5]; //Indicates quality of unit
    char name[20]; //Name
    char unitClass[20]; //Gives player hint on proper use of unit i.e Grenadier gets extra grenades.

    int hp;
    int ap;
    int mobility; //How far unit can move with its action points
    int clipSize; //Max ammo a unit can carry, dictates reload size
    int ammo;
    int aim; //Likelihood of hitting target.
    int grenades; //How many grenades the unit has in its inventory.
    int report;

    loc location; //unit location
} unit;
typedef struct enemy { //Represents enemy unit
    int hp;
    int ap;
    int mobility; //How far unit can move with its action points
    int aim;
    int report;

    loc location; //unit location
} enemyUnit;

///
void initGame(int board[BOARD_Y][BOARD_X], unit squad[6], enemyUnit enemies[NUM_ENEMIES]); //Calls the next three functions, prints splash screen and waits for input to start game.
void initBoard(int board[BOARD_Y][BOARD_X]); //Initializes board
void initSquad(unit squad[6], int board[BOARD_Y][BOARD_X]); //Initializes Squad
void initEnemies(enemyUnit enemies[NUM_ENEMIES], int board[BOARD_Y][BOARD_X]); //Initializes Enemies

int calcCover(int board[BOARD_Y][BOARD_X], loc shooter, loc target); //Calculates cover based on shooter and target's position

void moveUnit(int board[BOARD_Y][BOARD_X], unit *un); //Moves unit to specified location
void shootUnit(int board[BOARD_Y][BOARD_X], unit *un, enemyUnit enemies[NUM_ENEMIES]); //Handle's friendly unit shooting
void grenade(int board[BOARD_Y][BOARD_X], unit *un, enemyUnit[NUM_ENEMIES], unit squad[6]); //Handles throwing grenades
void command(int board[BOARD_Y][BOARD_X], unit *un, unit squad[6], enemyUnit enemies[NUM_ENEMIES]); //Handles issuing player commands
void playerTurn(int board[BOARD_Y][BOARD_X], unit squad[6], enemyUnit enemies[NUM_ENEMIES], int *unitsLeft); //Handles iterating though squad during player turn

void moveEnemy(int board[BOARD_Y][BOARD_X], enemyUnit *en, unit un); //Handles enemy movement logic
void shootEnemy(int board[BOARD_Y][BOARD_X], enemyUnit *en, unit *un); //Handles enemy shooting logic
void enemyTurn(int board[BOARD_Y][BOARD_X], unit squad[6], enemyUnit enemies[NUM_ENEMIES], int *enemiesLeft); //Controls enemy turns

void updateBoard(int board[BOARD_Y][BOARD_X], int y, int x, int replace); //Used exclusively to modify the board
void checkDeaths(int board[BOARD_Y][BOARD_X], enemyUnit enemies[NUM_ENEMIES], unit squad[6]); //Checks to see if any units have died due to game actions and reports it to player

void printBoard(int board[BOARD_Y][BOARD_X]); //Prints the board as ASCII characters
void printUnit(unit un); //Prints a single unit
void printSquad(unit squad[6]); //Prints Entire Squad
void printEnemySquad(enemyUnit enemies[NUM_ENEMIES]); //Prints enemy Squad, used for debugging

int checkEnd(unit squad[SQUAD_SIZE], enemyUnit enemies[NUM_ENEMIES]); //Checks to see if game is over.

int main(){
    int board[BOARD_Y][BOARD_X];
    unit squad[6];
    enemyUnit enemies[NUM_ENEMIES];
    initGame(board, squad, enemies);
    int unitsLeft = 6, enemiesLeft= NUM_ENEMIES, gameOver = 0;
    while (gameOver == 0){
        playerTurn(board, squad, enemies, &unitsLeft);
        enemyTurn(board, squad, enemies, &enemiesLeft);
        gameOver = checkEnd(squad, enemies); //CHECK REPORT FLAG
    }
    return 0;
}

/*
    Precondition: board, squad, and enemies are all in main.
    Postcondition: Initializes the board and both teams
*/
void initGame(int board[BOARD_Y][BOARD_X], unit squad[6], enemyUnit enemies[NUM_ENEMIES]){
    initBoard(board);
    initSquad(squad, board);
    initEnemies(enemies, board);
    printf("\n");
    printf("   _____             _____    ____    __  __ \n");
    printf("  / ____|           / ____|  / __ \\  |  \\/  |\n");
    printf(" | |       ______  | |      | |  | | | \\  / |\n");
    printf(" | |      |______| | |      | |  | | | |\\/| |\n");
    printf(" | |____           | |____  | |__| | | |  | |\n");
    printf("  \\_____|           \\_____|  \\____/  |_|  |_|\n");
    printf("Welcome commander. ");
    system("pause");
    return;
}
/*
    Precondition: board is defined in main.
    Postcondition: Modifies board to reflect the map dictated in board.config
    The file is guaranteed to be filled with integers in a BOARD_Y x BOARD_X configuration
*/
void initBoard(int board[BOARD_Y][BOARD_X]){
    int i, j, temp;
    FILE *ifp;
    ifp = fopen("board.config", "r");
    for(i = 0; i < BOARD_Y; i++)
        for(j = 0; j < BOARD_X; j++){
            fscanf(ifp, "%d", &temp);
            board[i][j] = temp;
        }
    fclose(ifp);
    return;
}
/*
    Precondition: squad is defined in main. Board is already initialized, and has not been modified further.

    Postcondition: populates player squad with units from squad.config.
    Squad.config can have up to six units.
    Input specification:
        x - number of units to be added
        for each unit:
            string - rank
            string - name
            string class
            int - hp
            int - mobility
            int - ammo
            int - aim
            int - grenades


    After squad is populated units are placed on the board in order of available space as dictated
    by the initial configuration of the board.
*/
void initSquad(unit squad[6], int board[BOARD_Y][BOARD_X]){
    int i, j, numUnits, assigned = 0;
    char temp[20];
    int number;
    FILE *ifp;
    ifp = fopen("squad.config", "r");
    fscanf(ifp, "%d", &numUnits);
    for(i = 0; i < numUnits; i++){ //Reads in stats
        //Scanning in names
        fscanf(ifp, "%s", temp);
        strcpy(squad[i].rank, temp);
        fscanf(ifp, "%s", temp);
        strcpy(squad[i].name, temp);
        fscanf(ifp, "%s", temp);
        strcpy(squad[i].unitClass, temp);

        //Scanning in stats
        fscanf(ifp, "%d", &squad[i].hp);
        squad[i].ap = 2;
        fscanf(ifp, "%d", &squad[i].mobility);
        fscanf(ifp, "%d", &squad[i].clipSize);
        fscanf(ifp, "%d", &squad[i].ammo);
        fscanf(ifp, "%d", &squad[i].aim);
        fscanf(ifp, "%d", &squad[i].grenades);
        squad[i].report = 0;
    }

    //Loop to assign locations
    for(i = 0; i < BOARD_Y; i++)
        for(j = 0; j < BOARD_X; j++){
            if(assigned > 5)
                break;
            if(board[i][j] == 1){
                squad[assigned].location.x = j;
                squad[assigned].location.y = i;
                assigned++;
            }
        }
    fclose(ifp);
    return;
}
/*
    Precondition: enemies is defined in main. Board is already initialized, and has not been modified further.
    Postcondition: populates enemy squad with NUM_EMENIES identical units, assigns starting locations according to board.config.
*/
void initEnemies(enemyUnit enemies[NUM_ENEMIES], int board[BOARD_Y][BOARD_X]){
    int i, j, assigned = 0;

    //Reads in stats
    for(i = 0; i < NUM_ENEMIES; i++){
        enemies[i].hp = 8;
        enemies[i].ap = 2;
        enemies[i].mobility = 7;
        enemies[i].aim = 70;
    }

    //Loop to assign locations
    for(i = 0; i < BOARD_Y; i++)
        for(j = 0; j < BOARD_X; j++){
            if(assigned > NUM_ENEMIES - 1)
                break;
            if(board[i][j] == 2){
                enemies[assigned].location.x = j;
                enemies[assigned].location.y = i;
                assigned++;
            }
        }

    return;
}
/*
    Precondition: Board and unit are initialized.
    Postcondition: Unit is moved from current location to a new one.
*/
void moveUnit(int board[BOARD_Y][BOARD_X], unit *un){
    int i, j;
    int y, x;
    int moved = 0;
    printf("  ");

    //Top coordinate system
    for(i = 0; i < BOARD_X; i++)
        printf("%d ", i%10);
    printf("\n");


    for(i = 0; i < BOARD_Y; i++){
        printf("%d ", i%10);
        for(j = 0; j < BOARD_X; j++){
            if(board[i][j] == 0 && (abs(i - un->location.y) + abs(j - un->location.x)) <= un->mobility){
                printf("* ");
            }
            else{
                switch(board[i][j]){
                    case 0:
                        printf("_ ");
                        break;
                    case 1:
                        printf("X ");
                        break;
                    case 2:
                        printf("A ");
                        break;
                    case 3:
                        printf("%c ", 219); //176
                        break;
                    case 4:
                        printf("%c ", 220);
                        break;
                    default:
                        board[i][j] = 0;
                        printf("_ ");
                }
            }
        }
        printf("\n");
    }

    printf("Enter a location (X,Y) to move to marked by a \"*\" \n");
    scanf("%d %d", &x, &y);
    while(!moved){
        if((board[y][x] == 0 && (abs(y - un->location.y) + abs(x - un->location.x)) <= un->mobility) && (y >= 0 && x  >= 0) && (y < BOARD_Y && x < BOARD_X) ){
            updateBoard(board, un->location.y, un->location.x, 0);
            un->location.x = x;
            un->location.y = y;
            updateBoard(board, un->location.y, un->location.x, 1);

            moved = 1;

        }
        else{
            printf("\nEnter a location (X,Y) to move to marked by a \"*\" \n");
            scanf("%d %d", &x, &y);
        }
    }
    return;
}

/*
    Precondition: Board is initialized.
    Postcondition: Returns number that represents value of cover unit is up against.
*/
int calcCover(int board[BOARD_Y][BOARD_X], loc shooter, loc target){
    int cover;
    //printf("ENEMY RECIEVED: %d, %d\n\n", en.location.x, en.location.y);
    if(shooter.x < target.x){ //Shooter is left of target
        if(shooter.y < target.y){ // Left and above, left and above cover count, taking higher value
            int cover1, cover2;
            switch (board[target.y][--target.x]){ // Left Cover
                case 0: // no cover
                    cover1 = 0;
                    break;
                case 3: //full cover
                    cover1 = 40;
                    break;
                case 4: //half cover
                    cover1 = 20;
                    break;
                default:
                    cover1 = 0;
                    break;
            }
            switch (board[--target.y][target.x]){ // Top Cover
                case 0: // no cover
                    cover2 = 0;
                    break;
                case 3: //full cover
                    cover2 = 40;
                    break;
                case 4: //half cover
                    cover2 = 20;
                    break;
                default:
                    cover2 = 0;
                    break;
            }
            if(cover1 > cover2)
                cover = cover1;
            else
                cover = cover2;
        }
        else if(shooter.y == target.y){// Directly Left, only left cover value counts
            switch (board[target.y][--target.x]){
                case 0: // no cover
                    cover = 0;
                    break;
                case 3: //full cover
                    cover = 40;
                    break;
                case 4: //half cover
                    cover = 20;
                    break;
                default:
                    cover = 0;
                    break;
            }
        }
        else{ // Left and below, left and below cover count, taking higher value
            int cover1, cover2;
            switch (board[target.y][--target.x]){ // Left Cover
                case 0: // no cover
                    cover1 = 0;
                    break;
                case 3: //full cover
                    cover1 = 40;
                    break;
                case 4: //half cover
                    cover1 = 20;
                    break;
                default:
                    cover1 = 0;
                    break;
            }
            switch (board[++target.y][target.x]){ // Below Cover
                case 0: // no cover
                    cover2 = 0;
                    break;
                case 3: //full cover
                    cover2 = 40;
                    break;
                case 4: //half cover
                    cover2 = 20;
                    break;
                default:
                    cover2 = 0;
                    break;
            }
            if(cover1 > cover2)
                cover = cover1;
            else
                cover = cover2;
        }
    }
    else if(shooter.x == target.x){ // shooter is directly above or below target
        if(shooter.y < target.y){ //Shooter above target
            switch (board[--target.y][target.x]){
                case 0: // no cover
                    cover = 0;
                    break;
                case 3: //full cover
                    cover = 40;
                    break;
                case 4: //half cover
                    cover = 20;
                    break;
                default:
                    cover = 0;
                    break;
            }
        }
        else { //shooter is below target
            switch (board[++target.y][target.x]){
                case 0: // no cover
                    cover = 0;
                    break;
                case 3: //full cover
                    cover = 40;
                    break;
                case 4: //half cover
                    cover = 20;
                    break;
                default:
                    cover = 0;
                    break;
            }
        }
    }
    else{ //Shooter is right of target
        if(shooter.y < target.y){ // Right and above, right and above cover count, taking higher value
            int cover1, cover2;
            switch (board[target.y][++target.x]){ // Right Cover
                case 0: // no cover
                    cover1 = 0;
                    break;
                case 3: //full cover
                    cover1 = 40;
                    break;
                case 4: //half cover
                    cover1 = 20;
                    break;
                default:
                    cover1 = 0;
                    break;
            }
            switch (board[--target.y][target.x]){ // Top Cover
                case 0: // no cover
                    cover2 = 0;
                    break;
                case 3: //full cover
                    cover2 = 40;
                    break;
                case 4: //half cover
                    cover2 = 20;
                    break;
                default:
                    cover2 = 0;
                    break;
            }
            if(cover1 > cover2)
                cover = cover1;
            else
                cover = cover2;
        }
        else if(shooter.y == target.y){// Directly Right, only left cover value counts
            switch (board[target.y][++target.x]){
                case 0: // no cover
                    cover = 0;
                    break;
                case 3: //full cover
                    cover = 40;
                    break;
                case 4: //half cover
                    cover = 20;
                    break;
                default:
                    cover = 0;
                    break;
            }
        }
        else{ // Right and below, left and below cover count, taking higher value
            int cover1, cover2;
            switch (board[target.y][++target.x]){ // Right Cover
                case 0: // no cover
                    cover1 = 0;
                    break;
                case 3: //full cover
                    cover1 = 40;
                    break;
                case 4: //half cover
                    cover1 = 20;
                    break;
                default:
                    cover1 = 0;
                    break;
            }
            switch (board[++target.y][target.x]){ // Below Cover
                case 0: // no cover
                    cover2 = 0;
                    break;
                case 3: //full cover
                    cover2 = 40;
                    break;
                case 4: //half cover
                    cover2 = 20;
                    break;
                default:
                    cover2 = 0;
                    break;
            }
            if(cover1 > cover2)
                cover = cover1;
            else
                cover = cover2;
        }
    }

    return cover;
}
/*
    Precondition: Board, un, enemies are initialized.
    Postcondition: Returns number that represents value of cover unit is up against.
*/
void shootUnit(int board[BOARD_Y][BOARD_X], unit *un, enemyUnit enemies[NUM_ENEMIES]){
    int i, shoot, cover, distance, count = 0, unitsInRange = 0;

    //printf("DISTANCE %d\n", distance);
    double chance = 100.0;
    double shot;
    for(i = 0; i < NUM_ENEMIES; i++){
        //printf("Calculated Distance: %d\n", (abs(enemies[i].location.y - un->location.y)) + abs(abs(enemies[i].location.x - un->location.x)));
        //printf("Unit Range: %d\n", un->aim / 10);
        if(abs(enemies[i].location.y - un->location.y) + abs(enemies[i].location.x - un->location.x) <= un->aim / 5){
            unitsInRange++;
        }
    }

    //system("pause");
    if(unitsInRange == 0){
        printf("No enemies in range. ");
        system("pause");
        un->ap += 2;
        return;
    }
    printf("%d units in range.\n", unitsInRange);
    enemyUnit *enemiesInRange[NUM_ENEMIES]; //pointers to original enemies.
    for( i = 0; i < NUM_ENEMIES; i++){
        if(abs(enemies[i].location.y - un->location.y) + abs(enemies[i].location.x - un->location.x) <= un->aim / 5){
            enemiesInRange[count] = &enemies[i];
            distance = abs(enemies[i].location.y - un->location.y) + abs(enemies[i].location.x - un->location.x);
            cover = calcCover(board, un->location, enemies[i].location);
            chance = un->aim - .5 * distance - cover;
            printf("%d). Enemy @ (%d, %d) Chance to hit %.2lf \n", count + 1, enemiesInRange[count]->location.x, enemiesInRange[count]->location.y, chance);
            count++;
        }
    }
    printf("Select an enemy to shoot: \n");

    scanf("%d", &shoot);
    shoot--; //Converts to base 0 for use in array.

    //Calculate hit chance (Aim -  2 * Distance – Cover Bonus)
        //determine cover bonus if applicable. 0 = no cover, 20 = half cover; 40 = full cover
        //Handles cover bonus depending on relative location of unit to target
    cover = calcCover(board, un->location, enemiesInRange[shoot]->location);
    distance = abs(enemiesInRange[shoot]->location.y - un->location.y) + abs(enemiesInRange[shoot]->location.x - un->location.x);
    chance = un->aim - .5 * distance - cover;
    //Calculate hit success (pass /fail)
    srand(time(0));
    shot = rand() % 100 + 1;
    //printf("Unit rolled a %lf\n", shot);
    if(shot > chance){
        printf("%s %s's shot missed. \nEnemy unscathed.\n", un->rank, un->name);
    }
    else{
        int damage = 3;
        int damageModifier = 2;
        srand(time(0));
        double modify = rand() % 100 + 1;
        if(modify < 75){
            enemiesInRange[shoot]->hp-= damage;
        }
        else if(modify > 75 && modify < 85){ //Subtract damage modifier
            damage -= damageModifier;
            enemiesInRange[shoot]->hp -= damage;
        }
        else{
            damage += damageModifier;
            enemiesInRange[shoot]->hp -= damage;
        }
        printf("%s %s's shot connected. \nEnemy damaged for %d hp. \n", un->rank, un->name, damage);
    }
    system("pause");
    return;
}
//Units can throw grenades in a 7 square radius. ALL units within a three square radius of the grenade are damaged. ALL Terrain in radius is destroyed.
void grenade(int board[BOARD_Y][BOARD_X], unit *un, enemyUnit enemies[NUM_ENEMIES], unit squad[6]){
    int x, y, i, j, k;
    if(un->grenades == 0){
        printf("Unit is out of grenades.\n");
        system("pause");
        un->ap+=2; //Gives unit their action point back. No grenade is thrown.
        return;
    }
    //Top coordinate system
    printf("  ");
    for(i = 0; i < BOARD_X; i++)
        printf("%d ", i%10);
    printf("\n");


    for(i = 0; i < BOARD_Y; i++){
        printf("%d ", i%10);
        for(j = 0; j < BOARD_X; j++){
            if(board[i][j] == 0 && (abs(i - un->location.y) + abs(j - un->location.x)) <= 7){
                printf("* ");
            }
            else{
                switch(board[i][j]){
                    case 0:
                        printf("_ ");
                        break;
                    case 1:
                        printf("X ");
                        break;
                    case 2:
                        printf("A ");
                        break;
                    case 3:
                        printf("%c ", 219); //176
                        break;
                    case 4:
                        printf("%c ", 220);
                        break;
                    default:
                        board[i][j] = 0;
                        printf("_ ");
                }
            }
        }
        printf("\n");
    }

    printf("Enter a location (X,Y) to obliterate marked by a \"*\" \n");
    scanf("%d %d", &x, &y);
    int naded = 0;
    while(!naded){
        if((abs(y - un->location.y) + abs(x - un->location.x)) <= 7){
            //printf("\a"); Not as funny as I hoped it would be
            for(i = y-2; i < y + 2; i++){
                for(j = x - 2; j < x + 2; j++){
                    if(board[i][j] == 3 || board[i][j] == 4){

                        printf("Cover at %d %d obliterated\n", j, i);
                        system("pause");
                        updateBoard(board, i, j, 0);
                    }
                    if(board[i][j] == 1){
                        for(k = 0; k < 6; k++){
                            if(squad[k].location.x == j && squad[k].location.y == i){
                                squad[k].hp -= 3;
                                printf("%s %s was damaged for 3 hp\n", squad[k].rank, squad[k].name);
                                system("pause");
                            }
                        }
                    }
                    if(board[i][j] == 2){
                        for(k = 0; k < NUM_ENEMIES; k++){
                            if(enemies[k].location.x == j && enemies[k].location.y == i){
                                enemies[k].hp -= 3;
                                printf("Alien at (%d, %d) was damaged for 3 hp\n", j, i);
                                system("pause");
                            }
                        }
                    }
                }
            }
            naded = 1;
            un->grenades--;
        }
        else{
            printf("\nEnter a location (X,Y) to obliterate marked by a \"*\" \n");
            scanf("%d %d", &x, &y);
        }
    }
}
/*
    Precondition: Player turn has started
    Postcondition: Unit has used all action points.
*/
void command(int board[BOARD_Y][BOARD_X], unit *un, unit squad[6], enemyUnit enemies[NUM_ENEMIES]){
    int choice;

    while(un->ap > 0){
        printBoard(board);
        printUnit(*un);
        printf("Menu: \n");
        printf("1). Move \n");
        printf("2). Shoot \n");
        printf("3). Grenade \n");
        printf("4). Reload \n");
        printf("5). Hunker Down \n"); //Unit uses remaining ap and does nothing.
        scanf("%d", &choice);
        switch(choice){
            case 1:
                moveUnit(board, un);
                un->ap--;
                break;
            case 2:
                shootUnit(board, un, enemies);
                un->ap -= 2;
                checkDeaths(board, enemies, squad);
                break;
            case 3:
                grenade(board, un, enemies, squad);
                un->ap-= 2; //grenades end turn. -=2 accounts for failed grenade throws unlike = 0.
                checkDeaths(board, enemies, squad); //Need to account for units that are already reported dead. Add a flag for death report
                break;
            case 4:
                un->ammo = un->clipSize;
                un->ap--;
                break;
            case 5:
                un->ap = 0; //Automatically ends turn
                break;
            default:
                printf("Invalid selection. \n");
        }
    }

    return;
}
/*
    Precondition: Game is initialized.
    Postcondition: Handles entirety of the player turn.
*/
void playerTurn(int board[BOARD_Y][BOARD_X], unit squad[6], enemyUnit enemies[NUM_ENEMIES], int *unitsLeft){
    int i, choice = -1;
    //int turnOverFlag = 0;
    //Refresh units
    for(i = 0; i < 6; i++){
        squad[i].ap = 2;
    }
    printf("YOUR TURN: \n");
    printBoard(board);
    while(choice != 1){ //Once you start commanding units you must finish
        printf("Menu: \n");
        printf("1. Command Units\n");
        printf("2. View Units\n");
        scanf("%d", &choice);
        if (choice == 2){
            printSquad(squad);
            system("pause");
            printBoard(board);
        }
    }
    for(i = 0; i < *unitsLeft; i++){
        //Check unit is alive first otherwise continue.
        if(squad[i].hp < 1)
            continue;
        command(board, &squad[i], squad, enemies);
    }

    return;
}
/*
    Precondition: Board is initialized.
    Postcondition: Enemy is in a new spot.
*/
void moveEnemy(int board[BOARD_Y][BOARD_X], enemyUnit *en, unit un){
    int i, j, temp, distance = 0;
    loc targetPosition;
    targetPosition.x = en->location.x;
    targetPosition.y = en->location.y;

    if(en->location.x > un.location.x){ //Enemy is to the right of the target
        for(i = 0; i < BOARD_Y; i++){
            for(j = 0; j < en->location.x; j++){
                temp = abs(en->location.x - j) + abs(en->location.y - i);
                if(temp > en->mobility) //Location outside unit mobility range
                    continue;
                if(board[i][j] != 0)
                    continue;

                if(temp > distance && board[i][j - 1] == 3 || board[i][j - 1] == 4){
                    targetPosition.x = j;
                    targetPosition.y = i;
                    break;
                }
            }
        }
    }
    else {
        for(i = 0; i < BOARD_Y; i++){
            for(j = en->location.x; j < BOARD_X ; j++){
                temp = abs(en->location.x - j) + abs(en->location.y - i);
                if(temp > en->mobility) //Location outside unit mobility range
                    continue;
                if(board[i][j] != 0)
                    continue;

                if(temp > distance && board[i][j + 1] == 3 || board[i][j + 1] == 4){
                    targetPosition.x = j;
                    targetPosition.y = i;
                    break;
                }
            }
        }
    }
    printf("ENEMY @ (%d, %d) MOVING ", en->location.x, en->location.y);
    updateBoard(board, en->location.y, en->location.x, 0);
    en->location.x = targetPosition.x;
    en->location.y = targetPosition.y;
    printf(" TO (%d, %d)\n", en->location.x, en->location.y);
    updateBoard(board, en->location.y, en->location.x, 2);
    return;
}
/*
    Precondition: Board is initialized.
    Postcondition: Enemy shoots at player unit.
*/
void shootEnemy(int board[BOARD_Y][BOARD_X], enemyUnit *en, unit *un){
    int cover, distance;
    double chance = 100.0;
    double shot;

    cover = calcCover(board, en->location, un->location);
    //Calculate hit chance (Aim -  2 * Distance – Cover Bonus)
        //determine cover bonus if applicable. 0 = no cover, 20 = half cover; 40 = full cover
        //Handles cover bonus depending on relative location of unit to target
    distance = abs(en->location.y - un->location.y) + abs(en->location.x - un->location.x);
    chance = en->aim - .5 * distance - cover;
    //printf("ENEMY CHANCE TO HIT %lf\n", chance);
    //Calculate hit success (pass /fail)
    srand(time(0));
    shot = rand() % 100 + 1;
    //printf("ENEMY ROLL %lf\n", shot);
    if(shot > chance){
        printf("%s %s evaded an enemy's shot. \nUnit unscathed.\n", un->rank, un->name);
    }
    else{
        int damage = 3;
        int damageModifier = 2;
        srand(time(0));
        double modify = rand() % 100 + 1;
        if(modify < 75){
            un->hp-= damage;
        }
        else if(modify > 75 && modify < 85){ //Subtract damage modifier
            damage -= damageModifier;
            un->hp -= damage;
        }
        else{
            damage += damageModifier;
            un->hp -= damage;
        }
        printf("%s %s was shot for %d hp. \n", un->rank, un->name, damage);
    }
    system("pause");

    return;
}
/*
    Precondition: Board is initialized.
    Postcondition: Enemy turn is completed.
*/
void enemyTurn(int board[BOARD_Y][BOARD_X], unit squad[6], enemyUnit enemies[NUM_ENEMIES], int *enemiesLeft){
    int i, j, temp, distance = 100000;
    unit *closest;
    printf("\n\nENEMY TURN:\n");
    for(i = 0; i < NUM_ENEMIES; i++){
        if(enemies[i].hp < 1)
            continue;
        //Find closest unit
        for(j = 0; j < SQUAD_SIZE; j++){
            temp = abs(squad[j].location.y - enemies[i].location.y) + abs(squad[j].location.x - enemies[i].location.x);
            if(temp < distance){
                distance = temp;
                closest = &squad[j];
            }
        }
        if(distance > enemies[i].aim / 5){ //If closet enemy unit is out of shooting range, move
            moveEnemy(board, &enemies[i], *closest);
        }
        else{//Otherwise shoot
            shootEnemy(board, &enemies[i], closest);
            checkDeaths(board, enemies, squad);
        }
    }
    system("pause");
    return;
}

/*
    Precondition: Board is initialized.
    Postcondition: Modifies board at location y, x with replace.
*/
void updateBoard(int board[BOARD_Y][BOARD_X], int r, int c, int replace){
    board[r][c] = replace;
    return;
}
/*
    Precondition: Board is initialized.
    Postcondition: Checks for freshly deceased units.
*/
void checkDeaths(int board[BOARD_Y][BOARD_X], enemyUnit enemies[NUM_ENEMIES], unit squad[6]){
    int i;
    for(i = 0; i < NUM_ENEMIES; i++){
        if(enemies[i].hp  < 1){
            printf("Alien at (%d, %d) is deceased.\n", enemies[i].location.x, enemies[i].location.y);
            system("pause");
            updateBoard(board, enemies[i].location.y, enemies[i].location.x, 0);
            enemies[i].location.x = -1;
            enemies[i].location.y = -1;
            enemies[i].report = 1;
        }
    }
    for(i = 0; i < 6; i++){
        if(squad[i].hp  < 1 && squad[i].report != 1){ //&&report != 1
            printf("%s %s is deceased.\n", squad[i].rank, squad[i].name);
            system("pause");
            strcat(squad[i].name, "*DECEASED*");
            updateBoard(board, squad[i].location.y, squad[i].location.x, 0);
            squad[i].location.x = -1;
            squad[i].location.y = -1;
            squad[i].report = 1;
        }
    }
}

//Prints Board
void printBoard(int board[BOARD_Y][BOARD_X]){
    int i, j;
    printf("  ");

    //Top coordinate system
    for(i = 0; i < BOARD_X; i++)
        printf("%d ", i%10);
    printf("\n");


    for(i = 0; i < BOARD_Y; i++){
        printf("%d ", i%10);
        for(j = 0; j < BOARD_X; j++){
            switch(board[i][j]){
                case 0:
                    printf("_ ");
                    break;
                case 1:
                    printf("X ");
                    break;
                case 2:
                    printf("A ");
                    break;
                case 3:
                    printf("%c ", 219); //176
                    break;
                case 4:
                    printf("%c ", 220);
                    break;
                default:
                    board[i][j] = 0;
                    printf("_ ");
            }
            //switch to convert number into display character.
        }
        printf("\n");
    }

    return;
}
//Prints single unit
void printUnit(unit un){
    printf(" %s %s, %s: \n\tHP: %d | AMMO: %d | GRENADES: %d\n\tAIM: %d | MOBILITY %d\n\tLOCATION: x:%d, y:%d\n\n",
               un.rank, un.name, un.unitClass, un.hp, un.ammo, un.grenades, un.aim, un.mobility, un.location.x, un.location.y);
}
/*
    Precondition: UNIT MAX HEALTH MUST BE <= 10 OR IT WON'T BE PRINTED
                  Squad array is initialized.
    Postcondition: Prints remaining alive squad members.
*/
void printSquad(unit squad[6]){
    int i;
    for(i = 0; i < 6; i++){
        if(squad[i].hp > 10 || squad[i].hp < 1)
            continue;
        printf("%d).", i+1);
        printUnit(squad[i]);
    }
    return;
}
/*
    Precondition: UNIT MAX HEALTH MUST BE <= 10 OR IT WON'T BE PRINTED
                  Squad array is initialized.
    Postcondition: Prints remaining alive enemies members.
*/
void printEnemySquad(enemyUnit enemies[NUM_ENEMIES]){
    int i;
    for(i = 0; i < NUM_ENEMIES; i++){
        if(enemies[i].hp > 10 || enemies[i].hp < 1)
            continue;
        printf("%d). Enemy at (%d, %d)\n", i+1, enemies[i].location.x, enemies[i].location.y);

    }
    return;
}
//Returns 1 for player defeat, 2 for enemy defeat, 0 for no defeat
int checkEnd(unit squad[SQUAD_SIZE], enemyUnit enemies[NUM_ENEMIES]){
    int i, flag = -1;
    for(i = 0; i < SQUAD_SIZE; i++){
        if(squad[i].hp > 0){
           flag = 1;
           break;
        }
    }
    if(flag < 0){
        printf(" _____    ______   ______   ______              _______ \n");
        printf("|  __ \\  |  ____| |  ____| |  ____|     /\\     |__   __|\n");
        printf("| |  | | | |__    | |__    | |__       /  \\       | |   \n");
        printf("| |  | | |  __|   |  __|   |  __|     / /\\ \\      | |   \n");
        printf("| |__| | | |____  | |      | |____   / ____ \\     | |   \n");
        printf("|_____/  |______| |_|      |______| /_/    \\_\\    |_|   \n");


        return 1;
    }

    flag = -1;
    for(i = 0; i < NUM_ENEMIES; i++){
        if(enemies[i].hp > 0){
           flag = 1;
           break;
        }
    }
    if(flag < 0){
        printf(" __      __  _____    _____   _______    ____    _____   __     __\n");
        printf(" \\ \\    / / |_   _|  / ____| |__   __|  / __ \\  |  __ \\  \\ \\   / /\n");
        printf("  \\ \\  / /    | |   | |         | |    | |  | | | |__) |  \\ \\_/ / \n");
        printf("   \\ \\/ /     | |   | |         | |    | |  | | |  _  /    \\   /\n");
        printf("    \\  /     _| |_  | |____     | |    | |__| | | | \\ \\     | |   \n");
        printf("     \\/     |_____|  \\_____|    |_|     \\____/  |_|  \\_\\    |_|\n");
        return 2;
    }

    return 0;
}
