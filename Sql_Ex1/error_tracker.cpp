#include <iostream>
#include <string>
#include "error_tracker.h"

using std::cout;
using std::endl;

error_tracker::error_tracker() 
{ 
	_err_tracker = new list <error_detailes*>();
	initialize_error_dictionary();

#pragma region ERROR - MSGS:
	//No-Inserted parametters
	_error_msgs[0] = R"Errorstr(
SQL query is empty, nothing to process.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[1] = R"Errorstr(
SQL Query is missing  ';'.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[2] = R"Errorstr(
SQL query missing main keywords: SELECT, FROM, WHERE.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[3] = R"Errorstr(
SQL query missing 'FROM' keyword. Can't locate the source of requested data.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[4] = R"Errorstr(
SQL query missing 'SELECT' keyword. Nothing to process.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[5] = R"Errorstr(
SQL query missing main keywords: SELECT, FROM.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[6] = R"Errorstr(
SQL query main keywords SELECT and FROM are misplaced.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[7] = R"Errorstr(
SQL query main keywords SELECT and WHERE are misplaced.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[8] = R"Errorstr(
SQL query main keywords SELECT and WHERE are misplaced, FROM keyword missing.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[9] = R"Errorstr(
SQL query main keywords FROM and WHERE are misplaced.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[10] = R"Errorstr(
SQL query main keywords FROM and WHERE are misplaced,  SELECT keyword missing.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[11] = R"Errorstr(
SQL query main keywords SELECT, FROM and WHERE are misplaced.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[12] = R"Errorstr(
Query's 'FROM' part is empty.
)Errorstr";

	//Inserted parametter: Table Name, @ index: 9
	_error_msgs[13] = R"Errorstr(
Table: [] Wasn't found on current Schema.
)Errorstr";

	//Inserted parametters: Attribute Name, @ index: 13, Table Name, @ index: 39
	_error_msgs[14] = R"Errorstr(
Attribute: [] Wasn't found in table: [].
)Errorstr";

	//No-Inserted parametters
	_error_msgs[15] = R"Errorstr(
Unrecognized Token.
)Errorstr";

	//Inserted parametter: Condition, @ index: 28
	_error_msgs[16] = R"Errorstr(
No operator in condition: [].
)Errorstr";

	//Inserted parametter: Condition, @ index: 35
	_error_msgs[17] = R"Errorstr(
Too Many operators in condition: [].
)Errorstr";

	//Inserted parametters: Operator type, @ index: 26, Condition, @ index: 43
	_error_msgs[18] = R"Errorstr(
Both sides of operator: [] in condition: [] aren't constants.
)Errorstr";

	//Inserted parametters: Operator type, @ index: 26, Condition, @ index: 43
	_error_msgs[19] = R"Errorstr(
Right side of operator: [] in condition: [] isn't constant.
)Errorstr";

	//Inserted parametters: Operator type, @ index: 25, Condition, @ index: 42
	_error_msgs[20] = R"Errorstr(
left side of operator: [] in condition: [] isn't constant.
)Errorstr";

	//Inserted parametters: Operator type, @ indexes: 35,43 Condition, @ index: 61
	_error_msgs[21] = R"Errorstr(
Operand types are Incompatible: ([]  and []) in condition: [].
)Errorstr";

	//No-Inserted parametters
	_error_msgs[22] = R"Errorstr(
Query's 'WHERE' part is empty.
)Errorstr";

	//No-Inserted parametters
	_error_msgs[23] = R"Errorstr(
Query's 'SELECT' part is empty
)Errorstr";

#pragma endregion

#pragma region ERROR - FIX-SUGGESTIONS:
	//No-Inserted parametters
	_error_fixes[0] = R"Errorstr(
Insert valid SQL query and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[1] = R"Errorstr(
Each SQL query must end with semicolon.
Insert at the end and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[2] = R"Errorstr(
In case of non-empty 'SELECT', Valid SQL query must contain at least 'SELECT'
and 'FROM' keywords, together with list of tables and attributes. 
Complete  missing parts and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[3] = R"Errorstr(
In case of non-empty 'SELECT', Valid SQL query should contain 'FROM' keyword,
together with list of tables which comes after 'FROM'.
Complete  missing parts and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[4] = R"Errorstr(
Each SQL query must contain at least 'SELECT' keyword.
Insert valid SQL query and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[5] = R"Errorstr(
In case of non-empty 'SELECT', Valid SQL query must contain at least 'SELECT'
and 'FROM' keywords, together with list of tables and attributes.
Complete  missing parts and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[6] = R"Errorstr(
SQL syntax order: SELECT <attributes_list> FROM <table_list> 
{WHERE <conditions_list>}.
Rearrange order of elements inside query and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[7] = R"Errorstr(
SQL syntax order: SELECT <attributes_list> FROM <table_list>
 {WHERE <conditions_list>}.
