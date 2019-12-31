#include "utilities.h"

bool utilities::is_int(string i_to_check)
{
	if (i_to_check.length() == ZERO_LENGTH)
	{
		return false;
	}
	else if (i_to_check.length() == ONE_CHAR)
		return is_digit(i_to_check[STRT_IDX]);
	else
	{
		if (i_to_check[STRT_IDX] == '(' && 
			i_to_check[last_significant_pos(i_to_check)] == ')')
		{
			return is_int(i_to_check.substr(IDX_ADJUSTMENT,
						 (last_significant_pos(i_to_check) - IDX_ADJUSTMENT)));
		}
		else
		{
			return ((is_digit(i_to_check[STRT_IDX]) || is_num_prefix(i_to_check[STRT_IDX]))
				&& (is_unsigned_int(i_to_check.substr(IDX_ADJUSTMENT))));
		}
	}
}

bool utilities::is_unsigned_int(string i_to_check)
{
	if (i_to_check.length() == ZERO_LENGTH)
	{
		return false;
	}
	else if (i_to_check.length() == ONE_CHAR)
		return is_digit(i_to_check[STRT_IDX]);
	else
	{
		return (is_digit((i_to_check[STRT_IDX])) &&
			is_unsigned_int(i_to_check.substr(IDX_ADJUSTMENT)));
	}
}

//Remove duplicate whitespaces (extra Whitespaces)
string utilities::rm_dup_wtsp(string i_str_to_proccess)
{
	string result;
	
	for (size_t i = 0; i < i_str_to_proccess.length(); i++)
	{
		if ((i_str_to_proccess[i] == ' ') && (i_str_to_proccess[i + IDX_ADJUSTMENT] == ' ')  ||
			(i_str_to_proccess[i] == ' ') && (i_str_to_proccess[i + IDX_ADJUSTMENT] == '\n') ||
			(i_str_to_proccess[i] == '\n') && (i_str_to_proccess[i + IDX_ADJUSTMENT] == ' ') ||
			(i_str_to_proccess[i] == '\n') && (i_str_to_proccess[i + IDX_ADJUSTMENT] == '\n'))
		{
			continue;
		}
		else
		{
			if ((i_str_to_proccess[i] == ' ') || (i_str_to_proccess[i] == '\n'))
			{
				result.append(ONE_CHAR, ' ');
			}
			else
			{
				result.append(ONE_CHAR, i_str_to_proccess[i]);
			}
		}
	}

	return result;
}

string utilities::convert_input_to_lower_case(string i_str_to_proccess)
{
	string result = i_str_to_proccess;

	for_each(result.begin(), result.end(), [](char &c) 
	{
		c = tolower(c);
	});

	return result;
}

//Remove whitespaces before and after comma sign
string utilities::rm_wtsp_bef_and_aft_comma(string i_str_to_proccess)
{
	string result;
	
	for (size_t i = 0; i < i_str_to_proccess.length(); i++)
	{
		if (i_str_to_proccess[i] == ',' && i_str_to_proccess[i + IDX_ADJUSTMENT] == ' ' ||
			i_str_to_proccess[i] == ' ' && i_str_to_proccess[i + IDX_ADJUSTMENT] == ',')
		{
			//Change first char to "," and skip over the second undesired char
			result.append(ONE_CHAR, ',');
			i++;		
		}
		else
		{
			result.append(ONE_CHAR, i_str_to_proccess[i]);
		}
	}
	
	return result;
}

//Remove all whitespaces
string utilities::rm_all_wtsp(string i_str_to_proccess)
{
	string result;

	for (int i = 0; i < i_str_to_proccess.length(); i++)
	{
		if (i_str_to_proccess[i] == ' ')
		{
			result.append("");
		}
		else
		{
			result.append(ONE_CHAR, i_str_to_proccess[i]);
		}
	}

	return result;
}