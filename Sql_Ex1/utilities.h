#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>

#define SCHEMA_FILE "scheme.txt"
#define NOT_FOUND string::npos

using std::string;
using std::back_insert_iterator;
using std::list;
using std::map;
using std::for_each;
using std::cout;
using std::endl;

//Special indexing
#define STRT_IDX 0
#define ZERO_LENGTH 0
#define IDX_ADJUSTMENT 1
#define ONE_WHITE_SPACE 1
#define ONE_CHAR_LENGTH_STRING 1
#define ONE_CHAR 1
#define TWO_CHARS 2

//Special charachters
#define SINGLE_QUOTE "\'"
#define SEMICOLON ";"
#define COLON ":"
#define COMMA ","
#define DOT "."

//static inline functions
static inline size_t last_significant_pos(string str_to_check) {
	return (str_to_check.length() - 1);
}

static inline bool is_digit(char c){
	return (c >= '0' && c <= '9');
}

static inline bool is_alpha(char c){
	return (c >= 'a' && c <= 'z');
}

static inline bool is_string(string str_chck){
	
	if (str_chck[STRT_IDX] == '(' && str_chck[last_significant_pos(str_chck)] == ')')
	{
		return is_string(str_chck.substr(IDX_ADJUSTMENT,
						(last_significant_pos(str_chck) - IDX_ADJUSTMENT)));
	}
	else
	{
		size_t first = str_chck.find_first_of(SINGLE_QUOTE);
		size_t last = str_chck.find_last_of(SINGLE_QUOTE);
	
		return ((first == 0) && (last == str_chck.length() - 1));
	}
}

bool static inline is_num_prefix(char c){
	return (c == '+' || c == '-');
}

static inline bool is_command_end_w_semicolon(string i_str_chck) {

	size_t semicolon_idx = i_str_chck.find_last_of(SEMICOLON);

	return (last_significant_pos(i_str_chck) == semicolon_idx);
}

	class utilities
	{
		
	public:
		static bool is_int(string input);
		static bool is_unsigned_int(std::string input);
		static string convert_input_to_lower_case(string str_to_proccess);

		static string rm_dup_wtsp(string str_to_proccess);
		static string rm_wtsp_bef_and_aft_comma(string str_to_proccess);
		static string rm_all_wtsp(string str_to_proccess);
	};

#endif //!UTILITIES_H_