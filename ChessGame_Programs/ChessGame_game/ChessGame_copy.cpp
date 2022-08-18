// ////////////////////
// ChessGame.cpp
// Copyright © 2020 
// Theodore Shore
// All Rights reserved
// ////////////////////

#include<iostream> // std::cout, cin, etc
#include<string> // std::string, string functions
#include<vector> // std::vector
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
bool checkNotCheck(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
bool checkNotCheck(std::vector<Piece> &pieces, int turn);
int canMove(std::vector<Piece> &pieces, std::vector<Square> squares, int turn);
char checkPiece(std::vector<Piece> pieces, std::string l);
int checkTeam(std::vector<Piece> pieces, std::string l);
std::string boardState(char allSpaces[], int turn);
bool checkCanEP(std::vector<Piece> pieces, int x, std::string end);
bool isAdjacent(std::vector<Piece> pieces, std::string location, char c);

int main(void)
{
    std::vector<Piece> pieces;
    std::vector<std::string> boardStates;

    for(int i = 16; i < 24; i++)
    {
        char rank{'8'};
        static char file{'a'};
        Piece temp(rank, file, i+1);
        pieces.push_back(temp);

        file++;
    }

    for(int i = 24; i < 32; i++)
    {
        char rank{'7'};
        static char file{'a'};
        Piece temp(rank, file, i+1);
        pieces.push_back(temp);

        file++;
    }

    for(int i = 0; i < 8; i++)
    {
        char rank{'1'};
        static char file{'a'};
        Piece temp(rank, file, i+1);
        pieces.push_back(temp);

        file++;
    }

    for(int i = 8; i < 16; i++)
    {
        char rank{'2'};
        static char file{'a'};
        Piece temp(rank, file, i+1);
        pieces.push_back(temp);

        file++;
    }
    
    char spaces[64];
    std::vector<Square> squares;
    int ocho2{0};
    for(char c1 = '8'; c1 > '0'; c1--)
    {
        for(char c0 = 'a'; c0 < 'i'; c0++)
        {
            Square new_square(c1, c0, pieces);
            squares.push_back(new_square);
            spaces[ocho2] = new_square.getPiece(pieces);
            ocho2++;
        }
    }
/*
    Square capture1('0', 'A', pieces);
    spaces[64] = capture1.getPiece(pieces);
    squares.push_back(capture1);
*/
    printBoard(spaces, '!');
    std::cout << "\n1. White to move. Please enter your move: ";
    static int turn{WHITE};
    static unsigned long int turnNumber{1};
    std::string firstState = boardState(spaces, WHITE);
    boardStates.push_back(firstState);
    do
    {
        std::string start, end;
        std::cin >> start;
        std::cin >> end;
        bool legal = false;

        static int x{0};
        for(x = 0; x < 32; x++)
        {
            if(start[1] == '0'){
                x = 32;
                break;
            }
            else if(pieces[x].Piece::getLocation() == start)
            {
                break;
            }
            else if(x < 31)
                continue;
            else
                x = 32;
            break;
        }

        checkNotCheck(pieces, start, end, turn);

        
        if(checkLegal(pieces, start, end, turn, false, false) == false)
        {
            printBoard(spaces, '!');
            std::cout << "Illegal move!" << std::endl;
            legal = false;
        }
        else if(x == 32)
        {
            printBoard(spaces, '!');
            std::cout << "I don't see a piece there. " << std::endl; 
            legal = false;
        }
        else if(pieces[x].Piece::getTeam() != turn)
        {
            printBoard(spaces, '!');
            std::cout << "It's not your turn! " << std::endl; 
            legal = false;
        }
        else if(checkNotCheck(pieces, start, end, turn) == false)
        {
            printBoard(spaces, '!');
            if(pieces[x].Piece::getCastling() == false)
            std::cout << "That would be check! " << std::endl;
            else if(pieces[x].Piece::getCastling() == true)
            std::cout << "You can't castle out of check. You can't castle through check. " << std::endl;
            legal = false;
        }
        else
        legal = true;
        
        if(legal == false)
        {
            std::cout << "Try again! ";
            if(turn == BLACK)
            {
                std::cout << turnNumber << "... Black to move: ";
            }
            else
            {
                std::cout << turnNumber << ". White to move: ";
            }
            continue;
        }
        else if(legal == true)
        { 
            legal = movePiece(pieces, start, end, false, turn);
        }

        static int rook{0};
        if(pieces[x].Piece::getName() == 'k' && pieces[x].Piece::getCastling() == true)
        {
            std::string rookPlace;
            if(end == "g1")
            {
                rookPlace = "f1";
                for(rook = 0; rook < 32; rook++)
                {
                    if(pieces[rook].Piece::getLocation() == "h1")
                    break;
                    else if(rook < 31)
                    continue;
                }
                
            }
            else if(end == "c1")
            {  
                rookPlace = "a1";
                for(rook = 0; rook < 32; rook++)
                {
                    if(pieces[rook].Piece::getLocation() == "a1")
                    break;
                    else if(rook < 31)
                    continue;
                }
            }
            if(rook < 32) {
                movePiece(pieces, pieces[rook].Piece::getLocation(), rookPlace, false, turn);
                pieces[rook].Piece::addTimeMoved();
            }
        }
        if(pieces[x].Piece::getName() == 'K' && pieces[x].Piece::getCastling() == true)
        {
            std::string rookPlace;
            if(end == "g8")
            {
                rookPlace = "f8";
                for(rook = 0; rook < 32; rook++)
                {
                    if(pieces[rook].Piece::getLocation() == "h8")
                    break;
                    else if(rook < 31)
                    continue;
                }
                
            }
            else if(end == "c8")
            {  
                rookPlace = "a8";
                for(rook = 0; rook < 32; rook++)
                {
                    if(pieces[rook].Piece::getLocation() == "a8")
                    break;
                    else if(rook < 31)
                    continue;
                }
            }
            if(rook < 32) {
                movePiece(pieces, pieces[rook].Piece::getLocation(), rookPlace, false, turn);
                pieces[rook].Piece::addTimeMoved();
            }
        }
        
        char temp[64];

        for(ocho2 = 0; ocho2 < 64; ocho2++)
        {
            temp[ocho2] = squares[ocho2].getPiece(pieces);
        }

        for(int i = 0; i < 64; i++)
        {
            spaces[i] = temp[i];
        }

        int turnsTilDraw = printBoard(spaces, pieces[x].Piece::getName());
        turn *= -1;

        if((pieces[x].Piece::getName() == 'k' || pieces[x].Piece::getName() == 'K' || pieces[x].Piece::getName() == 'r' || pieces[x].Piece::getName() == 'R' || pieces[x].Piece::getName() == 'p' || pieces[x].Piece::getName() == 'P') && pieces[x].getTimesMoved() == 1)
        {
            boardStates.clear(); // When any of these pieces moves for th first time, it permanently changes the state of the board, thus, there is no need to keep old boardState ID's, and in the case of the king and rook, it's actually detrimental to keep them.
        }
        std::string newState;
        do{
        // if(turn == BLACK && pieces[])
        
        newState = boardState(spaces, turn);
        boardStates.push_back(newState);

        if(checkCanEP(pieces, x, end) == true){
            std::cout << "En passant possible! " << std::endl;
            boardStates.pop_back();
        }
        }while(0);
        int repCount = std::count (boardStates.begin(), boardStates.end(), newState);

        std::cout << "This position has now occurred " << repCount << " time";
        if(repCount == 1){std::cout << ". " << std::endl;}
        else{std::cout << "s. " << std::endl;}
        std::cout << newState <<std::endl;

        if(repCount >= 3)
        {
            std::cout << "Threefold repetition rule invoked! It's a draw! " << std::endl;
            break;
        }

        int notMate{0};
        notMate = canMove(pieces, squares, turn);

            if(checkNotCheck(pieces, turn) == false && notMate > 0)
            {
                std::cout << "Check!" << std::endl;
            }
            else if(checkNotCheck(pieces, turn) == true && notMate > 0)
            {
                std::cout << std::endl;
            }
            else if(checkNotCheck(pieces, turn) == false && notMate == 0)
            {
                std::string winner;
                if(turn == BLACK)
                {
                    winner = "White ";
                }
                else if(turn == WHITE)
                {
                    winner = "Black ";
                }
                std::cout << "Checkmate! " << winner << "wins! " << std::endl;
                break;
            }
            else if(checkNotCheck(pieces, turn) == true && notMate == 0)
            {
                std::cout << "No legal moves! Stalemate! ";
                break;
            }
            
            if(turnsTilDraw == 0)
                break;
            else if(turn == BLACK)
            {
                std::cout << turnNumber << "... Black to move. ";
                for(int i = 0; i < 32; i++)
                {
                    pieces[i].Piece::setEPVul(pieces[i].getLastMoved());
                }
            }
            else if(turn == WHITE)
            {
                turnNumber++;
                std::cout << turnNumber << ". White to move. ";
                for(int i = 0; i < 32; i++)
                {
                    pieces[i].Piece::setEPVul(pieces[i].getLastMoved());
                }
            }
            
            std::cout << "Please enter your move: ";

    }while(1);

    std::cout << "Game Over. Goodbye! \n" <<std::endl;

    return 0;

}

// This function prints the chessboard after each move or attempted move. It also keeps track of the conditions necessary to trigger the 50-move draw condition and returns an integer reflecting how many ply (moves by one player AKA half moves) remain before the draw condition must either be triggered or delayed
int printBoard(char spaces[64], char pieceName) 
{
    std::cout << "\n\n\n\n\n" << std::endl;
    static int turnsTilDraw{100};
    int piecesLeft{64};
    
    for(int j = 0; j < 8; j++)
    {
        if(j == 0)
        {
            for(int i = 0; i < 6; i++) {std::cout << std::endl;}
            for(char c = 'a'; c < 'i'; c++)
            {
                if(c == 'a') {std::cout << "    ";}
                std::cout << "   (" << c << ")  ";
                if(c == 'h') {std::cout << std::endl;}
            }
        }
        for(int i = 0; i < 8; i++)
        {
            if(i == 0) {std::cout << "    ";}
            std::cout << "*-------";
            if(i == 7) {std::cout << "*" << std::endl;}
        }

        for(int i = 0; i < 8; i++)
        {
            if(i == 0) {std::cout << "    ";}
            std::cout << "|";

            if((i+j) % 2 == 0)
                std::cout << "*******";
            else
                std::cout << "       ";

            if(i == 7)
                std::cout << "|" << std::endl;
        }

        for(int i = 0; i < 8; i++)
        {
            if(i == 0)
            {
                int k = 8-j;
                std::cout << "(" << k << ") ";
            }

            std::cout << "|";

            char c = spaces[i+8*j];
            char d = ' ';
            char e = ' ';
            char f = ' ';
            if(c == ' ')
                piecesLeft--;

            if(c == ' ' && ((i+j) % 2 == 0)) 
            {
                c = '*';
                d = c;
                e = c;
                f = c;
            }
            else if(c <= 'z' && c >= 'a')
            {
                d = '/';
                e = '\\';
            }
            else if(c <= 'Z' && c >= 'A')
            {
                d = '\\';
                e = '/';
            }

            if((i+j) % 2 == 0)
                std::cout << "*" << f << d << c << e << f << "*";
            else
                std::cout << "  " << d << c << e << "  ";
        
            if(i == 7)
                std::cout << "|" << std::endl;
        }

        for(int i = 0; i < 8; i++)
        {
            if(i == 0) {std::cout << "    ";}
            std::cout << "|";

            if((i+j) % 2 == 0)
                std::cout << "*******";
            else
                std::cout << "       ";

            if(i == 7)
                std::cout << "|" << std::endl;
        }

        if(j == 7)
        {
            for(int i = 0; i < 8; i++)
            {
                if(i == 0) {std::cout << "    ";}
                std::cout << "*-------";
            }

            std::cout << "*\n";

            for(char c = 'a'; c < 'i'; c++)
            {
                if(c == 'a') {std::cout << "    ";}
                std::cout << "   (" << c << ")  ";
                if(c == 'h') {std::cout << std::endl;}
            }

            std::cout << "\n\n" << std::endl;

        }
    }
    static int piecesPrevious{32}; // This is a static int, while piecesLeft is not because I want the function to remember the value of piecesPrevious in between function calls, while I need piecesLeft to reset every time
    
    if(pieceName == '!')
    {
        return turnsTilDraw;
    }
    else if(pieceName == 'p' || pieceName == 'P')
    {
        turnsTilDraw = 100;
    }
    else if(piecesPrevious > piecesLeft)
    {
        turnsTilDraw = 100;
        piecesPrevious = piecesLeft;
    }
    else
    {
        turnsTilDraw--;
        if(turnsTilDraw == 0)
        {
            std::cout << "50-move rule invoked! It's a draw! " << std::endl;
        }
    }
    // std::cout << "turnsTilDraw = " << turnsTilDraw << std::endl;
    return turnsTilDraw;
}

// This function contains most of the rules that determine how each piece can move. Some elements that determine legality, such as being in check, are determined elsewhere
// The bool test is designed to prevent checkLegal from running the pawn promotion sequence if and when it tests moving a pawn to the far rank, while the captureHere bool is designed to distinguish between testing if pieces can perform a move normally and testing if pieces are ATTACKING the end square, i.e. if they could capture an opposing king on the end space. This only has an impact on pawns. 
bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn, bool test, bool captureHere)
{   
    // can't choose start or endpoints off the board
    if(start[0] < 'a' || start[0] > 'h' || start[1] < '0' || start[1] > '8')
        return false;
    
    if(end[0] < 'a' || end[0] > 'h' || end[1] < '1' || end[1] > '8')
        return false;

    int x{0};
    for(x = 0; x < 32; x++)
    {
        if(pieces[x].Piece::getLocation() == start)
            break;
        else if(x < 31)
            continue;
        else
            return false;
    }

    if(pieces[x].Piece::getName() != 'n' && pieces[x].Piece::getName() != 'N')
    {
        std::string vanguard = start; 
        do
        {
            if(vanguard[0] < 'a' || vanguard[0] > 'h')
                return false;
            else if(vanguard[1] < '1' || vanguard[1] > '8')
                return false;
            
            // if(test == false){std::cout << vanguard << " ";}
            if(vanguard[0] != end[0]){
                if(vanguard[0] < end[0]){
                    vanguard[0]++;
                }
                else if(vanguard[0] > end[0]){
                    vanguard[0]--;
                }
            }

            if(vanguard[1] != end[1])
            {
                if(vanguard[1] < end[1]){
                    vanguard[1]++;
                }
                else if(vanguard[1] > end[1]){
                    vanguard[1]--;
                }
            }
            Square guard(vanguard[1], vanguard[0], pieces);
            // if(test == false){std::cout << "move to " << vanguard << ". Vanguard contains " << guard.Square::getPiece(pieces);} 
            if(guard.Square::getPiece(pieces) != ' ' && vanguard != end){
                //if(test == false) {std::cout <<" and is " << "blocked! " << std::endl;}
                return false;
            } // if(test == false){std::cout << " and is not blocked!" << std::endl;}
            
        }while(vanguard != end);
    }
    

    int y{0};
    for(y = 0; y < 32; y++)
    {
        if(pieces[y].Piece::getLocation() == end)
            break;
        else if(y < 31)
            continue;
        else
            y = 32;
        break;
    }

    if(y < 32 && pieces[y].Piece::getTeam() == pieces[x].Piece::getTeam()) // Also cannot capture your own pieces. // This line of code has no problems.
        return false;

    bool pawnFwd{true};
    if(captureHere == true)
        pawnFwd = false;

    if(pieces[x].Piece::getName() == 'p') // basic rules for white pawns
    {
        int l{0};
        for(l = 0; l < 32; l++)
        {
            if(pieces[l].Piece::getLocation() == end)
                break;
            else if(l < 31)
                continue;
            else
                l = 32;
            break;
        }


        int k{0};
        for(k = 0; k < 32; k++)
        {
            if(pieces[k].Piece::getRank() == start[1] && end[0] == pieces[k].Piece::getFile())
            {
                if(pieces[k].Piece::getEPVul() == true)
                break;
                else
                continue;
            }
            else if(k < 31)
                continue;
            else
                k = 32;
            break;
        }

        if(end[1] - start[1] > 2 || end[1] - start[1] < 1)
        return false;
        else if(end[1] - start[1] == 2 && pieces[x].Piece::getTimesMoved() != 0)
        return false;
        else if(end[1] - start[1] == 2 && end[0] != start[0])
        return false;
        else if(end[0] - start[0] > 1 || start[0] - end[0] > 1)
        return false;
        else if(end[0] == start[0] && l != 32)
        return false;
        else if(end[0] == start[0])
        return pawnFwd; // This bool serves a complementary purpose to captureHere a few lines later. This else if statement allows the program to recognize that pawns do not protect the space directly in front of them, which is important for determining where the king is safe.
        else if(k != 32 && isDiagonal(start, end) == true)
        return true;
        else if(isDiagonal(start, end) == true && l == 32)
        return captureHere; // This bool reflects whether or not we are testing legality to prevent a king from moving to a space attacked by the pawn. Since there isn't an actual piece at end when the test is run, the pawn would normally not be able to move there, but we need to show that it can CAPTURE there
        
        
        if(end[1] == '8' && test == false) // Pawn Promotion. This should be the final condition for pawns.
        {
            std::cout << "Pawn Promotion! Please enter 'q', 'r', 'b', or 'n' to choose new value, or 'cancel' to cancel: ";
            std::string newPieceS = "000000";
            while(newPieceS == "000000")
            {
                std::cin >> newPieceS;
                if(newPieceS == "q" || newPieceS == "r" || newPieceS == "b" || newPieceS == "n")
                {
                    char newPiece = newPieceS[0];
                    pieces[x].Piece::promotePawn(newPiece);
                    return true;
                }
                else if(newPieceS == "cancel")
                    return false;
                else
                {
                    newPieceS = "000000";
                    std::cout << "Invalid input. Try again: ";
                    continue;
                }
            }
        }
    }
    else if(pieces[x].Piece::getName() == 'P') // basic rules for black pawns
    {
        int j{0};
        for(j = 0; j < 32; j++)
        {
            if(pieces[j].Piece::getLocation() == end)
                break;
            else if(j < 31)
                continue;
            else
                j = 32;
            break;
        }

        int k{0};
        for(k = 0; k < 32; k++)
        {
            if(pieces[k].Piece::getRank() == start[1] && end[0] == pieces[k].Piece::getFile())
            {
                if(pieces[k].Piece::getEPVul() == true)
                break;
                else
                continue;
            }
            else if(k < 31)
                continue;
            else
                k = 32;
            break;
        }
        
        if(start[1] - end[1] > 2 || start[1] - end[1] < 1)
        return false;
        else if(start[1] - end[1] == 2 && pieces[x].Piece::getTimesMoved() != 0)
        return false;
        else if(start[1] - end[1] == 2 && start[0] != end[0])
        return false;
        else if(end[0] - start[0] > 1 || start[0] - end[0] > 1)
        return false;
        else if(end[0] == start[0] && j != 32)
        return false;
        else if(end[0] == start[0])
        return pawnFwd; // This bool serves a complementary purpose to captureHere a few lines later. This else if statement allows the program to recognize that pawns do not protect the space directly in front of them, which is important for determining where the king is safe.
        else if(k != 32 && isDiagonal(start, end) == true)
        return true;
        else if(isDiagonal(start, end) == true && j == 32)
        return captureHere; // This bool reflects whether or not we are testing legality to prevent a king from moving to a space attacked by the pawn. Since there isn't an actual piece at end when the test is run, the pawn would normally not be able to move there, but we need to show that it can CAPTURE there
        

        if(end[1] == '1' && test == false) // Pawn Promotion. This should be the final condition for pawns.
        {
            std::cout << "Pawn Promotion! Please enter 'Q', 'R', 'B', or 'N' to choose new value, or 'cancel' to cancel: ";
            std::string newPieceS = "000000";
            while(newPieceS == "000000")
            {
                std::cin >> newPieceS;
                if(newPieceS == "Q" || newPieceS == "R" || newPieceS == "B" || newPieceS == "N")
                {
                    char newPiece = newPieceS[0];
                    pieces[x].Piece::promotePawn(newPiece);
                    return true;
                }
                else if(newPieceS == "cancel")
                    return false;
                else
                {
                    newPieceS = "000000";
                    std::cout << "Invalid input. Try again: ";
                    continue;
                }
            }
        }
    }
    else if(pieces[x].Piece::getName() == 'k' || pieces[x].Piece::getName() == 'K') // basic rules for kings
    {
        
        // basic rules for castling
        if((start == "e1" && end == "g1" && pieces[x].Piece::getTimesMoved() == 0)) // White castle kingside
        {
            for(int i = 0; i < 32; i++)
            {
                if(pieces[i].Piece::getLocation() == "f1" || pieces[i].Piece::getLocation() == "g1")
                    {return false;}
                else
                    continue;
            }
            int j{0};
            for(j = 0; j < 32; j++)
            {
                if(pieces[j].Piece::getLocation() == "h1")
                    break;
                else if(j < 31)
                    continue;
                else
                    j = 32;
                    break;
            }

            if(pieces[j].Piece::getName() != 'r' || pieces[j].Piece::getTimesMoved() != 0)
                return false;
        }
        else if((start == "e1" && end == "c1" && pieces[x].Piece::getTimesMoved() == 0)) // White castle queenside
        {
            for(int i = 0; i < 32; i++)
            {
                if(pieces[i].Piece::getLocation() == "b1" || pieces[i].Piece::getLocation() == "c1" || pieces[i].Piece::getLocation() == "d1")
                    return false;
                else
                    continue;
            }
            int j{0};
            for(j = 0; j < 32; j++)
            {
                if(pieces[j].Piece::getLocation() == "a1")
                    break;
                else if(j < 31)
                    continue;
                else
                    j = 32;
                    break;
            }

            if(pieces[j].Piece::getName() != 'r' || pieces[j].Piece::getTimesMoved() != 0)
                return false;
        }
        else if((start == "e8" && end == "g8" && pieces[x].Piece::getTimesMoved() == 0)) // Black castle kingside
        {
            for(int i = 0; i < 32; i++)
            {
                if(pieces[i].Piece::getLocation() == "f8" || pieces[i].Piece::getLocation() == "g8")
                    return false;
                else
                    continue;
            }
            int j{0};
            for(j = 0; j < 32; j++)
            {
                if(pieces[j].Piece::getLocation() == "h8")
                    break;
                else if(j < 31)
                    continue;
                else
                    j = 32;
                    break;
            }

            if(pieces[j].Piece::getName() != 'R' || pieces[j].Piece::getTimesMoved() != 0)
                return false;
        }
        else if((start == "e8" && end == "c8" && pieces[x].Piece::getTimesMoved() == 0)) // Black castle queenside
        {
            for(int i = 0; i < 32; i++)
            {
                if(pieces[i].Piece::getLocation() == "b8" || pieces[i].Piece::getLocation() == "c8" || pieces[i].Piece::getLocation() == "d8")
                    return false;
                else
                    continue;
            }
            int j{0};
            for(j = 0; j < 32; j++)
            {
                if(pieces[j].Piece::getLocation() == "a8")
                    break;
                else if(j < 31)
                    continue;
                else
                    j = 32;
                    break;
            }

            if(pieces[j].Piece::getName() != 'R' || pieces[j].Piece::getTimesMoved() != 0)
                return false;
        }
        else if((start[1] - end[1] > 1 || end[1] - start[1] > 1) || (start[0] - end[0] > 1 || end[0] - start[0] > 1))
        return false;
    }
    else if(pieces[x].Piece::getName() == 'r' || pieces[x].Piece::getName() == 'R')
    {
        start = pieces[x].Piece::getLocation();
        if(end[0] != start[0] && end[1] != start[1])
            return false;
    }
    
    else if(pieces[x].Piece::getName() == 'n' || pieces[x].Piece::getName() == 'N')
    {
        if((end[0] - start[0] == 1 && end[1] - start[1] == 2) || (start[0] - end[0] == 1 && end[1] - start[1] == 2))
        return true;
        else if((end[0] - start[0] == 2 && end[1] - start[1] == 1) || (start[0] - end[0] == 2 && end[1] - start[1] == 1))
        return true;
        else if((end[0] - start[0] == 2 && start[1] - end[1] == 1) || (start[0] - end[0] == 2 && start[1] - end[1] == 1))
        return true;
        else if((end[0] - start[0] == 1 && start[1] - end[1] == 2) || (start[0] - end[0] == 1 && start[1] - end[1] == 2))
        return true;
        else
        return false;
    }
    else if(pieces[x].Piece::getName() == 'b' || pieces[x].Piece::getName() == 'B')
    {
        if(isDiagonal(start, end) == false)
            return false;
        
    }
    else if(pieces[x].Piece::getName() == 'q' || pieces[x].Piece::getName() == 'Q')
    {
        if(start[0] == end[0] && start[1] == end[1]){
            return false;
        }
        else if((start[0] != end[0] && start[1] != end[1]) && isDiagonal(start, end) == false){
            return false;
        }
    }

        return true;

}

