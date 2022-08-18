// /////////////////////////
// ChessGame_functions.cpp
// /////////////////////////

#include "ChessGame_files.hpp"

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
        return turnsTilDraw;
    else if(pieceName == 'p' || pieceName == 'P')
        turnsTilDraw = 100;
    else if(piecesPrevious > piecesLeft){
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

        pieces[x].Piece::setCanEP()
        int k{0};
        for(k = 0; k < 32; k++)
        {
            if(pieces[k].Piece::getRank() == start[1] && end[0] == pieces[k].Piece::getFile())
            {
                if(pieces[k].Piece::getEPVul() == true)
                {
                    
                    pieces[x].Piece::setCanEP(true);
                    break;
                }
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
        else if(k != 32 && isDiagonal(start, end) == true){
            pieces[x].Piece::setCanEP('1');
        return true;
    }
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
        else if(k != 32 && isDiagonal(start, end) == true){
            pieces[x].Piece::setCanEP('1');
            return true;
        }
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
            NotCheck = checkKingIsSafe(pieces, turn);
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

    NotCheck = checkKingIsSafe(pieces, turn);

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

bool checkMoveIsSafe(std::vector<Piece> &pieces, std::string start, std::string end, int turn) // This function determines if a specific move would put the current player in check. It also returns false if the player attempts to castle while in check, or in a way that would cause them to pass through check.
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
            if(checkKingIsSafe(pieces, turn) == false)
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
std::string createBoardState(char allSpaces[], int turn, std::vector<Piece> pieces)
{
    std::string boardState;

    if(turn == WHITE)
        boardState.append("W.");
    else if(turn == BLACK)
        boardState.append("B.");

    // boardState.append(1,'.');

    for(int i = 0; i < 64; i++)
        boardState.append(1, allSpaces[i]);

    boardState.append(1, '/');

    for(int j = 0; j < 32; j++)
    {
        if(pieces[j].Piece::getName() != 'p' && pieces[j].Piece::getName() != 'P')
            continue;
        if(pieces[j].Piece::getEPVul() == true)
            boardState.append(1, '1');
        else
            boardState.append(1, 'o');

        // boardState.append(1, pieces[j].Piece::getCanEP());
    }

    return boardState;
}

/* 
 // This function determines if the current board state includes a possible en-passant move. If so, it implements pop_back() because any future iteration of this position will have the same boardStates ID but will be a new position by rule.
 bool checkCanEP(std::vector<Piece> pieces, int x, std::string end){
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
} */