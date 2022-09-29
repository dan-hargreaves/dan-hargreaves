//Contains everything relating to a chess game.  A game consists of an instance of the class Board::game_board as well as methods for getting moves from the
//user and checking the moves are valid etc.
#ifndef	GAME
#define GAME
#include"Pieces.h"
#include"Board.h"
#include<string>
namespace Game{
	class game {
		//This class will store all information pertaining to one game. It will include the board, whose turn it is, who won if the game is over...
		//...etc.
	private:
		std::string _white_player_name, _black_player_name;
		char _turn,_winner;
		bool _game_over, _resigned, _quit;
		Board::game_board _board;
	public:
		~game();
		game(std::string, std::string);
		void play();//Plays the game.
		Board::move get_move();
		Board::location get_location();
		bool valid_move(Board::move&);//Checks if a move is valid (can that piece move in that way).
		bool in_check();//Checks if the player is in check in the current layout of the board.
		bool moves_into_check(Board::move&); //Checks if making a move puts the current player in check.
		bool cannot_move();//Returns whether the player can make any move (all moves might put them in check);
		void switch_turn();// Switches whose turn it is, i.e. white->black or black->white.
		void print_turn();
		void print_winner();
		void print_players();
		void show_help();
		bool valid_castle(Board::move&);//Checks if a castle move is valid
		bool game_over();
		void show_board();
	};
};
#endif // !GAME

