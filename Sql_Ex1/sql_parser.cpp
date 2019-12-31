#include <fstream>
#include <string>

#include "sql_parser.h"

using namespace std;

sql_parser::sql_parser(string i_scheme_filename)
{
	//If there are AND | OR in this condition, don't return error and return the first OP pos found.

	bool first_err = true;
	_is_schema_file_opened_ok = true;	//The assumption it that schema is ok.

	ifstream input_f(i_scheme_filename, ios::in);

	if (input_f.is_open())
	{
		string input_line;

		while (getline(input_f, input_line))
		{
			if (input_line == "\n" || input_line.empty())
			{
				continue;
			}
		
			size_t l_parent = input_line.find('(');

			if (l_parent != NOT_FOUND && 
				input_line[input_line.length() - IDX_ADJUSTMENT] == ')')
			{
				string table_name = 
					utilities::convert_input_to_lower_case(input_line.substr(STRT_IDX, 
																			 l_parent));
				string field_detailes = input_line.substr(l_parent, NOT_FOUND);
				this->_table_scheme[table_name] = 
					new table_prop_expansion(field_detailes);

				if (this->_table_scheme[table_name]->error_loading_table_detailes())
				{
					if (first_err)
					{
						cerr << "Detected Some errors while loading the schema: " << endl;
						cerr << "======================================================================== " << endl;
					}

					cerr << "Error Reading Attribute: " << field_detailes
						 << " Of table:" << table_name << endl
						 << "Skipping this table!" << endl
						 << "========================================================================" << endl << endl;
					
					this->_table_scheme.erase(table_name);
					this->_is_schema_file_opened_ok = false;
				}
			}
		}
	}
	else 
	{
		cerr << "Error opening file" << endl;
		this->_is_schema_file_opened_ok = false;
	}

	input_f.close();
}

sql_parser::~sql_parser()
{
	cout << endl;
	cout << "==================================================================" << endl;
	cout << "The program is about to End, let's release everything and ger out!" << endl;
	cout << "Press Enter." << endl;
	cout << "==================================================================" << endl;
	cout << endl;
	getchar();

	cout << "Erasing main Dictionary:" << endl;
	cout << "========================" << endl;

	int i = 1;
	for (auto itr = this->_table_scheme.begin(); itr != this->_table_scheme.end();  itr++)
	{
		cout << "Table #" << i << " - '" << itr->first << "': ";
		delete itr->second;
		cout << endl;
		i++;
	}

	this->_table_scheme.clear();
	delete this->_sql_err_trckr;

	cout <<  endl << "Press Enter to Exit..." << endl;
	getchar();
}

void sql_parser::parse(string i_str_to_check)
{
	pre_parse_initialization(i_str_to_check);

	if (!is_command_end_w_semicolon(_i_str_lowcase_no_extra_spaces))
	{
		this->_is_semicolon_present = false;	
		this->_statment_parsing_error_type = E_C_MISSING_SEMICOLON;
		//locate_position_of_error(this->_pos_err_col, 
		//						 this->_pos_err_row,
		//						 _i_str_lowcase_no_extra_spaces);

		this->_sql_err_trckr->add_err(E_C_MISSING_SEMICOLON);
	}
	else //Remove the finishing ";" and continue parsing
	{
		_i_str_lowcase_no_extra_spaces = 
			_i_str_lowcase_no_extra_spaces.substr(STRT_IDX,
			last_significant_pos(_i_str_lowcase_no_extra_spaces));
	}

	//check main Keywords and get positions of all query parts on the way
	determine_str_parse_case();

	//No Errors with main keywords
	if (this->_is_parsed_str_valide_sql)
	{
		if (_statment_parsing_type == M_P_EMPTY_SELECT_SQL_STATMENT)
		{
			return;	//EMPTY select case: exit parsing function and report success.
		}

		validate_table_list();
		validate_attribute_list();
		
		if (_statment_parsing_type == M_P_FULL_SQL_STATMENT)
		{
			validate_conditions_list();
		}
	}

	//Errors with main keywords Detected
	else 
	{
		this->_is_parsed_str_valide_sql = false;
		this->_sql_err_trckr->add_err(_statment_parsing_error_type);
	}

	//IF comma was the only error (found on line 65), still need to mark this query as wrong
	if (this->_is_semicolon_present == false)
	{
		this->_is_parsed_str_valide_sql = false;
	}
}