// This function can either alter the location values of Piece objects or return a bool used to help determine the legality of a move or the ability to move at all
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, bool test, int turn)
{
    static int i{0};
    bool gotcha{false};
    bool gotcha2{false};
    std::string oldPlace{"M0"};
    bool NotCheck{true};
    int oppoTurn;
    static int rook{0};
    static char captureR{'0'};
    static char captureF{'N'};

    
    

    for(i = 0; i < 32; i++)
    {
        if(pieces[i].Piece::getLocation() == start)
        {
            oppoTurn = -1 * turn;
            NotCheck = checkNotCheck(pieces, turn);
            break;
        }
        else
            continue;
    }

    static int j{0};
    for(j = 0; j < 32; j++)
    {
        if(pieces[j].Piece::getLocation() == end)
            break;
        else if(j < 31)
            continue;
        else
            j = 32;
    }

    if(j != 32)
    {
        gotcha = true;
        std::string captureBox = "M0";
        oldPlace = pieces[j].getLocation();
        if(test == true){
        pieces[j].Piece::setRankAndFile(captureBox);
        }
        else
        {
            pieces[j].Piece::setRankAndFile(captureF, captureR);
            if(captureR == '9')
            {
                captureF++;
                captureR = '0';
            }
            else
            {
                captureR++;
            }
            
        }
        
    }

    pieces[i].Piece::setRankAndFile(end);

    if((pieces[i].Piece::getName() == 'p' && end[1] - start[1] == 2 ) || (pieces[i].Piece::getName() == 'P' && start[1] - end[1] == 2))
        pieces[i].Piece::setEPVul(true);
    
    if((pieces[i].Piece::getName() == 'p' || pieces[i].Piece::getName() == 'P') && isDiagonal(start, end) == true) {
       static int k{0};
        for(k = 0; k < 32; k++)
        {
            if(pieces[k].Piece::getRank() == start[1] && end[0] == pieces[k].Piece::getFile())
            {
                if(pieces[k].Piece::getEPVul() == true)
                break;
                else
                continue;
            }
            else if(k < 31)
                continue;
            else
                k = 32;
            break;
        }
            std::string captureBox = "M0";
            if(k != 32){
                gotcha = true;
                oldPlace = pieces[k].getLocation();
                if(test == true)
                {
                    pieces[k].Piece::setRankAndFile(captureBox);
                }
                else
                {
                    pieces[k].Piece::setRankAndFile(captureF, captureR);
                    if(captureR == '9')
                    {
                        captureF++;
                        captureR = '0';
                    }
                    else
                    {
                        captureR++;
                    }
            
                }
            }        
    }

    NotCheck = checkNotCheck(pieces, turn);

    if(test == false)
    {
        pieces[i].Piece::addTimeMoved();

        for(int t = 0; t < 32; t++)
        {
            pieces[t].Piece::setLastMoved();
        }
        pieces[i].Piece::setLastMoved(0);
    }

    if(test == true)
    {
        pieces[i].Piece::setRankAndFile(start);
        if(gotcha == true)
        {
            for(int p = 0; p < 32; p++)
            {
                if(pieces[p].Piece::getLocation() == "M0"){
                    pieces[p].Piece::setRankAndFile(oldPlace);
                    break;
                }
                else
                    continue;
            }
        }
        else if(gotcha2 == true)
        {
            pieces[rook].Piece::setRankAndFile(oldPlace);
        }
    }

    return NotCheck;

}

