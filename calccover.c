/*
int calcCover(int board[BOARD_Y][BOARD_X], unit un, enemyUnit en){
    int cover;
    //printf("ENEMY RECIEVED: %d, %d\n\n", en.location.x, en.location.y);
    if(un.location.x < en.location.x){ //Unit is left of target
        if(un.location.y < en.location.y){ // Left and above, left and above cover count, taking higher value
            int cover1, cover2;
            switch (board[en.location.y][--en.location.x]){ // Left Cover
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
            switch (board[--en.location.y][en.location.x]){ // Top Cover
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
        else if(un.location.y == en.location.y){// Directly Left, only left cover value counts
            switch (board[en.location.y][--en.location.x]){
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
            switch (board[en.location.y][--en.location.x]){ // Left Cover
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
            switch (board[++en.location.y][en.location.x]){ // Below Cover
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
    else if(un.location.x == en.location.x){ // Unit is directly above or below target
        if(un.location.y < en.location.y){ //Unit above target
            switch (board[--en.location.y][en.location.x]){
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
        else { //Unit is below target
            switch (board[++en.location.y][en.location.x]){
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
    else{ //Unit is right of target
        if(un.location.y < en.location.y){ // Right and above, right and above cover count, taking higher value
            int cover1, cover2;
            switch (board[en.location.y][++en.location.x]){ // Right Cover
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
            switch (board[--en.location.y][en.location.x]){ // Top Cover
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
        else if(un.location.y == en.location.y){// Directly Right, only left cover value counts
            switch (board[en.location.y][++en.location.x]){
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
            switch (board[en.location.y][++en.location.x]){ // Right Cover
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
            switch (board[++en.location.y][en.location.x]){ // Below Cover
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
}*/
