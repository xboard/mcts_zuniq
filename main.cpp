/**
 * An AI to play the game Zuniq.
 * This is my submission to CodeCup's 2021 tournament (https://www.codecup.nl/zuniq/rules.php).
 *
 * @author: Flavio Regis de Arruda <xboard>
 * @copyright: Flavio Regis de Arruda <xboard>
 */
#ifndef NDEBUG
#define NDEBUG
#endif

#include "zuniq.hpp"

int main() {
    cerr << "Zuniq version 0.8.0" << " (" << __DATE__ << " " << __TIME__ << ")" << endl;
#ifdef QUIET_MODE
    cerr << "Zuniq rules: https://www.codecup.nl/zuniq/rules.php" << endl;
    cerr << "For more information, check: https://github.com/xboard/mcts_zuniq" << endl;
    cerr << "Type a valid move or 'Start' to play as black. Type 'Quit' to finish game." << endl;
#endif
    game_loop();
    return 0;
}
