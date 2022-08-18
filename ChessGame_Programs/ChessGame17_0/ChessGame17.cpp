//ChessGame17.cpp

#include<iostream> //std::cout, cin, etc
#include<fstream> //std::ifstream, std::ofstream
#include<string> //std::string, string functions
#include<vector> //std::vector, vector operations
#include<math.h> //operators
#include<algorithm> //std::count
#include<chrono>
#include<thread>
//#include<ios>

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

enum modeType
{
    QUIT = 0,
    PLAY = 1,
    REVIEW = 2,
    BRANCH = 3,
    RETURN = 4,
    MAX_MODES = 5
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
std::string printTurn(int turnNum, int color, int* branchPoint, modeType gameMode, int offerTurn, bool manualClaim, bool wasOffered, bool wasClaimed, bool valid, bool validEntry, int next, int argc, int branchLvl);
void formatString(std::string &longString, int i, std::string newppgh);

//Finders
int findPiece(std::vector<Piece> pieces, std::string location);
int findPiece(std::vector<Piece> pieces, char f, char r);
int findEmpty(std::vector<Piece> pieces, std::string location);
int findKing(int color);

//Piece manipulators
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
void removePiece(std::vector<Piece> &pieces, int i);
void promotePawn(std::vector<Piece> &pieces, int i, int viewTurn, int oppoView, int turnNum, int &offerTurn, bool &wasOffered, bool &wasClaimed, bool caughtUp, std::vector<std::string> &oldInputs, std::vector<std::string> &newInputs, int &inputsRead, std::string finput, bool &needsEntry, char &newValEntered, int* branchPoint, modeType gameMode, int branchLvl, int &escpaeNum);

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

inline std::vector<int> convertToMinutes(int seconds){
    int minutes = seconds/60;
    seconds -= (60*minutes);

    std::vector<int> timeLeft;
    timeLeft.push_back(minutes);
    timeLeft.push_back(seconds);
    
    return timeLeft;
}

//Threads
void basicCinThread(std::string &T, bool &done)
{
    std::cin >> T;
    done = true;
    return;
}

void cinThread(std::string &T, std::string target)
{
    while(T != target)
    { 
        std::cin >> T;
        if(T == "quit")
            break;
        else if(T == "yes" && target == "review")
            T = target;
    }
    return;
}

void cinThread2(std::string &T, bool &last, bool &go, bool &skip, int &mode, modeType &gameMode, int branchLvl)
{
    while(1)
    {
        modeType oldGameMode = gameMode;
        while(skip == true || gameMode != REVIEW || mode < -1)
            std::this_thread::yield();
        
        std::cin >> T;

        //std::cout << "In this thread, navigation == " << T << std::flush;
        go = true;
        if(T != "auto" || last == true)
            skip = true;

        if(T == "quit" || (T == "branch" && last == false) || (T == "return" && branchLvl > 0) || (oldGameMode == RETURN && T == "all"))
            break;
    }
    return;
}

//MAIN FUNCTION DEFINITION////////////////////////////////
int main(int argc, char **argv)
{
    std::string choice;
    modeType gameMode{PLAY};
    bool freeStyle{false};
    int branchNum{1};
    int endingNum{0};
    unsigned int branchLvl{0};
    int branchPoint[2] = {1, 1};
    int startPt[2] = {1, 1};
    int branchPointFirst[2] = {1, 1};
    std::vector<int*> oldEndings;
    
    std::vector<unsigned long int> n_remembered;
    std::vector<std::vector<std::string>> oldMovesCatalog;

    std::string newOutput;
    std::string oldOutput;
    std::string outputCopy;

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
    
    std::cout << "Welcome to ChessGame17.cpp! \n" << std::endl;

    if(argc == 1)
        std::cout << "If you wish to save a record of your game, or use such a record to resume an incomplete game, \nenter 'quit' to quit, then relaunch while including a .txt file in the command line. \n\nTo copy or transfer the game data from one file to a new file, or to swap data between two files, \nrelaunch while including two such files. \n\nOtherwise enter 'start' to begin the game: \n";
    else if(argc == 2)
    {
        std::string filename = argv[1];
        if(filename.substr(filename.size()-4, filename.size()-1) != ".txt")
        {
            std::cout << argv[1] << " does not have the correct suffix. Your file must be a .txt file. \nPress enter to exit." << std::endl;
            pauseRun(1000);
            std::cout << "Goodbye! " << std::endl;
            pauseRun(500);
            return 0;
        }
        
        std::cout << "If you do not wish to save a record of a game, enter 'quit' to quit, \nthen relaunch without including a .txt file in the command line. \n\nIf you wish to resume or review an old game, make sure the included .txt file contains \na log of the moves already performed in standard algebraic notation (SAN), \nand enter 'resume' or 'review', respectively. \n\nTo copy or transfer the game data from one file to a new file, or to swap data between two files, relaunch while including two such files.  \nOtherwise, enter 'start' to begin the game: \n";
    }
    else if(argc == 3)
    {
        std::string file1_name, file2_name;
        file1_name = argv[1];
        file2_name = argv[2];

        std::string suffix1 = file1_name.substr(file1_name.size()-4, file1_name.size()-1);
        std::string suffix2 = file2_name.substr(file2_name.size()-4, file2_name.size()-1);

        if(suffix1 != ".txt" || suffix2 != ".txt")
        {
            std::string report1;
            std::string report2;

            if(suffix1 != ".txt")
            {
                std::cout << "File 1 (" << file1_name << ") ";

                if(suffix2 != ".txt")
                    std::cout << "and File 2 (" << file2_name << ") do not have ";
                else
                    std::cout << "does not have ";
            }
            else if(suffix2 != ".txt")
                std::cout << "File 2 (" << file2_name << ") does not have ";

            std::cout << "the correct suffix. Both files must be .txt files. \nPress enter to exit." << std::endl;

            std::cin.clear();
            std::cin.ignore(10000, '\n');

            while(std::cin.get() != '\n');
            
            std::cout << "Exiting..." << std::flush;
            pauseRun(2000);
            
            std::cout << "Goodbye! " << std::endl;
            pauseRun(1000);

            return 0;
        }
        
        std::fstream file1 (argv[1], std::ios::out | std::ios::in);
        std::fstream file2 (argv[2], std::ios::out | std::ios::in);

        file1.seekg(0, file1.end);
        int file1Size = file1.tellg();
        file1.seekg(0, file1.beg);

        if(file1Size == 0)
            file1.setstate(std::ios::eofbit);

        file2.seekp(0, file2.end);
        int file2Size = file2.tellp();
        file2.seekp(0, file2.beg);

        if(file2Size == 0)
            file2.setstate(std::ios::eofbit);

        if(!file1)
        {
            std::cout << "Error! " << file1_name << " is empty. Exiting... " << std::flush;
            pauseRun(2000);

            std::cout << "Goodbye! " << std::endl;
            pauseRun(1000);

            return 0;
        }

        while(1)
        {
            int tries{0};
            while(choice != "copy" && choice != "move" && !(choice == "swap" && file2 && file2Size != 0) && choice != "quit")
            {
                if(tries != 0)
                    std::cout << "Invalid input! " << std::endl;

                std::cout << "To copy and paste the contents of " << file1_name << " onto " << file2_name << ", enter 'copy'. \n\nTo remove the contents of " << file1_name << " and transfer them to " << file2_name << ", enter 'move'. \n";
                
                if(file2 && file2Size != 0)
                    std::cout << "\nTo swap the contents of " << file1_name << " and " << file2_name << ", enter 'swap'. ";

                std::cout << "\nOtherwise, enter 'quit': " << std::endl;
                std::chrono::steady_clock::time_point limit = std::chrono::steady_clock::now();
                bool done{false};
                int minsLeft{5};
                int secsLeft{0};
                std::string myTimer{"5:00       "};
                std::cout << myTimer;
                std::thread timer1(basicCinThread, std::ref(choice), std::ref(done));
                while(done == false && (minsLeft > 0 || secsLeft > 0))
                {
                    if(std::chrono::steady_clock::now() - limit >= std::chrono::milliseconds(1000))
                    {
                        myTimer.clear();
                        if(secsLeft == 0 && minsLeft > 0)
                        {
                            secsLeft = 59;
                            minsLeft--;
                        }
                        else if(secsLeft > 0)
                            secsLeft--;

                        myTimer = std::to_string(minsLeft);
                        myTimer.append(":");
                        if(secsLeft < 10)
                            myTimer.append("0");
                        
                        myTimer.append(std::to_string(secsLeft));
                        myTimer.append("       ");
                        std::cout << "\x1B[s\r" << myTimer << "\x1B[u" << std::flush;
                        //std::cout << "\x1B[s\x1B[1G" << myTimer << "\x1B[u" << std::flush;
                        limit = std::chrono::steady_clock::now();
                    }
                }
                if(done == false)
                {
                    timer1.detach();
                    std::cout << "\nSession timed out. Goodbye!" << std::endl;
                    pauseRun(1000);
                    return 0;
                }
                else
                    timer1.join();

                tolower(choice);

                tries++;

                for(int num = 0; num < 3; num++)
                    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << std::endl;
                
            }

            if(choice == "quit")
            {
                std::cout << "Quitting... \n" << std::endl;
                pauseRun(1000);
                return 0;
            }
            else if(choice == "copy")
                std::cout << "Copy the contents of " << file1_name << " onto " << file2_name << "?\nEnter 'y' to confirm or 'n' to cancel: " << std::flush;
            else if(choice == "move")
                std::cout << "Remove the contents of " << file1_name << " and transfer them to " << file2_name << "? \nEnter 'y' to confirm or 'n' to cancel: " << std::flush;
            else if(choice == "swap")
                std::cout << "Swap the contents of " << file1_name << " and " << file2_name << "? \nEnter 'y' to confirm or 'n' to cancel: " << std::flush;

            std::cin.clear();
            std::cin.ignore(10000, '\n');

            char confirm;
            std::cin >> confirm;

            confirm = tolower(confirm);

            if(confirm == 'n')
            {
                choice = " ";
                for(int num = 0; num < 3; num++)
                    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << std::endl;

                continue;
            }
            else if(confirm != 'y')
            {
                std::cout << "Invalid input. Please enter 'y' or 'n'. " << std::flush;
                continue;
            }
            else if(confirm == 'y' && ((choice == "copy" || choice == "move") && file2 && file2Size != 0))
            {
                std::cout << "WARNING! " << file2_name << " is not empty. All existing data will be overwritten. \nAre you sure you want to " << choice << " data from " << file1_name << "? Enter 'y' or 'n': " << std::flush;

                confirm = ' ';
                
                while(confirm != 'y' && confirm != 'n')
                {
                    confirm = ' ';

                    std::cin.clear();
                    std::cin.ignore(10000, '\n');

                    std::cin >> confirm;
                    confirm = tolower(confirm);

                    if(confirm != 'y' && confirm != 'n')
                    {
                        std::cout << "Invalid input. All data in " << file2_name << " will be overwritten. " << choice << " " << file1_name << " anyway? Enter 'y' or 'n': " << std::flush;
                    }
                }

                if(confirm == 'n')
                {
                    choice = " ";
                    for(int num = 0; num < 3; num++)
                        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << std::endl;

                    continue;
                }
            }

            if(choice == "copy" || choice == "move")
            {
                file2.clear();
                char* bridge = new char[file1Size];
                file1.read(bridge, file1Size);

                file2.close();

                std::ofstream file2plus(argv[2], std::ios::out);

                file2plus << bridge;

                if(choice == "copy")
                {                    
                    file1.seekg(0, file1.beg);
                    std::cout << "Copy operation successful! " << std::flush; 
                        
                    file1.close();    
                }
                else if(file1.gcount() == file1Size && file2plus.tellp() == file1Size)
                {
                    file1.close();
                    std::ofstream file1plus(argv[1], std::ios::out);
                    std::cout << "Move operation successful! " << std::flush;

                    file1plus.close();
                }
                      
                file2plus.close();
                delete[] bridge;
            }
            else if(choice == "swap")
            {
                std::string f1to2, f2to1;

                file1 >> f1to2;
                file2 >> f2to1;

                file1.close();
                file2.close();

                std::ofstream file1plus(argv[1], std::ios::out);
                std::ofstream file2plus(argv[2], std::ios::out);

                file1plus << f2to1;
                file2plus << f1to2;

                file1plus.close();
                file2plus.close();
            }

            pauseRun(1000);

            if(choice == "copy")
                std::cout << "\n" << file1_name << " copied onto " << file2_name << ". " << std::flush;
            else if(choice == "move")
                std::cout << "\nAll " << file1_name << " data transferred to " << file2_name << ". " << std::flush;
            else if(choice == "swap")
                std::cout << "\nAll contents of " << file1_name << " and " << file2_name << " have been swapped. " << std::flush;
    
            pauseRun(1000);
            std::cout << "\nGoodbye! " << std::endl;

            pauseRun(1000);
            return 0;
        }    
    }
    else
    {
        std::cout << "Too many arguments passed. Exiting... \n" << std::endl;
        pauseRun(1000);
        return 0;
    }

    while(choice != "quit" && choice != "start" && ((choice != "resume" && choice != "review") || argc != 2))
    {
        std::chrono::steady_clock::time_point limit = std::chrono::steady_clock::now();
        bool done{false};
        int minsLeft{5};
        int secsLeft{0};
        std::string myTimer{"5:00       "};
        std::cout << myTimer;
        std::thread timer1(basicCinThread, std::ref(choice), std::ref(done));
        while(done == false && (minsLeft > 0 || secsLeft > 0))
        {
            if(std::chrono::steady_clock::now() - limit >= std::chrono::milliseconds(1000))
            {
                myTimer.clear();
                if(secsLeft == 0 && minsLeft > 0)
                {
                    secsLeft = 59;
                    minsLeft--;
                }
                else if(secsLeft > 0)
                    secsLeft--;

                myTimer = std::to_string(minsLeft);
                myTimer.append(":");
                if(secsLeft < 10)
                    myTimer.append("0");
                
                myTimer.append(std::to_string(secsLeft));
                myTimer.append("       ");
                std::cout << "\x1B[s\r" << myTimer << "\x1B[u" << std::flush;
                limit = std::chrono::steady_clock::now();
            }
        }
        if(done == false)
        {
            timer1.detach();
            std::cout << "\nSession timed out. Goodbye!" << std::endl;
            pauseRun(1000);
            return 0;
        }
        else
            timer1.join();

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

            std::cout << "\nPlease enter ";
            if(argc == 2)
                std::cout << "'start', 'quit', 'resume' or 'review': " << std::endl;
            else
                std::cout << "'quit' or 'start': " << std::endl;

            continue;
        } 
    }

