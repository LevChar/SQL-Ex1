#ifndef ERROR_TRACKER_H_
#define ERROR_TRACKER_H_

#include <list>
#include <map>

#define CURR_AMT_OF_SUPPORTED_ERRORS 24

using std::string;
using std::list;
using std::map;

typedef unsigned int Uint;

//Parsing error flags Enum
enum ERR_TYPE_CODE : Uint
{
	E_C_EMPTY_STR =						 1 << 0,	//Binary 0000000000000000000  - 1
	E_C_MISSING_SEMICOLON =				 1 << 1,	//Binary 0000000000000000001  - 2
	E_C_MISSING_KEYW_SELECT =			 1 << 2,	//Binary 0000000000000000010  - 4
	E_C_MISSING_KEYW_FROM =				 1 << 3,	//Binary 0000000000000000100  - 8
	E_C_MISSING_KEYW_WHERE =			 1 << 4,	//Binary 0000000000000001000  - 16
	E_C_KEYW_SELECT_MISPLACED =			 1 << 5,    //Binary 0000000000000010000  - 32
	E_C_KEYW_FROM_MISPLACED =			 1 << 6,    //Binary 0000000000000100000  - 64
	E_C_KEYW_WHERE_MISPLACED =			 1 << 7,    //Binary 0000000000001000000  - 128
	E_C_FROM_PART_EMPTY =				 1 << 8,    //Binary 0000000000010000000  - 256
	E_C_TBL_NOT_ON_SCHEMA =				 1 << 9,    //Binary 0000000000100000000  - 512
	E_C_ATRB_NOT_IN_TBL =				 1 << 10,   //Binary 0000000001000000000  - 1024
	E_C_UNRECOGNIZED_TOKEN =			 1 << 11,   //Binary 0000000010000000000  - 2048
	E_C_MISSING_OPERATOR =				 1 << 12,   //Binary v000000100000000000  - 4096
	E_C_TOO_MANY_OPERATORS =			 1 << 13,	//Binary 0000001000000000000  - 8192
	E_C_BOTH_OPERANDS_NOT_CONSTS =		 1 << 14,	//Binary 0000010000000000000  - 16384
	E_C_RHS_OPERAND_NOT_CONST =			 1 << 15,	//Binary 0000100000000000000  - 32768
	E_C_LHS_OPERAND_NOT_CONST =			 1 << 16,	//Binary 0001000000000000000  - 65536
	E_C_INCOMPATIBLE_OPERANDS =			 1 << 17,	//Binary 0010000000000000000  - 131072
	E_C_WHERE_PART_EMPTY =				 1 << 18,	//Binary 0100000000000000000  - 262144
	E_C_SELECT_PART_EMPTY =				 1 << 19,	//Binary 1000000000000000000  - 524288
};

enum ERR_FAMILY : Uint
{
	 ATTR_LIST,
	 TBL_LIST,
	 COND_LIST,
	 GENERAL
};

inline ERR_TYPE_CODE& operator|(ERR_TYPE_CODE& io_a, const ERR_TYPE_CODE& io_b)
{
	return io_a | io_b;
}

inline ERR_TYPE_CODE& operator|=(ERR_TYPE_CODE& io_a, const ERR_TYPE_CODE& io_b)
{
	io_a = io_a | io_b;
	return io_a;
}

class error_tracker
{
	friend class sql_parser;

	typedef struct {
		size_t row;
		size_t col;
	}error_position;

	typedef struct error_detailes {
		ERR_TYPE_CODE error_type_code;
		Uint main_error_family;
		error_position error_pos;	
		string err_msg;
		string err_repair;
		bool is_pos_relevent;
	} ;

	list <error_detailes*> *_err_tracker;
	map <ERR_TYPE_CODE, Uint> _Err_converter;
	string _error_msgs[CURR_AMT_OF_SUPPORTED_ERRORS];
	string _error_fixes[CURR_AMT_OF_SUPPORTED_ERRORS];
	
	void initialize_error_dictionary();
	
public:

	//C'tor
	error_tracker();

	void add_err(ERR_TYPE_CODE i_error_type,
		size_t i_row = 0,
		size_t i_col = 0,
		string i_err_tok1 = "",
		string i_err_tok2 = "",
		string i_err_tok3 = "",
		string i_err_tok4 = "");

	void print_err_list();
	Uint get_first_err_type();
	void remove_duplicate_errors();

	//Getter
	inline size_t amount_of_errors() {
		return _err_tracker->size();
	}

	friend bool inline operator==(const error_position& lhs, const error_position& rhs)
	{
		return ((lhs.col == rhs.col) && (lhs.row == rhs.row));
	}

	~error_tracker();
};

#endif //!ERROR_TRACKER_H_