void sql_parser::validate_table_list()
{
	size_t total_query_len = _i_str_lowcase_no_extra_spaces.length();
	size_t tbl_list_start_idx = _from_idx + FROM_OFFSET_TO_FIRST_TABLE;
	size_t tbl_list_len =

		//the whole length of SQL statment
		total_query_len -

		//Minus the part before FROM, strating from FROM, including all 
		//it's length and 1 whitespace
		(_from_idx + _command[SELECT].length() + ONE_WHITE_SPACE) -

		//Minus the part after tbl lst of FROM, strating from WHERE, 
		//including 1 whitespace before it.
		(total_query_len - (_where_idx + ONE_WHITE_SPACE));

	string raw_table_list = _i_str_lowcase_no_extra_spaces.substr(tbl_list_start_idx,
																  tbl_list_len);
	//Case of empty table list
	if (tbl_list_start_idx == _where_idx)
	{
		locate_position_of_error(this->_pos_err_col, this->_pos_err_row, raw_table_list);
		this->_sql_err_trckr->add_err(E_C_FROM_PART_EMPTY, this->_pos_err_row, this->_pos_err_col);
		this->_is_parsed_str_valide_sql = false;
	}

	//Remove all whitespaces before and after comma
	raw_table_list = utilities::rm_wtsp_bef_and_aft_comma(raw_table_list);

	//Validate all tables Recursively
	validate_table_list_rec(raw_table_list);
}

void sql_parser::validate_table_list_rec(string i_raw_tbl_lst)
{
	string current_table_name;
	bool table_not_found = false;

	size_t curr_comma_idx = i_raw_tbl_lst.find(COMMA);
	
	////There is comma separation - probably more than one table
	if (curr_comma_idx != NOT_FOUND)
	{
		current_table_name = i_raw_tbl_lst.substr(0, curr_comma_idx);

		//Table found
		if (_table_scheme.find(current_table_name) != _table_scheme.end())
		{
			_table_list_in_curr_sql.emplace_back(current_table_name);
		}
		//Table Not found
		else
		{
			table_not_found = true;
		}

		//Recursive call, to get all the tables
		validate_table_list_rec(i_raw_tbl_lst.substr(curr_comma_idx + 1));
	}
	
	//no comma separation - probably: last table \ one table \ error
	else
	{
		current_table_name = i_raw_tbl_lst;

		//Table found
		if (_table_scheme.find(current_table_name) != _table_scheme.end())
		{
			_table_list_in_curr_sql.emplace_back(current_table_name);
		}
		//Table Not found
		else
		{
			table_not_found = true;
		}
	}

	if (table_not_found == true)
	{
		locate_position_of_error(this->_pos_err_col, this->_pos_err_row, current_table_name);
		this->_sql_err_trckr->add_err(E_C_TBL_NOT_ON_SCHEMA, 
									  this->_pos_err_row, 
									  this->_pos_err_col, 
									  current_table_name);
		this->_is_parsed_str_valide_sql = false;
		table_not_found = false;
	}
}

void sql_parser::validate_attribute_list()
{
	size_t total_query_len = _i_str_lowcase_no_extra_spaces.length();
	size_t atr_list_start_idx;

	//Treatment of DISTINCT clause
	if (_distinct_idx == NOT_FOUND)
	{
		atr_list_start_idx = _select_idx + SELECT_OFFSET_TO_FIRST_ATR_NO_DISTINCT;
	}
	else
	{
		atr_list_start_idx = _select_idx + SELECT_OFFSET_TO_FIRST_ATR_WITH_DISTINCT;
		this->_is_distinct_cmd_present = true;
	}
	
	size_t atr_list_length =

		//the whole length of SQL statment
		total_query_len -

		//Minus the first part, trating from SELECT and till DISTINCT + 1(whitespace)
		atr_list_start_idx -

		//Minus the part after attribute list of SELECT, strating at FROM and 
		//including 1 whitespace
		(total_query_len - (_from_idx - 1));

	string raw_atrb_list = _i_str_lowcase_no_extra_spaces.substr(atr_list_start_idx,
																 atr_list_length);
	//Case of empty attribute list
	if (atr_list_start_idx == _from_idx)
	{
		locate_position_of_error(this->_pos_err_col, this->_pos_err_row, raw_atrb_list);
		this->_sql_err_trckr->add_err(E_C_SELECT_PART_EMPTY, this->_pos_err_row, this->_pos_err_col);
		this->_is_parsed_str_valide_sql = false;
	}

	//Remove all whitespaces before and after comma
	raw_atrb_list = utilities::rm_wtsp_bef_and_aft_comma(raw_atrb_list);

	if (raw_atrb_list.length() == ONE_CHAR_LENGTH_STRING && raw_atrb_list[0] =='*')
	{
		this->_is_asterix_cmd_in_select_present = true;
	}
	else
	{
		//Validate all attributes Recursively
		validate_attribute_list_rec(raw_atrb_list);
	}
}