    std::vector<Piece> pieces = initializePieces();
    std::vector<Piece> startPieces = pieces;
    //std::string output1;
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
    static bool caughtUpBranch{false};
    bool readCin{false};
    bool decision{true};
    bool extraTab{false};
    bool alert{true};
    bool gameOver{false};
    int escapeNum{0};
    bool flag{false};

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
        //bool reformat{true};
       
        while(1)
        {
            oldLog.get(c);
            if(std::count(extraHead.begin(), extraHead.end(), '\n') <= 1 || c != '\n')
                extraHead.append(1, c);
            else
                continue;

            if(oldLog.good() == false)
            {
                std::cout << "\nNothing to " << choice << ". \n" << argv[1] << " is empty. \nExiting..." << std::endl;
                pauseRun(2000);
                oldLog.close();
                std::cout << "Goodbye! \n" << std::endl;
                return 0;
            }
            
            if(i < 4)
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

        if(finput.find_first_of('\t') != std::string::npos && finput.find_first_of('\t') > 8)
            extraTab = true;

        while(oldLog.good() == true)
        {
            char c{'\0'};
            newOldMove.clear();
            //bool starred{false};
            while(c != ' ' && c != '\t' && c != '\n')
            {
                oldLog.get(c);
                if(oldLog.good() == false)
                    break;
                else
                    newOldMove.append(1, c);
            }

            bool gotOffer{false};
            int extraTail{0};

            if(std::count(newOldMove.begin(), newOldMove.end(), 'O') - std::count(newOldMove.begin(), newOldMove.end(), '-') == -1 && choice == "resume")
            {
                std::cout << "This game is already over. Goodbye! " << std::endl;
                oldLog.close();
                pauseRun(1000);
                return 0;
            }

            if(std::count(newOldMove.begin(), newOldMove.end(), '-') >= 5)
            {
                if(choice == "resume")
                {
                    std::cout << "This game is already over. Goodbye!" << std::endl;
                    oldLog.close();
                    pauseRun(1000);
                    return 0;
                }
                else if(choice == "review")
                {
                    oldLog.setstate(std::ios::eofbit);
                    oldLog.close();
                    break;
                }
                else
                {
                    std::cout << "Error! We shouldn't be here! Exiting..." << std::endl;
                    oldLog.close();
                    pauseRun(1000);
                    return 0;
                }     
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

            if(extraTail != 0 || newOldMove[newOldMove.size()-1] < '!')
            {
                while(newOldMove[newOldMove.size()-1] < '!')
                    newOldMove.pop_back();
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
                else if(extraHead[i-1] != '.' || extraHead[i] != ' ')
                    i++;
                else
                {
                    gotOffer = false;
                    break;
                }
            }

            if(gotOffer == true)
            {
                newInputs.pop_back();
                newInputs.push_back("claim");
                newInputs.push_back("accept");
            }
            else if(oldLog.good() == false)
            {
                int resignFlag = extraHead.find("resigns");
                if(resignFlag != std::string::npos)
                    newInputs.push_back("resign");
            }

            extraTail = 0;
        }
        oldLog.close();
        pieces = startPieces;
    }

    const bool newGame = !resume;
    bool isDefault{false};
    //int extraSize{0};

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
            std::string wName{" "}, bName{" "};
            
            do{
                if(wName.find_first_of('\t', 0) != std::string::npos)
                    std::cout << "Name invalid! ";

                std::cout << "\nEnter White's name (15 characters max, no spaces), \nor enter 'default' (case-sensitive) to begin the game: " << std::flush;
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::getline(std::cin, wName);
            
                if(wName == "default" || wName == "\\\\\\")
                {
                    wName = "White";
                    bName = "Black";
                    isDefault = true;
                }
            }while(wName == "(freestyle)" || wName.find_first_of('\t', 0) != std::string::npos);

            while(wName.size() > 15)
                wName.pop_back();

            finput.append(wName);
            if(wName.size() < 9)
                finput.append("\t");
            else
                extraTab = true;

            finput.append(" vs.   \t");
       
            if(isDefault == false)
            {
                
                do{
                    if(bName.find_first_of('\t', 0) != std::string::npos)
                        std::cout << "Name invalid! ";

                    std::cout << "Enter Black's name (15 characters max), \nor enter 'default' (case-sensitive) to begin the game: " << std::flush;
                    std::getline(std::cin, bName);
                }while(bName == "(freestyle)" || bName.find_first_of('\t', 0) != std::string::npos);
            }

            if(bName == "default")
                bName = "Black";

            finput.append(bName);
            if(freeStyle == true)
                finput.append(" (freestyle)");

            //finput.append("\n\t|\t|");
        }
    }
    else if(choice != "review")
        finput.append("White  \tvs.  \tBlack\n");
    else
        manualClaim = false; //This is redundant, but I wanted to provide clarity.

    if(choice == "start" && isDefault == false)
    {
        char c{' '};

        std::cout << "\nBy default, this program emits an audio alert when a player is in check and to signal the end of the game. \nDo you wish to keep this feature on? Enter 'y' for yes, or 'n' for no: \n";

        while(c == ' ')
        {
            static int loop{0};
            if(loop++ > 0)
                std::cout << "\nInvalid input. Enter 'y' to play with audio feedback or 'n' to turn it off: ";
            if(std::cin.peek() == 'q' || std::cin.peek() == 'Q')
            {
                std::string possibleChoice;
                std::cin >> possibleChoice;
                tolower(possibleChoice);

                if(possibleChoice == "quit")
                {
                    choice = "quit";
                    break;
                }
            }
            std::cin >> c;
            c = tolower(c);

            if(c == 'y')
            {
                alert = true;
                //std::cout << 
            }
            else if(c == 'n')
            {
                alert = false;
            }
            else
                c = ' ';

        }
    }

    int viewMode{ALT};
    int viewTurn{WHITE};
    int oppoView{BLACK};
    int oldViewMode{ALT};

    newOutput.append(printBoard(pieces, viewTurn, finput, 10));
    newOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, true, true, 10, argc, branchLvl));

    if(choice != "review")
    {
        std::string magicWord{""};

        std::thread th1(cinThread, std::ref(magicWord), "go");
        th1.detach();

        for(int num = 0; num < 3; num++)
            std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

        std::cout << "\nWelcome to ChessGame17.cpp! \n\nTo play, enter your desired move or moves in either chess notation or by entering the start and end squares of the desired move.\n" << std::endl;
        
        std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
        while(magicWord != "go" && magicWord != "quit" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(3000));
        
        std::cout << "Multiple commands can be entered at once, in which case each will be performed sequentially.\n" << std::endl;

        t = std::chrono::steady_clock::now();
        while(magicWord != "go" && magicWord != "quit" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(4000));

        std::cout << "To resign, enter 'I resign' in place of your move.\n" << std::endl;

        t = std::chrono::steady_clock::now();
        while(magicWord != "go" && magicWord != "quit" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(3500));

        std::cout << "If you wish to perform additional actions during your move, such as offering " << std::flush;

        if(manualClaim == true)
            std::cout << "or claiming " << std::flush;

        std::cout << "a draw, \nyou must do so before pressing enter to input your move. \n" << std::endl;

        t = std::chrono::steady_clock::now();
        while(magicWord != "go" && magicWord != "quit" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(5500));
        
        std::cout << "If you plan to offer a draw at the end of your turn and you are promoting a pawn, \ninclude the promotion before the draw offer when entering your move, \ne.g. 'exd8=Q offer draw', 'exd8 q offer draw', or 'e7 d8 q offer draw'.\n " << std::endl; 

        t = std::chrono::steady_clock::now();
        while(magicWord != "go" && magicWord != "quit" && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(15000));

        if(magicWord != "go")
        {
            std::cout << "In the future, you may skip these instructions by entering 'go' at any time. Enter 'go' to begin. " << std::flush;    
            while(magicWord != "go" && magicWord != "quit");

            if(magicWord == "quit")
            {
                std::cout << "Quitting..." << std::flush;
                pauseRun(1500);
                std::cout << "Goodbye! " << std::endl;
                pauseRun(500);
                gameMode = QUIT;
            }
        }
        else
        {
            for(int num = 0; num < 3; num++)
                std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

            std::cout << "Welcome back! " << std::flush;
        }
    }

    if(gameMode != QUIT)
    {
        std::cout << "\nLoading..." << std::flush;
        if(choice != "review")
            pauseRun(1750);
        else
            pauseRun(500);

        if(choice != "review" && choice != "resume")
            std::cout << newOutput;
    }

    static int inputsRead{0};
    
    std::vector<std::string> oldBoards;
    std::vector<std::string> oldBoardsFlip;
    std::vector<std::string> oldBoardsWhite;
    std::vector<std::string> oldBoardsBlack;

    bool changeView{false};

    std::vector<std::string> oldInputs;
    const std::string fSkeleton = finput;

    std::string turnStr;
    int format(2);
    
    std::chrono::steady_clock::time_point turnTime = std::chrono::steady_clock::now();

    while(gameMode != QUIT) //When this loop ends, so does the program
    {
        bool undo{false};
        if(branchLvl == 1 && gameMode == BRANCH)
        {
            branchPointFirst[0] = branchPoint[0];
            branchPointFirst[1] = branchPoint[1];
        }
        while(gameOver == false)
        {
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
            int goBack{1};

            switch (viewMode)
            {
            case ALT:
                viewTurn = turn;
                oppoView = oppoTurn;
                oldViewMode = viewMode;
                break;
            case WHITE:
                viewTurn = WHITE;
                oppoView = WHITE;
                oldViewMode = viewMode;
                break;
            case BLACK:
                viewTurn = BLACK;
                oppoView = BLACK;
                oldViewMode = viewMode;
                break;
            case FLIP:
                viewTurn = oppoTurn;
                oppoView = turn;
                viewMode = oldViewMode;
                break;
            
            default:
                break;
            }
            
            if(changeView == false)
            {
                fNewInput.clear();
                turnStr = std::to_string(turnNum);
                turnTime = std::chrono::steady_clock::now();
                wasClaimed = false;

                if(finput[finput.size()-3] == '(' && finput[finput.size()-2] == '=' && finput[finput.size()-1] == ')')
                    oldInputs.push_back("offer");
            }
            else
            {
                newOutput = printBoard(pieces, viewTurn, finput, 10);
                newOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, true, true, 10, argc, branchLvl));
                std::cout << newOutput << std::flush;
            }
            
            changeView = false;
            wasClaimed = false;
            
            if(offerTurn == turn)
                wasOffered = false;

            while(gameOver == false) //Start of inner loop. This loop iterates every attempted move.
            {
                int i{-1}; //This will become the index of the startPiece // breakpoint 1 here
                oldPieces = pieces;
                appendDraw = false;
                readCin = caughtUp;
                bool smallTime = !caughtUp;
                char newValEntered{' '};

                if(gameMode == BRANCH && (turnNum > startPt[0] || (turnNum == startPt[0] && turn <= startPt[1])))
                    caughtUpBranch = true;
                else
                    caughtUpBranch = false;

                if(caughtUp == false) 
                {
                    //inputsRead;
                    //std::cout << "turnNum == " << turnNum << " and newTurnNum == " << newTurnNum << ".\nturn == " << turn << " and newTurn == " << newTurn << "." << std::endl;
                    if(((turnNum == newTurnNum && turn == newTurn && (newGame == true || undo == true || gameMode == BRANCH)) || (inputsRead == newInputs.size())) && gameMode != RETURN)
                    {
                        undo = false;
                        if(gameMode != BRANCH)
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
                        }

                        caughtUp = true;
                        resume = false;
                        oldOutput.clear();
                        oldOutput.append(printBoard(pieces, viewTurn, finput, next));
                        oldOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, true, true, 10, argc, branchLvl));
                        std::cout << oldOutput << std::flush;

                        oldOutput.clear();
                        
                        newInputs.clear();

                        inputsRead = 0;

                        if(gameMode == BRANCH)
                            choice = "start";
                        
                        continue;
                    }
                    else
                        newOutput.clear();
                    
                    if(newInputs[inputsRead] == "resign")
                    {
                        start = "i";
                        end = "resign";
                        inputsRead++;
                    }
                    else if(newInputs[inputsRead] == "accept" && wasOffered == true)
                    {
                        start = "accept";
                        end = "draw";
                        inputsRead++;
                    }
                    
                    move = newInputs[inputsRead++];
                    while(move[move.size()-1] < '!')
                        move.pop_back();

                    if(move == "offer")
                    {
                        wasOffered = true;
                        offerTurn = oppoTurn;
                        finput.append("(=)");
                        format += 3;
                        continue;
                    }
                    else if(move == "claim")
                    {
                        //start = "claim";
                        //end = "draw";

                        wasClaimed = true;
                        wasOffered = true;
                        offerTurn = oppoTurn;
                        caughtUp = true;
                        resume = false;
                        finput.append("(=)");
                        format += 3;
                        break;
                    }
                    else
                    {
                        wasOffered = false;
                        wasClaimed = false;

                        route = convertMove(pieces, move, turn, readCin);
                        start = route[0];
                        end = route[1];

                        if(route.size() == 3)
                        {
                            //std::cout << "move == " << move << std::flush;
                            newValEntered = route[2][0];
                        }
                    }        

                    if(start != "accept" && end == "draw" && wasOffered == true)
                    {
                        moveEntered = false;
                        valid = false;
                    }

                    if(end == "draw" || end == "resign")
                    {
                        moveEntered = false;
                        valid = false;
                        if(end == "draw")
                            oldInputs.push_back(start);
                        else
                            oldInputs.push_back(end);
                    }
                    else //The saved input is an actual move.
                    {
                        moveEntered = true;
                        i = findPiece(pieces, start);

                        if(i != -1)
                            pMoved = pieces[i].getType(); //This will be used later but needs to be initialized now.
                        
                        for(int e = 0; e < MAX_SQUARES; e++)
                            pieces[e].setIsCastling(false);

                        valid = movePiece(pieces, start, end, turn); //If the move is valid, performs the move and returns true. Else, returns false.

                        if(valid == false)
                        {
                            if((choice == "review" || choice == "resume") && gameMode == PLAY)
                                std::cout << printBoard(pieces, viewTurn, finput, 10) << std::flush;

                            std::cout << "Error! start == " << start << ", end == " << end << ", and move == " << move << "is an invalid move at " << turnNum;

                            if(turn == WHITE)
                                std::cout << ", white." << std::endl;
                            else
                                std::cout << ", black." << std::endl;

                            //std::cout << "move == " << move << ", start == " << start << " and end == " << end << std::endl;

                            newTurnNum = turnNum;
                            newTurn = turn;
                            //caughtUp = true;
                            return 0;
                        }           
                    }
                    move = start;
                }
                else if(caughtUp == true)
                {
                    bool done{false};
                    std::thread threadThread(basicCinThread, std::ref(move), std::ref(done));
                    while(done == false && flag == false);
                    if(flag == true)
                    {
                        threadThread.detach();
                    }
                    else
                        threadThread.join();
 
                    //std::thread thMove(cinMoveThread, std::ref(move), std::ref(flag)); //breakpoint 2 here
                    //thMove.join();
                    
                    buffer = move;
                    //std::cout << "move == " << move << std::flush;

                    if(std::chrono::steady_clock::now() - turnTime > std::chrono::milliseconds(100))
                    {
                        smallTime = false;
                        next = 10;
                        if(wasOffered == false)
                            offerTurn = turn;
                    }
                    else if(std::chrono::steady_clock::now() - turnTime <= std::chrono::milliseconds(100))
                    {
                        smallTime = true; 
                        if(next == 10 && (viewMode == WHITE || viewMode == BLACK))
                            std::cout << oldOutput;
                        next = 0;
                        std::cout << "Executing multiple moves... " << std::flush;
                        pauseRun(1000);
                    }
                    oldOutput = printBoard(pieces, oppoView, finput, next);
                    
                    tolower(buffer);
                    
                    if(buffer == "return" && branchLvl > 0)
                    {
                        gameMode = RETURN;
                        choice = "review";
                        oldInputs.clear();
                        newInputs.clear();
                        branchLvl--;
                        newInputs = oldMovesCatalog[branchLvl];
                        finput.clear();
                        finput = fSkeleton;
                        finput2.clear();
                        pieces = startPieces;
                        getDraw(pieces, WHITE, NO_PIECE, true);
                        turnNum = 0;
                        turn = WHITE;
                        oppoTurn = BLACK;
                        offerTurn = WHITE;
                        wasOffered = false;
                        wasClaimed = false;
                        branchPoint[0] = oldEndings[branchLvl][0];
                        branchPoint[1] = oldEndings[branchLvl][1];
                        caughtUp = false;
                        inputsRead = 0;
                        oldBoardsBlack.clear();
                        oldBoards.clear();
                        oldBoardsWhite.clear();
                        oldBoardsFlip.clear();

                        //for(int i = 0; i < oldMovesCatalog[0].size(); i++)
                            //std::cout << "oldMovesCatalog[0][" << i << "] == " << oldMovesCatalog[0][i];

                        oldMovesCatalog.pop_back();
                        oldEndings.pop_back();

                        //for(int i = 0; i < newInputs.size(); i++)
                            //std::cout << " and newInputs[" << i << "] == " << newInputs[i] << std::endl;

                        break;
                        
                    }
                    else if(buffer == "change")
                    {
                        std::string tempOutput = printBoard(pieces, viewTurn, finput, 10);
                        std::string changeRules = "Enter 'w' to view all future moves from White's perspective, \nenter 'b' to view all future moves from Black's perspective, \nenter 'a' to view the board from the perspective of the player to move at all times, \nenter 'flip' to flip the current perspective for one turn, or enter 'c' to cancel: \n";

                        std::cout << tempOutput << changeRules << std::flush;
                        
                        std::string newView;
                        std::cin >> newView;
                        tolower(newView);
                        tempOutput.append("Invalid input. ");

                        while(newView != "w" && newView != "b" && newView != "a" && newView != "flip" && newView != "c")
                        {
                            std::cout << tempOutput << changeRules << std::flush;

                            std::cin.clear();
                            std::cin.ignore(10000, '\n');

                            std::cin >> newView;
                            tolower(newView);
                        }

                        switch (newView[0])
                        {
                        case 'w':
                            viewMode = WHITE;
                            break;
                        case 'b':
                            viewMode = BLACK;
                            break;
                        case 'a':
                            viewMode = ALT;
                            break;
                        case 'f':
                            viewMode = FLIP;
                            break;
                        
                        default:
                            break;
                        }

                        changeView = true;
                        break;

                    }
                    else if(buffer == "adjourn" && gameMode == PLAY && argc == 2) 
                    {
                        gameOver = true;
                        decision = false;
                        oldOutput = printBoard(pieces, viewTurn, finput, 10);

                        std::cout << oldOutput << "Game adjourned. This game can be resumed by relaunching ChessGame17.cpp \nwith the same command-line arguments and entering 'resume'. Goodbye!" << std::endl;

                        break;
                    }
                    else if(buffer == "undo" && (turnNum > branchPoint[0] || (turnNum == branchPoint[0] && turn < branchPoint[1])))
                    {
                        oldBoardsBlack.push_back(printBoard(pieces, BLACK, finput, 10));
                        oldBoards.push_back(printBoard(pieces, turn, finput, 10));
                        oldBoardsWhite.push_back(printBoard(pieces, WHITE, finput, 10));
                        oldBoardsFlip.push_back(printBoard(pieces, oppoTurn, finput, 10));

                        std::vector<std::vector<std::string>> oldBoardsSet;
                        oldBoardsSet.push_back(oldBoardsBlack);
                        oldBoardsSet.push_back(oldBoards);
                        oldBoardsSet.push_back(oldBoardsWhite);
                        oldBoardsSet.push_back(oldBoardsFlip);

                        oldBoardsBlack.pop_back();
                        oldBoards.pop_back();
                        oldBoardsWhite.pop_back();
                        oldBoardsFlip.pop_back();

                        oldOutput.clear();
                        oldOutput.append(oldBoardsSet[viewMode+1][oldBoardsSet.size()-1]);
                        oldOutput.append(turnStr);
                        if(turn == WHITE)
                            oldOutput.append(". ");
                        else if(turn == BLACK)
                            oldOutput.append("..." );

                        std::cout << "\n" << oldOutput << "Please enter the turn number and player to move ('white' or 'black') \nyou would like to return to, 'b' to begin browsing through the \nprevious board positions, or enter 'c' to cancel: \n" << std::endl;

                        std::string alternate;
                        std::string confirm{""};
                        int rep{0};
                        char c;
                            
                        do 
                        {  
                            if(rep == 0)
                            {
                                std::cin >> c;
                                c = tolower(c);
                            }
                            
                            if(c == 'b' || c == 'c')
                            {
                                newTurnNum = turnNum;
                                newTurn = turn;

                                if(c == 'c')
                                    alternate = "cancel";
                            }
                            else
                            {
                                if(c >= '0' && c <= '9')
                                {
                                    std::cin.unget();
                                    std::cin >> newTurnNum;
                                }
                                else
                                    newTurnNum = 0;
                                    
                                c = ' ';
                            }
                            
                            if(viewMode == ALT || viewMode == FLIP)
                                oppoView = -1*viewTurn;
                            else
                                oppoView = viewTurn;
                            
                            while(((newTurnNum > turnNum || newTurnNum <= 0) && alternate != "cancel" && newTurnNum < branchPoint[0]) || rep == 1)
                            {
                                if(newTurnNum < branchPoint[0] && gameMode == BRANCH)
                                    rep = 1;
                                std::cin.clear();
                                std::cin.ignore(10000, '\n');
                                std::cout << oldOutput << " Invalid input. ";
                                if(rep == 1)
                                {
                                    std::cout << "Cannot undo before turn " << branchPoint[0] << ", ";
                                    if(branchPoint[1] == WHITE)
                                        std::cout << "white ";
                                    else
                                        std::cout << "black ";
                                    
                                    std::cout << "on this branch. ";
                                }
                                std::cout << "\nPlease enter the turn number and player to move ('white' or 'black') \nyou would like to return to, 'b' to begin browsing through the \nprevious board positions, or enter 'c' to cancel: \n" << std::flush;
                                
                                rep = 0;
                                std::cin >> c;
                                c = tolower(c);

                                if(c == 'b' || c == 'c')
                                {
                                    newTurnNum = turnNum;
                                    newTurn = turn;

                                    if(c == 'c')
                                        alternate = "cancel";
                                }
                                else 
                                {
                                    c = ' ';
                                    std::cin.unget();
                                    std::cin >> newTurnNum;
                                }
                            }

                            if(std::cin.peek() == 10 && c != 'b' && c != 'c')
                                std::cout << oldOutput << "\nPlease enter the player to move ('white' or 'black'), \nor enter 'c' to cancel: \n" << std::endl;
                                
                            std::string player;
                            if(c != 'b' && c != 'c')
                            {
                                std::cin >> player;
                                tolower(player);
                                alternate = player;
                            }

                            while(player != "white" && player != "black" && alternate != "cancel" && c != 'b')
                            {
                                std::cin.clear();
                                std::cin.ignore(10000, '\n');
                                std::cout << oldOutput << "\nInvalid input. \nPlease enter the player to move ('white' or 'black'), or enter 'c' to cancel: \n" << std::endl;
                                std::cin >> player;
                                tolower(player);
                                alternate = player;
                            }
                            
                            if(alternate == "cancel")
                                break;
                            else if(player == "white")
                                newTurn = WHITE;
                            else if(player == "black")
                                newTurn = BLACK;

                            int turn_i = (newTurnNum-1)*2;
                            if(newTurn == BLACK)
                                turn_i++;

                            const int turn_0 = (2*(branchPoint[0]-1))+(1-branchPoint[1])/2;

                            if(turn_i < turn_0)
                            {
                                confirm = "no";
                                rep = 1;
                                continue;
                            }

                            std::cout << oldBoardsSet[viewMode+1][turn_i] << newTurnNum << std::flush;
                            if(newTurn == WHITE)
                                std::cout << ". " << std::flush;
                            else if(newTurn == BLACK)
                                std::cout << "... " << std::flush;
                                    
                            std::cout << "Return to this position? \nEnter 'y' to confirm, 'n' to choose a different position, \n";
                                    
                            if(turn_i > turn_0 && turn_i < oldBoards.size())
                                std::cout << "'<' or '>' to view the previous/next position, ";
                            else if(turn_i == turn_0)
                                std::cout << "'>' to view the next position, ";
                            else if(turn_i == oldBoards.size())
                                std::cout << "'<' to view the previous position, ";
                                
                            std::cout << "'flip' to flip the board, \n't' to toggle automatic board rotation or enter 'c' to cancel: \n" << std::flush;

                            std::cin >> confirm;
                            tolower(confirm);
                            bool freeze{false};
                            int oldViewMode2;

                            if(confirm == "c")
                                confirm = "cancel";

                            if(viewMode == WHITE || viewMode == BLACK)
                                freeze = true;

                            while(confirm[0] != 'y' && confirm[0] != 'n' && confirm != "cancel")
                            {
                                bool adjusted{false};
                                if(freeze == false)
                                    oldViewMode2 = viewMode;

                                if((confirm == "<" && turn_i > turn_0) || (confirm == ">" && turn_i < oldBoards.size()) || confirm == "flip" || confirm == "t")
                                {
                                    if(confirm == "flip" && (viewMode == FLIP || viewMode == ALT))
                                        viewMode = 2-viewMode;
                                    else if(confirm == "flip")
                                        viewMode *= -1;
                                    else if(confirm == "t" && freeze == true)
                                    {
                                        if(viewMode == FLIP)
                                            viewMode = -1*newTurn;
                                        else
                                            viewMode = newTurn;
                                    }
                                    else if(confirm == "<")
                                    {
                                        turn_i--;
                                        newTurn *= -1;
                                        if(newTurn == BLACK)
                                            newTurnNum--;
                                    }
                                    else if(confirm == ">")
                                    {
                                        turn_i++;
                                        newTurn *= -1;
                                        if(newTurn == WHITE)
                                            newTurnNum++;
                                    }
                                    adjusted = true;
                                }

                                std::cout << oldBoardsSet[viewMode+1][turn_i];

                                if(newTurn == WHITE)
                                    std::cout << newTurnNum << ". ";
                                else if(newTurn == BLACK)
                                    std::cout << newTurnNum << "... ";
                                    
                                if(adjusted == false)         
                                {
                                    std::cout << "Invalid input. Please enter 'y', 'n', "; 

                                    if(turn_i > 0 && turn_i < oldBoards.size()-1)
                                        std::cout << "'<', '>', ";
                                    else if(turn_i == 0)
                                        std::cout << "'>', ";
                                    else if(turn_i == oldBoards.size()-1)
                                        std::cout << "'<', ";
                                        
                                    std::cout << "\n'flip', 't' (for toggle), or 'c' to cancel.\nReturn to this position? \n" << std::endl;
                                }
                                else
                                {
                                    std::cout << "Return to this position? \nEnter 'y' to confirm, 'n' to choose a different position, \n";
                                    
                                    if(turn_i > turn_0 && turn_i < oldBoards.size())
                                        std::cout << "'<' or '>' to view the previous/next position, ";
                                    else if(turn_i == turn_0)
                                        std::cout << "'>' to view the next position, ";
                                    else if(turn_i == oldBoards.size())
                                        std::cout << "'<' to view the previous position, ";
                                        
                                    std::cout << "'flip' to flip the board, \n't' to toggle automatic board rotation or enter 'c' to cancel: \n" << std::flush;
                                }

                                std::cin.clear();
                                std::cin.ignore(10000, '\n');

                                std::cin >> confirm;
                                tolower(confirm);

                                if((confirm[0] == 'y' && turn_i == oldBoards.size()) || confirm == "c")
                                    confirm = "cancel";
                                else if(confirm == "t")
                                    freeze = !freeze;

                                if(freeze == false || confirm[0] == 'y' || confirm[0] == 'n' || confirm == "cancel")
                                    viewMode = oldViewMode2;
                            }

                            viewMode = oldViewMode;

                            if(confirm == "cancel"){
                                alternate = "cancel";
                                break;
                            }

                            if(confirm[0] == 'n')
                            {
                                newTurn = turn;
                                newTurnNum = turnNum;
                                std::cout << "\n" << oldOutput << "\nPlease enter the turn number and player to move ('white' or 'black') \nyou would like to return to, 'b' to begin browsing through the \nprevious board positions, or enter 'c' to cancel: \n" << std::flush;
                                continue;
                            }                        
                        }while((newTurnNum > turnNum || (newTurnNum == turnNum && newTurn <= turn)) || confirm[0] == 'n');

                        if(alternate == "cancel")
                        {
                            newTurnNum = turnNum;
                            newTurn = turn;
                            oldOutput.clear();
                            
                            newOutput.clear();
                            newOutput.append(printBoard(pieces, viewTurn, finput, 10));
                            newOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, true, false, 10, argc, branchLvl));

                            std::cout << newOutput << std::flush;

                            continue;
                        }

                        if(alternate != "cancel")
                        {                    
                            undo = true;
                            caughtUp = false;
                            pieces = startPieces;
                            getDrawNum = getDraw(pieces, WHITE, NO_PIECE, true);
                            finput.clear();
                            finput2.clear();
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
                        
                        pMoved = pieces[i].getType();
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

                if(valid == false && moveEntered == true) //Why does the branch get stuck HERE?
                {
                    //newOutput.clear();
                    newOutput.append(printBoard(pieces, viewTurn, finput, 10));
                    bool validEntry{true};
                    if(start == "a0" && end == "a0")
                        validEntry = false;

                    newOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, false, validEntry, 10, argc, branchLvl));

                    if(caughtUp == true && choice != "review")
                    {
                        std::cout << newOutput << std::flush;
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                    }

                    newOutput.clear();

                    continue;
                }
                else if(valid == true && moveEntered == true)
                {          
                    if(viewMode == FLIP)
                        viewTurn *= -1;

                    oldBoards.push_back(printBoard(oldPieces, turn, finput, 10));
                    oldBoardsFlip.push_back(printBoard(oldPieces, oppoTurn, finput, 10));
                    oldBoardsWhite.push_back(printBoard(oldPieces, WHITE, finput, 10));
                    oldBoardsBlack.push_back(printBoard(oldPieces, BLACK, finput, 10));

                    if(branchLvl == 1 && turnNum == branchPoint[0] && turn == branchPoint[1])
                    {
                        for(int i = 0; i <= goBack; i++)
                            finput.pop_back();
                    }

                    if(turn == WHITE)
                    {
                        fNewInput.append("\n");
                        fNewInput.append(turnStr);
                        fNewInput.append(". ");
                    }
                    else if(turn == BLACK)
                    {
                        if(extraTab == true)
                            fNewInput.append("\t       ");
                            
                        fNewInput.append("\t");

                        if(format < 9)
                            fNewInput.append("\t");
                        
                        fNewInput.append(turnStr);
                        fNewInput.append("... "); 
                    }

                    newInput = convertNotation(oldPieces, pieces, pMoved, move, start, end, turn);
                    fNewInput.append(newInput);
                    finput.append(fNewInput);

                    if(caughtUpBranch == true)
                    {
                        if(wasOffered == true && offerTurn == turn && (turnNum > branchPoint[0] || turn < branchPoint[1]))   
                            finput2.append("(=)");
                        
                        if(turnNum == branchPoint[0] && turn == branchPoint[1] && branchPoint[1] == BLACK)
                            finput2.append("\t\t");
                        finput2.append(fNewInput);
                    }  

                    if(turn == WHITE)
                        format = fNewInput.size();
                    if(branchLvl == 1 && turnNum == branchPoint[0] && turn == branchPoint[1] && turn == WHITE)
                        format++;

                    /*if(caughtUp == true)
                    {
                        oldInputs.push_back(start);
                        oldInputs.push_back(end);
                    }*/
                }
                else //moveEntered == false
                {
                    if(start == "i" && end == "resign")
                    {
                        oldInputs.push_back("resign");
                        if(turn == WHITE)
                        {
                            finput.append("\nWhite resigns! \t0 - 1");
                            if(caughtUpBranch == true)
                                finput2.append("\nWhite resigns!");

                            newOutput.clear();
                            newOutput.append(printBoard(pieces, -1*viewTurn, finput, 10));
                            newOutput.append("White resigns! Black wins! \n");
                            outputCopy = newOutput;
                            std::cout << newOutput << std::flush;
                            gameOver = true;
                            
                            break;
                        }
                        else if(turn == BLACK)
                        {
                            finput.append("\nBlack resigns! \t1 - 0");
                            if(caughtUpBranch == true)
                                finput2.append("\nBlack resigns! \t1 - 0");

                            newOutput.clear();
                            newOutput.append(printBoard(pieces, -1*viewTurn, finput, 10));
                            newOutput.append("Black resigns! White wins! \n");
                            outputCopy = newOutput;
                            std::cout << newOutput << std::flush;
                            gameOver = true;
                            
                            break;
                        }
                    }
                    else if(start == "offer" && end == "draw" && wasOffered == false)
                    {
                        wasOffered = true;
                        if(smallTime == false)
                        {
                            next = 10;
                            offerTurn = turn;
                        }
                        else   
                            offerTurn = oppoTurn;

                        newOutput.append(printBoard(pieces, viewTurn, finput, next));
                        newOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, true, true, 10, argc, branchLvl));
                        if(caughtUp == true)
                            std::cout << newOutput << std::flush;
                        newOutput.clear();

                        continue;
                    }
                    else if((start == "offer" || (start == "claim" && wasClaimed == true)) && end == "draw" && wasOffered == true)
                    {
                        newOutput.append(printBoard(pieces, viewTurn, finput, next));
                        newOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, false, false, 10, argc, branchLvl));
                        
                        if(caughtUp == true)
                        {
                            std::cout << newOutput << std::flush;
                            std::cin.clear();
                            std::cin.ignore(10000, '\n');
                        }
                        continue;
                    }
                    else if(start == "accept" && end == "draw" && wasOffered == true)
                    {
                        oldInputs.push_back("accept");
                        finput.append("\nDraw offered and accepted! \n\n\t1/2 - 1/2");
                        if(caughtUpBranch == true)
                            finput2.append("\nDraw offered and accepted! \n\n\t1/2 - 1/2");

                        newOutput.append(printBoard(pieces, viewTurn, finput, 10));
                        newOutput.append("\nDraw offered and accepted! \n");
                        outputCopy = newOutput;
                        std::cout << newOutput << std::flush;
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
                            if(smallTime == false)
                            {
                                next = 10;
                                offerTurn = turn;
                            }
                            else
                                offerTurn = oppoTurn;
                            
                            if(getDrawNum > 0)
                                oldInputs.push_back("claim");
                            //oldInputs.push_back(end);
                        }

                        if(getDrawNum == 3)
                        {  
                            finput.append("(=)\n\n\t1/2 - 1/2");
                            if(caughtUpBranch == true)
                                finput2.append("(=)\n\n\t1/2 - 1/2");

                            newOutput.append(printBoard(pieces, -1*viewTurn, finput, 10));
                            newOutput.append("Threefold repetition rule invoked! It's a draw! \n");
                            outputCopy = newOutput;
                            std::cout << newOutput << std::flush;
                            newOutput.clear();
                            gameOver = true;
                            break;
                        }
                        else if(getDrawNum >= 50)
                        {
                            finput.append("(=)\n\n\t1/2 - 1/2");
                            if(caughtUpBranch == true)
                                finput2.append("(=)\n\n\t1/2 - 1/2");

                            newOutput.append(printBoard(pieces, -1*viewTurn, finput, 10));
                            newOutput.append("50-move rule invoked! It's a draw! \n");
                            outputCopy = newOutput;
                            std::cout << newOutput << std::endl;
                            newOutput.clear();
                            gameOver = true;
                            break;
                        }
                        else //claim entered and invalid
                        {
                            newOutput.append(printBoard(pieces, viewTurn, finput, 10));
                            newOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, true, false, 10, argc, branchLvl));
    
                            std::cout << newOutput << std::flush;
                            if(offerTurn == oppoTurn)
                                pauseRun(1000);
                            
                            newOutput.clear();
                            gameOver = false;
                            continue;
                        }
                    } 
                }

                oldInputs.push_back(newInput);
                
                if(pieces[i].getType() == PAWN && (end[1] == '1' || end[1] == '8'))
                {    
                    bool redundant = wasOffered;
                    if(std::count(move.begin(), move.end(), '=') == 1)
                    {
                        newValEntered = move[move.size()-1];
                        readCin = false;
                    }

                    promotePawn(pieces, i, viewTurn, oppoView, turnNum, offerTurn, wasOffered, wasClaimed, caughtUp, oldInputs, newInputs, inputsRead, finput, readCin, newValEntered, branchPoint, gameMode, branchLvl, escapeNum);

                    if(escapeNum >= 10)
                    {
                        std::cout << "Error! Infinite while loop. escapeNum == " << escapeNum << ". \nProgram aborted. Goodbye! " << std::endl;
                        pauseRun(1000);
                        return 0;
                    }

                    readCin = caughtUp;

                    if(offerTurn == turn)
                        appendDraw = !redundant;

                    if(finput.find_last_of('=') != finput.size()-2)
                    {
                        std::cout << "append it here!" << std::flush;
                        finput.append("=");
                        
                        pieceType t = pieces[i].getType();
                        switch (t)
                        {
                        case QUEEN:
                            finput.append("Q");
                            oldInputs[oldInputs.size()-1].append("=Q");
                            break;
                        case ROOK:
                            finput.append("R");
                            oldInputs[oldInputs.size()-1].append("=R");
                            break;
                        case BISHOP:
                            finput.append("B");
                            oldInputs[oldInputs.size()-1].append("=B");
                            break;
                        case KNIGHT:
                            finput.append("N");
                            oldInputs[oldInputs.size()-1].append("=N");
                            break;
                        default:
                            break;
                        }
                    }
                    if(caughtUpBranch == true && std::count(newInput.begin(), newInput.end(), '=') == 0)
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
                if(viewMode == ALT)
                    viewTurn = turn;

                if(offerTurn == turn)
                    wasOffered = false;

                oldOutput.append(printTurn(turnNum, oppoTurn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, valid, validEntry, next, argc, branchLvl));

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
                    {
                        if(alert == true)
                            std::cout << "\a" << std::flush;
                        break;
                    }
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
                }
                
                if(countMoves > 0 && wasOffered == true && offerTurn == oppoTurn)
                {
                    finput.append("(=)");
                    if(caughtUpBranch == true)
                        finput2.append("(=)");
                    
                    if(offerTurn == WHITE)
                        format += 3;

                    newOutput = printBoard(pieces, viewTurn, finput, next);
                    outputCopy = newOutput;
                }
                else
                {
                    newOutput = printBoard(pieces, viewTurn, finput, next);
                    outputCopy = newOutput;
                }

                newOutput.append(printTurn(turnNum, turn, branchPoint, gameMode, offerTurn, manualClaim, wasOffered, wasClaimed, valid, validEntry, 10, argc, branchLvl));

                break;

            } //End of inner loop

            if(gameMode == RETURN && turnNum == 0)
            {
                turnNum++;
                continue;
            }
            else if(changeView == true)
                continue;
            else if(gameOver == true)
                break;
            else if(caughtUp == true && choice != "review" && viewMode != WHITE && viewMode != BLACK)
            {
                std::cout << oldOutput << std::flush;
                pauseRun(1000);
                //std::cout << "did you pause? " << std::endl;
            }

            if(isCheck == true && countMoves == 0)
            {
                    finput.append("#\n\n");
                    if(oppoTurn == WHITE)
                        finput.append("\t1 - 0");
                    else
                        finput.append("\t0 - 1");

                    if(caughtUpBranch == true)
                    {
                        finput2.append("#\n\n");
                        if(oppoTurn == WHITE)
                            finput2.append("\t1 - 0");
                        else
                            finput2.append("\t0 - 1");
                    }

                if(choice != "review"){
                    outputCopy = printBoard(pieces, viewTurn, finput, 10);
                    outputCopy.append("Checkmate! ");

                    if(oppoTurn == WHITE)
                        outputCopy.append("White wins! ");
                    else if(oppoTurn == BLACK)
                        outputCopy.append("Black wins! ");

                    std::cout << outputCopy << std::endl;
                }

                newOutput.clear();

                break;
            }
            else if(isCheck == false && countMoves == 0)
            {
                finput.append("\n\n\t1/2 - 1/2");
                if(caughtUpBranch == true)
                    finput2.append("\n\n\t1/2 - 1/2");

                outputCopy = printBoard(pieces, viewTurn, finput, next);
                outputCopy.append("\nNo legal moves! Stalemate! ");

                if(choice != "review")
                    std::cout << outputCopy << std::endl;

                newOutput.clear();
                break;
            }

            getDrawNum = getDraw(pieces, turn, pMoved, false);

            if(getDrawNum == 3 && (manualClaim == false || wasClaimed == true))
            {
                //std::string partingWords = "\nThreefold repetition rule ";

                if(manualClaim == false && resume == true)
                {
                    outputCopy.append("\nThreefold repetition rule would be applicable. \nTurning off automatic draw claiming... ");
                        
                    if(choice != "review")
                        std::cout << outputCopy << std::endl;

                    manualClaim = true;
                    pauseRun(2000);
                    continue;
                }
                else
                {   
                    if(wasClaimed == true)
                        finput.append("(=)");

                    finput.append("\n\n\t1/2 - 1/2");

                    if(caughtUpBranch == true)
                    {
                        if(wasClaimed == true)
                            finput2.append("(=)");
                        finput2.append("\n\n\t1/2 - 1/2");
                    }

                    outputCopy = printBoard(pieces, viewTurn, finput, 10);
                      
                    if(choice != "review")
                        std::cout << outputCopy << std::endl;
                    
                    newOutput.clear();
                    break;
                }
            }
            else if(getDrawNum == 50 && (manualClaim == false || wasClaimed == true))
            {
                //std::string partingWords = "\n50-move rule ";

                if(manualClaim == false && resume == true)
                {
                    if(choice != "review")
                    {
                        outputCopy.append("\n50-move rule would be applicable. \nTurning off automatic draw claiming... ");
                        std::cout << outputCopy << std::endl;
                        pauseRun(2000);
                    }

                    manualClaim = true;
                    continue;
                }
                else
                {   
                    if(wasClaimed == true)
                        finput.append("(=)");

                    finput.append("\n\n\t1/2 - 1/2");

                    if(caughtUpBranch == true)
                    {
                        if(wasClaimed == true)
                            finput2.append("(=)");
                        finput2.append("\n\n\t1/2 - 1/2");
                    }

                    outputCopy = printBoard(pieces, viewTurn, finput, 10);
                    outputCopy.append("\n50-move rule would be applicable. \nTurning off automatic draw claiming... ");    
                    if(choice != "review")
                        std::cout << outputCopy << std::endl;
                    
                    newOutput.clear();
                    break;
                }
            }
            else if(getDrawNum == 5)
            {
                finput.append("\n\n\t1/2 - 1/2");
                
                if(caughtUpBranch == true)
                    finput2.append("\n\n\t1/2 - 1/2");

                outputCopy = printBoard(pieces, viewTurn, finput, 10);
                outputCopy.append("\nFivefold repetition rule invoked! Automatic draw! ");
                
                if(choice != "review")
                    std::cout << outputCopy << std::endl;
                newOutput.clear();
                break;
            }
            else if(getDrawNum == 75)
            {
                finput.append("\n\n\t1/2 - 1/2");
                
                if(caughtUpBranch == true)
                    finput2.append("\n\n\t1/2 - 1/2");

                outputCopy = printBoard(pieces, viewTurn, finput, 10);
                outputCopy.append("\n75-move rule invoked! Automatic draw! ");

                if(choice != "review")
                    std::cout << outputCopy << std::endl;

                newOutput.clear();
                break;
            }
            else if(getDrawNum == 100)
            {
                outputCopy.append("\nInsufficient material left! It's a draw! ");
                if(choice != "review")
                    std::cout << outputCopy << std::endl;

                finput.append("\n\n\t1/2 - 1/2");
                
                if(caughtUpBranch == true)
                    finput2.append("\n\n\t1/2 - 1/2");

                newOutput.clear();
                break;
            }
            else if(isCheck == true && countMoves > 0)
            {
                newOutput = printBoard(pieces, viewTurn, finput, next);
                outputCopy = newOutput;
                newOutput.append("Check! ");
            }

            if(gameOver == false && caughtUp == true)
            {
                std::cout << newOutput << std::flush; //check!
                oldOutput.clear();
            }
        } //End of outer loop
        if(gameMode == PLAY || gameMode == BRANCH)
        {
            int oldEnding[2];
            oldEnding[0] = turnNum;
            oldEnding[1] = turn;
            oldEndings.push_back(oldEnding);
        }
        if(gameMode != QUIT && gameMode != REVIEW)
        {
            oldMovesCatalog.push_back(oldInputs);
        }
        //if(decision == true)
        bool save{true};

        if(argc == 2 && choice != "quit" && gameMode != BRANCH && gameMode != RETURN)
        {
            std::ofstream gameLog;
            gameLog.open(argv[1], std::ios::out);
            gameLog << finput;
            gameLog.close();
        }
        else if((newGame == false || gameMode == RETURN || gameMode == BRANCH) && choice != "quit")
        {
            //bool save{true};
            if(gameMode == BRANCH/* && branchPoint[0] == branchPointFirst[0] && branchPoint[1] == branchPointFirst[1]*/)
            {
                std::cout << "\nWould you like to save this alternate ending? \nEnter 'y' or 'n': " << std::endl;
                std::string choice2;
                std::cin >> choice2;
                tolower(choice2);

                while(choice2[0] != 'y' && choice2[0] != 'n')
                {
                    std::cout << "\nInvalid input. \nEnter 'y' or 'n': " << std::endl;
                    std::cin >> choice2;
                    tolower(choice2);
                }

                if(choice2[0] == 'y')
                {  
                    static std::vector<int> oldEndingNums;
                    //static int oldEndingNum{1};
                    static std::vector<int> oldBranchLvls;
                    static int oldBranchLvl{1};

                    if(branchLvl == oldBranchLvl)
                        endingNum++;
                    else if(branchLvl > oldBranchLvl)
                    {
                        if(oldEndingNums.size() < branchLvl)
                            oldEndingNums.push_back(endingNum);
                        else
                            oldEndingNums[oldBranchLvl-1] = endingNum;
                        endingNum = 1;
                        oldBranchLvls.push_back(oldBranchLvl);
                        oldBranchLvl = branchLvl;
                    }
                    else if(branchLvl == 1)
                    {
                        endingNum = oldEndingNums[0];
                        endingNum++;
                        oldEndingNums.clear();
                        branchNum++;
                        oldBranchLvls.clear();
                        oldBranchLvl = 1;
                        oldBranchLvls.push_back(oldBranchLvl);
                    }
                    else if(branchLvl < oldBranchLvl)
                    {
                        oldEndingNums.pop_back();
                        endingNum = oldEndingNums[branchLvl-1];
                        endingNum++;
                        oldBranchLvls.pop_back();
                        oldBranchLvl = oldBranchLvls[branchLvl-1];
                    }
                    std::string branchStr = "\n";
                    if(branchLvl == 1)
                    {
                        branchStr.append(55, '=');
                        branchStr.append("\nAlternate ending set ");
                        branchStr.append(std::to_string(branchNum));
                        branchStr.append(", level 1, ");
                    }  
                    else
                    {
                        branchStr.append(55, '-');
                        branchStr.append("\n\tBranch ");
                        branchStr.append(std::to_string(branchNum));
                        branchStr.append(", level ");
                        branchStr.append(std::to_string(branchLvl));
                        branchStr.append(",");
                    }
   
                    branchStr.append(" starting at move ");
                    branchStr.append(std::to_string(branchPoint[0]));
                    if(branchPoint[1] == WHITE)
                        branchStr.append(", white:");
                    else if(branchPoint[1] == BLACK)
                        branchStr.append(", black:");

                    branchStr.append(" \n");
                    finput2.insert(0, branchStr);

                    std::ofstream branchLog;
                    if(branchLvl == 1)
                    {
                        branchLog.open(argv[1], std::ios::trunc);
                        branchLog << finput;
                    }
                    branchLog.close();
                    branchLog.open(argv[1], std::ios::app);
                    branchLog << finput2;
                    branchLog.close();
                }
                else
                    save = false;
            }
            choice = "review";
        }
        
        if(decision == true && choice != "quit")
        {
            if(choice != "review")
                std::cout << "\nGame over. Would you like to review the game? \nEnter 'yes' to enter review mode or enter 'quit' to quit. \n30 \t" << std::flush;

            //std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
            std::thread th2(cinThread, std::ref(choice), "review");
            th2.detach();

            int timeLeft = 30;
            std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();

            while(choice != "review" && choice != "quit" && userInput == true && timeLeft > 0)
            {
                tolower(choice);
                if(choice == "yes")
                    choice = "review";
                else if(choice == "quit")
                {
                    gameMode = QUIT;
                    break;
                }
                else if(choice != "review" && choice != "quit")
                {
                    if(std::chrono::steady_clock::now() -  t >= std::chrono::milliseconds(1000))
                    {
                        timeLeft--;
                        t = std::chrono::steady_clock::now();
                        std::cout << "\x1B[s\r" << std::to_string(timeLeft) << " \x1B[u" << std::flush; 
                    }
                }
            }
            if(timeLeft == 0 && choice != "review")
            {
                choice = "quit";
                std::cout << "\nSession timed out. Goodbye! " << std::endl;
            }

            if(choice == "review")
            {
                int first{0};
                if(branchLvl == 0)
                    first = 0;
                else
                    first = n_remembered[branchLvl-1];

                std::vector<std::vector<std::string>> oldBoardsAuto;
                std::vector<std::vector<std::string>> oldBoardsAll;
                std::vector<std::vector<std::string>> allBoardsShort;
                std::vector<std::string> allBoardsTemp;
                std::vector<std::string> turnStrings;
                static std::string navigation{"\n"};
                static std::string defaultNav{"\n"};
                std::vector<std::string> instructionsManual;
                static unsigned long int n{0};
                int mode = viewMode;
                int oldMode = oldViewMode;
                bool skip{false};
                bool go{false};
                int last_n{-1};

                oldBoards.push_back(printBoard(pieces, turn, finput, 10));
                oldBoardsFlip.push_back(printBoard(pieces, oppoTurn, finput, 10));
                oldBoardsWhite.push_back(printBoard(pieces, WHITE, finput, 10));
                oldBoardsBlack.push_back(printBoard(pieces, BLACK, finput, 10));

                int minus{1};
                if(branchLvl > 0)
                {
                    std::string extraString{"BRANCH #"};
                    extraString.append(std::to_string(branchNum));
                    extraString.append(", Level ");
                    extraString.append(std::to_string(branchLvl));
                    if(save == true)
                    {
                        extraString.append(", ending ");
                        extraString.append(std::to_string(endingNum));
                    }
                    minus += extraString.size()+1;
                    for(int n = 0; n < oldBoards.size(); n++)
                    {
                        oldBoardsBlack[n].insert(oldBoardsBlack[n].size()-1, extraString);
                        oldBoards[n].insert(oldBoards[n].size()-1, extraString);
                        oldBoardsWhite[n].insert(oldBoardsWhite[n].size()-1, extraString);
                        oldBoardsFlip[n].insert(oldBoardsFlip[n].size()-1, extraString);
                    }
                }

                allBoardsShort.push_back(oldBoardsBlack);
                allBoardsShort.push_back(oldBoards);
                allBoardsShort.push_back(oldBoardsWhite);
                allBoardsShort.push_back(oldBoardsFlip);

                for(int n = 0; n < oldBoards.size(); n++)
                {
                    oldBoards[n].insert(oldBoards[n].size()-minus, "VIEW MODE: NORMAL         ");
                    oldBoardsWhite[n].insert(oldBoardsWhite[n].size()-minus, "VIEW MODE: WHITE          ");
                    oldBoardsBlack[n].insert(oldBoardsBlack[n].size()-minus, "VIEW MODE: BLACK          ");
                    oldBoardsFlip[n].insert(oldBoardsFlip[n].size()-minus, "VIEW MODE: FLIPPED        ");
                }

                oldBoardsAuto.push_back(oldBoardsBlack);
                oldBoardsAuto.push_back(oldBoards);
                oldBoardsAuto.push_back(oldBoardsWhite);
                oldBoardsAuto.push_back(oldBoardsFlip);

                turn = WHITE;
                turnNum = 1;

                std::string newppgh{"\n   "};

                std::string instructions1st = " Enter '>' to advance forward one move, 'w', 'b', or 'n' respectively to change the perspective to White, Black, or normal (alternating), 'flip' to temporarily flip the board, 'skip+ n' to skip forward n moves, '>>' to skip to the end, 'auto' to watch the game unfold automatically, or simply 'jump' to skip directly to any point in the game. To return to the previous position viewed, enter 'last'. To quit the progam, enter 'quit': ";

                std::string instructions = " Enter '>' to advance forward one move, '<' to return to the previous move, 'w', 'b', or 'n' respectively to change the perspective to White, Black, or normal (alternating), 'flip' to temporarily flip the board, 'skip+ n' or 'skip- n' to skip forward or back n moves, '<<' to skip to the beginning, '>>' to skip to the end, 'auto' to watch the game unfold automatically, or simply 'jump' to skip directly to any point in the game. To return to the previous position viewed, enter 'last'. To quit the program, enter 'quit': ";

                std::string instructionsLast = " Enter '<' to return to the previous move, 'w', 'b', or 'n' respectively to change the perspective to White, Black, or normal (alternating), 'flip' to temporarily flip the board, 'skip- n' to skip back n moves, '<<' to skip to the beginning, or simply 'jump' to skip directly to any point in the game. To return to the previous position viewed, enter 'last'. To quit the program, enter 'quit': ";

                std::string instructions1stBranch = instructions1st;
                std::string instructionsBranch = instructions;

                instructions1stBranch.insert(instructions1stBranch.size()-37, ". To explore an alternate ending from this point, enter 'branch'");
                instructionsBranch.insert(instructionsBranch.size()-37, ". To explore an alternate ending from this point, enter 'branch'");

                if(branchLvl > 0)
                {
                    std::string returnString = ". To end this tangent and return to the previous branch, enter 'return'. To end all tangents, enter 'return all'";
                    instructions1st.insert(instructions1st.size()-37, returnString);
                    instructions1stBranch.insert(instructions1stBranch.size()-37, returnString);
                    instructions.insert(instructions.size()-37, returnString);
                    instructionsBranch.insert(instructionsBranch.size()-37, returnString);
                    instructionsLast.insert(instructionsLast.size()-37, returnString);
                }
                
                formatString(instructions1st, 4, newppgh);
                formatString(instructions, 4, newppgh);
                formatString(instructionsLast, 4, newppgh);  
                
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
                    if(n <= first)
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
                        std::string newLineSpace{"\n"};
                        newLineSpace.append(turnStr.size()+1, ' ');
                        if(n % 2 == 0)
                        {
                            oldBoardsAuto[z][n].append("  ");
                            newLineSpace.append(2, ' ');
                        }

                        std::string autoInstructions = " This game will advance automatically by one ply (half-move) every ";
                        if(z % 2 == 0)
                            autoInstructions.append("two ");
                        else
                            autoInstructions.append("three ");
                        
                        autoInstructions.append("seconds. Enter 's' at any time to exit AUTO mode: \n");

                        formatString(autoInstructions, 2, newLineSpace);
                        oldBoardsAuto[z][n].append(autoInstructions);
                    }

                    turn *= -1;
                    if(turn == WHITE)
                        turnNum++;

                    oldBoardsAll.push_back(oldBoardsBlack);
                    oldBoardsAll.push_back(oldBoards);
                    oldBoardsAll.push_back(oldBoardsWhite);
                    oldBoardsAll.push_back(oldBoardsFlip);

                    instructionsManual.clear();
                    
                    if(branchLvl > 0)
                    {
                        instructionsManual.push_back(instructions1stBranch);
                        for(int i = 1; i < oldBoards.size()-1; i++)
                            instructionsManual.push_back(instructionsBranch);
                        instructionsManual.push_back(instructionsLast);
                    }
                    else
                    {
                        instructionsManual.push_back(instructions1st);
                        for(int i = 1; i < oldBoards.size()-1; i++)
                            instructionsManual.push_back(instructions);
                        instructionsManual.push_back(instructionsLast);
                    }
                }
                /*else */if(gameMode == RETURN)
                {
                    navigation = defaultNav;
                    n = n_remembered[branchLvl];
                    n_remembered.pop_back();    
                }
                else if(gameMode == BRANCH)
                {
                    navigation = defaultNav;
                    n = n_remembered[branchLvl-1];
                }

                bool last{false};
                //modeType oldGameMode = gameMode;

                std::thread th3(cinThread2, std::ref(navigation), std::ref(last), std::ref(go), std::ref(skip), std::ref(mode), std::ref(gameMode), std::ref(branchLvl));
                th3.detach();
                gameMode = REVIEW;
                //std::cout << "navigation == " << navigation << std::endl;
                t = std::chrono::steady_clock::now();
                while(navigation != "all" && std::chrono::steady_clock::now() - t < std::chrono::milliseconds(500));

                if(navigation == "all")
                {
                    gameMode = RETURN;
                    oldInputs.clear();
                    newInputs.clear();
                    branchLvl = 0;
                    newInputs = oldMovesCatalog[0];
                    finput = fSkeleton;
                    finput2.clear();
                    pieces = startPieces;
                    getDraw(pieces, WHITE, NO_PIECE, true);
                    turnNum = 1;
                    turn = WHITE;
                    oppoTurn = BLACK;
                    offerTurn = WHITE;
                    wasOffered = false;
                    wasClaimed = false;
                    branchPoint[0] = oldEndings[0][0];
                    branchPoint[1] = oldEndings[0][1];
                    caughtUp = false;
                    inputsRead = 0;
                    oldBoardsBlack.clear();
                    oldBoards.clear();
                    oldBoardsWhite.clear();
                    oldBoardsFlip.clear();
                    oldMovesCatalog.clear();
                    oldEndings.clear();
                    navigation = defaultNav;

                    continue;
                }

                

                while(navigation != "quit")
                {
                    if(n == oldBoards.size()-1)
                        last = true;
                    else   
                        last = false;

                    bool valid{true};
                    skip = false;
                    
                    if(oldMode != mode && (mode == BLACK || mode == ALT || mode == WHITE))
                        oldMode = mode;

                    std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
                    int once{0};

                    switch (mode)
                    {
                    case BLACK:
                        std::cout << oldBoardsBlack[n] << std::flush;
                        go = false;
                        break;
                    case ALT:
                        std::cout << oldBoards[n] << std::flush;
                        go = false;
                        break;
                    case WHITE:
                        std::cout << oldBoardsWhite[n] << std::flush;
                        go = false;
                        break;
                    case FLIP:
                        std::cout << oldBoardsFlip[n] << std::flush;
                        mode = oldMode;
                        go = false;
                        break;
                    case AUTO:
                        std::cout << oldBoardsAuto[oldMode+1][n] << std::flush;
                        while(std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(2000) && navigation != "s");
                        while(navigation != "s" && n < oldBoards.size()-2 && oldMode == ALT && std::chrono::steady_clock::now() - t <= std::chrono::milliseconds(3000))
                        {
                            if(once++ == 0) 
                                std::cout << oldBoardsAuto[FLIP+1][n].substr(0, allBoardsShort[FLIP+1][n].size()-1) << oldBoardsAuto[ALT+1][n].substr(oldBoardsAuto[ALT+1][n].find("VIEW MODE:", 0), oldBoardsAuto[ALT+1][n].size()-1) << std::flush;

                            if(navigation != "s" && navigation != "auto" && navigation != "quit")
                            {
                                navigation = "auto";
                                once = 0;
                            }
                            else if(navigation == "quit")
                            {
                                navigation = "s";
                                gameMode = QUIT;
                            }
                        }
                        
                        break;
                    
                    default:
                        std::cout << allBoardsShort[mode+21][n].substr(0, allBoardsShort[mode+21][n].size()-1) << "INVALID INPUT. \n" << turnStrings[n] << instructionsManual[n] << std::flush;
                        
                        pauseRun(1000);
                        valid = false;
                        mode += 20;
                        break;
                    }

                    if(mode == AUTO && (navigation == "s" || n >= oldBoards.size()-1))
                    {
                        go = false;
                        mode = oldMode;
                        navigation = defaultNav;
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
                    else if(navigation == "\n")
                        continue;
                    else if(navigation == "<" && n > first)
                    {
                        last_n = n;
                        n--;
                    }
                    else if(navigation == ">" && n < oldBoards.size()-1)
                    {
                        last_n = n;
                        n++;
                    }
                    else if(navigation == "<<")
                    {
                        last_n = n;
                        n = first;
                    }
                    else if(navigation == ">>")
                    {
                        last_n = n;
                        n = oldBoards.size()-1;
                    }
                    else if((navigation == "skip+" && n < oldBoards.size()-1) || (navigation == "skip-" && n > first))
                    {
                        std::cout << allBoardsShort[mode+1][n] << "Please enter the number of moves you wish to skip, or 0 to cancel: \n\n\n" << std:: endl;

                        int jumpSize{0};
                        std::cin >> jumpSize;

                        jumpSize *= 2;
                        
                        int last_n_backup = last_n;
                        last_n = n;

                        if(navigation == "skip+" && jumpSize < oldBoards.size()-n && jumpSize > 0)
                            n += jumpSize;
                        else if(navigation == "skip-" && jumpSize <= n-first && jumpSize > 0)
                            n -= jumpSize;
                        else if(navigation == "skip+" && jumpSize > 0)
                            n = oldBoards.size()-1;
                        else if(navigation == "skip-" && jumpSize > 0)
                            n = first;
                        else if(jumpSize != 0)
                        {
                            mode -= 20;
                            last_n = last_n_backup;
                        }

                        navigation = defaultNav;
                        continue;
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
                        std::cout << allBoardsShort[mode+1][n] << "\n\nPlease enter the desired move number and player to move, \nor enter 0 to cancel: " << std::endl;

                        int target;
                        std::cin >> target;

                        while((target < branchPoint[0] && target != 0) || target*2 > oldBoards.size())
                        {
                            std::cout << allBoardsShort[mode+1][n] << "\nInvalid input. \nPlease enter a valid move number and player to move, \nor 0 to cancel: " << std::endl;

                            std::cin.clear();
                            std::cin.ignore(10000, '\n');

                            std::cin >> target;
                        }
                        if(target == 0)
                            continue;

                        std::cout << allBoardsShort[mode+1][n] << "\n\nPlease enter the player to move, \nor 'cancel' to cancel: " << std::endl;

                        std::string target2;
                        std::cin >> target2;
                        tolower(target2);

                        while(target2 != "white" && target2 != "black" && target2 != "cancel")
                        {
                            std::cout << allBoardsShort[mode+1][n] << "\nInvalid input. \nPlease enter a valid player to move \nor 'cancel' to cancel: " << std::endl;

                            std::cin.clear();
                            std::cin.ignore(10000, '\n');

                            std::cin >> target2;
                            tolower(target2);
                        }

                        int plus{0};
                        target--;

                        if(target2 == "black")
                            plus = 1;
                        else if(target2 == "cancel")
                            continue;
                        
                        if(target*2+plus < first || target*2+plus > oldBoards.size()-1)
                            mode -= 20;
                        else
                        {
                            last_n = n;
                            n = target*2+plus;       
                        }   
                    }
                    else if(navigation == "auto" && n < oldBoards.size()-1)
                        mode = AUTO;
                    else if(mode == AUTO && n == oldBoards.size()-1)
                        continue;
                    else if(navigation == "branch" && last == false)
                    {
                        gameMode = BRANCH;
                        choice = "start";
                        navigation = "\n";     
                        newTurnNum = 1 + n/2;
                        if(n % 2 == 0)
                            newTurn = WHITE;
                        else
                            newTurn = BLACK;

                        if(save == false)
                        {
                            startPt[0] = branchPoint[0];
                            startPt[1] = branchPoint[1];
                        }

                        branchPoint[0] = newTurnNum;
                        branchPoint[1] = newTurn;

                        if(save == true)
                        {
                            startPt[0] = branchPoint[0];
                            startPt[1] = branchPoint[1];
                        }
                        viewMode = mode;
                        oldViewMode = oldMode;
                        newInputs = oldMovesCatalog[branchLvl];
                        branchLvl = oldMovesCatalog.size();
                        inputsRead = 0;
                        gameOver = false;
                        caughtUp = false;
                        finput.clear();
                        finput2.clear();
                        finput = fSkeleton;
                        pieces = startPieces;
                        turn = WHITE;
                        turnNum = 1;
                        oppoTurn = BLACK;
                        offerTurn = WHITE;
                        wasOffered = false;
                        wasClaimed = false;
                        oldInputs.clear();
                        getDraw(pieces, WHITE, NO_PIECE, true);
                        oldBoardsBlack.clear();
                        oldBoards.clear();
                        oldBoardsWhite.clear();
                        oldBoardsFlip.clear();
                        n_remembered.push_back(n);
                        n = 0;

                        break;
                    }
                    else if(navigation == "return" && branchLvl > 0)
                    {
                        gameMode = RETURN;
                        oldInputs.clear();
                        newInputs.clear();
                        branchLvl--;
                        newInputs = oldMovesCatalog[branchLvl];
                        finput = fSkeleton;
                        finput2.clear();
                        pieces = startPieces;
                        getDraw(pieces, WHITE, NO_PIECE, true);
                        turnNum = 1;
                        turn = WHITE;
                        oppoTurn = BLACK;
                        offerTurn = WHITE;
                        wasOffered = false;
                        wasClaimed = false;
                        branchPoint[0] = oldEndings[branchLvl][0];
                        branchPoint[1] = oldEndings[branchLvl][1];
                        caughtUp = false;
                        inputsRead = 0;
                        oldBoardsBlack.clear();
                        oldBoards.clear();
                        oldBoardsWhite.clear();
                        oldBoardsFlip.clear();
                        oldMovesCatalog.pop_back();
                        oldEndings.pop_back();
                        navigation = defaultNav;

                        break;
                        
                    }
                    else if(navigation == "last" && last_n >= first)
                    {
                        int temp_n = n;
                        n = last_n;
                        last_n = temp_n;
                        continue;
                    }
                    else if(navigation == "quit")
                    {
                        gameMode = QUIT;
                        break;
                    }
                    else
                        mode -= 20;
                }
            }
            else
                gameMode = QUIT;

            if(gameMode == QUIT)
                std::cout << "Game Over. Goodbye! " << std::endl;
            else if(gameMode == BRANCH || gameMode == RETURN)
                continue; //goes to while(gameMode != QUIT)
            else
            {
                std::cout << "Error. Something went wrong. \nQuitting without saving... " << std::flush;
                pauseRun(2000);
                gameMode = QUIT;
                std::cout << "\nProgram aborted. Goodbye!" << std::endl;
                pauseRun(1000);
                continue;
            }
        }
        else
        {
            std::cout << "\nGame Over. Goodbye! " << std::endl;
            gameMode = QUIT;
        }

    }
    //std::cout << "choice == " << choice << "\nand gameMode == " << gameMode << std::endl;
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
    if(team == WHITE) 
        i += NUM_FILES;

    canCastle = false; 
    EPVul = false;

    if(team == EMPTY) 
        type = NO_PIECE;
    else if(i % PIECES_PER_TEAM == 9 || i % PIECES_PER_TEAM == 0)
    {   
        if(timesMoved == 0)
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
    if(i <= PIECES_PER_TEAM) 
        team = WHITE;
    else if(i > PIECES_PER_TEAM && i <= MAX_SQUARES - PIECES_PER_TEAM) 
        team = EMPTY;
    else
        team = BLACK;

    return;
}
//This function generates the string that will be represent the piece whenever printBoard is called
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
//This function keeps track of the original name of the piece in case a later change to the value needs to be undone (such as when a promoted pawn gets captured)
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
//This function returns a string representing the piece name, but only if the location matches.
std::string Piece::getNameString(int i, int j){
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
std::vector<std::string> convertMove(std::vector<Piece> &pieces, std::string realMove, int turn, bool &readCin) // search here
{
    std::vector<std::string> route;
    std::vector<std::string> badRoute;

    std::string move = realMove;

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

            if(r == -1 || pieces[k].getFile() != 'e')
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

            if(r == -1 || pieces[k].getFile() != 'e')
                return badRoute;
            else 
            {
                target.append(1, 'c');
                target.append(1, pieces[k].getRank());
                route.push_back(target);
                //std::cout << "castle queenside == " << route[0] << " to " << route[1] << std::endl;
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

            //This checks that the information provided is sufficient.
            for(int i = 0; i < MAX_SQUARES; i++)
            {
                std::vector<Piece> testPieces = pieces;
                if(movePiece(testPieces, testPieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
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
                else if(promoting == false)
                    return route;
                else if(route[1][1] != '8' && route[1][1] != '1')
                    return badRoute;
                else
                {
                    readCin = false;
                    std::string newValue;
                    newValue.append(1, move[move.size()-1]);
                    route.push_back(newValue);

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
                std::vector<Piece> testPieces = pieces;
                if(movePiece(testPieces, testPieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
                {
                    if(pieces[i].getFile() != move[1])
                        alts++;
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
                    readCin = false;
                    std::string newValue;
                    newValue.append(1, move[move.size()-1]);
                    route.push_back(newValue);
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

        std::string start{"s0"};
        std::string end{""};
        end.append(1, move[3]);
        end.append(1, move[4]);

        std::string altFiles;    

        for(int i = 0; i < MAX_SQUARES; i++)
        {
            std::vector<Piece> testPieces = pieces;
            if(movePiece(testPieces, testPieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
            {
                if(pieces[i].getRank() != move[1])
                    altFiles.push_back(pieces[i].getFile());
                else
                {
                    options++;
                    start = pieces[i].getLocation();
                }
            }
        }

        alts = std::count(altFiles.begin(), altFiles.end(), start[0]);
        //std::cout << "alts == " << alts << std::flush;
        
        route.push_back(start);
        route.push_back(end);

        if(options == 1 && alts == 1)
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
                std::vector<Piece> testPieces = pieces;
                if(movePiece(testPieces, testPieces[i].getLocation(), end, turn) == true && pieces[i].getType() == searchType)
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
//This function converts the entered move into chess notation.
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
        input = "R";
        break;
    case KNIGHT:
        input = "N";
        break;
    case BISHOP:
        input = "B";
        break;
    case QUEEN:
        input = "Q";
        break;
    case KING:
        input = "K";
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
        captives.append("     BLACK\n\t\t\t **");
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
        captives.append("     WHITE\n\t\t\t **");
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
        
        captives.append("** \n\t\t\t           ");
        
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
        
        captives.append("** \n\t\t\t           ");

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
    int y{0};
    int partialSize{19};

    if(next == 10)
    {
        chessBoard.append(finput);
        chessBoard.append("\n\n\n\n\n\n");
    }

    fpartial.clear();

    bool startGame{false};
    bool endGame{false};

    while(x < finput.size())
    {
        fline.clear();
        do
        { 
            if(finput[x] != '\n' /*&& finput[x] != '*'*/)
                fline.append(1, finput[x]);
            x++;
        }while(x < finput.size() && finput[x] != '\n');
        fpartial.push_back(fline);

        if(fline.find("1.") == std::string::npos && startGame == false)
            y++;
        else 
            startGame = true;

        if(fline.size() == 0)
            partialSize++;

        if(std::count(fline.begin(), fline.end(), '-') == 1 && std::count(fline.begin(), fline.end(), 'O') == 0)
            endGame = true;
    }

    //std::cout << "next step" << std::endl;
    
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
                //std::cout << chessBoard.substr(chessBoard.find_last_of('\n'), chessBoard.size()) << std::flush;
                chessBoard.append("\n\t\t\t");
                
                if(row == 0) //This will be used to print the top edge of each row
                {
                    chessBoard.append("          ");

                    for(int j = 0; j < NUM_FILES; j++)
                        chessBoard.append("*-------"); 

                    chessBoard.append("*");
                }
                else if(row != 2) //This will be used to print the top and bottom thirds of each actual square
                {
                    chessBoard.append("          ");
                    for(int j = 0; j < NUM_FILES; j++) 
                    {
                        if((i+j) % 2 == LIGHT)
                            chessBoard.append("|*******");
                        else
                            chessBoard.append("|       ");
                    }
                    chessBoard.append("| ");
                }
                else //This prints the center third of each square, and any piece that may be on it
                {
                    chessBoard.append("      (" );
                    chessBoard.append(1, '0' + i);
                    chessBoard.append(") ");

                    for(int j = 0; j < NUM_FILES; j++) 
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

                //This prints out the final twenty moves of the score sheet next to the board
                if(i == 7 && row == 0)
                    chessBoard.append(fpartial[0]);
                else if(i == 7 && row == 1)
                    continue;
                else if(i >= 1 && i < 8) 
                {
                    if(endGame == true)
                        partialSize++;
                    
                    while(fpartial.size()-y > partialSize && fpartial.size()-y < 10000 && fpartial.size()-y >= 0)
                        y++;

                    if(y < fpartial.size())
                    {
                        chessBoard.append(fpartial[y]);
                        y++;
                    }
                    if(y >= fpartial.size() && y == 2)
                    {
                        if(finput[finput.size()-1] == '*')
                            chessBoard.append("*");
                        y = 1234567899;
                    }
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
    else if(turn == BLACK) 
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

                //This prints the last twenty moves of the score sheet next to the board
                if(i == 2 && row == 0)
                    chessBoard.append(fpartial[0]);
                else if(i == 2 && row == 1)
                    continue;
                else if(i > 1 && i <= 8)
                {
                    if(endGame == true)
                        partialSize++;

                    while(fpartial.size()-y > partialSize && fpartial.size()-y < 10000 && fpartial.size()-y >= 0)
                        y++;

                    if(y < fpartial.size())
                    {
                        chessBoard.append(fpartial[y]);
                        y++;
                    }
                    /*if(y >= fpartial.size() && y == 2)
                    {
                        if(finput[finput.size()-1] == '*')
                            chessBoard.append("*");
                        y = 1234567899;
                    }*/
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
std::string printTurn(int turnNum, int color, int* branchPoint, modeType gameMode, int offerTurn, bool manualClaim, bool wasOffered, bool wasClaimed, bool valid, bool validEntry, int next, int argc, int branchLvl) 
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
        turnPhrase.append("Invalid move. Try again.");

    int n = turnPhrase.size()-1;

    if(argc == 2 && branchLvl == 0)
        claimClause.append(", enter 'adjourn game' to save the game to a file for later"); 
    
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
        
        if(offerTurn != color || offerTurn != checkBackTurn)
            offerClause = ", or enter 'accept draw' to accept your opponent's draw offer";
        else if(wasClaimed == false && manualClaim == true && offerTurn == color)
            claimClause.append(", or enter 'claim draw' to initiate a draw claim");
    }   

    int x = turnPhrase.size()-1;

    std::string undoPhrase{" "};
    if(turnNum > branchPoint[0] || (turnNum == branchPoint[0] && color < branchPoint[1]))
        undoPhrase.append("'undo' to undo ");
    
    turnPhrase.append("\nPlease enter your move,");
    turnPhrase.append(undoPhrase);
    turnPhrase.append("or 'I resign' to resign");
    turnPhrase.append(offerClause);
    turnPhrase.append(claimClause);
    turnPhrase.append(", or enter 'change' to change the board perspective. To quit without saving, enter 'I quit'");
    if(branchLvl != 0 && gameMode == BRANCH)
        turnPhrase.append(". To end this tangent and return to the previous branchpoint, enter 'return', or, to end all tangents, enter 'return all'");
    turnPhrase.append(": ");

    float max = (turnPhrase.size()-x)/3;
    //do{x++;}while(turnPhrase[x] != '\n');
    //int extraLength{0};
    //bool tooLong{false};

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
            turnPhrase.append(" White to move. \nPlease enter your move or 'undo' to undo: ");
        else if(color == BLACK)
            turnPhrase.append(" Black to move. \nPlease enter your move or 'undo' to undo: ");
    }
    
    while(std::count(turnPhrase.begin(), turnPhrase.end(), '\n') < 3)
        turnPhrase.append("\n");
    
    turnPhrase.append("\n");
    if(next != 10)
        turnPhrase.append("Executing multiple moves...");
    return turnPhrase;       
}

void formatString(std::string &longString, int i, std::string newppgh)
{
    int size = longString.size();
    int lineSize = 10 + size/(i+1);
    int x{0};

    for(int n = 0; n < i; n++)
    {
        int x2{0};
        while(longString[x+x2] != ' ' || x2 <= lineSize)
            x2++;
        
        x += x2+1;
        x2 = 0;

        longString.insert(x, newppgh);
    }

    return;
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
void promotePawn(std::vector<Piece> &pieces, int i, int viewTurn, int oppoView, int turnNum, int &offerTurn, bool &wasOffered, bool &wasClaimed, bool caughtUp, std::vector<std::string> &oldInputs, std::vector<std::string> &newInputs, int &inputsRead, std::string finput, bool &needsEntry, char &newValEntered, int* branchPoint, modeType gameMode, int branchLvl, int &escapeNum)
{
    std::string outputTemp;
    outputTemp.append(printBoard(pieces, oppoView, finput, 10));
    outputTemp.append(printTurn(turnNum, oppoView, branchPoint, gameMode, offerTurn, false, wasOffered, wasClaimed, true, true, 10, 1, branchLvl)); //prints the board and pieces
    
    if(caughtUp == true && needsEntry == true)
    {
        if(oppoView != viewTurn)
            std::cout << outputTemp << std::flush;
        pauseRun(1000);
    }
    
    outputTemp.clear();

    outputTemp.append(printBoard(pieces, viewTurn, finput, 10));
    outputTemp.append("PAWN PROMOTION! \nPlease "); 
    if(wasOffered == false)
        outputTemp.append("enter 'offer draw' if you wish to offer a draw, then ");
    outputTemp.append("select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: \n");

    if(caughtUp == true && needsEntry == true)
        std::cout << outputTemp << std::flush;

    outputTemp.clear();

    if(needsEntry == true)
    {
        if(std::cin.peek() != 10 && caughtUp == true)
            pauseRun(1500);
    }

    char newVal{' '};
    std::string newString;
    int once{0};

    while(newVal == ' ')
    {
        //std::cout << " WHILE LOOP HIT! " << std::flush;
        if(once != 0 && needsEntry == true && gameMode != BRANCH && gameMode != RETURN)
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
        else
            once++;

        if(caughtUp == true && needsEntry == true)
        {
            std::cin >> newString;
            newVal = newString[0];
        }
        else// if(caughtUp == true && needsEntry == false)
            newVal = newValEntered;
        /*else
        {
            newVal = newInputs[inputsRead][0];
            std::cout << newInputs[inputsRead] << " == newInputs[inputsRead] and THIS HAD BETTER WORK" << std::endl;
            inputsRead++;
        }*/

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
            std::string myOffer;
            std::cin >> myOffer;

            for(int n = 0; n < myOffer.size(); n++)

            outputTemp.append(printBoard(pieces, viewTurn, finput, 10));

            if(newString == "offer" && myOffer == "draw")
            {
                wasOffered = true;
                outputTemp.append("\nDraw offer extended. \nPlease enter 'q', 'r', 'b', or 'n': \n");
                once--;
                offerTurn = pieces[i].getTeam();
            }
            else if(myOffer == "claim" && myOffer == "draw")
            {
                wasClaimed = true;
                wasOffered = true;
                outputTemp.append("\nDraw claim invalid. All draw claims double as draw offers. \nPlease enter 'q', 'r', 'b', or 'n': \n");
                once--;
                offerTurn = pieces[i].getTeam();
            }
            else
            {
                outputTemp.append("Invalid input. \nPlease select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: \n");
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
            outputTemp.append(printBoard(pieces, viewTurn, finput, 10));
            outputTemp.append("Invalid input. Please select a piece to promote to by entering \n'q' for queen, 'r' for rook, 'b' for bishop, or 'n' for knight: \n");
            std::cout << outputTemp << std::flush;
            outputTemp.clear();
            newVal = ' ';
            needsEntry = true;
            if(caughtUp == false && escapeNum > 10)
                return;
            else if(caughtUp == false)
                escapeNum++;

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
                //oldInputs.push_back("draw");
            }
        }
    }

    pieces[i].setName(pieces[i].getType(), pieces[i].getTeam(), WHITE);
    escapeNum = 0;
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
//This function returns output depending on various circumstances that affect whether draw conditions are met.
int getDraw(std::vector<Piece> &pieces, int turn, pieceType pMoved, bool reset)
{
    std::vector<Piece> oldPieces = pieces;
    static std::vector<std::string> boardStates;
    std::string newState{""};
    static int oldPiecesLeft{MAX_PIECES};
    static int turnsTowardsDraw{0};
    int newPiecesLeft{MAX_PIECES};
    std::vector<int> b;
    b.clear();

    if(reset == true)
    {
        boardStates.clear();
        newState.clear();
        oldPiecesLeft = MAX_PIECES;
        turnsTowardsDraw = 0;
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
            std::string epCheckSpot2 = epCheckSpot1;

            epCheckSpot1[0]--;
            epCheckSpot2[0]++;

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