Rearrange order of elements inside query and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[8] = R"Errorstr(
SQL syntax order: SELECT <attributes_list> FROM <table_list>
{WHERE <conditions_list>}.
Rearrange order of elements inside query, add missing FROM part,
together with list of tables and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[9] = R"Errorstr(
SQL syntax order: SELECT <attributes_list> FROM <table_list>
{WHERE <conditions_list>}.
Rearrange order of elements inside query and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[10] = R"Errorstr(
SQL syntax order: SELECT <attributes_list> FROM <table_list>
{WHERE <conditions_list>}.
Rearrange order of elements inside query, add missing SELECT part,
together with list of attributes and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[11] = R"Errorstr(
SQL syntax order: SELECT <attributes_list> FROM <table_list>
{WHERE <conditions_list>}.
Rearrange order of elements inside query and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[12] = R"Errorstr(
Insert all required tables after 'FROM', separated by comma and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[13] = R"Errorstr(
Verify table name and spelling and try again.
)Errorstr";

	//Inserted parametters: Table Name, @ index: 16, Attribute Name, @ index: 77 
	_error_fixes[14] = R"Errorstr(
Verify table: [] contains the attribute, make sure that Attribute's 
name: [] is written correctly and try again.
)Errorstr";

	//Inserted parametters: Table Name, @ index: 25, Attribute Name, @ index: 26 
	_error_fixes[15] = R"Errorstr(
Verify that this input: [:] represents existing table and 
attribute names and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[16] = R"Errorstr(
Condition syntax is: <Constant> <Single Operator> <Constant>.
Operator is: '=' or '<>' or '>' or '>=' or '<' or '<='
Insert appropriate operator and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[17] = R"Errorstr(
Condition syntax is: <Constant> <Single Operator> <Constant>.
Operator is: '=' or '<>' or '>' or '>=' or '<' or '<='
Insert appropriate operator and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[18] = R"Errorstr(
Condition syntax is: <Constant> <Single Operator> <Constant>.
Operator is: '=' or '<>' or '>' or '>=' or '<' or '<='
Insert appropriate operator and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[19] = R"Errorstr(
Condition syntax is: <Constant> <Single Operator> <Constant>.
Operator is: '=' or '<>' or '>' or '>=' or '<' or '<='
Insert appropriate operator and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[20] = R"Errorstr(
Condition syntax is: <Constant> <Single Operator> <Constant>.
Operator is: '=' or '<>' or '>' or '>=' or '<' or '<='
Insert appropriate operator and try again.
)Errorstr";

	//Inserted parametters: Op1 type, @ index: 22, Op2 type, @ index: 28
	_error_fixes[21] = R"Errorstr(
No conversion from: [] to [].
Verify the logical operation and match the types.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[22] = R"Errorstr(
Insert all required conditions after 'WHERE', 
Separated and organized with the use of  'AND' & 'OR' keywords and 
Parenthesis and try again.
)Errorstr";

	//No-Inserted parametters
	_error_fixes[23] = R"Errorstr(
Insert all required attributes after 'SELECT' separated by comma,
You can select all attributes, by writing '*' instead. and try again.
)Errorstr";

}
#pragma endregion

