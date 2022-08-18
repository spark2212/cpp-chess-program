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

int printBoard(char spaces[], char pieceName);
bool checkLegal(std::vector<Piece> &pieces, std::string start, std::string end, int turn, bool testCheck, bool captureHere);
bool movePiece(std::vector<Piece> &pieces, std::string start, std::string end, bool test, int turn);
bool isDiagonal(std::string start, std::string end);
bool checkMoveIsSafe(std::vector<Piece> &pieces, std::string start, std::string end, int turn);
bool checkKingIsSafe(std::vector<Piece> &pieces, int turn);
int CountLegalMoves(std::vector<Piece> &pieces, std::vector<Square> squares, int turn);
char checkPiece(std::vector<Piece> pieces, std::string l);
int checkTeam(std::vector<Piece> pieces, std::string l);
std::string createBoardState(char allSpaces[], int turn, std::vector<Piece> pieces);
// bool checkCanEP(std::vector<Piece> pieces, int x, std::string end);

#endif