void sql_parser::validate_attribute_list_rec(string i_raw_atrb_lst)
{
	string curr_tbl_name, curr_atr_name, curr_tbl_AND_atr_name;
	bool table_not_found = false;
	bool atrb_not_found = false;
	bool wrong_input = false;

	size_t curr_comma_idx = i_raw_atrb_lst.find(COMMA);

	////There is comma separation - probably more than one table
	if (curr_comma_idx != NOT_FOUND)
	{
		curr_tbl_AND_atr_name = i_raw_atrb_lst.substr(STRT_IDX, curr_comma_idx);
		size_t curr_dot_idx = curr_tbl_AND_atr_name.find(DOT);

		if (curr_dot_idx != NOT_FOUND)
		{
			curr_tbl_name = curr_tbl_AND_atr_name.substr(STRT_IDX, curr_dot_idx);
			curr_atr_name = curr_tbl_AND_atr_name.substr(curr_dot_idx + IDX_ADJUSTMENT);

			//table found
			if (_table_scheme.find(curr_tbl_name) != _table_scheme.end())
			{
				//attribute found
				if (_table_scheme[curr_tbl_name]->_attribute_details.find(curr_atr_name) !=
					_table_scheme[curr_tbl_name]->_attribute_details.end())
				{
				
					_attrb_list_in_curr_sql.emplace_back(curr_atr_name);

				}
				//attribute NOT found
				else
				{
					atrb_not_found = true;
				}
			}
			//table NOT found
			else
			{
				table_not_found = true;
			}
		}
		else
		{
			wrong_input = true;
		}

		//Recursive call, to get all the attributes
		validate_attribute_list_rec(i_raw_atrb_lst.substr(curr_comma_idx + IDX_ADJUSTMENT));
	}

	//no comma separation - probably: last table \ one table \ error
	else
	{
		curr_tbl_AND_atr_name = i_raw_atrb_lst;

		size_t curr_dot_idx = curr_tbl_AND_atr_name.find(DOT);

		if (curr_dot_idx != NOT_FOUND)
		{
			curr_tbl_name = curr_tbl_AND_atr_name.substr(STRT_IDX, curr_dot_idx);
			curr_atr_name = curr_tbl_AND_atr_name.substr(curr_dot_idx + IDX_ADJUSTMENT);

			//table found
			if (_table_scheme.find(curr_tbl_name) != _table_scheme.end())
			{
				//attribute found
				if (_table_scheme[curr_tbl_name]->_attribute_details.find(curr_atr_name) !=
					_table_scheme[curr_tbl_name]->_attribute_details.end())
				{

					_attrb_list_in_curr_sql.emplace_back(curr_atr_name);
				}
				//attribute NOT found
				else
				{
					atrb_not_found = true;
				}
			}
			//table NOT found
			else
			{
				table_not_found = true;
			}
		}
		else
		{
			wrong_input = true;
		}
	}

	if (table_not_found == true || wrong_input == true || atrb_not_found == true)
	{
		locate_position_of_error(this->_pos_err_col, this->_pos_err_row, curr_tbl_AND_atr_name);

		if(wrong_input)
		{
			this->_sql_err_trckr->add_err(E_C_UNRECOGNIZED_TOKEN,
									      this->_pos_err_row,
										  this->_pos_err_col, 
										  curr_tbl_AND_atr_name);
		}

		if (table_not_found)
		{
			this->_sql_err_trckr->add_err(E_C_TBL_NOT_ON_SCHEMA,
										  this->_pos_err_row,
										  this->_pos_err_col,
										  curr_tbl_name);
		}
		
		if (atrb_not_found)
		{
			this->_sql_err_trckr->add_err(E_C_ATRB_NOT_IN_TBL,
									   	  this->_pos_err_row,
										  this->_pos_err_col,
										  curr_atr_name,
										  curr_tbl_name);
		}

		this->_is_parsed_str_valide_sql = false;

		table_not_found = false;
		wrong_input == false;
		atrb_not_found == false;
	}
}

