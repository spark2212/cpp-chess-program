// ChessGameRewrite.cpp

#include<iostream> // std::cout, cin, etc
#include<string> // std::string, string functions
#include<vector> // std::vector, vector operations
#include<math.h> // operators
#include<algorithm> // std::count
// #include "ChessGame_files.hpp"

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
        char rank;
        char file;
        std::string location;
        int timesMoved;
        int timeSinceMoved;
        bool canCastle;
        bool isCastling;
        bool EPVul;
        bool canEP;

    public:
        //ctor
        Piece(char f, char r, int i);

        //setters
        void setType(int i); 
        void setTeam(int i);
        void setName(pieceType tp, int tm); 
        void setLocation(char f, char r); //sets location based on separate rank and file inputs
        void setLocation(std::string l); //sets location based on a single location input
        void resetTimeSinceMoved();
        void setCanCastle(bool allowed); //determines if a piece may be used in castling
        void setIsCastling(bool is00);
        void setEPVul(bool isVul); //determines if a piece is vulnerable to being captured by En Passant
        void setCanEP(bool canEP);

        //getters
        pieceType getType();
        int getTeam();
        std::string getName();
        std::string getLocation();
        char getFile();
        char getRank();
        int getTimesMoved();
        int getTimeSinceMoved();
        bool getCanCastle();
        bool getIsCastling();
        bool getEPVul();
        bool getCanEP();

        //other functions
        void printName(int i, int j); //prints the name of the piece if the corresponding location matches, else prints nothing
        void addTimeMoved();
        void addTimeSinceMoved();
        void subtractTimeMoved();
        void subtractTimeSinceMoved();
        void restoreTimeSinceMoved(int i);
        void changeType(pieceType newType);
    };

