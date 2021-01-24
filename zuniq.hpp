#ifndef ZUNIQ_ZUNIQ_HPP
#define ZUNIQ_ZUNIQ_HPP

#include "config.hpp"
#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

using namespace std;

#define MP make_pair
#define ALL(c) (c).begin(), (c).end()
#define SZ(c) (int) (c).size()


using Move = pair<uint32_t, uint32_t>;


enum class CTX_VAR {
    ROUND,
    ELAPSED_TIME_MILLIS,
};

using Context = unordered_map<CTX_VAR, int32_t>;


// constants.
static constexpr uint8_t N = 6;
static constexpr size_t TOTAL_SQUARES = (N - 1) * (N - 1);
static constexpr size_t TOTAL_MOVES = N * (N - 1) * 2;
static constexpr pair<int8_t, int8_t> NEIGH_SQ[4] = {{-1, 0},
                                                     {1, 0},
                                                     {0, -1},
                                                     {0, 1}};


static constexpr int TOP_N_FIRST_LEVEL = 5;
static constexpr int TOP_N_SECOND_LEVEL = 5;
static constexpr unsigned int TOTAL_TIME_MILLIS = 30'000U;

struct MoveHash {
    size_t operator()(const Move &m) const {
        return TOTAL_MOVES * (m.first) + m.second;
    }
};

ostream &operator<<(ostream &os, const Move &m) {
    os << "Move(" << m.first << "," << m.second << ")";
    return os;
}


enum class Player {
    WHITE,
    BLACK
};

enum class GameCommand {
    START,
    QUIT,
};

template<typename T>
struct UnionFind {

    UnionFind() = default;

    UnionFind(const UnionFind &rhs) = default;

    explicit UnionFind(size_t max) {
        resize(max);
    }

    void resize(size_t max) {
        parent.assign(max, -1);
    }

    [[nodiscard]] T find_set_const(T e) const {
        if (parent[e] < 0) {
            return e;
        } else {
            T root = e;

            /* Find root */
            while (parent[root] >= 0) {
                root = parent[root];
            }

            return root;
        }
    }

    T find_set(T e) {
        if (parent[e] < 0) {
            return e;
        } else {
            T root = e;

            /* Find root */
            while (parent[root] >= 0) {
                root = parent[root];
            }

            /* No need of path compression */
            if (parent[e] == static_cast<int32_t>(root)) return root;

            /* Compress path */
            while (parent[e] >= 0) {
                T tmp = parent[e];
                parent[e] = root;
                e = tmp;
            }

            return root;
        }
    }


    void union_set(T e1, T e2) {
        e1 = find_set(e1);
        e2 = find_set(e2);
        if (e1 != e2) {
            if (parent[e1] < parent[e2]) {
                parent[e1] += parent[e2];
                parent[e2] = e1;
            } else {
                parent[e2] += parent[e1];
                parent[e1] = e2;
            }
        }
    }

private:
    vector<T> parent;
};

/**
 * From @mcleary: https://gist.github.com/mcleary/b0bf4fa88830ff7c882d
 */
class Timer {

    std::chrono::time_point<std::chrono::steady_clock> start_time_;
    std::chrono::time_point<std::chrono::steady_clock> end_time_;
    bool is_running = false;

    std::chrono::time_point<std::chrono::steady_clock> elapsed() {
        if (is_running) {
            return std::chrono::steady_clock::now();
        } else {
            return end_time_;
        }
    }

public:
    Timer &start() {
        start_time_ = std::chrono::steady_clock::now();
        is_running = true;
        return *this;
    }

    void stop() {
        end_time_ = std::chrono::steady_clock::now();
        is_running = false;
    }

    int64_t elapsed_milli() {
        auto end_time = elapsed();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_).count();
    }

    int64_t elapsed_micro() {
        auto end_time = elapsed();
        return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_).count();
    }
};


struct Board {

    Board() : turn_(Player::WHITE), uf_(N * N) {
        init_available_moves();
    }

    Board(const Board &rhs) = default;

    Board(Board &&rhs) = default;

    Board &operator=(const Board &rhs) = default;

    Board &operator=(Board &&rhs) = default;

    bool is_over() const {
        return available_moves_.empty();
    }