bool sql_parser::validate_simple_condition(string i_smpl_cond_to_chck)
{
	bool is_valid_op = true;						//We assumpt that input is ok
	bool is_left_part_const = true;					//We assumpt that input is ok
	bool is_right_part_const = true;				//We assumpt that input is ok
	bool is_compatible_operandstype = true;			//We assumpt that input is ok
	string op1_type = "", op2_type = "";
	string oper_type;
	size_t oper_idx;
	OP_RESULT oper_check_result;
	ERR_TYPE_CODE err_in_simple_cond;
	Uint oper_length;
	
	oper_check_result = locate_operator(i_smpl_cond_to_chck, 
									    oper_type, 
		                                oper_idx, 
		                                oper_length);
	if (oper_check_result == OP_FOUND)
	{
		is_left_part_const = 
		check_if_constant(utilities::rm_all_wtsp(i_smpl_cond_to_chck.substr(STRT_IDX, oper_idx)),op1_type);
		is_right_part_const = 
		check_if_constant(utilities::rm_all_wtsp(i_smpl_cond_to_chck.substr(oper_idx + oper_length)),op2_type);
		is_compatible_operandstype = check_operands_compatibility(op1_type, op2_type, oper_type);
	}
	else
	{
		is_valid_op = false;
	}

	if (is_valid_op && is_left_part_const && is_right_part_const && is_compatible_operandstype)
	{
		return true;
	}
	else
	{
		if (!is_valid_op)
		{
			if (oper_check_result == NO_OP_FOUND)
			{
				err_in_simple_cond = E_C_MISSING_OPERATOR;
			}
			else if (oper_check_result == TOO_MANY_OP_FOUND)
			{
				err_in_simple_cond = E_C_TOO_MANY_OPERATORS;
			}
		}

		else if (!is_left_part_const && !is_right_part_const)
		{
			err_in_simple_cond = E_C_BOTH_OPERANDS_NOT_CONSTS;
		}
		else if (!is_left_part_const)
		{
			err_in_simple_cond = E_C_LHS_OPERAND_NOT_CONST;
		}
		else if (!is_right_part_const)
		{
			err_in_simple_cond = E_C_RHS_OPERAND_NOT_CONST;
		}
		else
		{
			err_in_simple_cond = E_C_INCOMPATIBLE_OPERANDS;
		}

		locate_position_of_error(this->_pos_err_col, this->_pos_err_row, i_smpl_cond_to_chck);
		this->_sql_err_trckr->add_err(err_in_simple_cond, 
			this->_pos_err_row, 
			this->_pos_err_col, 
			i_smpl_cond_to_chck,
			oper_type,
			op1_type,
			op2_type);

		this->_is_parsed_str_valide_sql = false;
		return false;
	}
}

bool sql_parser::check_operands_compatibility(string i_op1_type, string i_op2_type, string i_oper_type)
{
	if (i_op1_type != i_op2_type)
	{
		return false;
	}
	else
	{
		return true;
	}
	//In the future, to make dictionary of all possible types, converions and casting 
	//between different types and logical operations allowed between them.
}

void sql_parser::validate_conditions_list()
{
	size_t cond_list_start_idx = _where_idx + WHERE_OFFSET_TO_FIRST_COND;

	string raw_cond_list = _i_str_lowcase_no_extra_spaces.substr(cond_list_start_idx);

	//Case of empty table list
	if (cond_list_start_idx == last_significant_pos(this->_i_str_lowcase_no_extra_spaces))
	{
		locate_position_of_error(this->_pos_err_col, this->_pos_err_row, raw_cond_list);
		this->_sql_err_trckr->add_err(E_C_WHERE_PART_EMPTY, this->_pos_err_row, this->_pos_err_col);
		this->_is_parsed_str_valide_sql = false;
	}

	//Validate all Conditions Recursively
	validate_conditions_list_rec(raw_cond_list);
	
	//Remove duplicate errors if exist
	this->_sql_err_trckr->remove_duplicate_errors();
}

