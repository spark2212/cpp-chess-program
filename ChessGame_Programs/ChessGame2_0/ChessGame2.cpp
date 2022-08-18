// ChessGameRewrite.cpp

#include<iostream> //std::cout, cin, etc
#include<fstream> //std::ifstream
#include<string> //std::string, string functions
#include<vector> //std::vector, vector operations
#include<math.h> //operators
#include<algorithm> //std::count

#define BLACK -1
#define WHITE 1
#define EMPTY 0

#define LIGHT 0
#define DARK 1

#define CAPTURE_INDEX 32
#define ROWS_PER_RANK 4
#define NUM_FILES 8
#define NUM_RANKS 8
#define PIECES_PER_TEAM 16
#define MAX_PIECES 32
#define MAX_SQUARES 64

enum pieceType
{
KING = 1,
QUEEN = 2,
ROOK = 3,
BISHOP = 4,
KNIGHT = 5,
PAWN = 6,
NO_PIECE = 7,
MAX_TYPES = 8
};

//CLASS DEFINITIONS//////////////////////////////////
class Piece
{
private:
    pieceType type;
    int team;
    std::string name;
    std::string firstNameB;
    std::string firstNameW;
    char rank;
    char file;
    std::string location;
    int timesMoved;
    int timeSinceMoved;
    bool isCaptured;
    bool canCastle;
    bool isCastling;
    bool EPVul;
    bool canEP;
    int squareColor;

public:
    //ctor
    Piece(char f, char r, int i);

    //setters
    void setType(int i); 
    void setTeam(int i);
    void setName(pieceType tp, int tm, int turn);
    void setFirstNames(); 
    void setLocation(char f, char r); //sets location based on separate rank and file inputs
    void setLocation(std::string l); //sets location based on a single location input
    void resetTimeSinceMoved();
    void setIsCaptured(bool captive);
    void setCanCastle(bool allowed); //determines if a piece may be used in castling
    void setIsCastling(bool is00);
    void setEPVul(bool isVul); //determines if a piece is vulnerable to being captured by En Passant
    void setCanEP(bool canEP);

    //getters
    pieceType getType();
    int getTeam();
    std::string getName();
    std::string getFirstName(int turn);
    std::string getLocation();
    char getFile();
    char getRank();
    int getTimesMoved();
    int getTimeSinceMoved();
    bool getIsCaptured();
    bool getCanCastle();
    bool getIsCastling();
    bool getEPVul();
    bool getCanEP();
    int getSquareColor();

    //other functions
    void printName(int i, int j); //prints the name of the piece if the corresponding location matches, else prints nothing
    void addTimeMoved();
    void addTimeSinceMoved();
    void changeType(pieceType newType);
};

//FUNCTION DECLARATIONS///////////////////////////////////
//Initializers
std::vector<Piece> initializePieces(); 

//Printers
void printBoard(std::vector<Piece> &pieces, int turn, bool manualClaim, std::string finput);
void printCaptureBoxFar(std::vector<Piece> &pieces, int turn);
void printCaptureBoxNear(std::vector<Piece> &pieces, int turn);
void printTurn(int turnNum, int color, bool manualClaim, bool wasOffered);

//Finders
int findPiece(std::vector<Piece> &pieces, std::string location);
int findPiece(std::vector<Piece> &pieces, char f, char r);
int findEmpty(std::vector<Piece> &pieces, std::string location);
int findKing(std::vector<Piece> &pieces, int color);

//Piece manipulators
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
void removePiece(std::vector<Piece> &pieces, int i);
void promotePawn(std::vector<Piece> &pieces, int i);

