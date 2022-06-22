#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * stores information about the value of each cell and
 * the player on top of the cell. f
 */
typedef struct {
    int pointOfCell;
    char playerOnCell;
} Cell;

/*
 * An error handle function Intakes an error code and prints the
 * corresponding message to stderr. The function also returns the
 * corresponding value that matches the error message.
 */
int exit_error(int err) {
    switch (err) {
        //Incorrect number of args
        case 1:
            fprintf(stderr, "Usage: push2310 typeO typeX fname\n");
            return 1;
        //case 2 is the valid players(Player types are not 0,1,H)
        case 2:
            fprintf(stderr, "Invalid player type\n");
            return 2;
        //case 3 is cant read file
        case 3:
            fprintf(stderr, "No file to load from\n");
            return 3;
        //case 4 soemthing wrong with content of file
        case 4:
            fprintf(stderr, "Invalid file contents\n");
            return 4;
        //EOF on stdin when input required
        case 5:
            fprintf(stderr, "End of file\n");
            return 5;
        //The game being loaded has no empty cells in the interior
        case 6:
            fprintf(stderr, "Full board in load\n");
            return 6;
        //The default case meaning that game exits with no error
        default:
            return 0;
    }
}

/*
 * reads a line from the given file pointer and returns the line in the
 * form of a char pointer, otherwise the function returns NULL.
 */
char *read_line(FILE *file) {
    char *text = malloc(sizeof(char) * 10);
    int length = 0;
    int cap = 10;
    while (1) {
        if (length < cap) {
            int next = fgetc(file);
            //if newline or EOF then return text
            if (next == EOF || next == '\n') {
                text[length++] = '\0';//added this to fix valgrind
                return text;
            } else {
                text[length] = (char) next;
                length++;
            }
        }
        //check if length has reach mutiple of 10 and increase text by 10
        if (length % 10 == 0) {
            cap += 10;
            text = (char *) realloc(text, cap * sizeof(char));
        }
    }
    return NULL;
}

/*
 * This function is to read a file pointer from stdin and returns
 * the content of the stdin input as a char pointer. Otherwise returns Null if
 * the file pointer is invaild.
 */
char *read_stdin(FILE *file) {
    char *text = malloc(sizeof(char) * 10);
    int length = 0;
    int cap = 10;
    while (1) {
        if (length < cap) {
            int next = fgetc(file);
            if (next == EOF && length == 0) {//deal with no input for stdin
                return NULL;
            }
            if (next == EOF || next == '\n') {
                text[length++] = '\0';
                return text;
            } else {
                text[length] = (char) next;
                length++;
            }
        }
        //check if length has reach mutiple of 10 and increase text by 10
        if (length % 10 == 0) {
            cap += 10;
            text = (char *) realloc(text, cap * sizeof(char));
        }
    }
    return NULL;
}

/*
 * Reads and returns the first int inside the given char pointer and remove
 * the returned int from the given char pointer
 */
int read_int(char *line) {

    char *rem;

    int intValue = strtol(line, &rem, 10);

    for (int i = 0; i <= strlen(rem); i++) {
        line[i] = rem[i];
    }

    return intValue;
}

/*
 * helper function of load_each_cell.
 * creates and returns a right hand side corner cell from the given
 * file pointer and set the player ontop of the cell to be blank space
 * and value of cell to be -1.
 */
Cell load_right_corner_cell(FILE *file) {
    Cell cell;
    char whitespace = fgetc(file);
    fgetc(file);
    cell.playerOnCell = whitespace;
    cell.pointOfCell = -1;
    fgetc(file);
    return cell;
}

/*
 * helper function of load_each_cell
 * creates and returns a left hand side corner cell from the given
 * file pointer and set the player ontop of the cell to be blank space
 * and value of cell to be -1.
 */
Cell load_left_corner_cell(FILE *file) {
    Cell cell;
    char whitespace = fgetc(file);
    fgetc(file);
    cell.playerOnCell = whitespace;
    cell.pointOfCell = -1;
    return cell;
}

/*
 * creates and returns a corner cell from the given file pointer and
 * set the player type and value of the cell according to the content of the
 * file pointer.
 */
