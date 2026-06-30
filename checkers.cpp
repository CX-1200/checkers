#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;

struct GameData 
{
    string playerBlackName, playerWhiteName;
    int boardSize;
    char currentPlayer;
    int capturedWhite, capturedBlack;
};

struct GamePiece
{
    vector<vector<char>> coordinate;
    vector<vector<char>> powerGrid;
    int moveFromRow, moveFromCol;
    int moveToRow, moveToCol;
    int middleRow, middleCol;
    bool pieceCapture;

};

struct PlayerInputMove
{
    string playerMove; 
    int playerMoveRow, playerMoveCol;
};

struct SaveGamePiece
{
    int saveFromRow, saveFromCol;
    int saveToRow, saveToCol;
    int saveMiddleRow, saveMiddleCol;
};


// *********************************************************
// Function Prototypes
// *********************************************************

// Game setup - Get Player Name, Board Size, Save File
void gameSetup(GameData& );
void playerName(string&, string&);
int getBoardSize(int&);
int validateInput();

// Game state - Black and White pieces, King promotion
bool noPiecesLeft(GameData&, GamePiece&, char);
void announcement(GameData&, char, string);
void saveGame(GameData&, GamePiece&);
bool loadGame(GameData&, GamePiece&);

// Game play - Players' Movement, Eat
void displayBoard(GameData& , GamePiece&);
void initializeBoard(GameData& , GamePiece&);
void gamePlay(GameData& , GamePiece&);
void playerMove(GameData&, GamePiece&);
bool endTurn(GameData&, GamePiece&, SaveGamePiece&);

bool validateMove(GameData& , GamePiece&);
void convertInput(GamePiece&, PlayerInputMove&, string);
bool legalMove(GameData&, GamePiece&, SaveGamePiece&, char);
void checkKingPromotion(GameData&, GamePiece&);

// Superpowers
void chooseSuperpower(GameData&, GamePiece&, int, int);
bool hasSuperpower(GameData&, GamePiece&);
void doTeleport(GameData&, GamePiece&, int, int);
void doResurrection(GameData&, GamePiece&, int, int);
void doLight(GameData&, GamePiece&, int, int);
void doDark(GameData&, GamePiece&, int, int);
void activateSuperpower(GameData&, GamePiece&);

// Explaination
void checkerTutorial();
void powerTutorial();




// *********************************************************
// MAIN function
// *********************************************************
int main()
{
    int selectMenu;
    GameData data;
    GamePiece piece;

    cout << "============================================" << endl;
    cout << setw(32) << "Checkermania" << endl;
    cout << "============================================" << endl;

    cout << endl;
    cout << "1. New Game" << endl;
    cout << "2. Load Game" << endl;
    cout << "3. Exit" << endl;

    cout << "Choose: " << endl;
    selectMenu = validateInput();
    
    switch (selectMenu)
    {
        case 1: gameSetup(data);
                gamePlay(data,piece);
                break;
        case 2: if(loadGame(data,piece))
                playerMove(data,piece);
                break;
        case 3:
                break;
        default: 
                break;
    }

    return 0;

}

// Game setup - Player Name, Board Size, Save File

void gameSetup(GameData& data)
{
    playerName(data.playerBlackName, data.playerWhiteName);
    getBoardSize(data.boardSize);
    
    int tutorialGuide;
    cout << " Would you like a quick tutorial? (1 = Yes, 0 = Skip): " << endl;
    tutorialGuide  = validateInput();
        
        if (tutorialGuide == 1) 
        { 
            checkerTutorial(); 
        }
    
}

void playerName(string& playerBlackName, string& playerWhiteName)
{

    cout << "Enter your name:" << endl;
    cout << "Player 1 (Black): ";
    playerBlackName = validateInput();
    cout << endl;
    cout << "Player 2 (White): ";
    playerWhiteName = validateInput();
    cout << endl;
}

