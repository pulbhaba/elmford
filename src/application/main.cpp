#include "core/config/simulation_config.hpp"
#include "core/simulation/simulation_runner.hpp"

#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace {

struct Options {
    fantasy::core::SimulationConfig simulation{fantasy::core::default_simulation_config};
};

std::uint64_t parse_unsigned(std::string_view value, std::string_view argument_name) {
    std::uint64_t parsed{};
    const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), parsed);
    if (error != std::errc{} || end != value.data() + value.size()) {
        throw std::invalid_argument("invalid value for " + std::string(argument_name));
    }
    return parsed;
}

Options parse_options(int argc, char* argv[]) {
    Options options;
    for (int index = 1; index < argc; ++index) {
        const std::string_view argument{argv[index]};
        if (argument == "--help" || argument == "-h") {
            std::cout << "Usage: fantasy_sim [--seed INTEGER] [--ticks INTEGER] [--step-ns INTEGER]\n"
                         "  --step-ns INTEGER  Fixed step duration in nanoseconds (default: 1000000000 = 1 world-second)\n";
            std::exit(0);
        }
        if (argument != "--seed" && argument != "--ticks" && argument != "--step-ns") {
            throw std::invalid_argument("unknown argument: " + std::string(argument));
        }
        if (index + 1 >= argc) {
            throw std::invalid_argument("missing value for " + std::string(argument));
        }

        const auto value = parse_unsigned(argv[++index], argument);
        if (argument == "--seed") {
            options.simulation.seed = value;
        } else if (argument == "--ticks") {
            options.simulation.ticks = value;
        } else {
            if (value == 0 || value > static_cast<std::uint64_t>(std::chrono::nanoseconds::max().count())) {
                throw std::invalid_argument("invalid value for --step-ns (expected positive nanoseconds)");
            }
            options.simulation.fixed_step = std::chrono::nanoseconds{
                static_cast<std::chrono::nanoseconds::rep>(value)};
        }
    }
    return options;
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const auto options = parse_options(argc, argv);
        if (const auto errors = fantasy::core::validate(options.simulation); !errors.empty()) {
            for (const auto& error : errors) {
                std::cerr << "[ERROR] invalid configuration: " << error << '\n';
            }
            return 2;
        }
        fantasy::core::SimulationRunner runner{options.simulation};
        std::uint64_t checksum = 0;

        const auto stats = runner.run(options.simulation.ticks, [&checksum](const auto&, auto& random) {
            checksum += random.next_u64();
        });

        const auto elapsed_ms = std::chrono::duration<double, std::milli>{stats.wall_time}.count();
        std::cout << "Seed: " << options.simulation.seed << '\n'
                  << "Simulation steps: " << stats.steps_executed << '\n'
                  << "Fixed simulation step: " << options.simulation.fixed_step.count() << " ns\n"
                  << "Final simulation time: " << stats.final_simulation_time.count() << " ns\n"
                  << "Execution duration: " << elapsed_ms << " ms\n"
                  << "Updates executed: " << stats.steps_executed << '\n'
                  << "Deterministic checksum: " << checksum << '\n';
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "[ERROR] " << exception.what() << '\n';
        return 2;
    }
}
