// ChessGame13.cpp

#include<iostream> //std::cout, cin, etc
#include<fstream> //std::ifstream, std::ofstream
#include<string> //std::string, string functions
#include<vector> //std::vector, vector operations
#include<math.h> //operators
#include<algorithm> //std::count
#include<chrono>
#include<thread>
//#include<iterator>
//#include<typeinfo>
//#include<pthread.h>

#define AUTO -2
#define BLACK -1
#define EMPTY 0
#define ALT 0
#define WHITE 1
#define FLIP 2

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
    //bool canEP;
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
    //void setCanEP(bool canEP);

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
    //bool getCanEP();
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
std::vector<std::string> convertMove(std::vector<Piece> &pieces, std::string move, int turn, bool &readCin);
std::string convertNotation(std::vector<Piece> oldPieces, std::vector<Piece> pieces, pieceType pMoved, std::string move, std::string start, std::string end, int turn);
void tolower(std::string &str)
{
    for(int n = 0; n < str.size(); n++)
        str[n] = tolower(str[n]);
}

//Printers
std::string getCaptureBoxFar(std::vector<Piece> &pieces, int turn);
std::string getCaptureBoxNear(std::vector<Piece> &pieces, int turn);
std::string printBoard(std::vector<Piece> &pieces, int turn, std::string finput, int next);
std::string printTurn(int turnNum, int color, int offerTurn, bool manualClaim, bool wasOffered, bool wasClaimed, bool valid, bool validEntry, int next, int argc);

//Finders
int findPiece(std::vector<Piece> pieces, std::string location);
int findPiece(std::vector<Piece> pieces, char f, char r);
int findEmpty(std::vector<Piece> pieces, std::string location);
int findKing(int color);

//Piece manipulators
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
void removePiece(std::vector<Piece> &pieces, int i);
void promotePawn(std::vector<Piece> &pieces, int i, int turnNum, int &offerTurn, bool &wasOffered, bool &wasClaimed, bool caughtUp, std::vector<std::string> &oldInputs, std::vector<std::string> &newInputs, int &inputsRead, std::string finput, bool needsEntry);

