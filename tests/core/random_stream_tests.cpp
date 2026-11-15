#include "core/random/random_stream.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>

TEST_CASE("random streams reproduce integer and floating sequences from a seed") {
    fantasy::core::RandomStream first{123456};
    fantasy::core::RandomStream second{123456};

    for (int index = 0; index < 20; ++index) {
        REQUIRE(first.next_u64() == second.next_u64());
    }

    fantasy::core::RandomStream first_values{9988};
    fantasy::core::RandomStream second_values{9988};
    for (int index = 0; index < 20; ++index) {
        REQUIRE(first_values.uniform_int(-50, 75) == second_values.uniform_int(-50, 75));
        REQUIRE(first_values.uniform_real(-1.0, 1.0) == second_values.uniform_real(-1.0, 1.0));
    }
}

TEST_CASE("different random seeds produce different sequences") {
    fantasy::core::RandomStream first{1};
    fantasy::core::RandomStream second{2};
    std::array<std::uint64_t, 8> first_values{};
    std::array<std::uint64_t, 8> second_values{};

    for (auto& value : first_values) value = first.next_u64();
    for (auto& value : second_values) value = second.next_u64();

    REQUIRE(first_values != second_values);
}

TEST_CASE("random streams with distinct identifiers are independent and repeatable") {
    fantasy::core::RandomStream stream_a{42, 10};
    fantasy::core::RandomStream stream_a_again{42, 10};
    fantasy::core::RandomStream stream_b{42, 11};

    REQUIRE(stream_a.next_u64() == stream_a_again.next_u64());
    REQUIRE(stream_a_again.next_u64() != stream_b.next_u64());
}

TEST_CASE("random values respect requested bounds") {
    fantasy::core::RandomStream random{7};
    for (int index = 0; index < 100; ++index) {
        const auto integer = random.uniform_int(-3, 8);
        const auto real = random.uniform_real(-4.5, 12.25);
        REQUIRE(integer >= -3);
        REQUIRE(integer <= 8);
        REQUIRE(real >= -4.5);
        REQUIRE(real < 12.25);
    }

    REQUIRE(random.uniform_int(std::numeric_limits<std::int64_t>::min(),
                               std::numeric_limits<std::int64_t>::max()) >=
            std::numeric_limits<std::int64_t>::min());
}

TEST_CASE("random value generation rejects invalid ranges") {
    fantasy::core::RandomStream random{7};

    REQUIRE_THROWS_AS(random.uniform_int(2, 1), std::invalid_argument);
    REQUIRE_THROWS_AS(random.uniform_real(1.0, 1.0), std::invalid_argument);
    REQUIRE_THROWS_AS(random.uniform_real(std::numeric_limits<double>::infinity(), 2.0),
                      std::invalid_argument);
}