void error_tracker::initialize_error_dictionary()
{
	this->_Err_converter = {
		{ E_C_EMPTY_STR,																	(Uint)0 },
		{ E_C_MISSING_SEMICOLON,														    (Uint)1 },
		{ static_cast<ERR_TYPE_CODE>(E_C_MISSING_KEYW_SELECT |
									 E_C_MISSING_KEYW_FROM   |
									 E_C_MISSING_KEYW_WHERE),								(Uint)2 },
		{ E_C_MISSING_KEYW_FROM,															(Uint)3 },
		{ E_C_MISSING_KEYW_SELECT,															(Uint)4 },
		{ static_cast<ERR_TYPE_CODE>(E_C_MISSING_KEYW_SELECT | 
									 E_C_MISSING_KEYW_FROM),								(Uint)5 },
		{ static_cast<ERR_TYPE_CODE>(E_C_KEYW_SELECT_MISPLACED |
									 E_C_KEYW_FROM_MISPLACED),								(Uint)6 },
		{ static_cast<ERR_TYPE_CODE>(E_C_KEYW_SELECT_MISPLACED | 
									 E_C_KEYW_WHERE_MISPLACED),								(Uint)7 },
		{ static_cast<ERR_TYPE_CODE>(E_C_KEYW_SELECT_MISPLACED | 
									 E_C_KEYW_WHERE_MISPLACED | 
									 E_C_MISSING_KEYW_FROM),								(Uint)8 },
		{ static_cast<ERR_TYPE_CODE>(E_C_KEYW_FROM_MISPLACED | 
									 E_C_KEYW_WHERE_MISPLACED),								(Uint)9 },
		{ static_cast<ERR_TYPE_CODE>(E_C_KEYW_FROM_MISPLACED | 
									 E_C_KEYW_WHERE_MISPLACED | 
									 E_C_MISSING_KEYW_SELECT),								(Uint)10 },
		{ static_cast<ERR_TYPE_CODE>(E_C_KEYW_SELECT_MISPLACED | 
									 E_C_KEYW_FROM_MISPLACED   | 
									 E_C_KEYW_WHERE_MISPLACED),								(Uint)11 },
		{ E_C_FROM_PART_EMPTY,																(Uint)12 },
		{ E_C_TBL_NOT_ON_SCHEMA,															(Uint)13 },
		{ E_C_ATRB_NOT_IN_TBL,																(Uint)14 },
		{ E_C_UNRECOGNIZED_TOKEN,															(Uint)15 },
		{ E_C_MISSING_OPERATOR,																(Uint)16 },
		{ E_C_TOO_MANY_OPERATORS,															(Uint)17 },
		{ E_C_BOTH_OPERANDS_NOT_CONSTS,														(Uint)18 },
		{ E_C_RHS_OPERAND_NOT_CONST,														(Uint)19 },
		{ E_C_LHS_OPERAND_NOT_CONST,														(Uint)20 },
		{ E_C_INCOMPATIBLE_OPERANDS,														(Uint)21 },
		{ E_C_WHERE_PART_EMPTY,																(Uint)22 },
		{ E_C_SELECT_PART_EMPTY,															(Uint)23 },
	};
}

