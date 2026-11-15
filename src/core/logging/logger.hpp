#pragma once

#include <iosfwd>
#include <string_view>

namespace fantasy::core {

class SimulationClock;

enum class LogLevel {
    info,
    warning,
    error,
};

class Logger {
public:
    explicit Logger(std::ostream& output, const SimulationClock* clock = nullptr) noexcept;

    void info(std::string_view message);
    void warning(std::string_view message);
    void error(std::string_view message);

private:
    void write(LogLevel level, std::string_view message);

    std::ostream& output_;
    const SimulationClock* clock_;
};

} // namespace fantasy::core