    Player winner() const {
        assert(is_over());
        Player winner = Player::WHITE;
        if (get_turn() == Player::WHITE) winner = Player::BLACK;
        return winner;
    }

    [[nodiscard]] const Player &get_turn() const {
        return turn_;
    }

    [[nodiscard]] const unordered_set<Move, MoveHash> &get_available_moves() const {
        return available_moves_;
    }

    [[nodiscard]] bool is_valid(Move m) const {
        return available_moves_.count(m) > 0;
    }

    inline bool is_closing_region(const Move &m) const {
        return (uf_.find_set_const(m.first) == uf_.find_set_const(m.second));
    }

    void apply_move(const Move &m) {
        assert(is_valid(m));
        applied_moves_.insert(m);
        if (uf_.find_set(m.first) == uf_.find_set(m.second)) {
            const auto &rc = count_regions(m);
            assert(rc.first > 0);
            assert(closed_sizes_.count(rc.first) == 0);
            closed_sizes_.insert(rc.first);
            const auto &squares_inside_close_region = rc.second;
            for (const auto sq : squares_inside_close_region) {
                available_moves_.erase(get_line_above(sq));
                on_hold_moves_.erase(get_line_above(sq));

                available_moves_.erase(get_line_left(sq));
                on_hold_moves_.erase(get_line_left(sq));

                available_moves_.erase(get_line_right(sq));
                on_hold_moves_.erase(get_line_right(sq));

                available_moves_.erase(get_line_bellow(sq));
                on_hold_moves_.erase(get_line_bellow(sq));
            }
        }

        uf_.union_set(m.first, m.second);
        available_moves_.erase(m);
        exclude_invalid_next_moves();
        change_turn();
    }

    // Get square number above a horizontal move.
    static inline uint32_t get_square_above(uint32_t move_first) {
        uint32_t row = get_line_row(move_first);
        uint32_t col = get_line_col(move_first);
        assert(row > 0);
        return (N - 1) * (row - 1) + col;
    }

    // Get square number bellow a horizontal move.
    static inline uint32_t get_square_bellow(uint32_t move_first) {
        uint32_t row = get_line_row(move_first);
        uint32_t col = get_line_col(move_first);
        assert(row < N - 1);
        return (N - 1) * row + col;
    }

    // Get square number to the left of a vertical move.
    static inline uint32_t get_square_left(uint32_t move_first) {
        uint32_t row = get_line_row(move_first);
        uint32_t col = get_line_col(move_first);
        assert(col > 0);
        return (N - 1) * row + col - 1;
    }

    // Get square number to the right of a vertical move.
    static inline uint32_t get_square_right(uint32_t move_first) {
        uint32_t row = get_line_row(move_first);
        uint32_t col = get_line_col(move_first);
        assert(col < N - 1);
        return (N - 1) * row + col;
    }

    static inline Move get_line_left(uint32_t square) {
        assert(square < TOTAL_SQUARES);
        uint32_t row = get_square_row(square);
        uint32_t col = get_square_col(square);
        uint32_t start = get_move_start(row, col);
        return MP(start, start + 6);
    }

    static inline Move get_line_right(uint32_t square) {
        assert(square < TOTAL_SQUARES);
        uint32_t row = get_square_row(square);
        uint32_t col = get_square_col(square);
        uint32_t start = get_move_start(row, col + 1);
        return MP(start, start + 6);
    }

    static inline Move get_line_above(uint32_t square) {
        assert(square < TOTAL_SQUARES);
        uint32_t row = get_square_row(square);
        uint32_t col = get_square_col(square);
        uint32_t start = get_move_start(row, col);
        return MP(start, start + 1);
    }