bool sql_parser::validate_conditions_list_rec(string i_cond_list)
{
	bool in_brackets_cond = false;
	bool AND_cond = false;
	bool OR_cond = false;
	bool complex_condition = false;
	size_t and_idx, or_idx, or_actual_start_idx;
	Uint amt_of_err_after_check;
	Uint amt_of_err_before_check = this->_sql_err_trckr->amount_of_errors();

	if ((i_cond_list[STRT_IDX] == '(' && i_cond_list[last_significant_pos(i_cond_list)] == ')'))
	{
		complex_condition = true;
		
		if ((this->validate_conditions_list_rec(i_cond_list.substr(IDX_ADJUSTMENT,
			last_significant_pos(i_cond_list) - IDX_ADJUSTMENT))))
		{
			in_brackets_cond = true;
		}
	}
	
	and_idx = find_log_operator(this->_command[AND], i_cond_list);
	or_idx = find_log_operator(this->_command[OR], i_cond_list);

	if (and_idx != NOT_FOUND)
	{
		complex_condition = true;
																			//Somehow the || op works and get 
																			//more accurate results than &&
		if (validate_conditions_list_rec(i_cond_list.substr(STRT_IDX, and_idx - 1)) &&
			validate_conditions_list_rec(i_cond_list.substr(and_idx + this->_command[AND].length() + 1)))
		{
			AND_cond = true;
		}
	}

	if (or_idx != NOT_FOUND)
	{
		complex_condition = true;

		or_actual_start_idx = and_idx != NOT_FOUND ?
			and_idx + this->_command[OR].length() + 1 : STRT_IDX;
																			//Somehow the || op works and get 
																			//more accurate results than &&
		if (validate_conditions_list_rec(i_cond_list.substr(or_actual_start_idx, or_idx - 1)) &&
			validate_conditions_list_rec(i_cond_list.substr(or_idx + this->_command[OR].length() + 1)))
		{
			OR_cond = true;
		}
	}

	// Possibilities @ this point of the recursion:
		//1. If all the boolean flags of complex conditions are false at this point,
		//   there are 2 possibilities:
		//   A||: We didn't enter any of the complex conditions, because the input is actually a
		//   simple cond.
		//   The indicator for this will be the "complex" flag that will stay false.
		//	 So, in this case - need to check the input as a simple condition.
		
		//   B||: We entered at least one of the complex conditions, but the check returned false.
		//   The indicator for this will be the "complex" flag that will be set as true.
		//	 So, in this case - No need to check the input as a simple condition.
			
		//2. If at least one of the complex conditions came back true - we need to discard
		//   all the errors that the algorithm found during the visit at the lower levels of 
		//   the recursion tree (beneath this node) because @ this point we know for sure that
		//	 there is c correct parsing path, that we missed @ the earlier visit.

	if (in_brackets_cond || AND_cond || OR_cond)
	{
		amt_of_err_after_check = this->_sql_err_trckr->amount_of_errors();

		if (amt_of_err_after_check != amt_of_err_before_check)
		{
			for (size_t i = 0; i < (amt_of_err_after_check - amt_of_err_before_check); i++)
			{
				this->_sql_err_trckr->_err_tracker->pop_back();
			}
		}

		return true;
	}
	else
	{
		if (!complex_condition)
		{
			return this->validate_simple_condition(i_cond_list);
		}
		else
		{
			return false;
		}
	}
}

bool sql_parser::check_if_attrb(string i_str_to_chck, string& o_type)
{
	bool is_attribute = false; //Assume it's Not attribute
	string curr_tbl_name, curr_atr_name, curr_tbl_AND_atr_name;

	curr_tbl_AND_atr_name = i_str_to_chck;

	size_t curr_dot_idx = curr_tbl_AND_atr_name.find(DOT);

	if (curr_dot_idx != NOT_FOUND)
	{
		curr_tbl_name = curr_tbl_AND_atr_name.substr(STRT_IDX, curr_dot_idx);
		curr_atr_name = curr_tbl_AND_atr_name.substr(curr_dot_idx + IDX_ADJUSTMENT);

		//table found
		if (this->_table_scheme.find(curr_tbl_name) != this->_table_scheme.end())
		{
			//attribute found
			if (this->_table_scheme[curr_tbl_name]->_attribute_details.find(curr_atr_name) !=
				this->_table_scheme[curr_tbl_name]->_attribute_details.end())
			{
				is_attribute = true;
				o_type = this->_table_scheme[curr_tbl_name]->_attribute_details.find(curr_atr_name)->second;
			}
		}
	}

	return is_attribute;
}