// This function returns a bool representing whether or not two squares are diagonal from each other
bool isDiagonal(std::string start, std::string end)
{
    if((end[0] - start[0] != end[1] - start[1]) && (end[0] - start[0] != start[1] - end[1]))
        return false;
    else
        return true;
}

bool checkNotCheck(std::vector<Piece> &pieces, std::string start, std::string end, int turn) // This function determines if a specific move would put the current player in check. It also returns false if the player attempts to castle while in check, or in a way that would cause them to pass through check.
{
    char c;
    if(turn == BLACK){
        c = 'K';
    }
    else if(turn == WHITE){
        c = 'k';
    }

    if(checkLegal(pieces, start, end, turn, true, false) == false){
        return false;
    }

    static int x{0};
    for(x = 0; x < 32; x++)
    {
        if(pieces[x].Piece::getName() == c)
            break;
        else if(x < 31){
            pieces[x].Piece::setCastling(false);
            continue;
        }    
        else
            x = 32;
        break;  
    }

    if(start == "e1" && end == "g1"){
        pieces[x].Piece::setCastling(true);
    }
    else if(start == "e1" && end == "c1"){
        pieces[x].Piece::setCastling(true);
    }
    else if(start == "e8" && end == "g8"){
        pieces[x].Piece::setCastling(true);
    }
    else if(start == "e8" && end == "c8"){
        pieces[x].Piece::setCastling(true);
    }
    else{
        pieces[x].Piece::setCastling(false);
    }

    if(x != 32){
        if(end == "g1" && pieces[x].Piece::getCastling() == true)
        {
            if(checkNotCheck(pieces, turn) == false)
                return false;
            else if(movePiece(pieces, start, "f1", true, turn) == false){
                return false;
            }
        }
        else if(pieces[x].Piece::getCastling() == true && end == "c1")
        {
            if(checkNotCheck(pieces, turn) == false)
                return false;
            else if(movePiece(pieces, start, "d1", true, turn) == false){
                return false;
            }
        }
        else if(pieces[x].Piece::getCastling() == true && end == "g8")
        {
            if(checkNotCheck(pieces, turn) == false)
                return false;
            else if(movePiece(pieces, start, "f8", true, turn) == false){
                return false;
            }
        }
        else if(pieces[x].Piece::getCastling() == true && end == "c8")
        {
            if(checkNotCheck(pieces, turn) == false)
                return false;
            else if(movePiece(pieces, start, "d8", true, turn) == false){
                return false;
            }
        }  
    }

    bool move = movePiece(pieces, start, end, true, turn);
    
    if(move == false)
    {
        pieces[x].Piece::setCastling(false);
    }

    return move;
}

