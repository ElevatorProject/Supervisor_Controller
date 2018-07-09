#include "sql_db.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>
#include <mysql_connection.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

Elevator_db::Elevator_db() {
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "ese", "ese");
    con->setSchema("elevator");
}

Elevator_db::~Elevator_db() {
    delete res;
    delete con;
}

int Elevator_db::db_get_floor_request() {
    int floorNum;
    Statement *stmt; // Crealte a pointer to a Statement object to hold statements
    stmt = con->createStatement();
    res = stmt->executeQuery(
        "SELECT currentFloor FROM elevatorNetwork WHERE nodeID = 1"); // message
                                                                      // query
    while (res->next()) {
        floorNum = res->getInt("currentFloor");
    }

    delete stmt; //may need to move to get db
    printf("%s, latest floor number is: %d\n", __func__, floorNum);
    return floorNum;
}

int Elevator_db::db_set_floor_request(int floor) {
    printf("%s, setting database to floor: %d\n", __func__, floor);
    sql::PreparedStatement *pstmt; // Create a pointer to a prepared statement

    pstmt = con->prepareStatement(
        "UPDATE elevatorNetwork SET currentFloor = ? WHERE nodeID = 1");
    pstmt->setInt(1, floor);
    pstmt->executeUpdate();
    delete pstmt;
}