int getBoardSize(int& boardSize)
{
    cout << "Enter your board size (6 - 10): ";
    boardSize = validateInput();

    // Validate input between 6 - 10
    while (boardSize < 6 || boardSize > 10)
    {
        cout << "The board size must between 6 - 10: ";
        boardSize = validateInput();
    }

    return boardSize;
}

int validateInput()
{
    int value;
    while (!(cin >> value))
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    return value;
}

// Game state - Black and White pieces, King promotion
void announcement(GameData& data, char winner, string reason)
{
    string winnerName;
    if (winner == 'x')
    {
        winnerName = data.playerBlackName;
    } else
    {
        winnerName = data.playerWhiteName;
    }

    cout << endl;
    cout << "============================================" << endl;
    cout << "                 GAME OVER                 " << endl;
    cout << "============================================" << endl;
    cout << winnerName << ", you are the winner!" << endl;
    cout << "Reason: " << reason << endl;
    cout << "============================================" << endl;
}

bool noPiecesLeft(GameData& data, GamePiece& piece, char player)
{
    for (int row = 0; row < data.boardSize; row++)
    {
        for (int col = 0; col < data.boardSize; col++)
        {
            char remainingPieces = piece.coordinate[row][col];
            if (player == 'x' && (remainingPieces == 'x' || remainingPieces == 'X')) return false;
            if (player == 'o' && (remainingPieces == 'o' || remainingPieces == 'O')) return false;
        }
    }
    return true;
}

void saveGame(GameData& data, GamePiece& piece)
{
    string saveFilename;
    cout << "Enter save file name: ";
    cin >> saveFilename;
    saveFilename += ".txt";

    ofstream outFile(saveFilename);
    if (!outFile)
    {
        cout << "Error: Could not save file." << endl;
        return;
    }

    outFile << data.playerBlackName << endl;
    outFile << data.playerWhiteName << endl;
    outFile << data.boardSize << endl;
    outFile << data.currentPlayer << endl;
    outFile << data.capturedWhite << " " << data.capturedBlack << endl;

    for (int row = 0; row < data.boardSize; row++)
    {
        for (int col = 0; col < data.boardSize; col++)
        {
            outFile << piece.coordinate[row][col];
            outFile << piece.powerGrid[row][col];
        }
        outFile << endl;
    }

    outFile.close();
    cout << "Game saved as: " << saveFilename << endl;
}

bool loadGame(GameData& data, GamePiece& piece)
{
    string loadFilename;
    cout << "Enter save file name: ";
    cin >> loadFilename;
    loadFilename += ".txt";

    ifstream inFile(loadFilename);
    if (!inFile)
    {
        cout << "Error: File not found." << endl;
        return false;
    }

    getline(inFile, data.playerBlackName);
    getline(inFile, data.playerWhiteName);
    inFile >> data.boardSize;
    inFile >> data.currentPlayer;
    inFile >> data.capturedWhite >> data.capturedBlack;
    inFile.ignore();

    piece.coordinate = vector<vector<char>>(data.boardSize, vector<char>(data.boardSize, ' '));
    piece.powerGrid  = vector<vector<char>>(data.boardSize, vector<char>(data.boardSize, ' '));

    for (int row = 0; row < data.boardSize; row++)
    {
        string rowStr;
        getline(inFile, rowStr);
        for (int col = 0; col < data.boardSize; col++)
        {
            piece.coordinate[row][col] = rowStr[col * 2];
            piece.powerGrid[row][col]  = rowStr[col * 2 + 1];
        }
    }

    inFile.close();
    cout << "Game loaded successfully." << endl;
    return true;
}

// Game play - Players' Movement, Eat