    static inline Move get_line_bellow(uint32_t square) {
        assert(square < TOTAL_SQUARES);
        uint32_t row = get_square_row(square);
        uint32_t col = get_square_col(square);
        uint32_t start = get_move_start(row + 1, col);
        return MP(start, start + 1);
    }


private:
    [[nodiscard]] pair<size_t, set<uint32_t>> count_regions(Move candidate_move) const {
        queue<uint32_t> Q1;
        queue<uint32_t> Q2;

        if (candidate_move.second - candidate_move.first == 1) {
            // horizontal move.
            if (get_line_row(candidate_move.first) > 0) {
                // include square above line
                Q1.push(get_square_above(candidate_move.first));
            }
            if (get_line_row(candidate_move.first) < N - 1) {
                // includes square bellow line
                Q2.push(get_square_bellow(candidate_move.first));
            }
        } else {
            // vertical move.
            if (get_line_col(candidate_move.first) > 0) {
                // include square left to line
                Q1.push(get_square_left(candidate_move.first));
            }
            if (get_line_col(candidate_move.first) < N - 1) {
                // includes square right to line
                Q2.push(get_square_right(candidate_move.first));
            }
        }
        auto ans = BFS(Q1);
        if (ans.first <= 0) {
            ans = BFS(Q2);
        }
        return ans;
    }

    /**
     * Exclude moves that would make closed regions with sizes in closed_region.
     */
    void exclude_invalid_next_moves() {
        vector<Move> to_exclude;

        // Finds out which ones must be excluded
        available_moves_.insert(ALL(on_hold_moves_));
        if (!on_hold_moves_.empty()) {
            to_exclude.reserve(available_moves_.size());
        }
        on_hold_moves_.clear();
        for (const auto &move : available_moves_) {
            if (uf_.find_set(move.first) == uf_.find_set(move.second)) {
                applied_moves_.insert(move);
                auto rc = count_regions(move);
                applied_moves_.erase(move);
                if (rc.first > 0 && closed_sizes_.count(rc.first) > 0) {
                    bool exclusion_flag = true;
                    for (size_t reg_cnt = 1; reg_cnt <= rc.first; reg_cnt++) {
                        exclusion_flag &= (closed_sizes_.count(reg_cnt) > 0);
                    }
                    if (!exclusion_flag) on_hold_moves_.insert(move);
                    to_exclude.push_back(move);
                }
            }
        }

        // Excludes then
        for (auto &move : to_exclude) {
            available_moves_.erase(move);
        }
    }

    static inline uint32_t get_line_row(uint32_t pos) {
        return pos / N;
    }

    static inline uint32_t get_line_col(uint32_t pos) {
        return pos % N;
    }

    static inline uint32_t get_square_row(uint32_t pos) {
        return pos / (N - 1);
    }

    static inline uint32_t get_square_col(uint32_t pos) {
        return pos % (N - 1);
    }

    static inline uint32_t get_move_start(uint32_t row, uint32_t col) {
        return (row * N) + col;
    }

    inline void change_turn() {
        if (turn_ == Player::WHITE) turn_ = Player::BLACK;
        else
            turn_ = Player::WHITE;
    }

    void init_available_moves() {
        for (size_t row = 0; row < N; row++) {
            for (size_t col = 0; col < N - 1; col++) {
                uint32_t start = N * row + col;
                available_moves_.emplace(start, start + 1);
            }
        }

        for (size_t row = 0; row < N - 1; row++) {
            for (size_t col = 0; col < N; col++) {
                uint32_t start = N * row + col;
                available_moves_.emplace(start, start + 6);
            }
        }
        assert(available_moves_.size() == TOTAL_MOVES);
    }

