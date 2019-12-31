/* ======================================================== */
/*   Exercise #1 - Databases Course	    			        */
/* ======================================================== */
/*   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
/*   Exercise: 01										    */
/*   Lecturer: Roi Krakovski						        */
/* ======================================================== */
/* Purpose: This Program was written as part of first 		*/
/* assgnment in Databases course. it receives SQL query		*/
/* string assgnment in Databases course. it receives SQL    */
/* query string input, analyzes it's correctness and gives 	*/
/* appropriate feedback, including list of bugs and errors, */
/* their location, when it's relevant, and suggestions for  */
/* repar.													*/		
/* ======================================================== */

#include <iostream>
#include <string>
#include "sql_parser.h"

using std::cout;

//comment The advanced_mode flag to view simple mode.
#define advanced_mode

int main(void)
{
	string test[50];

	cout << "[Loading Schema: scheme.txt and initializing Parser]..." << endl << endl;
	sql_parser new_parser(SCHEMA_FILE);

	//Valid
	test[0] = "SELECT Customers.Name, Customers.Age From Customers WHERE Customers.Age >= 12;";
	test[1] = "SELECT;";
	test[2] = "SELECT Customers.Name FROM Customers WHERE Customers.Age=25;";
	test[3] = "SELECT Customers.Name FROM Customers WHERE Customers.Name='Mike';";
	test[4] = "SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE Customers.Name=Orders.CustomerName;";
	test[5] = "SELECT DISTINCT Customers.Name FROM Customers WHERE Customers.Age = 25;";
	test[6] = "SELECT * FROM orders;";
	test[7] = "SELECT DISTINCT * FROM orders;";
	test[8] = "       SELECT Customers.Name, Orders.Price FROM      Customers, \nOrders WHERE          \nCustomers.Name = Orders.CustomerName;";
	test[9] = "       SELECT Customers.Name, Orders.Price FROM      Customers, Orders WHERE          Customers.Name = Orders.CustomerName;";
	test[10] = "SELECT Customers.Name,Orders.Price FROM      Customers,     Orders WHERE          Customers.Name=     Orders.CustomerName;";
	test[11] = "SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE (Customers.Name=Orders.CustomerName) OR Orders.Price>1000;";
	test[12] = "SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE (Customers.Name=Orders.CustomerName) OR (Orders.Price>59);";
	test[13] = "SELECT Customers.Name, Orders.Price FROM Customers, Orders WHERE ((Customers.Name = Orders.CustomerName) AND Orders.Price>1000);";
	test[14] = "SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE (Customers.Name=Orders.CustomerName) AND Orders.Price>1000;";
	test[15] = "SELECT Customers.Name, Orders.Price FROM Customers, Orders WHERE ((Customers.Name = Orders.CustomerName));";
	test[16] = "SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE ((Customers.Name=Orders.CustomerName) OR (Orders.Price>1000)) AND Orders.Price>1000;";
	test[17] = "SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE (((Customers.Name=Orders.CustomerName)) AND Orders.Price>1000);";
	test[18] = "SELECT Customers.Name FROM customers WHERE Customers.Age = ((25));";

	//InValid
	//missing SELCT, FROM, WHERE
	test[19] = "adfljadlfjlafsafs;";
	//missing SELCT, FROM, WHERE
	test[20] = "adflja,dlfjlaf,safs;";
	//Missing ";", table blaaaa not exist on shema, incompatible types
	test[21] = "SELECT DISTINCT * FROM Customers, Diners, \n\n\n\nblaaaaaa, Orders\n WHERE 5 >= 'sdfsfdf'";
	//Operands Don't match!!
	test[22] = "SELECT Customers.Name, Customers.Age From Customers WHERE 'dani' >= 12;";
	//NO field called CustomerName in table customers.
	test[23] = "       SELECT Customers.Name, Orders.Price FROM      Customers, Orders WHERE          Customers.CustomerName = Orders.CustomerName;";
	//Customer wasn't found on current schema.
	test[24] = "SELECT Customer.Name, Orders.Price FROM      Customers, Orders WHERE          Customers.Name = Orders.CustomerName;";
	//Missing ;
	test[25] = "SELECT Customers.Name, Orders.Price FROM Customers, Orders WHERE Customers.Name = Orders.CustomerName AND Orders.Price>1000";
	//Wrong attribute name, wrong sign in rightmost condition
	test[26] = "SELECT Customers.CustomerName FROM Customers WHERE Customers.Age = �25;";
	//Missing ; and wrong table name in condition "Customer.Name".
	test[27] = "SELECT Customers.Name, Orders.Price FROM Customers, Orders WHERE Customer.Name = Orders.CustomerName AND Orders.Price>1000 OR Orders.Price<1000";
	//Wrong attribute in attributes list  - CustomerID and wrong attribute in condition
	test[28] = "SELECT Customers.CustomerID,Orders.Price FROM Customers,Orders WHERE (Customers.CustomerName=Orders.CustomerName) OR (Orders.Price>1000);";
	//FROM and WHERE are misplaced
	test[29] = "SELECT \nWHERE \nFROM \n;";
	
	for (size_t i = 0; i < 30; i++)
	{
		new_parser.parse(test[i]);

		cout << endl << "================================= Examin: # " << i + 1 << " SQL query ======================================" << endl << endl;

		if (new_parser.err_tracker()->amount_of_errors() == 0)
		{
			cout << "VALID SQL query: " << test[i] << endl << endl;
		}

		else
		{
			cout << "INVALID SQL query: " << test[i] << endl << endl;
			
			#ifdef advanced_mode
			new_parser.err_tracker()->print_err_list();
			#else
			
			switch (new_parser.err_tracker()->get_first_err_type())
			{
				case(ATTR_LIST):
					cout << "Parsing <attribute_list> failed."<< endl << endl;
					break;

				case(TBL_LIST):
					cout << "Parsing <table_list> failed." << endl << endl;
					break;

				case(COND_LIST):
					cout << "Parsing <condition> failed." << endl << endl;
					break;
				default:
					cout << "General Failure." << endl << endl;
					break;
			}
			#endif
		}
	}

	return 0;
}

	//TODO ==============================================================================================
	//1. Currently the code doesn't recognize Correctly if Parenthesis are Tight to the 
	// WHERE keyword or other logical keywords (AND, OR) -  "... where(....) " - isn't recognized
	// with the correct error.

	//2. Advanced errors in Conditions, like Missing or extra paranteses,
	// or operands surrounded with parantesesnot inside complex conditions aren't recognized Precisely.
	
	//3. Queries with attributes selected from incorrect tables, aren't currently handled as mistakes.
	//===================================================================================================

	//======================= Examples for the above problems ===========================================

	//	//*** Not identifieng (operands)
	//test[30] = "SELECT DISTINCT * FROM Customers, Diners, Orders\n WHERE((((5) >= ('sdfsfdf'))));";
	////*** Wrong Parenthesis right side
	//test[31] = "SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE (Customers.Name=Orders.CustomerName) OR (Orders.Price>1000;";
	////*** Wrong Parenthesis right side and wrong attribue name
	//test[32] = "SELECT Customers.CustomerID,Orders.Price FROM Customers,Orders WHERE (Customers.CustomerName=Orders.CustomerName) OR (Orders.Price>1000;";
	////*** Wrong Parenthesis right side and wrong attribue name
	//test[33] = "SELECT Customers.Color,Orders.Price FROM Customers,Orders WHERE (Customers.Name=Orders.CustomerName) OR (Orders.Price>1000;";
	////*** Wrong Parenthesis right side and wrong attribue name
	//test[34] = "SELECT Customers.Color,Orders.Price FROM Customers,Orders WHERE(Customers.Name=Orders.CustomerName) OR(Orders.Price>1000);";
	////*** Wrong Parenthesis + col calculation!
	//test[35] = "SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE ((Customers.CustomerName  =  Orders.CustomerName) AND (Orders.Price    >    1000) OR (Orders.Price    < 100));";
	////*** Wrong Parenthesis + too many operators
	//test[36] = "SELECT Customers.CustomerName, Orders.Price FROM Customers, Orders WHERE(Customers.CustomerName = Orders.CustomerName) AND(Orders.Price>1000 OR Orders.Price<100);";
	
	////Shouldn't pass but its passing (because we want something from costumers but not mention it as table).
	////Validate that conditions related to the attributes written in the begining - 
	////if i write column name mistakely in FROM, i can't use it in WHERE, etc with tables.
	////test[36] = "SELECT Customers.Name FROM Orders WHERE Customers.Age=25;";
	////test[37] = "SELECT Customers.CustomerName FROM Orders WHERE Customers.Age = 25;";
	//===================================================================================================