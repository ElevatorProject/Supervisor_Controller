#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>
#include <mysql_connection.h>
#include <stdlib.h>
#include <stdint.h>

using namespace sql;

typedef struct {
    uint16_t node_id;
    uint32_t epoch_time;
    uint16_t msg;
} msg_log;

class Elevator_db {
public:

    Elevator_db(uint16_t n_floor_history, uint16_t n_log_history);
    ~Elevator_db();

    int db_get_floor_request();
    void db_set_floor_request(int floor);
    void db_set_can_log(msg_log to_log);

private:
    Driver *driver;  // Create a pointer to a MySQL driver object
    Connection *con; // Create a pointer to a database connection object
    ResultSet *res;  // Create a pointer to a ResultSet object to hold results
    uint16_t n_floors;
    uint16_t n_logs;

    int manage_n_logs();
    int manage_n_floors();
};
