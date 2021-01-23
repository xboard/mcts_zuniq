//
// Created by xboard on 30/09/2020.
//
#include "lib/catch.hpp"
#include "zuniq.hpp"
#include <thread>

TEST_CASE("UnionFind operations", "[ds]") {
    UnionFind<int8_t> uf(N * N);
    SECTION("Testing initial conditions") {
        REQUIRE(uf.find_set(0) != uf.find_set(1));
        REQUIRE(uf.find_set_const(0) != uf.find_set_const(1));

        REQUIRE(uf.find_set(1) != uf.find_set(2));
        REQUIRE(uf.find_set_const(1) != uf.find_set_const(2));

        REQUIRE(uf.find_set(2) != uf.find_set(3));
        REQUIRE(uf.find_set_const(2) != uf.find_set_const(3));

        REQUIRE(uf.find_set(0) != uf.find_set(6));
        REQUIRE(uf.find_set_const(0) != uf.find_set_const(6));

        REQUIRE(uf.find_set(3) != uf.find_set(9));
        REQUIRE(uf.find_set_const(3) != uf.find_set_const(9));
    }SECTION("Making Unions and Testing") {
        uf.union_set(0, 1);
        REQUIRE(uf.find_set_const(0) == uf.find_set_const(1));
        REQUIRE(uf.find_set(0) == uf.find_set(1));

        uf.union_set(2, 3);
        REQUIRE(uf.find_set_const(2) == uf.find_set_const(3));
        REQUIRE(uf.find_set(2) == uf.find_set(3));

        uf.union_set(0, 6);
        REQUIRE(uf.find_set_const(0) == uf.find_set_const(6));
        REQUIRE(uf.find_set(0) == uf.find_set(6));

        uf.union_set(3, 9);
        REQUIRE(uf.find_set_const(3) == uf.find_set_const(9));
        REQUIRE(uf.find_set(3) == uf.find_set(9));

        REQUIRE(uf.find_set_const(1) != uf.find_set_const(3));
        REQUIRE(uf.find_set(1) != uf.find_set(3));

        REQUIRE(uf.find_set_const(0) != uf.find_set_const(9));
        REQUIRE(uf.find_set(0) != uf.find_set(9));

        REQUIRE(uf.find_set_const(6) != uf.find_set_const(9));
        REQUIRE(uf.find_set(6) != uf.find_set(9));

        uf.union_set(1, 2);
        REQUIRE(uf.find_set_const(0) == uf.find_set_const(9));
        REQUIRE(uf.find_set(0) == uf.find_set(9));

        REQUIRE(uf.find_set_const(1) == uf.find_set_const(3));
        REQUIRE(uf.find_set(1) == uf.find_set(3));

        REQUIRE(uf.find_set_const(6) == uf.find_set_const(9));
        REQUIRE(uf.find_set(6) == uf.find_set(9));
    }
}

TEST_CASE("MoveHash values", "[ds]") {
    MoveHash hash;
    Board b1, b2;
    for (const Move &m1 : b1.get_available_moves()) {
        for (const Move &m2 : b2.get_available_moves()) {
            if (m1 == m2) {
                REQUIRE(hash(m1) == hash(m2));
            } else {
                REQUIRE(hash(m1) != hash(m2));
            }
        }
    }
}

TEST_CASE("Timer", "[ds]") {
    using namespace std::chrono_literals;
    Timer timer;
    SECTION("Initial conditions") {
        REQUIRE(timer.elapsed_milli() == 0.0);
    }

    SECTION("Start/Stop after sleeping for 233ms + 100ms") {
        timer.start();
        std::this_thread::sleep_for(233ms);
        timer.stop();
        REQUIRE(timer.elapsed_milli() >= 233.00);
        std::this_thread::sleep_for(100ms);
        REQUIRE(timer.elapsed_milli() >= 233.00);
    }

    SECTION("Start/Stop after sleeping for 233µs + 100µs") {
        timer.start();
        std::this_thread::sleep_for(233us);
        timer.stop();
        REQUIRE(timer.elapsed_micro() >= 233.00);
        std::this_thread::sleep_for(100us);
        REQUIRE(timer.elapsed_micro() >= 233.00);
    }

    SECTION("Start/Elapsed after sleeping for 233ms + 100ms") {
        timer.start();
        std::this_thread::sleep_for(233ms);
        REQUIRE(timer.elapsed_milli() >= 233.00);
        std::this_thread::sleep_for(100ms);
        REQUIRE(timer.elapsed_milli() >= 333.00);
        timer.start();
        std::this_thread::sleep_for(100ms);
        REQUIRE(timer.elapsed_milli() >= 100.00);
    }

    SECTION("Start/Elapsed after sleeping for 233µs + 100µs") {
        timer.start();
        std::this_thread::sleep_for(233us);
        REQUIRE(timer.elapsed_micro() >= 233.00);
        std::this_thread::sleep_for(100us);
        REQUIRE(timer.elapsed_micro() >= 333.00);
        timer.start();
        std::this_thread::sleep_for(100us);
        REQUIRE(timer.elapsed_micro() >= 100.00);
    }
}

