#include <array>

#include "settings.h"
#include "sqllite3wrapper.h"
#include "tcp_client.h"

namespace
{
    const std::string host{"google.com"};
    constexpr uint16_t port{80};
}

namespace scheduler
{
    class Scheduler
    {

    private:
        struct Statistics {
            longIntType min{0};
            double avg{0};
            longIntType max{0};
            longIntType cnt{0};
        };

        std::array< Statistics, supported_interfaces_cnt > m_stats;

    private:
        const std::string m_host;
        const uint16_t m_port;
        TCPClient m_tcpClient;
        Sqllite3Wrapper &m_sqlWrapper;

        // Tcp connection service
        constexpr static int m_tcp_service_id = 1;
        boost::asio::io_service m_tcpService;
        boost::posix_time::seconds m_tcpInterval;
        boost::asio::deadline_timer m_tcpTimer{m_tcpService, m_tcpInterval};
        std::chrono::time_point< std::chrono::high_resolution_clock > m_tcpStart;

        // Resident set size service
        constexpr static int m_rss_service_id = 2;
        boost::asio::io_service m_rssService;
        boost::posix_time::seconds m_rssInterval{1};
        boost::asio::deadline_timer m_rssTimer{m_rssService, m_rssInterval};

    private:
        void start();
        void stop();

        std::string getStatisticsUpdateSql(int serviceId, longIntType newVal);

        void handle_connected(const boost::system::error_code &error);

        void handle_disconnected();

        /**
         * Returns the current resident set size (physical memory use) measured
         * in bytes, or zero if the value cannot be determined.
        */
        size_t get_current_rss();

        void parse_initial_statistics(Statistics &stat, const serializedType &serializedData);

        void tick_tcp_connect();

        void tick_rss();

    public:
        explicit Scheduler(Sqllite3Wrapper &sqlWrapper, Settings &settings)
            : m_host(host)
            , m_port(port)
            , m_sqlWrapper(sqlWrapper)
            , m_tcpInterval(settings.getTCPInterval())
            , m_rssInterval(settings.getRSSInterval())
        {
            m_tcpClient.set_connected_callback(boost::bind(&Scheduler::handle_connected, this, _1));
            m_tcpClient.set_disconnected_callback(boost::bind(&Scheduler::handle_disconnected, this));
        }

        Scheduler(const Scheduler &) = delete;
        Scheduler &operator=(const Scheduler &) = delete;
        ~Scheduler() = default;

        auto init();

        auto process();
    };
}