void error_tracker:: add_err(ERR_TYPE_CODE i_error_type,
							 size_t i_row,
						   	 size_t i_col,
							 string i_err_tok1,
							 string i_err_tok2,
							 string i_err_tok3,
							 string i_err_tok4)
{
	error_detailes *detailes_to_add = new error_detailes();

	detailes_to_add->error_type_code = i_error_type;
	detailes_to_add->error_pos.col = i_col;
	detailes_to_add->error_pos.row = i_row;
	detailes_to_add->is_pos_relevent = false;

	//Construct the mesages and the fixes with their parametters
	Uint err_msg_idx = _Err_converter[i_error_type];

	detailes_to_add->err_msg = _error_msgs[err_msg_idx];
	detailes_to_add->err_repair = _error_fixes[err_msg_idx];

	switch (err_msg_idx)
	{
		case(12):
			detailes_to_add->is_pos_relevent = true;
			detailes_to_add->main_error_family = TBL_LIST;
		case(13):
			detailes_to_add->err_msg.insert(9, i_err_tok1);
			detailes_to_add->main_error_family = TBL_LIST;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(14):
			detailes_to_add->err_msg.insert(39, i_err_tok2);
			detailes_to_add->err_msg.insert(13, i_err_tok1);
			detailes_to_add->err_repair.insert(77, i_err_tok1);
			detailes_to_add->err_repair.insert(16, i_err_tok2);
			detailes_to_add->main_error_family = ATTR_LIST;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(15):
			detailes_to_add->err_repair.insert(26, i_err_tok2);
			detailes_to_add->err_repair.insert(25, i_err_tok1);
			detailes_to_add->main_error_family = GENERAL;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(16):
			detailes_to_add->err_msg.insert(28, i_err_tok1);
			detailes_to_add->main_error_family = COND_LIST;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(17):
			detailes_to_add->err_msg.insert(35, i_err_tok1);
			detailes_to_add->main_error_family = COND_LIST;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(18):
			detailes_to_add->err_msg.insert(43, i_err_tok1);
			detailes_to_add->err_msg.insert(26, i_err_tok2);
			detailes_to_add->main_error_family = COND_LIST;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(19):
			detailes_to_add->err_msg.insert(43, i_err_tok1);
			detailes_to_add->err_msg.insert(26, i_err_tok2);
			detailes_to_add->main_error_family = COND_LIST;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(20):
			detailes_to_add->err_msg.insert(42, i_err_tok1);
			detailes_to_add->err_msg.insert(25, i_err_tok2);
			detailes_to_add->main_error_family = COND_LIST;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(21):
			detailes_to_add->err_msg.insert(61, i_err_tok1);
			detailes_to_add->err_msg.insert(43, i_err_tok4);
			detailes_to_add->err_msg.insert(35, i_err_tok3);
			detailes_to_add->err_repair.insert(28, i_err_tok4);
			detailes_to_add->err_repair.insert(22, i_err_tok3);
			detailes_to_add->main_error_family = COND_LIST;
			detailes_to_add->is_pos_relevent = true;
			break;
		case(22):
			detailes_to_add->main_error_family = COND_LIST;
			detailes_to_add->is_pos_relevent = true;
		case(23):
			detailes_to_add->main_error_family = ATTR_LIST;
			detailes_to_add->is_pos_relevent = true;
		default:
			detailes_to_add->main_error_family = GENERAL;
			break;
		}

	_err_tracker->push_back(detailes_to_add);
}

void error_tracker::print_err_list()
{
	Uint i = 1;

	for (auto it = _err_tracker->begin(); it != _err_tracker->end(); ++it)
	{		
		cout << "==================================== # "<< i << " Error =============================================" << endl;
		
		if ((*it)->is_pos_relevent)
		{
			cout << "==============================" << endl;
			
			cout << "Location:   [Row: " << (*it)->error_pos.row << ", Col: "
				<< (*it)->error_pos.col << "]:" << endl;
			cout << "==============================" << endl;
		}

		cout << "Error:" << endl;
		cout << "======";
		cout << (*it)->err_msg << endl;		

		cout << "Repair Suggestion:" << endl;
		cout << "==================";
		cout << (*it)->err_repair;

		cout << "============================================================================================" << endl;
		
		auto validate_dsgn = it;
		validate_dsgn++;
	
		if (validate_dsgn != _err_tracker->end())
		{		
			cout << "                                           |                                                " << endl;
			cout << "                                           |                                                " << endl;
			cout << "                                           V                                                " << endl;
		}
		i++;
	}	
}

Uint error_tracker::get_first_err_type()
{
	return _err_tracker[0].front()->main_error_family;
}

void error_tracker::remove_duplicate_errors()
{
	bool first_loop = true;

	for (auto it_first = _err_tracker->end(); it_first != _err_tracker->begin(); --it_first)
	{
		if (first_loop)
		{
			it_first--;
			first_loop = false;
		}

		for (auto it_second = it_first; it_second != _err_tracker->begin(); it_second--)
		{
			it_second--;

			
			if ((*it_second)->main_error_family == (*it_first)->main_error_family &&
				(*it_second)->error_pos == (*it_first)->error_pos)
			{
				auto tmp_new_it_second = --it_second;
				it_second++;
				this->_err_tracker->erase(it_second);
				it_second = tmp_new_it_second;
				
			}

			if (it_second == _err_tracker->begin())
			{
				break;
			}
		}

		if (it_first == _err_tracker->begin())
		{
			break;
		}
	}
}

error_tracker::~error_tracker()
{
	int i = 0;
	
	cout << endl;
	cout << "Deleting all Error detailes" << endl;
	
	for (auto itr = this->_err_tracker->begin(); itr != this->_err_tracker->end(); itr++)
	{	
		if (i < this->_err_tracker->size())
		{
			delete (*itr);
			cout << "Error #" << i + 1 << " - " << "deleted." << endl;
			i++;
		}
		else 
		{
			break;
		}
	}
}