    [[nodiscard]] pair<size_t, set<uint32_t>> BFS(queue<uint32_t> &Q) const {
        set<uint32_t> visited;
        if (Q.empty()) return MP(0, visited);
        visited.insert(Q.front());
        function<bool(int32_t, int32_t)> valid = [&visited](int32_t row, int32_t col) {
            if (row < 0 || col < 0 || row >= static_cast<int32_t>(N) || col >= static_cast<int32_t>(N) ||
                visited.count((N - 1) * row + col) > 0)
                return false;

            return true;
        };

        function<bool(uint32_t, uint32_t)> is_open = [this](uint32_t row, uint32_t col) {
            assert(row < (uint32_t) N);
            assert(col < (uint32_t) N);
            uint32_t sq = (N - 1) * row + col;
            if ((row == 0 && applied_moves_.count(get_line_above(sq)) == 0) ||
                (col == 0 && applied_moves_.count(get_line_left(sq)) == 0) ||
                (row == N - 2 && applied_moves_.count(get_line_bellow(sq)) == 0) ||
                (col == N - 2 && applied_moves_.count(get_line_right(sq)) == 0)) {
                return true;
            }
            return false;
        };

        size_t cnt = 0;
        while (!Q.empty()) {
            size_t sz = Q.size();
            while (sz--) {
                uint32_t sq = Q.front();
                Q.pop();
                cnt++;
                int32_t row = get_square_row(sq);
                int32_t col = get_square_col(sq);

                if (is_open(row, col)) {
                    cnt = 0;
                    visited.clear();
                    goto fim;
                }

                for (auto &[dr, dc] : NEIGH_SQ) {
                    if (dr < 0 && applied_moves_.count(get_line_above(sq)) > 0) continue;
                    if (dr > 0 && applied_moves_.count(get_line_bellow(sq)) > 0) continue;
                    if (dc < 0 && applied_moves_.count(get_line_left(sq)) > 0) continue;
                    if (dc > 0 && applied_moves_.count(get_line_right(sq)) > 0) continue;
                    uint32_t new_row = row + dr;
                    uint32_t new_col = col + dc;
                    if (valid(new_row, new_col)) {
                        uint32_t new_sq = (N - 1) * new_row + new_col;
                        Q.push(new_sq);
                        visited.insert(new_sq);
                    }
                }
            }
        }
    fim:
        return MP(cnt, visited);
    }

    // Who is the turn_ to play;
    Player turn_;

    // Available moves
    unordered_set<Move, MoveHash> available_moves_;

    //Keep track of connected components
    UnionFind<int8_t> uf_;

    // keep record of closed area sizes
    unordered_set<size_t> closed_sizes_;

    // Executed moves.
    unordered_set<Move, MoveHash> applied_moves_;
    unordered_set<Move, MoveHash> on_hold_moves_;
};

namespace IO {
    string readln() {
        string input;
        getline(cin, input);
        return input;
    }

    void writeln(string_view s) {
        cout << s << endl;
    }

    Move parse_move(string_view s) {
        assert(s.size() == 3 || (s.size() == 4 && s[3] == '!'));
        assert((tolower(s[2]) == 'v') || (tolower(s[2]) == 'h'));
        uint32_t row = toupper(s[0]) - 'A';
        assert(row < N);

        uint32_t col = s[1] - '1';
        assert(col < N);

        uint32_t start = (N * row) + col;
        uint32_t end;
        if (s[2] == 'h') {
            assert(col < N - 1);
            end = start + 1;
        } else {
            assert(row < N - 1);
            end = start + N;
        }
        assert(end < N * N);

        return MP(start, end);
    }

    string format_move(Move m) {
        assert(m.first < m.second);
        assert(m.second < 36);

        string ans;
        ans.resize(3);
        ans[0] = (m.first / N) + 'A';
        ans[1] = (m.first % N) + '1';

        if (m.second - m.first == 1) {
            ans[2] = 'h';
        } else {
            assert(m.second - m.first == N);
            ans[2] = 'v';
        }

        return ans;
    }

    variant<Move, GameCommand> parse_input(string_view s) {
        variant<Move, GameCommand> resp;
        string input;
        input.resize(s.size());
        std::transform(s.begin(), s.end(), input.begin(),
                       [](unsigned char c) { return tolower(c); });
        if (input == "start") {
            resp = GameCommand::START;
        } else if (input == "quit") {
            resp = GameCommand::QUIT;
        } else {
            resp = parse_move(input);
        }

        return resp;
    }

}// namespace IO

// --------- TimeStrategy ------------//
class TimeStrategy {
    uint32_t total_time_millis_;

protected:
    [[nodiscard]] uint32_t get_total_time_millis() const noexcept { return total_time_millis_; }

public:
    explicit TimeStrategy(uint32_t total_time_millis) : total_time_millis_(total_time_millis) {}
    [[nodiscard]] virtual double max_move_time(const Context &) const noexcept = 0;
};

class ConstantTimeStrategy : public TimeStrategy {

public:
    explicit ConstantTimeStrategy(uint32_t total_time_millis) : TimeStrategy(total_time_millis) {}
    [[nodiscard]] double max_move_time(const Context &) const noexcept override {
        return (0.995 * get_total_time_millis()) / 20.0;
    }
};

