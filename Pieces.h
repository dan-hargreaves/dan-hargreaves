//Contains the classes for to chess pieces.  Each piece has a location on the board, a colour, a value (e.g. 'k' for king). The pieces contain methods...
//...for returning which squares on a board they can move to.  This is used to check if the player makes a valid move.
#ifndef PIECES
#define PIECES
#include"Board.h"
#include<vector>
#include<memory>
#include<array>
typedef std::shared_ptr<std::unique_ptr<Pieces::square>> square_ptr;//This is used throughout the program
namespace Pieces{	
	//###########################################   square   ########################################################
	class square {
		//Abstract base class for a board square.
	protected:
		Board::location _location;//_location of the square on the board.
		const char _colour;//Colour of the piece. e.g. 'w' for white.
		const char _text;//Text value of the piece e.g. 'k' for king.
		bool _moved;//Stores where the piece has _moved or not.
	public:
		~square();	
		square(int, int, const char, const char, bool);
		const char value() const;//Returns the _text label of the piece.
		const char colour() const;//Returns colour of the piece.
		bool moved() const; //Returns whether the piece has been moved
		Board::location location() const;//Returns location
		virtual bool is_empty() const=0;//Returns whether the piece is empty
		virtual void change_location(Board::location&)=0;
		virtual std::vector<square_ptr> possible_moves(Board::game_board &) = 0;//Returns the possible moves a piece can make.
		std::vector<square_ptr> scan_along_board(Board::game_board&, int, int);//Scans along the board in a given direction(as a rook/bishop moves) and returns the possible...
		//...squares the piece can move to.
	};
	//##########################################   empty   ############################################################
	class empty : public square {
		//A class for empty squares.
	public:
		~empty();
		empty(int, int);
		empty(Board::location);
		bool is_empty() const;
		void change_location(Board::location&);
		std::vector<square_ptr> possible_moves(Board::game_board&);
	};
	//#########################################   piece   #############################################################
	class piece :public square {
	public:
		~piece();
		piece(int, int, const char, const char);
		bool is_empty() const;
		void change_location(Board::location&);
	};
	//########################################   king   ###############################################################
	class king : public piece {
	public:
		~king();
		king(int, int, const char);
		std::vector<square_ptr> possible_moves(Board::game_board&);
	};
	//#######################################   queen   ################################################################
	class queen : public piece {
	public:
		~queen();
		queen(int, int, const char);
		std::vector<square_ptr> possible_moves(Board::game_board&);
	};
	//#######################################   bishop   ################################################################
	class bishop : public piece {
	public:
		~bishop();
		bishop(int, int, const char);
		std::vector<square_ptr> possible_moves(Board::game_board&);
	};
	//######################################   knight   ##################################################################
	class knight : public piece {
	public:
		~knight();
		knight(int, int, const char);
		std::vector<square_ptr> possible_moves(Board::game_board&);
	};
	//######################################   rook   #####################################################################
	class rook : public piece {
	public:
		~rook();
		rook(int, int, const char);
		std::vector<square_ptr> possible_moves(Board::game_board&);
	};
	//######################################   pawn   #####################################################################
	class pawn : public piece {
	private:
		int home_row, row_change;//The row the pawn starts on and the direction in which it can move
	public:
		~pawn();
		pawn(int, int, const char);
		std::vector<square_ptr> possible_moves(Board::game_board&);
	};
};
#endif// !PIECES