#ifndef VALID
#define VALID
#include<string>
#include<vector>
namespace Valid {
	template<class T>
	bool valid_input(T, std::vector<T>&);
	bool valid_input(std::string , int );
	bool valid_input(char, int );
}
#endif//!VALID
