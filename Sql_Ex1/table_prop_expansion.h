#ifndef TABLE_PROP_EXPANSION_H
#define TABLE_PROP_EXPANSION_H

#include "utilities.h"

class table_prop_expansion
{
	friend class sql_parser;

	//Private Members
	map<string, string> _attribute_details;
	bool _error_loading_table_detailes;

public:
	
	table_prop_expansion(string attribute_details_input);
	~table_prop_expansion();

	//Getter
	inline bool error_loading_table_detailes() { 
		return this->_error_loading_table_detailes; }
};

#endif // !TABLE_PROP_EXPANSION_H_