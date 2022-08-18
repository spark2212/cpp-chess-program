// ChessGameRewrite.cpp

#include<iostream> //std::cout, cin, etc
#include<fstream> //std::ifstream
#include<sstream>
#include<string> //std::string, string functions
#include<vector> //std::vector, vector operations
#include<math.h> //operators
#include<algorithm> //std::count
#include<chrono>
#include<thread>

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
    std::string getNameString(int i, int j); //returns the name of the piece if the corresponding location matches, else returns an empty string
    void addTimeMoved();
    void addTimeSinceMoved();
    void changeType(pieceType newType);
};

//FUNCTION DECLARATIONS///////////////////////////////////
//Initializers
std::vector<Piece> initializePieces(); 

//Converters
std::vector<std::string> convertMove(std::vector<Piece> &pieces, std::string move, int turn);
std::string getCaptureBoxFar(std::vector<Piece> &pieces, int turn);
std::string getCaptureBoxNear(std::vector<Piece> &pieces, int turn);

//Printers
//void printBoard(std::vector<Piece> &pieces, int turn, bool manualClaim, std::string finput);
std::string printBoard(std::vector<Piece> &pieces, int turn, bool manualClaim, std::string finput, int next);
//void printTurn(int turnNum, int color, bool manualClaim, bool wasOffered);
std::string printTurn(int turnNum, int color, bool manualClaim, bool wasOffered, int next);
    /*clarity does nothing but remove ambiguity between the overloaded functions in the compiler*/

//Finders
int findPiece(std::vector<Piece> pieces, std::string location);
int findPiece(std::vector<Piece> pieces, char f, char r);
int findEmpty(std::vector<Piece> pieces, std::string location);
int findKing(std::vector<Piece> pieces, int color);

//Piece manipulators
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
void removePiece(std::vector<Piece> &pieces, int i);
void promotePawn(std::vector<Piece> &pieces, int i, int turnNum, bool wasOffered);