void displayBoard(GameData& data, GamePiece& piece)
{

    char labelRow = 'A';
    
    cout << endl;
    
    for(int row = 0; row < data.boardSize; row++)
    {
        // Border
        for(int headerRow = 0; headerRow < data.boardSize; headerRow++)
        {
            cout << "|-----|";
        }
        
        cout << endl;

        // Content
        for(int col = 0; col < data.boardSize; col++)
        {
            char cellChar = piece.coordinate[row][col];
            char powerPiece = piece.powerGrid[row][col];
            bool kingWithPower = (cellChar == 'X' || cellChar == 'O') && powerPiece != ' ';

            if (kingWithPower)
            {
                cout << "| " << cellChar << " " << powerPiece << " |";
            } else
            {
                cout << "|  " << cellChar << "  |";
            }
        }

        cout << " " << labelRow++;
        cout << endl;

    }

    for(int footerRow = 0; footerRow < data.boardSize; footerRow++)
    {
        cout << "|-----|";
    }

    cout << endl;

    for (int labelColumn = 0; labelColumn < data.boardSize; labelColumn++)
    {
        cout << "   " << labelColumn + 1 << "   ";
    }
        
    cout << endl << endl;

    cout << "Men: white = o, black = x" << setw(30);
    cout << "Kings: white = O, black = X" << endl << endl;

    cout << "Power tags: " << endl;
    cout << "T=Teleport  R=Resurrect  L=Light  D=Dark  (lowercase = used)" << endl << endl;

    cout << "Captured men -> White: " << data.capturedWhite << "   Black: " << data.capturedBlack << endl;

}

void initializeBoard(GameData& data, GamePiece& piece)
{
    int rowSide = (data.boardSize / 2) - 1;
    piece.coordinate = vector<vector<char>>(data.boardSize, vector<char>(data.boardSize, ' '));
    piece.powerGrid = vector<vector<char>>(data.boardSize, vector<char>(data.boardSize, ' '));

    for (int row = 0; row < data.boardSize; row++)
    {
        for(int col = 0; col < data.boardSize; col++)
        {
            if((row + col) % 2 == 1)
            {
                if(row < rowSide)
                {
                    piece.coordinate[row][col] = 'o';  // White piece
                } else if (row >= (data.boardSize - rowSide))
                {
                    piece.coordinate[row][col] = 'x';  // Black piece
                } else
                {
                    piece.coordinate[row][col] = '.';
                }
                
            }
        }
    }

}

void gamePlay(GameData& data, GamePiece& piece)
{       
    initializeBoard(data,piece);

    data.capturedBlack = 0;
    data.capturedWhite = 0;
    data.currentPlayer = 'x';

    playerMove(data,piece);
}