// This function states if the current player to move is in check
bool checkNotCheck(std::vector<Piece> &pieces, int turn) 
{
    char c;
    if(turn == BLACK){
        c = 'K';
    }
    else if(turn == WHITE){
        c = 'k';
    }

    static int x{0};
    for(x = 0; x < 32; x++)
    {
        if(pieces[x].Piece::getName() == c)
            break;
        else
            continue;  
    }

    int oppoTurn = -1 * turn;

    bool NotCheck{true};
    for(int j = 0; j < 32; j++)
    {
        if(checkLegal(pieces, pieces[j].Piece::getLocation(), pieces[x].Piece::getLocation(), oppoTurn, true, true) == true) // if your opponent can capture your king from this board position
        {
            NotCheck = false;
            break;
        }
        else
            continue;
    }

    return NotCheck;
}

// This function determines if the current player has at least one legal move
int canMove(std::vector<Piece> &pieces, std::vector<Square> squares, int turn) 
{
    int moves{0};
    for(int i = 0; i < 32; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            if(pieces[i].Piece::getTeam() != turn){
                continue;
            }
            std::string s = pieces[i].Piece::getLocation();
            std::string e = squares[j].Square::getLocation();
            if(checkNotCheck(pieces, s, e, turn) == false){
                continue;
            }
            else
            moves++;
        }
        
    }

    // std::cout << "Number of possible moves = " << moves << std::endl; // This piece is useful
    return moves;
}

