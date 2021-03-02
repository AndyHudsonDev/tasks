#pragma once

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>

class Settings
{

private:
    // Default values
    const int tcp_interval{3};
    const int rss_interval{3};
    int m_tcpInterval{0};
    int m_rssInterval{0};

public:
    Settings()
    {
        namespace pt = boost::property_tree;
        try {
            pt::ptree root;
            // Load the json file in this ptree
            pt::read_json("scheduler.json", root);
            int tcp = root.get< int >("tcp_interval_seconds", tcp_interval);
            int rss = root.get< int >("rss_interval_seconds", rss_interval);

            if (tcp < tcp_interval) {
                std::cout << "\nTCP connection service interval value is too small. Setting to default.\n";
                tcp = tcp_interval;
            }

            if (rss < rss_interval) {
                std::cout << "\nRSS service interval value is too small. Setting to default.\n";
                rss = rss_interval;
            }
            m_tcpInterval = tcp;
            m_rssInterval = rss;
            std::cout << "\nTCP connection service interval value: " << tcp << "\n";
            std::cout << "\nRSS service interval value: " << rss << "\n";
        } catch (const pt::ptree_error &exc) {
            std::cerr << "Exception has been thrown: " << exc.what() << "\n";
            std::cerr << "Default settings will be used:\ntcp interval = " << tcp_interval << " seconds\n";
            std::cerr << "rss interval = " << rss_interval << " seconds\n";
            m_tcpInterval = tcp_interval;
            m_rssInterval = rss_interval;
        }
    }
    ~Settings() = default;

    auto getRSSInterval()
    {
        return m_rssInterval;
    }

    auto getTCPInterval()
    {
        return m_tcpInterval;
    }
};