class RemainingTimeStrategy : public TimeStrategy {
public:
    explicit RemainingTimeStrategy(uint32_t total_time_millis) : TimeStrategy(total_time_millis) {}
    [[nodiscard]] double max_move_time(const Context &ctx) const noexcept override {
        assert(ctx.count(CTX_VAR::ROUND) && ctx.count(CTX_VAR::ELAPSED_TIME_MILLIS));
        uint32_t remaining_moves = max(1, 20 - ctx.at(CTX_VAR::ROUND) / 2);
        uint32_t remaining_time = get_total_time_millis() - ctx.at(CTX_VAR::ELAPSED_TIME_MILLIS);
        double move_time = static_cast<double>(0.999 * remaining_time) / remaining_moves;
        return move_time;
    }
};

class HardCodedTimeStrategy : public TimeStrategy {
public:
    explicit HardCodedTimeStrategy(uint32_t total_time_millis) : TimeStrategy(total_time_millis) {}
    [[nodiscard]] double max_move_time(const Context &ctx) const noexcept override {
        assert(ctx.count(CTX_VAR::ROUND) && ctx.count(CTX_VAR::ELAPSED_TIME_MILLIS));
        uint32_t round = ctx.at(CTX_VAR::ROUND);
        if (round < 10) return 48.00 * 2;
        else if (round < 20)
            return 148.00 * 2;
        else if (round < 25)
            return 750.00 * 2;
        else if (round < 35)
            return 2'350.00 * 2;
        else if (round < 37)
            return 200.00 * 2;
        else
            return 25.00 * 2;
    }
};

// --------- Agent -------------------//
struct Agent {
    explicit Agent(Player color) : color_(color) {}

    void set_color(Player color) { color_ = color; }

    virtual pair<Move, bool> select_move(const Board &, const Context &) = 0;

    virtual ~Agent() = default;

protected:
    Player color_;
};

// --------- RandomAgent -------------------//
struct RandomAgent : public Agent {
    RandomAgent(Player color, mt19937 rng, bool with_priority = false, bool verbose = false) : Agent(color),
                                                                                               rng_(rng),
                                                                                               with_priority_(with_priority),
                                                                                               verbose_(verbose) {}

    pair<Move, bool> select_move(const Board &b, const Context &) override {
        assert(b.get_turn() == color_);
        Timer timer = Timer().start();

        pair<Move, bool> pmove;
        if (with_priority_) {
            pmove = select_move_with_priority(b);
        } else {
            pmove = select_move_no_priority(b);
        }

        if (verbose_) {
            timer.stop();
#ifndef QUIET_MODE
            cerr << "[I]: RNDEngine selected: " << IO::format_move((pmove).first) << " in "
                 << timer.elapsed_micro() << " µs." << endl;
#endif
        }

        return pmove;
    }

private:
    mt19937 rng_;
    bool with_priority_;
    bool verbose_;


    pair<Move, bool> select_move_with_priority(const Board &board) {
        assert(board.get_turn() == color_);

        vector<Move> moves(ALL(board.get_available_moves()));

        vector<double> prob(moves.size());

        size_t i = 0;
        for (Move move : moves) {
            if (board.is_closing_region(move)) {
                prob[i] = WEIGHT_CLOSE_REGION_MOVE;
            } else {
                prob[i] = WEIGHT_REGULAR_MOVE;
            }
            ++i;
        }

        discrete_distribution dist(prob.begin(), prob.end());

        int offset = dist(rng_);

        return make_pair(moves[offset], false);
    }

    pair<Move, bool> select_move_no_priority(const Board &board) {
        assert(board.get_turn() == color_);

        const auto &moves = board.get_available_moves();

        size_t num_moves = moves.size();

        size_t offset = uniform_int_distribution<size_t>(0, num_moves - 1)(rng_);

        return make_pair(*std::next(moves.begin(), offset), false);
    }
};

// --------- MCTS AGENT ---------------//
class MCTSAgent : public Agent {
    uint32_t num_rounds_;
    double temperature_;
    unique_ptr<TimeStrategy> ts_;
    mt19937 &rng_;
    bool sent_is_winning_;
    struct ScoredMove;
    // --------- MCTSNode -------------------//
    struct MCTSNode {

