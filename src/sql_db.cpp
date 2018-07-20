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
        "SELECT requestedFloor FROM elevatorNetwork WHERE nodeID = 1"); // message
                                                                      // query
    while (res->next()) {
        floorNum = res->getInt("requestedFloor");
    }

    delete stmt; //may need to move to get db
    printf("%s, latest floor number is: %d\n", __func__, floorNum);
    return floorNum;
}

void Elevator_db::db_set_floor_request(int floor) {
    sql::PreparedStatement *pstmt; // Create a pointer to a prepared statement
    printf("%s, setting database to floor: %d\n", __func__, floor);

    pstmt = con->prepareStatement(
        "UPDATE elevatorNetwork SET currentFloor = ? WHERE nodeID = 1");
    pstmt->setInt(1, floor);
    pstmt->executeUpdate();
    delete pstmt;
}

void Elevator_db::db_set_can_log(msg_log to_log) {
    sql::PreparedStatement *pstmt; // Create a pointer to a prepared statement
    printf("%s, logging the following: epoch time %d, node %d, %d\n", __func__, to_log.epoch_time, to_log.node_id, to_log.msg);

    pstmt = con->prepareStatement(
        "INSERT INTO logging_table VALUES(?,?,?)");
    pstmt->setInt(1, to_log.epoch_time);
    pstmt->setInt(2, to_log.node_id);
    pstmt->setInt(3, to_log.msg);
    pstmt->executeUpdate();
    delete pstmt;
}
