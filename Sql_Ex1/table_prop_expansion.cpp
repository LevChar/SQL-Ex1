#include <iostream>
#include <string>
#include <map>

#include "table_prop_expansion.h"


table_prop_expansion::table_prop_expansion(string i_attribute_details)
{
	this->_error_loading_table_detailes = false;
	bool continue_reading_flag = true;
	bool error_while_reading = false;	
	string col_name, col_type;
	size_t colon, comma;

	string att_list = i_attribute_details.substr(IDX_ADJUSTMENT, 
		(last_significant_pos(i_attribute_details) - IDX_ADJUSTMENT));

	att_list = utilities::convert_input_to_lower_case(att_list);
	
	while (continue_reading_flag)
	{
		comma = att_list.find(COMMA);
	
		if (comma != NOT_FOUND)
		{
			colon = att_list.find(':');

			if (colon != NOT_FOUND)
			{
				col_name = att_list.substr(0, colon);
				col_type = att_list.substr(colon + 1, comma - (colon + 1));

				att_list = att_list.substr(comma + 2, 
										   att_list.length() - colon);
			}
			else
			{
				error_while_reading = true;
				continue_reading_flag = false;
			}
		}
		else
		{
			continue_reading_flag = false;

			if (((colon = att_list.find(COLON)) == NOT_FOUND) && 
				(att_list.length() != 0))
			{
				error_while_reading = true;
				break;
			}
			else
			{
				col_name = att_list.substr(0, colon);
				col_type = att_list.substr(colon + 1, 
										   att_list.length() - (colon + 1));
			}
		}

		this->_attribute_details[col_name] = col_type;
	}

	this->_error_loading_table_detailes = error_while_reading ? true : false;
}

table_prop_expansion::~table_prop_expansion()
{
	cout << "Erased.";
}