        MCTSNode(const shared_ptr<Board> &game_state, MCTSNode *parent, std::optional<Move> &&move, mt19937 &rng) : game_state_(game_state), parent_(parent), move_(move), rng_(rng), num_rollouts_(0),
                                                                                                                    white_win_counts_(0), black_win_count_(0),
                                                                                                                    unvisited_moves_(ALL(game_state->get_available_moves())) {
            std::shuffle(ALL(unvisited_moves_), rng_);
            children_.reserve(unvisited_moves_.size());
        }

        MCTSNode(const MCTSNode &rhs) = default;

        MCTSNode(MCTSNode &&rhs) = default;

        MCTSNode &operator=(const MCTSNode &rhs) = delete;

        MCTSNode &operator=(MCTSNode &&rhs) = delete;

        [[nodiscard]] std::optional<Move> get_move() const noexcept { return move_; }

        MCTSNode &add_random_child() {
            Move &new_move = unvisited_moves_.back();
            unvisited_moves_.pop_back();
            shared_ptr<Board> new_game_state = std::make_shared<Board>(*game_state_);
            assert(new_game_state != game_state_);
            new_game_state->apply_move(new_move);
            children_.emplace_back(new_game_state, this, make_optional(new_move), rng_);
            return children_.back();
        }

        void record_win(Player winner) noexcept {
            if (winner == Player::WHITE) ++white_win_counts_;
            else
                ++black_win_count_;
            ++num_rollouts_;
        }

        [[nodiscard]] inline bool can_add_child() const noexcept {
            return SZ(unvisited_moves_) > 0;
        }

        [[nodiscard]] inline bool is_terminal() const noexcept {
            return game_state_->is_over();
        }

        [[nodiscard]] double winning_frac(Player player) const noexcept {
            double win_counts = black_win_count_;
            if (player == Player::WHITE) win_counts = white_win_counts_;
            return win_counts / double(num_rollouts_);
        }

        [[nodiscard]] vector<ScoredMove> top_n(size_t n) const noexcept {
            vector<ScoredMove> scored_moves;
            const size_t CHILDREN_SIZE = this->children_.size();
            scored_moves.reserve(CHILDREN_SIZE);
            for (size_t i = 0; i < CHILDREN_SIZE; i++) {
                const auto &child = children_[i];
                assert(child.move_.has_value());
                scored_moves.emplace_back(child.winning_frac(game_state_->get_turn()), &child,
                                          child.num_rollouts_);
            }
            sort(ALL(scored_moves));
            vector<ScoredMove> top_n(scored_moves.begin(), scored_moves.begin() + min(n, CHILDREN_SIZE));
            return top_n;
        }

    private:
        shared_ptr<Board> game_state_;
        MCTSNode *const parent_;
        optional<Move> move_;
        mt19937 &rng_;
        uint32_t num_rollouts_;
        uint32_t white_win_counts_;
        uint32_t black_win_count_;
        vector<Move> unvisited_moves_;
        vector<MCTSNode> children_;
        friend class MCTSAgent;
    };// end of struct MCTSNode.


    // --------- ScoredMove -------------------//
    struct ScoredMove {
        double winning_fraction_;
        const MCTSNode *node_;
        uint32_t num_rollouts_;

        ScoredMove(double wf, const MCTSNode *const node, int nr) : winning_fraction_(wf), node_(node), num_rollouts_(nr) {}

        bool operator<(const ScoredMove &other) const { return winning_fraction_ > other.winning_fraction_; }

        friend ostream &operator<<(ostream &out, const ScoredMove &sd) noexcept {
            out << IO::format_move(sd.node_->get_move().value()) << ' ' << std::setprecision(2) << sd.winning_fraction_ << '('
                << sd.num_rollouts_ << ')';
            return out;
        }

    };// end of struct ScoredMove

public:
    MCTSAgent(uint32_t num_rounds, double temperature, unique_ptr<TimeStrategy> &ts, Player color, mt19937 &rng) : Agent(color),
                                                                                                                   num_rounds_(num_rounds),
                                                                                                                   temperature_(temperature),
                                                                                                                   ts_(std::move(ts)),
                                                                                                                   rng_(rng),
                                                                                                                   sent_is_winning_(false) {}