void playerMove(GameData& data, GamePiece& piece)
{
    PlayerInputMove fromInput, toInput;
    SaveGamePiece savedState;
       
    while (true)
    {
        displayBoard(data,piece);          
        string currentPlayerName;
        string playerMoveFrom, playerMoveTo;
        int playerAction;

        if (data.currentPlayer == 'x')
        {
            currentPlayerName = data.playerBlackName;
        } else
        {
            currentPlayerName = data.playerWhiteName;
        }

        cout << currentPlayerName << "'s turn (" << data.currentPlayer << ")" << endl;

        cout << "1. Move" << endl;
        cout << "2. Surrender" << endl;
        cout << "3. Save Game" << endl;
        cout << "4. Quit Game" << endl;

        if (hasSuperpower(data, piece))
        cout << "5. Use Superpower" << endl;

        cout << "Choose: ";
        playerAction = validateInput();

        if (playerAction == 2)
        {
            char winner;
            if (data.currentPlayer == 'x')
            {
                winner = 'o';
            } else
            {    
                winner = 'x';
            }

            announcement(data, winner, "Opponent surrendered.");
            break;

        } else if (playerAction == 3)
        {
            saveGame(data,piece);
        } else if (playerAction == 4)
        {
            char saveChoice;
            cout << "Save before quitting? (y/n): ";
            cin >> saveChoice;

            if (saveChoice == 'y' || saveChoice == 'Y')
            {
                saveGame(data, piece);
                cout << "Game exited." << endl;
            }
            exit(0);

        } else if (playerAction == 1)
        {
            cout << "Enter coordinates (example: A1, B2):" << endl;
            cout << "From: ";
            cin >> playerMoveFrom;
            cout << "To: ";
            cin >> playerMoveTo;

            convertInput(piece, fromInput, playerMoveFrom);
            piece.moveFromRow = fromInput.playerMoveRow;
            piece.moveFromCol = fromInput.playerMoveCol;

            convertInput(piece, toInput, playerMoveTo);
            piece.moveToRow = toInput.playerMoveRow;
            piece.moveToCol = toInput.playerMoveCol;

            piece.pieceCapture = false;

            if (validateMove(data, piece))
            {
                piece.coordinate[piece.moveToRow][piece.moveToCol] = piece.coordinate[piece.moveFromRow][piece.moveFromCol];
                piece.coordinate[piece.moveFromRow][piece.moveFromCol] = '.';

                if (piece.pieceCapture)
                {
                    char captured = piece.coordinate[piece.middleRow][piece.middleCol];
                    piece.coordinate[piece.middleRow][piece.middleCol] = '.';
                    piece.powerGrid[piece.middleRow][piece.middleCol] = ' ';

                    if (captured == 'o' || captured == 'O')
                        data.capturedWhite++;
                    else
                        data.capturedBlack++;
                }

                checkKingPromotion(data, piece);

                if (endTurn(data, piece, savedState))
                break;

            } else
            {
                cout << "Invalid move! Try again." << endl;
            }
        } else if (playerAction == 5)
        {
            if (!hasSuperpower(data, piece))
            {
                cout << "No active superpowers available." << endl;
            } else
            {
                activateSuperpower(data, piece);
                if (endTurn(data, piece, savedState))
                break;
            }
        } else
        {
            cout << "Invalid choice. Please re-enter." << endl;
        }
    }
}

bool endTurn(GameData& data, GamePiece& piece, SaveGamePiece& savedState)
{
    char opponent;
    if (data.currentPlayer == 'x')
        opponent = 'o';
    else
        opponent = 'x';

    if (noPiecesLeft(data, piece, opponent))
    {
        displayBoard(data, piece);
        announcement(data, data.currentPlayer, "All opponent pieces captured.");
        return true;
    }

    if (data.currentPlayer == 'x')
        data.currentPlayer = 'o';
    else
        data.currentPlayer = 'x';

    if (!legalMove(data, piece, savedState, data.currentPlayer))
    {
        displayBoard(data, piece);
        char winner;
        if (data.currentPlayer == 'x')
        {
            winner = 'o';
        } else
        {    
            winner = 'x';
        }

        announcement(data, winner, "Opponent has no legal moves.");

        return true;
    }

    return false;
}

