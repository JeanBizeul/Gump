#include <iostream>

#include "Logger.hpp"

int main() {
    Logs::Logger::getInstance().setMinimumLogLevel(Logs::LogLevel::Debug);

    LOG_DEBUG("Hello world !");
    LOG_INFO("Hello world !");
    LOG_WARNING("Hello world !");
    LOG_ERROR("Hello world !");
    LOG_FATAL("Hello world !");
    return 0;
}