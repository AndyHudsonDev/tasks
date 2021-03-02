#include <array>

#include "scheduler.h"

void scheduler::Scheduler::start()
{
    try {
        m_tcpStart = std::chrono::high_resolution_clock::now();
        m_tcpClient.connect_to(m_host, m_port);
    } catch (boost::wrapexcept< boost::system::system_error > &exc) {
        std::cerr << __PRETTY_FUNCTION__ << "\n Connection error: " << exc.what();
    }
}

void scheduler::Scheduler::stop()
{
    m_tcpClient.disconnect();
}

std::string scheduler::Scheduler::getStatisticsUpdateSql(int serviceId, longIntType newVal)
{
    std::string toAdd;
    double avg{0.0};

    int ind = serviceId - 1;
    if (m_stats[ind].cnt != 0) {
        avg = (m_stats[ind].cnt * m_stats[ind].avg + newVal) / (m_stats[ind].cnt + 1);
    } else {
        avg = newVal;
    }
    ++m_stats[ind].cnt;
    toAdd += "cnt=" + std::to_string(m_stats[ind].cnt);

    m_stats[ind].avg = avg;
    toAdd += ", avg=" + std::to_string(avg);

    if (newVal < m_stats[ind].min) {
        m_stats[ind].min = newVal;
        toAdd += ", min=" + std::to_string(newVal);
    }

    if (newVal > m_stats[ind].max) {
        m_stats[ind].max = newVal;
        toAdd += ", max=" + std::to_string(newVal);
    }

    return "Update STATISTICS set " + toAdd + " where ID = " + std::to_string(serviceId);
}

void scheduler::Scheduler::handle_connected(const boost::system::error_code &error)
{
    auto elapsed = std::chrono::high_resolution_clock::now() - m_tcpStart;
    long long diff = std::chrono::duration_cast< std::chrono::microseconds >(elapsed).count();

    if (!error) {
        std::cout << "tcp_client: connected to " << host << ". Took " << diff << " microseconds." << std::endl;

        auto sql = "INSERT INTO TCP_CONN_HIST (VALUE) "
                   "VALUES ("
                   + std::to_string(diff) + ");";

        if (results::OK == m_sqlWrapper.exec(sql.c_str())) {
            sql = getStatisticsUpdateSql(m_tcp_service_id, diff);
            std::cout << sql << "\n";
            m_sqlWrapper.exec(sql.c_str());
        }
    } else {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cerr << error.message() << " : " << error.value() << ", tcp_client: disconnecting.\n" << std::endl;
    }
    m_tcpClient.disconnect();
}

void scheduler::Scheduler::handle_disconnected()
{
    std::cout << "tcp_client: disconnected." << std::endl;
}

/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined.
*/
size_t scheduler::Scheduler::get_current_rss()
{
    long rss = 0L;
    FILE *fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r")) == NULL)
        return (size_t)0L; /* Can't open */
    if (fscanf(fp, "%*s%ld", &rss) != 1) {
        fclose(fp);
        return (size_t)0L; /* Can't read */
    }
    fclose(fp);
    return (size_t)rss * (size_t)sysconf(_SC_PAGESIZE);
}

void scheduler::Scheduler::parse_initial_statistics(Statistics &stat, const serializedType &serializedData)
{
    stat.min = atoll(serializedData.at(0).c_str());
    stat.avg = atof(serializedData.at(1).c_str());
    stat.max = atoll(serializedData.at(2).c_str());
    stat.cnt = atoll(serializedData.at(3).c_str());
}

void scheduler::Scheduler::tick_tcp_connect()
{
    start();
    m_tcpTimer.expires_at(m_tcpTimer.expires_at() + m_tcpInterval);
    // Posts the timer event
    m_tcpTimer.async_wait(boost::bind(&Scheduler::tick_tcp_connect, this));
}

void scheduler::Scheduler::tick_rss()
{
    auto rss = get_current_rss();
    std::cout << "\nCurrent Resident Set Size: " << rss << "\n";

    auto sql = "INSERT INTO RSS_HIST (VALUE) "
               "VALUES ("
               + std::to_string(rss) + ");";

    if (results::OK == m_sqlWrapper.exec(sql.c_str())) {
        sql = getStatisticsUpdateSql(m_rss_service_id, rss);
        std::cout << sql << "\n";
        m_sqlWrapper.exec(sql.c_str());
    }

    // Reschedule the timer for rss_interval in the future:
    m_rssTimer.expires_at(m_rssTimer.expires_at() + m_rssInterval);
    // Posts the timer event
    m_rssTimer.async_wait(boost::bind(&Scheduler::tick_rss, this));
}

auto scheduler::Scheduler::init()
{
    serializedTypeVec serializedData(supported_interfaces_cnt);

    if ((results::OK != m_sqlWrapper.open()) || (results::OK != m_sqlWrapper.create_tables_if_not_exists())
        || (results::OK != m_sqlWrapper.set_initial_statistics_if_needed(serializedData))) {

        return results::FAILURE;
    }

    for (auto i = 0; i < supported_interfaces_cnt; ++i) {

        parse_initial_statistics(m_stats[i], serializedData.at(i));
    }
    return results::OK;
}

auto scheduler::Scheduler::process()
{
    if (results::OK != init()) {
        return results::FAILURE;
    }

    // Schedule the tcp connection timer for the first time to fire:
    m_tcpTimer.async_wait(boost::bind(&Scheduler::tick_tcp_connect, this));
    // Enter IO loop.
    // Schedule the rss timer for the first time to fire:
    m_rssTimer.async_wait(boost::bind(&Scheduler::tick_rss, this));
    // Polling
    while (!m_tcpService.stopped() && !m_rssService.stopped()) {
        auto handlersNumber = m_tcpService.poll() + m_rssService.poll();
        // If no handlers ran, then sleep.
        if (handlersNumber == 0) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
        }
    }

    return results::OK;
}

int main()
{
    try {
        scheduler::Sqllite3Wrapper sqlWrapper;
        Settings settings;
        scheduler::Scheduler a(sqlWrapper, settings);

        if (scheduler::results::FAILURE == a.process()) {
            return scheduler::results::FAILURE;
        }

        return scheduler::results::OK;
    } catch (std::exception &exc) {
        std::cerr << "Exception has been thrown: " << exc.what() << std::endl;
        return scheduler::results::FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception has been thrown. Exiting." << std::endl;
        return scheduler::results::FAILURE;
    }
}