bool sql_parser::check_if_constant(string i_str_to_chck, string& o_atrb_type)
{
	bool is_int_operand = utilities::is_int(i_str_to_chck);
	bool is_string_operand = is_string(i_str_to_chck);
	bool atrb_check = check_if_attrb(i_str_to_chck, o_atrb_type);

	if (is_int_operand)
	{
		o_atrb_type = "integer";
		return true;
	}
	if (is_string_operand)
	{
		o_atrb_type = "string";
		return true;
	}
	if (atrb_check)
	{
		return true;
	}
	
	return false;
}

void sql_parser::pre_parse_initialization(string i_str_to_store)
{
	this->_is_parsed_str_valide_sql = true;	//The assumption is that input is ok.
	this->_is_semicolon_present = true;			//The assumption is that input is ok.
	this->_statment_parsing_type = M_P_NOT_YET_DEFINED;
	this->_i_original_str = i_str_to_store;
	this->_i_str_lowcase = utilities::convert_input_to_lower_case(_i_original_str);
	this->_i_str_lowcase_no_extra_spaces = utilities::rm_dup_wtsp(_i_str_lowcase);
	this->_sql_err_trckr = new error_tracker();
	this->_select_idx = -1;
	this->_from_idx = -1;
	this->_where_idx = -1;
	this->_distinct_idx = -1;
	this->_is_distinct_cmd_present = false;
	this->_is_asterix_cmd_in_select_present = false;
	this->initialize_command_dictionary();
	this->initialize_op_dictionary();
}

