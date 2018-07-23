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
#include <stdbool.h>

using namespace std;

Elevator_db::Elevator_db(uint16_t n_floor_history, uint16_t n_log_history): n_floors(n_floor_history), n_logs(n_log_history) {
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "ese", "ese");
    con->setSchema("elevator");
}

Elevator_db::~Elevator_db() {
    delete res;
    delete con;
}

int Elevator_db::manage_n_logs() {
    int counter = 0;

    uint16_t n_logs_to_remove = 0;
    uint32_t  *epoch_time_arr;
    sql::PreparedStatement *pstmt; // Create a pointer to a prepared statement
    Statement *stmt; // Crealte a pointer to a Statement object to hold statements

    stmt = con->createStatement();
    res = stmt->executeQuery(
        "SELECT epoch_time FROM logging_table "); // message
                                                                      // query
    if (n_logs == 0) {
        printf("%s: in unlimited log mode no logs will be deleted", __func__);
        return -1;
    }

    while (res->next()) {
        counter++;
       /* new_epoch = res->getInt("epoch_time");
        if (first_check == true) {
            oldest_epoch = new_epoch;
            first_check = false;
        } else if (oldest_epoch > new_epoch) {
            oldest_epoch = new_epoch;
        }
        */
    }
    if (counter < n_logs) {
        return 0;
    }

    n_logs_to_remove = counter - n_floors;
    epoch_time_arr = (uint32_t*)calloc(n_logs_to_remove, sizeof(uint32_t));

    stmt = con->createStatement();
    res = stmt->executeQuery(
        "SELECT epoch_time FROM logging_table"); // message
    for (int i = 0; i < n_logs_to_remove; i++) {
        res->next();
        epoch_time_arr[i] = res->getInt("epoch_time");
    }

    for (int i =0; i < n_logs_to_remove; i++) {
        pstmt = con->prepareStatement(
            "DELETE FROM logging_table WHERE epoch_time = ?");
        pstmt->setInt(1, epoch_time_arr[i]);
        pstmt->executeUpdate();
    }

    delete stmt; //may need to move to get db
    delete pstmt;
    free(epoch_time_arr);
    return n_logs_to_remove;
}

int Elevator_db::manage_n_floors() {
    uint16_t counter = 0;
    Statement *stmt; // Crealte a pointer to a Statement object to hold statements

    stmt = con->createStatement();
    res = stmt->executeQuery(
        "SELECT requestedFloor FROM elevatorNetwork WHERE nodeID = 1"); // message
                                                                      // query
    while (res->next()) {
        counter++;
    }
    if (counter < n_floors) {
        return 0;
    }
    delete stmt; //may need to move to get db
    return counter;

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
    manage_n_logs();
    pstmt = con->prepareStatement(
        "INSERT INTO logging_table VALUES(?,?,?)");
    pstmt->setInt(1, to_log.epoch_time);
    pstmt->setInt(2, to_log.node_id);
    pstmt->setInt(3, to_log.msg);
    pstmt->executeUpdate();
    delete pstmt;
}
