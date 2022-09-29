//Contains classes for a chess board.  A board consists of a 2d array of its pieces.  All changes to the board are made via methods in the game_board class.
#ifndef BOARD
#define BOARD
#include<array>
#include<memory>
namespace Pieces {//Forward identifier.
	class square;
	class empty;
}
typedef std::shared_ptr<std::unique_ptr<Pieces::square>> square_ptr;//This is used throughout the program.
namespace Board {
	struct location {
		//A location on the board.
		int row, column;
		bool out_of_range();//Whether the location is out of the board range.
	};
	struct move {
		//A player move.
		location move_from, move_to;
	};
	class game_board {
		//A class for a game board. Contains an 8x8 2d array of square pointers.
	private:
		//The type square_ptr is chosen so that game_board has sole control (unique_ptr) of the underlying Pieces::square object. The unique_ptr is then wrapped in a shared_ptr so that multiple refrences are allowed.
		//The unique_ptr means that the underlying object can be swapped for all instances of the shared_ptr.
		std::array<std::array<square_ptr, 8>, 8> _pieces;//An 8x8 array of square_ptrs.
		square_ptr _null_square;//A null value piece. Returned if access location is out of range.
	public:
		~game_board();
		game_board();
		square_ptr piece(int, int);//Returns a square_ptr to a piece on the board
		square_ptr piece(Board::location&);
		void make_empty(Board::location&);//Deletes a piece from the board.
		void make_move(move&);
		void show(const char);//Prints a representation of the board.
		void get_pawn_promotion(Board::location&);//Promotes a pawn.
		void castle(Board::location&);//Performs a castling move.
		square_ptr make_piece(const char, Board::location&);
		square_ptr make_piece(const char, int, int);
	};
}
#endif//!BOARD