void sql_parser::determine_str_parse_case()
{
	size_t i_str_length = _i_str_lowcase_no_extra_spaces.length();

	this->_select_idx = _i_str_lowcase_no_extra_spaces.find(_command[SELECT]);
	this->_from_idx = _i_str_lowcase_no_extra_spaces.find(_command[FROM]);
	this->_where_idx = _i_str_lowcase_no_extra_spaces.find(_command[WHERE]);
	this->_distinct_idx = _i_str_lowcase_no_extra_spaces.find(_command[DISTINCT]);

	//CASE: Empty SQL input string
	if (i_str_length == 0)
	{
		this->_statment_parsing_error_type = E_C_EMPTY_STR;
		return;
	}

	//CASE: missing Keywords: SELECT, FROM, WHERE
	if (_select_idx == NOT_FOUND && _from_idx == NOT_FOUND && _where_idx == NOT_FOUND)
	{
		this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
											(E_C_MISSING_KEYW_SELECT |
											 E_C_MISSING_KEYW_FROM   |
											 E_C_MISSING_KEYW_WHERE);
		this->_is_parsed_str_valide_sql = false;
		return;
	}

	//CASE: missing Keywords: FROM (WHERE isn't mandatory)
	if (_select_idx != NOT_FOUND && _from_idx == NOT_FOUND)
	{
		//Empty SELECT is OK by Language Definitions
		if (i_str_length == _command[SELECT].length())
		{
			this->_statment_parsing_type = M_P_EMPTY_SELECT_SQL_STATMENT;
			return;
		}
		else
		{
			this->_statment_parsing_error_type = E_C_MISSING_KEYW_FROM;
			this->_is_parsed_str_valide_sql = false;
			return;
		}
	}

	//CASE: missing Keywords: SELECT (WHERE isn't mandatory)
	if (_select_idx == NOT_FOUND && _from_idx != NOT_FOUND)
	{
		this->_statment_parsing_error_type = E_C_MISSING_KEYW_SELECT;
		this->_is_parsed_str_valide_sql = false;
		return;
	}

	//CASE: missing Keywords: SELECT, FROM
	if (_select_idx == NOT_FOUND && _from_idx == NOT_FOUND && _where_idx != NOT_FOUND)
	{
		this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
											 (E_C_MISSING_KEYW_SELECT |
											  E_C_MISSING_KEYW_FROM);
		this->_is_parsed_str_valide_sql = false;
		return;
	}

	//CASE: missing Keywords: WHERE and with\out Misplaced SELECT & FROM
	if (_select_idx != NOT_FOUND && _from_idx != NOT_FOUND && _where_idx == NOT_FOUND)
	{
		if (_from_idx < _select_idx + MIN_DIST_BETWEEN_FROM_AND_SELECT)
		{
			this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
								 				 (E_C_KEYW_SELECT_MISPLACED |
												  E_C_KEYW_FROM_MISPLACED);
		}
		else //there is at least one space between SELECT & FROM
		{
			this->_statment_parsing_type = M_P_NO_WHERE_SQL_STATMENT;
			return;
		}

		this->_is_parsed_str_valide_sql = false;
	}

	//CASE: missing Keywords: FROM and with\out Misplaced SELECT & WHERE
	if (_select_idx != NOT_FOUND && _from_idx == NOT_FOUND && _where_idx != NOT_FOUND)
	{
		//there is no space between SELECT & WHERE or WHERE comes before SELECT
		if (_where_idx < _select_idx + MIN_DIST_BETWEEN_SELECT_AND_WHERE)
		{
			this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
												 (E_C_KEYW_SELECT_MISPLACED |
												  E_C_KEYW_WHERE_MISPLACED);
		}

		this->_statment_parsing_error_type |= E_C_MISSING_KEYW_FROM;
		this->_is_parsed_str_valide_sql = false;
		return;
	}

	//CASE: missing Keywords: SELECT and with\out Misplaced WHERE & FROM - case changed?!
	if (_select_idx == NOT_FOUND && _from_idx != NOT_FOUND && _where_idx != NOT_FOUND)
	{
		if (_where_idx < _from_idx + MIN_DIST_BETWEEN_WHERE_AND_FROM)
		{
			this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
												 (E_C_KEYW_FROM_MISPLACED |
												  E_C_KEYW_WHERE_MISPLACED);
		}
		
		this->_statment_parsing_error_type |= E_C_MISSING_KEYW_SELECT;
		this->_is_parsed_str_valide_sql = false;
		return;
	}

	 //==================================================\\
	// If we got here ->>> all the 3 Keywords are present \\
   //======================================================\\

	//Full and in correct order SQL input string
	if (_where_idx >= _from_idx + MIN_DIST_BETWEEN_WHERE_AND_FROM &&
		_from_idx >= _select_idx + MIN_DIST_BETWEEN_FROM_AND_SELECT)
	{
		this->_statment_parsing_type = M_P_FULL_SQL_STATMENT;
		return;
	}

	//CASE: not in correct order WHERE <> FROM
	if (_from_idx >= _select_idx + MIN_DIST_BETWEEN_FROM_AND_SELECT &&
		_where_idx < _from_idx + MIN_DIST_BETWEEN_WHERE_AND_FROM)
	{
		this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
											 (E_C_KEYW_FROM_MISPLACED |
											  E_C_KEYW_WHERE_MISPLACED);
		
		this->_is_parsed_str_valide_sql = false;
		return;
	}

	//CASE: not in correct order FROM <> SELECT
	if (_from_idx < _select_idx + MIN_DIST_BETWEEN_FROM_AND_SELECT &&
		_where_idx >= _from_idx + MIN_DIST_BETWEEN_WHERE_AND_FROM)
	{
		this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
											 (E_C_KEYW_SELECT_MISPLACED |
											  E_C_KEYW_FROM_MISPLACED);
		
		this->_is_parsed_str_valide_sql = false;
		return;
	}

	//CASE: not in correct order WHERE <> SELECT
	if (_from_idx < _select_idx + MIN_DIST_BETWEEN_FROM_AND_SELECT &&
		_where_idx < _from_idx + MIN_DIST_BETWEEN_WHERE_AND_FROM)
	{
		this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
											 (E_C_KEYW_SELECT_MISPLACED |
											  E_C_KEYW_WHERE_MISPLACED);
		
		this->_is_parsed_str_valide_sql = false;
		return;
	}

	 //=====================================================================\\
	// If we got here ->>> all the 3 Keywords are present, But all Misplaced \\
   //=========================================================================\\

	this->_statment_parsing_error_type = static_cast<ERR_TYPE_CODE>
													(E_C_KEYW_SELECT_MISPLACED |
													 E_C_KEYW_FROM_MISPLACED   |
 													 E_C_KEYW_WHERE_MISPLACED);
	this->_is_parsed_str_valide_sql = false;
	return;
}

void sql_parser::initialize_command_dictionary()
{
	this->_command = {
		{SELECT,	"select"   },
		{FROM,		"from"	   },
		{WHERE,		"where"	   },
		{DISTINCT,  "distinct" },
		{AND,		"and"      },
		{OR,		"or"	   }
	};
}

