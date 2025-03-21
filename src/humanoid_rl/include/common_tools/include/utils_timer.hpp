/**
 * Copyright (c) [2025] XinChengYang <vertax@foxmail.com> <yaphetys@gmail.com>
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __UTILS_TIMER_HPP__
#define __UTILS_TIMER_HPP__

#define TOOLS_TIMER_VERSION "V2.0"
#define TOOLS_TIMER_VERSION_INFO "Add timer logger"

#include <chrono>
#include <iostream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

namespace common_tools {

static std::chrono::time_point<std::chrono::system_clock> timer_now() { return std::chrono::system_clock::now(); }

static std::chrono::time_point<std::chrono::system_clock> g_commont_tools_timer_first_tic
    = std::chrono::system_clock::now();

static inline double timer_tic_ms()
{
    std::chrono::duration<double> time_diff(timer_now() - g_commont_tools_timer_first_tic);
    return time_diff.count() * 1000.0;
}

static inline double timer_tic()
{
    std::chrono::duration<double> time_diff(timer_now() - g_commont_tools_timer_first_tic);
    return time_diff.count();
}

static inline std::string get_current_date_str()
{
    const time_t t = time(NULL);
    const struct tm *tmp = localtime(&t);
    char szDate[256];
    strftime(szDate, 80, "%Y/%m/%d", tmp);
    return std::string(szDate);
}

static inline std::string get_current_time_str()
{
    const time_t t = time(NULL);
    const struct tm *tmp = localtime(&t);
    char szTime[256];
    strftime(szTime, 80, "%H:%M:%S", tmp);
    return std::string(szTime);
}

static inline std::string get_current_date_time_str()
{
    const time_t t = time(NULL);
    const struct tm *tmp = localtime(&t);
    char szTime[256];
    char szDate[256];
    strftime(szTime, 80, "%H:%M:%S", tmp);
    strftime(szDate, 80, "%Y/%m/%d", tmp);
    return (std::string(szDate) + " " + std::string(szTime));
}

class Timer {
public:
    typedef std::map<std::string, std::chrono::time_point<std::chrono::system_clock> > Map_string_timepoint;
    typedef std::map<std::string, std::chrono::time_point<std::chrono::system_clock> >::iterator
        Map_string_timepoint_it;
    // typedef std::map<std::string, double> Map_string_timepoint;
private:
    Map_string_timepoint m_map_str_timepoint;
    char m_temp_char[4096];
    int m_if_with_threadid = 1;

public:
    Map_string_timepoint_it find_timepoint(const std::string &str)
    {
        Map_string_timepoint_it it = m_map_str_timepoint.find(str);
        if(it == m_map_str_timepoint.end())
        {
            m_map_str_timepoint.insert(std::make_pair(str, timer_now()));
            return m_map_str_timepoint.find(str);
        } else
        {
            return it;
        }
    }

    Timer() { (find_timepoint(std::string(" ")))->second = timer_now(); }

    std::string get_thread_id_str()
    {
        if(m_if_with_threadid)
        {
            std::stringstream ss;
            ss << std::this_thread::get_id();
            // cout << "Id =" << std::this_thread::get_id() << endl;
            return ss.str();
        } else
        {
            return std::to_string(0);
        }
    }

    uint64_t get_thread_id() { return std::stoull(get_thread_id_str()); }

    void tic(std::string str = std::string(" "))
    {
        find_timepoint(str.append(get_thread_id_str()))->second = timer_now();
    }

    double toc(std::string str = std::string(" "), int if_retick = 1)
    {

        Map_string_timepoint_it it = find_timepoint(str.append(get_thread_id_str()));

        std::chrono::duration<double> time_diff = timer_now() - it->second;
        if(if_retick)
        {
            it->second = timer_now();
        }
        return time_diff.count() * 1000;
        ;
    }

    std::string toc_string(std::string str = std::string(" "), int if_retick = 1)
    {

        sprintf(m_temp_char, "[Timer](%s): %s cost time = %.2f ms ", get_thread_id_str().c_str(), str.c_str(),
                toc(str, if_retick));
        return std::string(m_temp_char);
    }
};

struct Cost_time_logger {
    std::map<std::string, float> m_map_item_cost_time;
    std::map<std::string, std::vector<float> > m_map_item_statics;
    FILE *m_fp = nullptr;
    std::string SPLIT_NOTATION = std::string(":");
    Cost_time_logger() {};
    Cost_time_logger(std::string file_name) { init_log(file_name); };
    void init_log(std::string file_name) { m_fp = fopen(file_name.c_str(), "w+"); }

    void record(const std::string &item, const float &cost_time)
    {
        m_map_item_cost_time[item] = cost_time;
        m_map_item_statics[item].emplace_back(cost_time);
    }

    void record(common_tools::Timer &timer, const std::string &item) { record(item, timer.toc(item, 0)); }

    void flush()
    {
        if(m_fp != nullptr)
        {
            for(std::map<std::string, float>::iterator it = m_map_item_cost_time.begin();
                it != m_map_item_cost_time.end(); it++)
            {
                fprintf(m_fp, "%s%s%.3f ", it->first.c_str(), SPLIT_NOTATION.c_str(), it->second);
            }
            fprintf(m_fp, "\r\n");
            fflush(m_fp);
        }
    };

    void flush_d()
    {
        if(m_fp != nullptr)
        {
            for(std::map<std::string, float>::iterator it = m_map_item_cost_time.begin();
                it != m_map_item_cost_time.end(); it++)
            {
                fprintf(m_fp, "%s%s%d ", it->first.c_str(), SPLIT_NOTATION.c_str(), (int)it->second);
            }
            fprintf(m_fp, "\r\n");
            fflush(m_fp);
        }
    };
};

} // namespace common_tools

#endif // UTILS_TIMER_HPP
