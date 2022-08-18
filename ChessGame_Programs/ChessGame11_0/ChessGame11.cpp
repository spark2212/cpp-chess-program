// ChessGame11.cpp

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
//Threads
void cinThread(std::string &T, std::string target) {while(T != target) std::cin >> T;}

//Initializers 
std::vector<Piece> initializePieces(); 


//Converters
std::vector<std::string> convertMove(std::vector<Piece> &pieces, std::string move, int turn, bool readCin);
std::string convertNotation(std::vector<Piece> oldPieces, std::vector<Piece> pieces, pieceType pMoved, std::string move, std::string start, std::string end, int turn);
//std::vector<std::string> resumeGame(char** argv);

//Printers
std::string getCaptureBoxFar(std::vector<Piece> &pieces, int turn);
std::string getCaptureBoxNear(std::vector<Piece> &pieces, int turn);
std::string printBoard(std::vector<Piece> &pieces, int turn, bool manualClaim, std::string finput, int next);
std::string printTurn(int turnNum, int color, int offerTurn, bool manualClaim, bool wasOffered, bool wasClaimed, int next, bool valid, int argc);

//Finders
int findPiece(std::vector<Piece> pieces, std::string location);
int findPiece(std::vector<Piece> pieces, char f, char r);
int findEmpty(std::vector<Piece> pieces, std::string location);
int findKing(int color);

//Piece manipulators
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
void removePiece(std::vector<Piece> &pieces, int i);
bool promotePawn(std::vector<Piece> &pieces, int i, int turnNum, int &offerTurn, bool &wasOffered, bool &wasClaimed, bool caughtUp, std::vector<std::string> &oldInputs, std::vector<std::string> &newInputs, int &inputsRead);

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


