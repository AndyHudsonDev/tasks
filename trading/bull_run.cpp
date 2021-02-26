/*
====================================================================================
Problem Statement
====================================================================================

    Given the time series data of daily closing price of a oil
    Find the starting date of longest bull run for oil

    longest bull run : The longest period which instrument saw continous increase in price

====================================================================================
*/

#include <iostream>
#include <map>
#include <vector>
#include <optional>
#include <cassert>

struct PriceOnDate {
    std::string time;
    double value;
};

typedef std::vector<PriceOnDate> TimeSeries;

// Assumption. Input dates are already sorted chronologically.
// Dynamic programming solution, O(N) time complexity, O(1) space
std::optional<std::string> bull_run_start(const TimeSeries& ts_obj) {

    std::size_t res = 0;
    std::size_t cnt = 0;
    std::size_t cur_ind = 0;
    std::size_t res_ind = 0;

    if (ts_obj.size() <= 1) {
        return {};
    }

    for (std::size_t i = 1; i < ts_obj.size(); ++i) {
        if (ts_obj[i].value > ts_obj[i - 1].value) {
            ++cnt;
            if (cnt > res) {
                res = cnt;
                res_ind = cur_ind;
            }
        }
        else {
            cnt = 0;
            cur_ind = i;
        }
    }

    if (res >= 1) {
        return {ts_obj[res_ind].time};
    } else {
        return {};
    }
}

int main() {

    {
        TimeSeries time_series = {
            {"20190102", 33.90009},
            {"20190103", 32.90009},
            {"20190104", 33.80009},
            {"20190105", 33.90001},
            {"20190106", 33.90009},
            {"20190107", 34.90009},
            {"20190108", 35.90009},
            {"20190109", 35.90509},
            {"20190110", 39.90009},
            {"20190111", 31.90009},
            {"20190112", 33.90009},
            {"20190113", 34.90009},
            {"20190114", 36.90009},
            {"20190115", 35.90009},
            {"20190116", 36.90709},
            {"20190117", 33.90009},
            {"20190118", 30.90009}
        };

        auto output = bull_run_start(time_series);
        assert(output.has_value());
        assert(output.value() == "20190103");
    }

    {
        TimeSeries time_series = {

        };

        auto output = bull_run_start(time_series);
        assert(!output.has_value());

    }



    {
        TimeSeries time_series = {
            {"20190108", 35.90009},
        };

        auto output = bull_run_start(time_series);

        assert(!output.has_value());

    }

    {
        TimeSeries time_series = {
            {"20190108", 35.90009},
            {"20190109", 36.90009},
        };
        auto output = bull_run_start(time_series);
        assert(output.has_value());
        assert(output.value() == "20190108");
    }

    {
        TimeSeries time_series = {
            {"20190108", 37.90009},
            {"20190109", 36.90009},
        };

        auto output = bull_run_start(time_series);
        assert(!output.has_value());
    }

    {
        TimeSeries time_series = {
            {"20190108", 37.90009},
            {"20190109", 36.90009},
            {"20190110", 37.90009},
        };

        auto output = bull_run_start(time_series);
        assert(output.has_value());
        assert(output.value() == "20190109");
    }

    {
        TimeSeries time_series = {
            {"20190108", 37.90009},
            {"20190109", 36.90009},
            {"20190110", 36.90009},
        };

        auto output = bull_run_start(time_series);
        assert(!output.has_value());
    }

    {
        TimeSeries time_series = {
            {"20190108", 37.90009},
            {"20190109", 36.90009},
            {"20190110", 35.90009},
            {"20190111", 36.90009},
            {"20190112", 37.90009},
            {"20190113", 35.90009},
            {"20190114", 36.90009},
            {"20190115", 37.90009},
        };

        auto output = bull_run_start(time_series);
        assert(output.has_value());
        assert(output.value() == "20190110");
    }

    {
        TimeSeries time_series = {
            {"20190108", 37.90009},
            {"20190109", 36.90009},
            {"20190110", 35.90009},
            {"20190111", 36.90009},
            {"20190112", 37.90009},
            {"20190113", 38.90009},
            {"20190114", 36.90009},
            {"20190115", 37.90009},
            {"20190116", 38.90009}

        };

        auto output = bull_run_start(time_series);
        assert(output.has_value());
        assert(output.value() == "20190110");
    }


    {
        TimeSeries time_series = {
            {"20190108", 37.90009},
            {"20190109", 36.90009},
            {"20190110", 35.90009},
            {"20190111", 36.90009},
            {"20190112", 37.90009},
            {"20190113", 35.90009},
            {"20190114", 36.90009},
            {"20190115", 37.90009},
            {"20190116", 38.90009}

        };

        auto output = bull_run_start(time_series);
        assert(output.has_value());
        assert(output.value() == "20190113");
    }



    {
        TimeSeries time_series = {
            {"20190108", 37.90009},
            {"20190109", 36.90009},
            {"20190110", 35.90009},
            {"20190111", 36.90009},
            {"20190112", 37.90009},
            {"20190113", 37.90009},
            {"20190114", 35.90009},
            {"20190115", 36.90009},
            {"20190116", 37.90009},
            {"20190117", 38.90009}

        };

        auto output = bull_run_start(time_series);
        assert(output.has_value());
        assert(output.value() == "20190114");
    }

    {
        TimeSeries time_series = {
            {"20190108", 37.90009},
            {"20190109", 37.90009},
            {"20190110", 37.90009}
        };

        auto output = bull_run_start(time_series);
        assert(!output.has_value());
    }

    std::cout << "All tests successfully passed.\n";

    return 0;
}