//FUNCTION DECLARATIONS///////////////////////////////////
    std::vector<Piece> initializePieces(); 
    void printBoard(std::vector<Piece> &pieces);
    int findPiece(std::vector<Piece> &pieces, std::string location);
    int findPiece(std::vector<Piece> &pieces, char f, char r);
    int findEmpty(std::vector<Piece> &pieces, std::string location);
    int findKing(std::vector<Piece> &pieces, int color);
    bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
    void removePiece(std::vector<Piece> &pieces, int i);
    void printTurn(int turnNum, int color);
    bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
    bool checkCanCapture(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
    bool isDiagonal(std::string start, std::string end);
    int getCastlingRookIndex(std::vector<Piece> &pieces, int i, int j, std::string start, int oppoTurn);
    int countLegalMoves(std::vector<Piece> &pieces, int turn);

//MAIN FUNCTION DEFINITION////////////////////////////////
int main(void)
{
    std::vector<Piece> pieces = initializePieces();
    printBoard(pieces);

    static unsigned long int turnNum{1};
    int turn{WHITE};
    int oppoTurn{BLACK};
    bool draw{false};

    while(1)
    {
        std::string start;
        std::string end;
        printTurn(turnNum, turn);

        std::cin >> start;
        std::cin >> end; //Takes user input for the desired move

        bool valid = movePiece(pieces, start, end, turn); //If the move is valid, performs the move and returns true. Else, returns false.
        printBoard(pieces); //prints the board and pieces
        if(valid == false)
        {
            std::cout << "Invalid move. Try again.";
            continue;
        }
        else
        {
            oppoTurn = turn;
            turn *= -1;
            if(turn == WHITE)
                turnNum++;
        }
        bool isCheck{false};
        
        int k = findKing(pieces, turn);
        for(int j = 0; j < MAX_SQUARES; j++)
        {
            isCheck = checkCanCapture(pieces, pieces[j].Piece::getLocation(), pieces[k].Piece::getLocation(), oppoTurn);
            if(isCheck == true)
                break;
        }
        int countMoves = countLegalMoves(pieces, turn);

        //int countMoves = 1;
        if(isCheck == true && countMoves > 0)
            std::cout << "Check! ";
        else if(isCheck == true && countMoves == 0)
        {
            std::cout << "Checkmate! ";
            if(oppoTurn == WHITE)
                std::cout << "White wins! " << std::endl;
            else if(oppoTurn == BLACK)
                std::cout << "Black wins! " << std::endl;

            break;
        }
        else if(isCheck == false && countMoves == 0)
        {
            std::cout << "No legal moves! Stalemate! " << std::endl;
            break;
        }
        else if(draw == true)
        {
            std::cout << "Draw! " << std::endl;
            break;
        }
        else
        {
            for(int a = 0; a < MAX_SQUARES; a++) //Nothing to do but reset En Passant values
            {
                if(pieces[a].Piece::getTeam() == turn)
                    pieces[a].Piece::setEPVul(false);
            }
        }
    }

    std::cout << "Game Over. Goodbye! \n" <<std::endl;
    
    return 0;
}

//CLASS FUNCTION DEFINITIONS///////////////////////////////
    //Piece class ctor
        Piece::Piece(char f, char r, int i){
            setTeam(i);
            setType(i);
            setName(type, team);
            setLocation(f, r);
            setCanCastle(true);
            setEPVul(true);

            timesMoved = 0;
            timeSinceMoved = 0;
            isCastling = false;
            canEP = false;
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
        void Piece::setName(pieceType tp, int tm){
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
            else if(tm == BLACK){
                c = toupper(c);
                name = " \\/ ";
                name.insert(name.begin()+2, c);
            }
            else if(tm == WHITE){
                c = tolower(c);
                name = " /\\ ";
                name.insert(name.begin()+2, c);
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


    //Other Piece class functions
        void Piece::printName(int i, int j){//This prints the piece name to the console, or if the location is empty, prints stars or spaces to match the square, but only if the argument corresponds to the correct location.
            char r = '0' + i;
            char f = 'a' + j;

            if(f == file && r == rank){
                if(team == EMPTY && (i+j) % 2 == DARK)
                    std::cout << "     ";
                else
                    std::cout << name;
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
        void Piece::subtractTimeMoved(){
            timesMoved--;
            return;
        }
        void Piece::subtractTimeSinceMoved(){
            timeSinceMoved--;
            return;
        }
        void Piece::restoreTimeSinceMoved(int i){
            timeSinceMoved = i;
            return;
        }
        void Piece::changeType(pieceType newType){
            type = newType;
            return;
        }

//OTHER FUNCTION DEFINITIONS///////////////////////////////
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

    //This function prints the board and the pieces at their locations on the board
    void printBoard(std::vector<Piece> &pieces)
    {
        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n    ";
        for(char a = 'a'; a <= 'h'; a++)
            std::cout << "   (" << a << ")  ";

        for(int i = NUM_RANKS; i > 0; i--)
        {
            for(int row = 0; row < ROWS_PER_RANK; row++)
            {
                std::cout << std::endl;
                if(row == 0)
                {
                    std::cout << "    ";
                    for(int j = 0; j < NUM_FILES; j++)
                    {
                        std::cout << "*-------"; //This prints the top edge of each row
                    }
                    std::cout << "*";
                }
                else if(row != 2)
                {
                    std::cout << "    ";
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
                    std::cout << "(" << i << ") ";
                    for(int j = 0; j < NUM_FILES; j++) //This prints the center third of each square, and any piece that may be on it
                    {
                        if((i+j) % 2 == LIGHT)
                        {
                            std::cout << "|*";
                            for(int k = 0; k < MAX_SQUARES; k++)
                                pieces[k].Piece::printName(i, j);
                            
                            std::cout << "*";
                        }
                        else if((i+j) % 2 == DARK)
                        {
                            std::cout << "| ";
                            for(int k = 0; k < MAX_SQUARES; k++)
                                pieces[k].Piece::printName(i, j);

                            std::cout << " ";
                        }
                    }
                    std::cout << "| (" << i << ") ";
                }
            }
        }

        std::cout << std::endl;
        std::cout << "    ";
        for(int j = 0; j < NUM_FILES; j++)
        {
            std::cout << "*-------";
        }
        std::cout << "*";

        std::cout << "\n    ";
        for(char a = 'a'; a <= 'h'; a++)
            {std::cout << "   (" << a << ")  ";}

        std::cout << "\n\n\n" << std::endl;
        
        return;
    }

    //This function returns the index of the Piece object in pieces that exists at a location and where team != EMPTY. If no such piece exists, it returns -1.
    int findPiece(std::vector<Piece> &pieces, std::string location)
    {
        for(int i = 0; i < MAX_SQUARES; i++)
        {
            if(pieces[i].Piece::getTeam() != EMPTY && pieces[i].Piece::getLocation() == location)
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
            if(pieces[i].Piece::getTeam() != EMPTY && pieces[i].Piece::getLocation() == location)
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
            if(pieces[i].Piece::getTeam() == EMPTY && pieces[i].Piece::getLocation() == location)
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
            if(pieces[i].Piece::getTeam() == color && pieces[i].Piece::getType() == KING)
            {
                return i;
            }
        }

        return -1;
    }

    //This function performs the entered move, if possible. This is a compound function that calls several other functions to determine the validity of the move before performing it.
    bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, int turn) 
    {   
        std::vector<Piece> oldPieces = pieces;
        int i = findPiece(pieces, start);
        int p2{-1};
        int blank{-1};
        bool isPawn{false};

        if(i == -1)
            return false;

        if(pieces[i].Piece::getType() == PAWN)
        {
            p2 = findPiece(pieces, end[0], start[1]);
            blank = findEmpty(pieces, end);
            isPawn = true;
            if(p2 != -1)
            {
                if(pieces[p2].Piece::getType() == PAWN && pieces[p2].Piece::getEPVul() == true)
                {
                    pieces[p2].Piece::setLocation(end);
                    pieces[blank].Piece::setLocation(end[0], start[1]);
                    pieces[i].Piece::setCanEP(true);
                }
                else
                    p2 = -1;
            }
        }
        int j = findPiece(pieces, end);

        bool isLegal = checkLegal(pieces, start, end, turn); //This determines whether or not an attempted move follows the rules of movement
        if(isLegal == false)
        {
            pieces = oldPieces;
            return false;
        }

        removePiece(pieces, j);
        j = findEmpty(pieces, end);

        pieces[i].Piece::setLocation(end);
        pieces[j].Piece::setLocation(start);

        int oppoTurn = -1 * turn;
        int k = findKing(pieces, turn);

        pieces[i].Piece::addTimeMoved();

        for(int index = 0; index < MAX_SQUARES; index++)
        {
            if(pieces[index].Piece::getTeam() == turn)
                pieces[index].Piece::addTimeSinceMoved();
        }

        pieces[i].Piece::resetTimeSinceMoved();

        int goodCastle{-5};

        bool isSafe{true};

        for(int test = 0; test < MAX_SQUARES; test++)
        {
            if(checkCanCapture(pieces, pieces[test].Piece::getLocation(), pieces[k].Piece::getLocation(), oppoTurn) == true)
                isSafe = false;

            if(pieces[i].Piece::getIsCastling() == true) //When castling, a few more rules apply. If all conditions are met, this returns the index of the rook that needs to be moved
                goodCastle = getCastlingRookIndex(pieces, i, j, start, oppoTurn);

            if(isLegal == false || isSafe == false || goodCastle == -1)
            {
                pieces = oldPieces;
                return false;
            }
        }

        if(goodCastle >= 0)
        {
            std::string rookSpot = start;
            rookSpot[0] = (start[0]+end[0])/2;
            pieces[goodCastle].Piece::setLocation(rookSpot);
            pieces[goodCastle].Piece::addTimeMoved();
            pieces[goodCastle].Piece::resetTimeSinceMoved();
        }

        if(pieces[k].Piece::getTimesMoved() != 0)
        {
            for(int all = 0; all < MAX_SQUARES; all++)
            {
                if(pieces[all].Piece::getTeam() == turn)
                {
                    pieces[all].Piece::setCanCastle(false);
                }
            }
        }

        if(isPawn == true && (end[1] - start[1])*turn == 2)
            pieces[i].Piece::setEPVul(true);

        return true;
    }

    void removePiece(std::vector<Piece> &pieces, int i) //This function lets you change a piece's team to EMPTY, thus removing it from sight
    {
        pieces[i].Piece::setTeam(CAPTURE_INDEX);
        pieces[i].Piece::setType(CAPTURE_INDEX);
        pieces[i].Piece::setName(pieces[i].Piece::getType(), pieces[i].Piece::getTeam());

        return;
    }

    void printTurn(int turnNum, int color) //This function takes an argument BLACK or WHITE and the current turn number and returns a corresponding string
    {
        std::string p1 = "White";
        std::string p2 = "Black";

        if(color == WHITE)
        {
            std::cout << "\n" << turnNum << ". " << p1 << " to move. Please enter your move: ";
        }
        else                //Implies color == BLACK
        {
            std::cout << "\n" << turnNum << "... " << p2 << " to move. Please enter your move: ";
        }

        return;        
    }

    //This function determines if the attempted move is legal according to movement rules and returns a corresponding bool.
    bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn)
    {
        int i = findPiece(pieces, start);
        int j = findPiece(pieces, end);

        if(start == end) //You can't skip your turn
            return false;
        else if(i == -1) //You can't move a non-existent piece
            return false;
        else if(pieces[i].Piece::getTeam() != turn) //You can't move if it's not your turn
            return false;
        else if(pieces[j].Piece::getTeam() == turn) //You can't move to a square occupied by another one of your pieces
            return false;

        if(pieces[i].Piece::getType() != KNIGHT) //Only knights can jump over other pieces
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

        if(pieces[i].Piece::getType() == PAWN)
        {   
            if((end[1] - start[1])*turn > 2)
                return false;
            else if((end[1] - start[1])*turn == 2 && end[0] != start[0])
                return false;
            else if((end[1] - start[1])*turn != 1 && pieces[i].Piece::getTimesMoved() != 0)
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
        else if(pieces[i].Piece::getType() == KING)
        {
            if(end[0] - start[0] > 1 && pieces[i].Piece::getIsCastling() == false)
                return false;
            else if(end[0] - start[0] < -1 && pieces[i].Piece::getIsCastling() == false)
                return false;
            else if(end[1] - start[1] > 1 || start[1] - end[1] > 1)
                return false;
            else if(end[0] - start[0] > 2 || start[0] - end[0] > 2)
                return false;
        }
        else if(pieces[i].Piece::getType() == QUEEN)
        {
            if(isDiagonal(start, end) == false && (start[0] != end[0] && start[1] != end[1]))
                return false;
        }
        else if(pieces[i].Piece::getType() == BISHOP)
        {
            if(isDiagonal(start, end) == false)
                return false;
        }
        else if(pieces[i].Piece::getType() == ROOK)
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
        else if(pieces[i].Piece::getTeam() != turn) //You can't move if it's not your turn
            return false;
        else if(pieces[j].Piece::getTeam() == turn) //You can't move to a square occupied by another one of your pieces
            return false;

        if(pieces[i].Piece::getType() != KNIGHT) //Only knights can jump over other pieces
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

        if(pieces[i].Piece::getType() == PAWN) //The movement rules for pawns are different when capturing than when moving to an empty square
        {
            if(start[0] == end[0])
                return false;
            else if(end[0] - start[0] != 1 && start[0] - end[0] != 1)
                return false;
            else if((end[1] - start[1])*turn != 1)
                return false;
        }
        else if(pieces[i].Piece::getType() == KING)
        {
            if(end[0] - start[0] > 1 || end[0] - start[0] < -1)
                return false;
            else if(end[1] - start[1] > 1 || start[1] - end[1] > 1)
                return false;
        }
        else if(pieces[i].Piece::getType() == QUEEN)
        {
            if(isDiagonal(start, end) == false && (start[0] != end[0] && start[1] != end[1]))
                return false;
        }
        else if(pieces[i].Piece::getType() == BISHOP)
        {
            if(isDiagonal(start, end) == false)
                return false;
        }
        else if(pieces[i].Piece::getType() == ROOK)
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

    int getCastlingRookIndex(std::vector<Piece> &pieces, int i, int j, std::string start, int oppoTurn)
    {
        int rook{-5};
        if(pieces[i].Piece::getFile() == 'g')
        {
            std::string breadcrumb = start;
                breadcrumb[0]++;
            rook = findPiece(pieces, 'h', start[1]);

            for(char c = 'e'; c < 'h'; c++)
            {
                if(findPiece(pieces, c, pieces[i].Piece::getRank()) != -1 && findPiece(pieces, c, pieces[i].Piece::getRank()) != i) //All spaces between the king and the rook must be empty
                    return -1;
                else if(j != -1) //This means there cannot be a piece there to capture
                    return -1;
                else if(rook == -1 || pieces[rook].Piece::getTimesMoved() != 0) //The rook must not have moved
                    return -1;
                else if(pieces[i].Piece::getTimesMoved() != 1) //The king also must not have moved, however, when this function is called in movePiece, the change to timesMoved will have already been applied
                    return -1;
                else
                {
                    for(int stopper = 0; stopper < MAX_PIECES; stopper++)
                    {
                        if(checkCanCapture(pieces, pieces[stopper].Piece::getLocation(), start, oppoTurn) == true || checkCanCapture(pieces, pieces[stopper].Piece::getLocation(), breadcrumb, oppoTurn) == true) //You cannot castle out of check or through check
                            return -1;
                    }
                }
            }
        }
        if(pieces[i].Piece::getFile() == 'c')
        {
            std::string breadcrumb = start;
                breadcrumb[0]--;
            rook = findPiece(pieces, 'a', start[1]);

            for(char c = 'e'; c > 'a'; c--)
            {
                if(findPiece(pieces, c, pieces[i].Piece::getRank()) != -1 && findPiece(pieces, c, pieces[i].Piece::getRank()) != i) //All spaces between the king and the rook must be empty
                    return -1;
                else if(j != -1) //This means there cannot be a piece there to capture
                    return -1;
                else if(rook == -1 || pieces[rook].Piece::getTimesMoved() != 0) //The rook must not have moved
                    return -1;
                else if(pieces[i].Piece::getTimesMoved() != 1) //The king also must not have moved, however, when this function is called in movePiece, the change to timesMoved will have already been applied
                    return -1;
                else
                {
                    for(int stopper = 0; stopper < MAX_PIECES; stopper++)
                    {
                        if(checkCanCapture(pieces, pieces[stopper].Piece::getLocation(), start, oppoTurn) == true || checkCanCapture(pieces, pieces[stopper].Piece::getLocation(), breadcrumb, oppoTurn) == true) //You cannot castle out of check or through check
                            return -1;
                    }
                }
            }
        }

        return rook;
    }

    // This function determines if the current player has at least one legal move
    int countLegalMoves(std::vector<Piece> &pieces, int turn) 
    {
        int moves{0};
        std::vector<Piece> oldPieces = pieces;

        for(int i = 0; i < MAX_SQUARES; i++)
        {
            for(int j = 0; j < MAX_SQUARES; j++)
            {
                if(pieces[i].Piece::getTeam() != turn){
                    continue;
                }
                else if(pieces[j].Piece::getTeam() == turn){
                    continue;
                }
                std::string s = pieces[i].Piece::getLocation();
                std::string e = pieces[j].Piece::getLocation();
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

        std::cout << "Number of possible moves = " << moves << std::endl; // This piece is useful
        return moves;
    }