#include"Pieces.h"
#include"Game.h"
#include<algorithm>
typedef std::shared_ptr<std::unique_ptr<Pieces::square>> square_ptr;//This is used throughout the program
using namespace Pieces;
//############################   square   ########################################################################
square::~square() {};
square::square(int row, int column, const char text, const char colour, bool moved) :
	_location{ row, column },
	_text{ text }, 
	_colour{ colour },
	_moved{ moved }{};
const char square::colour() const { return _colour; };
const char square::value() const { return _text; };
bool square::is_empty() const { return false; };
bool square::moved()const {return _moved;};
Board::location square::location()const { return _location; };
std::vector<square_ptr> square::scan_along_board(Board::game_board& board, int row_scan, int column_scan) 
{
	//Scans along the board in the direction defined by _row_scan and _column_scan starting at the location of the piece.
	//Returns all squares in that direction which the piece could move into
	std::vector<square_ptr> possible_moves; square_ptr piece;
	if (row_scan == 0 && column_scan == 0) return possible_moves;
	int start_row = _location.row; int start_column = _location.column;
	for (int i{ 1 }; i < 8; i++) {//Must scan at most 7 squares in any direction.
		piece = board.piece(start_row + i * row_scan, start_column + i * column_scan);
		if ((*piece)->is_empty()) { possible_moves.push_back(piece); }
		else {
			//If the square isn't empty then the loop will end. If the piece is an opponet's piece then it can be taken.
			if ((*piece)->colour() != _colour) possible_moves.push_back(piece);
			i = 8;
		}
	}
	return possible_moves;
};
//#############################   empty   #######################################################################
empty::~empty() {};
empty::empty(int row, int column) :
	square(row, column, ' ', 'w', true) {};
empty::empty(Board::location loc) :
	empty(loc.row, loc.column) {};
bool empty::is_empty() const { return true; };
void empty::change_location(Board::location& new_location) 
{
	_location = new_location;
};
std::vector<square_ptr>empty::possible_moves(Board::game_board& board)
{
	std::vector<square_ptr> possible_moves;
	return possible_moves;
};
//############################   piece   #######################################################################
piece::~piece() {};
piece::piece(int row, int column, const char text, const char colour) :
	square(row, column, text, colour, false) {};
bool piece::is_empty() const { return false; };
void piece::change_location(Board::location& new_location) 
{
	_location = new_location; _moved = true;
};
//############################   king   ##########################################################################
king::~king() {};
king::king(int row, int column, const char colour) : 
	piece(row, column, 'k', colour) {};
std::vector<square_ptr> king::possible_moves(Board::game_board& board)
{
	std::vector<square_ptr> possible_moves;
	square_ptr piece;
	for (int i{ -1 }; i < 2; i++) {//Check squares around king
		for (int j{ -1 }; j < 2; j++) {
			piece = board.piece(_location.row + i, _location.column + j);
			if ((*piece)->colour() != _colour || (*piece)->is_empty()) possible_moves.push_back(piece);
		}
	}
	return possible_moves;
};
//############################   queen   ########################################################################
queen::~queen() {};
queen::queen(int row, int column, const char colour) : 
	piece(row, column, 'q', colour) {};
std::vector<square_ptr> queen::possible_moves(Board::game_board& board) 
{
	std::vector<square_ptr> possible_moves,moves;
	for (int i{ -1 }; i < 2; i++) {
		for (int j{ -1 }; j < 2; j++) {
			moves = scan_along_board(board,i,j);
			possible_moves.insert(possible_moves.end(), moves.begin(), moves.end());
		}
	}
	return possible_moves;
};
//############################   bishop   #########################################################################
bishop::~bishop() {}; 
bishop::bishop(int row, int column, const char colour) :
	piece(row, column, 'b', colour) {};
std::vector<square_ptr> bishop::possible_moves(Board::game_board& board)
{
	std::vector<square_ptr> possible_moves, moves;
	for (int i{ -1 }; i < 2; i+=2) {
		for (int j{ -1 }; j < 2; j+=2) {
			moves = scan_along_board(board, i, j);
			possible_moves.insert(possible_moves.end(), moves.begin(), moves.end());
		}
	}
	return possible_moves;
};
//############################   knight   ########################################################################
knight::~knight() {}; 
knight::knight(int row, int column, const char colour) :
	piece(row, column, 'n', colour) {};
std::vector<square_ptr> knight::possible_moves(Board::game_board& board)
{
	std::vector<square_ptr> possible_moves;
	square_ptr piece;
	for (int i{ -1 }; i < 2; i+=2) {
		for (int j{ -1 }; j < 2; j+=2) {
			piece = board.piece(_location.row + i, _location.column + j * 2);
			if((*piece)->colour()!=_colour|| (*piece)->is_empty()) possible_moves.push_back(piece);
			piece = board.piece(_location.row + i * 2, _location.column + j);
			if ((*piece)->colour() != _colour || (*piece)->is_empty()) possible_moves.push_back(piece);
		}
	}
	return possible_moves;
};
//############################   rook   ##########################################################################
rook::~rook() {};
rook::rook(int row, int column, const char colour) :
	piece(row, column, 'r', colour) {};
std::vector<square_ptr> rook::possible_moves(Board::game_board& board) 
{
	std::vector<square_ptr> possible_moves, moves;
	for (int i{ -1 }; i < 2; i+=2) {
			moves = scan_along_board(board, i, 0);
			possible_moves.insert(possible_moves.end(), moves.begin(), moves.end());
			moves = scan_along_board(board, 0, i);
			possible_moves.insert(possible_moves.end(), moves.begin(), moves.end());
	}
	return possible_moves;
};
//############################   pawn   ###########################################################################
pawn::~pawn() {};
pawn::pawn(int row, int column, const char colour) :
	piece(row, column, 'p', colour)
{
	if (_colour == 'b') {
		home_row = 6; row_change = -1;
	}
	else {
		home_row = 1; row_change = 1;
	}
};
std::vector<square_ptr> pawn::possible_moves(Board::game_board& board) 
{
	std::vector<square_ptr>possible_moves;
	square_ptr piece = board.piece(_location.row + row_change, _location.column);//First check square directly infront of pawn.
	if ((*piece)->is_empty()) {
		possible_moves.push_back(piece);
		piece = board.piece(_location.row + 2 * row_change, _location.column);//Next check if the pawn can double jump. Only possible is the first square was empty.
		if ((*piece)->is_empty() && !_moved) possible_moves.push_back(piece);
	}
	piece = board.piece(_location.row+ row_change, _location.column - 1);//Next check whether the pawn can take to the left.
	if (!(*piece)->is_empty() && (*piece)->colour() != _colour) possible_moves.push_back(piece);
	piece = board.piece(_location.row + row_change, _location.column + 1);//Next check whether the pawn can take to the right.
	if (!(*piece)->is_empty() && (*piece)->colour() != _colour) possible_moves.push_back(piece);
	return possible_moves;
}








