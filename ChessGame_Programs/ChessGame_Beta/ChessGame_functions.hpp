// /////////////////////////
// ChessGame.hpp
// /////////////////////////

#ifndef CHESSGAME_FILES_HPP
#define CHESSGAME_FILES_HPP

#include<iostream> // std::cout, cin, etc
#include<string> // std::string, string functions
#include<vector> // std::vector, vector operations
#include<math.h> // operators
#include<algorithm> // std::count

#define BLACK -1
#define WHITE 1
#define EMPTY 0

// This line is useful. You can copy and paste this text if you need to uncomment it.
// std::cout << "Number of possible moves = " << moves << std::endl; 

// contains piece name, location, and team information, along with some other data for special cases
class Piece
{
protected:
    char name;
    char file;
    char rank;
    std::string location;
    int pieceID;
    int team;
    int timesMoved;
    bool EPVul;
    int lastMoved;
    bool castling;
    bool canCastle;
    char canEP;
public: 
    // ctor for object of Piece class
    Piece(char f, char r, int ID) {
        rank = r;
        file = f;
        setLocation(rank, file);
        pieceID = ID;
        setName(pieceID);
        setTeam(pieceID);
        timesMoved = 0;
        setEPVul();
        setCastling(false);
        canEP = '0';
    }

    void setRankAndFile(std::string l){
        file = l[0];
        rank = l[1];
        location = l;
    }
    void setRankAndFile(char f, char r){
        file = f;
        rank = r;
        location[0] = f;
        location[1] = r;
    }

    void setName(int ID) {
        if(ID == 1 || ID == 8)
            name = 'r';
        else if(ID == 2 || ID == 7)
            name = 'n';
        else if(ID == 3 || ID == 6)
            name = 'b';
        else if(ID == 4)
            name = 'q';
        else if(ID == 5)
            name = 'k';
        else if(ID > 8 && ID <= 16)
            name = 'p';
        else if(ID == 17 || ID == 24)
            name = 'R';
        else if(ID == 18 || ID == 23)
            name = 'N';
        else if(ID == 19 || ID == 22)
            name = 'B';
        else if(ID == 20)
            name = 'Q';
        else if(ID == 21)
            name = 'K';
        else if(ID > 24 && ID <= 32)
            name = 'P';
        else
            name = ' ';

        return;
    }

    void promotePawn(char n) {
        name = n;
        return;
    }

    void setCanEP(char c){
        canEP = c;
    }

    char getCanEP(){
        return canEP;
    }

    char getName(){
        return name;
    }

    char getRank(){
        return rank;
    }
    char getFile(){
        return file;
    }

    std::string getLocation() {
        return location;
    }
    void setLocation(char r, char f) {
        location = "";
        location.push_back(r);
        location.push_back(f);
        return;
    }

    void setTeam(int ID) {
        if(ID >= 1 && ID <= 16)
            team = WHITE;
        else if(ID >= 17 && ID <= 32)
            team = BLACK;
        else
            team = EMPTY;

        return;
    }

    int getTeam() {
        return team;
    } 

    void addTimeMoved(){
        timesMoved++;
        return;
    }

    void addTimeMoved(int fix){
        timesMoved += fix;
    }

    int getTimesMoved() {
        return timesMoved;
    }
    
    void setEPVul() {
        EPVul = false;
        return;
    }
    void setEPVul(bool vul) {
        EPVul = vul;
        return;
    }
    void setEPVul(int last){
        if(last != 0)
        EPVul = false;
    }

    bool getEPVul() {
        return EPVul;
    }

    void setLastMoved(int last){
        lastMoved = 0;
    }

    void setLastMoved(){
        lastMoved++;
    }

    int getLastMoved(){
        return lastMoved;
    }

    void setCastling(bool oo){
        castling = oo;
    }

    bool getCastling(){
        return castling;
    }


};

// This class contains the information about a square on the chessboard. It contains the name of the piece occupying it, if any, its location and its status as empty or not empty
class Square
{
public:
    Square(char r, char f, std::vector<Piece> &p) {
        rank = r;
        file = f;
        setLocation(r, f);
        pieces = p;
    }
    void setLocation(char r, char f) {
        location = "";
        location.push_back(f);
        location.push_back(r);
        return;
    }
    std::string getLocation(){
        return location;
    }
    std::vector<Piece> pieces;

    // This getter function searches the vector 'pieces' for what piece, if any, shares the same location data. If that piece exists, the fnction returns the piece's name. Else, it returns ' '.
    char getPiece(std::vector<Piece> pieces) {
        
        static int i{0};
        for(i = 0; i < 32; i++)
        {
            if(pieces[i].Piece::getLocation() == location)
            {
                setFull(true);
                return pieces[i].Piece::getName();
            }
            else
                continue;
        }
        setFull(false);
        return ' ';
    }
    void setFull(bool nonempty){
        isFull = nonempty;
    }
    bool full(){
        return isFull;
    }


protected:
    std::string location;
    char file;
    char rank;
    bool isFull;
};

int printBoard(char spaces[], char pieceName);
bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn, bool testCheck, bool captureHere);
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, bool test, int turn);
bool isDiagonal(std::string start, std::string end);
bool checkSafeMove(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
bool checkNotCheck(std::vector<Piece> &pieces, int turn);
int canMove(std::vector<Piece> &pieces, std::vector<Square> squares, int turn);
char checkPiece(std::vector<Piece> pieces, std::string l);
int checkTeam(std::vector<Piece> pieces, std::string l);
std::string createBoardState(char allSpaces[], int turn, std::vector<Piece> pieces);
// bool checkCanEP(std::vector<Piece> pieces, int x, std::string end);

#endif