//Legality checkers
int getLegalCastlingRookIndex(std::vector<Piece> &pieces, int i, int j0, std::string start, int oppoTurn);
bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
bool checkCanCapture(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
inline bool isDiagonal(std::string start, std::string end) // This function returns a bool representing whether or not two squares are diagonal from each other
{
    if((end[0] - start[0] != end[1] - start[1]) && (end[0] - start[0] != start[1] - end[1]))
        return false;
    else
        return true;
}

//End condition checkers
int countLegalMoves(std::vector<Piece> &pieces, int turn);
int getDraw(std::vector<Piece> &pieces, int turn, pieceType pMoved, bool reset);

//Timing functions
inline void pauseRun(int milliseconds){
    
    std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
    while(std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(milliseconds));

    return;
}

//Threads
void cinThread(std::string &T, std::string target)
{
    while(T != target)
    {
        std::cin >> T;
        //tolower(T);
    }
}

void cinThread2(std::string &T, std::string oldNav) {
    //while(T != "quit")
    while(T != "auto")
        std::this_thread::yield();
        
    while(std::cin.get() != '\n');

    T = oldNav;
    return;
}

void cinThread3(std::string &T, bool &go, bool &skip)
{
    while(1)
    {
        while(skip == true)
            std::this_thread::yield();

        std::cin >> T;
        go = true;
        if(T == "quit")
            break;
        else if(T == "jump" || T == "skip+" || T == "skip-")
            skip = true;
    }
    return;
}

/*void exitThread(int &mode, int oldMode, std::string &navigation, std::string oldNav, int &movesLeft)
{
    if(navigation != "auto")
        return;

    while(std::cin.get() != '\n' && movesLeft > 0)
    {
        std::cout << "still in auto" << std::flush;
        if(navigation != "auto")
            return;
    }

    if(navigation == "auto")
    {
        navigation = oldNav;
        mode = oldMode;
    }

    //navigation = 
    //mode = oldMode;
    return;
}*/


//MAIN FUNCTION DEFINITION////////////////////////////////
int main(int argc, char **argv)
{
    std::string finput{""};
    std::string finput2{""};
    std::string start;
    std::string end;
    std::vector<std::string> newInputs;
    std::vector<std::string> newOldInput;
    bool resume{false};
    bool userInput{true};
    std::vector<bool> converted;

    for(int num = 0; num < 3; num++)
        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    
    std::cout << "Welcome to ChessGame13.cpp! \n" << std::endl;

    if(argc == 1)
        std::cout << "If you wish to save a record of your game, or use such a record to resume an incomplete game, \nenter 'quit' to quit, then relaunch while including a .txt file in the command line. \nOtherwise enter 'start' to begin the game: ";
    else if(argc == 2)
        std::cout << "If you do not wish to save a record of a game, enter 'quit' to quit, \nthen relaunch without including a .txt file in the command line. \n\nIf you wish to resume or review an old game, make sure the included .txt file contains \na log of the moves already performed in standard algebraic notation (SAN), \nand enter 'resume' or 'review', respectively, \nor enter 'start' to begin the game: ";
    else
    {
        std::cout << "Too many arguments passed. Exiting... \n" << std::endl;
        pauseRun(1000);
        return 0;
    }

    std::string choice;

    while(choice != "quit" && choice != "start" && ((choice != "resume" && choice != "review") || argc != 2))
    {
        std::cin >> choice;
        tolower(choice);

        if(choice == "quit")
        {
            std::cout << "Quitting... \n" << std::endl;
            pauseRun(1000);
            return 0;
        }
        else if(choice == "start")
            break;
        else if((choice == "resume" || choice == "review") && argc == 2)
            break;
        else
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');

            std::cout << "Please enter either 'quit' or 'start': ";

            continue;
        } 
    }

    std::vector<Piece> pieces = initializePieces();
    std::vector<Piece> startPieces = pieces;
    std::string output1;
    static unsigned long int turnNum{1};
    static unsigned long int newTurnNum{0};
    int turn{WHITE};
    int oppoTurn{BLACK};
    static int newTurn{EMPTY};
    static int offerTurn{WHITE};
    static int getDrawNum = getDraw(pieces, turn, PAWN, true);
    bool manualClaim{false};
    bool wasClaimed{false};
    bool wasOffered{false};
    static bool caughtUp{true};
    bool readCin{false};
    bool decision{true};

    if((choice == "resume" || choice == "review") && argc == 2)
    {
        if(choice == "review")
            userInput = false;

        std::ifstream oldLog;
        oldLog.open(argv[1]);

        int i{0};
        std::string extraHead{""};
        std::string newOldMove;
        char c;
        newTurn = WHITE;
        newTurnNum = 1;
        resume = true;
        caughtUp = false;
       

        while(1)
        {
            oldLog.get(c);
            extraHead.append(1, c);
            
            if(oldLog.good() == false)
            {
                std::cout << "\nNothing to " << choice << ". \n" << argv[1] << " is empty. \nExiting..." << std::endl;
                pauseRun(2000);
                oldLog.close();
                std::cout << "Goodbye! \n" << std::endl;
                return 0;
            }
            else if(i < 4)
                i++;
            else if(extraHead[i-4] == '\n' && extraHead[i-3] == '1' && extraHead[i-2] == '.' && extraHead[i-1] == ' ')
            {
                oldLog.unget();
                break;
            }
            else
                i++;
        }
        for(int n = 0; n < extraHead.size()-5; n++)
            finput.append(1, extraHead[n]);

        while(oldLog.good() == true)
        {
            if(newTurn == WHITE)
                std::getline(oldLog, newOldMove, '\t');
            else if(newTurn == BLACK)
                std::getline(oldLog, newOldMove, '\n');

            bool gotOffer{false};
            int extraTail{0};

            /*for(int n = 0; n < newOldMove.size(); n++)
            {
                if(n == 0)
                    continue;
                if(newOldMove[n] == ' ' && newOldMove[n-1] != '.')
                {
                    std::cout << "Could not read file beyond move " << newTurnNum - 1 << ". Please convert one or more moves into standard algebraic notation (SAN) and relaunch the program. " << std::endl;

                    pauseRun(1000);
                    std::cout << "Goodbye!" << std::endl;
                    oldLog.close();
                    pauseRun(500);

                    return 0;
                }
            }*/

            if(std::count(newOldMove.begin(), newOldMove.end(), '-') == 1 && choice == "resume")
            {
                std::cout << "This game is already over. Goodbye! " << std::endl;
                oldLog.close();
                pauseRun(1000);
                return 0;
            }

            if(std::count(newOldMove.begin(), newOldMove.end(), '(') == 1)
            {
                gotOffer = true;
                extraTail += 3;
            }

            if(std::count(newOldMove.begin(), newOldMove.end(), '+') == 1)
                extraTail++;

            if(std::count(newOldMove.begin(), newOldMove.end(), '#') > 0)
            {
                while(std::count(newOldMove.begin(), newOldMove.end(), '#') > 0)
                    newOldMove.pop_back();
            }

            if(extraTail != 0)
            {
                for(int n = 0; n < extraTail; n++)
                    newOldMove.pop_back();
            }

            newInputs.push_back(newOldMove);

            if(gotOffer == true)
                newInputs.push_back("offer");
 
            newTurn *= -1;
            if(newTurn == WHITE)
                newTurnNum++;

            if(oldLog.good() == false)
                break;

            extraHead.clear();
            i = 0;

            while(oldLog.good() == true)
            {
                oldLog.get(c);
                extraHead.append(1, c);

                if(i < 1)
                    i++;
                else if (extraHead[i-1] != '.' || extraHead[i] != ' ')
                    i++;
                else
                    break;
            }
            extraTail = 0;
            gotOffer = false;
        }
        oldLog.close();
        pieces = startPieces;
    }

    const bool newGame = !resume;

    if(argc == 2 && choice != "review")
    {
        std::cout << "\nDo you wish to claim draws by threefold repetition and the fifty-move rule automatically (recommended for casual games) \nor manually (FIDE standard)? \nEnter 'auto' or 'man': ";

        while(choice[0] != 'a' && choice[0] != 'm')
        {
            std::cin >> choice;

            for(int i = 0; i < choice.size(); i++)
                tolower(choice);

            if(choice == "quit")
            {
                std::cout << "Quitting..." << std::endl;
                return 0;
            }
            
            if(choice[0] == 'a')
            {
                std::cout << "\nAll draws will be automatically declared... " << std::endl;
                break;
            }
            else if(choice[0] == 'm')
            {
                std::cout << "\nDraws via the 50-move rule and threefold repetition must be claimed. \nThe current list of moves will be printed above the board. " << std::endl;
                manualClaim = true;
                break;
            }
            else
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');

                std::cout << "\nPlease enter either 'auto' or 'man': ";

                continue;
            } 
        }

        if(resume == false)
        {
            std::string wName, bName;
            bool isDefault{false};

            std::cout << "\nEnter White's name, or enter 'default' (case-sensitive) to begin the game: ";
            std::cin >> wName;
        
            if(wName == "default" || wName == "\\\\\\")
            {
                wName = "White";
                bName = "Black";
                isDefault = true;
            }
            finput.append(wName);

            finput.append("\t vs. \t");

            
            if(isDefault == false){
                std::cout << "Enter Black's name, or enter 'default' (case-sensitive) to begin the game: ";
                std::cin >> bName;
            }

            if(bName == "default")
                bName = "Black";

            finput.append(bName);
            finput.append("\n");
        }
    }
    else if(choice != "review")
        finput.append("White \t vs.\tBlack\n");
    else
        manualClaim = false; //This is redundant, but I wanted to provide clarity.

    output1.append(printBoard(pieces, turn, finput, 10));
    output1.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, true, true, 10, argc));

    if(choice != "review")
    {
        std::string magicWord{""};

        std::thread th1(cinThread, std::ref(magicWord), "go");
        th1.detach();

        for(int num = 0; num < 3; num++)
            std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

        std::cout << "\nWelcome to ChessGame13.cpp! \n\nTo play, enter your desired move or moves in either chess notation or by entering the start and end squares of the desired move.\n" << std::endl;
        
        std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
        while(magicWord != "go" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(3000));
        
        std::cout << "Multiple commands can be entered at once, in which case each will be performed sequentially.\n" << std::endl;

        t = std::chrono::steady_clock::now();
        while(magicWord != "go" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(4000));

        std::cout << "To resign, enter 'I resign' in place of your move.\n" << std::endl;

        t = std::chrono::steady_clock::now();
        while(magicWord != "go" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(3500));

        std::cout << "If you wish to perform additional actions during your move, such as offering " << std::flush;

        if(manualClaim == true)
            std::cout << "or claiming " << std::flush;

        std::cout << "a draw, \nyou must do so before pressing enter to input your move. \n" << std::endl;

        t = std::chrono::steady_clock::now();
        while(magicWord != "go" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(5500));
        
        std::cout << "If you plan to offer a draw at the end of your turn and you are promoting a pawn, \ninclude the promotion before the draw offer when entering your move, \ne.g. 'exd8=Q offer draw', 'exd8 q offer draw', or 'e7 d8 q offer draw'.\n " << std::endl; 

        t = std::chrono::steady_clock::now();
        while(magicWord != "go" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(15000));

        if(magicWord != "go")
        {
            std::cout << "In the future, you may skip these instructions by entering 'go' at any time. Enter 'go' to begin. " << std::flush;    
            while(magicWord != "go");
        }
        else
        {
            for(int num = 0; num < 3; num++)
                std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

            std::cout << "Welcome back! " << std::flush;
        }
    }

    std::cout << "\nLoading..." << std::flush;
    pauseRun(1750);

    if(choice != "review")
        std::cout << output1;

    bool gameOver{false};
    static int inputsRead{0};

    const std::string fSkeleton = finput;
    std::vector<std::string> oldBoards;
    std::vector<std::string> oldBoardsFlip;
    std::vector<std::string> oldBoardsWhite;
    std::vector<std::string> oldBoardsBlack;
    std::vector<std::string> oldInputs;
    std::string turnStr;
    int format(2);
    std::chrono::steady_clock::time_point turnTime = std::chrono::steady_clock::now();

    while(gameOver == false)
    {
        std::string newOutput;
        std::string oldOutput;
        std::string outputCopy;
        std::string move;
        std::string buffer;
        std::vector<std::string> route;
        pieceType pMoved{NO_PIECE}; 
        std::vector<Piece> oldPieces = pieces;
        std::string newInput;
        std::string fNewInput;
        bool moveEntered{true};
        bool valid{true};
        bool validEntry{true};
        static bool isCheck{false};
        static int countMoves{0};
        int next{10};
        bool appendDraw{false};
        
        fNewInput.clear();
        turnStr = std::to_string(turnNum);
        turnTime = std::chrono::steady_clock::now();
        wasClaimed = false;

        if(finput[finput.size()-3] == '(' && finput[finput.size()-2] == '=' && finput[finput.size()-1] == ')')
        {
            oldInputs.push_back("offer");
            oldInputs.push_back("draw");
        }

        while(gameOver == false) //Start of inner loop. This loop iterates every attempted move.
        {
            int i{-1}; //This will become the index of the startPiece
            oldPieces = pieces;
            appendDraw = false;
            readCin = caughtUp;
            
            if(wasOffered == false)
                offerTurn = oppoTurn;
            else if(offerTurn == oppoTurn)
                wasOffered = false;
            
            if(caughtUp == false) //One or more moves to be implemented have already been stored somewhere other than the standard input buffer. Moves will be implemented without pausing, blocking, or outputting to the console.
            {
                if(resume == false)
                {
                    start = newInputs[inputsRead++];
                    end = newInputs[inputsRead++];
                }
                else if(resume == true)
                {
                    move = newInputs[inputsRead++];

                    if(move == "offer")
                    {
                        start = "offer";
                        end = "draw";
                    }
                    else
                    {
                        route = convertMove(pieces, move, turn, readCin);
                        start = route[0];
                        end = route[1];

                        if(route.size() == 3)
                            newInputs.insert(newInputs.begin()+inputsRead, route[2]);
                    }        
                }

                if(end == "draw" || end == "resign")
                {
                    moveEntered = false;
                    valid = false;
                    oldInputs.push_back(start);
                    oldInputs.push_back(end);
                }
                else if(turnNum == newTurnNum && turn == newTurn)
                {
                    if(choice == "review" && getDrawNum == 0)
                    {
                        std::cout << "Game unfinished. Nothing to review. \nGoodbye!" << std::endl;
                        pauseRun(1000);
                        return 0;
                    }
                    else if(choice == "review" && getDrawNum > 0)
                    {
                        moveEntered = false;
                        valid = false;
                    }
                    caughtUp = true;
                    resume = false;
                    oldOutput.append(printBoard(pieces, turn, finput, next));
                    oldOutput.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, true, true, 10, argc));
                    std::cout << oldOutput << std::flush;

                    oldOutput.clear();
                    newInputs.clear();
                    finput2.clear();

                    inputsRead = 0;
                    
                    continue;
                }
                else
                {
                    moveEntered = true;
                    i = findPiece(pieces, start);

                    if(i != -1)
                        pMoved = pieces[i].getType(); //This will be used later but needs to be initialized now.
                    
                    for(int e = 0; e < MAX_SQUARES; e++)
                        pieces[e].setIsCastling(false);

                    valid = movePiece(pieces, start, end, turn); //If the move is valid, performs the move and returns true. Else, returns false.

                    if(valid == true)
                    {
                        oldInputs.push_back(start);
                        oldInputs.push_back(end);
                    }
                    else
                    {
                        if(choice == "review")
                            std::cout << printBoard(pieces, turn, finput, 10) << std::flush;

                        std::cout << "Error! Invalid move at " << turnNum;

                        if(turn == WHITE)
                            std::cout << ", white." << std::endl;
                        else
                            std::cout << ", black." << std::endl;

                        newTurnNum = turnNum;
                        newTurn = turn;
                        continue;
                    }
                    
                }

                move = start;
            }
            else if(caughtUp == true)
            {
                //std::chrono::steady_clock::time_point timeDiff;
                oldOutput = printBoard(pieces, oppoTurn, finput, next);

                std::cin >> move;
                buffer = move;

                if(/*std::cin.peek() == 10 && */std::chrono::steady_clock::now() - turnTime > std::chrono::milliseconds(100))
                {
                    next = 10;
                    if(wasOffered == false)
                        offerTurn = turn;
                }
                else if(std::chrono::steady_clock::now() - turnTime <= std::chrono::milliseconds(100))
                {
                    next = 0;
                    std::cout << "Executing multiple moves... " << std::flush;
                    pauseRun(1000);
                }
                
                buffer = move;
                tolower(buffer);

                if((buffer == "adjourn" || buffer == "pause") && argc == 2)
                {
                    gameOver = true;
                    decision = false;
                    oldOutput = printBoard(pieces, turn, finput, 10);

                    std::cout << oldOutput << "Game adjourned. This game can be resumed by relaunching ChessGame13.cpp \nwith the same command-line arguments and entering 'resume'. Goodye!" << std::endl;

                    break;

                }
                else if(buffer == "undo")
                {
                    oldOutput.clear();
                    oldOutput.append(printBoard(pieces, turn, finput, next));
                    oldOutput.append(turnStr);
                    if(turn == WHITE)
                        oldOutput.append(". ");
                    else if(turn == BLACK)
                        oldOutput.append("..." );

                    std::cout << "\n" << oldOutput << "Please enter the turn number and player to move ('white' or 'black') \nyou would like to return to, or enter 0 to cancel: " << std::endl;

                    std::string alternate;
                    std::string confirm{""};
                        
                    do 
                    {  
                        std::cin >> newTurnNum;

                        if(newTurnNum == 0)
                        {
                            oldOutput.clear();
                            oldOutput.append(printBoard(pieces, oppoTurn, finput, next));
                            
                            newOutput.clear();
                            newOutput.append(printBoard(pieces, turn, finput, 10));
                            newOutput.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, valid, validEntry, 10, argc));

                            std::cout << newOutput << std::flush;
                            newTurnNum = turnNum;
                            newTurn = turn;

                            break;
                        }
                        
                        while(newTurnNum > turnNum || newTurnNum <= 0)
                        {
                            std::cin.clear();
                            std::cin.ignore(10000, '\n');
                            std::cout << oldOutput << "\nPlease enter the turn number and player to move ('white' or 'black') \nyou would like to return to, or enter 'cancel' to cancel: " << std::endl;
                            
                            std::cin >> newTurnNum;

                            if(newTurnNum == 0)
                            {
                                alternate = "cancel";
                                newTurnNum = turnNum;
                                newTurn = turn;
                                break;
                            }
                        }

                        if(newTurnNum == 0)
                        {
                            alternate = "cancel";
                            newTurnNum = turnNum;
                            newTurn = turn;
                            break;
                        }

                        if(std::cin.peek() == 10)
                            std::cout << oldOutput << "\nPlease enter the player to move ('white' or 'black'), or enter 'cancel' to cancel: " << std::endl;
                        std::string player;
                        std::cin >> player;

                        tolower(player);

                        alternate = player;

                        while(player != "white" && player != "black" && player != "cancel")
                        {
                            std::cin.clear();
                            std::cin.ignore(10000, '\n');
                            std::cout << oldOutput << "\nInvalid input. \nPlease enter the player to move ('white' or 'black'), or enter 'cancel' to cancel: " << std::endl;
                            std::cin >> player;
                            tolower(player);
                            alternate = player;
                        }
                        
                        if(alternate == "cancel")
                        {
                            newTurnNum = turnNum;
                            newTurn = turn;
                            break;
                        }
                        else if(player == "white")
                            newTurn = WHITE;
                        else if(player == "black")
                            newTurn = BLACK;

                        int turn_i = (newTurnNum-1)*2;
                        if(newTurn == BLACK)
                            turn_i++;

                        std::cout << oldBoards[turn_i] << newTurnNum << std::flush;
                        if(newTurn == WHITE)
                            std::cout << ". " << std::flush;
                        else if(newTurn == BLACK)
                            std::cout << "... " << std::flush;
                                
                        std::cout << "Return to this position? \nEnter 'y' or 'n', or enter 'cancel' to cancel: " << std::endl;
                        std::cin >> confirm;
                        tolower(confirm);

                        while(confirm[0] != 'y' && confirm[0] != 'n' && confirm != "cancel")
                        {
                            std::cout << oldBoards[turn_i] << "\n" << newTurnNum << std::flush;
                            if(newTurn == WHITE)
                                std::cout << ". " << std::flush;
                            else if(newTurn == BLACK)
                                std::cout << "... " << std::flush;
                                
                            std::cout << "Please enter 'y', 'n', or 'cancel'.\nReturn to this position? " << std::endl;

                            std::cin.clear();
                            std::cin.ignore(10000, '\n');
                            std::cin >> confirm;
                            tolower(confirm);
                        }
                        if(confirm == "cancel")
                        {
                            alternate = "cancel";
                            break;
                        }

                        if(confirm[0] == 'n')
                        {
                            newTurn = turn;
                            newTurnNum = turnNum;
                            std::cout << "\n" << oldOutput << "Please enter the turn number and player to move ('white' or 'black') \nyou would like to return to, or enter 0 to cancel: " << std::endl;
                            continue;
                        }
                        
                    }while((newTurnNum > turnNum || (newTurnNum == turnNum && newTurn <= turn)) || confirm[0] == 'n');

                    if(alternate == "cancel")
                    {
                        newTurnNum = turnNum;
                        newTurn = turn;
                        oldOutput.clear();
                        
                        newOutput.clear();
                        newOutput.append(printBoard(pieces, turn, finput, 10));
                        newOutput.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, valid, validEntry, 10, argc));

                        std::cout << newOutput << std::flush;

                        continue;
                    }

                    if(alternate != "cancel")
                    {                    
                        caughtUp = false;
                        pieces = startPieces;
                        getDrawNum = getDraw(pieces, WHITE, NO_PIECE, true);
                        finput.clear();
                        finput = fSkeleton;
                        fNewInput.clear();
                        turn = WHITE;
                        oppoTurn = BLACK;
                        offerTurn = WHITE;
                        wasOffered = false;
                        wasClaimed = false;
                        turnNum = 1;
                        oldOutput.clear();
                        oldBoards.clear();
                        newInputs.clear();
                        newInputs = oldInputs;
                        oldInputs.clear();
                        turnStr = "1";
                    }
                    continue;
                }
                else if((buffer == "i" || buffer == "claim" || buffer == "offer" || buffer == "accept"))
                {
                    moveEntered = false;
                    valid = false;
                    start = buffer;
                    std::cin >> end;
                    tolower(end);
                    i = -1;

                    if(offerTurn == oppoTurn && end == "draw")
                    {
                        oldInputs.insert(oldInputs.end()-2, start);
                        oldInputs.insert(oldInputs.end()-2, end);

                        if((start == "offer" || getDrawNum == 0) && finput[finput.size()-1] != ')')
                            finput.append("(=)");

                        if(offerTurn == WHITE)
                            format += 3;        
                    }
                    else if(end == "quit") //This is a dev exit. 
                    {
                        choice = "quit";
                        gameOver = true;
                        decision = true;
                        break;
                    }

                    //next = std::cin.peek();
                }
                else
                {
                    i = findPiece(pieces, buffer); 
                    moveEntered = true;
                }
            
                if(i != -1)
                {
                    if(pieces[i].getTeam() == turn)
                    {
                        start = buffer;
                        std::cin >> end;
                        tolower(end);
                    }
                    else
                    {
                        route = convertMove(pieces, move, turn, readCin);
                        start = route[0];
                        end = route[1];
                    }

                    valid = movePiece(pieces, start, end, turn); //If the move is valid, performs the move and returns true. Else, returns false.
                }
                else if(moveEntered == true)
                {
                    route = convertMove(pieces, move, turn, readCin);
                    start = route[0];
                    end = route[1];

                    i = findPiece(pieces, start);

                    if(i != -1)
                        pMoved = pieces[i].getType(); //This will be used later but needs to be initialized now.
                    
                    for(int e = 0; e < MAX_SQUARES; e++)
                        pieces[e].setIsCastling(false);

                    valid = movePiece(pieces, start, end, turn); //If the move is valid, performs the move and returns true. Else, returns false.
                }
            }

            if(valid == false && moveEntered == true)
            {

                newOutput.append(printBoard(pieces, turn, finput, 10));
                bool validEntry{true};
                if(start == "a0" && end == "a0")
                {
                    //newOutput.erase(newOutput.end()-1);
                    //newOutput.append("Invalid entry. Try again.\n");
                    validEntry = false;
                }
                //else
                {
                    //newOutput.erase(newOutput.end()-1);
                    //newOutput.append("Invalid move. Try again.\n");
                }

                newOutput.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, false, validEntry, 10, argc));

                if(caughtUp == true)
                {
                    std::cout << newOutput << std::flush;
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                }

                newOutput.clear();

                continue;
            }
            else if(valid == true && moveEntered == true) //This is the append
            {          
                oldBoards.push_back(printBoard(oldPieces, turn, finput, 10));
                oldBoardsFlip.push_back(printBoard(oldPieces, oppoTurn, finput, 10));
                oldBoardsWhite.push_back(printBoard(oldPieces, WHITE, finput, 10));
                oldBoardsBlack.push_back(printBoard(oldPieces, BLACK, finput, 10));

                if(turn == WHITE)
                {
                    fNewInput.append("\n");
                    fNewInput.append(turnStr);
                    fNewInput.append(". ");
                }
                else if(turn == BLACK)
                {
                    fNewInput.append("\t");

                    if(format < 9)
                        fNewInput.append("\t");
                    
                    fNewInput.append(turnStr);
                    fNewInput.append("... "); 
                }
                newInput = convertNotation(oldPieces, pieces, pMoved, move, start, end, turn);
                
                fNewInput.append(newInput);
                finput.append(fNewInput);

                if(resume == false)
                    finput2.append(fNewInput);

                if(turn == WHITE)
                    format = fNewInput.size();

                /*if(wasOffered == true && offerTurn == turn)
                {
                    if(wasClaimed == true)
                        oldInputs.push_back("claim");
                    else
                        oldInputs.push_back("offer");

                    oldInputs.push_back("draw");
                }*/
                if(caughtUp == true)
                {
                    oldInputs.push_back(start);
                    oldInputs.push_back(end);
                }
                //wasClaimed = false;
                
            }
            else
            {
                if(start == "i" && end == "resign")
                {
                    if(turn == WHITE)
                    {
                        finput.append("\nWhite resigns! Black wins! \n\t0 - 1");


                        newOutput.append(printBoard(pieces, oppoTurn, finput, 10));
                        newOutput.append("White resigns! Black wins! \n");
                        std::cout << newOutput << std::flush;
                        gameOver = true;
                        
                        break;
                    }
                    else if(turn == BLACK)
                    {
                        finput.append("\nBlack resigns! White wins! \n\t1 - 0");
                            
                        newOutput.append(printBoard(pieces, oppoTurn, finput, 10));
                        newOutput.append("Black resigns! White wins! \n");
                        std::cout << newOutput << std::flush;
                        gameOver = true;
                        
                        break;
                    }
                }
                else if(start == "offer" && end == "draw" && wasOffered == false)
                {
                    wasOffered = true;
                    if(/*std::cin.peek() == 10 && */std::chrono::steady_clock::now() - turnTime > std::chrono::milliseconds(100))
                    {
                        next = 10;
                        offerTurn = turn;
                    }
                    else   
                        offerTurn = oppoTurn;

                    newOutput.append(printBoard(pieces, turn, finput, next));
                    newOutput.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, true, true, 10, argc));
                    std::cout << newOutput << std::flush;
                    newOutput.clear();

                    continue;
                }
                else if(start == "offer" && end == "draw" && wasOffered == true)
                {
                    newOutput.append(printBoard(pieces, turn, finput, next));
                    newOutput.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, true, false, 10, argc));
                    std::cout << newOutput << std::flush;
                    if(caughtUp == true)
                    {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                    }
                    continue;
                }
                else if(start == "accept" && end == "draw" && wasOffered == true)
                {
                    finput.append("\nDraw offered and accepted! \n\n\t1/2 - 1/2");
                    newOutput.append(printBoard(pieces, turn, finput, 10));
                    newOutput.append("\nDraw offered and accepted! \n");
                    std::cout << newOutput;
                    newOutput.clear();
                    gameOver = true;
                    break;
                }
                else if((start == "claim" && end == "draw" && manualClaim == true && wasClaimed == false) || (manualClaim == false && getDrawNum > 0))
                {
                    wasClaimed = manualClaim;
                    wasOffered = manualClaim;

                    if(manualClaim == true)
                    {
                        if(std::chrono::steady_clock::now() - turnTime > std::chrono::milliseconds(100))
                        {
                            next = 10;
                            offerTurn = turn;
                        }
                        else
                            offerTurn = oppoTurn;
                        
                        oldInputs.push_back(start);
                        oldInputs.push_back(end);
                    }

                    if(getDrawNum == 3)
                    {
                        finput.append("\n\n\t1/2 - 1/2");
                        newOutput.append(printBoard(pieces, oppoTurn, finput, 10));
                        newOutput.append("Threefold repetition rule invoked! It's a draw! \n");
                        std::cout << newOutput;
                        newOutput.clear();
                        gameOver = true;
                        break;
                    }
                    else if(getDrawNum >= 50)
                    {
                        finput.append("\n\n\t1/2 - 1/2");
                        newOutput.append(printBoard(pieces, oppoTurn, finput, 10));
                        newOutput.append("50-move rule invoked! It's a draw! \n");
                        std::cout << newOutput;
                        newOutput.clear();
                        gameOver = true;
                        break;
                    }
                    else
                    {
                        newOutput.append(printBoard(pieces, turn, finput, 10));
                        //newOutput.erase(newOutput.end()-1);
                        newOutput.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, true, false, 10, argc));
                        std::cout << newOutput << std::flush;
                        if(offerTurn == oppoTurn)
                            pauseRun(1000);
                        std::cout << "you can move. " << std::flush;
                        newOutput.clear();
                        gameOver = false;
                        continue;
                    }
                } 
            }
            
            if(pieces[i].getType() == PAWN && (end[1] == '1' || end[1] == '8'))
            {    
                bool redundant = wasOffered;
                //bool needsEntry{true};
                //if(move[move.size()-1] == 'Q' || move[move.size()-1] == 'R' || move[move.size()-1] == 'B' || move[move.size()-1] == 'N')
                    //needsEntry = false;

                bool needsEntry = readCin;
                promotePawn(pieces, i, turnNum, offerTurn, wasOffered, wasClaimed, caughtUp, oldInputs, newInputs, inputsRead, finput, needsEntry); //promotes the pawn to the desired piece

                if(offerTurn == turn)
                    appendDraw = !redundant;

                if(std::count(newInput.begin(), newInput.end(), '=') == 0)
                {
                    finput.append("=");
                    pieceType t = pieces[i].getType();
                    switch (t)
                    {
                    case QUEEN:
                        finput.append("Q");
                        oldInputs.push_back("Q");
                        break;
                    case ROOK:
                        finput.append("R");
                        oldInputs.push_back("R");
                        break;
                    case BISHOP:
                        finput.append("B");
                        oldInputs.push_back("B");
                        break;
                    case KNIGHT:
                        finput.append("N");
                        oldInputs.push_back("N");
                        break;
                    default:
                        break;
                    }
                }
                if(resume == false && std::count(newInput.begin(), newInput.end(), '=') == 0)
                {
                    finput2.append("=");
                    finput2.append(oldInputs[oldInputs.size()-1]);
                }
                

                if(turn == WHITE)
                    format += 2;

                oldPieces = pieces;
            }

            oppoTurn = turn;
            turn *= -1;

            //if(next != 10)
                //oldNext = next;

            oldOutput.append(printTurn(turnNum, oppoTurn, offerTurn, manualClaim, wasOffered, wasClaimed, valid, validEntry, next, argc)); // was oldNext

            //next = std::cin.peek();
            //if(next != 10)
                //oldNext = next;

            for(int a = 0; a < MAX_SQUARES; a++) 
            {
                if(pieces[a].getTeam() == turn)
                    pieces[a].setEPVul(false);
            }
            
            int k = findKing(turn);
            for(int j = 0; j < MAX_SQUARES; j++)
            {
                isCheck = checkCanCapture(pieces, pieces[j].getLocation(), pieces[k].getLocation(), oppoTurn);
                if(isCheck == true)
                    break;
            }
            countMoves = countLegalMoves(pieces, turn);

            if(turn == WHITE)
                turnNum++;

            if(isCheck == true && countMoves > 0)
            {
                if(resume == false)
                {
                    finput.append("+");
                    if(turn == BLACK)
                        format++;
                }
                    
                //wasClaimed = false;

                newOutput = printBoard(pieces, turn, finput, next);
                outputCopy = newOutput;
                newOutput.append("Check! ");
            }
            
            if(countMoves > 0 && wasOffered == true && offerTurn == oppoTurn)
            {
                finput.append("(=)");
                if(resume == false)
                    finput2.append("(=)");
                
                if(offerTurn == WHITE)
                    format += 3;

                newOutput = printBoard(pieces, turn, finput, next);
                outputCopy = newOutput;
            }
            else
            {
                newOutput = printBoard(pieces, turn, finput, next);
                outputCopy = newOutput;
            }

            newOutput.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, valid, validEntry, 10, argc));

            break;

        } //End of inner loop

        if(gameOver == true)
            break;
        else if(caughtUp == true)
        {
            std::cout << oldOutput << std::flush;
            pauseRun(1000);
        }
        
        if(isCheck == true && countMoves == 0)
        {
                finput.append("#\n\n");
                
                if(oppoTurn == WHITE)
                    finput.append("\t1 - 0");
                else
                    finput.append("\t0 - 1");

                if(resume == false)
                {
                    finput2.append("#\n\n");
                    
                    if(oppoTurn == WHITE)
                        finput2.append("\t1 - 0");
                    else
                        finput2.append("\t0 - 1");
                }

            outputCopy = printBoard(pieces, turn, finput, 10);
            outputCopy.append("\nCheckmate! ");

            if(oppoTurn == WHITE)
                outputCopy.append("White wins! ");
            else if(oppoTurn == BLACK)
                outputCopy.append("Black wins! ");

            if(choice != "review")
                std::cout << outputCopy << std::endl;

            newOutput.clear();

            break;
        }
        else if(isCheck == false && countMoves == 0)
        {
            finput.append("\n\n\t1/2 - 1/2");
            if(resume == false) finput2.append("\n\n\t1/2 - 1/2");

            outputCopy = printBoard(pieces, turn, finput, next);

            if(choice != "review")
                std::cout << outputCopy << "\nNo legal moves! Stalemate! " << std::endl;

            newOutput.clear();
            break;
        }

        getDrawNum = getDraw(pieces, turn, pMoved, false);

        if(getDrawNum == 3 && (manualClaim == false || wasClaimed == true))
        {
            std::string partingWords = "\nThreefold repetition rule ";

            if(manualClaim == false && resume == true)
            {
                if(choice != "review")
                    std::cout << outputCopy << partingWords << "would be applicable. \nTurning off automatic draw claiming... " << std::endl;

                manualClaim = true;
                pauseRun(3000);
                continue;
            }
            else
            {   
                finput.append("\n\n\t1/2 - 1/2");

                if(resume == false)
                    finput2.append("\n\n\t1/2 - 1/2");

                outputCopy = printBoard(pieces, turn, finput, 10);
                    
                if(choice != "review")
                    std::cout << outputCopy << partingWords << "invoked! It's a draw! " << std::endl;
                
                newOutput.clear();
                break;
            }
        }
        else if(getDrawNum == 50 && (manualClaim == false || wasClaimed == true))
        {
            std::string partingWords = "\n50-move rule ";

            if(manualClaim == false && resume == true)
            {
                if(choice != "review")
                    std::cout << outputCopy << partingWords << "would be applicable. \nTurning off automatic draw claiming... " << std::endl;

                manualClaim = true;
                pauseRun(3000);
                continue;
            }
            else
            {   
                finput.append("\n\n\t1/2 - 1/2");

                if(resume == false)
                    finput2.append("\n\n\t1/2 - 1/2");

                outputCopy = printBoard(pieces, turn, finput, 10);
                    
                if(choice != "review")
                    std::cout << outputCopy << partingWords << "invoked! It's a draw! " << std::endl;
                
                newOutput.clear();
                break;
            }
        }
        else if(getDrawNum == 5)
        {
            finput.append("\n\n\t1/2 - 1/2");
            
            if(resume == false)
                finput2.append("\n\n\t1/2 - 1/2");

            outputCopy = printBoard(pieces, turn, finput, 10);
            
            if(choice != "review")
                std::cout << outputCopy << "\nFivefold repetition rule invoked! Automatic draw! " << std::endl;
            newOutput.clear();
            break;
        }
        else if(getDrawNum == 75)
        {
            finput.append("\n\n\t1/2 - 1/2");
            
            if(resume == false)
                finput2.append("\n\n\t1/2 - 1/2");

            outputCopy = printBoard(pieces, turn, finput, 10);

            if(choice != "review")
                std::cout << outputCopy << "\n75-move rule invoked! Automatic draw! " << std::endl;

            newOutput.clear();
            break;
        }
        else if(getDrawNum == 100)
        {
            if(choice != "review")
                std::cout << outputCopy << "\nInsufficient material left! It's a draw! " << std::endl;

            finput.append("\n\n\t1/2 - 1/2");
            
            if(resume == false)
                finput2.append("\n\n\t1/2 - 1/2");

            newOutput.clear();
            break;
        }

        if(gameOver == false && caughtUp == true)
        {
            std::cout << newOutput << std::flush;
            oldOutput.clear();
            newOutput.clear();
        }
    } //End of outer loop

    if(argc == 2 && newGame == true && choice != "quit")
    {
        std::ofstream gameLog;
        gameLog.open(argv[1], std::ios::trunc);
        gameLog << finput;
        gameLog.close();
    }
    else if(newGame == false && choice != "quit")
    {
        std::ofstream gameLog;
        gameLog.open(argv[1], std::ios::app);
        gameLog << finput2;
        gameLog.close();
    }

    if(decision == true && choice != "quit")
    {
        if(choice != "review")
            std::cout << "Game over. Would you like to review the game? \nEnter 'yes' to enter review mode or enter 'quit' to quit: " << std::flush;

        std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
        std::thread th2(cinThread, std::ref(choice), "review");
        th2.detach();

        while(choice != "review" && choice != "quit" && userInput == true && std::chrono::steady_clock::now() -  t <= std::chrono::milliseconds(10000))
        {
            tolower(choice);
            if(choice == "yes")
                choice = "review";
        }
        if(choice == "review")
        {
            oldBoards.push_back(printBoard(pieces, turn, finput, 10));
            oldBoardsFlip.push_back(printBoard(pieces, oppoTurn, finput, 10));
            oldBoardsWhite.push_back(printBoard(pieces, WHITE, finput, 10));
            oldBoardsBlack.push_back(printBoard(pieces, BLACK, finput, 10));

            std::vector<std::vector<std::string>> oldBoardsAuto;
            std::vector<std::vector<std::string>> oldBoardsAll;
            std::vector<std::string> allBoardsTemp;

            for(int n = 0; n < oldBoards.size(); n++)
            {
                oldBoards[n].insert(oldBoards[n].size()-1, "VIEW MODE: NORMAL         ");
                oldBoardsWhite[n].insert(oldBoardsWhite[n].size()-1, "VIEW MODE: WHITE          ");
                oldBoardsBlack[n].insert(oldBoardsBlack[n].size()-1, "VIEW MODE: BLACK          ");
                oldBoardsFlip[n].insert(oldBoardsFlip[n].size()-1, "VIEW MODE: FLIPPED        ");
            }

            oldBoardsAuto.push_back(oldBoardsBlack);
            oldBoardsAuto.push_back(oldBoards);
            oldBoardsAuto.push_back(oldBoardsWhite);
            oldBoardsAuto.push_back(oldBoardsFlip);

            turn = WHITE;
            turnNum = 1;

            std::string instructions1st = " Enter '>' to advance forward one move, 'w', 'b', or 'n' respectively\n   to change the perspective to White, Black, or normal (alternating), 'flip' to temporarily flip the board, \n   'skip+ n' to skip forward n moves, '>>' to skip to the end, \n   'auto' to watch the game unfold automatically, or simply 'jump' to skip directly to any point in the game: ";

            std::string instructions = " Enter '>' to advance forward one move, '<' to return to the previous move, 'w', 'b', or 'n' respectively\n   to change the perspective to White, Black, or normal (alternating), 'flip' to temporarily flip the board, \n   'skip+ n' or 'skip- n' to skip forward or back n moves, '<<' to skip to the beginning, '>>' to skip to the end, \n   'auto' to watch the game unfold automatically, or simply 'jump' to skip directly to any point in the game: ";

            std::string instructionsLast = " Enter '<' to return to the previous move, 'w', 'b', or 'n' respectively\n   to change the perspective to White, Black, or normal (alternating), 'flip' to temporarily flip the board, \n   'skip- n' to skip back n moves, '<<' to skip to the beginning, \n   or simply 'jump' to skip directly to any point in the game: ";

            std::vector<std::string> turnStrings;
            
            for(int n = 0; n < oldBoards.size(); n++)
            {
                turnStr = std::to_string(turnNum);
                for(int m = 0; m < 2-turn; m++)
                    turnStr.append(".");

                turnStrings.push_back(turnStr);

                oldBoardsBlack[n].append(turnStr);
                oldBoards[n].append(turnStr);
                oldBoardsWhite[n].append(turnStr);
                oldBoardsFlip[n].append(turnStr);

                std::string manual;
                if(n == 0)
                    manual = instructions1st;
                else if(n == oldBoards.size()-1)
                    manual = instructionsLast;
                else
                    manual = instructions;

                oldBoards[n].append(manual);
                oldBoardsFlip[n].append(manual);
                oldBoardsWhite[n].append(manual);
                oldBoardsBlack[n].append(manual);

                for(int z = 0; z < 4; z++)
                {
                    oldBoardsAuto[z][n].insert(oldBoardsAuto[z][n].size()-1, "\nRUN MODE: AUTO");
                    oldBoardsAuto[z][n].append(turnStr);
                    oldBoardsAuto[z][n].append(" This game will advance automatically by one ply (half-move) every two seconds. ");
                    oldBoardsAuto[z][n].append("\nEnter 's' at any time to exit AUTO mode. \n");
                }

                turn *= -1;
                if(turn == WHITE)
                    turnNum++;
            }

            oldBoardsAll.push_back(oldBoardsBlack);
            oldBoardsAll.push_back(oldBoards);
            oldBoardsAll.push_back(oldBoardsWhite);
            oldBoardsAll.push_back(oldBoardsFlip);
            std::string oldBoardsRecent = oldBoards[0];
            std::string navigation{"n"};
            std::string oldNav{"n"};
            unsigned long int n{0};
            int emptyTurn{EMPTY};
            int mode{ALT};
            int oldMode{ALT};
            bool skip{false};
            bool go{false};
            //int lowestBranch{0};
            std::thread th3(cinThread3, std::ref(navigation), std::ref(go), std::ref(skip));
            th3.detach();

            while(navigation != "quit")
            {
                bool valid{true};
                skip = false;
                
                if(oldMode != mode && (mode == BLACK || mode == ALT || mode == WHITE))
                    oldMode = mode;

                switch (mode)
                {
                case BLACK:
                    oldBoardsRecent = oldBoardsBlack[n];
                    std::cout << oldBoardsBlack[n] << std::flush;
                    go = false;
                    break;
                case ALT:
                    oldBoardsRecent = oldBoards[n];
                    std::cout << oldBoards[n] << std::flush;
                    go = false;
                    break;
                case WHITE:
                    oldBoardsRecent = oldBoardsWhite[n];
                    std::cout << oldBoardsWhite[n] << std::flush;
                    go = false;
                    break;
                case FLIP:
                    oldBoardsRecent = oldBoardsFlip[n];
                    std::cout << oldBoardsFlip[n] << std::flush;
                    mode = oldMode;
                    go = false;
                    break;
                case AUTO:
                    std::cout << oldBoardsAuto[oldMode+1][n] << std::flush;
                    if(n == oldBoards.size()-1)
                        go = false;

                    pauseRun(2000);
                    break;
                
                default:
                    std::cout << oldBoardsRecent.substr(0, oldBoardsRecent.size()-424) << "INVALID INPUT. \n" << turnStrings[n] << instructions << std::flush;
                    pauseRun(1500);
                    valid = false;
                    mode += 20;
                    break;
                }

                if(mode == AUTO && (navigation == "s" || n >= oldBoards.size()-1))
                {
                    go = false;
                    mode = oldMode;
                    navigation = oldNav;
                    continue;
                }
                else if(mode == AUTO)
                {
                    go = true;
                    n++;
                }
                else
                    while(go == false);

                if(valid == false)
                    continue;
                else if(navigation == "<" && n > 0)
                    n--;
                else if(navigation == ">" && n < oldBoards.size()-1)
                    n++;
                else if(navigation == "<<")
                    n = 0;
                else if(navigation == ">>")
                    n = oldBoards.size()-1;
                else if(navigation == "skip+" || navigation == "skip-")
                {
                    std::cout << oldBoardsRecent.substr(0, oldBoardsRecent.size()-441) << "Please enter the number of moves you wish to skip, or 0 to cancel: \n\n" << std:: endl;

                    int jumpSize{0};
                    std::cin >> jumpSize;

                    jumpSize *= 2;


                    if(navigation == "skip+" && jumpSize < oldBoards.size()-n && jumpSize > 0)
                        n += jumpSize;
                    else if(navigation == "skip-" && jumpSize <= n && jumpSize > 0)
                        n -= jumpSize;
                    else if(navigation == "skip+" && jumpSize > 0)
                        n = oldBoards.size()-1;
                    else if(navigation == "skip-" && jumpSize > 0)
                        n = 0;
                    else if(jumpSize != 0)
                        mode -= 20;

                    skip = false;
                    navigation = oldNav;
                }
                else if(navigation == "w")
                    mode = WHITE;
                else if(navigation == "b")
                    mode = BLACK;
                else if(navigation == "n")
                    mode = ALT;
                else if(navigation == "flip")
                {
                    oldMode = mode;
                    mode = FLIP;
                }
                else if(navigation == "jump")
                {
                    std::cout << oldBoardsRecent.substr(0, oldBoardsRecent.size()-441) << "\nPlease enter the desired move number and player to move, \nor enter 0 to cancel: \n" << std::endl;

                    int target;
                    std::cin >> target;

                    while(target < 0 || target*2 > oldBoards.size())
                    {
                        std::cout << oldBoardsRecent.substr(0, oldBoardsRecent.size()-441) << "\nInvalid input. Please enter a valid move number and player to move, \nor 0 to cancel: " << std::endl;

                        std::cin >> target;
                    }
                    if(target == 0)
                        continue;

                    std::cout << oldBoardsRecent.substr(0, oldBoardsRecent.size()-441) << "\nPlease enter the player to move, \nor 'cancel' to cancel: \n" << std::endl;

                    std::string target2;
                    std::cin >> target2;
                    tolower(target2);

                    while(target2 != "white" && target2 != "black" && target2 != "cancel")
                    {
                        std::cout << oldBoardsRecent.substr(0, oldBoardsRecent.size()-441) << "\nInvalid input. Please enter a valid player to move \nor 'cancel' to cancel: " << std::endl;

                        std::cin >> target2;
                        tolower(target2);
                    }

                    int plus{0};
                    target--;

                    if(target2 == "black")
                        plus = 1;
                    else if(target2 == "cancel")
                        continue;
                    
                    if(target*2+plus < 0 || target*2+plus > oldBoards.size()-1)
                        mode -= 20;
                    else
                        n = target*2+plus;          
                }
                else if(navigation == "auto" && n < oldBoards.size()-1)
                    mode = AUTO;
                /*else if(navigation == "branch" &&  n >= lowestBranch && n < oldBoards.size()-1)
                {
                    if(n > lowestBranch)
                        lowestBranch = n;

                    go = false;
                    std::string move{""};
                    std::vector<Piece> treePieces = initializePieces();
                    
                    std::vector<int> treeTrunk; //This keeps track of the point in the game that the alternate path begins
                    treeTrunk.push_back(n);

                    std::vector<std::string> finputBranch;
                    std::string finput3;

                    int pathNum = finputBranch.size()+1;
                    
                    while(treeTrunk.size() > 0)
                    {
                        std::vector<Piece> oldTreePieces = treePieces;
                        int treeTurn{WHITE};
                        int oppoTreeTurn{BLACK};
                        unsigned long int treeTurnNum{1};
                        inputsRead = 0;
                        std::string finputLeaf;
                        std::string finputFake = fSkeleton;
                        std::string finputFakeLine;
                        std::string treeTurnString;
                        int lineSize;

                        newInputs = oldInputs;
                        getDraw(treePieces, WHITE, NO_PIECE, true);

                        while(2*(treeTurnNum-1)+(1-treeTurn)/2 < treeTrunk[treeTrunk.size()-1]) //Catch up to the current point
                        {
                            oldTreePieces = treePieces;
                            start = newInputs[inputsRead];
                            inputsRead++;
                            end = newInputs[inputsRead];
                            inputsRead++;
                            move = start;

                            readCin = false;

                            int i = findPiece(treePieces, start);

                            treeTurnString = std::to_string(treeTurnNum);
                            for(int n = 0; n < 2-treeTurn; n++)
                                treeTurnString.append(".");

                            finputFakeLine.append(treeTurnString);
                            movePiece(treePieces, start, end, treeTurn);
                            finputFakeLine.append(convertNotation(oldTreePieces, treePieces, oldTreePieces[i].getType(), move, start, end, treeTurn));


                            if(treePieces[i].getType() == PAWN && (end[1] == '1' || end[1] == '8'))
                            {
                                promotePawn(treePieces, i, treeTurnNum, emptyTurn, wasOffered, wasClaimed, false, oldInputs, newInputs, inputsRead, finputFake, false);

                                finputFakeLine.append("=");
                                switch (treePieces[i].getType())
                                {
                                case QUEEN:
                                    finputFakeLine.append("Q");
                                    break;
                                case ROOK:
                                    finputFakeLine.append("R");
                                    break;
                                case BISHOP:
                                    finputFakeLine.append("B");
                                    break;
                                case KNIGHT:
                                    finputFakeLine.append("N");
                                    break;
                                
                                default:
                                    break;
                                }
                            }

                            oppoTreeTurn = treeTurn;
                            treeTurn *= -1;

                            int k = findKing(treeTurn);

                            if(checkCanCapture(treePieces, start, treePieces[k].getLocation(), treeTurn) == true)
                                finputFake.append("+");

                            if(treeTurn == WHITE)
                            {
                                finputFake.append(finputFakeLine);
                                finputFake.append("\n");
                            }
                            else if(treeTurn == BLACK)
                            {
                                lineSize = finputFakeLine.size();
                                finputFake.append(finputFakeLine);
                                if(lineSize < 9)
                                    finputFake.append("\t\t");
                                else
                                    finputFake.append("\t");
                            }  

                            getDraw(treePieces, treeTurn, oldTreePieces[i].getType(), false);  
                        }

                        std::string outputB;
                        bool canMove{true};
                        bool valid{true};
                        int next{10};

                        while(move != "branch" && move != "return" && move != "quit" && canMove == true)
                        {
                            readCin = true;
                            outputB.append(printBoard(treePieces, treeTurn, finputFake, 10));
                            outputB.append(printTurn(treeTurnNum, treeTurn, emptyTurn, manualClaim, true, true, valid, true, next, argc));

                            std::cin >> move;
                            std::string buffer = move;
                            tolower(move);

                            if(move == "branch" || move == "return" || move == "quit")
                                break;
                            else
                                move = buffer;

                            std::vector<std::string> route = convertMove(treePieces, move, treeTurn, readCin);

                            start = route[0];
                            end = route[1];


                        }

                        if(move == "return" || canMove == false)
                        {
                            std::string outputR = printBoard(treePieces, treeTurn, finputFake, 10);

                            if(canMove == false)
                            {
                                std::cout << outputR << "End of branch " << pathNum << ". \nEnter return or quit: " << std::endl;
                                std::cin >> move;
                                tolower(move);

                                while(move != "return" && move != "quit")
                                {
                                    std::cout << outputR << "Invalid input. End of branch " << pathNum << ". \nEnter return or quit: " << std::endl;
                                    std::cin >> move;
                                    tolower(move);

                                    if(move == "quit")
                                    {
                                        std::cout << outputR << "\nClose all branches without saving? \nEnter 'y' or 'n': " << std::endl;

                                        std::cin >> move;
                                        tolower(move);

                                        while(move[0] != 'y' && move[0] != 'n')
                                        {
                                            std::cout << outputR << "Invalid input. \nClose all branches without saving? \nEnter 'y' or 'n': " << std::endl;
                                        }
                                    }
                                }
                            }

                            outputR.append("End of branch ");
                            outputR.append(std::to_string(pathNum));
                            outputR.append(". \nWould you like to save this alternate ending? Enter 's' to save, 'd' to discard, \nor 'o' to save this ending and overwrite the original game: ");

                            while(move[0] != 's' && move[0] != 'd' && move[0] != 'o')
                            {
                                std::cin >> move;
                                tolower(move);
                            }

                            //if(move[0] == 's')
                            {

                            }

                        }
                    }
                }*/
                else if(navigation == "quit")
                    break;
                else
                    mode -= 20;
            }
        }

        std::cout << "Game Over. Goodbye! " << std::endl;
    }
    else
        std::cout << "\nGame Over. Goodbye! " << std::endl;

    pauseRun(500);

    return 0;
} //End of main()