void sql_parser::initialize_op_dictionary()
{
	this->_op = {

	{"=" ,	EQUAL_TO				 },
	{">" ,	GREATER_THAN			 },
	{"<" ,	LESS_THAN				 },
	{ "<>",  NOT_EQUAL_TO			 },
	{">=",	GREATER_THAN_OR_EQUAL_TO },
	{"<=",	LESS_THAN_OR_EQUAL_TO	 }
	};
}

sql_parser::OP_RESULT sql_parser::locate_operator(string i_str_to_check,
												  string& o_op,
												  size_t& o_idx,
												  Uint& o_length)
{
	OP_RESULT result;
	
	//in case there is error in string and there are too many.
	Uint counter_of_ops_found = 0;	
	size_t idx = 0;

	while (i_str_to_check[idx] != '\0')
	{
		map<string, OP_KEYWORD>::iterator itr_unary =
			_op.find(i_str_to_check.substr(idx, ONE_CHAR));
		map<string, OP_KEYWORD>::iterator itr_binary = 
			_op.find(i_str_to_check.substr(idx, TWO_CHARS));

		if (itr_unary == _op.end()) { /* Not found - Move on */ }
		else
		{
			switch (itr_unary->second)
			{
				case(EQUAL_TO):						// "="
						o_op = "=";
						o_idx = idx;
						counter_of_ops_found++;
						o_length = ONE_CHAR;
					break;
				case(GREATER_THAN):					// ">" | ">="
						if (itr_binary != _op.end() && 
							itr_binary->second == GREATER_THAN_OR_EQUAL_TO)
						{
							o_op = ">=";
							o_idx = idx;
							counter_of_ops_found++;
							o_length = TWO_CHARS;
							idx++;	//skip one more charr
						}
						else
						{
							o_op = ">";
							o_idx = idx;
							counter_of_ops_found++;
							o_length = ONE_CHAR;
						}
					break;

				case(LESS_THAN):					// "<" | "<>" | "<="
					if (itr_binary != _op.end() && 
						(itr_binary->second == LESS_THAN_OR_EQUAL_TO ||
						itr_binary->second == NOT_EQUAL_TO))
						{
							o_idx = idx;
							counter_of_ops_found++;
							o_length = TWO_CHARS;
							idx++;	//skip one more charr
							o_op = _op[i_str_to_check.substr(idx, TWO_CHARS)] == 
								LESS_THAN_OR_EQUAL_TO ? "<=" : "<>";
						}
						else
						{
							o_op = "<";
							o_idx = idx;
							counter_of_ops_found++;
							o_length = ONE_CHAR;
						}
					break;
			}
		}
		idx++;
	}

	result = counter_of_ops_found > OP_FOUND ? TOO_MANY_OP_FOUND : 
			  counter_of_ops_found ==  OP_FOUND ? OP_FOUND : NO_OP_FOUND;

	return result;
}

void sql_parser::locate_position_of_error(Uint & o_col,
										  Uint & o_row, 
										  string i_current_token)
{
	size_t idx_of_new_line = 0;
	size_t next_idx = 0;
	size_t last_starting_new_row_col_pos = 1;
	o_col = 1;
	o_row = 1;
	
	//dana kama

	//if they are equal - the current token is the whole SQL statment, so col=row=0 it's enough
	if (i_current_token.length() != _i_str_lowcase_no_extra_spaces.length())
	{
		o_col = _i_str_lowcase.find(i_current_token);

		while ((idx_of_new_line = _i_str_lowcase.find('\n', next_idx)) != NOT_FOUND)
		{
			if (idx_of_new_line <= o_col)
			{
				o_row++;
				next_idx = ++idx_of_new_line;
				last_starting_new_row_col_pos = idx_of_new_line;
			}
			else
			{
				break;
			}
		}
		if (o_row > 1)
		{
			o_col -= last_starting_new_row_col_pos;
		}
	}
}

size_t sql_parser::find_log_operator(string i_op_to_look, string i_look_in)
{
	size_t result_idx = -1;
	bool found = false;

	while(!found)
	{	
		result_idx++;
		result_idx = i_look_in.find(i_op_to_look, result_idx);
		
		if (result_idx == NOT_FOUND)
		{
			break;
		}

		found = i_look_in[result_idx + i_op_to_look.length()] == ' ' &&
			i_look_in[result_idx - 1] == ' ' ? true : false;
	}

	result_idx = found ? result_idx : NOT_FOUND;
	
	return result_idx;
}