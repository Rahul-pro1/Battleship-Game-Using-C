#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BOARD_SIZE 10

struct Node {
    int x;
    int y;
    struct Node* next;
};

struct Node* playerList = NULL;
struct Node* computerList = NULL;
int playerAttacks[BOARD_SIZE][BOARD_SIZE] = {{0}};  // Initialize to 0; 1 means the coordinate has been attacked

void initializeBoard(char board[][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

void display(char board[][BOARD_SIZE]) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

void displayHide(char board[][BOARD_SIZE]) {
    printf("   A  B  C  D  E  F  G  H  I  J\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 'X') printf(" X ");
            else if (board[i][j] == 'O') printf(" O ");
            else printf(" . ");
        }
        printf("\n");
    }
}

int isValidPosition(char board[][BOARD_SIZE], int row, int col, int n, int orientation) {
    if (orientation == 0) {
        return (row >= 0 && row < BOARD_SIZE && col >= 0 && col + n - 1 < BOARD_SIZE);
    }
    if (orientation == 1) {
        return (row >= 0 && row + n - 1 < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
    }
    return 0;
}

int hasOverlap(char board[][BOARD_SIZE], int row, int col, int n, int orientation) {
    if (orientation == 0) {
        for (int i = 0; i < n; i++) {
            if (board[row][col + i] != ' ') {
                return 1;
            }
        }
    }
    if (orientation == 1) {
        for (int i = 0; i < n; i++) {
            if (board[row + i][col] != ' ') {
                return 1;
            }
        }
    }
    return 0;
}

// Change the return type to int
int placeShip(char board[][BOARD_SIZE], char shipName, int n, int isPlayer) {
    int row, col, orientation;

    do {
        if (isPlayer) {
            printf("Enter the starting position (e.g., A0 or B3) for your %d-block ship '%c': ", n, shipName);
            char position[3];
            scanf("%s", position);

            if (position[0] < 'A' || position[0] > 'J' || position[1] < '0' || position[1] > '9' || position[2] != '\0') {
                printf("Invalid position. Use the format 'A0' to 'J9'.\n");
                continue;
            }

            row = position[1] - '0';
            col = position[0] - 'A';

            printf("Enter the orientation (0 for horizontal, 1 for vertical) for your ship '%c': ", shipName);
            scanf("%d", &orientation);
        } else {
            row = rand() % BOARD_SIZE;
            col = rand() % BOARD_SIZE;
            orientation = rand() % 2;

            // Check if the ship fits on the board
            if (!isValidPosition(board, row, col, n, orientation)) {
                // Don't display an error message for the computer
                continue;
            }

            // Check for overlapping ships
            if (hasOverlap(board, row, col, n, orientation)) {
                // Don't display an error message for the computer
                continue;
            }
        }

        // Check if the ship fits on the board (for the player)
        if (isPlayer && !isValidPosition(board, row, col, n, orientation)) {
            printf("Invalid position. Ship doesn't fit on the board.\n");
            continue;
        }

        // Check for overlapping ships (for the player)
        if (isPlayer && hasOverlap(board, row, col, n, orientation)) {
            printf("Invalid placement. Overlapping ships detected. Please choose a different position.\n");
            continue;
        }

        // If all checks pass, break out of the loop
        break;

    } while (1);  // Infinite loop; breaks are used to exit when a valid position is found

    if (orientation == 0) {
        for (int i = 0; i < n; i++) {
            board[row][col + i] = shipName;
        }
    }
    if (orientation == 1) {
        for (int i = 0; i < n; i++) {
            board[row + i][col] = shipName;
        }
    }

    // Record ship positions in the linked list
    struct Node* shipPosition = NULL;
    for (int i = 0; i < n; i++) {
        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->x = row + (orientation == 1 ? i : 0);
        newNode->y = col + (orientation == 0 ? i : 0);
        newNode->next = shipPosition;
        shipPosition = newNode;
    }

    while (shipPosition != NULL) {
        struct Node* temp = shipPosition;
        shipPosition = shipPosition->next;
        if (isPlayer) {
            temp->next = playerList;
            playerList = temp;
        } else {
            temp->next = computerList;
            computerList = temp;
        }
    }

    // Return 1 to indicate success
    return 1;
}

int isListEmpty(struct Node* head) {
    return head == NULL;
}

void deleteNode(struct Node** head, int x, int y, char playerName) {
    struct Node* current = *head;
    struct Node* prev = NULL;
    int i = 0;

    while (current != NULL) {
        if (current->x == x && current->y == y) {
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);

            i++;
            break; // Break out of the loop once a node is deleted
        }
        prev = current;
        current = current->next;
    }

    // Check if the entire ship is sunk
    if (*head == NULL) {
        if (playerName == 'P') {
            printf("You have sunk the %d-block ship of your opponent!!\n", i);
        } else if (playerName == 'C') {
            printf("The computer sank your %d-block ship!!\n", i);
        }
    }
}

int isNodeInList(struct Node* head, int x, int y) {
    struct Node* current = head;
    while (current != NULL) {
        if (current->x == x && current->y == y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void computerAttack(char board[][BOARD_SIZE]) {
    int computerGuessX, computerGuessY;

    do {
        computerGuessX = rand() % BOARD_SIZE;
        computerGuessY = rand() % BOARD_SIZE;
    } while (board[computerGuessX][computerGuessY] == 'X' || board[computerGuessX][computerGuessY] == 'O');

    if (isNodeInList(playerList, computerGuessX, computerGuessY)) {
        printf("Computer hit your ship at (%c%d)!\n", computerGuessY + 'A', computerGuessX);
        deleteNode(&playerList, computerGuessX, computerGuessY, 'P');
        if (isListEmpty(playerList)) {
            printf("Game Over! The computer sank all your ships. You lose!\n");
            exit(0);
        }
        board[computerGuessX][computerGuessY] = 'X';
    } else {
        printf("Computer missed your ships at (%c%d).\n", computerGuessY + 'A', computerGuessX);
        board[computerGuessX][computerGuessY] = 'O';
    }
}


void revealComputerShips(char board[][BOARD_SIZE], struct Node* shipList) {
    // Display the computer's board with ship positions
    printf("Revealing computer's board with ship positions:\n");
    printf("   A  B  C  D  E  F  G  H  I  J\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {

            if (isNodeInList(shipList, i, j)) {
                printf(" %c ", board[i][j]);
            } else {
                printf(" . ");
            }
        }
        printf("\n");
    }
}

int main() {
    srand(time(NULL));

    char playerBoard[BOARD_SIZE][BOARD_SIZE];
    char computerBoard[BOARD_SIZE][BOARD_SIZE];

    initializeBoard(playerBoard);
    initializeBoard(computerBoard);

    printf("Welcome to the Battleship Game!\n");

    display(playerBoard);

    printf("Please place your ships on the board.\n");

    // Place ship A
    if (!placeShip(playerBoard, 'A', 5, 1)) {
        return 1; // An error occurred during ship placement
    }
    display(playerBoard);

    // Place ship B
    if (!placeShip(playerBoard, 'B', 4, 1)) {
        return 1; // An error occurred during ship placement
    }
    display(playerBoard);

    // Place ship C
    if (!placeShip(playerBoard, 'C', 3, 1)) {
        return 1; // An error occurred during ship placement
    }
    display(playerBoard);

    // Place ship D
    if (!placeShip(playerBoard, 'D', 3, 1)) {
        return 1; // An error occurred during ship placement
    }
    display(playerBoard);

    // Place ship E
    if (!placeShip(playerBoard, 'E', 2, 1)) {
        return 1; // An error occurred during ship placement
    }
    display(playerBoard);

    printf("Your Board:\n");
    display(playerBoard);

    // Place computer ships
    placeShip(computerBoard, 'A', 5, 0);
    placeShip(computerBoard, 'B', 4, 0);
    placeShip(computerBoard, 'C', 3, 0);
    placeShip(computerBoard, 'D', 3, 0);
    placeShip(computerBoard, 'E', 2, 0);

    printf("Computer's Board:\n");
    displayHide(computerBoard);

    while (1) {
        // Player's turn
        char attackPosition[6];
        int playerGuessX, playerGuessY;  // Declare variables here

        do {
            printf("Enter your guess for attack (e.g., A0 or J9): ");
            scanf("%s", attackPosition);

            if (strcmp(attackPosition, "reveal") == 0) {
                // Reveal computer's ships
                revealComputerShips(computerBoard, computerList);
                continue;
            } else if (strcmp(attackPosition, "hide") == 0) {
                // Hide computer's ships
                displayHide(computerBoard);
                continue;
            }

            if (attackPosition[0] < 'A' || attackPosition[0] > 'J' || attackPosition[1] < '0' || attackPosition[1] > '9' || attackPosition[2] != '\0') {
                printf("Invalid attack position. Use the format 'A0' to 'J9'.\n");
                continue;
            }

            playerGuessX = attackPosition[1] - '0';
            playerGuessY = attackPosition[0] - 'A';

            if (playerAttacks[playerGuessX][playerGuessY]) {
                printf("You have already attacked this coordinate before. Please enter a different coordinate.\n");
            } else {
                playerAttacks[playerGuessX][playerGuessY] = 1;  // Mark the coordinate as attacked
                break;  // Break out of the loop if it's a new coordinate
            }
        } while (1);

        if (isNodeInList(computerList, playerGuessX, playerGuessY)) {
            printf("You hit the computer's ship at (%c%d)!\n", playerGuessY + 'A', playerGuessX);
            deleteNode(&computerList, playerGuessX, playerGuessY, 'P');
            if (isListEmpty(computerList)) {
                printf("Congratulations! You've sunk all the computer's ships. You win!\n");
                exit(0);
            }
            computerBoard[playerGuessX][playerGuessY] = 'X';
        } else {
            printf("You missed the computer's ships at (%c%d).\n", playerGuessY + 'A', playerGuessX);
            computerBoard[playerGuessX][playerGuessY] = 'O';
        }

        // Computer's turn
        computerAttack(playerBoard);

        // Display the updated boards
        printf("\nYour Board:\n");
        display(playerBoard);
        printf("\nComputer's Board:\n");
        displayHide(computerBoard);
    }

    // Free allocated memory for linked lists
    while (playerList != NULL) {
        struct Node* temp = playerList;
        playerList = playerList->next;
        free(temp);
    }

    while (computerList != NULL) {
        struct Node* temp = computerList;
        computerList = computerList->next;
        free(temp);
    }

    return 0;
}