//CLASS FUNCTION DEFINITIONS///////////////////////////////
//Piece class ctor
Piece::Piece(char f, char r, int i){
    timesMoved = 0;
    timeSinceMoved = 0;
    isCaptured = false;
    isCastling = false;
    squareColor = DARK;
    
    setTeam(i);
    setType(i);
    setName(type, team, WHITE);
    setFirstNames();
    setLocation(f, r);
    setCanCastle(true);
    setEPVul(true);
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
std::vector<std::string> convertMove(std::vector<Piece> &pieces, std::string move, int turn, bool &readCin) // search here
{
    std::vector<std::string> route;
    std::vector<std::string> badRoute;

    if(move[move.size()-1] == '#' && readCin == false)
        move.pop_back();

    std::string badSpot = "a0";

    badRoute.push_back(badSpot);
    badRoute.push_back(badSpot);

    bool captureMove{false};
    bool promoting{false};

    //Step 1: Determine the piece type, and promote type if necessary
    pieceType searchType;

    switch(move[0])
    {
    case 'R':
        searchType = ROOK;
        break;
    case 'N':
        searchType = KNIGHT;
        break;
    case 'B':
        searchType = BISHOP;
        break;
    case 'Q':
        searchType = QUEEN;
        break;
    case 'K': 
        searchType = KING;
        break;
    case 'O':
        searchType = KING;
        break;
    default:
        searchType = PAWN;
        break;
    }

    if(searchType == PAWN && move[move.size()-2] == '=')
    {
        promoting = true;
        std::cout << move[move.size()-1];

        switch(move[move.size()-1])
        {
        case 'R':
            break;
        case 'N':
            break;
        case 'B':
            break;
        case 'Q':
            break;
        default:
            return badRoute;
            break;
        }
    }
    //Step 2: Determine capture, castle, or regular move
    if(move == "O-O" || move == "O-O-O")
    {
        int k = findKing(turn);
        route.push_back(pieces[k].getLocation());
        std::string target;

        if(move == "O-O")
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
        else if(move == "O-O-O")
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
                    //std::vector<Piece> testPieces = pieces;
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
                else if(promoting == false)
                    return route;
                else if(route[1][1] != '8' && route[1][1] != '1')
                    return badRoute;
                else
                {
                    //if(promoting /*readCin */== true)
                    {
                        //std::cin.putback(move[move.size()-1]);
                        readCin = false;
                    }
                    //else
                    {
                        std::string newValue;
                        newValue.push_back(move[move.size()-1]);
                        route.push_back(newValue);
                    }

                    return route;
                }
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
                else if(promoting == false)
                    return route;
                else if(route[1][1] != '8' && route[1][1] != '1')
                    return badRoute;
                else
                {
                    //if(readCin == true)
                        //std::cin.putback(move[move.size()-1]);
                    //else
                    {
                        readCin = false;
                        std::string newValue;
                        newValue.push_back(move[move.size()-1]);
                        route.push_back(newValue);
                    }
                    return route;
                }
            }
            else if(options == 0)
                return route;
            else
                return badRoute;
        } 
    }
    else if(fileCount == 1 && rankCount == 2) //e.g. R1xf3, N2b5
    {
        if(searchType == PAWN)
            return badRoute;
        
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
//This function returns the performed move in standard chess notation.
std::string convertNotation(std::vector<Piece> oldPieces, std::vector<Piece> pieces, pieceType pMoved, std::string move, std::string start, std::string end, int turn)
{
    if(pMoved == KING && end[0] - start[0] == 2)
        return "O-O";
    else if(pMoved == KING && end[0] - start[0] == -2)
        return "O-O-O";
    else if(move != start)
        return move;

    bool readCin{true};

    std::string input;

    switch (pMoved)
    {
    case ROOK:
        input.append(1, 'R');
        break;
    case KNIGHT:
        input.append(1, 'N');
        break;
    case BISHOP:
        input.append(1, 'B');
        break;
    case QUEEN:
        input.append(1, 'Q');
        break;
    case KING:
        input.append(1, 'K');
        break;
    
    default:
        break;
    }

    input.append(end);

    int newPiece = findPiece(pieces, end);
    int oldPiece = findPiece(oldPieces, end);

    if(pMoved == PAWN && pieces[newPiece].getFile() != oldPieces[newPiece].getFile())
    {
        input.insert(input.begin(), oldPieces[newPiece].getFile());
        input.insert(input.begin()+1, 'x');
    }
    else if(oldPiece != -1)
        input.insert(input.begin()+1, 'x');
    
    std::vector<std::string> test;
    std::vector<std::vector<std::string>> testResults;
    test.push_back(input);
    input.insert(input.begin()+1, start[0]);
    test.push_back(input);
    input.insert(input.begin()+2, start[1]);
    test.push_back(input);
    input.erase(input.begin()+1);
    test.push_back(input);

    int j{0};

    testResults.push_back(convertMove(oldPieces, test[j], pieces[newPiece].getTeam(), readCin));

    while(testResults[0][1] != end || testResults[0][1] == "a0") //Editing here
    {
        j++;
        testResults.pop_back();
        testResults.push_back(convertMove(oldPieces, test[j], pieces[newPiece].getTeam(), readCin));   
    }

    input = test[j];

    return input;
}
//Printers///////////////////////////
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
//This function, along with getCaptureBoxFar, outputs a string that, when printed, displays the captured pieces when called by printBoard
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
//This function returns a string that can be printed to display the board and the pieces at their locations, and optionally, the list of moves
std::string printBoard(std::vector<Piece> &pieces, int turn, std::string finput, int next)
{
    for(int e = 0; e < MAX_SQUARES; e++)
        pieces[e].setName(pieces[e].getType(), pieces[e].getTeam(), turn);

    std::string chessBoard = "\n\n\n";
    std::string fline;
    std::vector<std::string> fpartial;
    int x{0};
    int y{1};

    if(next == 10)
    {
        chessBoard.append(finput);
        chessBoard.append("\n\n\n\n\n\n");
    }

    fpartial.clear();
    x = 0;
    y = 1;

    while(x < finput.size())
    {
        fline.clear();
        do
        { 
            if(finput[x] != '\n')
                fline.append(1, finput[x]);
            x++;
        }while(x < finput.size() && finput[x] != '\n');

        //if(std::count(fpartial.begin(), fpartial.end(), fline))
        fpartial.push_back(fline);
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
            
                if(row != 2)
                    chessBoard.append("\t\t");
                else
                    chessBoard.append("\t");

                if(i == 7 && row == 0)
                    chessBoard.append(fpartial[0]);
                else if(i == 7 && row == 1)
                    continue;
                else if(i > 1 && i < 8)
                {
                    while(fpartial.size()-y > 21)
                        y++;

                    if(y < fpartial.size())
                    {
                        chessBoard.append(fpartial[y]);
                        y++;
                    };
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
                if(row != 2)
                    chessBoard.append("\t\t");
                else
                    chessBoard.append("\t");

                if(i == 2 && row == 0)
                    chessBoard.append(fpartial[0]);
                else if(i == 2 && row == 1)
                    continue;
                else if(i > 1 && i < 8)
                {
                    while(fpartial.size()-y > 21)
                        y++;

                    if(y < fpartial.size())
                    {
                        chessBoard.append(fpartial[y]);
                        y++;
                    }
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
    chessBoard.append("\n");

    return chessBoard;
}

//This function takes an argument BLACK or WHITE and the current turn number and prints a corresponding string.
std::string printTurn(int turnNum, int color, int offerTurn, bool manualClaim, bool wasOffered, bool wasClaimed, bool valid, bool validEntry, int next, int argc) 
{
    std::string turnPhrase{""};
    static int checkBackOffer{0};
    static int checkBackClaim{0};
    static int checkBackTurn{0};

    std::string p1 = "White";
    std::string p2 = "Black";

    std::string offerClause{""};
    std::string claimClause{""};

    turnPhrase.append(std::to_string(turnNum));

    if(color == WHITE)
        turnPhrase.append(". White to move. ");
    else if(color == BLACK)
        turnPhrase.append("... Black to move. ");

    if(validEntry == false && valid == false)
        turnPhrase.append("Invalid entry. Try again.");
    else if(validEntry == true && valid == false)
    {
        //turnPhrase.erase(newOutput.end()-1);
        turnPhrase.append("Invalid move. Try again.");
    }

    int n = turnPhrase.size()-1;

    if(argc == 2)
        claimClause.append(", enter 'adjourn game' to save the game to a file for later"); //"... the game \nto a file..."

    
    if(wasOffered == false)
    {
        offerClause = ", or enter 'offer draw' to offer a draw";
        checkBackOffer = 0;
        checkBackClaim = 0;
        checkBackTurn = color;

        if(manualClaim == true)
            claimClause.append(", or enter 'claim draw' to initiate a draw claim");
    } 
    else if(wasOffered == true)
    {
        if(offerTurn == color || (wasClaimed == true && checkBackClaim == 0))
        {
            if(checkBackOffer == 0 || (checkBackClaim == 0 && wasClaimed == true))
            {
                checkBackOffer++;
                if(wasClaimed == false)
                    turnPhrase.append("Draw offer extended. \nYour opponent may accept at any time before your next move by entering 'accept draw'.");
                else
                {
                    turnPhrase.append("Claim invalid. All draw claims double as draw offers.");
                    checkBackClaim++;
                }
            }
        }
        else if(offerTurn != color || offerTurn != checkBackTurn)
            offerClause = ", or enter 'accept draw' to accept your opponent's draw offer";
        else if(wasClaimed == false && manualClaim == true && offerTurn == color)
        {
            claimClause.append(", or enter 'claim draw' to initiate a draw claim");
        }
    }   

   

    int x = turnPhrase.size()-1;
    turnPhrase.append("\nPlease enter your move, 'undo' to undo or 'I resign' to resign");
    turnPhrase.append(offerClause);
    turnPhrase.append(claimClause);
    turnPhrase.append(": ");

    float max = (turnPhrase.size())/3;
    //do{x++;}while(turnPhrase[x] != '\n');

    while(x+n < turnPhrase.size())
    {
        n++;
        if(x+n >= turnPhrase.size())
            break;
        else if(turnPhrase[x+n] == '\n')
        {
            x += n;
            n = 0;
        }
        else if(turnPhrase[x+n] == ' ' && n >= max)
        {
            x += n;
            n = 0;
            turnPhrase.insert(turnPhrase.begin()+x+1, '\n');
        }
    }

    if(offerTurn == EMPTY)
    {
        turnPhrase = std::to_string(turnNum);
        for(int n = 0; n < 2-color; n++)
            turnPhrase.append(".");

        if(color == WHITE)
        {
            turnPhrase.append(" White to move. \nPlease enter your move or 'undo' to undo: ");
        }
        else if(color == BLACK)
        {
            turnPhrase.append(" Black to move. \nPlease enter your move or 'undo' to undo: ");
        }
    }
    
    while(std::count(turnPhrase.begin(), turnPhrase.end(), '\n') < 3)
        turnPhrase.append("\n");


    if(next != 10)
        turnPhrase.append("Executing multiple moves...");

    return turnPhrase;       
}
//Finders/////////////////////////
//This function returns the index of the Piece object in pieces that exists at a location and where team != EMPTY. If no such piece exists, it returns -1.
int findPiece(std::vector<Piece> pieces, std::string location)
{
    for(int i = 0; i < PIECES_PER_TEAM; i++)
    {
        if(pieces[i].getTeam() != EMPTY && pieces[i].getLocation() == location)
            return i;
    }

    for(int i = MAX_SQUARES - 1; i >= MAX_SQUARES - PIECES_PER_TEAM; i--)
    {
        if(pieces[i].getTeam() != EMPTY && pieces[i].getLocation() == location)
            return i;
    }

    return -1;
}
//This function does the same thing as the one above, but it takes separate rank and file information.
int findPiece(std::vector<Piece> pieces, char f, char r)
{
    std::string location{""};
    location.append(1, f);
    location.append(1, r);

    for(int i = 0; i < PIECES_PER_TEAM; i++)
    {
        if(pieces[i].getTeam() != EMPTY && pieces[i].getLocation() == location)
            return i;
    }

    for(int i = MAX_SQUARES - 1; i >= MAX_SQUARES - PIECES_PER_TEAM; i--)
    {
        if(pieces[i].getTeam() != EMPTY && pieces[i].getLocation() == location)
            return i;
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
int findKing(int color)
{
    if(color == WHITE)
        return 59; //index of the white king
    else if(color == BLACK)
        return 3; //index of the black king

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
                pieces[p2].setLocation(end);
                pieces[blank].setLocation(end[0], start[1]);
            }
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
    int k = findKing(turn);

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
    else
        pieces[i].setCanCastle(false);

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
void promotePawn(std::vector<Piece> &pieces, int i, int turnNum, int &offerTurn, bool &wasOffered, bool &wasClaimed, bool caughtUp, std::vector<std::string> &oldInputs, std::vector<std::string> &newInputs, int &inputsRead, std::string finput, bool needsEntry)
{
    std::string outputTemp;
    outputTemp.append(printBoard(pieces, -1*pieces[i].getTeam(), finput, 10));
    outputTemp.append(printTurn(turnNum, -1*pieces[i].getTeam(), offerTurn, false, wasOffered, wasClaimed, true, true, 10, 1)); //prints the board and pieces
    
    if(caughtUp == true && needsEntry == true)
    {
        std::cout << outputTemp << std::endl;
        pauseRun(1000);
    }
    
    outputTemp.clear();

    outputTemp.append(printBoard(pieces, pieces[i].getTeam(), finput, 10));
    outputTemp.append("PAWN PROMOTION! Please "); 
    if(wasOffered == false)
        outputTemp.append("enter 'offer draw' if you wish to offer a draw, then ");
    outputTemp.append("select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: \n");

    if(caughtUp == true && needsEntry == true)
        std::cout << outputTemp;

    outputTemp.clear();

    if(std::cin.peek() != 10 && caughtUp == true && needsEntry == true)
    {
        //std::cout << std::cin.peek() << std::flush;
        pauseRun(1500);
    }

    char newVal{' '};
    std::string newString;
    int once{0};

    while(newVal == ' ')
    {
        if(once != 0)
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
        else
            once++;

        if(caughtUp == true)
        {
            std::cin >> newString;
            newVal = newString[0];
        }
        else
        {
            newVal = newInputs[inputsRead][0];
            inputsRead++;
        }

        newVal = tolower(newVal);

        if(newVal == 'q')
            pieces[i].changeType(QUEEN);
        else if(newVal == 'r')
            pieces[i].changeType(ROOK);
        else if(newVal == 'b')
            pieces[i].changeType(BISHOP);
        else if(newVal == 'n')
            pieces[i].changeType(KNIGHT);
        else if((newVal == 'o' || newVal == 'c') && wasOffered == false)
        {
            //std::cin.unget();
            std::string myOffer;
            std::cin >> myOffer;

            for(int n = 0; n < myOffer.size(); n++)

            outputTemp.append(printBoard(pieces, pieces[i].getTeam(), finput, 10));

            if(newString == "offer" && myOffer == "draw")
            {
                wasOffered = true;
                outputTemp.append("Draw offer extended. \nPlease enter 'q', 'r', 'b', or 'n': \n");
                once--;
                offerTurn = pieces[i].getTeam();
            }
            else if(myOffer == "claim" && myOffer == "draw")
            {
                wasClaimed = true;
                wasOffered = true;
                outputTemp.append("Draw claim invalid. All draw claims double as draw offers. \nPlease enter 'q', 'r', 'b', or 'n': ");
                once--;
                offerTurn = pieces[i].getTeam();
            }
            else
            {
                outputTemp.append("Invalid input. Please select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: \n");
                newVal = ' ';
            }

            if(newVal != ' ')
            {
                if(caughtUp == true)
                    std::cout << outputTemp << std::flush;
    
                outputTemp.clear();
                newVal = ' ';
            }
            else
                std::cout << outputTemp << std::flush;

        }
        else
        {
            outputTemp.append(printBoard(pieces, pieces[i].getTeam(), finput, 10));
            outputTemp.append("Invalid input. Please select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: \n");
            std::cout << outputTemp << std::flush;
            outputTemp.clear();
            newVal = ' ';
            continue;
        }

        if(caughtUp == true)
        {
            std::string newIn;
            newIn.push_back(newVal);

            oldInputs.push_back(newIn);

            if(wasOffered == true)
            {
                oldInputs.push_back("offer");
                oldInputs.push_back("draw");
            }
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

    if(pieces[i].getCanCastle() == false)
        return -1;
    else if(pieces[i].getFile() == 'g')
    {
        std::string breadcrumb = "f";
        breadcrumb.append(1, start[1]);
        rook = findPiece(pieces, 'h', start[1]);
        
        if(rook == -1)
            return -1;
        else if(pieces[rook].getCanCastle() == false)
            return -1;

        for(char c = 'e'; c < 'h'; c++)
        {
            if(findPiece(pieces, c, pieces[i].getRank()) != -1 && findPiece(pieces, c, pieces[i].getRank()) != i) //All spaces between the king and the rook must be empty
                return -1;
            else if(j0 != -1) //This means there cannot be a piece there to capture
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
        else if(pieces[rook].getCanCastle() == false)
            return -1;

        for(char c = 'e'; c > 'a'; c--)
        {
            if(findPiece(pieces, c, pieces[i].getRank()) != -1 && findPiece(pieces, c, pieces[i].getRank()) != i) //All spaces between the king and the rook must be empty
                return -1;
            else if(j0 != -1) //This means there cannot be a piece there to capture
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
            if(movePiece(pieces, s, e, turn) == false){
                continue;
            }
            else{
                //std::cout << s << " to " << e << " is legal.";
                pieces = oldPieces;
                return 1;
            }
        }       
    }
    //std::cout << "Number of possible moves = " << moves << std::endl; // This piece is useful
    return moves;
}
//When get50move is false, this function compares the current board state to all previous board states and returns an int stating how many times this board state has occured. When get50move is true, this function determines and keeps track of the remaining moves without a pawn move or a capture before the 50-move draw rule is invoked.
int getDraw(std::vector<Piece> &pieces, int turn, pieceType pMoved, bool reset)
{
    std::vector<Piece> oldPieces = pieces;
    static std::vector<std::string> boardStates;
    std::string newState{""};
    static int oldPiecesLeft{MAX_PIECES};
    static int turnsTowardsDraw{100};
    int newPiecesLeft{MAX_PIECES};
    std::vector<int> b;
    b.clear();

    if(reset == true)
    {
        boardStates.clear();
        newState.clear();
        oldPiecesLeft = MAX_PIECES;
        turnsTowardsDraw = 100;
    }

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
            
            //epCheckSpot1[1] += (pieces[j].getTeam());
            std::string epCheckSpot2 = epCheckSpot1;

            epCheckSpot1[0]--;
            epCheckSpot2[0]++;

            //movePiece(pieces, pieces[j].getLocation(), epCheckSpot1, turn);
            int j1 = findPiece(pieces, epCheckSpot1);
            int j2 = findPiece(pieces, epCheckSpot2);
            if(j1 == -1 && j2 == -1)
            {
                newState.append(1, 'o');
                continue;
            }
            else if(j1 != -1)
            {
                if(pieces[j1].getEPVul() == true)
                    newState.append(1, '1');
                else if(j2 != -1){
                    if(pieces[j2].getEPVul() == true)
                        newState.append(1, '1');
                    else
                        newState.append(1, 'o');
                }

                continue;
            }
            else if(j2 != -1)
            {
                if(pieces[j2].getEPVul() == true)
                    newState.append(1, '1');
                else
                    newState.append(1, 'o');
                
                continue;
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

    newPiecesLeft = MAX_SQUARES - std::count (newState.begin(), newState.end(), ' ');

    if(newPiecesLeft < oldPiecesLeft || pMoved == PAWN)
    {
        oldPiecesLeft = newPiecesLeft;
        turnsTowardsDraw = 0;
        boardStates.clear();
        boardStates.push_back(newState);
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
    else if(newPiecesLeft > 4)
        return 0;
    
    if(newPiecesLeft == 2)
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