bool validateMove(GameData& data, GamePiece& piece)
{
    piece.pieceCapture = false;

    if (piece.moveToRow < 0 || piece.moveToRow >= data.boardSize || piece.moveToCol < 0 || piece.moveToCol >= data.boardSize)
    {
        return false;
    }

    if (piece.moveFromRow < 0 || piece.moveFromRow >= data.boardSize || piece.moveFromCol < 0 || piece.moveFromCol >= data.boardSize)
    {
        return false;
    }

    char pieceLocation = piece.coordinate[piece.moveFromRow][piece.moveFromCol];
    
    bool moveBlackPiece = (pieceLocation == 'x' || pieceLocation == 'X');
    bool moveWhitePiece = (pieceLocation == 'o' || pieceLocation == 'O');

    if(data.currentPlayer == 'x' && !moveBlackPiece)
    {
        return false;
    }
    
    if(data.currentPlayer == 'o' && !moveWhitePiece)
    {
        return false;
    }

    bool moveKingPiece = (pieceLocation == 'X' || pieceLocation == 'O');
    int moveForward;

    //int moveForward = moveBlackPiece ? -1 : 1;
    if (moveBlackPiece)
    {
        moveForward = -1;
    }   else
    {
        moveForward = 1;
    }

    int distanceRow = piece.moveToRow - piece.moveFromRow;
    int distanceCol = piece.moveToCol - piece.moveFromCol;

    // Validate if it is a diagonal move
    if (abs(distanceRow) != abs(distanceCol))
    {
        return false;
    }

    // Must move to empty space
    if (piece.coordinate[piece.moveToRow][piece.moveToCol] != '.')
    {
        return false;
    }

    if (abs(distanceRow) == 1)
    {
        if(!moveKingPiece && distanceRow != moveForward)
        {
            return false;
        }

        return true;
    }

    if (abs(distanceRow) == 2)
    {
        piece.middleRow = piece.moveFromRow + distanceRow / 2;
        piece.middleCol = piece.moveFromCol + distanceCol / 2;

        char midPiece = piece.coordinate[piece.middleRow][piece.middleCol];

        bool opponentPiece = false;
        if (data.currentPlayer == 'x')
        {
            opponentPiece = (midPiece == 'o' || midPiece == 'O');
        } else
            {
            opponentPiece = (midPiece == 'x' || midPiece == 'X');
            }

        if (!opponentPiece) 
        {
            return false;
        }

        if (!moveKingPiece && distanceRow != moveForward * 2) 
        {
            return false;
        }
        piece.pieceCapture = true;

        return true;
    }

    return false;

}

void convertInput(GamePiece& piece, PlayerInputMove& playerInput, string playerMove)
{
    playerInput.playerMoveRow = toupper(playerMove[0]) - 'A';

    // Check if input exceeds 2 digits: F10, E10
    if (playerMove.length() >= 3)
    {
        playerInput.playerMoveCol = (playerMove[1] - '0') * 10 + (playerMove[2] - '0') - 1;
    } else
    {
        playerInput.playerMoveCol = playerMove[1] - '1';
    }
}

bool legalMove(GameData& data, GamePiece& piece,SaveGamePiece& savepiece, char player)
{
    int direction[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};

    savepiece.saveFromRow = piece.moveFromRow;
    savepiece.saveFromCol = piece.moveFromCol;

    savepiece.saveToRow = piece.moveToRow;
    savepiece.saveToCol = piece.moveToCol;
    
    savepiece.saveMiddleRow = piece.middleRow;
    savepiece.saveMiddleCol = piece.middleCol;

    bool saveCapture = piece.pieceCapture;
    char savePlayer = data.currentPlayer;

    data.currentPlayer = player;

    bool legalFound = false;

    for (int row = 0; row < data.boardSize && !legalFound; row++)
    {
        for (int col = 0; col < data.boardSize && !legalFound; col++)
        {
            char cellPiece = piece.coordinate[row][col];
            bool pieceBelongs;

            if (player == 'x') 
            {
                pieceBelongs = (cellPiece == 'x' || cellPiece == 'X');
            } else {
                pieceBelongs = (cellPiece == 'o' || cellPiece == 'O');
            }
            
            if (!pieceBelongs) continue;

            piece.moveFromRow = row;
            piece.moveFromCol = col;

            for (int distance = 0; distance < 4 && !legalFound; distance++)
            {
                int possibleSteps[2] = {1, 2};
                for (int s = 0; s < 2 && !legalFound; s++)
                {
                    piece.moveToRow = row + direction[distance][0] * possibleSteps[s];
                    piece.moveToCol = col + direction[distance][1] * possibleSteps[s];

                    if (validateMove(data, piece))
                    legalFound = true;
                }
            }
        }
    }

    piece.moveFromRow = savepiece.saveFromRow; piece.moveFromCol = savepiece.saveFromCol;
    piece.moveToRow   = savepiece.saveToRow;   piece.moveToCol   = savepiece.saveToCol;
    piece.middleRow   = savepiece.saveMiddleRow;  piece.middleCol   = savepiece.saveMiddleCol;
    piece.pieceCapture    = saveCapture;
    data.currentPlayer    = savePlayer;

    return legalFound;

}