    MCTSAgent(const MCTSAgent &rhs) = delete;

    MCTSAgent(MCTSAgent &&rhs) = delete;

    MCTSAgent &operator=(const MCTSAgent &rhs) = delete;

    MCTSAgent &operator=(MCTSAgent &&rhs) = delete;

    pair<Move, bool> select_move(const Board &game_state, const Context &ctx) override {
        assert(color_ == game_state.get_turn());
        Timer timer = Timer().start();
        auto sgs = make_shared<Board>(game_state);
        MCTSNode root = MCTSNode(sgs, nullptr, make_optional<Move>(), rng_);
        for (uint32_t i = 0; i < num_rounds_; i++) {
            if ((i % 10 == 0) && (timer.elapsed_milli() >= ts_->max_move_time(ctx))) {
#ifndef QUIET_MODE                
                cerr << "[I]: num_rounds: " << i << "/" << num_rounds_ << endl;
#endif                
                break;
            }
            MCTSNode *node = &root;
            while (!node->can_add_child() && !node->is_terminal()) {
                node = this->select_child(node);
            }

            // Add a new child node into the tree.
            if (node->can_add_child()) {
                node = &node->add_random_child();
            }

            // Simulate a random game from this node.
            Player winner = this->simulate_random_game(*node->game_state_, ctx);

            // Propagate scores back up the tree.
            while (node != nullptr) {
                node->record_win(winner);
                node = node->parent_;
            }
        }


#ifndef QUIET_MODE
        auto scored_moves = root.top_n(TOP_N_FIRST_LEVEL);

        cerr << "[I]: Top " << SZ(scored_moves) << " moves:\n";

        for (auto &score : scored_moves) {
            cerr << "[I]: " << score << "\n";

            auto sl_scored_moves = score.node_->top_n(TOP_N_SECOND_LEVEL);

            cerr << "[I]:\t\t";
            for (auto &score2 : sl_scored_moves) {
                cerr << score2 << ",";
            }
            cerr << "\n";
        }

        cerr.flush();
#endif

        const Move *best_move = nullptr;
        double best_pct = -1.0;
        for (auto &child : root.children_) {
            double child_pct = child.winning_frac(game_state.get_turn());
            if (child_pct > best_pct) {
                best_pct = child_pct;
                assert(child.move_.has_value());
                best_move = &child.move_.value();
            }
        }
        assert(best_move != nullptr);
        bool is_winning = (best_pct > IS_WINNING_THRESHOLD) && (!sent_is_winning_) && (ctx.at(CTX_VAR::ROUND) >= MIN_ROUND_TO_CLAIM_IS_WINNING);
        sent_is_winning_ = (sent_is_winning_ || is_winning);

#ifndef QUIET_MODE
        timer.stop();
        cerr << "[I]: Selected: " << IO::format_move(*best_move) << (is_winning ? "!" : "") << " in "
             << timer.elapsed_milli() << " ms." << endl;
#endif
        return make_pair(*best_move, is_winning);
    }

private:
    /**
     * Select a child according to the UCT metric.
     * @param node : MCTSNode pointing to parent
     * @return pointer to best MCTSNode child of this parent.
     */
    MCTSNode *select_child(MCTSNode *const node) const noexcept {
        uint32_t total_rollouts = node->num_rollouts_;
        double log_rollouts = log(total_rollouts);

        double best_score = -1.0;

        MCTSNode *best_child = nullptr;

        for (int i = 0; i < SZ(node->children_); i++) {
            MCTSNode &child = node->children_[i];
            double win_percentage = child.winning_frac(node->game_state_->get_turn());
            double exploration_factor = sqrt(log_rollouts / child.num_rollouts_);
            double uct_score = win_percentage + temperature_ * exploration_factor;
            if (uct_score > best_score) {
                best_score = uct_score;
                best_child = &node->children_[i];
            }
        }

        assert(best_child != nullptr);
        return best_child;
    }

