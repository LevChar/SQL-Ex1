# SQL-Parser
This project is a very basic SQL parser.

## **Purpose:**
This Program was written as part of the first assignment in Databases course (SQL).
It receives SQL query input (string), analyzes it's correctness and gives
appropriate feedback, including a list of bugs and errors, their location, 
(when it's relevant) and suggestions for repair.		

## **To run the program:**
Download \ clone the whole repo & run it in VS.

## **Compatibility:**
The program was compiled and tested in VS2017 environment 
(under windows 10 OS).

## **Bugs \ Issues not yet supported:**

1. Currently, the code isn't dealing Correctly the cases when Parenthesis are Tight to 
the main keywords (AND, OR, WHERE, FROM, etc) -  for example: 
"... where(....) " - won't be parsed correctly.
2. Advanced errors in Conditions, like Missing or extra paranteses, or operands surrounded
with parantesesnot inside complex conditions aren't recognized Precisely.
3. Need to implement conditions validation in relation with the attributes written in the "SELECT" and "FROM" clauses.
For now, Queries with attributes selected from incorrect tables, aren't currently handled as wrong input.

### **Examples of the above problems:**
***Not identifieng (operands):***

"SELECT DISTINCT * FROM Customers, Diners, Orders\n WHERE((((5) >= ('sdfsfdf'))));";

***Wrong Parenthesis right side:***

"SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE (Customers.Name=Orders.CustomerName) OR (Orders.Price>1000;";

***Wrong Parenthesis right side and wrong attribue name:***

"SELECT Customers.CustomerID,Orders.Price FROM Customers,Orders WHERE (Customers.CustomerName=Orders.CustomerName) OR (Orders.Price>1000;";

***Wrong Parenthesis right side and wrong attribue name:***

"SELECT Customers.Color,Orders.Price FROM Customers,Orders WHERE (Customers.Name=Orders.CustomerName) OR (Orders.Price>1000;";

***Wrong Parenthesis right side and wrong attribue name:***

"SELECT Customers.Color,Orders.Price FROM Customers,Orders WHERE(Customers.Name=Orders.CustomerName) OR(Orders.Price>1000);";

***Wrong Parenthesis + col calculation!:***

"SELECT Customers.Name,Orders.Price FROM Customers,Orders WHERE ((Customers.CustomerName  =  Orders.CustomerName) AND (Orders.Price    >    1000) OR (Orders.Price    < 100));";

***Wrong Parenthesis + too many operators:***

"SELECT Customers.CustomerName, Orders.Price FROM Customers, Orders WHERE(Customers.CustomerName = Orders.CustomerName) AND(Orders.Price>1000 OR Orders.Price<100);";

***Shouldn't pass as valid input (because the requested attribute - "AGE", belongs to a table - "costumers", which was not
included in the "FROM" clause of this query), but it's passing:***

"SELECT Customers.Name FROM Orders WHERE Customers.Age=25;";

"SELECT Customers.CustomerName FROM Orders WHERE Customers.Age = 25;";