Cell load_each_cell(FILE *file, int loopcount, int loopcountmax, int 
        columnCount, int columnMax) {
    //deal with the whitespace  first row first column
    if (loopcount == 0 && columnCount == 0) {

        return load_left_corner_cell(file);
    } else if (loopcount == 0 && columnCount == columnMax) {
        //deal with first row last col
        return load_right_corner_cell(file);

    } else if (loopcount == loopcountmax && columnCount == 0) {
        //deal with last row first col
        return load_left_corner_cell(file);

    } else if (loopcount == loopcountmax && columnCount == columnMax) {
        //deal with last row last col
        Cell cell;
        char whitespace = fgetc(file);
        fgetc(file);
        char endOffile = fgetc(file);
        if (endOffile != '\n') {
            cell.playerOnCell = whitespace;
            cell.pointOfCell = -3;
            return cell;
        } else {
            cell.playerOnCell = whitespace;
            cell.pointOfCell = -1;
            return cell;
        }
    } else if (columnCount == columnMax) {
        //deal with last cell in other row
        Cell cell;
        char charvalue = fgetc(file);
        int value = charvalue - '0';
        char player = fgetc(file);
        fgetc(file);
        cell.pointOfCell = value;
        cell.playerOnCell = player;
        return cell;
    } else {
        //deal with interior cell
        Cell cell;
        char charvalue = fgetc(file);
        int value = charvalue - '0';
        char player = fgetc(file);
        cell.pointOfCell = value;
        cell.playerOnCell = player;
        return cell;
    }
}


/*
 * Caluclate the score of the game based on the given Cell**(2d array that
 * stores cells).Returns the player type of winner(X or O). if a draw occurs
 * the function will return T instead.
 */
char calculate_score(Cell **grid, int rowHeight, int columnHeight) {
    int i, j;
    int playerXscore = 0;
    int playerOscore = 0;
    //loops through the grid to calculate the score of Player X and O
    for (i = 1; i < (rowHeight - 1); i++) {
        for (j = 1; j < (columnHeight - 1); j++) {
            if (grid[i][j].playerOnCell == 'X') {
                playerXscore += grid[i][j].pointOfCell;

            } else if (grid[i][j].playerOnCell == 'O') {
                playerOscore += grid[i][j].pointOfCell;
            }

        }
    }
    if (playerXscore == playerOscore) {//returns T if draw
        return 'T';
    }
    if (playerOscore > playerXscore) {
        return 'O';
    }
    return 'X';

}

/*
 * checks if the given Cell**(2-D array that contains cell) is full internally
 * or not.Returns 0 if the board is full.Otherwise return 1.
 */
int check_full_board(Cell **grid, int rowHeight, int columnHeight) {
    int i, j;
    int count = 0;
    //checks if the internal cells are full or not
    for (i = 1; i < (rowHeight - 1); i++) {
        for (j = 1; j < (columnHeight - 1); j++) {
            if (grid[i][j].playerOnCell != '.') {
                count++;
            }

        }
    }
    if (count == (rowHeight - 2) * (columnHeight - 2)) {

        return 0;

    }
    return 1;

}

/*
 * a void function that is used to save the current board state into the given
 * file pointer. With parameter Cell** grid been the 2-D array that stores
 * all the cell's current statment and char currenthand been the current
 * player in turn.
 */
void save_board(FILE *filetoSave, Cell **grid, int number1, int number2,
        char currentHand) {
    //prints board dimensions to given file
    fprintf(filetoSave, "%d %d\n", number1, number2);

    //prints current player in turn to given file
    fprintf(filetoSave, "%c\n", currentHand);

    //loops through to print all cells
    for (int i = 0; i < number1; i++) {
        for (int j = 0; j < number2; j++) {
            if ((i == 0 && j == 0) || (i == 0 && j == number2 - 1)
                    || (i == number1 - 1 && j == 0) ||
                    (i == number1 - 1 && j == number2 - 1)) {
                fprintf(filetoSave, "%c", ' ');
                fprintf(filetoSave, "%c", ' ');
            } else {
                fprintf(filetoSave, "%d", grid[i][j].pointOfCell);
                fprintf(filetoSave, "%c", grid[i][j].playerOnCell);

            }
            if (j == number2 - 1) {
                fprintf(filetoSave, "\n");
            }
        }
    }


}

/*
 * print_board is a void function that prints the current state of cells
 * inside Cell** grid(2-D array that stores all the cell's current statment)
 * to stdout.
 */
void print_board(Cell **grid, int number1, int number2) {
    for (int i = 0; i < number1; i++) {
        for (int j = 0; j < number2; j++) {
            if ((i == 0 && j == 0) || (i == 0 && j == number2 - 1)
                    || (i == number1 - 1 && j == 0) ||
                    (i == number1 - 1 && j == number2 - 1)) {
                printf("%c", ' ');
                printf("%c", ' ');
            } else {
                printf("%d", grid[i][j].pointOfCell);
                printf("%c", grid[i][j].playerOnCell);
            }

            if (j == number2 - 1) {
                printf("\n");
            }
        }
    }
}


/*
 * checks if the given Cell** grid(the 2-D array that stores all the cells)
 * is vaild or not.Returns o if not vaild,otherwise 1.
 */
