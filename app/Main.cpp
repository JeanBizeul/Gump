#include <iostream>

#include "Logger/Logger.hpp"
#include "Core/Application.hpp"

int main() {
    Logs::Logger::getInstance().setMinimumLogLevel(Logs::LogLevel::Debug);

    try {
        Gump::Application app;

        app.run();
    } catch (std::exception e) {
        LOG_FATAL("An error as occured while running the app: {}", e.what());
        return 84;
    }
    return 0;
}
