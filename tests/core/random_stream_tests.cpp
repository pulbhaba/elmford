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

TEST_CASE("derived streams are repeatable and independent per system and entity") {
    using fantasy::core::derive_stream;
    using fantasy::core::derive_stream_id;

    REQUIRE(derive_stream_id(7, 1, 42) == derive_stream_id(7, 1, 42));
    REQUIRE(derive_stream_id(7, 1, 42) != derive_stream_id(7, 1, 43));
    REQUIRE(derive_stream_id(7, 1, 42) != derive_stream_id(7, 2, 42));
    REQUIRE(derive_stream_id(7, 1, 42) != derive_stream_id(8, 1, 42));
    REQUIRE(derive_stream_id(7, 1, 0) != derive_stream_id(7, 1));
    REQUIRE(derive_stream_id(7, 1, 2) != derive_stream_id(7, 2, 1));

    auto first = derive_stream(7, 1, 42);
    auto second = derive_stream(7, 1, 42);
    auto other = derive_stream(7, 1, 43);
    bool differs = false;
    for (int index = 0; index < 8; ++index) {
        const auto value = first.next_u64();
        REQUIRE(value == second.next_u64());
        differs = differs || value != other.next_u64();
    }
    REQUIRE(differs);
}

TEST_CASE("adding an unrelated entity or system does not perturb existing streams") {
    using fantasy::core::derive_stream;

    std::array<std::uint64_t, 10> baseline{};
    {
        auto stream = derive_stream(99, 3, 1000);
        for (auto& value : baseline) {
            value = stream.next_u64();
        }
    }

    // Create and consume streams for new entities and systems, in mixed order.
    auto new_entity = derive_stream(99, 3, 1001);
    auto new_system = derive_stream(99, 4, 1000);
    auto system_wide = derive_stream(99, 3);
    for (int index = 0; index < 5; ++index) {
        (void)new_entity.next_u64();
        (void)new_system.next_u64();
        (void)system_wide.next_u64();
    }

    auto stream = derive_stream(99, 3, 1000);
    for (const auto expected : baseline) {
        REQUIRE(stream.next_u64() == expected);
    }
}
