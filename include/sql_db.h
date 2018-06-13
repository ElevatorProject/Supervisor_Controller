#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>
#include <mysql_connection.h>
#include <stdlib.h>

using namespace sql;

class Elevator_db {
public:
    Elevator_db();
    ~Elevator_db();
    int db_get_floor_request();
private:
    Driver *driver;  // Create a pointer to a MySQL driver object
    Connection *con; // Create a pointer to a database connection object
    Statement *stmt; // Crealte a pointer to a Statement object to hold statements
    ResultSet *res;  // Create a pointer to a ResultSet object to hold results
};