TEST_CASE("ConstantTimeStrategy", "[ds]") {
    ConstantTimeStrategy ts40(40'000);// 40 seconds
    ConstantTimeStrategy ts30(30'000);// 30 seconds
    Context ctx;
    SECTION("Returning expected answer") {
        REQUIRE(ts40.max_move_time(ctx) == Approx(1'990.00));
        REQUIRE(ts30.max_move_time(ctx) == Approx(1'492.50));
    }
}

TEST_CASE("RemainingTimeStrategy", "[ds]") {
    RemainingTimeStrategy ts40(40'000);// 40 seconds
    RemainingTimeStrategy ts30(30'000);// 30 seconds
    Context ctx;
    SECTION("Returning expected answer for rounds 0 and 1") {
        ctx[CTX_VAR::ROUND] = 0;
        ctx[CTX_VAR::ELAPSED_TIME_MILLIS] = 0;
        REQUIRE(ts40.max_move_time(ctx) == Approx(1'998.00));
        REQUIRE(ts30.max_move_time(ctx) == Approx(1'498.50));

        ctx[CTX_VAR::ROUND] = 1;
        ctx[CTX_VAR::ELAPSED_TIME_MILLIS] = 0;
        REQUIRE(ts40.max_move_time(ctx) == Approx(1'998.00));
        REQUIRE(ts30.max_move_time(ctx) == Approx(1'498.50));

        ctx[CTX_VAR::ROUND] = 0;
        ctx[CTX_VAR::ELAPSED_TIME_MILLIS] = 100;
        REQUIRE(ts40.max_move_time(ctx) == Approx(1'993.00));
        REQUIRE(ts30.max_move_time(ctx) == Approx(1'493.50));

        ctx[CTX_VAR::ROUND] = 1;
        ctx[CTX_VAR::ELAPSED_TIME_MILLIS] = 100;
        REQUIRE(ts40.max_move_time(ctx) == Approx(1'993.00));
        REQUIRE(ts30.max_move_time(ctx) == Approx(1'493.50));
    }
    SECTION("Returning expected answer for rounds 38, 39 and 40") {
        ctx[CTX_VAR::ROUND] = 38;
        ctx[CTX_VAR::ELAPSED_TIME_MILLIS] = 29900;
        REQUIRE(ts40.max_move_time(ctx) == Approx(10'089.9));
        REQUIRE(ts30.max_move_time(ctx) == Approx(99.9));

        ctx[CTX_VAR::ROUND] = 39;
        REQUIRE(ts40.max_move_time(ctx) == Approx(10'090.00));
        REQUIRE(ts30.max_move_time(ctx) == Approx(99.9));

        ctx[CTX_VAR::ROUND] = 40;
        REQUIRE(ts40.max_move_time(ctx) == Approx(10'090.00));
        REQUIRE(ts30.max_move_time(ctx) == Approx(99.90));
    }
    SECTION("Returning expected answer for rounds 37") {
        ctx[CTX_VAR::ROUND] = 37;
        ctx[CTX_VAR::ELAPSED_TIME_MILLIS] = 29900;
        REQUIRE(ts40.max_move_time(ctx) == Approx(5044.95));
        REQUIRE(ts30.max_move_time(ctx) == Approx(49.95));
    }
}


TEST_CASE("HardCodedTimeStrategy", "[ds]") {
    HardCodedTimeStrategy ts30(TOTAL_TIME_MILLIS);// 30 seconds
    Context ctx;
    int32_t &round = ctx[CTX_VAR::ROUND];
    ctx[CTX_VAR::ELAPSED_TIME_MILLIS] = 0;
    int32_t elapsed_time = 0;
    for (round = 0; round < 42; round++) {
        elapsed_time += ts30.max_move_time(ctx);
        if (round == 39)
            REQUIRE(elapsed_time >= (static_cast<int32_t>(TOTAL_TIME_MILLIS * 2) - 1000));
        REQUIRE(elapsed_time < static_cast<int32_t>(TOTAL_TIME_MILLIS * 2));
    }
}

TEST_CASE("IO operations", "[io]") {
    SECTION("Parsing and formatting moves") {
        REQUIRE(IO::format_move(IO::parse_move("E5v")) == "E5v");
        REQUIRE(IO::format_move(IO::parse_move("A2h")) == "A2h");
        REQUIRE(IO::format_move(IO::parse_move("B3h")) == "B3h");
        REQUIRE(IO::format_move(IO::parse_move("C4v")) == "C4v");
        REQUIRE(IO::format_move(IO::parse_move("F5h")) == "F5h");
        REQUIRE(IO::format_move(IO::parse_move("E6v")) == "E6v");
    }
    SECTION("Parsing CAIA input") {
        REQUIRE(get<GameCommand>(IO::parse_input("Start")) == GameCommand::START);
        REQUIRE(get<GameCommand>(IO::parse_input("start")) == GameCommand::START);
        REQUIRE(get<GameCommand>(IO::parse_input("START")) == GameCommand::START);
        REQUIRE(get<GameCommand>(IO::parse_input("Quit")) == GameCommand::QUIT);
        REQUIRE(get<GameCommand>(IO::parse_input("quit")) == GameCommand::QUIT);
        REQUIRE(get<GameCommand>(IO::parse_input("QUIT")) == GameCommand::QUIT);
        REQUIRE(get<Move>(IO::parse_input("E6v")) == IO::parse_move("E6v"));
        REQUIRE(get<Move>(IO::parse_input("E6V")) == IO::parse_move("E6V"));
        REQUIRE(get<Move>(IO::parse_input("e6v")) == IO::parse_move("E6v"));
    }
}

TEST_CASE("Board copy, assignment and move.", "[board]") {
    SECTION("Copy") {
        Board b1, b3;
        b1.apply_move(IO::parse_move("B1h"));
        REQUIRE(b1.get_turn() != b3.get_turn());
        REQUIRE(b1.get_available_moves().size() == b3.get_available_moves().size() - 1);
        REQUIRE(b1.is_valid(IO::parse_move("B1h")) != b3.is_valid(IO::parse_move("B1h")));
        Board b2 = b1; // Calls copy constructor.
        REQUIRE(b1.get_available_moves().size() == b2.get_available_moves().size());
        REQUIRE(b1.get_available_moves() == b2.get_available_moves());
        REQUIRE(b1.get_turn() == b2.get_turn());
        REQUIRE(b1.is_valid(IO::parse_move("B1h")) == b2.is_valid(IO::parse_move("B1h")));
    }SECTION("Assignment") {
        Board b1, b2;
        b1.apply_move(IO::parse_move("B1h"));
        REQUIRE(b1.get_turn() != b2.get_turn());
        REQUIRE(b1.get_available_moves().size() == b2.get_available_moves().size() - 1);
        REQUIRE(b1.is_valid(IO::parse_move("B1h")) != b2.is_valid(IO::parse_move("B1h")));
        b2 = b1; // Calls assignment operator.
        REQUIRE(b1.get_available_moves().size() == b2.get_available_moves().size());
        REQUIRE(b1.get_available_moves() == b2.get_available_moves());
        REQUIRE(b1.get_turn() == b2.get_turn());
        REQUIRE(b1.is_valid(IO::parse_move("B1h")) == b2.is_valid(IO::parse_move("B1h")));
    }SECTION("Move") {
        Board b1, b2;
        b1.apply_move(IO::parse_move("B1h"));
        REQUIRE(b1.get_turn() != b2.get_turn());
        REQUIRE(b1.get_available_moves().size() == b2.get_available_moves().size() - 1);
        REQUIRE(b1.is_valid(IO::parse_move("B1h")) != b2.is_valid(IO::parse_move("B1h")));

        Board b3 = b1; // Calls the copy constructor.
        b2 = std::move(b1); // Calls move assignment operator.
        REQUIRE(b1.get_available_moves().empty() == true); // NOLINT(bugprone-use-after-move)
        REQUIRE(b3.get_available_moves().size() == b2.get_available_moves().size());
        REQUIRE(b3.get_available_moves() == b2.get_available_moves());
        REQUIRE(b3.get_turn() == b2.get_turn());
        REQUIRE(b3.is_valid(IO::parse_move("B1h")) == b2.is_valid(IO::parse_move("B1h")));
    }
}

TEST_CASE("Board operations", "[board]") {
    Board board;
    SECTION("Initial state") {
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES);
        REQUIRE(board.get_turn() == Player::WHITE);
    }SECTION("Get squares") {
        REQUIRE(board.get_square_left(8U) == 6U);
        REQUIRE(board.get_square_right(8U) == 7U);
        REQUIRE(board.get_square_above(8U) == 2U);
        REQUIRE(board.get_square_bellow(8U) == 7U);
        REQUIRE(board.get_square_above(6U) == 0U);
        REQUIRE(board.get_square_above(10U) == 4U);
        REQUIRE(board.get_square_bellow(28U) == 24U);
        REQUIRE(board.get_square_above(28U) == 19U);
        REQUIRE(board.get_square_left(5U) == 4U);
        REQUIRE(board.get_square_right(4U) == 4U);
        REQUIRE(board.get_square_left(4U) == 3U);
        //REQUIRE(board.get_square_left(6U) == 4U); // assertion fails!
        REQUIRE(board.get_square_right(6U) == 5U);
        REQUIRE(board.get_square_left(23U) == 19U);
        REQUIRE(board.get_square_left(29U) == 24U);
        //REQUIRE(board.get_square_right(29U) == 5U); // assertion fails!
    }SECTION("Get lines") {
        REQUIRE(board.get_line_above(0) == IO::parse_move("A1h"));
        REQUIRE(board.get_line_left(0) == IO::parse_move("A1v"));
        REQUIRE(board.get_line_bellow(0) == IO::parse_move("B1h"));
        REQUIRE(board.get_line_right(0) == IO::parse_move("A2v"));
        //--------------------------------------------
        REQUIRE(board.get_line_above(24) == IO::parse_move("E5h"));
        REQUIRE(board.get_line_left(24) == IO::parse_move("E5v"));
        REQUIRE(board.get_line_bellow(24) == IO::parse_move("F5h"));
        REQUIRE(board.get_line_right(24) == IO::parse_move("E6v"));
        //--------------------------------------------
        REQUIRE(board.get_line_above(12) == IO::parse_move("C3h"));
        REQUIRE(board.get_line_left(12) == IO::parse_move("C3v"));
        REQUIRE(board.get_line_bellow(12) == IO::parse_move("D3h"));
        REQUIRE(board.get_line_right(12) == IO::parse_move("C4v"));
    }
    SECTION("Available moves") {
        REQUIRE(board.is_valid(IO::parse_move("B3h")));
        REQUIRE(board.is_valid(IO::parse_move("E6v")));
        for (uint8_t row = 0; row < N - 1; row++) {
            for (uint8_t col = 0; col < N; col++) {
                string move(3, 'v');
                move[0] = char(row + 'A');
                move[1] = char(col + '1');
                REQUIRE(board.is_valid(IO::parse_move(move)));
            }
        }

        for (uint8_t row = 0; row < N; row++) {
            for (uint8_t col = 0; col < N - 1; col++) {
                string move(3, 'h');
                move[0] = char(row + 'A');
                move[1] = char(col + '1');
                REQUIRE(board.is_valid(IO::parse_move(move)));
            }
        }
    }
    SECTION("Tests single move") {
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES);
        REQUIRE(board.get_turn() == Player::WHITE);

        board.apply_move(IO::parse_move("B2h"));
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 1);
        REQUIRE(board.get_turn() == Player::BLACK);
        REQUIRE(board.is_valid(IO::parse_move("B2h")) == false);
    }
    SECTION("Tests simple closed region of size 1 - right") {
        board.apply_move(IO::parse_move("C3h"));
        board.apply_move(IO::parse_move("D3h"));
        board.apply_move(IO::parse_move("C3v"));
        Move m = IO::parse_move("C4v");
        board.apply_move(m);
        REQUIRE(board.get_turn() == Player::WHITE);
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 4);
    }SECTION("Tests simple closed region of size 1 - left") {
        board.apply_move(IO::parse_move("C3h"));
        board.apply_move(IO::parse_move("D3h"));
        board.apply_move(IO::parse_move("C4v"));
        Move m = IO::parse_move("C3v");
        board.apply_move(m);
        REQUIRE(board.get_turn() == Player::WHITE);
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 4);
    }SECTION("Tests simple closed region of size 1 - top") {
        board.apply_move(IO::parse_move("C3v"));
        board.apply_move(IO::parse_move("D3h"));
        board.apply_move(IO::parse_move("C4v"));
        Move m = IO::parse_move("C3h");
        board.apply_move(m);
        REQUIRE(board.get_turn() == Player::WHITE);
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 4);
    }SECTION("Tests simple closed region of size 1 - down") {
        board.apply_move(IO::parse_move("C3h"));
        board.apply_move(IO::parse_move("C3v"));
        board.apply_move(IO::parse_move("C4v"));
        Move m = IO::parse_move("D3h");
        board.apply_move(m);
        REQUIRE(board.get_turn() == Player::WHITE);
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 4);
    }SECTION("Tests simple closed region of size 2") {
        board.apply_move(IO::parse_move("C3h"));
        board.apply_move(IO::parse_move("C4h"));
        board.apply_move(IO::parse_move("D3h"));
        board.apply_move(IO::parse_move("D4h"));
        board.apply_move(IO::parse_move("C3v"));
        Move m = IO::parse_move("C5v");
        board.apply_move(m);
        REQUIRE(board.get_turn() == Player::WHITE);
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 7);
    }SECTION("Tests move execution with region of size 3.") {
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES);
        REQUIRE(board.get_turn() == Player::WHITE);

        board.apply_move(IO::parse_move("C2h"));
        board.apply_move(IO::parse_move("C3h"));
        board.apply_move(IO::parse_move("C4h"));
        board.apply_move(IO::parse_move("C5v"));
        board.apply_move(IO::parse_move("C2v"));
        board.apply_move(IO::parse_move("D2h"));
        board.apply_move(IO::parse_move("D3h"));
        board.apply_move(IO::parse_move("D4h"));
        REQUIRE(board.get_turn() == Player::WHITE);
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 10);
    }SECTION("Tests excluding invalid moves from closed region of size already found") {
        board.apply_move(IO::parse_move("A1h"));
        board.apply_move(IO::parse_move("A1v"));
        board.apply_move(IO::parse_move("A2v"));
        board.apply_move(IO::parse_move("B1h"));
        REQUIRE(board.get_turn() == Player::WHITE);
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 4);
        board.apply_move(IO::parse_move("C3h"));
        board.apply_move(IO::parse_move("C3v"));
        REQUIRE(board.is_valid(IO::parse_move("C4v")));
        REQUIRE(board.is_valid(IO::parse_move("D3h")));
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 6);
        Move m = IO::parse_move("C4v");
        board.apply_move(m);
        REQUIRE(board.get_turn() == Player::BLACK);
        REQUIRE(board.is_valid(IO::parse_move("C4v")) == false);
        REQUIRE(board.is_valid(IO::parse_move("D3h")) == false);
        REQUIRE(board.get_available_moves().size() == TOTAL_MOVES - 8);
    }
}


TEST_CASE("Complete Game tests", "[game]") {
    Board board;
    SECTION("CodeCup sample game. https://www.codecup.nl/zuniq/sample_game.php") {
        board.apply_move(IO::parse_move("E5v")); // 1
        board.apply_move(IO::parse_move("A2h")); // 2
        board.apply_move(IO::parse_move("B3h")); // 3
        board.apply_move(IO::parse_move("C4v")); // 4
        board.apply_move(IO::parse_move("E5h")); // 5
        board.apply_move(IO::parse_move("C1v")); // 6
        board.apply_move(IO::parse_move("E3h")); // 7
        board.apply_move(IO::parse_move("F3h")); // 8
        board.apply_move(IO::parse_move("B5h")); // 9
        board.apply_move(IO::parse_move("A3h")); // 10
        board.apply_move(IO::parse_move("D5v")); // 11
        board.apply_move(IO::parse_move("D1v")); // 12
        board.apply_move(IO::parse_move("B5v")); // 13
        board.apply_move(IO::parse_move("C5h")); // 14
        board.apply_move(IO::parse_move("D4h")); // 15
        board.apply_move(IO::parse_move("C1h")); // 16
        board.apply_move(IO::parse_move("A4v")); // 17
        board.apply_move(IO::parse_move("F5h")); // 18
        board.apply_move(IO::parse_move("C4h")); // 19
        board.apply_move(IO::parse_move("A5h")); // 20
        board.apply_move(IO::parse_move("A5v")); // 21
        board.apply_move(IO::parse_move("B6v")); // 22
        board.apply_move(IO::parse_move("E3v")); // 23
        board.apply_move(IO::parse_move("D2h")); // 24
        board.apply_move(IO::parse_move("C2h")); // 25
        board.apply_move(IO::parse_move("A4h")); // 26
        board.apply_move(IO::parse_move("B2h")); // 27
        board.apply_move(IO::parse_move("C6v")); // 28
        board.apply_move(IO::parse_move("D2v")); // 29
        board.apply_move(IO::parse_move("C3v")); // 30
        board.apply_move(IO::parse_move("B4v")); // 31
        board.apply_move(IO::parse_move("D6v")); // 32
        board.apply_move(IO::parse_move("E4h")); // 33
        board.apply_move(IO::parse_move("E1v")); // 34
        board.apply_move(IO::parse_move("C3h")); // 35
        board.apply_move(IO::parse_move("B1v")); // 36
        board.apply_move(IO::parse_move("F1h")); // 37
        REQUIRE(board.get_available_moves().size() == 5);
        REQUIRE(board.is_valid(IO::parse_move("A1h")) == true);
        REQUIRE(board.is_valid(IO::parse_move("A1v")) == true);
        REQUIRE(board.is_valid(IO::parse_move("E2h")) == true);
        REQUIRE(board.is_valid(IO::parse_move("E2v")) == true);
        REQUIRE(board.is_valid(IO::parse_move("F2h")) == true);

        board.apply_move(IO::parse_move("F2h"));// 38
        REQUIRE(board.get_available_moves().size() == 2);
        REQUIRE(board.is_valid(IO::parse_move("A1h")) == true);
        REQUIRE(board.is_valid(IO::parse_move("A1v")) == true);

        board.apply_move(IO::parse_move("A1h"));// 39
        board.apply_move(IO::parse_move("A1v"));// 40
        REQUIRE(board.get_turn() == Player::WHITE);
        REQUIRE(board.is_over());
        REQUIRE(board.winner() == Player::BLACK);
    }
    SECTION("CodeCup game 158091. https://www.codecup.nl/showgame.php?ga=158091") {
        board.apply_move(IO::parse_move("A3h")); // 1
        board.apply_move(IO::parse_move("A1v")); // 2
        board.apply_move(IO::parse_move("B1h")); // 3
        board.apply_move(IO::parse_move("A1h")); // 4
        board.apply_move(IO::parse_move("A5v")); // 5
        board.apply_move(IO::parse_move("A2v")); // 6
        board.apply_move(IO::parse_move("B5h")); // 7
        board.apply_move(IO::parse_move("A2h")); // 8
        board.apply_move(IO::parse_move("C2v")); // 9
        board.apply_move(IO::parse_move("A3v")); // 10
        board.apply_move(IO::parse_move("B2v")); // 11
        board.apply_move(IO::parse_move("A4v")); // 12
        board.apply_move(IO::parse_move("D1h")); // 13
        board.apply_move(IO::parse_move("A4h")); // 14
        board.apply_move(IO::parse_move("D5h")); // 15
        board.apply_move(IO::parse_move("A5h")); // 16
        board.apply_move(IO::parse_move("E5v")); // 17
        board.apply_move(IO::parse_move("B1v")); // 18
        board.apply_move(IO::parse_move("D5v")); // 19
        board.apply_move(IO::parse_move("B3v")); // 20
        board.apply_move(IO::parse_move("C2h")); // 21
        board.apply_move(IO::parse_move("B4v")); // 22
        board.apply_move(IO::parse_move("D2v")); // 23
        board.apply_move(IO::parse_move("B5v")); // 24
        board.apply_move(IO::parse_move("E2v")); // 25
        board.apply_move(IO::parse_move("B6v")); // 26
        board.apply_move(IO::parse_move("C5v")); // 27
        board.apply_move(IO::parse_move("C3v")); // 28
        board.apply_move(IO::parse_move("D4h")); // 29
        board.apply_move(IO::parse_move("C4v")); // 30
        board.apply_move(IO::parse_move("E3v")); // 31
        board.apply_move(IO::parse_move("D1v")); // 32
        board.apply_move(IO::parse_move("D4v")); // 33
        board.apply_move(IO::parse_move("D3v")); // 34
        board.apply_move(IO::parse_move("D6v")); // 35
        board.apply_move(IO::parse_move("E1v")); // 36
        cout << "Size of board: " << sizeof(board) << " bytes" << endl;

        board.apply_move(IO::parse_move("E4v"));// 37
        REQUIRE(board.get_available_moves().size() == 4);
        REQUIRE(board.is_valid(IO::parse_move("E3h")) == true);
        REQUIRE(board.is_valid(IO::parse_move("E6v")) == true);
        REQUIRE(board.is_valid(IO::parse_move("F3h")) == true);
        REQUIRE(board.is_valid(IO::parse_move("F5h")) == true);

        board.apply_move(IO::parse_move("E6v"));// 38
        REQUIRE(board.get_available_moves().size() == 2);
        REQUIRE(board.is_valid(IO::parse_move("E3h")) == true);
        REQUIRE(board.is_valid(IO::parse_move("F3h")) == true);

        board.apply_move(IO::parse_move("F3h"));// 39
        REQUIRE(board.get_turn() == Player::BLACK);
        REQUIRE(board.is_over());
        REQUIRE(board.winner() == Player::WHITE);
    }
    SECTION("CodeCup game 159658. https://www.codecup.nl/showgame.php?ga=159658") {
        board.apply_move(IO::parse_move("A3v")); // 1
        board.apply_move(IO::parse_move("F3h")); // 2
        board.apply_move(IO::parse_move("B5v")); // 3
        board.apply_move(IO::parse_move("D6v")); // 4
        board.apply_move(IO::parse_move("D5h")); // 5
        board.apply_move(IO::parse_move("E3v")); // 6
        board.apply_move(IO::parse_move("C5h")); // 7
        board.apply_move(IO::parse_move("D1h")); // 8
        board.apply_move(IO::parse_move("A5v")); // 9
        board.apply_move(IO::parse_move("C3v")); // 10
        board.apply_move(IO::parse_move("E5v")); // 11
        board.apply_move(IO::parse_move("F1h")); // 12
        board.apply_move(IO::parse_move("D5v")); // 13
        board.apply_move(IO::parse_move("F4h")); // 14
        board.apply_move(IO::parse_move("C5v")); // 15
        board.apply_move(IO::parse_move("E6v")); // 16
        board.apply_move(IO::parse_move("B2h")); // 17
        board.apply_move(IO::parse_move("D4v")); // 18
        board.apply_move(IO::parse_move("B1h")); // 19
        board.apply_move(IO::parse_move("F5h")); // 20
        cout << "#moves remaining " << board.get_available_moves().size() << endl;
        board.apply_move(IO::parse_move("E1h")); // 21
        board.apply_move(IO::parse_move("C4v")); // 22
        board.apply_move(IO::parse_move("E2h")); // 23
        board.apply_move(IO::parse_move("D1v")); // 24
        board.apply_move(IO::parse_move("D2v")); // 25
        board.apply_move(IO::parse_move("B3v")); // 26
        board.apply_move(IO::parse_move("C2h")); // 27
        board.apply_move(IO::parse_move("B1v")); // 28
        board.apply_move(IO::parse_move("A4v")); // 29
        board.apply_move(IO::parse_move("E3h")); // 30
        board.apply_move(IO::parse_move("B4v!")); // 31
        board.apply_move(IO::parse_move("C2v")); // 32
        board.apply_move(IO::parse_move("D4h")); // 33
        board.apply_move(IO::parse_move("C3h")); // 34
        board.apply_move(IO::parse_move("A6v")); // 35
        board.apply_move(IO::parse_move("A5h")); // 36
        board.apply_move(IO::parse_move("E1v")); // 37
        board.apply_move(IO::parse_move("A1h")); // 38
        board.apply_move(IO::parse_move("A1v")); // 39
        REQUIRE(board.get_turn() == Player::BLACK);
        REQUIRE(board.is_over());
        REQUIRE(board.winner() == Player::WHITE);
    }
}

TEST_CASE("Type assertions", "[types]") {
    SECTION("Board type") {
        REQUIRE(std::is_copy_constructible<Board>::value == true);
        REQUIRE(std::is_move_constructible<Board>::value == true);
    }
}

TEST_CASE("Crashing games", "[board]") {
    Board board;
    SECTION("Game 1 - Temporary move exclusions") {
        board.apply_move(IO::parse_move("B6v")); // 1
        board.apply_move(IO::parse_move("D3v")); // 2
        board.apply_move(IO::parse_move("D6v")); // 3
        board.apply_move(IO::parse_move("A5v")); // 4
        board.apply_move(IO::parse_move("E6v")); // 5
        board.apply_move(IO::parse_move("C5v")); // 6
        board.apply_move(IO::parse_move("A3v")); // 7
        board.apply_move(IO::parse_move("C1h")); // 8
        board.apply_move(IO::parse_move("B2v")); // 9
        board.apply_move(IO::parse_move("C5h")); // 10
        board.apply_move(IO::parse_move("D1v")); // 11
        board.apply_move(IO::parse_move("F4h")); // 12
        board.apply_move(IO::parse_move("B5v")); // 13
        board.apply_move(IO::parse_move("B5h")); // 14
        board.apply_move(IO::parse_move("E2v")); // 15
        board.apply_move(IO::parse_move("B4h")); // 16
        board.apply_move(IO::parse_move("C2h")); // 17
        board.apply_move(IO::parse_move("E1v")); // 18
        board.apply_move(IO::parse_move("A2h")); // 19
        board.apply_move(IO::parse_move("D4v")); // 20
        board.apply_move(IO::parse_move("D4h")); // 21
        board.apply_move(IO::parse_move("E2h")); // 22
        board.apply_move(IO::parse_move("E4v")); // 23
        board.apply_move(IO::parse_move("A4h")); // 24
        board.apply_move(IO::parse_move("F1h")); // 25
        board.apply_move(IO::parse_move("A3h")); // 26
        board.apply_move(IO::parse_move("F5h")); // 27
        board.apply_move(IO::parse_move("E4h")); // 28
        board.apply_move(IO::parse_move("C1v")); // 29
        board.apply_move(IO::parse_move("D2v")); // 30
        REQUIRE(board.is_valid(IO::parse_move("C3v")) == true);
        board.apply_move(IO::parse_move("C6v")); // 31
        REQUIRE(board.is_valid(IO::parse_move("C3v")) == false);
        board.apply_move(IO::parse_move("E3v")); // 32
        board.apply_move(IO::parse_move("C4h")); // 33
        board.apply_move(IO::parse_move("B1h!")); // 34
        board.apply_move(IO::parse_move("D1h")); // 35
        REQUIRE(board.is_valid(IO::parse_move("C3v")) == true);
        board.apply_move(IO::parse_move("C3v")); // 36
        board.apply_move(IO::parse_move("A5h")); // 37
        board.apply_move(IO::parse_move("A1h")); // 38
        board.apply_move(IO::parse_move("E3h")); // 39
        board.apply_move(IO::parse_move("B3v")); // 40
    }
}

TEST_CASE("Games with wrong board/game state", "[board]") {
    Board board;
    SECTION("Game 1 - Zuniq vs Player3 - Tests removal of moves inside closed region.") {
        board.apply_move(IO::parse_move("F4h")); // 1
        board.apply_move(IO::parse_move("A2h")); // 2
        board.apply_move(IO::parse_move("A1h")); // 3
        board.apply_move(IO::parse_move("F3h")); // 4
        board.apply_move(IO::parse_move("A5h")); // 5
        board.apply_move(IO::parse_move("F5h")); // 6
        board.apply_move(IO::parse_move("C1v")); // 7
        board.apply_move(IO::parse_move("B3v")); // 8
        board.apply_move(IO::parse_move("A6v")); // 9
        board.apply_move(IO::parse_move("C4v")); // 10
        board.apply_move(IO::parse_move("E6v")); // 11
        board.apply_move(IO::parse_move("F1h")); // 12
        board.apply_move(IO::parse_move("B1v")); // 13
        board.apply_move(IO::parse_move("A4h")); // 14
        board.apply_move(IO::parse_move("A1v")); // 15
        board.apply_move(IO::parse_move("B2h")); // 16
        board.apply_move(IO::parse_move("A4v")); // 17
        board.apply_move(IO::parse_move("D3v")); // 18
        board.apply_move(IO::parse_move("D6v")); // 19
        board.apply_move(IO::parse_move("E1v")); // 20
        board.apply_move(IO::parse_move("D1h")); // 21
        board.apply_move(IO::parse_move("C3h")); // 22
        board.apply_move(IO::parse_move("C2v")); // 23
        board.apply_move(IO::parse_move("C5v")); // 24
        board.apply_move(IO::parse_move("D4v")); // 25
        board.apply_move(IO::parse_move("B5h")); // 26
        board.apply_move(IO::parse_move("F2h")); // 27
        board.apply_move(IO::parse_move("D2v")); // 28
        board.apply_move(IO::parse_move("B2v")); // 29
        board.apply_move(IO::parse_move("E4h")); // 30
        board.apply_move(IO::parse_move("E3v")); // 31
        board.apply_move(IO::parse_move("B1h")); // 32
        board.apply_move(IO::parse_move("E4v")); // 33
        board.apply_move(IO::parse_move("E1h")); // 34
        REQUIRE(board.is_valid(IO::parse_move("C2h")) == false);
        REQUIRE(board.is_valid(IO::parse_move("C3v")) == false);
        REQUIRE(board.is_valid(IO::parse_move("D2h")) == false);
        REQUIRE(board.is_valid(IO::parse_move("D3h")) == false); // <- Not removing.
        REQUIRE(board.is_valid(IO::parse_move("E2h")) == false);
        REQUIRE(board.is_valid(IO::parse_move("E3h")) == false);
        REQUIRE(board.is_valid(IO::parse_move("E2v")) == false);
        board.apply_move(IO::parse_move("E5v")); // 35
        board.apply_move(IO::parse_move("B4v")); // 36
        board.apply_move(IO::parse_move("B5v!"));// 37
        board.apply_move(IO::parse_move("C6v!"));// 38
        board.apply_move(IO::parse_move("B6v")); // 39
        board.apply_move(IO::parse_move("A3h")); // 40
    }
    SECTION("Game Test Player A vs Zuniq - CRASHED on move 34.") {
        board.apply_move(IO::parse_move("D5v")); // 1
        board.apply_move(IO::parse_move("E4h")); // 2
        board.apply_move(IO::parse_move("C6v")); // 3
        board.apply_move(IO::parse_move("D4v")); // 4
        board.apply_move(IO::parse_move("C4h")); // 5
        board.apply_move(IO::parse_move("D3h")); // 6
        board.apply_move(IO::parse_move("E2v")); // 7
        board.apply_move(IO::parse_move("E5v")); // 8
        board.apply_move(IO::parse_move("D2v")); // 9
        board.apply_move(IO::parse_move("B2v")); // 10
        board.apply_move(IO::parse_move("D1v")); // 11
        board.apply_move(IO::parse_move("F1h")); // 12
        board.apply_move(IO::parse_move("F4h")); // 13
        board.apply_move(IO::parse_move("C2h")); // 14
        board.apply_move(IO::parse_move("F3h")); // 15
        board.apply_move(IO::parse_move("E2h")); // 16
        board.apply_move(IO::parse_move("A1h")); // 17
        board.apply_move(IO::parse_move("A5h")); // 18
        board.apply_move(IO::parse_move("D4h")); // 19
        board.apply_move(IO::parse_move("C4v")); // 20
        board.apply_move(IO::parse_move("C3h")); // 21
        board.apply_move(IO::parse_move("E3v")); // 22
        board.apply_move(IO::parse_move("B1h")); // 23
        board.apply_move(IO::parse_move("A2h")); // 24
        board.apply_move(IO::parse_move("B2h")); // 25
        board.apply_move(IO::parse_move("D3v")); // 26
        board.apply_move(IO::parse_move("B3h")); // 27
        board.apply_move(IO::parse_move("D5h")); // 28
        board.apply_move(IO::parse_move("A4h")); // 29
        board.apply_move(IO::parse_move("C5h!"));// 30
        board.apply_move(IO::parse_move("E5h")); // 31
        REQUIRE(board.is_valid(IO::parse_move("E6v")));
        board.apply_move(IO::parse_move("B5v"));// 32
        REQUIRE(board.is_valid(IO::parse_move("E6v")));
        board.apply_move(IO::parse_move("E6v"));// 33
        REQUIRE(board.is_valid(IO::parse_move("C1h")));
        board.apply_move(IO::parse_move("C1h"));// 34
        board.apply_move(IO::parse_move("A4v"));// 35
        board.apply_move(IO::parse_move("B6v"));// 36
        board.apply_move(IO::parse_move("A3v"));// 37
        board.apply_move(IO::parse_move("E1h"));// 38
        board.apply_move(IO::parse_move("A5v"));// 39
        board.apply_move(IO::parse_move("C1v"));// 40
        REQUIRE(board.winner() == Player::BLACK);
    }
}

TEST_CASE("Games with subtle evaluation change", "[mcts]") {
    Board board;
    random_device dev;
    mt19937 rng(dev());
    unique_ptr<TimeStrategy> ts_white = make_unique<HardCodedTimeStrategy>(30'000U);
    unique_ptr<TimeStrategy> ts_black = make_unique<HardCodedTimeStrategy>(30'000U);
    auto white = MCTSAgent(60'000, 0.4, ts_white, Player::WHITE, rng);
    auto black = MCTSAgent(60'000, 0.4, ts_black, Player::BLACK, rng);
    Move move;
    bool is_winning;
    Context ctx = {{CTX_VAR::ROUND, 0}, {CTX_VAR::ELAPSED_TIME_MILLIS, 0}};
    int32_t &round = ctx[CTX_VAR::ROUND];
    int32_t &elapsed_time = ctx[CTX_VAR::ELAPSED_TIME_MILLIS];
    SECTION("Game 4 - Player3 vs Zuniq.") {
        board.apply_move(IO::parse_move("D4v")); // 1
        board.apply_move(IO::parse_move("D2h")); // 2
        board.apply_move(IO::parse_move("E5v")); // 3
        board.apply_move(IO::parse_move("C6v")); // 4
        board.apply_move(IO::parse_move("E4h")); // 5
        board.apply_move(IO::parse_move("E1v")); // 6
        board.apply_move(IO::parse_move("A4h")); // 7
        board.apply_move(IO::parse_move("C1h")); // 8
        board.apply_move(IO::parse_move("E1h")); // 9
        board.apply_move(IO::parse_move("D1v")); // 10
        board.apply_move(IO::parse_move("F2h")); // 11
        board.apply_move(IO::parse_move("A5v")); // 12
        board.apply_move(IO::parse_move("A1v")); // 13
        board.apply_move(IO::parse_move("C2v")); // 14
        board.apply_move(IO::parse_move("A1h")); // 15
        board.apply_move(IO::parse_move("D1h")); // 16
        board.apply_move(IO::parse_move("F3h")); // 17
        board.apply_move(IO::parse_move("C5h")); // 18
        board.apply_move(IO::parse_move("D5v")); // 19
        board.apply_move(IO::parse_move("B3v")); // 20
        board.apply_move(IO::parse_move("A2h")); // 21
        board.apply_move(IO::parse_move("E2h")); // 22
        board.apply_move(IO::parse_move("E5h")); // 23
        board.apply_move(IO::parse_move("B3h")); // 24
        board.apply_move(IO::parse_move("E3h")); // 25
        board.apply_move(IO::parse_move("C4v")); // 26
        board.apply_move(IO::parse_move("B5v")); // 27
        board.apply_move(IO::parse_move("B2h")); // 28
        board.apply_move(IO::parse_move("D5h")); // 29
        board.apply_move(IO::parse_move("C3h")); // 30
        board.apply_move(IO::parse_move("D2v")); // 31
        board.apply_move(IO::parse_move("F4h!"));// 32
        board.apply_move(IO::parse_move("E3v")); // 33
        cerr << "33. BLACK: " << endl;
        round = 32;
        elapsed_time = 20'000;
        std::tie(move, is_winning) = black.select_move(board, ctx);
        REQUIRE(move != IO::parse_move("C4h"));
        board.apply_move(IO::parse_move("C4h"));// 34
        cerr << endl;
        cerr << "34. WHITE: " << endl;
        round = 33;
        elapsed_time = 22'000;
        std::tie(move, is_winning) = white.select_move(board, ctx);
        REQUIRE(is_winning == true);
        board.apply_move(IO::parse_move("B6v!"));// 35
        cerr << endl;
        cerr << "35. BLACK: " << endl;
        round = 34;
        elapsed_time = 24'000;
        std::tie(move, is_winning) = black.select_move(board, ctx);
        REQUIRE(is_winning == false);
        board.apply_move(IO::parse_move("F5h"));// 36
        board.apply_move(IO::parse_move("A6v"));// 37
        board.apply_move(IO::parse_move("A3h"));// 38
        cerr << endl;
        cerr << "38. WHITE: " << endl;
        round = 37;
        elapsed_time = 28'000;
        std::tie(move, is_winning) = white.select_move(board, ctx);
        REQUIRE(is_winning == false);
        board.apply_move(IO::parse_move("B1v"));// 39
        REQUIRE(board.is_over());
        REQUIRE(board.winner() == Player::WHITE);
    }
}

TEST_CASE("Crashed games", "[games]") {
    Board board;
    board.apply_move(IO::parse_move("D5v")); // 1
    board.apply_move(IO::parse_move("E4h")); // 2
    board.apply_move(IO::parse_move("C6v")); // 3
    board.apply_move(IO::parse_move("D4v")); // 4
    board.apply_move(IO::parse_move("C4h")); // 5
    board.apply_move(IO::parse_move("D3h")); // 6
    board.apply_move(IO::parse_move("E2v")); // 7
    board.apply_move(IO::parse_move("E5v")); // 8
    board.apply_move(IO::parse_move("D2v")); // 9
    board.apply_move(IO::parse_move("B2v")); // 10
    board.apply_move(IO::parse_move("D1v")); // 11
    board.apply_move(IO::parse_move("F1h")); // 12
    board.apply_move(IO::parse_move("F4h")); // 13
    board.apply_move(IO::parse_move("C2h")); // 14
    board.apply_move(IO::parse_move("F3h")); // 15
    board.apply_move(IO::parse_move("E2h")); // 16
    board.apply_move(IO::parse_move("A1h")); // 17
    board.apply_move(IO::parse_move("A5h")); // 18
    board.apply_move(IO::parse_move("D4h")); // 19
    board.apply_move(IO::parse_move("C4v")); // 20
    board.apply_move(IO::parse_move("C3h")); // 21
    board.apply_move(IO::parse_move("E3v")); // 22
    board.apply_move(IO::parse_move("B1h")); // 23
    board.apply_move(IO::parse_move("A2h")); // 24
    board.apply_move(IO::parse_move("B2h")); // 25
    board.apply_move(IO::parse_move("D3v")); // 26
    board.apply_move(IO::parse_move("B3h")); // 27
    board.apply_move(IO::parse_move("D5h")); // 28
    board.apply_move(IO::parse_move("A4h")); // 29
    board.apply_move(IO::parse_move("C5h!"));// 30
    board.apply_move(IO::parse_move("E5h")); // 31
    board.apply_move(IO::parse_move("B5v")); // 32
    board.apply_move(IO::parse_move("E6v")); // 33
    board.apply_move(IO::parse_move("C1h")); // 34
    board.apply_move(IO::parse_move("A4v")); // 35
    board.apply_move(IO::parse_move("B6v")); // 36
    board.apply_move(IO::parse_move("A3v")); // 37
    board.apply_move(IO::parse_move("E1h")); // 38
    board.apply_move(IO::parse_move("A5v")); // 39
    board.apply_move(IO::parse_move("C1v")); // 40
}