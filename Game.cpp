#include"Game.h"
#include"Pieces.h"
#include"Board.h"
#include"Valid.h"
#include<iostream>
#include<sstream>
#include<vector>
#include<algorithm>
#include<memory>
#include<string>
typedef std::shared_ptr<std::unique_ptr<Pieces::square>> square_ptr;//This is used throughout the program
using namespace Game;
//###########################################   game   ################################################################
	game::~game() {};
	game::game(std::string name_1, std::string name_2) 
	{
		_white_player_name = name_1; _black_player_name = name_2;
		_game_over = false; _resigned = false;  _quit = false;
		_turn = 'w'; _winner = ' ';
	};
	bool game::game_over() { return _game_over; };
	void game::switch_turn() 
	{
		//Switch turn from white to black or black to white.
		_turn == 'w' ? _turn='b' : _turn='w';
	}
	Board::location game::get_location()
	{
		//Gets a location from user. A location is inputted as a chess coordinate (e.g. d5) which is then converted to type location.
		//If the user quits or resigns then an error is thrown to get_move();
		std::vector<char> letters{ 'a','b','c','d','e','f','g','h' };//Accep
		std::vector<char> letters_caps{'A','B','C','D','E','F','G','H' };
		std::vector<char> numbers{ '1','2','3','4','5','6','7','8' };
		bool valid_letter{ false }, valid_number{ false };
		int row_number, column_number;
		std::string input;//Input from the user.
		Board::location inputted_location;//The return value.
		for (;;) {//This loop will exit after a good iput is given.
			try {
				std::getline(std::cin, input);
				if (input.length() != 2) {//Throw if the input is too long or short
					if (input == "h" || input == "H") show_help();
					else if (input == "r" || input == "R") throw 'r';
					else if (input == "q" || input == "Q")throw 'q';
					else throw ' ';//A bad input is given.
				}
				for (int i{};i<8;i++){//Check if input is valid
					if (input[0] == letters[i] || input[0] == letters_caps[i]) {
						valid_letter = true;
						column_number = i;
					}
					if (input[1] == numbers[i]) {
						valid_number = true;
						row_number = i;
					}
				}
			}catch (const char error) {//Either quit or resigned.
				throw error;//Throw error to get_move();
			}
			catch (...) {//Some other bad input given
				valid_letter = false; valid_number = false;
			};
			if (!valid_letter) std::cout << "The first character must be a letter a-h." << std::endl;//Error messages
			if (!valid_number) std::cout << "The second character must be a number 1-8." << std::endl;
			if (valid_letter && valid_number) {//Good input has been given.
				inputted_location.column = column_number;
				inputted_location.row = row_number;
				break;//End for loop
			}
			else {
				valid_letter = false;  valid_number = false;
				std::cout << "Try again or enter 'h' for help." << std::endl;
			}
		}
		return inputted_location;
	}
	Board::move game::get_move() 
	{
		//Get's a move from the user. A move consists of 2 locations. The location of the piece moved and the location it's moved to.
		//If the user quits or resigns then an error is thrown to play().
		Board::move player_move;
		for (;;) {
			try {
				std::cout << "Enter the location of the piece you would like to move." << std::endl;
				player_move.move_from = get_location();
				square_ptr piece = _board.piece(player_move.move_from);//Check the user has tried to move their own piece
				if ((*piece)->colour() != _turn) std::cout << "You cannot move that piece." << std::endl;
				else if ((*piece)->is_empty()) std::cout << "That square is empty." << std::endl;//cannot move empty squar
				else {
					std::cout << "Enter the location you would like to move to." << std::endl;
					player_move.move_to = get_location();
					break;//Good input given.
				}
			}
			catch (const char error) {//Catches a resignation or quit.
				throw error;//Throw error up to play().
			};
		}
		return player_move;
	}
	bool game::cannot_move()
	{
		//Returns whether the player can make a move. Loops through the board and tests every move the player can make. If a move doesn't put...
		//...them in check then returns false.
		square_ptr piece;
		std::vector<square_ptr> piece_possible_moves;
		Board::move test_move;
		for (int i{}; i < 8; i++) {
			for (int j{}; j < 8; j++) {//Loop through all pieces on board.
				piece = _board.piece(i, j);
				if ((*piece)->colour() == _turn && !(*piece)->is_empty()) {//Consider only non-empty friend pieces.
					piece_possible_moves = (*piece)->possible_moves(_board);
					for (auto i : piece_possible_moves) {//Runs through every move the player can make.
						test_move.move_from = (*piece)->location(); test_move.move_to = (*i)->location();
						if (!moves_into_check(test_move)) return false;//There is a legal move the player can make.
					}
				}
			}
		}
		return true;
	};
	bool game::in_check()
	{
		//Returns whether the player is currently in check. Loops through each piece in the board. If an enemy...
		//...piece is able to take the king then the player is in check.
		square_ptr piece;
		std::vector<square_ptr> moves;
		for (int i{}; i < 8; i++) {
			for (int j{}; j < 8; j++) {//Loop through each piece in the board in turn.
				piece = _board.piece(i, j);
				if ((*piece)->colour() != _turn && !(*piece)->is_empty()) {//Consider only non-empty enemy pieces.
					moves = (*piece)->possible_moves(_board);
					for (auto i : moves) {//Every move the enemy can make.
						if ((*i)->value() == 'k') return true;//If one of the pieces can take the king
					}
				}
			}
		}
		return false;
	};
	bool game::moves_into_check(Board::move& player_move)
	{
		//Given a move, returns whether the player is in check if they make that move. The intended move is made, the board is then checked to see if the king is...
		//...in check. The board is then returned to its original layout.
		if (player_move.move_to.out_of_range()) return true; //piece.possible_moves() may return square out of range.
		bool moves_into_check;
		square_ptr taken_piece = _board.make_piece(' ', player_move.move_to);//Stores the the piece which was taken (if one was taken) so we can undo the move.
		square_ptr moved_to =  _board.piece(player_move.move_to);//The piece which is about to be taken
		square_ptr piece_moved =  _board.piece(player_move.move_from);
		(*moved_to).swap(*taken_piece);//moved_to is now empty
		(*moved_to).swap(*piece_moved);//Make the move which we intend to test.
		moves_into_check = in_check();//Check if the move puts us in check	
		(*moved_to).swap(*piece_moved);//Undo the move we made
		(*moved_to).swap(*taken_piece);//Swap back in the piece which was taken.
		return moves_into_check;
	};
	bool game::valid_move( Board::move& player_move) 
	{
		//Checks whether the move the player has made is valid. First checks whether they have castled.
		square_ptr piece_moved =  _board.piece(player_move.move_from);
		if ((*piece_moved)->value() == 'k' && abs(player_move.move_to.column - player_move.move_from.column) == 2) {
		//True if king has castled
			bool valid =  valid_castle(player_move);
			return valid;
		}
		square_ptr moved_to =  _board.piece(player_move.move_to);
		for (auto i  : (*piece_moved)->possible_moves( _board)) {//Checks if moved_to refers to any of the possible moves.
			if (moved_to == i) return true;
		}
		return false;
	};

	bool game::valid_castle(Board::move& player_move)
	{
		//Check castling moves. First checks if the squares to castle through are empty and then checks the king doesn't move through check.
		square_ptr rook;
		square_ptr king = _board.piece(player_move.move_from);
		int column_scan = (player_move.move_to.column - player_move.move_from.column) / 2;//Direction in which king castles
		if ((*king)->moved()) {
			std::cout << "The king has already been moved." << std::endl;
			return false;
		}
		if (in_check()) {
			std::cout << "Cannot castle out of check." << std::endl;
			return false;
		}
		int num_spaces{};//Number of empty squares that should be between the king and rook for a legal castle. We check this aginst the actual value.
		if (column_scan == 1) {
			rook = _board.piece((*king)->location().row, 7);//This piece will be a rook if castling is valid
			num_spaces = 2;
		}
		else {
			rook = _board.piece((*king)->location().row, 0);//This piece will be a rook if castling is valid
			num_spaces = 3;
		}
		if ((*rook)->value() != 'r' || (*rook)->moved()) {
			std::cout << "The rook has been moved." << std::endl;
			return false;//Checks whether there is a rook to castle with
		}
		std::vector<square_ptr> moves = (*king)->scan_along_board(_board, 0, column_scan);
		if (moves.size() != num_spaces) {
			std::cout << "Cannot castle through a piece." << std::endl;
			return false;
		}
		for (int i{ 1 }; i < 3; i++) {
			Board::move test_move = player_move;
			test_move.move_to.column -= column_scan * i;
			if (moves_into_check(test_move)) {
				std::cout << "Cannot castle though check." << std::endl;
				return false;
			}
		}
		return true;
	}
	void game::show_help()
	{
		std::cout << "  Moves are given by chess coordinates, column then row." << std::endl;
		std::cout << "  E.g. 'd5' inidcates column 4, row 5. Note 'h' (horse) is used to indicate knight." << std::endl;
		std::cout << "  To castle, move the king to the appropriate square." << std::endl;
		std::cout << "  White player is " << _white_player_name << ", black player is " << _black_player_name << std::endl;
		std::cout << "  Or" << std::endl;
		std::cout << "  q     Quit game. Stop playing the game. You can resume it later." << std::endl;
		std::cout << "  r     Resign. Your opponent wins." << std::endl;
	};
	void game::show_board() {
		std::cout << '\n';
		print_players();
		_board.show(_turn);
		print_winner();
	}
	void game::print_turn()
	{
		if (_turn == 'w') std::cout << _white_player_name << " to move (white)." << std::endl;
		else std::cout << _black_player_name << " to move (black)." << std::endl;
	};
	void game::print_winner() 
	{
		std::string str1, str2;
		if (_winner == 's')  std::cout << "Draw by stalemate!" << std::endl;
		else if (_winner = 'b') str1 = _white_player_name;
		else str2 = _black_player_name;
		if (_resigned) str2 = "resignation!";
		else str2 = "checkmate!";
		std::cout << str1 <<" wins by "<< str2 << std::endl;
	}
	void game::print_players()
	{
		std::cout << _white_player_name << "(w) vs " << _black_player_name << "(b)";
	}
	void game::play() 
	{
		//This loop implements the chess game, running a loop until the player is in checkmate or stale mate.
		while (!_game_over) {//Loop runs until game has ended.
			_board.show(_turn);
			print_turn();
			try {
				Board::move player_move = get_move();
				if (!valid_move(player_move)) std::cout << "Invalid move. Try again: " << std::endl;
				else {
					if (moves_into_check(player_move)) std::cout << "Cannot move into check. Try again: " << std::endl;
					else {
						_board.make_move(player_move);
						switch_turn();
					}
				}
			}
			catch (const char error) {//Catches a resignation or quit thrown by get_move();
				if (error == 'r') {
					_game_over = true;
					_resigned = true;
				}
				if (error == 'q') {
					_game_over = true;
					 _quit = true;
				}
			}
			if (cannot_move()) {//This means game is over by either checkmate or stalemate.
				_game_over = true;
			}
		}
		if (_quit) _game_over = false;//Game can be resumed later.
		else if (_resigned || in_check()) {//Game ended by resignation or checkmate.
			switch_turn();
			_winner = _turn;
			print_winner();
		}
		else _winner = 's';//Game ended by stalemate.
	}
	
