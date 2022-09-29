//A chess game by Dan Hargreaves. Started on 01/04/2020.
#include"Pieces.h"
#include"Game.h"
#include"Board.h"
#include"Valid.h"
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<cstdlib>
#include<memory>
#include<time.h>
typedef std::unique_ptr<Game::game> game_ptr;
void start_new_game(std::vector<game_ptr>&, std::vector<game_ptr>&);
//##################################################   show_active_games   #####################################
void show_active_games(std::vector<game_ptr>& active_games, std::vector<game_ptr>& finished_games) {
	//Shows a homepage of active games. Prints a table of all active games. The user can select an active game and resume it or start a new game.
	int num_active_games = active_games.size();
	std::cout << "\nYou have " << num_active_games << " active games" << std::endl;
	int i{ 1 };
	for (i; i < num_active_games + 1; i++) {//Prints a list of active games.
		std::cout << i << ": "; active_games[i - 1]->print_players();
	}
	std::cout << "\nOr\nn: Start a new game" << std::endl;
	std::cout << "b: Back" << std::endl;
	std::string input;
	for (;;) {//Loop ends when a good input is given.
		std::cout << "\nChoose an option: " << std::endl;
		try {
			std::getline(std::cin, input);
			if (input == "n") {
				start_new_game(active_games, finished_games);
				break;
			}
			if (input == "b") break;//Indicates back. Takes us back to homepage
			else if (input == "h") {
				std::cout << "  Enter the number of the game you wish to resume.\n  Or\n  n     New game.\n  b    Back. Go back to homepage." << std::endl;
			}
			else if (Valid::valid_input(input, num_active_games)) {
				int game_index = std::stoi(input) - 1;
				active_games[game_index]->play();
				if (active_games[game_index]->game_over()) {
					finished_games.push_back(std::move(active_games[game_index]));//moves the game from active to finished.
				}
				break;
			}
			else throw ' ';//Bad input given.
		}
		catch (...) {
			std::cout << "Bad input. Try again ('h' for help)" << std::endl;
		}
	}
}
//######################################   show_finished_games()   ##################################################################
void show_finished_games( std::vector<game_ptr>& active_games, std::vector<game_ptr>& finished_games)
{//Gives a page of the finished games. The user can choose a finished game and see the final layout of the board or start a new game.
	int num_finished_games = finished_games.size();
	std::cout << "\nYou have " << num_finished_games << " finished games." << std::endl;
	int i{ 1 };
	for (i; i < num_finished_games + 1; i++) {
		std::cout << i << ": "; finished_games[i - 1]->print_players();std::cout<<"\n   "; finished_games[i - 1]->print_winner();
	}
	std::cout << "Or\nn: Start a new game" << std::endl;
	std::cout << "b: Back" << std::endl;
	std::string input;
	for (;;) {//Runs until a good input is given.
		std::cout << "\nChoose an option: " << std::endl;
		try {
			std::getline(std::cin,input);
			if (input == "n") {
				start_new_game(active_games, finished_games);
				break;
			}
			else if (input == "b") break;//Takes us back to homepage.
			else if (input == "h") std::cout << "  Enter the number of the game to view the board. \n  Or\n  n     Start a new game.\n  b     Back. Go back to homepage." << std::endl;
			else if (Valid::valid_input(input, num_finished_games)) {
				int game_index = std::stoi(input) - 1;
				finished_games[game_index]->show_board();
				break;
			}
			else throw ' ';//Bad input given.
		}
		catch (...) {
			std::cout << "Bad input. Try again ('h' for help)" << std::endl;
		}
	}
}
//##############################################   start_new_game   ###############################################
void start_new_game(std::vector<game_ptr>& active_games, std::vector<game_ptr>& finished_games) {
	//Starts a new game. Creates a new game object and plays it. When the game.play() loop exits, the game is added to finished_games or active_games.
	std::string name_1;std::string name_2;
	std::cout << "\nStarting a new game. \nEnter name of player 1:" << std::endl;
	std::getline(std::cin, name_1);
	std::cout << "\nEnter name of player 2: " << std::endl;
	std::getline(std::cin, name_2);
	std::string input;
	for (;;) {//Ends once good input is given.
		std::cout << "\nWho should start?" << std::endl;
		std::cout << "1: " << name_1 << std::endl;
		std::cout << "2: " << name_2 << std::endl;
		std::cout << "3: Choose randomly" << std::endl;
		std::cout << "b: Back" << std::endl;
		try {
			std::getline(std::cin, input);
			if (input == "b") break;//Takes us back to homepage.
			else if (Valid::valid_input(input, 3)) break;//Good input given. End for loop
			else throw ' ';//Bad input given.
		}
		catch (...) {
			std::cout << "Bad input. Try again ('1','2','3')" << std::endl;
		}
	}
	if (input != "b") {//Checks the player didn't press back
		std::string white_player_name, black_player_name;
		if (input == "1") {
			white_player_name = name_1; black_player_name = name_2;
		}
		else if (input == "2") {
			white_player_name = name_2; black_player_name = name_1;
		}
		else if (input == "3") {
			srand((unsigned)time(NULL));
			int r = std::rand() % 2;//Random int 0 or 1.
			if (r == 1) {
				white_player_name = name_1; black_player_name = name_2;
			}
			else {
				white_player_name = name_2; black_player_name = name_1;
			}
		}
		game_ptr new_game = std::make_unique<Game::game>(white_player_name, black_player_name);
		new_game->play();
		if (new_game->game_over()) finished_games.push_back(std::move(new_game));
		else {
			active_games.push_back(std::move(new_game));
		}
	}
}
//###########################################   home_page   ###################################
void home_page(std::vector<game_ptr>& active_games, std::vector<game_ptr>& finished_games)
{//Shows a homepage for the user. User can view active games, finished games or start a new game.
	std::cout << "\n1: Active games" << std::endl;
	std::cout << "2: Finished games" << std::endl;
	std::cout << "3: Start a new game" << std::endl;
	std::string input;
	for (;;) {//Runs until a good input is given.
		std::cout << "\nChoose an option." << std::endl;
		try {
			std::getline(std::cin, input);
			if (Valid::valid_input(input, 3)) break;
			else throw' ';//Bad input
		}
		catch (...) {
			std::cout << "Bad input. Try again ('1', '2', '3'):" << std::endl;
		};
	}
	if (input == "1") show_active_games(active_games, finished_games);
	else if (input == "2") show_finished_games(active_games, finished_games);
	else if (input == "3")start_new_game(active_games, finished_games);
}
int main() {
	std::vector<game_ptr> active_games, finished_games;
	std::cout << "Welcome to Dan Hargreaves' Chess Game!" << std::endl;
	for (;;) {
		home_page(active_games, finished_games);
	}
	return 0;
}