char checkPiece(std::vector<Piece> pieces, std::string l){
    static int i{0};
    for(i = 0; i < 32; i++)
    {
        if(pieces[i].Piece::getLocation() == l)
        {
            return pieces[i].Piece::getName();
        }
        else
            continue;
    }
    return ' ';
}

int checkTeam(std::vector<Piece> pieces, std::string l){
    static int i{0};
    for(i = 0; i < 32; i++)
    {
        if(pieces[i].Piece::getLocation() == l)
        {
            return pieces[i].Piece::getTeam();
        }
        else
            continue;
    }
    return EMPTY;
}

// This function receives the current board position and returns a string unique to that board state (position AND turn)
std::string boardState(char allSpaces[], int turn)
{
    std::string boardState;

    if(turn == WHITE)
        boardState.append(1, 'W');
    else if(turn == BLACK)
        boardState.append(1, 'B');

    boardState.append(1,'.');

    for(int i = 0; i < 64; i++){
        boardState.append(1, allSpaces[i]);
    }

    return boardState;
}

// This function determines if the current board state includes a possible en-passant move. If so, it implements pop_back() because any future iteration of this position will have the same boardStates ID but will be a new position by rule.
bool checkCanEP(std::vector<Piece> pieces, int x, std::string end)
{
    if(pieces[x].Piece::getEPVul() == true)
    {
        std::string l1, l2 = end; 
        char c, d;
        if(end[0] != 'a')
        {
            l1[0]--;
            c = checkPiece(pieces, l1);
            if(tolower(c) == 'p' && checkTeam(pieces, l1) != checkTeam(pieces, end))
                return true;
        }
        if(end[0] != 'h')
        {
            l2[0]++;
            d = checkPiece(pieces, l2);
            if(tolower(d) == 'p' && checkTeam(pieces, l2) != checkTeam(pieces, end))
                return true;
        }   
    }
    return false;
}

bool isAdjacent(std::vector<Piece> pieces, std::string location, char c)
{
    std::string left = location;
    std::string right = location;
    left[0]--;
    right[0]++;
    for(int i = 0; i < 32; i++)
    {
        if(pieces[i].Piece::getName() == c && pieces[i].Piece::getLocation() == left)
        return true;
        else if(pieces[i].Piece::getName() == c && pieces[i].Piece::getLocation() == right)
        return true;
    }

return false;
}