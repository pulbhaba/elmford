#include "core/logging/logger.hpp"

#include "core/time/simulation_clock.hpp"

#include <ostream>

namespace fantasy::core {

Logger::Logger(std::ostream& output, const SimulationClock* clock) noexcept
    : output_(output), clock_(clock) {}

void Logger::info(std::string_view message) {
    write(LogLevel::info, message);
}

void Logger::warning(std::string_view message) {
    write(LogLevel::warning, message);
}

void Logger::error(std::string_view message) {
    write(LogLevel::error, message);
}

void Logger::write(LogLevel level, std::string_view message) {
    switch (level) {
    case LogLevel::info:
        output_ << "[INFO]";
        break;
    case LogLevel::warning:
        output_ << "[WARNING]";
        break;
    case LogLevel::error:
        output_ << "[ERROR]";
        break;
    }

    if (clock_ != nullptr) {
        output_ << " [t=" << clock_->current_time().count() << "ns]";
    }
    output_ << ' ' << message << '\n';
}

} // namespace fantasy::core
