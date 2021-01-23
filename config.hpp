
/* BEGINNING OF CONSTANTS AFFECTING MCTS ALGORITHM */

static constexpr int NUM_RANDOM_MOVE_ROUNDS = 10;

static constexpr int NUM_ROUNDS = 32'768;//65'536;

static constexpr double TEMPERATURE = 0.45;

static constexpr double IS_WINNING_THRESHOLD = 0.80;

static constexpr double WEIGHT_CLOSE_REGION_MOVE = 1.0;

static constexpr double WEIGHT_REGULAR_MOVE = 1.0;

static constexpr bool WHITE_USE_WEIGHT_ROLLOUT = false;

static constexpr bool BLACK_USE_WEIGHT_ROLLOUT = false;

static constexpr int MIN_ROUND_TO_CLAIM_IS_WINNING = 20;

/* END OF CONSTANTS AFFECTING MCTS ALGORITHM */
