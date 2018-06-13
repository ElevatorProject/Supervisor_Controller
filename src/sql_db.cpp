#include "sql_db.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>
#include <mysql_connection.h>
#include <stdlib.h>

using namespace std;

Elevator_db::Elevator_db() {
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "ese", "ese");
    con->setSchema("elevator");
}
Elevator_db::~Elevator_db() {
    delete res;
    delete stmt;
    delete con;
}