int check_vaild_grid(Cell **grid, int rowHeight, int columnHeight) {
    int i, j;
    for (i = 0; i < (rowHeight); i++) {
        for (j = 0; j < (columnHeight); j++) {
            //check if the internal cells contains 0
            if ((i >= 1 && i < rowHeight - 1) &&
                    (j >= 1 && j < columnHeight - 1)) {
                if (!(grid[i][j].pointOfCell >= 1 &&
                        grid[i][j].pointOfCell <= 9)) {
                    return 0;
                }
                //checks if interal cells contains X or O or .
                if(!(grid[i][j].playerOnCell == 'X' || grid[i][j].playerOnCell 
                        == 'O' || grid[i][j].playerOnCell == '.')) {
                    return 0;
                }
            } else {//deal with other cells
                //deal with corners whitespace
                if ((i == 0 && j == 0) || (i == 0 && j == columnHeight - 1) ||
                        (i == rowHeight - 1 && j == columnHeight - 1) ||
                        (i == rowHeight - 1 && j == 0)) {
                    
                    if (grid[i][j].pointOfCell != -1) {
                        return 0;
                    }

                } else {//deal with border case
                    if (grid[i][j].pointOfCell != 0) {
                        return 0;
                    }
                    //checks if interal cells contains X or O or .
                    if(!(grid[i][j].playerOnCell == 'X' || 
                            grid[i][j].playerOnCell == 'O' || 
                            grid[i][j].playerOnCell == '.')) {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

/*
 * a void function that performes the edge pushing play from top edges(row 0)
 * with Cell** been the 2-D array that stores all the cell's current states
 * ,emptycell been the row position of the empty cell and col been the column
 * edge push.
 */
void push_top(Cell **grid, int rowHeight, int columnHeight, char playerType,
        int emptyCell, int col) {
    Cell *buffer = malloc(emptyCell * sizeof(Cell));

    //populate empty temp array
    for (int i = 1; i < emptyCell; i++) {
        buffer[i] = grid[i][col];
    }
    //does actual pushing
    for (int r = emptyCell; r > 0; r--) {
        grid[r][col].playerOnCell = buffer[r - 1].playerOnCell;
    }

    grid[1][col].playerOnCell = playerType;
    free(buffer);

}

/*
 *  a void function that performes the edge pushing play from
 * left edges(column 0) with Cell** been the 2-D array that stores
 * all the cell's current states,emptycell been the column position of the
 * empty cell and col been the row of the edge push.
 */
void push_left(Cell **grid, int rowHeight, int columnHeight, char playerType,
        int emptyCell, int row) {
    Cell *buffer = malloc(columnHeight * sizeof(Cell));
    //populate buffer array
    for (int i = 1; i < emptyCell; i++) {
        buffer[i] = grid[row][i];
    }
    //does actual pushing
    for (int r = emptyCell; r > 0; r--) {
        grid[row][r].playerOnCell = buffer[r - 1].playerOnCell;
    }

    grid[row][1].playerOnCell = playerType;
    free(buffer);


}

/*
 * a void function that performes the edge pushing play from
 * right edges with Cell** been the 2-D array that stores
 * all the cell's current states,emptycell been the column position of the
 * empty cell and col been the row of the edge push.
 */
void push_right(Cell **grid, int rowHeight, int columnHeight, char playerType,
        int emptyCell, int row) {
    Cell *buffer = malloc(columnHeight * sizeof(Cell));
    //populate buffer array
    for (int i = (columnHeight - 2); i > emptyCell; i--) {
        buffer[i] = grid[row][i];
    }
    //does actual pushing
    for (int r = emptyCell; r < (columnHeight - 2); r++) {
        grid[row][r].playerOnCell = buffer[r + 1].playerOnCell;
    }

    grid[row][columnHeight - 2].playerOnCell = playerType;
    free(buffer);
}

/*
 * a void function that performes the edge pushing play from
 * bottom edges with Cell** been the 2-D array that stores
 * all the cell's current states,emptycell been the row position of the
 * empty cell and col been the column of the edge push.
 */
void push_bottom(Cell **grid, int rowHeight, int columnHeight, char playerType,
        int emptyCell, int col) {
    Cell *buffer = malloc(rowHeight * sizeof(Cell));

    //populate empty temp array
    for (int i = rowHeight - 2; i > emptyCell; i--) {
        buffer[i] = grid[i][col];
    }
    //does actual pushing
    for (int r = emptyCell; r < rowHeight - 1; r++) {
        grid[r][col].playerOnCell = buffer[r + 1].playerOnCell;
    }

    grid[rowHeight - 2][col].playerOnCell = playerType;
    free(buffer);


}

/*
 * a void function that will place the given player type
 * on the cell with the maximum value within the given 
 * Cell**(the 2-D array that stores all the cell's current states),
 */
void place_max(Cell **grid, int rowHeight, int columnHeight, char playerType) {
    int currentMax = 0;
    int coordX;
    int coordY;
    //find the coordinate with the highest value that is empty
    for (int i = 1; i < (rowHeight - 1); i++) {
        for (int j = 1; j < (columnHeight - 1); j++) {
            if (grid[i][j].playerOnCell == '.') {
                if (grid[i][j].pointOfCell > currentMax) {
                    currentMax = grid[i][j].pointOfCell;
                    coordX = i;
                    coordY = j;
                }
            }
        }
    }
    grid[coordX][coordY].playerOnCell = playerType;
    printf("Player %c placed at %d %d\n", playerType, coordX, coordY);

}

/*
 * helper function of computer_one, which calculates and performs edge pushing
 * from the top row if condtions are vaild. Returns 1 if edge push was vaild 
 * and sucessful, otherwise 0.
 */
int computer_top(Cell **grid, int rowHeight, int columnHeight, char playerType,
        char opponentType) {
    for (int j = 1; j < (columnHeight - 1); j++) {
        //check if cell to be placed is empty or not
        if (grid[0][j].playerOnCell == '.') {
            //check not pushing onto empty cell
            if (grid[1][j].playerOnCell != '.') {
                //check that row/column is not full
                if (grid[rowHeight - 1][j].playerOnCell == '.') {
                    int lengthEmpty;
                    //find the empty position
                    for (int i = 1; i < (rowHeight); i++) {

                        if (grid[i][j].playerOnCell == '.') {
                            lengthEmpty = i;
                            break;
                        }
                    }
                    int currentScore = 0;
                    for (int i = 1; i < lengthEmpty; i++) {
                        if (grid[i][j].playerOnCell == opponentType) {
                            currentScore += grid[i][j].pointOfCell;
                        }

                    }
                    //NOW CALCULATES consequence of push
                    int pushScore = 0;
                    for (int i = 2; i <= lengthEmpty; i++) {
                        if (grid[i - 1][j].playerOnCell == opponentType) {
                            pushScore += grid[i][j].pointOfCell;
                        }
                    }
                    //determines if push is okay or not 
                    if (pushScore < currentScore) {
                        push_top(grid, rowHeight, columnHeight, playerType,
                                lengthEmpty, j);
                        printf("Player %c placed at %d %d\n", playerType, 0,
                                j);

                        return 1;
                    }

                }
            }
        }
    }
    return 0;


}

/*
 * helper function of computer_one, which calculates and performs edge pushing
 * from the outer right column if condtions are vaild. 
 * Returns 1 if edge push was vaild and sucessful, otherwise 0.
 */
int computer_right(Cell **grid, int rowHeight, int columnHeight, 
        char playerType, char opponentType) {
    for (int j = 1; j < (rowHeight - 1); j++) {
        //check if cell is placed empty or not
        if (grid[j][columnHeight - 1].playerOnCell == '.') {
            //check if pushing to empty cell
            if (grid[j][columnHeight - 2].playerOnCell != '.') {
                //check if that column or row is full
                if (grid[j][0].playerOnCell == '.') {
                    int emptyCell;
                    for (int i = (columnHeight - 2); i >= 0; i--) {
                        if (grid[j][i].playerOnCell == '.') {
                            emptyCell = i;
                            break;
                        }

                    }
                    //calcualte the score before push
                    int currentScore = 0;
                    for (int i = (columnHeight - 2); i > emptyCell; i--) {


                        if (grid[j][i].playerOnCell == opponentType) {
                            currentScore += grid[j][i].pointOfCell;
                        }

                    }
                    //calculate the score if push occured
                    int pushScore = 0;
                    for (int i = (columnHeight - 3); i >= emptyCell; i--) {
                        if (grid[j][i + 1].playerOnCell == opponentType) {
                            pushScore += grid[j][i].pointOfCell;
                        }
                    }
                    //determine if push can be performed or not
                    if (pushScore < currentScore) {
                        push_right(grid, rowHeight, columnHeight, playerType,
                                emptyCell, j);
                        printf("Player %c placed at %d %d\n", playerType, j,
                                (columnHeight - 1));
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

/*
 * helper function of computer_one, which calculates and performs left
 * edge pushing from column 0 if condtions are vaild. 
 * Returns 1 if edge push was vaild and sucessful, otherwise 0.
 */
int computer_left(Cell **grid, int rowHeight, int columnHeight, 
        char playerType, char opponentType) {
    for (int j = (rowHeight - 2); j > 0; j--) {
        //check if cell to be place is empty or not
        if (grid[j][0].playerOnCell == '.') {
            //check if pushing to empty cell
            if (grid[j][1].playerOnCell != '.') {
                //check if that column or row is full
                if (grid[j][columnHeight - 1].playerOnCell == '.') {
                    int emptyCell;
                    for (int i = 1; i <= (columnHeight - 1); i++) {
                        if (grid[j][i].playerOnCell == '.') {
                            emptyCell = i;
                            break;
                        }
                    }
                    //calculate score before push
                    int currentScore = 0;
                    for (int i = 1; i < emptyCell; i++) {
                        if (grid[j][i].playerOnCell == opponentType) {
                            currentScore += grid[j][i].pointOfCell;
                        }

                    }
                    //calculate score if push occured
                    int pushScore = 0;
                    for (int i = 2; i <= emptyCell; i++) {
                        if (grid[j][i - 1].playerOnCell == opponentType) {
                            pushScore += grid[j][i].pointOfCell;
                        }
                    }
                    //determines if push is valid or not
                    if (pushScore < currentScore) {
                        push_left(grid, rowHeight, columnHeight, playerType,
                                emptyCell, j);
                        printf("Player %c placed at %d %d\n", playerType, j,
                                0);
                        return 1;
                    }


                }


            }

        }

    }
    return 0;
}

/*
 * helper function of computer_one, which calculates and performs edge pushing
 * from the bottom row if conditions are vaild. 
 * Returns 1 if edge push was vaild and successful, otherwise 0.
 */
int computer_bottom(Cell **grid, int rowHeight, int columnHeight, 
        char playerType, char opponentType) {
    for (int j = (columnHeight - 2); j > 0; j--) {
        //check if the edge is full or not
        if (grid[rowHeight - 1][j].playerOnCell == '.') {
            //check to ensure not pushing empty cell
            if (grid[rowHeight - 2][j].playerOnCell != '.') {
                if (grid[0][j].playerOnCell == '.') {
                    int emptylength;
                    for (int i = rowHeight - 2; i >= 0; i--) {

                        if (grid[i][j].playerOnCell == '.') {
                            emptylength = i;
                            break;
                        }
                    }
                    //calculate score before push
                    int currentScore = 0;
                    for (int i = (rowHeight - 2); i > emptylength; i--) {
                        if (grid[i][j].playerOnCell == opponentType) {
                            currentScore += grid[i][j].pointOfCell;
                        }

                    }
                    //calculate score if push occured
                    int pushScore = 0;
                    for (int i = (rowHeight - 3); i >= emptylength; i--) {
                        if (grid[i + 1][j].playerOnCell == opponentType) {
                            pushScore += grid[i][j].pointOfCell;
                        }
                    }
                    //determines if push is okay or not
                    if (pushScore < currentScore) {

                        push_bottom(grid, rowHeight, columnHeight, playerType,
                                emptylength, j);
                        printf("Player %c placed at %d %d\n", playerType,
                                (rowHeight - 1), j);
                        return 1;
                    }

                }

            }
        }
    }
    return 0;
}

/*
 * A function that moves as computer_one player for the given
 * player type char playerType.By performing  edge pushing to try lower 
 * the opponent's score. Return 1 if the move is vaild and successful.
 */
int computer_one(Cell **grid, int rowHeight, int columnHeight, 
        char playerType) {
    char opponentType = 'O';
    if (playerType == 'O') {
        opponentType = 'X';
    }
    //looks through the top row to look for push edge
    if (computer_top(grid, rowHeight, columnHeight, playerType,
            opponentType) == 1) {
        return 1;
    }

    //looks the right to look for push edge
    if (computer_right(grid, rowHeight, columnHeight, playerType,
            opponentType) == 1) {
        return 1;
    }

    //looks through the bottom for push edge
    if (computer_bottom(grid, rowHeight, columnHeight, playerType,
            opponentType) == 1) {
        return 1;
    }
    //looks through the left for push edge
    if (computer_left(grid, rowHeight, columnHeight, playerType,
            opponentType) == 1) {
        return 1;
    }

    //if no push then place at the max value cell that is empty
    place_max(grid, rowHeight, columnHeight, playerType);
    return 1;
}

/*
 * a function that moves as computer_zero inside the game by placing on 
 * internal edges for the given Cell**(2-D array that stores the cells).
 * Returns 1 for a successful move.
 */
int computer_zero(Cell **grid, int rowHeight, int columnHeight, 
        char playerType) {
    //loops for type O player top right to bottom left
    if (playerType == 'O') {

        for (int i = 1; i < (rowHeight - 1); i++) {
            for (int j = 1; j < (columnHeight - 1); j++) {
                
                if (grid[i][j].playerOnCell == '.') {
                    grid[i][j].playerOnCell = 'O';
                    printf("Player %c placed at %d %d\n", playerType, i, j);
                    return 1;

                }
            }
        }
    }
    //loops for X type comp 0 player bottom right to top left
    if (playerType == 'X') {

        for (int i = (rowHeight - 2); i > 0; i--) {
            for (int j = (columnHeight - 2); j > 0; j--) {
                if (grid[i][j].playerOnCell == '.') {
                    grid[i][j].playerOnCell = 'X';
                    printf("Player %c placed at %d %d\n", playerType, i, j);
                    return 1;

                }
            }
        }

    }

    return 1;

}

/*
 * Helper function for human_player. A function that checks if a top
 * edge push for the given coordinate(int column) is valid or not.
 * Return 1 for valid top edge push performed, otherwise return -1.
 */
int human_top_helper(Cell **grid, int rowHeight, int columnHeight,
        char playerType, int column) {
    if (grid[0][column].playerOnCell != '.') {

        return -1;
    }

    if (grid[rowHeight - 1][column].playerOnCell != '.') {

        return -1;
    }
    //check if pushing an empty cell
    if (grid[1][column].playerOnCell == '.') {
        return -1;
    } else {
        int lengthEmpty;

        //locates lengthEmpty
        for (int i = 1; i < (rowHeight); i++) {
            if (grid[i][column].playerOnCell == '.') {
                lengthEmpty = i;
                break;
            }
        }
        
        push_top(grid, rowHeight, columnHeight, playerType, lengthEmpty,
                column);

        return 1;
    }

}

/*
 * Helper function for human_player. A function that checks if a left
 * edge push for the given coordinate(int column) is valid or not.
 * Return 1 for valid top edge push performed, otherwise return -1.
 */
int human_left_helper(Cell **grid, int rowHeight, int columnHeight,
        char playerType, int row) {
    if (grid[row][0].playerOnCell != '.') {
        return -1;
    }
    if (grid[row][columnHeight - 1].playerOnCell != '.') {
        return -1;
    }
    //check if pushing an empty cell
    if (grid[row][1].playerOnCell == '.') {
        return -1;
    } else {
        int lengthEmpty;

        //locates lengthEmpty
        for (int i = 1; i < (columnHeight); i++) {
            if (grid[row][i].playerOnCell == '.') {
                lengthEmpty = i;
                break;
            }
        }
        push_left(grid, rowHeight, columnHeight, playerType, lengthEmpty, row);

        return 1;

    }
}

/*
 * Helper function for human_player. A function that checks if a right
 * edge push for the given coordinate(int column) is valid or not.
 * Return 1 for valid top edge push performed, otherwise return -1.
 */
int human_right_helper(Cell **grid, int rowHeight, int columnHeight,
        char playerType, int row) {
    if (grid[row][columnHeight - 1].playerOnCell != '.') {
        return -1;
    }
    //check if pushing to empty cell
    if (grid[row][columnHeight - 2].playerOnCell == '.') {
        return -1;

    }
    //check if that column or row is full
    if (grid[row][0].playerOnCell != '.') {
        return -1;

    } else {
        int emptyCell;
        for (int i = (columnHeight - 2); i >= 0; i--) {
            if (grid[row][i].playerOnCell == '.') {
                emptyCell = i;
                //printf("%d\n",i);
                break;
            }

        }
        push_right(grid, rowHeight, columnHeight, playerType, emptyCell, row);
        return 1;


    }

}

/*
 * Helper function for human_player. A function that checks if a bottom
 * edge push for the given coordinate(int column) is valid or not.
 * Return 1 for valid top edge push performed, otherwise return -1.
 */
int human_bottom_helper(Cell **grid, int rowHeight, int columnHeight,
        char playerType, int col) {
    //checks if the actual cell is empty of not
    if (grid[rowHeight - 1][col].playerOnCell != '.') {
        return -1;
    }
    //check to ensure not pushing empty cell
    if (grid[rowHeight - 2][col].playerOnCell == '.') {
        return -1;
    }
    //check to make not pushing on a full edge/col
    if (grid[0][col].playerOnCell != '.') {
        return -1;

    } else {
        int emptylength;
        for (int i = rowHeight - 2; i >= 0; i--) {

            if (grid[i][col].playerOnCell == '.') {
                emptylength = i;
                break;
            }
        }


        push_bottom(grid, rowHeight, columnHeight, playerType, emptylength,
                col);

        return 1;
    }
}

/*
 * a helper function that checks the coordinate of a human input(int row,
 * int column) is valid or not.Returns 1 for invalid otherwise 0.
 */
int check_human_input(int row, int column, int rowHeight, int columnHeight) {
    //check if human input coordinates are out of board size bound
    if (row > rowHeight - 1) {
        return -1;
    }
    if (row < 0) {
        return -1;
    }
    if (column > (columnHeight - 1)) {
        return -1;
    }
    if (column < 0) {
        return -1;
    }
    return 0;
}

/*
 * a helper function fo human_player that performs saving game from the 
 * given Cell**(2-d array that stores all the cells). The current game
 * state will be saved into the given file path(char* inputone).
 * return -1 regardless.
 */
int human_player_save(Cell **grid, int rowHeight, int columnHeight,
        char playerType, char *inputone) {
    
    inputone++;
    FILE *filetoSave = fopen(inputone, "w");
    //output save failes if filetosave is NULL
    if (filetoSave == NULL) {
        fprintf(stderr, "Save failed\n");
        return -1;
    }
    save_board(filetoSave, grid, rowHeight, columnHeight, playerType);
    fclose(filetoSave);
    return -1;


}

/*
 * a function that performs human_player inside the game by prompting 
 * user to input where they want to place at on the given Cell**(2-d array
 * that stores the cells).Returns -3 if human play is invalid, 
 * for a valid save return -1, valid play on board returns 1.
 */
int human_player(Cell **grid, int rowHeight, int columnHeight, 
        char playerType) {
    printf("%c:(R C)> ", playerType);
    char *inputone = read_stdin(stdin);
    if (inputone == NULL) {
        free(inputone);
        return -3;
    }
    if (strlen(inputone) < 3) {
        free(inputone);
        return -1;
    }
    if (inputone[0] == 's') {//deal with save game
        int save = human_player_save(grid, rowHeight, columnHeight, playerType,
                inputone);
        free(inputone);
        return save;
    }
    //deal with normal play
    if (inputone[0] != 's') {
        int row = read_int(inputone);
        int column = read_int(inputone);
        int humanInt = check_human_input(row, column, rowHeight, columnHeight);
        if (humanInt != 0) {
            free(inputone);
            return humanInt;
        }
        if (row == 0) {//place on top row
            free(inputone);
            return human_top_helper(grid, rowHeight, columnHeight, playerType,
                    column);
        } else if (column == 0) {//place on left col
            free(inputone);
            return human_left_helper(grid, rowHeight, columnHeight, playerType,
                    row);
        } else if (row == (rowHeight - 1)) {//place on bottom row
            free(inputone);
            return human_bottom_helper(grid, rowHeight, columnHeight,
                    playerType, column);
        } else if (column == (columnHeight - 1)) {//place on right col
            free(inputone);
            return human_right_helper(grid, rowHeight, columnHeight,
                    playerType, row);
        } else if (grid[row][column].playerOnCell == '.') {//place else where
            grid[row][column].playerOnCell = playerType;
            free(inputone);
            return 1;
        }
    }
    free(inputone);
    return -1;
}

/*
 * A helper function for game_main_loop.This function performs a around of
 * play for the current player in turn(char* handTurn). Returns 5 if the human
 * player play failed. Otherwise return 1.
 */
int player_turn(Cell **grid, int rowHeight, int columnHeight, char handTurn,
        char playType) {
    //play a turn if playtype is comp 0
    if (playType == '0') {
        computer_zero(grid, rowHeight, columnHeight, handTurn);
        print_board(grid, rowHeight, columnHeight);
    }
    //play a turn if play type is comp 1
    if (playType == '1') {
        computer_one(grid, rowHeight, columnHeight, handTurn);
        print_board(grid, rowHeight, columnHeight);
    }

    //play a turn if play type is human
    if (playType == 'H') {
        while (1) {
            int hPlayerFlag = human_player(grid, rowHeight, columnHeight,
                    handTurn);
            if (hPlayerFlag == -3) {
                return 5;
            }
            if (hPlayerFlag == 1) {
                break;
            }
        }
        print_board(grid, rowHeight, columnHeight);
    }

    return 1;

}

/*
 * A function that checks if the game is over or not
 * return 1 for gameover, otherwise 0.
 */
int check_gameover(Cell **grid, int rowHeight, int columnHeight) {
    if (check_full_board(grid, rowHeight, columnHeight) == 0) {
        char winner = calculate_score(grid, rowHeight, columnHeight);
        if (winner == 'T') {
            printf("Winners: O X\n");
            return 1;
        } else {
            printf("Winners: %c\n", winner);
            return 1;
        }
    }
    return 0;
}

/*
 * main game loop is a function that will keep players playing until the game
 * is over.Returns 5 if human player caused error,otherwise return 1. 3 is 
 * return in case if error is trigger.
 */
int game_main_loop(Cell **grid, int rowHeight, int columnHeight, char p1type,
        char p2type, char firstHand, char secondhand) {
    print_board(grid, rowHeight, columnHeight);
    while (1) {
        //TODO:CHECK FOR PARALLEL WINNERS
        if (firstHand == 'O') {

            if (player_turn(grid, rowHeight, columnHeight, firstHand,
                    p1type) == 5) {
                return 5;
            }

            if (check_gameover(grid, rowHeight, columnHeight) == 1) {
                return 1;
            }

            if (player_turn(grid, rowHeight, columnHeight, secondhand,
                    p2type) == 5) {
                return 5;
            }

            if (check_gameover(grid, rowHeight, columnHeight) == 1) {
                return 1;
            }

        }

        if (firstHand == 'X') {
            if (player_turn(grid, rowHeight, columnHeight, firstHand,
                    p2type) == 5) {
                return 5;
            }
            if (check_gameover(grid, rowHeight, columnHeight) == 1) {
                return 1;
            }
            if (player_turn(grid, rowHeight, columnHeight, secondhand,
                    p1type) == 5) {
                return 5;
            }
            if (check_gameover(grid, rowHeight, columnHeight) == 1) {
                return 1;
            }

        }

    }
    return 3;

}

/*
 * a void function that free all the dynamic memory that could only be freed at
 * the end of the game.
 */
void clean_up(Cell **grid, char *lineOne, char *lineTwo, int number1, 
        FILE *fp1) {
    for (int i = 0; i < number1; i++) {
        free(grid[i]);
    }
    free(grid);
    free(lineTwo);
    free(lineOne);
    fclose(fp1);
}

/*
 * a void function that free some of the dynamic memory used.
 */
void partial_cleanup(char *lineOne, char *lineTwo) {
    free(lineTwo);
    free(lineOne);
}

/*
 * a helper function for main the do arg checking, returns the error code 
 * triggerd.If no triggers,then return 0
 */
int checkarg(char *argOne, char *argTwo, FILE *fp1, char p1type,
        char p2type) {
    //check if the first 2 are lenght of 1
    if (strlen(argOne) != 1 || strlen(argTwo) != 1) {
        return 2;
    }
    //check if player type if 0 1 or H 
    if ((p1type != '0' && p1type != '1' && p1type != 'H') ||
            (p2type != '0' && p2type != '1' && p2type != 'H')) {
        return 2;
    }
    //check the given file to load game is valid
    if (fp1 == NULL) {
        return 3;
    }

    return 0;

}

/*
 *A function that intakes the Cell **(2-d array that stores all cells)
 * and check if the cell's content is valid or not
 * returns 1 for invalid,otherwise return 0
 */
int check_cell(Cell **grid, int row, int column) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (grid[i][j].pointOfCell == -3) {
                return 1;
            }
        }
    }
    return 0;

}

/*
 * a function that calles load_each_cell to populate a 2-d array with cells
 * from the information given by the FIle*.
 */
Cell **populate_cell(int number1, int number2, FILE *fp1) {
    //allocate dynamic mem for 2D array
    Cell **grid = malloc(number1 * sizeof(Cell *));

    for (int i = 0; i < number1; i++) {
        grid[i] = malloc(number2 * sizeof(Cell));
    }
    int max = (number1 - 1);
    int max2 = (number2 - 1);
    //populate 2d array
    for (int i = 0; i < number1; i++) {
        for (int j = 0; j < number2; j++) {
            grid[i][j] = load_each_cell(fp1, i, max, j, max2);
        }
    }
    return grid;
}

/*
 *Determine second hand is a function that returns the values of the second
 * hand based on the firsthand. Returns X if firsthand ='O',otherwise return O
 */
char return_second_hand(char firstHand) {
    char secondHand = 'X';
    if(firstHand == 'X') {
        secondHand = 'O';
        return secondHand;
    }
    return secondHand;

}

int main(int argc, char **argv) {
    if (argc != 4) {//check if argc is 4
        return exit_error(1);
    }
    char p1type = argv[1][0];
    char p2type = argv[2][0];
    FILE *fp1 = fopen(argv[3], "r");
    int argcheck = checkarg(argv[1], argv[2], fp1, p1type, p2type);
    if (argcheck != 0) {//does some arg cheking
        return exit_error(argcheck);
    }
    char *lineOne = read_line(fp1);//read the first line of the gamefile
    char *lineTwo = read_line(fp1);//read the second line of gamefile
    if (strlen(lineTwo) != 1) {//checks if second line only contains 1 playtype
        partial_cleanup(lineOne, lineTwo);
        return exit_error(4);
    }
    char firstHand = lineTwo[0];
    if (!(firstHand == 'X' || firstHand == 'O')) {
        partial_cleanup(lineOne, lineTwo);
        return exit_error(4);
    }
    char secondhand = return_second_hand(firstHand);
    int number1 = read_int(lineOne);//converts string into int for grid dim
    int number2 = read_int(lineOne);//converts string into int for grid dim
    if (number1 < 3 || number2 < 3) {
        partial_cleanup(lineOne, lineTwo);
        return exit_error(4); 
    }
    Cell **grid = populate_cell(number1, number2, fp1);
    if (check_cell(grid, number1, number2) == 1) {
        clean_up(grid, lineOne, lineTwo, number1, fp1);
        return exit_error(4);
    }
    if (check_vaild_grid(grid, number1, number2) == 0) {
        clean_up(grid, lineOne, lineTwo, number1, fp1);
        return exit_error(4);
    }
    if (check_full_board(grid, number1, number2) == 0) {
        clean_up(grid, lineOne, lineTwo, number1, fp1);
        return exit_error(6);
    }
    int mainLoopFlag = game_main_loop(grid, number1, number2, p1type, p2type,
            firstHand, secondhand);
    if (mainLoopFlag == 5) {
        clean_up(grid, lineOne, lineTwo, number1, fp1);
        return exit_error(5);
    }
    clean_up(grid, lineOne, lineTwo, number1, fp1);
    return 0;
}