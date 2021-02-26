/*
====================================================================================
Problem Statement
====================================================================================

    You are given initial conversion rates between different currencies

    example:
    USD/SGD: 1.4
    USD/INR: 87.0
    USD/CAD: 1.39
    PKR/AED: 35.0

    You have to design a class which will take these are input
    and give conversion rate between two currencies

    rate function should give the rate if direct or indirect conversion can be done
    for example INR/CAD is possible in above example as below

    INR/USD -> USD/CAD

    but SGD to PKR is not possible (in this case throw exception)

    FxConversion object will be initialized once with initial fx rates but
    rate function will be called many many times for lookup. The solution has to
    be more optimized for lookup

====================================================================================
*/

#include <vector>
#include <string>
#include <iostream>
#include <tuple>
#include <unordered_map>

typedef std::tuple<std::string, std::string, double>  RatePoint;
typedef std::vector<RatePoint>		RatePointList;

class FxConversion {
private:
    std::unordered_map<std::string, std::unordered_map<std::string, double>> m_storage;
public:

    // Floyd–Warshall algorithm, O(V^3) time complexity to construct look-up table in constructor
    // O(1) time complexity to look up for rate exchange
    FxConversion(std::vector<std::tuple<std::string, std::string, double>> &rates) {

        for (std::size_t i = 0; i < rates.size(); ++i) {
            auto from = std::get <0>(rates[i]);
            auto to = std::get <1>(rates[i]);
            auto val = std::get <2>(rates[i]);
            // To avoid incorrect values
            if(!from.empty() && !to.empty() && val > 0.0) {
                m_storage[from][from] = m_storage[to][to] = 1.0;
                m_storage[from][to] = val;
                m_storage[to][from] = 1.0 / val;
            }
        }

        for (auto& curr : m_storage) {
            for (auto& i : m_storage[curr.first]) {
                for (auto& j : m_storage[curr.first]) {
                    m_storage[i.first][j.first] = m_storage[i.first][curr.first] * m_storage[curr.first][j.first];
                }
            }
        }
    }

    double rate(std::string& from, std::string& to) {
        auto it = m_storage.find(from);
        if (it != m_storage.end()) {
            auto fx_info = it->second.find(to);
            if(fx_info != it->second.end()){
                return fx_info->second;
            }
        }

        throw std::logic_error("Can not convert from " + from + " to " + to);
    }
};

int main() {

    RatePointList rpl{
        std::make_tuple("USD", "SGD", 1.401),
                std::make_tuple("USD", "CAD", 1.390),
                std::make_tuple("USD", "IDR", 87000.001),
                std::make_tuple("PKR", "AED", 35.0)
    };

    FxConversion  fx_conv(rpl);

    std::string first = "CAD", second = "SGD";
    std::cout << "converting from " << first << " to " << second << " " <<  fx_conv.rate(first, second) << std::endl;

    first = "SGD"; second = "IDR";
    std::cout << "converting from " << first << " to " << second << " " << fx_conv.rate(first, second) << std::endl;

    first = "IDR"; second = "SGD";
    std::cout << "converting from " << first << " to " << second << " " << fx_conv.rate(first, second) << std::endl;

    try {
        first = "MYR"; second = "SGD";
        std::cout << "converting from " << first << " to " << second << " " << fx_conv.rate(first, second) << std::endl;
    }
    catch (...) {
        std::cout << "converting from " << first << " to " << second << " " << "Exception received" << std::endl;
    }
}