    [[nodiscard]] Player simulate_random_game(Board game, const Context &ctx) const noexcept {
        RandomAgent white_bot(Player::WHITE, rng_, WHITE_USE_WEIGHT_ROLLOUT, false);
        RandomAgent black_bot(Player::BLACK, rng_, BLACK_USE_WEIGHT_ROLLOUT, false);
        RandomAgent *bot;
        while (!game.is_over()) {
            if (game.get_turn() == Player::WHITE) bot = &white_bot;
            else
                bot = &black_bot;
            auto [bot_move, _] = bot->select_move(game, ctx);
            std::ignore = _;// pleases compiler warning.
            game.apply_move(bot_move);
        }
        return game.winner();
    }

};// End of class MCTSAgent

void game_loop() {
    random_device dev;
    mt19937 rng(dev());

    unique_ptr<TimeStrategy> ts = make_unique<RemainingTimeStrategy>(TOTAL_TIME_MILLIS);

    unique_ptr<Agent> rnd_engine = make_unique<RandomAgent>(Player::BLACK, rng, false, true);

    unique_ptr<Agent> mcts_engine = make_unique<MCTSAgent>(NUM_ROUNDS, TEMPERATURE, ts, Player::BLACK, rng);

    Board board;

    Context ctx;
    int32_t &round_number = ctx[CTX_VAR::ROUND];
    int32_t &elapsed_time = ctx[CTX_VAR::ELAPSED_TIME_MILLIS];

    Timer timer;
    Timer opponent_timer;

    bool finished = false;
    round_number = 0;
    elapsed_time = 0;

    while (not finished) {
        string input = IO::readln();
        timer.start();
        auto parsed = IO::parse_input(input);
        if (holds_alternative<GameCommand>(parsed)) {

            GameCommand cmd = std::get<GameCommand>(parsed);
            switch (cmd) {
                case GameCommand::QUIT: {
                    finished = true;
#ifndef QUIET_MODE
                    if (!board.is_over()) {
                        auto moves = board.get_available_moves();
                        cerr << "[I]: Received QUIT!" << endl;
                        cerr << "[I]: Remaining " << moves.size() << " moves:" << endl;
                        for (Move move : moves) {
                            cerr << "[I]: " << IO::format_move(move) << endl;
                        }
                        opponent_timer.stop();
                        cerr << "[I]: Opp took " << opponent_timer.elapsed_milli() << "ms." << endl;
                    }
#endif
                } break;

                case GameCommand::START: {
                    rnd_engine->set_color(Player::WHITE);
                    mcts_engine->set_color(Player::WHITE);
                    const auto &[move, _] = rnd_engine->select_move(board, ctx);
                    std::ignore = _;
                    if (!board.is_valid(move))
                        throw std::runtime_error("Move " + IO::format_move(move) + " is invalid!");
                    board.apply_move(move);
                    ++round_number;
                    IO::writeln(IO::format_move(move));
                    opponent_timer.start();
                    elapsed_time += timer.elapsed_milli();
                } break;
            }
        } else if (holds_alternative<Move>(parsed)) {
            Move move = std::get<Move>(parsed);
#ifndef QUIET_MODE            
            cerr << "[I]: Opp took " << opponent_timer.elapsed_milli() << "ms " << IO::format_move(move) << '\n';
#endif
            bool is_winning = false;
            if (!board.is_valid(move)) throw std::runtime_error("Move " + IO::format_move(move) + " is invalid!");
            board.apply_move(move);
            ++round_number;
#ifndef QUIET_MODE            
            cerr << "[I]: rnd " << round_number << " #avail_moves " << board.get_available_moves().size() << endl;
#endif            
            elapsed_time += timer.elapsed_milli();
            timer.start();
            if (round_number < NUM_RANDOM_MOVE_ROUNDS)
                tie(move, is_winning) = rnd_engine->select_move(board, ctx);
            else
                tie(move, is_winning) = mcts_engine->select_move(board, ctx);
            if (!board.is_valid(move)) throw std::runtime_error("Move " + IO::format_move(move) + " is invalid!");
            board.apply_move(move);
            string ws = is_winning ? "!" : "";
            IO::writeln(IO::format_move(move) + ws);
            opponent_timer.start();
            ++round_number;
            elapsed_time += timer.elapsed_milli();
        } else {
            throw std::runtime_error("Unrecognized input: " + input);
        }
    }// end of game_loop;
}

#endif//ZUNIQ_ZUNIQ_HPP