void checkKingPromotion(GameData& data, GamePiece& piece)
{
    int promotionRow = piece.moveToRow;
    int promotionCol = piece.moveToCol;
    char piecePromote = piece.coordinate[promotionRow][promotionCol];

    bool promotion = false;

    if (piecePromote == 'o' && promotionRow == data.boardSize - 1)
    {
        piece.coordinate[promotionRow][promotionCol] = 'O';
        promotion = true;
    }
    else if (piecePromote == 'x' && promotionRow == 0)
    {
        piece.coordinate[promotionRow][promotionCol] = 'X';
        promotion = true;
    }

    if (promotion)
    {
        cout << "King promotion! You can choose a superpower: " << endl;
        chooseSuperpower(data, piece, promotionRow, promotionCol);
    }
}

// Superpowers
void chooseSuperpower(GameData& data, GamePiece& piece, int row, int col)
{
    char pieceKing = piece.coordinate[row][col];
    int choice;

    powerTutorial();

    cout << "1. Teleport (T)" << endl;
    cout << "2. Resurrection (R)" << endl;
    if (pieceKing == 'O')
    {
        cout << "3. Light (L)" << endl;
    } else
    {    
        cout << "3. Dark (D)" << endl;
    }
    
    cout << "Choose a superpower: ";
    choice = validateInput();

    if (choice == 1)
    {
        piece.powerGrid[row][col] = 'T';
    } else if (choice == 2)
    {
        piece.powerGrid[row][col] = 'R';
    }
    else if (choice == 3)
    {
        if (pieceKing == 'O')
            piece.powerGrid[row][col] = 'L';
        else
            piece.powerGrid[row][col] = 'D';
    } else
    {
        cout << "Invalid choice. No superpower assigned." << endl;
    }
}

bool hasSuperpower(GameData& data, GamePiece& piece)
{
    char player = data.currentPlayer;
    for (int row = 0; row < data.boardSize; row++)
    {
        for (int col = 0; col < data.boardSize; col++)
        {
            char p = piece.coordinate[row][col];
            char power = piece.powerGrid[row][col];
            
            bool isPlayerKing;
            if (player == 'x')
            {
                isPlayerKing = (p == 'X');
            } else
            {
                isPlayerKing = (p == 'O');
            }

            bool hasActive = (power == 'T' || power == 'R' || power == 'L' || power == 'D');
            if (isPlayerKing && hasActive)
                return true;
        }
    }
    return false;
}

void doTeleport(GameData& data, GamePiece& piece, int kingRow, int kingCol)
{
    PlayerInputMove inputDestination;
    string destination;
    cout << "Teleport! Enter destination (e.g. D4): ";
    cin >> destination;
    convertInput(piece, inputDestination, destination);

    int destinationRow = inputDestination.playerMoveRow;
    int destinationCol = inputDestination.playerMoveCol;

    if (destinationRow < 0 || destinationRow >= data.boardSize || destinationCol < 0 || destinationCol >= data.boardSize
    || piece.coordinate[destinationRow][destinationCol] != '.')
    {
        cout << "Invalid destination. Power wasted." << endl;
        piece.powerGrid[kingRow][kingCol] = 't';
        return;
    }

    piece.coordinate[destinationRow][destinationCol] = piece.coordinate[kingRow][kingCol];
    piece.coordinate[kingRow][kingCol] = '.';
    piece.powerGrid[destinationRow][destinationCol] = 't';
    piece.powerGrid[kingRow][kingCol] = ' ';
}