//Legality checkers
int getLegalCastlingRookIndex(std::vector<Piece> &pieces, int i, int j0, std::string start, int oppoTurn);
bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
bool checkCanCapture(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
bool isDiagonal(std::string start, std::string end);

//End condition checkers
int countLegalMoves(std::vector<Piece> &pieces, int turn);
int getDraw(std::vector<Piece> &pieces, int turn, pieceType pMoved);

//MAIN FUNCTION DEFINITION////////////////////////////////
int main(int argc, char **argv)
{
    for(int num = 0; num < 3; num++)
        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

    std::cout << "Welcome to ChessGame2.cpp! \n" << std::endl;
    std::string finput{""};

    if(argc == 1)
        std::cout << "If you wish to save a record of your game, enter 'quit' to quit, \nthen relaunch while including a .txt file in the command line, \nelse enter 'start' to begin the game: ";
    else if(argc == 2)
        std::cout << "If you do not wish to save a record of a game, enter 'quit' to quit, \nthen relaunch without including a .txt file in the command line, \nelse enter 'start' to begin the game: ";
    else
    {
        std::cout << "Too many arguments passed. Exiting... " << std::endl;
        return 0;
    }

    std::string choice;

    while(choice != "quit" && choice != "start")
    {
        std::cin >> choice;

        for(int i = 0; i < choice.size(); i++)
        {
            choice[i] = tolower(choice[i]);
        }

        if(choice == "quit")
        {
            std::cout << "Quitting... " << std::endl;
            return 0;
        }
        else if(choice == "start")
        {
            break;
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');

            std::cout << "Please enter either 'quit' or 'start': ";

            continue;
        } 
    }
    bool manualClaim{false};

    if(argc == 2)
    {

        std::cout << "\nDo you wish to claim draws by threefold repetition and the fifty-move rule automatically (recommended) \nor manually (tournament standard)? \nEnter 'auto' or 'manual': ";

        while(choice != "auto" && choice != "manual")
        {
            std::cin >> choice;

            for(int i = 0; i < choice.size(); i++)
            {
                choice[i] = tolower(choice[i]);
            }

            if(choice == "auto")
            {
                std::cout << "\nAll draws will be automatically declared... " << std::endl;
                break;
            }
            else if(choice == "manual")
            {
                std::cout << "\nDraws via the 50-move rule and threefold repetition must be claimed. \nThe current list of moves will be printed above the board. " << std::endl;
                manualClaim = true;
                break;
            }
            else
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');

                std::cout << "\nPlease enter either 'quit' or 'start': ";

                continue;
            } 
        }
        std::string wName, bName;

        std::cout << "Enter White's name: ";
        std::cin >> wName;
        finput.append(wName);

        std::cin.clear();
        std::cin.ignore(10000, '\n');
        finput.append("\t vs. \t");

        std::cout << "Enter Black's name: ";
        std::cin >> bName;
        finput.append(bName);

        std::cin.clear();
        std::cin.ignore(10000, '\n');

        finput.append(2, '\n');
    }

    std::vector<Piece> pieces = initializePieces();

    static unsigned long int turnNum{1};
    int turn{WHITE};
    int oppoTurn{BLACK};
    printBoard(pieces, turn, manualClaim, finput);
    static int getDrawNum = getDraw(pieces, turn, PAWN);
    bool wasOffered{false};

    while(1)
    {
        std::string start;
        std::string end;
        printTurn(turnNum, turn, manualClaim, wasOffered);

        //Takes user input for the desired move
        std::cin >> start;
        std::cin >> end; 

        for(int m = 0; m < start.size(); m++)
            start[m] = tolower(start[m]);

        for(int n = 0; n < end.size(); n++)
            end[n] = tolower(end[n]);

        if(start == "i" && end == "resign")
        {
            if(turn == WHITE)
            {
                if(argc == 2)
                    finput.append("\nWhite resigns! Black wins! \n\t0 - 1");

                printBoard(pieces, oppoTurn, manualClaim, finput);
                std::cout << "White resigns! Black wins! " << std::endl;
                
                break;
            }
            else if(turn == BLACK)
            {
                if(argc == 2)
                    finput.append("\nBlack resigns! White wins! \n\t1 - 0");
                    
                printBoard(pieces, oppoTurn, manualClaim, finput);
                std::cout << "Black resigns! White wins! " << std::endl;
                
                break;
            }
        }
        else if(wasOffered == true && end == "draw")
        {
            printBoard(pieces, turn, manualClaim, finput);
            std::cout << "You have already extended a draw offer on this turn.";
            continue;
        }
        else if((start == "offer" || (start == "claim" && manualClaim == true)) && end == "draw")
        {
            wasOffered = true;
            printBoard(pieces, turn, manualClaim, finput);
            if(start == "claim")
                std::cout << "Your opponent has attempted to claim a draw. All draw claims double as draw offers... ";
                
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nYour opponent has offered a draw. Do you accept?\nEnter 'yes' to accept, 'no' to decline, or 'flip' to flip the board: " << std::endl;

            std::string response{""};

            while(response == "")
            {
                std::cin >> response;

                for(int n = 0; n < response.size(); n++)
                    response[n] = tolower(response[n]);

                std::cin.clear();
                std::cin.ignore(10000, '\n');
                
                if(response != "yes" && response != "no")
                {
                    if(response == "flip")
                    {
                        printBoard(pieces, oppoTurn, manualClaim, finput);
                        std::cout << "Board flipped.";
                    }
                    else
                    {
                        printBoard(pieces, turn, manualClaim, finput);
                        std::cout << "Invalid entry.";
                    }
                    std::cout << "\nYour opponent has offered a draw. Do you accept?\nEnter 'yes' to accept or 'no' to decline: " << std::endl;

                    response = "";
                }
            }

            if(response == "yes")
            {
                finput.append("(=)\nDraw offered and accepted! \n\n\t1/2 - 1/2");
                printBoard(pieces, turn, manualClaim, finput);
                std::cout << "\nDraw offered and accepted! " << std::endl;
            
                break;
            }
            else if(start == "claim")
            {
                if(getDrawNum == 3)
                {
                    finput.append("(=)\n\n\t1/2 - 1/2");
                    printBoard(pieces, oppoTurn, manualClaim, finput);
                    std::cout << "Threefold repetition rule invoked! It's a draw! " << std::endl;
                    break;
                }
                else if(getDrawNum >= 50)
                {
                    finput.append("(=)\n\n\t1/2 - 1/2");
                    printBoard(pieces, oppoTurn, manualClaim, finput);
                    std::cout << "50-move rule invoked! It's a draw! " << std::endl;
                    break;
                }
                else
                {
                    printBoard(pieces, turn, manualClaim, finput);
                    std::cout << "Claim invalid.";
                    continue;
                }
            }
            else if(response == "no")
            {
                printBoard(pieces, turn, manualClaim, finput);
                std::cout << "Draw offer declined. ";
                continue;
            }
        }

        pieceType pMoved{NO_PIECE}; 
        int startPiece = findPiece(pieces, start);
        if(startPiece != -1)
            pMoved = pieces[startPiece].getType(); //This will be used much later but needs to be initialized now.

        for(int e = 0; e < MAX_SQUARES; e++)
        {
            pieces[e].setIsCastling(false);
        }

        int i = findPiece(pieces, start);

        bool valid = movePiece(pieces, start, end, turn); //If the move is valid, performs the move and returns true. Else, returns false.
        
        if(valid == false)
        {
            printBoard(pieces, turn, manualClaim, finput);
            std::cout << "Invalid move. Try again.";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        else
        {
            if(argc == 2)
            {
                if(wasOffered == true)
                    finput.append("(=)");
                std::string turnStr = std::to_string(turnNum);
                if(turn == WHITE)
                {
                    finput.append("\n");
                    finput.append(turnStr);
                    finput.append(". ");
                    finput.append(start);
                    finput.append(" ");
                    finput.append(end);
                }
                else if(turn == BLACK)
                {
                    finput.append("\t\t");
                    finput.append(turnStr);
                    finput.append("... ");
                    finput.append(start);
                    finput.append(" ");
                    finput.append(end);
                }
            }
            if(pieces[i].getType() == PAWN && (end[1] == '1' || end[1] == '8'))
            {    
                printBoard(pieces, turn, manualClaim, finput); //prints the board and pieces
                promotePawn(pieces, i); //promotes the pawn to the desired piece

                if(argc == 2)
                {
                    finput.append("^");
                    pieceType t = pieces[i].getType();
                    if(t == QUEEN)
                        finput.append("Q");
                    else if(t == ROOK)
                        finput.append("R");
                    else if(t == BISHOP)
                        finput.append("B");
                    else if(t == KNIGHT)
                        finput.append("N");
                }
            }
                
            oppoTurn = turn;
            turn *= -1;
            if(turn == WHITE)
                turnNum++;

            for(int a = 0; a < MAX_SQUARES; a++) //Nothing to do but reset En Passant values
            {
                if(pieces[a].getTeam() == turn)
                    pieces[a].setEPVul(false);
                if(pieces[a].getTeam() != turn)
                    pieces[a].setCanEP(false);
            }

            printBoard(pieces, turn, manualClaim, finput);
        }

        bool isCheck{false};
        
        int k = findKing(pieces, turn);
        for(int j = 0; j < MAX_SQUARES; j++)
        {
            isCheck = checkCanCapture(pieces, pieces[j].getLocation(), pieces[k].getLocation(), oppoTurn);
            if(isCheck == true)
                break;
        }
        int countMoves = countLegalMoves(pieces, turn);

        if(isCheck == true && countMoves > 0)
        {
            std::cout << "Check! ";
            if(argc == 2)
                finput.append("+");
        }
        else if(isCheck == true && countMoves == 0)
        {
            
            std::cout << "Checkmate! ";
            if(oppoTurn == WHITE)
                std::cout << "White wins! " << std::endl;
            else if(oppoTurn == BLACK)
                std::cout << "Black wins! " << std::endl;

            if(argc == 2)
            {
                finput.append("# \n\n");
                if(oppoTurn == WHITE)
                    finput.append("\t1 - 0");
                else
                    finput.append("\t0 - 1");
            }

            break;
        }
        else if(isCheck == false && countMoves == 0)
        {
            std::cout << "No legal moves! Stalemate! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            break;
        }

        getDrawNum = getDraw(pieces, turn, pMoved);
        wasOffered = false;

        if(getDrawNum == 3 && manualClaim == false)
        {
            std::cout << "Threefold repetition rule invoked! It's a draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            break;
        }
        else if(getDrawNum == 50 && manualClaim == false)
        {
            std::cout << "50-move rule invoked! It's a draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            break;
        }
        else if(getDrawNum == 75)
        {
            std::cout << "75-move rule invoked! Automatic draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            break;
        }
        else if(getDrawNum == 100)
        {
            std::cout << "Insufficient material left! It's a draw! " <<std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            break;
        }
    } //End of loop

    if(argc == 2)
    {
        fopen(argv[1], "w");
        std::ofstream gameLog (argv[1]);

        gameLog << finput;

        gameLog.close();
    }

    std::cout << "Game Over. Goodbye! \n" <<std::endl;

    return 0;
}

//CLASS FUNCTION DEFINITIONS///////////////////////////////
//Piece class ctor
Piece::Piece(char f, char r, int i){
    setTeam(i);
    setType(i);
    setName(type, team, WHITE);
    setFirstNames();
    setLocation(f, r);
    setCanCastle(true);
    setEPVul(true);

    timesMoved = 0;
    timeSinceMoved = 0;
    isCaptured = false;
    isCastling = false;
    canEP = false;
    squareColor = DARK;
}

//Piece class setters
void Piece::setType(int i){
    if(team == WHITE) //This is necessary to save several lines. This allows me to initialize all pieces for both teams using the same mod (%) formula
        i += NUM_FILES;

    canCastle = false; //This is the default state for all pieces except kings and rooks. For kings and rooks, this value will be shortly changed to true
    EPVul = false; //This is the default value for all pieces. This can only ever be true for pawns, and only immediately after a pawn has moved two spaces

    if(team == EMPTY) //This overrides all else
        type = NO_PIECE;
    else if(i % PIECES_PER_TEAM == 9 || i % PIECES_PER_TEAM == 0)
    {   
        if(timesMoved == 0) //This needs to be conditional because of pawn promotion.
            canCastle = true;
        type = ROOK;
    }
    else if(i % PIECES_PER_TEAM == 10 || i % PIECES_PER_TEAM == 15)
        type = KNIGHT;
    else if(i % PIECES_PER_TEAM == 11 || i % PIECES_PER_TEAM == 14)
        type = BISHOP;
    else if(i % PIECES_PER_TEAM == 12)
        type = QUEEN;
    else if(i % PIECES_PER_TEAM == 13)
    {
        canCastle = true;
        type = KING;
    }
    else if(i % PIECES_PER_TEAM <= 8 && i % PIECES_PER_TEAM >= 1)
        type = PAWN;

    return;
} 
void Piece::setTeam(int i){
    if(i <= PIECES_PER_TEAM) //The first 16 pieces will be white
        team = WHITE;
    else if(i > PIECES_PER_TEAM && i <= MAX_SQUARES - PIECES_PER_TEAM) //The next 32 "pieces" will be empty squares
        team = EMPTY;
    else //The final 16 pieces will be black
        team = BLACK;

    return;
}
//This function takes as arguments the piece's team and type, along with whose turn it currently is, to generate the piece's name. The player whose turn it is will have their piece names surrounded by /\\ pointing away from them, while their opponent's pieces will be surrounded by \\/ pointing towards them. Empty pieces will only have their names displayed on dark squares.
void Piece::setName(pieceType tp, int tm, int turn){
    char c;

    if(tp == KING)
        c = 'k';
    else if(tp == QUEEN)
        c = 'q';
    else if(tp == ROOK)
        c = 'r';
    else if(tp == BISHOP)
        c = 'b';
    else if(tp == KNIGHT)
        c = 'n';
    else if(tp == PAWN)
        c = 'p';

    if(tm == EMPTY)
        name = "*****";
    else if(team == WHITE)
        c = toupper(c);
    else if(team == BLACK)
        c = tolower(c);
    
    if(tm == -1*turn){
        name = " \\/ ";
        name.insert(name.begin()+2, c);
    }
    else if(tm == turn){
        name = " /\\ ";
        name.insert(name.begin()+2, c);
    }

    return;
}
void Piece::setFirstNames(){
    std::string firstNameYou{" /"};
    firstNameYou.append(1, name[2]);
    firstNameYou.append("\\ ");
    
    std::string firstNameOppo{" \\"};
    firstNameOppo.append(1, name[2]);
    firstNameOppo.append("/ ");

    if(team == WHITE)
    {
        firstNameW = firstNameYou;
        firstNameB = firstNameOppo;
    }
    else if(team == BLACK)
    {
        firstNameB = firstNameYou;
        firstNameW = firstNameOppo;
    }
    else
    {
        firstNameB = "    ";
        firstNameW = "    ";
    }
    

    return;
}
void Piece::setLocation(char f, char r){
    location.clear();
    file = f;
    rank = r;
    location.append(1, f);
    location.append(1, r);

    return;
}
void Piece::setLocation(std::string l){
    location = l;
    file = l[0];
    rank = l[1];

    return;
}
void Piece::resetTimeSinceMoved(){
    timeSinceMoved = 0;
    return;
}
void Piece::setIsCaptured(bool captive){
    isCaptured = captive;
    return;
}
void Piece::setCanCastle(bool allowed){
    if(type != KING && type != ROOK)
        canCastle = false;
    else if(timesMoved != 0)
        canCastle = false;
    else
        canCastle = allowed;

    return;
}
void Piece::setEPVul(bool isVul){
    if(type != PAWN)
        EPVul = false;
    else if(timesMoved != 1)
        EPVul = false;
    else if(timeSinceMoved != 0)
        EPVul = false;
    else
        EPVul = isVul;

    return;
}
void Piece::setIsCastling(bool is00){
    isCastling = is00;
    return;
}
void Piece::setCanEP(bool EPable){
    canEP = EPable;
    return;
}

//Piece class getters
pieceType Piece::getType(){
    return type;
}
int Piece::getTeam(){
    return team;
}
std::string Piece::getName(){
    return name;
}
std::string Piece::getFirstName(int turn){
    if(turn == WHITE)
        return firstNameW;
    else if(turn == BLACK)
        return firstNameB;
    
    return "Error: whose turn is it?";
}
std::string Piece::getLocation(){
    return location;
}
char Piece::getFile(){
    return file;
}
char Piece::getRank(){
    return rank;
}
int Piece::getTimesMoved(){
    return timesMoved;
}
int Piece::getTimeSinceMoved(){
    return timeSinceMoved;
}
bool Piece::getIsCaptured(){
    return isCaptured;
}
bool Piece::getCanCastle(){
    return canCastle;
}
bool Piece::getIsCastling(){
    return isCastling;
}
bool Piece::getEPVul(){
    return EPVul;
}
bool Piece::getCanEP(){
    return canEP;
}
int Piece::getSquareColor(){
    return squareColor;
}
//Other Piece class functions
void Piece::printName(int i, int j){//This prints the piece name to the console, or if the location is empty, prints stars or spaces to match the square, but only if the argument corresponds to the correct location. It also sets the appropriate value for squareColor
    char r = '0' + i;
    char f = 'a' + j;

    if(f == file && r == rank){
        if(team == EMPTY && (i+j) % 2 == DARK)
            std::cout << "     ";
        else
            std::cout << name;
            
        squareColor = (i+j) % 2;
        return;
    }
    else
        return;
}
void Piece::addTimeMoved(){
    timesMoved++;
    return;
}
void Piece::addTimeSinceMoved(){
    timeSinceMoved++;
    return;
}
void Piece::changeType(pieceType newType){
    type = newType;
    return;
}

//OTHER FUNCTION DEFINITIONS///////////////////////////////
//Initializers///////////////////////////
//This function initializes and returns a vector of Piece objects at their starting locations on the chessboard.
std::vector<Piece> initializePieces()
{
    std::vector<Piece> newPieces;

    for(int i = MAX_SQUARES; i > 0; i--)
    {
        static char file{'h'};
        static char rank{'9'};

        if(i % 8 == 0){
            file = 'h';
            rank--;
        }
        else
            file--;

        Piece temp(file, rank, i);
        newPieces.push_back(temp);
    }

    return newPieces;
}
//Printers///////////////////////////
//This function prints the board and the pieces at their locations on the board
void printBoard(std::vector<Piece> &pieces, int turn, bool manualClaim, std::string finput)
{
    for(int e = 0; e < MAX_SQUARES; e++)
        pieces[e].setName(pieces[e].getType(), pieces[e].getTeam(), turn);

    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n";

    if(manualClaim == true)
        std::cout << finput << "\n\n\n\n\n" << std::endl;
    
    printCaptureBoxFar(pieces, turn);

    if(turn == WHITE)
    {   
        for(char a = 'a'; a <= 'h'; a++)
            std::cout << "   (" << a << ")  ";

        for(int i = NUM_RANKS; i > 0; i--)
        {
            for(int row = 0; row < ROWS_PER_RANK; row++)
            {
                std::cout << std::endl << "\t\t\t";
                if(row == 0)
                {
                    std::cout << "          ";
                    for(int j = 0; j < NUM_FILES; j++)
                    {
                        std::cout << "*-------"; //This prints the top edge of each row
                    }
                    std::cout << "*";
                }
                else if(row != 2)
                {
                    std::cout << "          ";
                    for(int j = 0; j < NUM_FILES; j++) //This prints the top and bottom thirds of each actual square
                    {
                        if((i+j) % 2 == LIGHT)
                            std::cout << "|*******";
                        else
                            std::cout << "|       ";
                    }
                    std::cout << "| ";
                }
                else
                {
                    std::cout << "      (" << i << ") ";
                    for(int j = 0; j < NUM_FILES; j++) //This prints the center third of each square, and any piece that may be on it
                    {
                        if((i+j) % 2 == LIGHT)
                        {
                            std::cout << "|*";
                            for(int k = 0; k < MAX_SQUARES; k++)
                                pieces[k].printName(i, j);
                            
                            std::cout << "*";
                        }
                        else if((i+j) % 2 == DARK)
                        {
                            std::cout << "| ";
                            for(int k = 0; k < MAX_SQUARES; k++)
                                pieces[k].printName(i, j);

                            std::cout << " ";
                        }
                    }
                    std::cout << "| (" << i << ") ";
                }
            }
        }

        std::cout << std::endl << "\t\t\t          ";

        for(int j = 0; j < NUM_FILES; j++)
            std::cout << "*-------";

        std::cout << "*";

        std::cout << "\n\t\t\t          ";

        for(char a = 'a'; a <= 'h'; a++){
            std::cout << "   (" << a << ")  ";
        }
    }
    else if(turn == BLACK)
    {
        for(char a = 'h'; a >= 'a'; a--)
            std::cout << "   (" << a << ")  ";

        for(int i = 1; i <= NUM_RANKS; i++)
        {
            for(int row = 0; row < ROWS_PER_RANK; row++)
            {
                std::cout << std::endl << "\t\t\t";
                if(row == 0)
                {
                    std::cout << "          ";
                    for(int j = NUM_FILES - 1; j >= 0; j--)
                    {
                        std::cout << "*-------"; //This prints the top edge of each row
                    }
                    std::cout << "*";
                }
                else if(row != 2)
                {
                    std::cout << "          ";
                    for(int j = NUM_FILES - 1; j >= 0; j--) //This prints the top and bottom thirds of each actual square
                    {
                        if((i+j) % 2 == LIGHT)
                            std::cout << "|*******";
                        else
                            std::cout << "|       ";
                    }
                    std::cout << "| ";
                }
                else
                {
                    std::cout << "      (" << i << ") ";
                    for(int j = NUM_FILES - 1; j >= 0; j--) //This prints the center third of each square, and any piece that may be on it
                    {
                        if((i+j) % 2 == LIGHT)
                        {
                            std::cout << "|*";
                            for(int k = 0; k < MAX_SQUARES; k++)
                                pieces[k].printName(i, j);
                            
                            std::cout << "*";
                        }
                        else if((i+j) % 2 == DARK)
                        {
                            std::cout << "| ";
                            for(int k = 0; k < MAX_SQUARES; k++)
                                pieces[k].printName(i, j);

                            std::cout << " ";
                        }
                    }
                    std::cout << "| (" << i << ") ";
                }
            }
        }

        std::cout << std::endl << "\t\t\t          ";

        for(int j = NUM_FILES - 1; j >= 0; j--)
            std::cout << "*-------";

        std::cout << "*\n\t\t\t          ";

        for(char a = 'h'; a >= 'a'; a--)
            std::cout << "   (" << a << ")  ";        
    }

    printCaptureBoxNear(pieces, turn);
    return;
}
//This function, along with printCaptureBoxNear, displays the captured pieces when called in printBoard
void printCaptureBoxFar(std::vector<Piece> &pieces, int turn)
{
    if(turn == WHITE)
    {
        std::cout << "\t\t\t **";
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            std::cout << "=====";
        
        std::cout << "**\n\t\t\t ||";
        
        //This prints the pieces captured by the opposing player
        for(int i = MAX_SQUARES - PIECES_PER_TEAM; i < MAX_SQUARES; i++) 
        {
            if(pieces[i].getType() == KING)
                continue;
            else if(pieces[i].getIsCaptured() == true)
                std::cout << pieces[i].getFirstName(turn);
            else
                std::cout << "     ";
            
            if(i % PIECES_PER_TEAM == 9)
                std::cout << "     ";
        }

        std::cout << "||\n\t\t\t **";
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            std::cout << "=====";
        
        std::cout << "**\n\n\t\t\t          ";
    }
    else if(turn == BLACK)
    {
        std::cout << "\t\t\t **";
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            std::cout << "=====";
        
        std::cout << "**\n\t\t\t ||";
        
        //This prints the pieces captured by the opposing player
        for(int i = PIECES_PER_TEAM - 1; i >= 0; i--) 
        {
            if(pieces[i].getType() == KING)
                continue;
            else if(pieces[i].getIsCaptured() == true)
                std::cout << pieces[i].getFirstName(turn);
            else
                std::cout << "     ";
            
            if(i % PIECES_PER_TEAM == 8)
                std::cout << "     ";
        }
        std::cout << "||\n\t\t\t **";
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            std::cout << "=====";
        
        std::cout << "**\n\n\t\t\t          ";
    }

    return;
}
//This function, along with printCaptureBoxFar, displays the captured pieces when called by printBoard
void printCaptureBoxNear(std::vector<Piece> &pieces, int turn)
{
    if(turn == WHITE)
    {
        std::cout << "\n\n\t\t\t **";
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            std::cout << "=====";
        
        std::cout << "**\n\t\t\t ||";
        
        //This prints the pieces captured by the current player
        for(int i = PIECES_PER_TEAM - 1; i >= 0; i--) 
        {
            if(pieces[i].getType() == KING)
                continue;
            else if(pieces[i].getIsCaptured() == true)
                std::cout << pieces[i].getFirstName(turn);
            else
                std::cout << "     ";
            
            if(i % PIECES_PER_TEAM == 8)
                std::cout << "     ";
        }
        std::cout << "||\n\t\t\t **";
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            std::cout << "=====";
        
        std::cout << "** \n" << std::endl;
    }
    else if(turn == BLACK)
    {
        std::cout << "\n\n\t\t\t **";
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            std::cout << "=====";
        
        std::cout << "**\n\t\t\t ||";
        
        //This prints the pieces captured by the current player
        for(int i = MAX_SQUARES - PIECES_PER_TEAM; i < MAX_SQUARES; i++) 
        {
            if(pieces[i].getType() == KING)
                continue;
            else if(pieces[i].getIsCaptured() == true)
                std::cout << pieces[i].getFirstName(turn);
            else
                std::cout << "     ";
            
            if(i % PIECES_PER_TEAM == 9)
                std::cout << "     ";
        }

        std::cout << "||\n\t\t\t **";
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            std::cout << "=====";
        
        std::cout << "** \n" << std::endl;
    }

    return;
}
void printTurn(int turnNum, int color, bool manualClaim, bool wasOffered) //This function takes an argument BLACK or WHITE and the current turn number and prints a corresponding string
{
    std::string p1 = "White";
    std::string p2 = "Black";

    std::string offerClause{""};
    std::string claimClause{""};
    
    if(wasOffered == true)
        std::cout << std::endl;
    else if(wasOffered == false)
        offerClause = ", \nor add 'offer draw' to the end of your move to offer a draw";
    if(manualClaim == true && wasOffered == false)
        claimClause = ", or enter or add 'claim draw' to initiate a draw claim";

    if(color == WHITE)
    {
        std::cout << "\n" << turnNum << ". " << p1 << " to move. Please enter your move or 'I resign' to resign" << offerClause << claimClause << ": " << std::endl;
    }
    else if(color == BLACK)
    {
        std::cout << "\n" << turnNum << "... " << p2 << " to move. Please enter your move or 'I resign' to resign" << offerClause << claimClause << ": " << std::endl;
    }

    return;        
}
//Finders/////////////////////////
//This function returns the index of the Piece object in pieces that exists at a location and where team != EMPTY. If no such piece exists, it returns -1.
int findPiece(std::vector<Piece> &pieces, std::string location)
{
    for(int i = 0; i < MAX_SQUARES; i++)
    {
        if(pieces[i].getTeam() != EMPTY && pieces[i].getLocation() == location)
        {
            return i;
        }
    }

    return -1;
}
//This function does the same thing as the one above, but it takes separate rank and file information.
int findPiece(std::vector<Piece> &pieces, char f, char r)
{
    std::string location{""};
    location.append(1, f);
    location.append(1, r);

    for(int i = 0; i < MAX_SQUARES; i++)
    {
        if(pieces[i].getTeam() != EMPTY && pieces[i].getLocation() == location)
        {
            return i;
        }
    }

    return -1;
}
//This function finds the index in pieces of the empty space at a location. If the location is not empty, it returns -1. 
int findEmpty(std::vector<Piece> &pieces, std::string location)
{
    for(int i = 0; i < MAX_SQUARES; i++)
    {
        if(pieces[i].getTeam() == EMPTY && pieces[i].getLocation() == location)
        {
            return i;
        }
    }

    return -1;
}
//This function returns the index of the Piece object in representing the king of a particular team. If no such piece exists, it returns -1, however, this should never happen.
int findKing(std::vector<Piece> &pieces, int color)
{
    for(int i = 0; i < MAX_SQUARES; i++)
    {
        if(pieces[i].getTeam() == color && pieces[i].getType() == KING)
        {
            return i;
        }
    }

    return -1;
}
//Piece manipulators///////////////////////
//This function performs the entered move, if possible, and returns a bool based on the legality of the attempted move. This is a compound function that calls several other functions to determine the validity of the move before performing it.
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, int turn) 
{   
    std::vector<Piece> oldPieces = pieces;
    int i = findPiece(pieces, start);

    if(i == -1)
        return false;
    else if(pieces[i].getType() == PAWN)
    {
        int p2 = findPiece(pieces, end[0], start[1]);
        int blank = findEmpty(pieces, end);
        if(p2 != -1 && blank != -1)
        {
            if(pieces[p2].getType() == PAWN && pieces[p2].getEPVul() == true)
            {
                std::cout << pieces[p2].getName() << pieces[p2].getFile();
                pieces[p2].setLocation(end);
                pieces[blank].setLocation(end[0], start[1]);
                pieces[i].setCanEP(true);
            }
            else
                pieces[i].setCanEP(false);
        }  
    }
    else if(pieces[i].getType() == KING && (start[0] - end[0] == 2 || end[0] - start[0] == 2))
        pieces[i].setIsCastling(true);

    bool isLegal = checkLegal(pieces, start, end, turn); //This determines whether or not an attempted move follows the rules of movement
    if(isLegal == false)
    {
        pieces = oldPieces;
        return false;
    }

    int j = findPiece(pieces, end);
    int j0 = j;
    removePiece(pieces, j);
    j = findEmpty(pieces, end);

    pieces[i].setLocation(end);
    pieces[j].setLocation(start);

    int oppoTurn = -1 * turn;
    int k = findKing(pieces, turn);

    pieces[i].addTimeMoved();
    pieces[i].resetTimeSinceMoved();

    for(int index = 0; index < MAX_SQUARES; index++)
    {
        if(pieces[index].getTeam() == turn && index != i)
            pieces[index].addTimeSinceMoved();
    }

    int goodCastle{-5};
    bool isSafe{true};

    for(int test = 0; test < MAX_SQUARES; test++)
    {
        if(checkCanCapture(pieces, pieces[test].getLocation(), pieces[k].getLocation(), oppoTurn) == true) //If any opposing piece could capture your king from this position, your attempted move is illegal
            isSafe = false;

        if(pieces[i].getIsCastling() == true) //When castling, a few more rules apply. If all conditions are met, this returns the index of the rook that needs to be moved
            goodCastle = getLegalCastlingRookIndex(pieces, i, j0, start, oppoTurn);

        if(isLegal == false || isSafe == false || goodCastle == -1) //If your attempted move breaks the movement rules, puts you into check, or is an illegal castling attempt, use oldPieces as a reference to restore the previous values of all Piece objects, then return false
        {
            pieces = oldPieces;
            return false;
        }
    }

    if(goodCastle >= 0) //This means there was a legal attempt to castle. The following if block completes the castling maneuver by moving the rook to the appropriate square
    {
        std::string rookSpot = start;
        rookSpot[0] = (start[0]+end[0])/2;
        int empty2 = findEmpty(pieces, rookSpot);
        std::string emptySpot = pieces[goodCastle].getLocation();
        pieces[goodCastle].setLocation(rookSpot);
        pieces[empty2].setLocation(emptySpot);
        pieces[goodCastle].addTimeMoved();
        pieces[goodCastle].resetTimeSinceMoved();
    }

    if(i == k)
    {
        for(int all = 0; all < MAX_SQUARES; all++)
        {
            if(pieces[all].getTeam() == turn)
                pieces[all].setCanCastle(false);
        }
    }
    else if(pieces[i].getType() == PAWN && (end[1] - start[1])*turn == 2)
        pieces[i].setEPVul(true);

    return true;
}
//This function lets you change a piece's team to EMPTY and change its captured status to true, thus removing it from the main board and allowing it to be displayed with the other captured pieces
void removePiece(std::vector<Piece> &pieces, int i)
{
    pieces[i].setIsCaptured(true);
    pieces[i].setTeam(CAPTURE_INDEX);
    pieces[i].setType(CAPTURE_INDEX);
    pieces[i].setName(pieces[i].getType(), pieces[i].getTeam(), WHITE);

    return;
}
//This function is used for pawn promotion
void promotePawn(std::vector<Piece> &pieces, int i)
{
    std::cout << "PAWN PROMOTION! \nPlease select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: " << std::endl;

    char newVal{' '};

    while(newVal == ' ')
    {
        std::cin >> newVal;
        newVal = tolower(newVal);

        if(newVal == 'q')
            pieces[i].changeType(QUEEN);
        else if(newVal == 'r')
            pieces[i].changeType(ROOK);
        else if(newVal == 'b')
            pieces[i].changeType(BISHOP);
        else if(newVal == 'n')
            pieces[i].changeType(KNIGHT);
        else
        {
            printBoard(pieces, pieces[i].getTeam(), false, " ");

            std::cout << "\nInvalid input. \nPlease enter 'q', 'r', 'b', or 'n': " << std::endl;
            newVal = ' ';
            continue;
        }
    }

    pieces[i].setName(pieces[i].getType(), pieces[i].getTeam(), WHITE);
    return;
}
//Legality checkers////////////////////////
//This function determines if the attempt to castle is legal, and returns the index of the involved rook if legal, or -1 if illegal
int getLegalCastlingRookIndex(std::vector<Piece> &pieces, int i, int j0, std::string start, int oppoTurn)
{
    int rook{-5};
    if(pieces[i].getFile() == 'g')
    {
        std::string breadcrumb = "f";
        breadcrumb.append(1, start[1]);
        rook = findPiece(pieces, 'h', start[1]);

        for(char c = 'e'; c < 'h'; c++)
        {
            if(findPiece(pieces, c, pieces[i].getRank()) != -1 && findPiece(pieces, c, pieces[i].getRank()) != i) //All spaces between the king and the rook must be empty
                return -1;
            else if(j0 != -1) //This means there cannot be a piece there to capture
                return -1;
            else if(rook == -1 || pieces[rook].getTimesMoved() != 0) //The rook must not have moved
                return -1;
            else if(pieces[i].getTimesMoved() != 1) //The king also must not have moved, however, when this function is called in movePiece, the change to timesMoved will have already been applied
                return -1;
            else
            {
                for(int stopper = 0; stopper < MAX_SQUARES; stopper++)
                {
                    if(checkCanCapture(pieces, pieces[stopper].getLocation(), start, oppoTurn) == true || checkCanCapture(pieces, pieces[stopper].getLocation(), breadcrumb, oppoTurn) == true) //You cannot castle out of check or through check
                        return -1;
                }
            }
        }
    }
    else if(pieces[i].getFile() == 'c')
    {
        std::string breadcrumb = "b";
        breadcrumb.append(1, start[1]);
        rook = findPiece(pieces, 'a', start[1]);

        for(char c = 'e'; c > 'a'; c--)
        {
            if(findPiece(pieces, c, pieces[i].getRank()) != -1 && findPiece(pieces, c, pieces[i].getRank()) != i) //All spaces between the king and the rook must be empty
                return -1;
            else if(j0 != -1) //This means there cannot be a piece there to capture
                return -1;
            else if(rook == -1 || pieces[rook].getTimesMoved() != 0) //The rook must not have moved
                return -1;
            else if(pieces[i].getTimesMoved() != 1) //The king also must not have moved, however, when this function is called in movePiece, the change to timesMoved will have already been applied
                return -1;
            else
            {
                for(int stopper = 0; stopper < MAX_SQUARES; stopper++)
                {
                    if(checkCanCapture(pieces, pieces[stopper].getLocation(), start, oppoTurn) == true || checkCanCapture(pieces, pieces[stopper].getLocation(), breadcrumb, oppoTurn) == true) //You cannot castle out of check or through check
                        return -1;
                }
            }
        }
    }
    return rook;
}
//This function determines if the attempted move follows the basic movement rules and returns a corresponding bool.
bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn)
{
    int i = findPiece(pieces, start);
    int j = findPiece(pieces, end);

    if(start == end) //You can't skip your turn
        return false;
    else if(i == -1) //You can't move a non-existent piece
        return false;
    else if(pieces[i].getTeam() != turn) //You can't move if it's not your turn
        return false;
    else if(pieces[j].getTeam() == turn) //You can't move to a square occupied by another one of your pieces
        return false;

    if(pieces[i].getType() != KNIGHT) //Only knights can jump over other pieces
    {
        std::string vanguard = start;
        while(vanguard != end)
        {
            if(vanguard[0] < end[0])
                vanguard[0]++;
            else if(vanguard[0] > end[0])
                vanguard[0]--;

            if(vanguard[1] < end[1])
                vanguard[1]++;
            else if(vanguard[1] > end[1])
                vanguard[1]--;

            if(vanguard == end)
                break;
            else if(findPiece(pieces, vanguard) != -1)
                return false;
            else
                continue;
        }
    }
    else //Implies type == KNIGHT
    {
        if(end[0] - start[0] > 2 || end[1] - start[1] > 2)
            return false;
        else if(start[0] - end[0] > 2 || start[1] - end[1] > 2)
            return false;
        else if(start[0] == end[0] || start[1] == end[1])
            return false;
        else if(end[0] - start[0] == end[1] - start[1] || end[0] - start[0] == start[1] - end[1])
            return false;
    }

    if(pieces[i].getType() == PAWN)
    {   
        if((end[1] - start[1])*turn > 2)
            return false;
        else if((end[1] - start[1])*turn == 2 && end[0] != start[0])
            return false;
        else if((end[1] - start[1])*turn != 1 && pieces[i].getTimesMoved() != 0)
            return false;
        else if((end[1] - start[1])*turn <= 0)
            return false;
        else if(end[0] == start[0] && j != -1)
            return false;
        else if(end[0] != start[0] && j == -1)
            return false;
        else if(end[0] - start[0] < -1 || end[0] - start[0] > 1)
            return false;
    }
    else if(pieces[i].getType() == KING)
    {
        if(end[0] - start[0] > 1 && pieces[i].getIsCastling() == false)
            return false;
        else if(end[0] - start[0] < -1 && pieces[i].getIsCastling() == false)
            return false;
        else if(end[1] - start[1] > 1 || start[1] - end[1] > 1)
            return false;
        else if(end[0] - start[0] > 2 || start[0] - end[0] > 2)
            return false;
    }
    else if(pieces[i].getType() == QUEEN)
    {
        if(isDiagonal(start, end) == false && (start[0] != end[0] && start[1] != end[1]))
            return false;
    }
    else if(pieces[i].getType() == BISHOP)
    {
        if(isDiagonal(start, end) == false)
            return false;
    }
    else if(pieces[i].getType() == ROOK)
    {
        if(start[0] != end[0] && start[1] != end[1])
        return false;
    }
    

    return true;
}
//This function determines if the piece on start can move to the end square when capturing according to movement rules and returns a corresponding bool.
bool checkCanCapture(std::vector<Piece> &pieces, std::string start, std::string end, int turn)
{
    int i = findPiece(pieces, start);
    int j = findPiece(pieces, end);

    if(start == end) //You can't skip your turn
        return false;
    else if(i == -1) //You can't move a non-existent piece
        return false;
    else if(pieces[i].getTeam() != turn) //You can't move if it's not your turn
        return false;
    else if(pieces[j].getTeam() == turn) //You can't move to a square occupied by another one of your pieces
        return false;

    if(pieces[i].getType() != KNIGHT) //Only knights can jump over other pieces
    {
        std::string vanguard = start;
        while(vanguard != end)
        {
            if(vanguard[0] < end[0])
                vanguard[0]++;
            else if(vanguard[0] > end[0])
                vanguard[0]--;

            if(vanguard[1] < end[1])
                vanguard[1]++;
            else if(vanguard[1] > end[1])
                vanguard[1]--;

            if(vanguard == end)
                break;
            else if(findPiece(pieces, vanguard) != -1)
                return false;
            else
                continue;
        }
    }
    else //Implies type == KNIGHT
    {
        if(end[0] - start[0] > 2 || end[1] - start[1] > 2)
            return false;
        else if(start[0] - end[0] > 2 || start[1] - end[1] > 2)
            return false;
        else if(start[0] == end[0] || start[1] == end[1])
            return false;
        else if(end[0] - start[0] == end[1] - start[1] || end[0] - start[0] == start[1] - end[1])
            return false;
    }

    if(pieces[i].getType() == PAWN) //The movement rules for pawns are different when capturing than when moving to an empty square
    {
        if(start[0] == end[0])
            return false;
        else if(end[0] - start[0] != 1 && start[0] - end[0] != 1)
            return false;
        else if((end[1] - start[1])*turn != 1)
            return false;
    }
    else if(pieces[i].getType() == KING)
    {
        if(end[0] - start[0] > 1 || end[0] - start[0] < -1)
            return false;
        else if(end[1] - start[1] > 1 || start[1] - end[1] > 1)
            return false;
    }
    else if(pieces[i].getType() == QUEEN)
    {
        if(isDiagonal(start, end) == false && (start[0] != end[0] && start[1] != end[1]))
            return false;
    }
    else if(pieces[i].getType() == BISHOP)
    {
        if(isDiagonal(start, end) == false)
            return false;
    }
    else if(pieces[i].getType() == ROOK)
    {
        if(start[0] != end[0] && start[1] != end[1])
        return false;
    }
    

    return true;
}
// This function returns a bool representing whether or not two squares are diagonal from each other
bool isDiagonal(std::string start, std::string end)
{
    if((end[0] - start[0] != end[1] - start[1]) && (end[0] - start[0] != start[1] - end[1]))
        return false;
    else
        return true;
}
//End condition checkers////////////////////////
// This function determines if the current player has at least one legal move and returns an int equal to the number of legal moves
int countLegalMoves(std::vector<Piece> &pieces, int turn) 
{
    int moves{0};
    std::vector<Piece> oldPieces = pieces;

    for(int i = 0; i < MAX_SQUARES; i++)
    {
        for(int j = 0; j < MAX_SQUARES; j++)
        {
            if(pieces[i].getTeam() != turn){
                continue;
            }
            else if(pieces[j].getTeam() == turn){
                continue;
            }
            std::string s = pieces[i].getLocation();
            std::string e = pieces[j].getLocation();
            if(checkLegal(pieces, s, e, turn) == false){
                continue;
            }
            else if(movePiece(pieces, s, e, turn) == false){
                continue;
            }
            else{
                pieces = oldPieces;
                moves++;
            }
        }       
    }
    //std::cout << "Number of possible moves = " << moves << std::endl; // This piece is useful
    return moves;
}
//When get50move is false, this function compares the current board state to all previous board states and returns an int stating how many times this board state has occured. When get50move is true, this function determines and keeps track of the remaining moves without a pawn move or a capture before the 50-move draw rule is invoked.
int getDraw(std::vector<Piece> &pieces, int turn, pieceType pMoved)
{
    std::vector<Piece> oldPieces = pieces;
    static std::vector<std::string> boardStates;
    std::string newState{""};
    std::vector<int> b;
    b.clear();

    if(turn == WHITE)
        newState.append("W.");
    else if(turn == BLACK)
        newState.append("B.");

    for(char r = '1'; r <= '8'; r++)
    {
        for(char f = 'a'; f <= 'h'; f++)
        {
            int i = findPiece(pieces, f, r);
            char c;
            if(i == -1)
            {
                newState.append(1, ' ');
                continue;
            }
            else if(pieces[i].getType() == KING)
                c = 'k';
            else if(pieces[i].getType() == QUEEN)
                c = 'q';
            else if(pieces[i].getType() == ROOK)
                c = 'r';
            else if(pieces[i].getType() == BISHOP)
            {
                c = 'b';
                b.push_back(i);
            }
            else if(pieces[i].getType() == KNIGHT)
                c = 'n';
            else if(pieces[i].getType() == PAWN)
                c = 'p';

            if(pieces[i].getTeam() == WHITE)
                c = toupper(c);

            newState.append(1, c);
        }
    }

    newState.append(1, '.');

    for(int j = 0; j < MAX_SQUARES; j++)
    {
        if(pieces[j].getType() == PAWN && pieces[j].getTeam() == turn)
        {
            std::string epCheckSpot1 = pieces[j].getLocation();
            
            epCheckSpot1[1] += (pieces[j].getTeam());
            std::string epCheckSpot2 = epCheckSpot1;

            epCheckSpot1[0]--;
            epCheckSpot2[0]++;

            movePiece(pieces, pieces[j].getLocation(), epCheckSpot1, turn);
            if(pieces[j].getCanEP() == true)
            {
                pieces = oldPieces;
                //std::cout << pieces[j].getLocation() << " to " << epCheckSpot1;
                newState.append(1, '1');
                continue;
            }
            else
                pieces = oldPieces;
            
            movePiece(pieces, pieces[j].getLocation(), epCheckSpot2, turn);
            if(pieces[j].getCanEP() == true)
            {
                pieces = oldPieces;
                newState.append(1, '1');
                continue;
            }
            else
            {
                pieces = oldPieces;
                newState.append(1, 'o');
            }     
        }
    }

    newState.append(1, '.');

    for(int k = 0; k < MAX_SQUARES; k++)
    {
        if(pieces[k].getType() == ROOK)
        {
            if(pieces[k].getCanCastle() == false)
                newState.append(1, 'x');
            else if(pieces[k].getTimesMoved() != 0)
            {
                newState.append(1, 'x');
                pieces[k].setCanCastle(false);
            }
            else if(pieces[k].getCanCastle() == true)
                newState.append(1, 'Y');
        }
    }

    //std::cout << newState << std::endl;
    boardStates.push_back(newState);

    int repeats = std::count (boardStates.begin(), boardStates.end(), newState);

    if(repeats >= 3)
        return 3;

    //std::cout << "This board state has occurred " << repeats << " time(s). " << std::endl;
    
    static int oldPiecesLeft{MAX_PIECES};
    static int turnsTowardsDraw{100};

    int newPiecesLeft = MAX_SQUARES - std::count (newState.begin(), newState.end(), ' ');

    if(newPiecesLeft < oldPiecesLeft || pMoved == PAWN)
    {
        oldPiecesLeft = newPiecesLeft;
        turnsTowardsDraw = 0;
    }
    else
        turnsTowardsDraw++;

    //std::cout << "Turns towards 50-move rule: " << (turnsTowardsDraw)/2 <<std::endl;
    //std::cout << "turnsTowardsDraw == " << turnsTowardsDraw << std::endl;

    if((turnsTowardsDraw - 1)/2 >= 50)
    {
        int drawNum = (turnsTowardsDraw - 1)/2;
        return drawNum;
    }
    else if(newPiecesLeft == 2)
        return 100;
    else if(newPiecesLeft == 3 && (std::count (newState.begin() + 2, newState.end(), 'n') == 1 || std::count (newState.begin() + 2, newState.end(), 'N') == 1))
        return 100;
    else if(newPiecesLeft == 3 && (std::count (newState.begin() + 2, newState.end(), 'b') == 1 || std::count (newState.begin() + 2, newState.end(), 'B') == 1))
        return 100;
    else if(newPiecesLeft == 4 && (std::count (newState.begin() + 2, newState.end(), 'b') == 2 || std::count (newState.begin() + 2, newState.end(), 'B') == 2))
    {
        if(pieces[b[0]].getTeam() == WHITE && pieces[b[0]].getSquareColor() == pieces[b[1]].getSquareColor())
            return 100;
        else if(pieces[b[b.size()-1]].getTeam() == BLACK && pieces[b[b.size()-1]].getSquareColor() == pieces[b[b.size()-2]].getSquareColor())
            return 100;
        else
            return 0;
    }
    else if(newPiecesLeft == 4 && std::count (newState.begin() + 2, newState.end(), 'b') == 1 && std::count (newState.begin() + 2, newState.end(), 'B') == 1)
    {
        if(pieces[b[b.size()-1]].getSquareColor() == pieces[b[0]].getSquareColor())
            return 100;
        else
            return 0;
    }
    else
        return 0;    
}