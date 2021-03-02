#pragma once

#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace scheduler
{
    const char *const db_name = "scheduler.db";
    constexpr int supported_interfaces_cnt = 2;

    using serializedType = std::vector< std::string >;
    using serializedTypeVec = std::vector< serializedType >;
    using longIntType = long long;

    namespace results
    {
        constexpr int OK = 0;
        constexpr int FAILURE = 1;
    }

    class Sqllite3Wrapper
    {

    public:
        int open();
        int create_tables_if_not_exists();
        int exec(const char *sql);
        int serialize(serializedTypeVec &data);
        int set_initial_statistics_if_needed(serializedTypeVec &data);
        void close();

    private:
        int sqlite3_response_check(int rc, char *zErrMsg) const;
        int select_statistics_by_id(int id, serializedType &row);

        static int callbackSelectStatistics(void *pObject, int argc, char **argv, char **azColName)
        {
            if (serializedType *stat = static_cast< serializedType * >(pObject)) {
                for (int i = 0; i < argc; ++i) {
                    stat->push_back(argv[i]);
                }
            }
            return 0;
        }

        static int callbackSelectCount(void *pObject, int argc, char **argv, char **azColName)
        {
            if (int *count = static_cast< int * >(pObject)) {
                if (argc == 1) {
                    *count = atoi(argv[0]);
                }
            }
            return 0;
        }
        static int callback(void *notUsed, int argc, char **argv, char **azColName)
        {
            return 0;
        }

    private:
        sqlite3 *m_db{nullptr};
    };
}