void doResurrection(GameData& data, GamePiece& piece, int kingRow, int kingCol)
{
    char player = data.currentPlayer;
    char normalPiece;
    if (player == 'x')
    {
        normalPiece = 'x';
    } else
    {
        normalPiece = 'o';
    }

    int capturedPool;
    if (player == 'x')
    {
        capturedPool = data.capturedBlack;
    } else
    {
        capturedPool = data.capturedWhite;
    }

    if (capturedPool == 0)
    {
        cout << "No captured pieces to resurrect." << endl;
        piece.powerGrid[kingRow][kingCol] = 'r';

        return;
    }

    int nearestDirection[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    int revivedPiece = 0;

    for (int direction = 0; direction < 4 && revivedPiece < 1 && capturedPool > 0; direction++)
    {
        int nearestRow = kingRow + nearestDirection[direction][0];
        int nearestCol = kingCol + nearestDirection[direction][1];

        if (nearestRow >= 0 && nearestRow < data.boardSize && nearestCol >= 0 && nearestCol < data.boardSize
            && piece.coordinate[nearestRow][nearestCol] == '.')
        {
            piece.coordinate[nearestRow][nearestCol] = normalPiece;
            capturedPool--;
            revivedPiece++;
        }
    }

    if (player == 'x')
        data.capturedBlack = capturedPool;
    else
        data.capturedWhite = capturedPool;

    cout << revivedPiece << " piece(s) resurrected." << endl;
    piece.powerGrid[kingRow][kingCol] = 'r';
}

void doLight(GameData& data, GamePiece& piece, int kingRow, int kingCol)
{
    cout << "Light flash! Make one extra move with this King." << endl;
    piece.powerGrid[kingRow][kingCol] = 'l';

    PlayerInputMove toInput;
    string extraMoveDestination;
    cout << "Extra move destination: ";
    cin >> extraMoveDestination;
    convertInput(piece, toInput, extraMoveDestination);

    piece.moveFromRow = kingRow;
    piece.moveFromCol = kingCol;
    piece.moveToRow = toInput.playerMoveRow;
    piece.moveToCol = toInput.playerMoveCol;

    if (validateMove(data, piece))
    {
        piece.coordinate[piece.moveToRow][piece.moveToCol] = piece.coordinate[kingRow][kingCol];
        piece.coordinate[kingRow][kingCol] = '.';
        piece.powerGrid[piece.moveToRow][piece.moveToCol] = 'l';
        piece.powerGrid[kingRow][kingCol] = ' ';

        if (piece.pieceCapture)
        {
            char captured = piece.coordinate[piece.middleRow][piece.middleCol];
            piece.coordinate[piece.middleRow][piece.middleCol] = '.';
            piece.powerGrid[piece.middleRow][piece.middleCol] = ' ';
            if (captured == 'o' || captured == 'O')
                data.capturedWhite++;
            else
                data.capturedBlack++;
        }
    }
    else
    {
        cout << "Invalid move. Power still consumed." << endl;
    }
}

void doDark(GameData& data, GamePiece& piece, int kingRow, int kingCol)
{
    cout << "Dark power! Choose an opponent piece to capture." << endl;
    cout << "Target (e.g. C3): ";

    PlayerInputMove targetInput;
    string captureTarget;
    cin >> captureTarget;
    convertInput(piece, targetInput, captureTarget);

    int targetRow = targetInput.playerMoveRow;
    int targetCol = targetInput.playerMoveCol;

    if (targetRow < 0 || targetRow >= data.boardSize || targetCol < 0 || targetCol >= data.boardSize)
    {
        cout << "Invalid target. Power wasted." << endl;
        piece.powerGrid[kingRow][kingCol] = 'd';
        return;
    }

    char targetPiece = piece.coordinate[targetRow][targetCol];
    bool isOpponent = (targetPiece == 'o' || targetPiece == 'O');

    if (!isOpponent)
    {
        cout << "Target must be an opponent piece. Power wasted." << endl;
        piece.powerGrid[kingRow][kingCol] = 'd';
        return;
    }

    piece.coordinate[targetRow][targetCol] = '.';
    piece.powerGrid[targetRow][targetCol] = ' ';
    data.capturedWhite++;
    cout << "Opponent piece eliminated by dark power!" << endl;
    piece.powerGrid[kingRow][kingCol] = 'd';
}

void activateSuperpower(GameData& data, GamePiece& piece)
{
    char player = data.currentPlayer;
    int powerKingRows[10], powerKingCols[10];
    int powerKingCount = 0;

    for (int row = 0; row < data.boardSize; row++)
    {
        for (int col = 0; col < data.boardSize; col++)
        {
            char pieceSuperpower = piece.coordinate[row][col];
            char power = piece.powerGrid[row][col];
            
            bool isPlayerKing;
            if (player == 'x')
            {
                isPlayerKing = (pieceSuperpower == 'X');
            } else
            {
                isPlayerKing = (pieceSuperpower == 'O');
            }

            bool hasActive = (power == 'T' || power == 'R' || power == 'L' || power == 'D');
            if (isPlayerKing && hasActive)
            {
                powerKingRows[powerKingCount] = row;
                powerKingCols[powerKingCount] = col;
                powerKingCount++;
            }
        }
    }

    cout << "Select a King to activate power:" << endl;
    for (int i = 0; i < powerKingCount; i++)
    {
        int r = powerKingRows[i];
        int c = powerKingCols[i];
        cout << i + 1 << ". King at " << (char)('A' + r) << (c + 1) << "  [" << piece.powerGrid[r][c] << "]" << endl;
    }

    int kingChoice;
    kingChoice = validateInput();

    if (kingChoice < 1 || kingChoice > powerKingCount)
    {
        cout << "Invalid choice." << endl;
        return;
    }

    int kingRow = powerKingRows[kingChoice - 1];
    int kingCol = powerKingCols[kingChoice - 1];
    char power = piece.powerGrid[kingRow][kingCol];

    if (power == 'T')
    {
        doTeleport(data, piece, kingRow, kingCol);
    } else if (power == 'R')
    {
        doResurrection(data, piece, kingRow, kingCol);
    } else if (power == 'L')
    {
        doLight(data, piece, kingRow, kingCol);
    } else if (power == 'D')
    {
        doDark(data, piece, kingRow, kingCol);
    }
}
// Explaination

void checkerTutorial() 
{
    cout << endl;
    cout << "  ---------------- HOW TO PLAY ----------------" << endl;
    cout << "  * The board uses chess-style squares: a Row letter" << endl;
    cout << "    plus a Column number, for example C5." << endl;
    cout << "  * Men are shown as o (white) and x (black)." << endl;
    cout << "  * Men move one step diagonally FORWARD onto an empty square." << endl;
    cout << "  * You capture by jumping diagonally over a neighbouring" << endl;
    cout << "    enemy piece onto the empty square just beyond it." << endl;
    cout << "  * Reach the far side of the board and your man is crowned a" << endl;
    cout << "    KING (O or X), which may move in all four diagonal directions." << endl;
    cout << "  * A few Kings may also unlock a rare one-time superpower -" << endl;
    cout << "    you'll get a full explanation the first time one appears." << endl;
    cout << "  * You win by capturing every enemy piece, or by leaving the" << endl;
    cout << "    opponent with no legal move." << endl;
    cout << "  ---------------------------------------------" << endl;
}

void powerTutorial() 
{
    cout << endl;
    cout << "  ************ SUPERPOWERS UNLOCKED ************" << endl;
    cout << "  Only THREE superpowers exist in the whole game, so only" << endl;
    cout << "  three Kings can ever hold one. Each is used a single time." << endl;
    cout << "  On the board a powered King shows a tag next to it:" << endl;
    cout << "    T  Teleport    - jump the King to any empty square." << endl;
    cout << "    R  Resurrect   - bring back up to 1 captured men beside it." << endl;
    cout << "    L  Light (white) - flash, then take one extra move." << endl;
    cout << "    D  Dark (black)  - a black hole devours one enemy, turn ends." << endl;
    cout << "  A lowercase tag (t, r, l, d) means that power was already used." << endl;
    cout << "  *********************************************" << endl;
}