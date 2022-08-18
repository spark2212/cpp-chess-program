// ////////////////////
// ChessGame.cpp
// Copyright © 2020 
// Theodore Shore
// All Rights reserved
// ////////////////////

#include "ChessGame_files.hpp"

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
    std::string firstState = createBoardState(spaces, WHITE, pieces);
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

        
        if(checkLegal(pieces, start, end, turn, false, false) == false) // This code checks if the move obeys the movement rules of the pieces
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
            boardStates.clear(); // When any of these pieces moves for the first time, it permanently changes the state of the board, thus, there is no need to keep old boardState ID's, and in the case of the king and rook, it's actually detrimental to keep them.
        }
        std::string newState = createBoardState(spaces, turn, pieces);
        boardStates.push_back(newState);

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
                    pieces[i].Piece::setCanEP('0');
                }
            }
            else if(turn == WHITE)
            {
                turnNumber++;
                std::cout << turnNumber << ". White to move. ";
                for(int i = 0; i < 32; i++)
                {
                    pieces[i].Piece::setEPVul(pieces[i].getLastMoved());
                    pieces[i].Piece::setCanEP('0');
                }
            }
            
            std::cout << "Please enter your move: ";

    }while(1);

    std::cout << "Game Over. Goodbye! \n" <<std::endl;

    return 0;

}