//Legality checkers
int getLegalCastlingRookIndex(std::vector<Piece> &pieces, int i, int j0, std::string start, int oppoTurn);
bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
bool checkCanCapture(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
// This function returns a bool representing whether or not two squares are diagonal from each other
inline bool isDiagonal(std::string start, std::string end)
{
    if((end[0] - start[0] != end[1] - start[1]) && (end[0] - start[0] != start[1] - end[1]))
        return false;
    else
        return true;
}

//End condition checkers
int countLegalMoves(std::vector<Piece> &pieces, int turn);
int getDraw(std::vector<Piece> &pieces, int turn, pieceType pMoved);

//Timing functions
inline void pauseRun(int milliseconds){
    std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
    while(std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(milliseconds));
}

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
    std::string output1;
    static unsigned long int turnNum{1};
    int turn{WHITE};
    int oppoTurn{BLACK};
    output1.append(printBoard(pieces, turn, manualClaim, finput, 10));
    static int getDrawNum = getDraw(pieces, turn, PAWN);
    bool wasOffered{false};
    output1.append(printTurn(turnNum, turn, manualClaim, wasOffered, 10));
    std::cout << "Loading..." << std::flush;

    pauseRun(1250);

    std::cout << output1 << std::flush;

    while(1)
    {
        pauseRun(2000);
        std::string output;
        std::string move;
        std::string buffer;
        std::vector<std::string> route;
        std::string start;
        std::string end;
        bool moveEntered{true};
        int next{10};
        //static int holdTurn{WHITE};

        next = std::cin.peek();

        int tNum = turnNum;
        if(turn == BLACK)
            tNum = turnNum + 1;
            
        output.append(printBoard(pieces, turn, false, " ", next));
        output.append(printTurn(tNum, turn, manualClaim, wasOffered, next));

        std::cin >> move;
        buffer = move;

        std::cout << output << std::flush;
        output.clear();

        for(int h = 0; h < move.size(); h++)
            buffer[h] = tolower(move[h]);

        int j = findPiece(pieces, buffer);

        if(j != -1 && std::cin.peek() != 10)
        {
            if(pieces[j].getTeam() == turn)
            {
                start = buffer;
                std::cin >> end;
                for(int j = 0; j < end.size(); j++)
                    end[j] = tolower(end[j]);
            }
            else
            {
                route = convertMove(pieces, move, turn);
                start = route[0];
                end = route[1];
            }
        }
        else if(buffer == "i" || buffer == "claim" || buffer == "offer")
        {
            moveEntered = false;
            start = buffer;
            std::cin >> end;
            for(int j = 0; j < end.size(); j++)
                end[j] = tolower(end[j]);
        }
        else
        {
            route = convertMove(pieces, move, turn);
            start = route[0];
            end = route[1];
        }

        pieceType pMoved{NO_PIECE}; 
        int startPiece = findPiece(pieces, start);
        if(startPiece != -1)
            pMoved = pieces[startPiece].getType(); //This will be used much later but needs to be initialized now.

        if(moveEntered == true)
        {
            //std::cout << isDiagonal(start, end);
            output.append(printBoard(pieces, oppoTurn, false, " ", next));
            output.append(printTurn(tNum, oppoTurn, manualClaim, wasOffered, next));
            std::cout << output << std::flush;
            output.clear();

            if(pMoved != PAWN || (end[1] != '1' && end[1] != '8'))
            {
                output.append(printBoard(pieces, oppoTurn, false, " ", next));
                output.append(printTurn(tNum, oppoTurn, manualClaim, wasOffered, next));
                std::cout << output << std::flush;
                output.clear();
            }
        }
        else 
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');

            if(start == "i" && end == "resign")
            {
                if(turn == WHITE)
                {
                    if(argc == 2)
                        finput.append("\nWhite resigns! Black wins! \n\t0 - 1");

                    output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                    output.append("White resigns! Black wins! \n");

                    std::cout << output << std::flush;
                    
                    break;
                }
                else if(turn == BLACK)
                {
                    if(argc == 2)
                        finput.append("\nBlack resigns! White wins! \n\t1 - 0");
                        
                    output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                    output.append("Black resigns! White wins! \n");

                    std::cout << output << std::flush;
                    
                    break;
                }
            }
            else if(wasOffered == true && end == "draw")
            {
                std::string output;
                output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                output.append("You have already extended a draw offer on this turn.");
                output.append(printTurn(turnNum, turn, manualClaim, wasOffered, 10));
                std::cout << output << std::flush;
                output.clear();
                continue;
            }
            else if((start == "offer" || (start == "claim" && manualClaim == true)) && end == "draw")
            {
                std::string output;
                wasOffered = true;
                output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                if(start == "claim")
                    output.append("Your opponent has attempted to claim a draw. All draw claims double as draw offers... ");
                    
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                output.append("\nYour opponent has offered a draw. Do you accept?\nEnter 'yes' to accept, 'no' to decline, or 'flip' to flip the board: \n");
                
                std::cout << output << std::flush;
                output.clear();

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
                            output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                            output.append("Board flipped.");
                        }
                        else
                        {
                            output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                            output.append("Invalid entry.");
                        }
                        output.append("\nYour opponent has offered a draw. Do you accept?\nEnter 'yes' to accept or 'no' to decline: \n");
                        std::cout << output << std::flush;
                        output.clear();

                        response = "";
                    }
                }

                if(response == "yes")
                {
                    finput.append("(=)\nDraw offered and accepted! \n\n\t1/2 - 1/2");
                    output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                    output.append("\nDraw offered and accepted! \n");
                    std::cout << output;
                    output.clear();
                
                    break;
                }
                else if(start == "claim")
                {
                    if(getDrawNum == 3)
                    {
                        finput.append("(=)\n\n\t1/2 - 1/2");
                        output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                        output.append("Threefold repetition rule invoked! It's a draw! \n");
                        std::cout << output;
                        output.clear();
                        break;
                    }
                    else if(getDrawNum >= 50)
                    {
                        finput.append("(=)\n\n\t1/2 - 1/2");
                        output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                        output.append("50-move rule invoked! It's a draw! \n");
                        std::cout << output;
                        output.clear();
                        break;
                    }
                    else
                    {
                        output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                        output.append("Claim invalid.");
                        output.append(printTurn(turnNum, turn, manualClaim, wasOffered, 10));
                        std::cout << output << std::flush;
                        output.clear();
                        continue;
                    }
                }
                else if(response == "no")
                {
                    output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                    output.append("Draw offer declined. ");
                    output.append(printTurn(turnNum, turn, manualClaim, wasOffered, 10));
                    std::cout << output << std::flush;
                    output.clear();
                    continue;
                }
            }
        }

        for(int e = 0; e < MAX_SQUARES; e++)
            pieces[e].setIsCastling(false);

        int i = findPiece(pieces, start);

        std::vector<Piece> oldPieces = pieces;
        bool valid = movePiece(pieces, start, end, turn); //If the move is valid, performs the move and returns true. Else, returns false.
        
        if(valid == false)
        {
            //std::string output;
            output.append(printBoard(pieces, turn, manualClaim, finput, 10));
            if(start == "a0" && end == "a0")
                output.append("Invalid entry. Try again.");
            else
                output.append("Invalid move. Try again.");

            output.append(printTurn(turnNum, turn, manualClaim, wasOffered, 10));
            std::cout << output << std::flush;
            output.clear();

            std::cin.clear();
            std::cin.ignore(10000, '\n');
            
            continue;
        }
        else
        {
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
                while(std::chrono::steady_clock::now() - t1 <= std::chrono::milliseconds(1000));
                
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

            oppoTurn = turn;
            turn *= -1;
            
        }
        if(pieces[i].getType() == PAWN && (end[1] == '1' || end[1] == '8'))
        {    
            promotePawn(pieces, i, turnNum, wasOffered); //promotes the pawn to the desired piece

            if(argc == 2)
            {
                finput.append("=");
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

        output.clear();    

        for(int a = 0; a < MAX_SQUARES; a++) //Nothing to do but reset En Passant values
        {
            if(pieces[a].getTeam() == turn)
                pieces[a].setEPVul(false);
            if(pieces[a].getTeam() != turn)
                pieces[a].setCanEP(false);
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

        if(turn == WHITE)
            turnNum++;
        
        next = std::cin.peek();

        output.append(printBoard(pieces, turn, manualClaim, finput, next));

        if(isCheck == true && countMoves > 0)
        {
            output.append("Check! ");
            if(argc == 2)
                finput.append("+");
        }
        else if(isCheck == true && countMoves == 0)
        {
            
            std::cout << output << "Checkmate! ";
            if(oppoTurn == WHITE)
                std::cout << "White wins! " << std::endl;
            else if(oppoTurn == BLACK)
                std::cout << "Black wins! " << std::endl;

            output.clear();

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
            std::cout << output << "No legal moves! Stalemate! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }

        getDrawNum = getDraw(pieces, turn, pMoved);
        wasOffered = false;

        if(getDrawNum == 3 && manualClaim == false)
        {
            std::cout << output << "Threefold repetition rule invoked! It's a draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }
        else if(getDrawNum == 50 && manualClaim == false)
        {
            std::cout << output << "50-move rule invoked! It's a draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }
        else if(getDrawNum == 5)
        {
            std::cout << output << "Fivefold repetition rule invoked! Automatic draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }
        else if(getDrawNum == 75)
        {
            std::cout << output << "75-move rule invoked! Automatic draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }
        else if(getDrawNum == 100)
        {
            std::cout << output << "Insufficient material left! It's a draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }
        
        output.append(printTurn(turnNum, turn, manualClaim, wasOffered, next));
        //pauseRun(1000);
        std::cout << output << std::flush;
        output.clear();
        

    } //End of loop

    if(argc == 2)
    {
        fopen(argv[1], "w");
        std::ofstream gameLog (argv[1]);

        gameLog << finput;

        gameLog.close();
    }

    std::cout << "Game Over. Goodbye! \n" << std::endl;

    return 0;
} //End of main()

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
std::string Piece::getNameString(int i, int j){//This prints the piece name to the console, or if the location is empty, prints stars or spaces to match the square, but only if the argument corresponds to the correct location. It also sets the appropriate value for squareColor
    std::string pieceName;
    char r = '0' + i;
    char f = 'a' + j;

    if(f == file && r == rank){
        if(team == EMPTY && (i+j) % 2 == DARK)
            pieceName.append("     ");
        else
            pieceName.append(name);
            
        squareColor = (i+j) % 2;
        return pieceName;
    }
    else
        return "";
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
//Converters//////////////////////////
//This function converts an entered move from standard chess notation into start and end locations.
std::vector<std::string> convertMove(std::vector<Piece> &pieces, std::string move, int turn)
{
    std::vector<std::string> route;
    std::vector<std::string> badRoute;

    std::string badSpot = "a0";

    badRoute.push_back(badSpot);
    badRoute.push_back(badSpot);

    bool captureMove{false};

    //Step 1: Determine the piece type
    pieceType searchType;

    if(move[0] == 'R')
        searchType = ROOK;
    else if(move[0] == 'N')
        searchType = KNIGHT;
    else if(move[0] == 'B')
        searchType = BISHOP;
    else if(move[0] == 'Q')
        searchType = QUEEN;
    else if(move[0] == 'K' || move[0] == '0')
        searchType = KING;
    else
        searchType = PAWN;

    //Step 2: Determine capture, castle, or regular move
    if(move == "0-0" || move == "0-0-0")
    {
        int k = findKing(pieces, turn);
        route.push_back(pieces[k].getLocation());
        std::string target;

        if(move == "0-0")
        {
            int r = findPiece(pieces, 'h', pieces[k].getRank());

            if(r == -1)
                return badRoute;
            else 
            {
                target.append(1, 'g');
                target.append(1, pieces[k].getRank());
                route.push_back(target);
                return route;
            }
        }
        else if(move == "0-0-0")
        {
            int r = findPiece(pieces, 'a', pieces[k].getRank());

            if(r == -1)
                return badRoute;
            else 
            {
                target.append(1, 'c');
                target.append(1, pieces[k].getRank());
                route.push_back(target);
                return route;
            }
        }
    }
    else if(std::count (move.begin(), move.end(), 'x') == 1)
        captureMove = true;
    else if(std::count (move.begin(), move.end(), 'x') > 1)
        return badRoute;
    
    //Step 3: Count identifiers
    int fileCount{0};
    int rankCount{0};

    for(char file = 'a'; file <= 'h'; file++)
        fileCount += std::count (move.begin(), move.end(), file);

    for(char rank = '1'; rank <= '8'; rank++)
        rankCount += std::count (move.begin(), move.end(), rank);

    //std::cout << "fileCount is " << fileCount << " and rankCount is " << rankCount;

    //Step 4: Interpret move
    if(fileCount == 1 && rankCount == 1) //e.g. e4, Nf6, Kc7
    {
        if(searchType == PAWN && captureMove == true)
            return badRoute;
        else
        {
            if(searchType == PAWN)
                move.insert(move.begin(), 'P');
            
            if(captureMove == false)
                move.insert(move.begin()+1, '-');

            int options{0};

            std::string start;
            std::string end;
            end.append(1, move[2]);
            end.append(1, move[3]);    

            //This checks that the information provided is sufficient
            for(int i = 0; i < MAX_SQUARES; i++)
            {
                if(checkLegal(pieces, pieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
                {
                    options++;
                    start = pieces[i].getLocation();
                }
            }
            
            route.push_back(start);
            route.push_back(end);

            if(options == 1)
            {
                if(captureMove == true && findPiece(pieces, end) == -1)
                    return badRoute;
                else if(captureMove == false && findPiece(pieces, end) != -1)
                    return badRoute;
                else
                    return route;
            }
            else if(options == 0)
                return route;
            else
                return badRoute;
            
        }
    }
    else if(fileCount == 2 && rankCount == 1) //e.g. exd5, Nab5
    {
        if(searchType == PAWN && captureMove == false)
            return badRoute;
        else
        {
            if(searchType == PAWN)
                move.insert(move.begin(), 'P');
            
            if(captureMove == false)
                move.insert(move.begin()+2, '-');

            int options{0};
            int alts{0};

            std::string start;
            std::string end;
            end.append(1, move[3]);
            end.append(1, move[4]);    

            //This checks that the information provided is sufficient and necessary
            for(int i = 0; i < MAX_SQUARES; i++)
            {
                if(checkLegal(pieces, pieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
                {
                    if(pieces[i].getFile() != move[1])
                    {
                        alts++;
                        continue;
                    }
                    else
                    {
                        options++;
                        start = pieces[i].getLocation();
                    }
                }
                else if(checkCanCapture(pieces, pieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
                {
                    if(pieces[i].getFile() != move[1])
                    {
                        alts++;
                        continue;
                    }
                    else
                    {
                        options++;
                        start = pieces[i].getLocation();
                    }
                }
            }
            
            route.push_back(start);
            route.push_back(end);

            if(options == 1 && (alts != 0 || searchType == PAWN))
            {
                if(captureMove == true && findPiece(pieces, end) == -1)
                {
                    if(checkCanCapture(pieces, start, end, turn) == false || searchType != PAWN)
                        return badRoute;
                    else
                        return route;
                }
                else if(captureMove == false && findPiece(pieces, end) != -1)
                {
                    if(checkCanCapture(pieces, start, end, turn) == true)
                        return badRoute;
                    else
                        return route;
                }
                else
                    return route;
            }
            else if(options == 0)
                return route;
            else
                return badRoute;
        } 
    }
    else if(fileCount == 1 && rankCount == 2) //e.g. R1xf3, N2b5
    {
        if(searchType == PAWN && captureMove == false)
            return badRoute;
        else
        {
            if(searchType == PAWN)
                move.insert(move.begin(), 'P');
            
            if(captureMove == false)
                move.insert(move.begin()+2, '-');

            int options{0};
            int alts{0};

            std::string start;
            std::string end;
            end.append(1, move[3]);
            end.append(1, move[4]);    

            for(int i = 0; i < MAX_SQUARES; i++)
            {
                if(checkLegal(pieces, pieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
                {
                    if(pieces[i].getRank() != move[1])
                    {
                        alts++;
                        continue;
                    }
                    else
                    {
                        options++;
                        start = pieces[i].getLocation();
                    }
                }
            }
            
            route.push_back(start);
            route.push_back(end);

            if(options == 1 && alts != 0)
            {
                int j = findPiece(pieces, end);
                

                if(captureMove == true && j == -1)
                    return badRoute;
                else if(captureMove == false && j != -1)
                {
                    if(pieces[j].getTeam() == -1*turn)
                        return badRoute;
                    else
                        return route;
                }
                else
                    return route;
            }
            else if(options == 0)
                return route;
            else
                return badRoute;
        } 
    }
    else if(fileCount == 2 && rankCount == 2) //e.g. Qd1xf3, Bb2e5
    {
        if(searchType == PAWN)
            return badRoute;
        else
        {
            if(captureMove == false)
                move.insert(move.begin()+3, '-');

            int options{0};
            int alts1{0};
            int alts2{0};

            std::string start;
            start.append(1, move[1]);
            start.append(1, move[2]);

            std::string end;
            end.append(1, move[4]);
            end.append(1, move[5]);    

            for(int i = 0; i < MAX_SQUARES; i++)
            {
                if(checkLegal(pieces, pieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
                {
                    if(pieces[i].getLocation() == start)
                        options++;
                    else if(pieces[i].getFile() == start[0])
                        alts1++;
                    else if(pieces[i].getRank() == start[1])
                        alts2++;
                }
            }

            route.push_back(start);
            route.push_back(end);
            
            if(options == 1 && alts1 != 0 && alts2 != 0)
            {
                if(captureMove == true && findPiece(pieces, end) == -1)
                    return badRoute;
                else if(captureMove == false && findPiece(pieces, end) != -1)
                    return badRoute;
                else
                    return route;
            }
            else if(options == 0)
                return route;
            else
                return badRoute;
        }  
    }

    return badRoute;
}
//This function, along with getCaptureBoxNear, outputs a string that, when printed, displays the captured pieces in printBoard
std::string getCaptureBoxFar(std::vector<Piece> &pieces, int turn)
{
    std::string captives;

    captives.append("\t\t\t           ");

    for(int x = 0; x < 3; x++)
        captives.append("        ");

    if(turn == WHITE)
    {
        captives.append("     BLACK\n\n\t\t\t **");
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            captives.append("=====");
        
        captives.append("**\n\t\t\t ||");
        
        //This includes the pieces captured by the opposing player
        for(int i = MAX_SQUARES - PIECES_PER_TEAM; i < MAX_SQUARES; i++) 
        {
            if(pieces[i].getType() == KING)
                continue;
            else if(pieces[i].getIsCaptured() == true)
                captives.append(pieces[i].getFirstName(turn));
            else
                captives.append("     ");
            
            if(i % PIECES_PER_TEAM == 9)
                captives.append("     ");
        }

        captives.append("||\n\t\t\t **");
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            captives.append("=====");
        
        captives.append("**\n\n\t\t\t          ");
    }
    else if(turn == BLACK)
    {
        captives.append("     WHITE\n\n\t\t\t **");
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            captives.append("=====");
        
        captives.append("**\n\t\t\t ||");
        
        //This prints the pieces captured by the opposing player
        for(int i = PIECES_PER_TEAM - 1; i >= 0; i--) 
        {
            if(pieces[i].getType() == KING)
                continue;
            else if(pieces[i].getIsCaptured() == true)
                captives.append(pieces[i].getFirstName(turn));
            else
                captives.append("     ");
            
            if(i % PIECES_PER_TEAM == 8)
                captives.append("     ");
        }
        captives.append("||\n\t\t\t **");
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            captives.append("=====");
        
        captives.append("**\n\n\t\t\t          ");
    }

    return captives;
}
//This function, along with getCaptureBoxFar, displays the captured pieces when called by printBoard
std::string getCaptureBoxNear(std::vector<Piece> &pieces, int turn)
{
    std::string captives;

    if(turn == WHITE)
    {
        captives.append("\n\n\t\t\t **");
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            captives.append("=====");
        
        captives.append("**\n\t\t\t ||");
        
        //This includes the pieces captured by the current player
        for(int i = PIECES_PER_TEAM - 1; i >= 0; i--) 
        {
            if(pieces[i].getType() == KING)
                continue;
            else if(pieces[i].getIsCaptured() == true)
                captives.append(pieces[i].getFirstName(turn));
            else
                captives.append("     ");
            
            if(i % PIECES_PER_TEAM == 8)
                captives.append("     ");
        }
        captives.append("||\n\t\t\t **");
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            captives.append("=====");
        
        captives.append("** \n\n\t\t\t           ");
        
        for(int x = 0; x < 3; x++)
            captives.append("        ");

        captives.append("     WHITE\n");
    }
    else if(turn == BLACK)
    {
        captives.append("\n\n\t\t\t **");
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            captives.append("=====");
        
        captives.append("**\n\t\t\t ||");
        
        //This prints the pieces captured by the current player
        for(int i = MAX_SQUARES - PIECES_PER_TEAM; i < MAX_SQUARES; i++) 
        {
            if(pieces[i].getType() == KING)
                continue;
            else if(pieces[i].getIsCaptured() == true)
                captives.append(pieces[i].getFirstName(turn));
            else
                captives.append("     ");
            
            if(i % PIECES_PER_TEAM == 9)
                captives.append("     ");
        }

        captives.append("||\n\t\t\t **");
        for(int num = 0; num < PIECES_PER_TEAM; num++)
            captives.append("=====");
        
        captives.append("** \n\n\t\t\t           ");

        for(int x = 0; x < 3; x++)
            captives.append("        ");

        captives.append("     BLACK\n");
    }

    return captives;
}
//Printers///////////////////////////
//This function prints the board and the pieces at their locations on the board
/*void printBoard(std::vector<Piece> &pieces, int turn, bool manualClaim, std::string finput)
{
    std::string chessBoard;

    for(int e = 0; e < MAX_SQUARES; e++)
        pieces[e].setName(pieces[e].getType(), pieces[e].getTeam(), turn);

    chessBoard.append("\n\n\n\n\n\n\n\n\n\n\n\n");

    if(manualClaim == true)
    {
        chessBoard.append(finput);
        chessBoard.append("\n\n\n\n\n\n");
    }
    
    chessBoard.append(getCaptureBoxFar(pieces, turn));

    if(turn == WHITE)
    {   
        for(char a = 'a'; a <= 'h'; a++)
        {
            chessBoard.append("   (" );
            chessBoard.append(1, a);
            chessBoard.append(")  ");
        }

        for(int i = NUM_RANKS; i > 0; i--)
        {
            for(int row = 0; row < ROWS_PER_RANK; row++)
            {
                chessBoard.append("\n\t\t\t");

                if(row == 0)
                {
                    chessBoard.append("          ");

                    for(int j = 0; j < NUM_FILES; j++)
                        chessBoard.append("*-------"); //This prints the top edge of each row

                    chessBoard.append("*");
                }
                else if(row != 2)
                {
                    chessBoard.append("          ");
                    for(int j = 0; j < NUM_FILES; j++) //This prints the top and bottom thirds of each actual square
                    {
                        if((i+j) % 2 == LIGHT)
                            chessBoard.append("|*******");
                        else
                            chessBoard.append("|       ");
                    }
                    chessBoard.append("| ");
                }
                else
                {
                    chessBoard.append("      (" );
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");

                    for(int j = 0; j < NUM_FILES; j++) //This prints the center third of each square, and any piece that may be on it
                    {
                        if((i+j) % 2 == LIGHT)
                        {
                            chessBoard.append("|*");
                            for(int k = 0; k < MAX_SQUARES; k++)
                                chessBoard.append(pieces[k].getNameString(i, j));
                            
                            chessBoard.append("*");
                        }
                        else if((i+j) % 2 == DARK)
                        {
                            chessBoard.append("| ");
                            for(int k = 0; k < MAX_SQUARES; k++)
                                chessBoard.append(pieces[k].getNameString(i, j));

                            chessBoard.append(" ");
                        }
                    }
                    chessBoard.append("| (" );
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");
                }
            }
        }

        chessBoard.append("\n\t\t\t          ");

        for(int j = 0; j < NUM_FILES; j++)
            chessBoard.append("*-------");

        chessBoard.append("*");

        chessBoard.append("\n\t\t\t          ");

        for(char a = 'a'; a <= 'h'; a++)
        {
            chessBoard.append("   (" );
            chessBoard.append(1, a);
            chessBoard.append(")  ");
        }
    }
    else if(turn == BLACK) //resume here
    {
        for(char a = 'h'; a >= 'a'; a--)
        {
            chessBoard.append("   (");
            chessBoard.append(1, a);
            chessBoard.append(")  ");
        }

        for(int i = 1; i <= NUM_RANKS; i++)
        {
            for(int row = 0; row < ROWS_PER_RANK; row++)
            {
                chessBoard.append("\n\t\t\t");
                if(row == 0)
                {
                    chessBoard.append("          ");
                    for(int j = NUM_FILES - 1; j >= 0; j--)
                        chessBoard.append("*-------"); //This will be used to print the top edge of each row
                    
                    chessBoard.append("*");
                }
                else if(row != 2)
                {
                    chessBoard.append("          ");
                    for(int j = NUM_FILES - 1; j >= 0; j--) //This will be used to print the top and bottom thirds of each actual square
                    {
                        if((i+j) % 2 == LIGHT)
                            chessBoard.append("|*******");
                        else
                            chessBoard.append("|       ");
                    }
                    chessBoard.append("| ");
                }
                else
                {
                    chessBoard.append("      (");
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");

                    for(int j = NUM_FILES - 1; j >= 0; j--) //This prints the center third of each square, and any piece that may be on it
                    {
                        if((i+j) % 2 == LIGHT)
                        {
                            chessBoard.append("|*");
                            for(int k = 0; k < MAX_SQUARES; k++)
                                chessBoard.append(pieces[k].getNameString(i, j));
                            
                            chessBoard.append("*");
                        }
                        else if((i+j) % 2 == DARK)
                        {
                            chessBoard.append("| ");
                            for(int k = 0; k < MAX_SQUARES; k++)
                                chessBoard.append(pieces[k].getNameString(i, j));
                            
                            chessBoard.append(" ");
                        }
                    }
                    chessBoard.append("| (" );
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");
                }
            }
        }

        chessBoard.append("\n\t\t\t          ");

        for(int j = NUM_FILES - 1; j >= 0; j--)
            chessBoard.append("*-------");

        chessBoard.append("*\n\t\t\t          ");

        for(char a = 'h'; a >= 'a'; a--)
        {
            chessBoard.append("   (");
            chessBoard.append(1, a);
            chessBoard.append(")  ");
        }     
    }

    chessBoard.append(getCaptureBoxNear(pieces, turn));

    std::cout << chessBoard;
    return;
}*/

//This function returns a string that can be printed to display the board and the pieces at their locations
std::string printBoard(std::vector<Piece> &pieces, int turn, bool manualClaim, std::string finput, int next)
{

    std::string chessBoard = "\n\n\n";

    for(int e = 0; e < MAX_SQUARES; e++)
        pieces[e].setName(pieces[e].getType(), pieces[e].getTeam(), turn);

    //chessBoard.append("\n\n\n\n\n\n\n\n\n\n\n\n");

    if(manualClaim == true)
    {
        chessBoard.append(finput);
        chessBoard.append("\n\n\n\n\n\n");
    }
    
    chessBoard.append(getCaptureBoxFar(pieces, turn));

    if(turn == WHITE)
    {   
        for(char a = 'a'; a <= 'h'; a++)
        {
            chessBoard.append("   (" );
            chessBoard.append(1, a);
            chessBoard.append(")  ");
        }

        for(int i = NUM_RANKS; i > 0; i--)
        {
            for(int row = 0; row < ROWS_PER_RANK; row++)
            {
                chessBoard.append("\n\t\t\t");

                if(row == 0)
                {
                    chessBoard.append("          ");

                    for(int j = 0; j < NUM_FILES; j++)
                        chessBoard.append("*-------"); //This prints the top edge of each row

                    chessBoard.append("*");
                }
                else if(row != 2)
                {
                    chessBoard.append("          ");
                    for(int j = 0; j < NUM_FILES; j++) //This prints the top and bottom thirds of each actual square
                    {
                        if((i+j) % 2 == LIGHT)
                            chessBoard.append("|*******");
                        else
                            chessBoard.append("|       ");
                    }
                    chessBoard.append("| ");
                }
                else
                {
                    chessBoard.append("      (" );
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");

                    for(int j = 0; j < NUM_FILES; j++) //This prints the center third of each square, and any piece that may be on it
                    {
                        if((i+j) % 2 == LIGHT)
                        {
                            chessBoard.append("|*");
                            for(int k = 0; k < MAX_SQUARES; k++)
                                chessBoard.append(pieces[k].getNameString(i, j));
                            
                            chessBoard.append("*");
                        }
                        else if((i+j) % 2 == DARK)
                        {
                            chessBoard.append("| ");
                            for(int k = 0; k < MAX_SQUARES; k++)
                                chessBoard.append(pieces[k].getNameString(i, j));

                            chessBoard.append(" ");
                        }
                    }
                    chessBoard.append("| (" );
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");
                }
            }
        }

        chessBoard.append("\n\t\t\t          ");

        for(int j = 0; j < NUM_FILES; j++)
            chessBoard.append("*-------");

        chessBoard.append("*");

        chessBoard.append("\n\t\t\t          ");

        for(char a = 'a'; a <= 'h'; a++)
        {
            chessBoard.append("   (" );
            chessBoard.append(1, a);
            chessBoard.append(")  ");
        }
    }
    else if(turn == BLACK) //resume here
    {
        for(char a = 'h'; a >= 'a'; a--)
        {
            chessBoard.append("   (");
            chessBoard.append(1, a);
            chessBoard.append(")  ");
        }

        for(int i = 1; i <= NUM_RANKS; i++)
        {
            for(int row = 0; row < ROWS_PER_RANK; row++)
            {
                chessBoard.append("\n\t\t\t");
                if(row == 0)
                {
                    chessBoard.append("          ");
                    for(int j = NUM_FILES - 1; j >= 0; j--)
                        chessBoard.append("*-------"); //This will be used to print the top edge of each row
                    
                    chessBoard.append("*");
                }
                else if(row != 2)
                {
                    chessBoard.append("          ");
                    for(int j = NUM_FILES - 1; j >= 0; j--) //This will be used to print the top and bottom thirds of each actual square
                    {
                        if((i+j) % 2 == LIGHT)
                            chessBoard.append("|*******");
                        else
                            chessBoard.append("|       ");
                    }
                    chessBoard.append("| ");
                }
                else
                {
                    chessBoard.append("      (");
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");

                    for(int j = NUM_FILES - 1; j >= 0; j--) //This prints the center third of each square, and any piece that may be on it
                    {
                        if((i+j) % 2 == LIGHT)
                        {
                            chessBoard.append("|*");
                            for(int k = 0; k < MAX_SQUARES; k++)
                                chessBoard.append(pieces[k].getNameString(i, j));
                            
                            chessBoard.append("*");
                        }
                        else if((i+j) % 2 == DARK)
                        {
                            chessBoard.append("| ");
                            for(int k = 0; k < MAX_SQUARES; k++)
                                chessBoard.append(pieces[k].getNameString(i, j));
                            
                            chessBoard.append(" ");
                        }
                    }
                    chessBoard.append("| (" );
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");
                }
            }
        }

        chessBoard.append("\n\t\t\t          ");

        for(int j = NUM_FILES - 1; j >= 0; j--)
            chessBoard.append("*-------");

        chessBoard.append("*\n\t\t\t          ");

        for(char a = 'h'; a >= 'a'; a--)
        {
            chessBoard.append("   (");
            chessBoard.append(1, a);
            chessBoard.append(")  ");
        }     
    }

    chessBoard.append(getCaptureBoxNear(pieces, turn));

    return chessBoard;
}

//This function takes an argument BLACK or WHITE and the current turn number and prints a corresponding string.
/*void printTurn(int turnNum, int color, bool manualClaim, bool wasOffered) 
{
    std::string p1 = "White";
    std::string p2 = "Black";

    std::string offerClause{""};
    std::string claimClause{""};

    std::string turnPhrase{""};
    
    if(wasOffered == true)
        turnPhrase.append("\n");
    else if(wasOffered == false)
        offerClause = ", \nor add 'offer draw' to the end of your move to offer a draw";
    if(manualClaim == true && wasOffered == false)
        claimClause = ", or enter or add 'claim draw' to initiate a draw claim";

    if(color == WHITE)
    {
        turnPhrase.append("\n");
        turnPhrase.append(std::to_string(turnNum));
        turnPhrase.append(". White to move. Please enter your move or 'I resign' to resign");
        turnPhrase.append(offerClause);
        turnPhrase.append(claimClause);
        turnPhrase.append(":");
    }
    else if(color == BLACK)
    {
        turnPhrase.append("\n");
        turnPhrase.append(std::to_string(turnNum));
        turnPhrase.append("... Black to move. Please enter your move or 'I resign' to resign");
        turnPhrase.append(offerClause);
        turnPhrase.append(claimClause);
        turnPhrase.append(":");
    }

    std::cout << turnPhrase;    
    return;    
}*/

//This function takes an argument BLACK or WHITE and the current turn number and prints a corresponding string.
std::string printTurn(int turnNum, int color, bool manualClaim, bool wasOffered, int next) 
{
    std::string p1 = "White";
    std::string p2 = "Black";

    std::string offerClause{""};
    std::string claimClause{""};

    std::string turnPhrase{""};
    
    if(wasOffered == true)
        turnPhrase.append("\n");
    else if(wasOffered == false)
        offerClause = ", \nor add 'offer draw' to the end of your move to offer a draw";
    if(manualClaim == true && wasOffered == false)
        claimClause = ", or enter or add 'claim draw' to initiate a draw claim";

    if(color == WHITE)
    {
        turnPhrase.append("\n");
        turnPhrase.append(std::to_string(turnNum));
        turnPhrase.append(". White to move. Please enter your move or 'I resign' to resign");
        turnPhrase.append(offerClause);
        turnPhrase.append(claimClause);
        turnPhrase.append(": \n");
    }
    else if(color == BLACK)
    {
        turnPhrase.append("\n");
        turnPhrase.append(std::to_string(turnNum));
        turnPhrase.append("... Black to move. Please enter your move or 'I resign' to resign");
        turnPhrase.append(offerClause);
        turnPhrase.append(claimClause);
        turnPhrase.append(": \n");
    }

    if(next != 10)
        turnPhrase.append("Executing multiple moves...");

    return turnPhrase;       
}
//Finders/////////////////////////
//This function returns the index of the Piece object in pieces that exists at a location and where team != EMPTY. If no such piece exists, it returns -1.
int findPiece(std::vector<Piece> pieces, std::string location)
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
int findPiece(std::vector<Piece> pieces, char f, char r)
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
int findEmpty(std::vector<Piece> pieces, std::string location)
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
int findKing(std::vector<Piece> pieces, int color)
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
                //std::cout << pieces[p2].getName() << pieces[p2].getFile();
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

    if(j != -1)
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
void promotePawn(std::vector<Piece> &pieces, int i, int turnNum, bool wasOffered)
{
    std::string output;
    output.append(printBoard(pieces, -1*pieces[i].getTeam(), false, " ", 10));
    output.append(printTurn(turnNum, -1*pieces[i].getTeam(), false, wasOffered, 10)); //prints the board and pieces
    std::cout << output << std::flush;
    output.clear();

    pauseRun(2000);

    output.append(printBoard(pieces, pieces[i].getTeam(), false, " ", 10));
    output.append("PAWN PROMOTION! \nPlease select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: \n");

    std::cout << output;
    output.clear();

    char newVal{' '};

    while(newVal == ' ')
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
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
            output.append(printBoard(pieces, pieces[i].getTeam(), false, " ", 10));
            output.append("\nInvalid input. \nPlease enter 'q', 'r', 'b', or 'n': ");
            std::cout << output << std::flush;
            output.clear();
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
        
        if(rook == -1)
            return -1;

        for(char c = 'e'; c < 'h'; c++)
        {
            if(findPiece(pieces, c, pieces[i].getRank()) != -1 && findPiece(pieces, c, pieces[i].getRank()) != i) //All spaces between the king and the rook must be empty
                return -1;
            else if(j0 != -1) //This means there cannot be a piece there to capture
                return -1;
            else if(pieces[rook].getTimesMoved() != 0) //The rook must not have moved
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

        if(rook == -1)
            return -1;

        for(char c = 'e'; c > 'a'; c--)
        {
            if(findPiece(pieces, c, pieces[i].getRank()) != -1 && findPiece(pieces, c, pieces[i].getRank()) != i) //All spaces between the king and the rook must be empty
                return -1;
            else if(j0 != -1) //This means there cannot be a piece there to capture
                return -1;
            else if(pieces[rook].getTimesMoved() != 0) //The rook must not have moved
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
    else if(j == -1);
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
    else if(j == -1);
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

    if(repeats == 5)
        return 5;
    else if(repeats >= 3)
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