def generate_game_test(moves: [str]) -> None:
    for i, move in enumerate(moves):
        print(f'board.apply_move(IO::parse_move("{move}")); // {i + 1}')


if __name__ == "__main__":
    move_list = ['D5v', 'E4h', 'C6v', 'D4v', 'C4h', 'D3h', 'E2v', 'E5v', 'D2v', 'B2v', 'D1v', 'F1h', 'F4h', 'C2h',
                 'F3h', 'E2h', 'A1h', 'A5h', 'D4h', 'C4v', 'C3h', 'E3v', 'B1h', 'A2h', 'B2h', 'D3v', 'B3h', 'D5h',
                 'A4h', 'C5h!', 'E5h', 'B5v', 'E6v', 'C1h', 'A4v', 'B6v', 'A3v', 'E1h', 'A5v', 'C1v']

    generate_game_test(move_list)