//MAIN FUNCTION DEFINITION////////////////////////////////
int main(int argc, char **argv)
{
    std::string finput{""};
    std::string finput2{""};
    std::vector<std::string> newInputs;
    bool resume{false};
    std::vector<std::string> newOldInput;
    //bool mode{true};

    for(int num = 0; num < 3; num++)
        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    
    std::cout << "Welcome to ChessGame11.cpp! \n" << std::endl;

    if(argc == 1)
        std::cout << "If you wish to save a record of your game, or use such a record to resume an incomplete game, \nenter 'quit' to quit, then relaunch while including a .txt file in the command line. \nOtherwise enter 'start' to begin the game: ";
    else if(argc == 2)
        std::cout << "If you do not wish to save a record of a game, enter 'quit' to quit, \nthen relaunch without including a .txt file in the command line. \n\nIf you wish to resume an old game, make sure the included .txt file contains \na log of the moves already performed in standard algebraic notation (SAN), \nand enter 'resume', or enter 'start' to begin the game: ";
    else
    {
        std::cout << "Too many arguments passed. Exiting... \n" << std::endl;
        pauseRun(1000);
        return 0;
    }

    std::string choice;

    while(choice != "quit" && choice != "start" && (choice != "resume" || argc != 2))
    {
        std::cin >> choice;

        for(int i = 0; i < choice.size(); i++)
        {
            choice[i] = tolower(choice[i]);
        }

        if(choice == "quit")
        {
            std::cout << "Quitting... \n" << std::endl;
            pauseRun(1000);
            return 0;
        }
        else if(choice == "start")
            break;
        else if(choice == "resume" && argc == 2)
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
    bool decision{true};

    if(choice == "resume")
    {
        std::ifstream oldLog;
        oldLog.open(argv[1]);

        int i{0};
        //int j{0};
        std::string extraHead{""};
        std::string newOldMove;
        char c;
        //bool fTurn{WHITE};
        newTurn = WHITE;
        newTurnNum = 1;
        resume = true;
        caughtUp = false;

        //std::cout << "line 249 reached" << std::endl;

        while(1)
        {
            //std::cout << "loop 1 entered" << std::flush;
            oldLog.get(c);
            extraHead.append(1, c);
            

            if(oldLog.good() == false)
            {
                std::cout << "\nNothing to resume. \n" << argv[1] << " is empty. \nExiting..." << std::endl;
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

        //std::cout << "\nline 265 reached" << std::endl;
        while(oldLog.good() == true)
        {
            /*while (c != ' ' && c != '(')
            {
                newOldMove.append(1, c);
                oldLog.get(c);
            }*/
            //std::cout << "loop 2 entered" << std::flush;
            if(newTurn == WHITE)
                std::getline(oldLog, newOldMove, '\t');
            else if(newTurn == BLACK)
                std::getline(oldLog, newOldMove, '\n');

            //std::getline(oldLog, newOldMove, '\t');

            bool gotOffer{false};
            int extraTail{0};

            if(std::count(newOldMove.begin(), newOldMove.end(), '(') == 1)
            {
                gotOffer = true;
                extraTail += 3;
            }

            if(std::count(newOldMove.begin(), newOldMove.end(), '+') == 1)
                extraTail++;

            if(extraTail != 0)
            {
                for(int n = 0; n < extraTail; n++)
                    newOldMove.pop_back();
            }

            //std::cout << "move " << newTurnNum << " for player " << 1+(1-newTurn)/2 << " is " << newOldMove << std::endl;

            newInputs.push_back(newOldMove);

            if(gotOffer == true)
            {
                newInputs.push_back("offer");
            }

            //newOldInput = convertMove(pieces, newOldMove, newTurn, false);

        /*
            for(int i = 0; i < newOldInput.size(); i++)
            {newInputs.push_back(newOldInput[i]);
            std::cout << newOldInput[i] << std::flush;}

            int p = findPiece(pieces, newInputs[j]);

            movePiece(pieces, newInputs[j], newInputs[j+1], newTurn);
            j += 2;
            if(newInputs.size() > j+1)
            {
                pieceType pType{NO_PIECE};

                switch (newInputs[j][0])
                {
                case 'Q':
                    pType = QUEEN;
                    break;
                case 'R':
                    pType = ROOK;
                    break;
                case 'B':
                    pType = BISHOP;
                    break;
                case 'N':
                    pType = KNIGHT;
                
                default:
                    break;
                }
                j++;
                pieces[p].changeType(pType);
            }

            if(gotOffer == true)
            {
                newInputs.push_back("offer");
                newInputs.push_back("draw");
                j += 2;
            }
        */
        
        
            newTurn *= -1;
            if(newTurn == WHITE)
                newTurnNum++;

            if(oldLog.good() == false)
                break;

            extraHead.clear();
            i = 0;

            while(1)
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

    if(argc == 2)
    {
        std::cout << "\nDo you wish to claim draws by threefold repetition and the fifty-move rule automatically (recommended for casual games) \nor manually (FIDE standard)? \nEnter 'auto' or 'man': ";

        while(choice[0] != 'a' && choice[0] != 'm')
        {
            std::cin >> choice;

            for(int i = 0; i < choice.size(); i++)
                choice[i] = tolower(choice[i]);

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
        
            if(wName == "default")
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

    output1.append(printBoard(pieces, turn, manualClaim, finput, 10));
    output1.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, 10, true, argc));

    std::string magicWord{""};
    //std::string passWord{"skip"};

    std::thread th1(cinThread, std::ref(magicWord), "go");
    th1.detach();

    for(int num = 0; num < 3; num++)
        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

    std::cout << "\nWelcome to ChessGame11.cpp! \n\nTo play, enter your desired move or moves in either chess notation or by entering the start and end squares of the desired move.\n" << std::endl;
    
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

    std::cout << "\nLoading..." << std::flush;
    pauseRun(1750);

    std::cout << output1;

    bool gameOver{false};
    static int inputsRead{0};

    const std::string fSkeleton = finput;
    std::vector<std::string> oldBoards;
    std::vector<std::string> oldInputs;
    int format(2);

    while(gameOver == false)
    {
        std::string output;
        std::string outputA;
        std::string outputCopy;
        std::string move;
        std::string buffer;
        std::vector<std::string> route;
        std::string start;
        std::string end;
        std::string fNewInput;
        bool moveEntered{true};
        bool valid{true};
        static bool isCheck{false};
        static int countMoves{0};
        int next{10};
        int oldNext{10};
        pieceType pMoved{NO_PIECE}; 
        std::vector<Piece> oldPieces = pieces;
        std::string turnStr;
        bool appendDraw;

        fNewInput.clear();
        turnStr = std::to_string(turnNum);

        while(gameOver == false) //Start of inner loop. This loop iterates every attempted move.
        {
            int i{-1}; //This will become the index of the startPiece
            appendDraw = false;

            oldPieces = pieces;
            
            if(caughtUp == false)
            {
                if(resume == false)
                {
                    start = newInputs[inputsRead];
                    inputsRead++;
                    end = newInputs[inputsRead];
                    inputsRead++;
                }
                else if(resume == true)
                {
                    move = newInputs[inputsRead];
                    inputsRead++;

                    if(move == "offer")
                    {
                        start = "offer";
                        end = "draw";
                    }
                    else
                    {
                        route = convertMove(pieces, move, turn, false);
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
                    caughtUp = true;
                    resume = false;
                    outputA.append(printBoard(pieces, turn, manualClaim, finput, next));
                    outputA.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, 10, true, argc));
                    std::cout << outputA << std::flush;

                    outputA.clear();
                    newInputs.clear();

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
                }

                move = start;
            }
            else if(caughtUp == true)
            {
                /*int m{1};
                int t{-1};
                int t2{-1};
            
                for(int n = 0; n < oldInputs.size(); n++) //This for loop lets me keep track of all moves currently stored in oldInputs. It's for dev testing only.
                {
                    

                    t2 *= -1; //-1 1 || -1 -1
                    t *= t2; //-1 1 || 1 -1
                    

                    if((t == -1 && t2 == 1) )
                    {std::cout << std::endl;
                    std::cout << m++ << ". " << std::flush;}
                    
                    std::cout << oldInputs[n] << std::flush;
                    
                    if(t2 == -1)
                        std::cout << " " << std::flush;
                    //if(t == -1 && t2 == -1)
                        
                }*/
                outputA = printBoard(pieces, oppoTurn, false, " ", next);

                std::cin >> move;
                buffer = move;

                for(int h = 0; h < move.size(); h++)
                    buffer[h] = tolower(move[h]);

                if((buffer == "adjourn" || buffer == "pause") && argc == 2)
                {
                    gameOver = true;
                    decision = false;
                    outputA = printBoard(pieces, turn, true, finput, 10);

                    std::cout << outputA << "Game adjourned. This game can be resumed by relaunching ChessGame11.cpp \nwith the same command-line arguments and entering 'resume'. Goodye!" << std::endl;

                    break;

                }
                else if(buffer == "undo")
                {
                    outputA.clear();
                    outputA.append(printBoard(pieces, turn, false, " ", next));
                    outputA.append(turnStr);
                    if(turn == WHITE)
                        outputA.append(". ");
                    else if(turn == BLACK)
                        outputA.append("..." );

                    std::cout << "\n" << outputA << "Please enter the turn number and player to move ('white' or 'black') \nyou would like to return to, or enter 0 to cancel: " << std::endl;

                    std::string alternate;
                    std::string confirm{""};
                        
                    do 
                    {  
                        std::cin >> newTurnNum;

                        if(newTurnNum == 0)
                        {
                            outputA.clear();
                            outputA.append(printBoard(pieces, oppoTurn, false, " ", next));
                            
                            output.clear();
                            output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                            output.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, next, valid, argc));

                            std::cout << output << std::flush;
                            newTurnNum = turnNum;
                            newTurn = turn;

                            break;
                        }
                        
                        while(newTurnNum > turnNum || newTurnNum <= 0)
                        {
                            std::cin.clear();
                            std::cin.ignore(10000, '\n');
                            std::cout << outputA << "\nPlease enter the turn number and player to move ('white' or 'black') \nyou would like to return to, or enter 'cancel' to cancel: " << std::endl;

                            
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
                            std::cout << outputA << "\nPlease enter the player to move ('white' or 'black'), or enter 'cancel' to cancel: " << std::endl;
                        std::string player;
                        std::cin >> player;

                        for(int n = 0; n < player.size(); n++)
                            player[n] = tolower(player[n]);

                        alternate = player;

                        while(player != "white" && player != "black" && player != "cancel")
                        {
                            std::cin.clear();
                            std::cin.ignore(10000, '\n');
                            std::cout << outputA << "\nInvalid input. \nPlease enter the player to move ('white' or 'black'), or enter 'cancel' to cancel: " << std::endl;
                            std::cin >> player;

                            for(int n = 0; n < player.size(); n++)
                                player[n] = tolower(player[n]);

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

                        for(int n = 0; n < confirm.size(); n++)
                            confirm[n] = tolower(confirm[n]);

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
                            for(int n = 0; n < confirm.size(); n++)
                                confirm[n] = tolower(confirm[n]);
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
                            std::cout << "\n" << outputA << "Please enter the turn number and player to move ('white' or 'black') \nyou would like to return to, or enter 0 to cancel: " << std::endl;
                            continue;
                        }
                        
                    }while((newTurnNum > turnNum || (newTurnNum == turnNum && newTurn <= turn)) || confirm[0] == 'n');

                    if(alternate == "cancel")
                    {
                        newTurnNum = turnNum;
                        newTurn = turn;
                        outputA.clear();
                        
                        output.clear();
                        output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                        output.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, next, true, argc));

                        std::cout << output << std::flush;

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
                        outputA.clear();
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

                    for(int j = 0; j < end.size(); j++)
                        end[j] = tolower(end[j]);

                    i = -1;

                    if(offerTurn == oppoTurn && end == "draw")
                    {
                        oldInputs.insert(oldInputs.end()-2, start);
                        oldInputs.insert(oldInputs.end()-2, end);

                        if(start == "offer" || getDrawNum == 0)
                            finput.append("(=)");

                        if(offerTurn == WHITE)
                            format += 3;

                        
                    }

                    next = std::cin.peek();
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
                        for(int j = 0; j < end.size(); j++)
                            end[j] = tolower(end[j]);
                    }
                    else
                    {
                        route = convertMove(pieces, move, turn, true);
                        start = route[0];
                        end = route[1];
                    }

                    valid = movePiece(pieces, start, end, turn); //If the move is valid, performs the move and returns true. Else, returns false.
                }
                else if(moveEntered == true)
                {
                    route = convertMove(pieces, move, turn, true);
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
                output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                if(start == "a0" && end == "a0")
                {
                    output.erase(output.end()-1);
                    output.append("Invalid entry. Try again.\n");
                }
                else
                {
                    output.erase(output.end()-1);
                    output.append("Invalid move. Try again.\n");
                }

                output.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, 10, valid, argc));

                if(caughtUp == true)
                {
                    std::cout << output;
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                }

                output.clear();

                continue;
            }
            else if(moveEntered == true) //This is the append
            {          
                //for(int n = 8; n > 0; n--)
                    //std::cout << "finput[finput.size()-" << n << "] == " << finput[finput.size()-n] << std::flush;
                wasClaimed = false;

                if(turn == WHITE)
                {
                    fNewInput.append("\n");
                    fNewInput.append(turnStr);
                    fNewInput.append(". ");
                    //fNewInput.append("\t");
                }
                else if(turn == BLACK)
                {
                    //std::cout << "format == " << format << std::flush;
                    fNewInput.append("\t");

                    if(format < 9)
                        fNewInput.append("\t");
                    
                    fNewInput.append(turnStr);
                    fNewInput.append("... "); 
                }
                std::string newInput = convertNotation(oldPieces, pieces, pMoved, move, start, end, turn);
                fNewInput.append(newInput);
                finput.append(fNewInput);

                if(resume == false)
                    finput2.append(fNewInput);

                if(turn == WHITE)
                    format = fNewInput.size();

                if(wasOffered == true && offerTurn == turn)
                {
                    if(wasClaimed == true)
                        oldInputs.push_back("claim");
                    else
                        oldInputs.push_back("offer");

                    oldInputs.push_back("draw");
                }
                if(caughtUp == true)
                {
                    oldInputs.push_back(start);
                    oldInputs.push_back(end);
                }
                oldBoards.push_back(printBoard(oldPieces, turn, false, " ", 10));
            }
            else
            {
                if(start == "i" && end == "resign")
                {
                    if(turn == WHITE)
                    {
                        if(argc == 2)
                            finput.append("\nWhite resigns! Black wins! \n\t0 - 1");


                        output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                        output.append("White resigns! Black wins! \n");
                        std::cout << output << std::flush;
                        gameOver = true;
                        
                        break;
                    }
                    else if(turn == BLACK)
                    {
                        if(argc == 2)
                            finput.append("\nBlack resigns! White wins! \n\t1 - 0");
                            
                        output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                        output.append("Black resigns! White wins! \n");
                        std::cout << output << std::flush;
                        gameOver = true;
                        
                        break;
                    }
                }
                else if(start == "offer" && end == "draw" && wasOffered == false)
                {
                    wasOffered = true;
                    output.append(printBoard(pieces, turn, manualClaim, finput, next));
                    output.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, next, moveEntered, argc));
                    std::cout << output << std::flush;
                    output.clear();

                    //oldInputs.push_back(start);
                    //oldInputs.push_back(end);

                    continue;
                }
                else if(start == "offer" && end == "draw" && wasOffered == true)
                {
                    output.append(printBoard(pieces, turn, manualClaim, finput, next));
                    output.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, next, moveEntered, argc));
                    std::cout << output << std::flush;
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    continue;
                }
                else if(start == "accept" && end == "draw" && wasOffered == true)
                {
                    finput.append("\nDraw offered and accepted! \n\n\t1/2 - 1/2");
                    output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                    output.append("\nDraw offered and accepted! \n");
                    std::cout << output;
                    output.clear();
                    gameOver = true;
                    break;
                }
                else if((start == "claim" && end == "draw" && manualClaim == true && wasClaimed == false))
                {
                    wasClaimed = true;
                    wasOffered = true;

                    oldInputs.push_back(start);
                    oldInputs.push_back(end);

                    if(getDrawNum == 3)
                    {
                        finput.append("\n\n\t1/2 - 1/2");
                        output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                        output.append("Threefold repetition rule invoked! It's a draw! \n");
                        std::cout << output;
                        output.clear();
                        gameOver = true;
                        break;
                    }
                    else if(getDrawNum >= 50)
                    {
                        finput.append("\n\n\t1/2 - 1/2");
                        output.append(printBoard(pieces, oppoTurn, manualClaim, finput, 10));
                        output.append("50-move rule invoked! It's a draw! \n");
                        std::cout << output;
                        output.clear();
                        gameOver = true;
                        break;
                    }
                    else
                    {
                        output.append(printBoard(pieces, turn, manualClaim, finput, 10));
                        output.erase(output.end()-1);
                        output.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, 10, moveEntered, argc));
                        std::cout << output << std::flush;
                        output.clear();
                        gameOver = false;
                        continue;
                    }
                } 
            }
            
            if(pieces[i].getType() == PAWN && (end[1] == '1' || end[1] == '8'))
            {    
                appendDraw = promotePawn(pieces, i, turnNum, offerTurn, wasOffered, wasClaimed, caughtUp, oldInputs, newInputs, inputsRead); //promotes the pawn to the desired piece

                if(resume == false)
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
                if(appendDraw == true)
                    offerTurn = turn;

                if(turn == WHITE)
                    format += 2;

                oldPieces = pieces;
            }

            oppoTurn = turn;
            turn *= -1;

            if(next != 10)
                oldNext = next;

            outputA.append(printTurn(turnNum, oppoTurn, offerTurn, manualClaim, wasOffered, wasClaimed, oldNext, valid, argc)); 

            next = std::cin.peek();
            if(next != 10)
                oldNext = next;

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
                    if(countMoves > 0 && wasOffered == true && offerTurn == oppoTurn)
                    {
                        finput.append("(=)");
                        
                        if(offerTurn == WHITE)
                            format += 3;
                    }
                    
                    //if(turn == BLACK && std::count(finput.end()-7, finput.end(), '\n') != 0)
                        //finput.append("\t");
                }

                if(wasOffered == false && next != 10)
                {
                    //wasOffered = false;
                    //offerTurn = oppoTurn;
                }
                    
                wasClaimed = false;

                output = printBoard(pieces, turn, manualClaim, finput, next);
                outputCopy = output;
                output.append("Check! ");
            }
            else if(countMoves > 0 && wasOffered == true && offerTurn == oppoTurn)
            {
                finput.append("(=)");
                if(resume == false)
                    finput2.append("(=)");
                
                if(offerTurn == WHITE)
                    format += 3;

                output = printBoard(pieces, turn, manualClaim, finput, next);
                outputCopy = output;
            }
            else
            {
                if(offerTurn == turn || wasOffered == false)
                {
                    wasOffered = false;
                    if(next != 10)
                        offerTurn = oppoTurn;
                    else
                        offerTurn = turn;
                }

                output = printBoard(pieces, turn, manualClaim, finput, next);
                outputCopy = output;
            }

            output.append(printTurn(turnNum, turn, offerTurn, manualClaim, wasOffered, wasClaimed, next, valid, argc));
            oldNext = next;

            break;

        } //End of inner loop

        if(gameOver == true)
            break;
        else if(caughtUp == true)
        {
            std::cout << outputA << std::flush;
            pauseRun(1000);
        }
        
        if(isCheck == true && countMoves == 0)
        {
            std::cout << outputCopy << "Checkmate! ";
            if(oppoTurn == WHITE)
                std::cout << "White wins! " << std::endl;
            else if(oppoTurn == BLACK)
                std::cout << "Black wins! " << std::endl;

            output.clear();

            if(argc == 2)
            {
                finput.append("#");

                finput.append(" \n\n");
                
                if(oppoTurn == WHITE)
                    finput.append("\t1 - 0");
                else
                    finput.append("\t0 - 1");
            }
            break;
        }
        else if(isCheck == false && countMoves == 0)
        {
            std::cout << outputCopy << "No legal moves! Stalemate! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }

        getDrawNum = getDraw(pieces, turn, pMoved, false);
       

        if(getDrawNum == 3 && (manualClaim == false || wasClaimed == true))
        {
            std::cout << outputCopy << "Threefold repetition rule " << std::flush;
            if(manualClaim == false && resume == true)
            {
                std::cout << "would be applicable. \nTurning off automatic draw claiming... " << std::endl;
                manualClaim = true;
                pauseRun(3000);
                continue;
            }
            else
            {   
                std::cout << "invoked! It's a draw! " << std::endl;
                finput.append("\n\n\t1/2 - 1/2");
                output.clear();
                break;
            }
        }
        else if(getDrawNum == 50 && (manualClaim == false || wasClaimed == true))
        {
            std::cout << outputCopy << "50-move rule invoked" << std::flush;
            if(manualClaim == false && resume == true)
            {
                std::cout << "would be applicable. \nTurning off automatic draw claiming... " << std::endl;
                manualClaim = true;
                pauseRun(3000);
                continue;
            }
            else
            {   
                std::cout << "invoked! It's a draw! " << std::endl;
                finput.append("\n\n\t1/2 - 1/2");
                output.clear();
                break;
            }
        }
        else if(getDrawNum == 5)
        {
            std::cout << outputCopy << "Fivefold repetition rule invoked! Automatic draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }
        else if(getDrawNum == 75)
        {
            std::cout << outputCopy << "75-move rule invoked! Automatic draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }
        else if(getDrawNum == 100)
        {
            std::cout << outputCopy << "Insufficient material left! It's a draw! " << std::endl;
            finput.append("\n\n\t1/2 - 1/2");
            output.clear();
            break;
        }

        if(gameOver == false && caughtUp == true)
        {
            std::cout << output << std::flush;

            if(next != 10)
                pauseRun(1000);

            outputA.clear();
            output.clear();
        }
    } //End of outer loop

    if(argc == 2 && newGame == true)
    {
        std::ofstream gameLog;
        gameLog.open(argv[1], std::ios::trunc);
        gameLog << finput;
        gameLog.close();
    }
    else if(newGame == false)
    {
        std::ofstream gameLog;
        gameLog.open(argv[1], std::ios::app);
        gameLog << finput2;
        gameLog.close();
    }

    if(decision == true)
        std::cout << "Game Over. Goodbye! " << std::endl;

    pauseRun(1500);

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
std::vector<std::string> convertMove(std::vector<Piece> &pieces, std::string move, int turn, bool readCin) // search here
{
    std::vector<std::string> route;
    std::vector<std::string> badRoute;

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
                    if(readCin == true)
                        std::cin.putback(move[move.size()-1]);
                    else
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
                    if(readCin == true)
                        std::cin.putback(move[move.size()-1]);
                    else
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
    //else if(move != start)
    //    return move;

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

    testResults.push_back(convertMove(oldPieces, test[j], pieces[newPiece].getTeam(), true));

    while(testResults[0][1] != end || testResults[0][1] == "a0") //Editing here
    {
        j++;
        testResults.pop_back();
        testResults.push_back(convertMove(oldPieces, test[j], pieces[newPiece].getTeam(), true));   
    }

    input = test[j];

    return input;
}
//This function reads an existing file containing the record of an incomplete game and converts it into a vector of strings
/*std::vector<std::string> resumeGame(char** argv)
{
    //fopen(argv[1], "r");
    std::ifstream oldLog;
    oldLog.open(argv[1]);

    std::vector<std::string

    int i{0};
    std::string extraHead{""};
    char c;

    while(1)
    {
        oldLog.get(c);
        extraHead.append(1, c);

        if(i < 4)
            continue;
        else if(extraHead[i-4] == '\n' && extraHead[i-3] == '1' && extraHead[i-2] == '.' && extraHead[i-1] == ' ')
            break;

        i++;
    }

    do
    {
        
    } while (c != ' ');

    return
    
}*/
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
std::string printBoard(std::vector<Piece> &pieces, int turn, bool manualClaim, std::string finput, int next)
{

    std::string chessBoard = "\n\n\n";

    for(int e = 0; e < MAX_SQUARES; e++)
        pieces[e].setName(pieces[e].getType(), pieces[e].getTeam(), turn);

    if(manualClaim == true && next == 10)
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
    chessBoard.append("\n");

    return chessBoard;
}

//This function takes an argument BLACK or WHITE and the current turn number and prints a corresponding string.
std::string printTurn(int turnNum, int color, int offerTurn, bool manualClaim, bool wasOffered, bool wasClaimed, int next, bool valid, int argc) 
{
    std::string p1 = "White";
    std::string p2 = "Black";

    std::string offerClause{""};
    std::string claimClause{""};

    if(argc == 2)
    {
        claimClause.append(", enter 'adjourn game' to save the game \nto a file for later");
    }
    else
        claimClause.append("\n");

    std::string turnPhrase{""};
    static int checkBack{0};
    static int checkBack2{0};

    if(wasOffered == false)
    {
        //turnPhrase.append("\n");
        offerClause = ", \nenter 'offer draw' to offer a draw";
        checkBack = 0;
        checkBack2 = color;
    }
    //else if(wasClaimed == false)

    //std::cout << "turnNum == " << turnNum << " and turn == " << color << " and offerTurn == " << offerTurn << " and checkBack2 == " << checkBack2;
    

    if(wasOffered == true && offerTurn == color && checkBack == 0)
    {
        checkBack++;
        if(wasClaimed == false)
            turnPhrase.append("Draw offer extended. Your opponent may accept at any time before your next move by entering 'accept draw'.\n");
        else
            turnPhrase.append("Claim invalid. All draw claims double as draw offers.\n");
    }
    else if(wasOffered == true && offerTurn == color && checkBack != 0 && valid == false)
    {
        turnPhrase.append("\nInvalid entry. Try again.\n\n");
    }
    else if(wasOffered == true && (offerTurn != color || checkBack2 != offerTurn))
    {
        offerClause = ", \nor enter 'accept draw' to accept your opponent's draw offer";
    }
    else if(wasClaimed == true)
        turnPhrase.append("\n");
    else if(manualClaim == true && wasClaimed == false && (wasOffered == false || offerTurn == color))
    {
        if(wasOffered == true)
            turnPhrase.append("\n");

        claimClause.append(", or enter 'claim draw' to initiate a draw claim");
    }
    
    if(color == WHITE)
    {
        turnPhrase.append(std::to_string(turnNum));
        turnPhrase.append(". White to move. Please enter your move or 'I resign' to resign");
        turnPhrase.append(offerClause);
        turnPhrase.append(claimClause);
        turnPhrase.append(": ");
    }
    else if(color == BLACK)
    {
        turnPhrase.append(std::to_string(turnNum));
        turnPhrase.append("... Black to move. Please enter your move or 'I resign' to resign");
        turnPhrase.append(offerClause);
        turnPhrase.append(claimClause);
        turnPhrase.append(": ");
    }

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
bool promotePawn(std::vector<Piece> &pieces, int i, int turnNum, int &offerTurn, bool &wasOffered, bool &wasClaimed, bool caughtUp, std::vector<std::string> &oldInputs, std::vector<std::string> &newInputs, int &inputsRead)
{
    std::string outputTemp;
    outputTemp.append(printBoard(pieces, -1*pieces[i].getTeam(), false, " ", 10));
    outputTemp.append(printTurn(turnNum, -1*pieces[i].getTeam(), offerTurn, false, wasOffered, wasClaimed, 10, true, 1)); //prints the board and pieces
    bool appendDraw{false};
    if(caughtUp == true)
    {
        std::cout << outputTemp << std::flush;
        pauseRun(2000);
    }
    
    outputTemp.clear();

    outputTemp.append(printBoard(pieces, pieces[i].getTeam(), false, " ", 10));
    outputTemp.append("PAWN PROMOTION! Please "); 
    if(wasOffered == false)
        outputTemp.append("enter 'offer draw' if you wish to offer a draw, then ");
    outputTemp.append("select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: \n");

    if(caughtUp == true)
        std::cout << outputTemp;

    outputTemp.clear();

    if(std::cin.peek() != 10 && caughtUp == true)
    {
        //std::cout << std::cin.peek() << std::flush;
        pauseRun(2000);
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

            outputTemp.append(printBoard(pieces, pieces[i].getTeam(), false, " ", 10));

            if(newString == "offer" && myOffer == "draw")
            {
                wasOffered = true;
                outputTemp.append("Draw offer extended. \nPlease enter 'q', 'r', 'b', or 'n': \n");
                once--;
                offerTurn = pieces[i].getTeam();
                appendDraw = true;
            }
            else if(myOffer == "claim" && myOffer == "draw")
            {
                wasClaimed = true;
                wasOffered = true;
                outputTemp.append("Draw claim invalid. All draw claims double as draw offers. \nPlease enter 'q', 'r', 'b', or 'n': ");
                once--;
                offerTurn = pieces[i].getTeam();
                appendDraw = true;
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
            outputTemp.append(printBoard(pieces, pieces[i].getTeam(), false, " ", 10));
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
    return appendDraw;
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