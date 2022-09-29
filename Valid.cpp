#include"Valid.h"
#include<string>
#include<vector>
namespace Valid{
	template<class T>
	bool valid_input(T input, std::vector<T>& valid_inputs)
	{
		for (auto x : valid_inputs) {
			if (x == input) return true;
		}
		return false;
	}
	bool valid_input(std::string input, int max_value)
	{//Checks input, as a number, is less than max_value
		int _input = std::stoi(input);
		if (_input > max_value||_input <0)return false;
		else return true;
	}
	bool valid_input(char input, int max_value) 
	{
		int _input = static_cast<int>(input-'0');//Converts char to int.
		if (_input > max_value||_input<0)return false;
		else return true;
	}
}