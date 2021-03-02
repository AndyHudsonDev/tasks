#include "sqllite3wrapper.h"

int scheduler::Sqllite3Wrapper::open()
{
    auto rc = sqlite3_open(db_name, &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(m_db);
        return results::FAILURE;
    } else {
        std::cout << "Database is opened successfully.\n";
        return results::OK;
    }
}

int scheduler::Sqllite3Wrapper::create_tables_if_not_exists()
{
    static constexpr auto tables = {"CREATE TABLE IF NOT EXISTS RSS_HIST("
                                    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                    "VALUE UNSIGNED BIG INT);",

                                    "CREATE TABLE IF NOT EXISTS TCP_CONN_HIST("
                                    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                    "VALUE UNSIGNED BIG INT);",

                                    "CREATE TABLE IF NOT EXISTS STATISTICS("
                                    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                    "MIN UNSIGNED BIG INT NOT NULL,"
                                    "AVG REAL NOT NULL,"
                                    "MAX UNSIGNED BIG INT NOT NULL,"
                                    "CNT UNSIGNED BIG INT NOT NULL);"};

    for (const auto &table : tables) {
        char *zErrMsg = 0;
        /* Execute SQL statement */
        auto rc = sqlite3_exec(m_db, table, callback, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << zErrMsg;
            sqlite3_free(zErrMsg);
            return results::FAILURE;
        }
    }
    return results::OK;
}

int scheduler::Sqllite3Wrapper::exec(const char *sql)
{
    char *zErrMsg = 0;
    /* Execute SQL statement */
    int rc = sqlite3_exec(m_db, sql, callback, 0, &zErrMsg);
    return sqlite3_response_check(rc, zErrMsg);
}

int scheduler::Sqllite3Wrapper::serialize(serializedTypeVec &data)
{
    for (int id = 0; id < supported_interfaces_cnt; ++id) {
        auto retVal = select_statistics_by_id(id + 1, data.at(id));
        if (retVal != results::OK) {
            return retVal;
        }
    }
    return results::OK;
}

int scheduler::Sqllite3Wrapper::set_initial_statistics_if_needed(serializedTypeVec &data)
{
    static constexpr auto sqlCount{"select count(*) from STATISTICS"};
    int rowsCount{0};
    char *zErrMsg = 0;
    /* Execute SQL statement */
    auto rc = sqlite3_exec(m_db, sqlCount, callbackSelectCount, static_cast< void * >(&rowsCount), &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
        return results::FAILURE;
    } else {
        if (rowsCount == supported_interfaces_cnt) {

            std::cout << "rowsCount: " << rowsCount << "\n";

            return serialize(data);
        } else {
            // clean up crap, insert initial data
            static constexpr auto sqlDelete = "delete from STATISTICS";
            rc = exec(sqlDelete);
            if (results::OK != sqlite3_response_check(rc, zErrMsg)) {
                return results::FAILURE;
            }

            // Done to compare further if min value received is smaller than this upper limit
            // then set it to this smaller value
            auto minVal = std::to_string(std::numeric_limits< longIntType >::max());
            auto sqlInsert = "INSERT INTO STATISTICS (MIN,AVG,MAX,CNT) "
                             "VALUES ("
                             + minVal + ", 0, 0, 0), "
                                        "("
                             + minVal + ", 0, 0, 0);";
            rc = exec(sqlInsert.c_str());
            if (results::OK != sqlite3_response_check(rc, zErrMsg)) {
                return results::FAILURE;
            }
            return serialize(data);
        }
    }
}

void scheduler::Sqllite3Wrapper::close()
{
    sqlite3_close(m_db);
}

int scheduler::Sqllite3Wrapper::sqlite3_response_check(int rc, char *zErrMsg) const
{
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
        return results::FAILURE;
    }
    return results::OK;
}

int scheduler::Sqllite3Wrapper::select_statistics_by_id(int id, serializedType &row)
{
    // get initial data
    const auto sqlSelectStat = "select MIN, AVG, MAX, CNT from STATISTICS where id=" + std::to_string(id);
    char *zErrMsg = 0;
    int rc = sqlite3_exec(m_db, sqlSelectStat.c_str(), callbackSelectStatistics, static_cast< void * >(&row), &zErrMsg);
    return sqlite3_response_check(rc, zErrMsg);
}
