#include"Board.h"
#include"Pieces.h"
#include"Valid.h"
#include<utility>
#include<memory>
#include<iostream>
#include<string>
#include<vector>
#if defined(_WIN32)||defined(_WIN64)//Works only on windows
#include<conio.h>
#include<Windows.h>
#endif
typedef std::shared_ptr<std::unique_ptr<Pieces::square>> square_ptr;//This is used throughout the program
using namespace Board;
//#######################################   location   ##################################################################
bool location::out_of_range()
{
	if (row > 7 || row < 0 || column>7 || column < 0) return true;
	else return false; 
};
//########################################   game_board   ################################################################
game_board::game_board() 
{//Initialises the pieces in the board.
	_null_square = std::make_shared<std::unique_ptr<Pieces::square>>(new Pieces::empty(8, 8));
	std::array<std::array<const char, 8>, 8> layout = {
	{
		{'R','H','B','Q','K','B','H','R'},
		{'P','P','P','P','P','P','P','P'},
		{' ',' ',' ',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' ',' ',' ',' '},
		{'p','p','p','p','p','p','p','p'},
		{'r','h','b','q','k','b','h','r'},
	}
	};
	for (int i{}; i < 8; i++) {
		for (int j{}; j < 8; j++) {
			const char x = layout[i][j];
			_pieces[i][j] = make_piece(x, i, j);
		}
	}
};
square_ptr game_board::make_piece(const char text, int row, int column)
{//Make a new piece based on text. Upper case indicates a white pice.
	square_ptr piece;
	if (text == 'R') piece = std::make_shared<std::unique_ptr<Pieces::square>>(new Pieces::rook{ row, column, 'w' });
	if (text == 'r') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::rook{ row, column, 'b' });
	if (text == 'K') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::king{ row, column, 'w' });
	if (text == 'k') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::king{ row, column, 'b' });
	if (text == 'B') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::bishop{ row, column, 'w' });
	if (text == 'b') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::bishop{ row, column, 'b' });
	if (text == 'Q') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::queen{ row, column, 'w' });
	if (text == 'q') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::queen{ row, column, 'b' });
	if (text == 'H') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::knight{ row, column, 'w' });
	if (text == 'h') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::knight{ row, column, 'b' });
	if (text == 'P') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::pawn{ row, column, 'w' });
	if (text == 'p') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::pawn{ row, column, 'b' });
	if (text == ' ') piece = std::make_shared< std::unique_ptr<Pieces::square>>(new Pieces::empty{ row, column });
	return piece;
}
square_ptr game_board::make_piece(const char text, Board::location& loc)
{
	return make_piece(text, loc.row, loc.column);
}
game_board::~game_board() 
{
	for (auto i : _pieces) {
		for (auto& j : i) {
			(*j).release();
		}
	}
};
square_ptr game_board::piece(int row, int column)
{//Returns a square_ptr to a board piece. The Pieces::possible_moves() functions will sometimes call a piece which is outside the board ranges.  When this happens...
//...nullptr is returned.
	if (row > 7 ||row<0|| column > 7||column<0) return _null_square; 
	else return _pieces[row][column];
};
square_ptr game_board::piece(Board::location& loc)
{
	return piece(loc.row, loc.column);
};
void game_board::make_empty(location& loc)
{//Deletes a piece at a given location in the board and replaces it with Pieces::empty. Used by game::make_move().
	square_ptr new_piece = make_piece(' ', loc);
	(*_pieces[loc.row][loc.column]).swap(*new_piece);
	(*new_piece).release();
};
void game_board::make_move(move& player_move)
{//Makes a player move. This is done after the legality of the move is checked. First swaps the corresponding pieces and then replaces the...
//... square which is _moved form with a Pieces::empty.
	if (player_move.move_from.out_of_range() || player_move.move_to.out_of_range()) return;
	square_ptr piece_moved = _pieces[player_move.move_from.row][player_move.move_from.column];
	square_ptr moved_to = _pieces[player_move.move_to.row][player_move.move_to.column];
	(*moved_to).swap(*piece_moved);
	(*moved_to)->change_location(player_move.move_to);//Updates the _location of the piece.
	make_empty(player_move.move_from);
	if ((*moved_to)->value() == 'p' && (player_move.move_to.row % 7) == 0) {//Pawn has reached end of board.
		get_pawn_promotion(player_move.move_to);
	}
	if ((*moved_to)->value() == 'k' && abs(player_move.move_to.column - player_move.move_from.column) == 2) {//True if king has castled
		castle(player_move.move_to);
	}
};
void game_board::castle(Board::location& king_location)
{//Makes a castling move. The king has already been moved by make_move(), so just moved the appropriate rook.
	square_ptr rook, square;
	if (king_location.column == 6) {
		rook = _pieces[king_location.row][7];
		square = _pieces[king_location.row][5];
	}
	else {
		rook = _pieces[king_location.row][0];
		square = _pieces[king_location.row][3];
	}
	(*rook).swap(*square);
	Board::location new_location = (*square)->location();
	(*rook)->change_location(new_location);//Updates rook location
};
void game_board::get_pawn_promotion(Board::location& pawn_location)
{//Asks user what they want to promote pawn to
	std::cout << "Promote pawn. What piece would you like?" << std::endl;
	std::string input;
	for (;;) {//Runs until a good input is given.
		try {
			std::getline(std::cin, input);
			if (input == "q" || input == "r" || input == "b" || input == "h") break;
			else throw ' ';
		}
		catch (...) {
			std::cout << "Bad input. Try again ('q','r','b','h'): " << std::endl;
		}
	}
	square_ptr pawn = _pieces[pawn_location.row][pawn_location.column];//Pawn which is being promoted
	if ((*pawn)->colour() == 'w') input[0] = std::toupper(input[0]);
	square_ptr new_piece = make_piece(input[0], pawn_location);
	(*pawn).swap(*new_piece);//Swap in the new piece
	(*new_piece).release();
};
#if defined(_WIN32)||defined(_WIN64)
void game_board::show(const char turn)
{
	//Prints a representation of the board.
	std::array<const char, 8> letters{ 'a','b','c','d','e','f','g','h' };//An array of column labels.
	auto get_colour_code = [](int i, int j, const char colour) {
		int wb{ 31 }, wg{ 47 }, rb{ 28 }, rg{ 44 };//These are colour codes. First character indicated _text colour, second character background colour.
		int colour_code;
		if (colour == 'b') {//Change _text colour depending on colour of piece. Red _text indicates black pieces.
			if ((i + j) % 2 == 0) colour_code= rb;//Change background colour depending on _location on board.
			else colour_code= rg;
		}
		else {//White _text colour for white pieces.
			if ((i + j) % 2 == 0) colour_code= wb;//Change background colour
			else colour_code= wg;
		}
		return colour_code;
	};
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	//Prints the board output.
	std::cout << "\n\n    a     b     c     d     e     f     g     h";
	std::cout  << "\n  _______________________________________________";
	int i, i_change, i_end;
	if (turn == 'w') {//Flips the board so active player is at the bottom.
		i = 7; i_change = -1; i_end = -1;
	}
	else {
		i = 0; i_change = 1;i_end = 8;
	}
	for (i; i != i_end; i+=i_change) {
		std::cout << "\n |" ;
		for (int j{}; j < 8; j++) {
			int colour_code = get_colour_code(i, j, 'w');
			SetConsoleTextAttribute(console, colour_code);
			std::cout << "     ";
			SetConsoleTextAttribute(console, 15);
			std::cout << "|";
		}std::cout << "\n"<<i+1<<"|";
		for (int j{}; j < 8; j++) {
			auto piece = _pieces[i][j];
			int colour_code = get_colour_code(i, j, (*piece)->colour());
			char text = (*piece)->value();
			if ((*piece)->colour() == 'b') text = toupper(text);//Print black pieces in upper case
			SetConsoleTextAttribute(console, colour_code);
			std::cout << "  " << text << "  ";
			SetConsoleTextAttribute(console, 15);//Resets colour to white on black.
			std::cout << "|";
		}
		std::cout << i + 1;
		std::cout<<"\n |";
		for (int j{}; j < 8; j++) {
			int colour_code = get_colour_code(i, j, 'w');
			SetConsoleTextAttribute(console, colour_code);
			std::cout << "_____";
			SetConsoleTextAttribute(console, 15);
			std::cout << "|";
		}
	}
	std::cout << "\n    a     b     c     d     e     f     g     h" << std::endl;
	SetConsoleTextAttribute(console, 15);
}
#else
void game_board::show()
{
	//Prints a representation of the board.
	std::array<const char, 8> letters{ 'a','b','c','d','e','f','g','h' };//An array of column labels.

	//Prints the board output.
	std::cout << "\n\n    a     b     c     d     e     f     g     h";
	std::cout << "\n  _______________________________________________";
	for (int i{ 7 }; i > -1; i--) {
		std::cout << "\n |";
		for (int j{}; j < 8; j++) {
			std::cout << "     |";
		}std::cout << "\n" << i + 1 << "|";
		for (int j{}; j < 8; j++) {
			auto piece = _pieces[i][j];
			char text = (*piece)->value();
			if ((*piece)->colour() == 'b') text = toupper(text);//Print black pieces in upper case
			SetConsoleTextAttribute(console, colour_code);
			std::cout << "  " << text << "  |";
		}
		std::cout << i + 1;
		std::cout << "\n |";
		for (int j{}; j < 8; j++) {
			std::cout << "_____|";
		}
	}
	std::cout << "\n    a     b     c     d     e     f     g     h" << std::endl;
	SetConsoleTextAttribute(console, 15);
}
#endif
