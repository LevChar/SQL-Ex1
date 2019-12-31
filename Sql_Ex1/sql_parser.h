#ifndef SQL_PARSER_H_
#define SQL_PARSER_H_

#include "table_prop_expansion.h"
#include "error_tracker.h"
#include "utilities.h"

#define MIN_DIST_BETWEEN_FROM_AND_SELECT 7
#define MIN_DIST_BETWEEN_SELECT_AND_WHERE 7
#define MIN_DIST_BETWEEN_WHERE_AND_FROM 5
#define FROM_OFFSET_TO_FIRST_TABLE 5
#define SELECT_OFFSET_TO_FIRST_ATR_NO_DISTINCT 7
#define SELECT_OFFSET_TO_FIRST_ATR_WITH_DISTINCT 16
#define WHERE_OFFSET_TO_FIRST_COND 6

class sql_parser
{
	//Parse related Enumerations
	enum MAIN_PARSE_CASE : unsigned char
	{
		M_P_NOT_YET_DEFINED,
		M_P_EMPTY_SELECT_SQL_STATMENT,
		M_P_NO_WHERE_SQL_STATMENT,
		M_P_FULL_SQL_STATMENT
	};

	enum COMMAND_KEYWORD : unsigned char
	{
		SELECT,
		FROM,
		WHERE,
		DISTINCT,
		AND,
		OR
	};

	enum OP_KEYWORD : unsigned char
	{
		EQUAL_TO,					// a  =   b
		NOT_EQUAL_TO,				// a  <>  b
		GREATER_THAN,				// a  >   b
		LESS_THAN,					// a  <   b
		GREATER_THAN_OR_EQUAL_TO,	// a  >=  b
		LESS_THAN_OR_EQUAL_TO,	    // a  <=  b
	};

	enum OP_RESULT : unsigned char
	{
		NO_OP_FOUND,
		OP_FOUND,
		TOO_MANY_OP_FOUND
	};

	//Private Members
	map <string, table_prop_expansion*> _table_scheme;
	map<COMMAND_KEYWORD, string> _command;
	map<string, OP_KEYWORD> _op;
	list<string> _attrb_list_in_curr_sql;				//For future use
	list<string> _table_list_in_curr_sql;				//For future use
	bool _is_schema_file_opened_ok;
	bool _is_parsed_str_valide_sql;
	bool _is_semicolon_present;
	bool _is_distinct_cmd_present;						//Currently just checked, but not used
	bool _is_asterix_cmd_in_select_present;				//Currently just checked, but not used
	string _i_original_str;
	string _i_str_lowcase;
	string _i_str_lowcase_no_extra_spaces;
	Uint _pos_err_col, _pos_err_row;
	size_t _select_idx, _from_idx, _where_idx, _distinct_idx;
	MAIN_PARSE_CASE _statment_parsing_type;
	ERR_TYPE_CODE _statment_parsing_error_type;
	error_tracker *_sql_err_trckr;

	//Private methods
	void determine_str_parse_case();
	void initialize_command_dictionary();
	void pre_parse_initialization(string i_str_to_store);
	void locate_position_of_error(Uint &o_col, Uint &o_row, string i_current_token);
	void validate_table_list();
	void validate_table_list_rec(string i_raw_tbl_lst);
	void validate_attribute_list();
	void validate_attribute_list_rec(string i_raw_atrb_lst);
	void initialize_op_dictionary();
	void validate_conditions_list();
	bool validate_conditions_list_rec(string i_cond_list);
	bool check_if_attrb(string i_str_to_chck, string& o_type);
	bool check_if_constant(string i_str_to_chck, string& o_atrb_type);
	bool validate_simple_condition(string i_smpl_cond_to_chck);
	bool check_operands_compatibility(string i_op1_type, string i_op2_type, string i_oper_type);
	OP_RESULT locate_operator(string i_str_to_check, string& o_op, size_t& o_idx, Uint& o_length);
	size_t find_log_operator(string i_op_to_look, string i_look_in);

public:

	//Main API's
	sql_parser(string i_scheme_filename);
	void parse(string i_str_to_check);
	
	//Getters
	inline bool is_parsed_str_valide_sql() {
		return this->_is_parsed_str_valide_sql;
	}

	inline bool is_schema_file_opened_ok() {
		return this->_is_schema_file_opened_ok;
	}

	inline error_tracker* err_tracker() {
		return _sql_err_trckr;
	}

	~sql_parser();
};

#endif //!SQL_PARSER_H_