#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>

#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <bits/stdc++.h>

#include "spdlog/spdlog.h"

#define FILE_LOGGER "file_logger"
#define CONSOLE_LOGGER "console"

#define LOGTRACE(logger, msg) log_trace(logger, __FILE__, __FUNCTION__, __LINE__, msg)
#define LOGDEBUG(logger, msg) log_debug(logger, __FILE__, __FUNCTION__, __LINE__, msg)
#define LOGINFO(logger, msg) log_info(logger, __FILE__, __FUNCTION__, __LINE__, msg)
#define LOGNOTICE(logger, msg) log_notice(logger, __FILE__, __FUNCTION__, __LINE__, msg)
#define LOGWARN(logger, msg) log_warn(logger, __FILE__, __FUNCTION__, __LINE__, msg)
#define LOGALERT(logger, msg) log_alert(logger, __FILE__, __FUNCTION__, __LINE__, msg)
#define LOGCRITICAL(logger, msg) log_critical(logger, __FILE__, __FUNCTION__, __LINE__, msg)
#define LOGEMERGENCY(logger, msg) log_emergency(logger, __FILE__, __FUNCTION__, __LINE__, msg)

void print_time_elapsed(std::string, struct timeval*, struct timeval*);
float get_time_elapsed(struct timeval* start, struct timeval* end);

void ignore_first_line();

bool is_valid_input(long,long,long);

std::string generate_random_string(long len);

std::vector<long> find_substr(std::string, std::string);

std::vector<std::pair<long,char>> generate_random_inserts(int,long);

void format_path(std::string &path);

bool fileExists(std::string filePath);

uint64_t str_to_int(std::string str);
std::string int_to_str(const uint64_t);

inline void log_trace(const std::string& logger, const std::string& file, const std::string& function, const int& line, const std::string& msg){
		spdlog::get(logger)->trace() << file << " " << function << "() line " << line << ": " << msg;
		spdlog::get(logger)->flush();
}

inline void log_debug(const std::string& logger, const std::string& file, const std::string& function, const int& line, const std::string& msg){
		spdlog::get(logger)->debug() << file << " " << function << "() line " << line << ": " << msg;
		spdlog::get(logger)->flush();
}

inline void log_info(const std::string& logger, const std::string& file, const std::string& function, const int& line, const std::string& msg){
		spdlog::get(logger)->info() << file << " " << function << "() line " << line << ": " << msg;
		spdlog::get(logger)->flush();
}

inline void log_notice(const std::string& logger, const std::string& file, const std::string& function, const int& line, const std::string& msg){
		spdlog::get(logger)->notice() << file << " " << function << "() line " << line << ": " << msg;
		spdlog::get(logger)->flush();
}

inline void log_warn(const std::string& logger, const std::string& file, const std::string& function, const int& line, const std::string& msg){
		spdlog::get(logger)->warn() << file << " " << function << "() line " << line << ": " << msg;
		spdlog::get(logger)->flush();
}

inline void log_alert(const std::string& logger, const std::string& file, const std::string& function, const int& line, const std::string& msg){
		spdlog::get(logger)->alert() << file << " " << function << "() line " << line << ": " << msg;
		spdlog::get(logger)->flush();
}

inline void log_critical(const std::string& logger, const std::string& file, const std::string& function, const int& line, const std::string& msg){
		spdlog::get(logger)->critical() << file << " " << function << "() line " << line << ": " << msg;
		spdlog::get(logger)->flush();
}

inline void log_emergency(const std::string& logger, const std::string& file, const std::string& function, const int& line, const std::string& msg){
		spdlog::get(logger)->emerg() << file << " " << function << "() line " << line << ": " << msg;
		spdlog::get(logger)->flush();
}

#endif
