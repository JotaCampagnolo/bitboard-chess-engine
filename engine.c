/******************************************************************************\
============================ BITBOARD CHESS ENGINE =============================

																			by

											 JotaCampagnolo & Code Monkey King

\******************************************************************************/

// System headers:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef WIN64
#include <windows.h>
#else
#include <sys/time.h>
#endif

// Define the engine version:
#define version "1.1"

// Define bitboard data type:
#define U64 unsigned long long

// Defining FEN debug positions:
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
#define repetitions "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 "

// Enumerate board squares:
enum
{
	a8,
	b8,
	c8,
	d8,
	e8,
	f8,
	g8,
	h8,
	a7,
	b7,
	c7,
	d7,
	e7,
	f7,
	g7,
	h7,
	a6,
	b6,
	c6,
	d6,
	e6,
	f6,
	g6,
	h6,
	a5,
	b5,
	c5,
	d5,
	e5,
	f5,
	g5,
	h5,
	a4,
	b4,
	c4,
	d4,
	e4,
	f4,
	g4,
	h4,
	a3,
	b3,
	c3,
	d3,
	e3,
	f3,
	g3,
	h3,
	a2,
	b2,
	c2,
	d2,
	e2,
	f2,
	g2,
	h2,
	a1,
	b1,
	c1,
	d1,
	e1,
	f1,
	g1,
	h1,
	no_sq
};

// Enumerate sides to move (colors):
enum
{
	white,
	black,
	both
};

// Enumerate rook and bishop:
enum
{
	rook,
	bishop
};

// Castle rights representation:
enum
{
	wk = 1,
	wq = 2,
	bk = 4,
	bq = 8
};

// Encode pieces:
enum
{
	P,
	N,
	B,
	R,
	Q,
	K,
	p,
	n,
	b,
	r,
	q,
	k
};

const char *square_to_coordinates[] = {
		"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
		"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
		"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
		"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
		"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
		"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
		"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
		"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};

// ASCII pieces:
char ascii_pieces[12] = "PNBRQKpnbrqk";

// Unicode pieces:
char *unicode_pieces[12] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚"};

// Convert ASCII character pieces to encoded constants:
int char_pieces[] = {
		['P'] = P,
		['N'] = N,
		['B'] = B,
		['R'] = R,
		['Q'] = Q,
		['K'] = K,
		['p'] = p,
		['n'] = n,
		['b'] = b,
		['r'] = r,
		['q'] = q,
		['k'] = k};

// Promoted pieces:
char promoted_pieces[] = {
		[Q] = 'q',
		[R] = 'r',
		[B] = 'b',
		[N] = 'n',
		[q] = 'q',
		[r] = 'r',
		[b] = 'b',
		[n] = 'n'};

/******************************************************************************\
================================ CHESS BOARD ===================================
\******************************************************************************/

/*

	= WHITE PIECES
	= Pawns             = Knights           = Bishops           = Rooks             = Queens            = King
	8 0 0 0 0 0 0 0 0   8 0 0 0 0 0 0 0 0   8 0 0 0 0 0 0 0 0   8 0 0 0 0 0 0 0 0   8 0 0 0 0 0 0 0 0   8 0 0 0 0 0 0 0 0
	7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0
	6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0
	5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0
	4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0
	3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0
	2 1 1 1 1 1 1 1 1   2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0
	1 0 0 0 0 0 0 0 0   1 0 1 0 0 0 0 1 0   1 0 0 1 0 0 1 0 0   1 1 0 0 0 0 0 0 1   1 0 0 0 1 0 0 0 0   1 0 0 0 0 1 0 0 0
		a b c d e f g h   	a b c d e f g h   	a b c d e f g h   	a b c d e f g h   	a b c d e f g h   	a b c d e f g h

	= BLACK PIECES
	= Pawns             = Knights           = Bishops           = Rooks             = Queens            = King
	8 0 0 0 0 0 0 0 0   8 0 1 0 0 0 0 1 0   8 0 0 1 0 0 1 0 0   8 1 0 0 0 0 0 0 1   8 0 0 0 1 0 0 0 0   8 0 0 0 0 1 0 0 0
	7 1 1 1 1 1 1 1 1   7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0   7 0 0 0 0 0 0 0 0
	6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0
	5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0
	4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0
	3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0
	2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0   2 0 0 0 0 0 0 0 0
	1 0 0 0 0 0 0 0 0   1 0 0 0 0 0 0 0 0   1 0 0 0 0 0 0 0 0   1 0 0 0 0 0 0 0 0   1 0 0 0 0 0 0 0 0   1 0 0 0 0 0 0 0 0
		a b c d e f g h   	a b c d e f g h   	a b c d e f g h   	a b c d e f g h   	a b c d e f g h   	a b c d e f g h

	= OCCUPANCIES
	= White Occupancy   = Black Occupancy   = All occupancies
	8 0 0 0 0 0 0 0 0   8 1 1 1 1 1 1 1 1   8 1 1 1 1 1 1 1 1
	7 0 0 0 0 0 0 0 0   7 1 1 1 1 1 1 1 1   7 1 1 1 1 1 1 1 1
	6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0   6 0 0 0 0 0 0 0 0
	5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0   5 0 0 0 0 0 0 0 0
	4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0   4 0 0 0 0 0 0 0 0
	3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0   3 0 0 0 0 0 0 0 0
	2 1 1 1 1 1 1 1 1   2 0 0 0 0 0 0 0 0   2 1 1 1 1 1 1 1 1
	1 1 1 1 1 1 1 1 1   1 0 0 0 0 0 0 0 0   1 1 1 1 1 1 1 1 1
		a b c d e f g h   	a b c d e f g h   	a b c d e f g h

	= SEMANTIC PRESENTATION
	= ASCII             = Unicode
	8 r n b q k b n r   8 ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜
	7 p p p p p p p p   7 ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎
	6 0 0 0 0 0 0 0 0   6 . . . . . . . .
	5 0 0 0 0 0 0 0 0   5 . . . . . . . .
	4 0 0 0 0 0 0 0 0   4 . . . . . . . .
	3 0 0 0 0 0 0 0 0   3 . . . . . . . .
	2 P P P P P P P P   2 ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙
	1 R N B Q K B N R   1 ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖
		a b c d e f g h   	a b c d e f g h

*/

// Defining the bitboards:
U64 bitboards[12];
U64 occupancies[3];

// Side to move:
int side;

// Enpassant square:
int enpassant = no_sq;

/*

	= CASTLING BITS

	 bin   dec  description
	0001     1  white king can castle to the king side
	0010     2  white king can castle to the queen side
	0100     4  black king can castle to the king side
	1000     8  black king can castle to the queen side

	examples
	1111        both sides can castle to both directions
	1001        black king => queen side, white king => king side

*/

// Castling rights:
int castle;

// "Almost" unique position identifier (aka hash key or position key):
U64 hash_key;

// Positions repetition table:
U64 repetition_table[1000];

// Repetition index:
int repetition_index;

// Half move counter:
int ply;

/******************************************************************************\
=========================== TIME CONTROL VARIABLES =============================
\******************************************************************************/

// Exit from engine flag:
int quit = 0;

// UCI <movestogo> command moves counter:
int movestogo = 30;

// UCI <movetime> command time counter:
int movetime = -1;

// UCI <time> command holder (ms):
int time = -1;

// UCI <inc> commands time increment holder:
int inc = 0;

// UCI <starttime> command time holder:
int starttime = 0;

// UCI <stoptime> command time holder:
int stoptime = 0;

// Variable to flag time control availability:
int timeset = 0;

// Variable to flag when the time is up:
int stopped = 0;

/******************************************************************************\
=========================== MISCELLANEOUS FUNCTIONS ============================
\******************************************************************************/

// Get time in miliseconds:
int get_time_ms()
{
#ifdef WIN64
	return GetTickCount();
#else
	struct timeval time_value;
	gettimeofday(&time_value, NULL);
	return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
#endif
}

/*

	Function to "listen" to GUI's input during search.
	It's waiting for the user input from STDIN.
	OS dependent.

	First Richard Allbert aka BluefeverSoftware grabbed it from somewhere...
	And then Code Monkey King has grabbed it from VICE)

*/

int input_waiting()
{
#ifndef WIN32
	fd_set readfds;
	struct timeval tv;
	FD_ZERO(&readfds);
	FD_SET(fileno(stdin), &readfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	select(16, &readfds, 0, 0, &tv);

	return (FD_ISSET(fileno(stdin), &readfds));
#else
	static int init = 0, pipe;
	static HANDLE inh;
	DWORD dw;

	if (!init)
	{
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe)
		{
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}

	if (pipe)
	{
		if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
			return 1;
		return dw;
	}

	else
	{
		GetNumberOfConsoleInputEvents(inh, &dw);
		return dw <= 1 ? 0 : dw;
	}

#endif
}

// Read user/GUI input:
void read_input()
{
	// Bytes to read holder:
	int bytes;
	// User/GUI input:
	char input[256] = "", *endc;
	// "Listen" to STDIN:
	if (input_waiting())
	{
		// Tell engine to stop calculating:
		stopped = 1;
		// Loop to read bytes from STDIN:
		do
		{
			// Read bytes from STDIN:
			bytes = read(fileno(stdin), input, 256);
		}
		// Until bytes available:
		while (bytes < 0);
		// Searches for the first occurrence of '\n':
		endc = strchr(input, '\n');
		// If found new line:
		if (endc)
		{
			// Set value at pointer to 0:
			*endc = 0;
		}
		// If input is available:
		if (strlen(input) > 0)
		{
			// Match UCI <quit> command:
			if (!strncmp(input, "quit", 4))
			{
				// Tell engine to terminate exacution:
				quit = 1;
			}
			// Match UCI <stop> command:
			else if (!strncmp(input, "stop", 4))
			{
				// Tell engine to terminate exacution:
				quit = 1;
			}
		}
	}
}

// A bridge function to interact between search and GUI input:
static void communicate()
{
	// If time is up break here:
	if (timeset == 1 && get_time_ms() > stoptime)
	{
		// Tell engine to stop calculating:
		stopped = 1;
	}
	// Read GUI input:
	read_input();
}

/******************************************************************************\
=============================== RANDOM NUMBERS =================================
\******************************************************************************/

// Pseudo random number state:
unsigned int random_state = 1804289383;

// Generate 32-bit pseudo legal numbers:
unsigned int get_random_U32_number()
{
	// Get current state:
	unsigned int number = random_state;
	// XOR shift algorithm:
	number ^= number << 13;
	number ^= number >> 17;
	number ^= number << 5;
	// Update the random number state:
	random_state = number;
	// Return the random number:
	return number;
}

// Generate 64-bit pseudo legal numbers:
U64 get_random_U64_number()
{
	// Define 4 random numbers:
	U64 n1, n2, n3, n4;
	// Initialize random numbers slicing 16 bits from MS1B side:
	n1 = (U64)(get_random_U32_number()) & 0xFFFF;
	n2 = (U64)(get_random_U32_number()) & 0xFFFF;
	n3 = (U64)(get_random_U32_number()) & 0xFFFF;
	n4 = (U64)(get_random_U32_number()) & 0xFFFF;
	// Return the random number:
	return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

// Generate magic number candidate:
U64 generate_magic_number()
{
	return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}

/******************************************************************************\
============================== BIT MANIPULATION ================================
\******************************************************************************/

// Bit set/get/pop macros:
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

// Count bits wihin a bitboard:
static inline int count_bits(U64 bitboard)
{
	// Bit counter:
	int count = 0;
	// Concecutively reset least significant first bit:
	while (bitboard)
	{
		// Increment count:
		count++;
		// Reset least significant first bit:
		bitboard &= bitboard - 1;
	}
	// Return bit count:
	return count;
}

// Get least significant first bit index:
static inline int get_ls1b_index(U64 bitboard)
{
	// Be sure that the bitboard is not ZERO:
	if (bitboard)
	{
		// Count trailing bits before LS1B:
		return count_bits((bitboard & -bitboard) - 1);
	}
	// If the bitboard is ZERO:
	else
	{
		return -1;
	}
}

/******************************************************************************\
================================ ZOBRIST KEYS ==================================
\******************************************************************************/

// Random piece keys [piece][square]:
U64 piece_keys[12][64];

// Random enpassant keys [square]:
U64 enpassant_keys[64];

// Random castling keys:
U64 castle_keys[16];

// Random side key:
U64 side_key;

// Initializate random hash keys:
void init_random_keys()
{
	// Update pseudo random number state:
	random_state = 1804289383;
	// Loop over piece codes:
	for (int piece = P; piece <= k; piece++)
	{
		// Loop over board squares:
		for (int square = 0; square < 64; square++)
		{
			// Initialiazte random piece keys:
			piece_keys[piece][square] = get_random_U64_number();
		}
	}
	// Loop over board squares:
	for (int square = 0; square < 64; square++)
	{
		// Initializate random enpassant keys:
		enpassant_keys[square] = get_random_U64_number();
	}
	// Loop over castling keys:
	for (int index = 0; index < 16; index++)
	{
		// Initializate castling keys:
		castle_keys[index] = get_random_U64_number();
	}
	// Initializate random side key:
	side_key = get_random_U64_number();
}

// Generate "almost" unique position identifier (aka hash key) from scratch:
U64 generate_hash_key()
{
	// Final hash key:
	U64 final_key = 0ULL;
	// Temporary piece bitboard copy:
	U64 bitboard;
	// Loop over pieces bitboards:
	for (int piece = P; piece <= k; piece++)
	{
		// Initialize piece bitboard copy:
		bitboard = bitboards[piece];
		// Loop over the piece within a given bitboard:
		while (bitboard)
		{
			// Initialize square occupied by the piece:
			int square = get_ls1b_index(bitboard);
			// Hash piece:
			final_key ^= piece_keys[piece][square];
			// Pop the LS1B:
			pop_bit(bitboard, square);
		}
	}
	// If enpassant square is available:
	if (enpassant != no_sq)
	{
		// Hash enpassant:
		final_key ^= enpassant_keys[enpassant];
	}
	// Hash the castling rights:
	final_key ^= castle_keys[castle];
	// Hash the side only if its black turn:
	if (side == black)
	{
		final_key ^= side_key;
	}
	// Return the generated hash key:
	return final_key;
}

/******************************************************************************\
============================== INPUT AND OUTPUT ================================
\******************************************************************************/

// Printing bitboard:
void print_bitboard(U64 bitboard)
{
	// Print horizontal top border:
	printf("\n");
	printf("\033[0;30m+---------------------------------------+\n");
	printf("|        \e[0mBITBOARD REPRESENTATION\033[0;30m        |\n");
	// Loop over board ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Print horizontal separator:
		printf("|   +---+---+---+---+---+---+---+---+   |\n");
		// Loop over board files:
		for (int file = 0; file < 8; file++)
		{
			// Convert the file and rank into square index:
			int square = rank * 8 + file;
			// Print rank label:
			if (!file)
				printf("| \e[0m%1d\033[0;30m |", 8 - rank);
			// Print bit state (either 1 or 0):
			get_bit(bitboard, square) ? printf(" \033[0;33m%d\033[0;30m |", 1) : printf(" %d\033[0;30m |", 0);
			if (file == 7)
				printf("   |");
		}
		// Print a new line after every rank:
		printf("\n");
	}
	// Print horizontal bottom border:
	printf("|   +---+---+---+---+---+---+---+---+   |\n");
	printf("|     \e[0ma   b   c   d   e   f   g   h\033[0;30m     |\n");
	printf("+---------------------------------------+\n\n");
	// Print the bitboard as unsigned decimal number:
	printf("\033[0;30m→ \e[0mBitboard state: \033[0;30m%llu\e[0m\n\n", bitboard);
}

// Print board:
void print_board()
{
	// Print horizontal top border:
	printf("\n");
	printf("\033[0;30m+---------------------------------------+\n");
	if (side == white)
	{
		printf("|            \033[0;33m=> WHITE TURN <=\033[0;30m           |\n");
	}
	else if (side == black)
	{
		printf("|            \033[0;34m=> BLACK TURN <=\033[0;30m           |\n");
	}
	else
	{
		printf("|                                       |\n");
	}
	// Print top horizontal separador:
	printf("|   +-");
	(castle & bq) ? printf("\033[0;32m%c\033[0;30m", '@') : printf("%c", '-');
	printf("-+---+---+---+---+---+---+-");
	(castle & bk) ? printf("\033[0;32m%c\033[0;30m", '@') : printf("%c", '-');
	printf("-+   |\n");
	// Loop over board ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Print horizontal separator:
		if (rank > 0)
		{
			printf("|   ");
			for (int file = 0; file < 8; file++)
			{
				if (enpassant != no_sq && (enpassant == ((rank * 8) + file) || enpassant == (((rank - 1) * 8) + file)))
				{
					printf("\033[0;31m+---");
				}
				else if (enpassant != no_sq && ((enpassant + 1 == ((rank * 8) + file) && ((enpassant + 1) % 8 > 0)) || (enpassant + 1 == (((rank - 1) * 8) + file) && ((enpassant + 1) % 8 > 0))))
				{
					printf("\033[0;31m+\033[0;30m---");
				}
				else
				{
					printf("\033[0;30m+---");
				}
			}
			printf("+\033[0;30m   |\n");
		}
		// Loop over board files:
		for (int file = 0; file < 8; file++)
		{
			// Convert the file and rank into square index:
			int square = rank * 8 + file;
			// Set the print square color character:
			char square_color = (square + rank) % 2 == 0 ? ' ' : ':';
			char square_empty = (square + rank) % 2 == 0 ? ' ' : ':';
			int piece_color = white;
			// Defining the piece to print:
			int piece = -1;
			// Loop over all 12 pieces bitboards:
			for (int bb_piece = P; bb_piece <= k; bb_piece++)
			{
				if (get_bit(bitboards[bb_piece], square))
				{
					piece = bb_piece;
					if (bb_piece >= p)
					{
						piece_color = black;
					}
				}
			}
			// Print rank label:
			if (!file)
			{
				if (enpassant % 8 == 0 && square == enpassant)
				{
					printf("| \e[0m%1d\033[0;31m |\033[0;30m", 8 - rank);
				}
				else
				{
					printf("| \e[0m%1d\033[0;30m |", 8 - rank);
				}
			}
// Print the piece depending on the OS:
#ifdef WIN64
			if (piece_color == white)
			{
				piece != -1 ? printf("%c\033[0;33m%c\033[0;30m%c", square_color, ascii_pieces[piece], square_color) : printf("%c%c%c\033[0;30m", square_color, square_empty, square_color);
			}
			else
			{
				piece != -1 ? printf("%c\033[0;34m%c\033[0;30m%c", square_color, ascii_pieces[piece], square_color) : printf("%c%c%c\033[0;30m", square_color, square_empty, square_color);
			}
#else
			if (piece_color == white)
			{
				piece != -1 ? printf("%c\033[0;33m%s\033[0;30m%c", square_color, unicode_pieces[piece], square_color) : printf("%c%c%c\033[0;30m", square_color, square_empty, square_color);
			}
			else
			{
				piece != -1 ? printf("%c\033[0;34m%s\033[0;30m%c", square_color, unicode_pieces[piece], square_color) : printf("%c%c%c\033[0;30m", square_color, square_empty, square_color);
			}
#endif
			if (enpassant != no_sq && (enpassant == (square) || (enpassant == (square + 1) && (square + 1) % 8 != 0)))
			{
				printf("\033[0;31m|\033[0;30m");
			}
			else
			{
				printf("|");
			}
			if (file == 7)
				printf("   |");
		}
		// Print a new line after every rank:
		printf("\n");
	}
	// Print horizontal bottom border:
	printf("|   +-");
	(castle & wq) ? printf("\033[0;32m%c\033[0;30m", '@') : printf("%c", '-');
	printf("-+---+---+---+---+---+---+-");
	(castle & wk) ? printf("\033[0;32m%c\033[0;30m", '@') : printf("%c", '-');
	printf("-+   |\n");
	printf("|     \e[0ma   b   c   d   e   f   g   h\033[0;30m     |\n");
	printf("+---------------------------------------+\n");
	// Print the hash key:
	printf("| \033[0;33mHash key: \033[0;32m%27llx \033[0;30m|\n", hash_key);
	printf("+---------------------------------------+\e[0m\n\n");
}

// Parse FEN string:
void parse_fen(char *fen)
{
	// Reset board positions:
	memset(bitboards, 0ULL, sizeof(bitboards));
	memset(occupancies, 0ULL, sizeof(occupancies));
	// Reset board states:
	side = 0;
	enpassant = no_sq;
	castle = 0;
	// Reset repetition index:
	repetition_index = 0;
	// Reset the repetition table:
	memset(repetition_table, 0ULL, sizeof(repetition_table));
	// Loop over board ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Loop over board files:
		for (int file = 0; file < 8; file++)
		{
			// Initialize current square:
			int square = rank * 8 + file;
			// Match ascii pieces within FEN string:
			if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z'))
			{
				// Initialize piece type:
				int piece = char_pieces[*fen];
				// Set piece on corresponding bitboard:
				set_bit(bitboards[piece], square);
				// Increment pointer to FEN string:
				fen++;
			}
			// Match empty squares numbers within FEN string:
			if (*fen >= '0' && *fen <= '9')
			{
				// Initialize offset (convert char '0' to int 0):
				int offset = *fen - '0';

				// Define piece variable:
				int piece = -1;
				// Loop over all pieces bitboards:
				for (int bb_piece = P; bb_piece <= k; bb_piece++)
				{
					// If there is a piece on current square:
					if (get_bit(bitboards[bb_piece], square))
					{
						// Get piece code:
						piece = bb_piece;
					}
				}
				// On empty current square:
				if (piece == -1)
				{
					// Decrement file:
					file--;
				}
				// Adjust file counter:
				file += offset;
				// Increment pointer to FEN string:
				fen++;
			}
			// Match rank separator:
			if (*fen == '/')
			{
				// Increment pointer to FEN string:
				fen++;
			}
		}
	}
	// Increment pointer to FEN string to go direct to side to move:
	fen++;
	// Parse side to move:
	(*fen == 'w') ? (side = white) : (side = black);
	// Increment pointer to FEN string to go direct to castling rights:
	fen += 2;
	// Parse castling rights:
	while (*fen != ' ')
	{
		// Grant castling rights depending on character:
		switch (*fen)
		{
		case 'K':
			castle |= wk;
			break;
		case 'Q':
			castle |= wq;
			break;
		case 'k':
			castle |= bk;
			break;
		case 'q':
			castle |= bq;
			break;
		case '-':
			break;
		}
		// Increment pointer to FEN string:
		fen++;
	}
	// Increment pointer to FEN string to go direct to enpassant square:
	fen++;
	// Parse enpassant square:
	if (*fen != '-')
	{
		// Parse enpassant file and rank:
		int file = fen[0] - 'a';
		int rank = 8 - (fen[1] - '0');
		// Initialize enpassant square:
		enpassant = rank * 8 + file;
	}
	// No enpassant square:
	else
	{
		enpassant = no_sq;
	}
	// Initialize white occupancies:
	for (int piece = P; piece <= K; piece++)
	{
		// Populate white occupancy bitboard:
		occupancies[white] |= bitboards[piece];
	}
	// Initialize black occupancies:
	for (int piece = p; piece <= k; piece++)
	{
		// Populate black occupancy bitboard:
		occupancies[black] |= bitboards[piece];
	}
	// Populate both occupancy bitboard:
	occupancies[both] |= occupancies[white];
	occupancies[both] |= occupancies[black];
	// Initialize hash key:
	hash_key = generate_hash_key();
}

/******************************************************************************\
=================================== ATTACKS ====================================
\******************************************************************************/

/*

	=	NOT A FILE:				=	NOT H FILE:				=	NOT AB FILE:			=	NOT HG FILE:

	8 0 1 1 1 1 1 1 1		8 1 1 1 1 1 1 1 0		8 0 0 1 1 1 1 1 1		8 1 1 1 1 1 1 0 0
	7 0 1 1 1 1 1 1 1		7 1 1 1 1 1 1 1 0		7 0 0 1 1 1 1 1 1		7 1 1 1 1 1 1 0 0
	6 0 1 1 1 1 1 1 1		6 1 1 1 1 1 1 1 0		6 0 0 1 1 1 1 1 1		6 1 1 1 1 1 1 0 0
	5 0 1 1 1 1 1 1 1		5 1 1 1 1 1 1 1 0		5 0 0 1 1 1 1 1 1		5 1 1 1 1 1 1 0 0
	4 0 1 1 1 1 1 1 1		4 1 1 1 1 1 1 1 0		4 0 0 1 1 1 1 1 1		4 1 1 1 1 1 1 0 0
	3 0 1 1 1 1 1 1 1		3 1 1 1 1 1 1 1 0		3 0 0 1 1 1 1 1 1		3 1 1 1 1 1 1 0 0
	2 0 1 1 1 1 1 1 1		2 1 1 1 1 1 1 1 0		2 0 0 1 1 1 1 1 1		2 1 1 1 1 1 1 0 0
	1 0 1 1 1 1 1 1 1		1 1 1 1 1 1 1 1 0		1 0 0 1 1 1 1 1 1		1 1 1 1 1 1 1 0 0
		a b c d e f g h			a b c d e f g h			a b c d e f g h			a b c d e f g h

*/

// Not specific files constants:
const U64 not_a_file = 18374403900871474942ULL;
const U64 not_h_file = 9187201950435737471ULL;
const U64 not_ab_file = 18229723555195321596ULL;
const U64 not_hg_file = 4557430888798830399ULL;

// Relevant occupancy bit count for every square on board:
const int bishop_relevant_bits[64] = {
		6, 5, 5, 5, 5, 5, 5, 6,
		5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 7, 7, 7, 7, 5, 5,
		5, 5, 7, 9, 9, 7, 5, 5,
		5, 5, 7, 9, 9, 7, 5, 5,
		5, 5, 7, 7, 7, 7, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5,
		6, 5, 5, 5, 5, 5, 5, 6};
const int rook_relevant_bits[64] = {
		12, 11, 11, 11, 11, 11, 11, 12,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		12, 11, 11, 11, 11, 11, 11, 12};

// Magic numbers for rook and bishop:
U64 rook_magic_numbers[64] = {
		0x8a80104000800020ULL,
		0x140002000100040ULL,
		0x2801880a0017001ULL,
		0x100081001000420ULL,
		0x200020010080420ULL,
		0x3001c0002010008ULL,
		0x8480008002000100ULL,
		0x2080088004402900ULL,
		0x800098204000ULL,
		0x2024401000200040ULL,
		0x100802000801000ULL,
		0x120800800801000ULL,
		0x208808088000400ULL,
		0x2802200800400ULL,
		0x2200800100020080ULL,
		0x801000060821100ULL,
		0x80044006422000ULL,
		0x100808020004000ULL,
		0x12108a0010204200ULL,
		0x140848010000802ULL,
		0x481828014002800ULL,
		0x8094004002004100ULL,
		0x4010040010010802ULL,
		0x20008806104ULL,
		0x100400080208000ULL,
		0x2040002120081000ULL,
		0x21200680100081ULL,
		0x20100080080080ULL,
		0x2000a00200410ULL,
		0x20080800400ULL,
		0x80088400100102ULL,
		0x80004600042881ULL,
		0x4040008040800020ULL,
		0x440003000200801ULL,
		0x4200011004500ULL,
		0x188020010100100ULL,
		0x14800401802800ULL,
		0x2080040080800200ULL,
		0x124080204001001ULL,
		0x200046502000484ULL,
		0x480400080088020ULL,
		0x1000422010034000ULL,
		0x30200100110040ULL,
		0x100021010009ULL,
		0x2002080100110004ULL,
		0x202008004008002ULL,
		0x20020004010100ULL,
		0x2048440040820001ULL,
		0x101002200408200ULL,
		0x40802000401080ULL,
		0x4008142004410100ULL,
		0x2060820c0120200ULL,
		0x1001004080100ULL,
		0x20c020080040080ULL,
		0x2935610830022400ULL,
		0x44440041009200ULL,
		0x280001040802101ULL,
		0x2100190040002085ULL,
		0x80c0084100102001ULL,
		0x4024081001000421ULL,
		0x20030a0244872ULL,
		0x12001008414402ULL,
		0x2006104900a0804ULL,
		0x1004081002402ULL};
U64 bishop_magic_numbers[64] = {
		0x40040844404084ULL,
		0x2004208a004208ULL,
		0x10190041080202ULL,
		0x108060845042010ULL,
		0x581104180800210ULL,
		0x2112080446200010ULL,
		0x1080820820060210ULL,
		0x3c0808410220200ULL,
		0x4050404440404ULL,
		0x21001420088ULL,
		0x24d0080801082102ULL,
		0x1020a0a020400ULL,
		0x40308200402ULL,
		0x4011002100800ULL,
		0x401484104104005ULL,
		0x801010402020200ULL,
		0x400210c3880100ULL,
		0x404022024108200ULL,
		0x810018200204102ULL,
		0x4002801a02003ULL,
		0x85040820080400ULL,
		0x810102c808880400ULL,
		0xe900410884800ULL,
		0x8002020480840102ULL,
		0x220200865090201ULL,
		0x2010100a02021202ULL,
		0x152048408022401ULL,
		0x20080002081110ULL,
		0x4001001021004000ULL,
		0x800040400a011002ULL,
		0xe4004081011002ULL,
		0x1c004001012080ULL,
		0x8004200962a00220ULL,
		0x8422100208500202ULL,
		0x2000402200300c08ULL,
		0x8646020080080080ULL,
		0x80020a0200100808ULL,
		0x2010004880111000ULL,
		0x623000a080011400ULL,
		0x42008c0340209202ULL,
		0x209188240001000ULL,
		0x400408a884001800ULL,
		0x110400a6080400ULL,
		0x1840060a44020800ULL,
		0x90080104000041ULL,
		0x201011000808101ULL,
		0x1a2208080504f080ULL,
		0x8012020600211212ULL,
		0x500861011240000ULL,
		0x180806108200800ULL,
		0x4000020e01040044ULL,
		0x300000261044000aULL,
		0x802241102020002ULL,
		0x20906061210001ULL,
		0x5a84841004010310ULL,
		0x4010801011c04ULL,
		0xa010109502200ULL,
		0x4a02012000ULL,
		0x500201010098b028ULL,
		0x8040002811040900ULL,
		0x28000010020204ULL,
		0x6000020202d0240ULL,
		0x8918844842082200ULL,
		0x4010011029020020ULL};

// Pawn attacks table [side][square]:
U64 pawn_attacks[2][64];

// Knight attacks table [square]:
U64 knight_attacks[64];

// King attacks table [square]:
U64 king_attacks[64];

// Bishop attacks masks:
U64 bishop_masks[64];

// Bishop attacks table [square][occupancies]
U64 bishop_attacks[64][512];

// Rook attacks masks:
U64 rook_masks[64];

// Rook attacks table [square][occupancies]
U64 rook_attacks[64][4096];

// Generate pawns attacks:
U64 mask_pawn_attacks(int side, int square)
{
	// Result attacks bitboard:
	U64 attacks = 0ULL;
	// Piece bitboard:
	U64 bitboard = 0ULL;
	// Set piece on board:
	set_bit(bitboard, square);
	// White pawns:
	if (!side)
	{
		// Generate pawn attacks:
		if ((bitboard >> 7) & not_a_file)
			attacks |= (bitboard >> 7);
		if ((bitboard >> 9) & not_h_file)
			attacks |= (bitboard >> 9);
	}
	// Black pawns:
	else
	{
		// Generate pawn attacks:
		if ((bitboard << 7) & not_h_file)
			attacks |= (bitboard << 7);
		if ((bitboard << 9) & not_a_file)
			attacks |= (bitboard << 9);
	}
	// Return attacks:
	return attacks;
}

// Generate knight attacks:
U64 mask_knight_attacks(int square)
{
	// Result attacks bitboard:
	U64 attacks = 0ULL;
	// Piece bitboard:
	U64 bitboard = 0ULL;
	// Set piece on board:
	set_bit(bitboard, square);
	// Generate knight attacks:
	if ((bitboard >> 17) & not_h_file)
		attacks |= (bitboard >> 17);
	if ((bitboard >> 15) & not_a_file)
		attacks |= (bitboard >> 15);
	if ((bitboard >> 10) & not_hg_file)
		attacks |= (bitboard >> 10);
	if ((bitboard >> 6) & not_ab_file)
		attacks |= (bitboard >> 6);
	if ((bitboard << 17) & not_a_file)
		attacks |= (bitboard << 17);
	if ((bitboard << 15) & not_h_file)
		attacks |= (bitboard << 15);
	if ((bitboard << 10) & not_ab_file)
		attacks |= (bitboard << 10);
	if ((bitboard << 6) & not_hg_file)
		attacks |= (bitboard << 6);
	// Return attacks:
	return attacks;
}

// Generate king attacks:
U64 mask_king_attacks(int square)
{
	// Result attacks bitboard:
	U64 attacks = 0ULL;
	// Piece bitboard:
	U64 bitboard = 0ULL;
	// Set piece on board:
	set_bit(bitboard, square);
	// Generate king attacks:
	if ((bitboard >> 1) & not_h_file)
		attacks |= (bitboard >> 1);
	if ((bitboard >> 7) & not_a_file)
		attacks |= (bitboard >> 7);
	if (bitboard >> 8)
		attacks |= (bitboard >> 8);
	if ((bitboard >> 9) & not_h_file)
		attacks |= (bitboard >> 9);
	if ((bitboard << 1) & not_a_file)
		attacks |= (bitboard << 1);
	if ((bitboard << 7) & not_h_file)
		attacks |= (bitboard << 7);
	if (bitboard << 8)
		attacks |= (bitboard << 8);
	if ((bitboard << 9) & not_a_file)
		attacks |= (bitboard << 9);
	// Return attacks:
	return attacks;
}

// Mask bishop attacks:
U64 mask_bishop_attacks(int square)
{
	// Result attacks bitboard:
	U64 attacks = 0ULL;
	// Initialize ranks and files:
	int r, f;
	// Initialize target ranks and files:
	int tr = square / 8;
	int tf = square % 8;
	// Mask relevant bishop occupancy bits:
	for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
		attacks |= (1ULL << (r * 8 + f));
	for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
		attacks |= (1ULL << (r * 8 + f));
	for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
		attacks |= (1ULL << (r * 8 + f));
	for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
		attacks |= (1ULL << (r * 8 + f));
	// Return attacks:
	return attacks;
}

// Mask rook attacks:
U64 mask_rook_attacks(int square)
{
	// Result attacks bitboard:
	U64 attacks = 0ULL;
	// Initialize ranks and files:
	int r, f;
	// Initialize target ranks and files:
	int tr = square / 8;
	int tf = square % 8;
	// Mask relevant rook occupancy bits:
	for (r = tr + 1; r <= 6; r++)
		attacks |= (1ULL << (r * 8 + tf));
	for (r = tr - 1; r >= 1; r--)
		attacks |= (1ULL << (r * 8 + tf));
	for (f = tf + 1; f <= 6; f++)
		attacks |= (1ULL << (tr * 8 + f));
	for (f = tf - 1; f >= 1; f--)
		attacks |= (1ULL << (tr * 8 + f));
	// Return attacks:
	return attacks;
}

// Generate bishop attacks on the fly:
U64 bishop_attacks_on_the_fly(int square, U64 block)
{
	// Result attacks bitboard:
	U64 attacks = 0ULL;
	// Initialize ranks and files:
	int r, f;
	// Initialize target ranks and files:
	int tr = square / 8;
	int tf = square % 8;
	// Generate bishop attacks:
	for (r = tr + 1, f = tf + 1; r < 8 && f < 8; r++, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & block)
			break;
	}
	for (r = tr - 1, f = tf + 1; r >= 0 && f < 8; r--, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & block)
			break;
	}
	for (r = tr + 1, f = tf - 1; r < 8 && f >= 0; r++, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & block)
			break;
	}
	for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & block)
			break;
	}
	// Return attacks:
	return attacks;
}

// Generate rook attacks on the fly:
U64 rook_attacks_on_the_fly(int square, U64 block)
{
	// Result attacks bitboard:
	U64 attacks = 0ULL;
	// Initialize ranks and files:
	int r, f;
	// Initialize target ranks and files:
	int tr = square / 8;
	int tf = square % 8;
	// Generate rook attacks:
	for (r = tr + 1; r < 8; r++)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if ((1ULL << (r * 8 + tf)) & block)
			break;
	}
	for (r = tr - 1; r >= 0; r--)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if ((1ULL << (r * 8 + tf)) & block)
			break;
	}
	for (f = tf + 1; f < 8; f++)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if ((1ULL << (tr * 8 + f)) & block)
			break;
	}
	for (f = tf - 1; f >= 0; f--)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if ((1ULL << (tr * 8 + f)) & block)
			break;
	}
	// Return attacks:
	return attacks;
}

// Initialize leaper pieces attacks:
void init_leapers_attacks()
{
	// Loop over 64 board squares:
	for (int square = 0; square < 64; square++)
	{
		// Initialize pawns attacks:
		pawn_attacks[white][square] = mask_pawn_attacks(white, square);
		pawn_attacks[black][square] = mask_pawn_attacks(black, square);
		// Initialize knights attacks:
		knight_attacks[square] = mask_knight_attacks(square);
		// Initialize king attacks:
		king_attacks[square] = mask_king_attacks(square);
	}
}

// Set occupancies:
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask)
{
	// Occupancy map:
	U64 occupancy = 0ULL;
	// Loop over the range of bits within attack mask:
	for (int count = 0; count < bits_in_mask; count++)
	{
		// Get LS1B index of attack mask:
		int square = get_ls1b_index(attack_mask);
		// Pop LS1B in attack mask:
		pop_bit(attack_mask, square);
		// Make sure that ocuupancy is on board:
		if (index & (1 << count))
			// Populare occupancy map:
			occupancy |= (1ULL << square);
	}
	// Return occupancy map:
	return occupancy;
}

/******************************************************************************\
=================================== MAGICS =====================================
\******************************************************************************/

// Find appropriate magic number:
U64 find_magic_number(int square, int relevant_bits, int bishop)
{
	// Initializate occupancies:
	U64 occupancies[4096];
	// Initialize attack tables:
	U64 attacks[4096];
	// Initialize used attacks:
	U64 used_attacks[4096];
	// Initialize attack mask for a current piece:
	U64 attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);
	// Initialize occupancy indicies:
	int occupancy_indicies = 1 << relevant_bits;
	// Loop over occupancy indicies:
	for (int index = 0; index < occupancy_indicies; index++)
	{
		// Initialize occupancies:
		occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);
		// Initialize attacks:
		attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupancies[index])
														: rook_attacks_on_the_fly(square, occupancies[index]);
	}
	// Test magic numbers loop:
	for (int random_count = 0; random_count < 100000000; random_count++)
	{
		// Generate magic number candidate:
		U64 magic_number = generate_magic_number();
		// Skip invalid magic numbers:
		if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6)
			continue;
		// Initialize used attacks:
		memset(used_attacks, 0ULL, sizeof(used_attacks));
		// Initialize index and fail flag:
		int index, fail;
		// Test magic index loop:
		for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
		{
			// Initialize magic index:
			int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));
			// If magic index works:
			if (used_attacks[magic_index] == 0ULL)
				// Initialize used attacks:
				used_attacks[magic_index] = attacks[index];
			// Otherwise:
			else if (used_attacks[magic_index] != attacks[index])
				// Magic index does not works:
				fail = 1;
		}
		// If magic number works:
		if (!fail)
			// Return the magic number:
			return magic_number;
	}
	// If magic number does not work:
	printf("Magic number fails!");
	return 0ULL;
}

// Initialize magic numbers:
void init_magic_numbers()
{
	// Loop over 64 board squares:
	for (int square = 0; square < 64; square++)
	{
		// Initialize rook magic numbers:
		rook_magic_numbers[square] = find_magic_number(square, rook_relevant_bits[square], rook);
	}
	// Loop over 64 board squares:
	for (int square = 0; square < 64; square++)
	{
		// Initialize bishop magic numbers:
		bishop_magic_numbers[square] = find_magic_number(square, bishop_relevant_bits[square], bishop);
	}
}

// Printing the magic numbers:
void print_magic_numbers()
{
	// Printing the rook magic numbers:
	for (int square = 0; square < 64; square++)
		printf("0x%llxULL,\n", rook_magic_numbers[square]);
	// Printing a space between each one:
	printf("\n");
	// Printing the bishop magic numbers:
	for (int square = 0; square < 64; square++)
		printf("0x%llxULL,\n", bishop_magic_numbers[square]);
}

// Initialize slider pieces attack tables:
void init_sliders_attacks(int bishop)
{
	// Loop over 64 board squares:
	for (int square = 0; square < 64; square++)
	{
		// Init bishop and rook masks:
		bishop_masks[square] = mask_bishop_attacks(square);
		rook_masks[square] = mask_rook_attacks(square);
		// Initialize current mask:
		U64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];
		// Initialize relevant occupancy bit count:
		int relevant_bits_count = count_bits(attack_mask);
		// Initialize occupancy indicies:
		int ocuupancy_indicies = (1 << relevant_bits_count);
		// Loop over occupancy indicies:
		for (int index = 0; index < ocuupancy_indicies; index++)
		{
			// Bishop:
			if (bishop)
			{
				// Initialize current occupancy variation:
				U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
				// Initialize magic index:
				int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);
				// Initialize bishop attacks:
				bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
			}
			// Rook
			else
			{
				// Initialize current occupancy variation:
				U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
				// Initialize magic index:
				int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);
				// Initialize rook attacks:
				rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
			}
		}
	}
}

// Get bishop attacks:
static inline U64 get_bishop_attacks(int square, U64 occupancy)
{
	// Get bishop attacks assuming current board occupancy:
	occupancy &= bishop_masks[square];
	occupancy *= bishop_magic_numbers[square];
	occupancy >>= 64 - bishop_relevant_bits[square];
	// Return bishop attacks:
	return bishop_attacks[square][occupancy];
}

// Get rook attacks:
static inline U64 get_rook_attacks(int square, U64 occupancy)
{
	// Get rook attacks assuming current board occupancy:
	occupancy &= rook_masks[square];
	occupancy *= rook_magic_numbers[square];
	occupancy >>= 64 - rook_relevant_bits[square];
	// Return rook attacks:
	return rook_attacks[square][occupancy];
}

// Get queen attacks:
static inline U64 get_queen_attacks(int square, U64 occupancy)
{
	// Initialize result attacks bitboard:
	U64 queen_attacks = 0ULL;
	// Initialize bishop occupancies:
	U64 bishop_occupancy = occupancy;
	// Initialize rook occupancies:
	U64 rook_occupancy = occupancy;
	// Get bishop attacks assuming current board occupancy:
	bishop_occupancy &= bishop_masks[square];
	bishop_occupancy *= bishop_magic_numbers[square];
	bishop_occupancy >>= 64 - bishop_relevant_bits[square];
	// Get bishop attacks:
	queen_attacks = bishop_attacks[square][bishop_occupancy];
	// Get rook attacks assuming current board occupancy:
	rook_occupancy &= rook_masks[square];
	rook_occupancy *= rook_magic_numbers[square];
	rook_occupancy >>= 64 - rook_relevant_bits[square];
	// Get rook attacks:
	queen_attacks |= rook_attacks[square][rook_occupancy];
	// Return queen attacks:
	return queen_attacks;
}

/******************************************************************************\
=============================== MOVE GENERATOR =================================
\******************************************************************************/

// Is a current given square being attacked by a current given side:
static inline int is_square_attacked(int square, int side)
{
	// Attacked by white pawns:
	if ((side == white) && (pawn_attacks[black][square] & bitboards[P]))
		return 1;
	// Attacked by black pawns:
	if ((side == black) && (pawn_attacks[white][square] & bitboards[p]))
		return 1;
	// Attacked by knights:
	if (knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n]))
		return 1;
	// Attacked by bishops:
	if (get_bishop_attacks(square, occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b]))
		return 1;
	// Attacked by rooks:
	if (get_rook_attacks(square, occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r]))
		return 1;
	// Attacked by queens:
	if (get_queen_attacks(square, occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q]))
		return 1;
	// Attacked by kings:
	if (king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k]))
		return 1;
	// By default return false:
	return 0;
}

// Print attacked squares:
void print_attacked_squares(int side)
{
	// Print horizontal top border:
	printf("\n");
	printf("\033[0;30m+---------------------------------------+\n");
	if (side)
	{
		printf("|       SQUARES ATTACKED BY \033[0;34mBLACK\033[0;30m       |\n");
	}
	else
	{
		printf("|       SQUARES ATTACKED BY \033[0;33mWHITE\033[0;30m       |\n");
	}
	// Loop over board ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Print horizontal separator:
		printf("|   +---+---+---+---+---+---+---+---+   |\n");
		// Loop over board files:
		for (int file = 0; file < 8; file++)
		{
			// Convert the file and rank into square index:
			int square = rank * 8 + file;
			// Print rank label:
			if (!file)
				printf("| \e[0m%1d\033[0;30m |", 8 - rank);
			// Check wheter current square is attacked or not:
			is_square_attacked(square, side) ? printf(" \033[0;31m%s\033[0;30m |", "⚔") : printf(" %d\033[0;30m |", 0);
			if (file == 7)
				printf("   |");
		}
		// Print a new line after every rank:
		printf("\n");
	}
	// Print horizontal bottom border:
	printf("|   +---+---+---+---+---+---+---+---+   |\n");
	printf("|     \e[0ma   b   c   d   e   f   g   h\033[0;30m     |\n");
	printf("+---------------------------------------+\e[0m\n\n");
	// Print the bitboard as unsigned decimal number:
	// printf("\e[0mBitboard state: %llu\n\n", bitboard);
}

/*
	ENCODING AND DECODING MOVES

	Bits type			→ Binary				  			→ Hexadecimal

	source square		→ 0000 0000 0000 0000 0011 1111		→ 0x3f
	target square		→ 0000 0000 0000 1111 1100 0000		→ 0xfc0
	piece				→ 0000 0000 1111 0000 0000 0000		→ 0xf000
	promoted piece		→ 0000 1111 0000 0000 0000 0000		→ 0xf0000
	capture flag		→ 0001 0000 0000 0000 0000 0000		→ 0x100000
	double push flag	→ 0010 0000 0000 0000 0000 0000		→ 0x200000
	enpassant flag		→ 0100 0000 0000 0000 0000 0000		→ 0x400000
	castling flag		→ 1000 0000 0000 0000 0000 0000		→ 0x800000
*/

// Encode move:
#define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
	(source) |                                                                               \
			(target << 6) |                                                                      \
			(piece << 12) |                                                                      \
			(promoted << 16) |                                                                   \
			(capture << 20) |                                                                    \
			(double << 21) |                                                                     \
			(enpassant << 22) |                                                                  \
			(castling << 23)

// Extract move properties:
#define get_move_source(move) (move & 0x3f)
#define get_move_target(move) ((move & 0xfc0) >> 6)
#define get_move_piece(move) ((move & 0xf000) >> 12)
#define get_move_promoted(move) ((move & 0xf0000) >> 16)
#define get_move_capture(move) (move & 0x100000)
#define get_move_double(move) (move & 0x200000)
#define get_move_enpassant(move) (move & 0x400000)
#define get_move_castling(move) (move & 0x800000)

// Move list structure:
typedef struct
{
	// Moves:
	int moves[256];
	// Move count:
	int count;
} moves;

// Add a move to the move list:
static inline void add_move(moves *move_list, int move)
{
	// Store move:
	move_list->moves[move_list->count] = move;
	// Increment move count:
	move_list->count++;
}

// Print move (for UCI purposes):
void print_move(int move)
{
	if (get_move_promoted(move))
	{
		printf("%s%s%c", square_to_coordinates[get_move_source(move)],
					 square_to_coordinates[get_move_target(move)],
					 promoted_pieces[get_move_promoted(move)]);
	}
	else
	{
		printf("%s%s", square_to_coordinates[get_move_source(move)],
					 square_to_coordinates[get_move_target(move)]);
	}
}

// Print move list:
void print_move_list(moves *move_list)
{
	printf("\n\033[0;30m+-------------------------------------------------------------------+\n");
	printf("|                       \033[0;33mPRINTING THE MOVE LIST\033[0;30m                      |\n");
	printf("+----------+----------+-----------+----------+-----------+----------+\n");
	printf("| \033[0;33mMove\033[0;30m     | \033[0;33mPiece\033[0;30m    | \033[0;33mCapture\033[0;30m   | \033[0;33mDouble\033[0;30m   | \033[0;33mEnpassant\033[0;30m | \033[0;33mCastling\033[0;30m |\n");
	printf("+----------+----------+-----------+----------+-----------+----------+\n");
	// On empty move list:
	if (move_list->count < 1)
	{
		printf("|                      the move list is empty                       |\n");
	}
	// Loop over moves within a move list:
	else
	{
		for (int move_count = 0; move_count < move_list->count; move_count++)
		{
			// Initialize move:
			int move = move_list->moves[move_count];
			// Print move:
			printf("| \e[0m%s%s%c\033[0;30m    | \e[0m%c → %s\033[0;30m    | %s       | %s      | %s       | %s      |\n",
						 square_to_coordinates[get_move_source(move)],
						 square_to_coordinates[get_move_target(move)],
						 promoted_pieces[get_move_promoted(move)] ? promoted_pieces[get_move_promoted(move)] : ' ',
						 ascii_pieces[get_move_piece(move)], unicode_pieces[get_move_piece(move)],
						 (get_move_capture(move) ? "\033[0;32myes\033[0;30m" : "no "),
						 (get_move_double(move) ? "\033[0;32myes\033[0;30m" : "no "),
						 (get_move_enpassant(move) ? "\033[0;32myes\033[0;30m" : "no "),
						 (get_move_castling(move) ? "\033[0;32myes\033[0;30m" : "no "));
		}
	}
	printf("+----------+----------+-----------+----------+-----------+----------+\n");
	printf("|                        \033[0;33mMOVE LIST SIZE: \033[0;32m%3d\033[0;30m                        |\n", move_list->count);
	printf("+-------------------------------------------------------------------+\e[0m\n");
}

// Preserve board state:
#define copy_board()                                                  \
	U64 bitboards_copy[12], occupancies_copy[3];                        \
	int side_copy, enpassant_copy, castle_copy;                         \
	memcpy(bitboards_copy, bitboards, 96);                              \
	memcpy(occupancies_copy, occupancies, 24);                          \
	side_copy = side, enpassant_copy = enpassant, castle_copy = castle; \
	U64 hash_key_copy = hash_key;

// Restore board state:
#define restore_board()                                               \
	memcpy(bitboards, bitboards_copy, 96);                              \
	memcpy(occupancies, occupancies_copy, 24);                          \
	side = side_copy, enpassant = enpassant_copy, castle = castle_copy; \
	hash_key = hash_key_copy;

// Move types:
enum
{
	all_moves,
	only_captures
};

/*

	 CR	    MU	   BIN  DEC  DESC

	1111 & 1111 = 1111   15  King and rooks didnt move.

	1111 & 1100 = 1100   12  White king moved.
	1111 & 1110 = 1110   14  White kings rook moved.
	1111 & 1101 = 1101   13  White queens rook moved.

	1111 & 0011 = 0011    3  Black king moved.
	1111 & 1011 = 1011   11  Black kings rook moved.
	1111 & 0111 = 0111    7  Black queens rook moved.

*/

// Castling rights update constants:
const int castling_rights[64] = {
		7, 15, 15, 15, 3, 15, 15, 11,
		15, 15, 15, 15, 15, 15, 15, 15,
		15, 15, 15, 15, 15, 15, 15, 15,
		15, 15, 15, 15, 15, 15, 15, 15,
		15, 15, 15, 15, 15, 15, 15, 15,
		15, 15, 15, 15, 15, 15, 15, 15,
		15, 15, 15, 15, 15, 15, 15, 15,
		13, 15, 15, 15, 12, 15, 15, 14};

// Make move on chess board:
static inline int make_move(int move, int move_flag)
{
	// Quiet moves:
	if (move_flag == all_moves)
	{
		// Preserve board state:
		copy_board();
		// Parse the move:
		int source_square = get_move_source(move);
		int target_square = get_move_target(move);
		int piece = get_move_piece(move);
		int promoted = get_move_promoted(move);
		int capture_flag = get_move_capture(move);
		int double_flag = get_move_double(move);
		int enpassant_flag = get_move_enpassant(move);
		int castling_flag = get_move_castling(move);
		// Move the piece:
		pop_bit(bitboards[piece], source_square);
		set_bit(bitboards[piece], target_square);
		// Hash piece:
		hash_key ^= piece_keys[piece][source_square]; // Remove the piece from source square in hash key.
		hash_key ^= piece_keys[piece][target_square]; // Place the piece on target square in hash key.
		// Handling capture moves:
		if (capture_flag)
		{
			// Pick up bitboard piece index ranges depending on side:
			int start_piece, end_piece;
			// White to move:
			if (side == white)
			{
				start_piece = p;
				end_piece = k;
			}
			// Black to move:
			else
			{
				start_piece = P;
				end_piece = K;
			}
			// Loop over the bitboards of the opposite side to move:
			for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
			{
				// If there is a piece on the target square:
				if (get_bit(bitboards[bb_piece], target_square))
				{
					// Pop the piece from the bitboard:
					pop_bit(bitboards[bb_piece], target_square);
					// Remove the piece from hash key:
					hash_key ^= piece_keys[bb_piece][target_square];
					break;
				}
			}
		}
		// Handling pawn promotions:
		if (promoted)
		{
			// White to move:
			if (side == white)
			{
				// Erase the pawn from the target square:
				pop_bit(bitboards[P], target_square);
				// Remove the pawn from hash key:
				hash_key ^= piece_keys[P][target_square];
			}
			// Black to move:
			else
			{
				// Erase the pawn from the target square:
				pop_bit(bitboards[p], target_square);
				// Remove the pawn from hash key:
				hash_key ^= piece_keys[p][target_square];
			}
			// Set up promoted piece on chess board:
			set_bit(bitboards[promoted], target_square);
			// Hash the promoted piece:
			hash_key ^= piece_keys[promoted][target_square];
		}
		// Handling enpassant captures:
		if (enpassant_flag)
		{
			// White to move:
			if (side == white)
			{
				// Remove captured pawn:
				pop_bit(bitboards[p], target_square + 8);
				// Remove pawn from the hash key:
				hash_key ^= piece_keys[p][target_square + 8];
			}
			// Black to move:
			else
			{
				// Remove captured pawn:
				pop_bit(bitboards[P], target_square - 8);
				// Remove pawn from the hash key:
				hash_key ^= piece_keys[P][target_square - 8];
			}
		}
		// Hash enpassant if available (remove enpassant square from hash key):
		if (enpassant != no_sq)
		{
			hash_key ^= enpassant_keys[enpassant];
		}
		// Reseting the enpassant square:
		enpassant = no_sq;
		// Handling double pawn push:
		if (double_flag)
		{
			// White to move:
			if (side == white)
			{
				// Set the enpassant square:
				enpassant = target_square + 8;
				// Hash enpassant:
				hash_key ^= enpassant_keys[target_square + 8];
			}
			// Black to move:
			else
			{
				// Set the enpassant square:
				enpassant = target_square - 8;
				// Hash enpassant:
				hash_key ^= enpassant_keys[target_square - 8];
			}
		}
		// Handling the castling moves:
		if (castling_flag)
		{
			// Depending on king target square:
			switch (target_square)
			{
			// White castles king side:
			case (g1):
				// Move the H rook:
				pop_bit(bitboards[R], h1);
				set_bit(bitboards[R], f1);
				// Hash rook:
				hash_key ^= piece_keys[R][h1]; // Remove rook from h1 of the hash key.
				hash_key ^= piece_keys[R][f1]; // Place rook on f1 in the hash key.
				break;
			// White castles queen side:
			case (c1):
				// Move the H rook:
				pop_bit(bitboards[R], a1);
				set_bit(bitboards[R], d1);
				// Hash rook:
				hash_key ^= piece_keys[R][a1]; // Remove rook from a1 of the hash key.
				hash_key ^= piece_keys[R][d1]; // Place rook on d1 in the hash key.
				break;
			// Black castles king side:
			case (g8):
				// Move the H rook:
				pop_bit(bitboards[r], h8);
				set_bit(bitboards[r], f8);
				// Hash rook:
				hash_key ^= piece_keys[r][h8]; // Remove rook from h8 of the hash key.
				hash_key ^= piece_keys[r][f8]; // Place rook on f8 in the hash key.
				break;
			// Black castles queen side:
			case (c8):
				// Move the H rook:
				pop_bit(bitboards[r], a8);
				set_bit(bitboards[r], d8);
				// Hash rook:
				hash_key ^= piece_keys[r][a8]; // Remove rook from a8 of the hash key.
				hash_key ^= piece_keys[r][d8]; // Place rook on d8 in the hash key.
				break;
			}
		}
		// Un-hash castling:
		hash_key ^= castle_keys[castle];
		// Update castling rights:
		castle &= castling_rights[source_square];
		castle &= castling_rights[target_square];
		// Hash castling again:
		hash_key ^= castle_keys[castle];
		// Reset occupancies:
		memset(occupancies, 0ULL, 24);
		// Loop over white pieces bitboards:
		for (int bb_piece = P; bb_piece <= K; bb_piece++)
		{
			// Update white occupancies:
			occupancies[white] |= bitboards[bb_piece];
		}
		// Loop over black pieces bitboards:
		for (int bb_piece = p; bb_piece <= k; bb_piece++)
		{
			// Update black occupancies:
			occupancies[black] |= bitboards[bb_piece];
		}
		// Update both sides occupancies:
		occupancies[both] |= occupancies[white];
		occupancies[both] |= occupancies[black];
		// Change side to move:
		side ^= 1;

		// Hash side:
		hash_key ^= side_key;

		/*******************************************************************\
		================ DEBUG HASH KEY INCREMENTAL UPDATES =================
		\*******************************************************************/
		/*
		// Build hash key for the updated position (after move is made) from scratch:
		U64 hash_from_scratch = generate_hash_key();
		// In case the built hash key from scratch does not match
		// the one that was incrementally updated we interrupt execution:
		if (hash_key != hash_from_scratch)
		{
			// Print the board:
			print_board();
			// Print the code area:
			printf("Make move:\n");
			// Print the move:
			printf("Move: ");
			print_move(move);
			// Print the desired hash key:
			printf("\nHash key should be: %llx\n", hash_from_scratch);
			getchar();
		}
		*/
		// Make sure that the king was not exposed to a check:
		if (is_square_attacked((side == white) ? get_ls1b_index(bitboards[k]) : get_ls1b_index(bitboards[K]), side))
		{
			// Move is illegal, take it back:
			restore_board();
			// Return illegal move:
			return 0;
		}
		else
		{
			// Return legal move:
			return 1;
		}
	}
	// Capture moves:
	else
	{
		// Make sure the move is a capture:
		if (get_move_capture(move))
		{
			make_move(move, all_moves);
		}
		// Otherwise the move is not a capture:
		else
		{
			return 0;
		}
	}
}

// Generate all moves:
static inline void generate_moves(moves *move_list)
{
	// Initialize move count:
	move_list->count = 0;
	// Define source and target squares:
	int source_square, target_square;
	// Define current pieces bitboard copy and its attacks:
	U64 bitboard, attacks;
	// Loop over all the bitboards:
	for (int piece = P; piece <= k; piece++)
	{
		// Initializate piece bitboard copy:
		bitboard = bitboards[piece];
		// Generate white pawns and white king castling moves:
		if (side == white)
		{
			// Select white pawns bitboard index:
			if (piece == P)
			{
				// Loop over white pawns within white pawns bitboard:
				while (bitboard)
				{
					// Initialize source square:
					source_square = get_ls1b_index(bitboard);
					// Initialize target square:
					target_square = source_square - 8;
					// Generate quiet pawn moves:
					if (!(target_square < a8) && !get_bit(occupancies[both], target_square))
					{
						// Pawn promotion:
						if (source_square >= a7 && source_square <= h7)
						{
							add_move(move_list, encode_move(source_square, target_square, piece, Q, 0, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, R, 0, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, B, 0, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, N, 0, 0, 0, 0));
						}
						else
						{
							// One square pawn move:
							add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
							// Two squares pawn move:
							if ((source_square >= a2 && source_square <= h2) && !get_bit(occupancies[both], target_square - 8))
							{
								add_move(move_list, encode_move(source_square, target_square - 8, piece, 0, 0, 1, 0, 0));
							}
						}
					}
					// Initialize pawn attacks bitboards:
					attacks = pawn_attacks[side][source_square] & occupancies[black];
					// Generate pawn captures:
					while (attacks)
					{
						// Initialize target square:
						target_square = get_ls1b_index(attacks);
						// Pawn promotion:
						if (source_square >= a7 && source_square <= h7)
						{
							add_move(move_list, encode_move(source_square, target_square, piece, Q, 1, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, R, 1, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, B, 1, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, N, 1, 0, 0, 0));
						}
						else
						{
							// One square pawn move:
							add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
						}
						// Pop the LS1B from attacks:
						pop_bit(attacks, target_square);
					}
					// Generate enpassant captures:
					if (enpassant != no_sq)
					{
						// Lookup pawn attacks and bitwise AND with enpassant square (bit):
						U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);
						// Make sure that enpassant capture is available:
						if (enpassant_attacks)
						{
							// Inititalize enpassant capture target square:
							int target_enpassant = get_ls1b_index(enpassant_attacks);
							add_move(move_list, encode_move(source_square, target_enpassant, piece, 0, 1, 0, 1, 0));
						}
					}
					// Pop LS1B from piece bitboard copy:
					pop_bit(bitboard, source_square);
				}
			}
			// Castling moves:
			if (piece == K)
			{
				// King side castling is available:
				if (castle & wk)
				{
					// Make sure the squares betwen king and kings rook are empty:
					if (!get_bit(occupancies[both], f1) && !get_bit(occupancies[both], g1))
					{
						// Make sure that king and destination square are not under attack:
						if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black))
						{
							add_move(move_list, encode_move(e1, g1, piece, 0, 0, 0, 0, 1));
						}
					}
				}
				// Queen side castling is available:
				if (castle & wq)
				{
					// Make sure the squares betwen king and queens rook are empty:
					if (!get_bit(occupancies[both], d1) && !get_bit(occupancies[both], c1) && !get_bit(occupancies[both], b1))
					{
						// Make sure that king and destination square are not under attack:
						if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black))
						{
							add_move(move_list, encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
						}
					}
				}
			}
		}
		// Generate black pawns and black king castling moves:
		else
		{
			// Select black pawns bitboard index:
			if (piece == p)
			{
				// Loop over black pawns within black pawns bitboard:
				while (bitboard)
				{
					// Initialize source square:
					source_square = get_ls1b_index(bitboard);
					// Initialize target square:
					target_square = source_square + 8;
					// Generate quiet pawn moves:
					if (!(target_square > h1) && !get_bit(occupancies[both], target_square))
					{
						// Pawn promotion:
						if (source_square >= a2 && source_square <= h2)
						{
							add_move(move_list, encode_move(source_square, target_square, piece, q, 0, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, r, 0, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, b, 0, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, n, 0, 0, 0, 0));
						}
						else
						{
							// One square pawn move:
							add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
							// Two squares pawn move:
							if ((source_square >= a7 && source_square <= h7) && !get_bit(occupancies[both], target_square + 8))
							{
								add_move(move_list, encode_move(source_square, target_square + 8, piece, 0, 0, 1, 0, 0));
							}
						}
					}
					// Initialize pawn attacks bitboards:
					attacks = pawn_attacks[side][source_square] & occupancies[white];
					// Generate pawn captures:
					while (attacks)
					{
						// Initialize target square:
						target_square = get_ls1b_index(attacks);
						// Pawn promotion:
						if (source_square >= a2 && source_square <= h2)
						{
							add_move(move_list, encode_move(source_square, target_square, piece, q, 1, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, r, 1, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, b, 1, 0, 0, 0));
							add_move(move_list, encode_move(source_square, target_square, piece, n, 1, 0, 0, 0));
						}
						else
						{
							// One square pawn move:
							add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
						}
						// Pop the LS1B from attacks:
						pop_bit(attacks, target_square);
					}
					// Generate enpassant captures:
					if (enpassant != no_sq)
					{
						// Lookup pawn attacks and bitwise AND with enpassant square (bit):
						U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);
						// Make sure that enpassant capture is available:
						if (enpassant_attacks)
						{
							// Inititalize enpassant capture target square:
							int target_enpassant = get_ls1b_index(enpassant_attacks);
							add_move(move_list, encode_move(source_square, target_enpassant, piece, 0, 1, 0, 1, 0));
						}
					}
					// Pop LS1B from piece bitboard copy:
					pop_bit(bitboard, source_square);
				}
			}
			// Castling moves:
			if (piece == k)
			{
				// King side castling is available:
				if (castle & bk)
				{
					// Make sure the squares betwen king and kings rook are empty:
					if (!get_bit(occupancies[both], f8) && !get_bit(occupancies[both], g8))
					{
						// Make sure that king and destination square are not under attack:
						if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white))
						{
							add_move(move_list, encode_move(e8, g8, piece, 0, 0, 0, 0, 1));
						}
					}
				}
				// Queen side castling is available:
				if (castle & bq)
				{
					// Make sure the squares betwen king and queens rook are empty:
					if (!get_bit(occupancies[both], d8) && !get_bit(occupancies[both], c8) && !get_bit(occupancies[both], b8))
					{
						// Make sure that king and destination square are not under attack:
						if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white))
						{
							add_move(move_list, encode_move(e8, c8, piece, 0, 0, 0, 0, 1));
						}
					}
				}
			}
		}
		// Generate knight moves:
		if ((side == white) ? piece == N : piece == n)
		{
			// Loop over source squares of piece bitboard copy:
			while (bitboard)
			{
				// Initialize source square:
				source_square = get_ls1b_index(bitboard);
				// Initialize piece attacks in order to set target squares:
				attacks = knight_attacks[source_square] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);
				// Loop over target squares available from generated attacks:
				while (attacks)
				{
					// initialize target square:
					target_square = get_ls1b_index(attacks);
					// Quiet moves:
					if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
					}
					// Capture moves:
					else
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
					}
					// pop LS1B in current attacks set:
					pop_bit(attacks, target_square);
				}
				// Pop LS1B from the current piece bitboard copy:
				pop_bit(bitboard, source_square);
			}
		}
		// Generate bishop moves:
		if ((side == white) ? piece == B : piece == b)
		{
			// Loop over source squares of piece bitboard copy:
			while (bitboard)
			{
				// Initialize source square:
				source_square = get_ls1b_index(bitboard);
				// Initialize piece attacks in order to set target squares:
				attacks = get_bishop_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);
				// Loop over target squares available from generated attacks:
				while (attacks)
				{
					// initialize target square:
					target_square = get_ls1b_index(attacks);
					// Quiet moves:
					if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
					}
					// Capture moves:
					else
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
					}
					// pop LS1B in current attacks set:
					pop_bit(attacks, target_square);
				}
				// Pop LS1B from the current piece bitboard copy:
				pop_bit(bitboard, source_square);
			}
		}
		// Generate rook moves:
		if ((side == white) ? piece == R : piece == r)
		{
			// Loop over source squares of piece bitboard copy:
			while (bitboard)
			{
				// Initialize source square:
				source_square = get_ls1b_index(bitboard);
				// Initialize piece attacks in order to set target squares:
				attacks = get_rook_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);
				// Loop over target squares available from generated attacks:
				while (attacks)
				{
					// initialize target square:
					target_square = get_ls1b_index(attacks);
					// Quiet moves:
					if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
					}
					// Capture moves:
					else
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
					}
					// pop LS1B in current attacks set:
					pop_bit(attacks, target_square);
				}
				// Pop LS1B from the current piece bitboard copy:
				pop_bit(bitboard, source_square);
			}
		}
		// Generate queen moves:
		if ((side == white) ? piece == Q : piece == q)
		{
			// Loop over source squares of piece bitboard copy:
			while (bitboard)
			{
				// Initialize source square:
				source_square = get_ls1b_index(bitboard);
				// Initialize piece attacks in order to set target squares:
				attacks = get_queen_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);
				// Loop over target squares available from generated attacks:
				while (attacks)
				{
					// initialize target square:
					target_square = get_ls1b_index(attacks);
					// Quiet moves:
					if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
					}
					// Capture moves:
					else
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
					}
					// pop LS1B in current attacks set:
					pop_bit(attacks, target_square);
				}
				// Pop LS1B from the current piece bitboard copy:
				pop_bit(bitboard, source_square);
			}
		}
		// Generate king moves:
		if ((side == white) ? piece == K : piece == k)
		{
			// Loop over source squares of piece bitboard copy:
			while (bitboard)
			{
				// Initialize source square:
				source_square = get_ls1b_index(bitboard);
				// Initialize piece attacks in order to set target squares:
				attacks = king_attacks[source_square] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);
				// Loop over target squares available from generated attacks:
				while (attacks)
				{
					// initialize target square:
					target_square = get_ls1b_index(attacks);
					// Quiet moves:
					if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
					}
					// Capture moves:
					else
					{
						add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
					}
					// pop LS1B in current attacks set:
					pop_bit(attacks, target_square);
				}
				// Pop LS1B from the current piece bitboard copy:
				pop_bit(bitboard, source_square);
			}
		}
	}
}

/******************************************************************************\
=================================== PERFT =====================================
\******************************************************************************/

// Leaf nodes (number of positions reached during the test of the moves generator at a given depth):
U64 nodes;

// PERFT driver:
static inline void perft_driver(int depth)
{
	// Recursion scape condition:
	if (depth == 0)
	{
		// increments nodes count (count reached positions):
		nodes++;
		return;
	}
	// Create a move list instance:
	moves move_list[1];
	// Generate moves:
	generate_moves(move_list);
	// Loop over generated moves:
	for (int move_count = 0; move_count < move_list->count; move_count++)
	{
		// Preserve board state:
		copy_board();
		// Make the move:
		if (!make_move(move_list->moves[move_count], all_moves))
		{
			continue;
		}
		// Call PERFT driver recursively:
		perft_driver(depth - 1);
		// Restore the board:
		restore_board();

		/*******************************************************************\
		================ DEBUG HASH KEY INCREMENTAL UPDATES =================
		\*******************************************************************/
		/*
		// Build hash key for the updated position (after move is made) from scratch:
		U64 hash_from_scratch = generate_hash_key();
		// In case the built hash key from scratch does not match
		// the one that was incrementally updated we interrupt execution:
		if (hash_key != hash_from_scratch)
		{
			// Print the board:
			print_board();
			// Print the code area:
			printf("Take move back:\n");
			// Print the move:
			printf("Move: ");
			print_move(move_list->moves[move_count]);
			// Print the desired hash key:
			printf("\nHash key should be: %llx\n", hash_from_scratch);
			getchar();
		}
		*/
	}
}

// PERFT test:
void perft_test(int depth)
{
	printf("\nPerform PERFT test:\n\n");
	// Create a move list instance:
	moves move_list[1];
	// Generate moves:
	generate_moves(move_list);
	// Initialize start time:
	long start = get_time_ms();
	// Loop over generated moves:
	for (int move_count = 0; move_count < move_list->count; move_count++)
	{
		// Preserve board state:
		copy_board();
		// Make the move:
		if (!make_move(move_list->moves[move_count], all_moves))
		{
			continue;
		}
		// Cummulative nodes:
		long cummulative_nodes = nodes;
		// Call PERFT driver recursively:
		perft_driver(depth - 1);
		// Old nodes:
		long old_nodes = nodes - cummulative_nodes;
		// Restore the board:
		restore_board();
		// Print move:
		printf("move: %s%s%c nodes: %ld\n",
					 square_to_coordinates[get_move_source(move_list->moves[move_count])],
					 square_to_coordinates[get_move_target(move_list->moves[move_count])],
					 promoted_pieces[get_move_promoted(move_list->moves[move_count])],
					 old_nodes);
	}
	// Print results:
	printf("\nDepth: %d\n", depth);
	printf("Nodes: %lld\n", nodes);
	printf("Time: %ldms\n\n", get_time_ms() - start);
}

/******************************************************************************\
================================ EVALUATION ====================================
\******************************************************************************/

/*

	♟︎	=	 100			= ♟︎
	♞	 =	300			 = ♟︎ * 3
	♝	 =	350			 = ♟︎ * 3 + 1
	♜	 =	500			 = ♟︎ * 5
	♛	 =	1000		 = ♟︎ * 9
	♚	 =	10000		 = ♟︎ * 100

*/

// Material score [game phase][piece]:
const int material_score[2][12] = {
		// Opening material score:
		82, 337, 365, 477, 1025, 12000, -82, -337, -365, -477, -1025, -12000,
		// Endgame material score:
		94, 281, 297, 512, 936, 12000, -94, -281, -297, -512, -936, -12000};

// Game phase scores:
const int opening_phase_score = 6192;
const int endgame_phase_score = 518;

// Game phases:
enum
{
	opening,
	endgame,
	middlegame
};

// Piece types:
enum
{
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
};

// Positional piece scores [game phase][piece][square]:
const int positional_score[2][6][64] = {
		/*
			OPENING POSITIONAL PIECE SCORES
		*/
		// Pawn:
		0, 0, 0, 0, 0, 0, 0, 0,
		98, 134, 61, 95, 68, 126, 34, -11,
		-6, 7, 26, 31, 65, 56, 25, -20,
		-14, 13, 6, 21, 23, 12, 17, -23,
		-27, -2, -5, 12, 17, 6, 10, -25,
		-26, -4, -4, -10, 3, 3, 33, -12,
		-35, -1, -20, -23, -15, 24, 38, -22,
		0, 0, 0, 0, 0, 0, 0, 0,
		// Knight:
		-167, -89, -34, -49, 61, -97, -15, -107,
		-73, -41, 72, 36, 23, 62, 7, -17,
		-47, 60, 37, 65, 84, 129, 73, 44,
		-9, 17, 19, 53, 37, 69, 18, 22,
		-13, 4, 16, 13, 28, 19, 21, -8,
		-23, -9, 12, 10, 19, 17, 25, -16,
		-29, -53, -12, -3, -1, 18, -14, -19,
		-105, -21, -58, -33, -17, -28, -19, -23,
		// Bishop:
		-29, 4, -82, -37, -25, -42, 7, -8,
		-26, 16, -18, -13, 30, 59, 18, -47,
		-16, 37, 43, 40, 35, 50, 37, -2,
		-4, 5, 19, 50, 37, 37, 7, -2,
		-6, 13, 13, 26, 34, 12, 10, 4,
		0, 15, 15, 15, 14, 27, 18, 10,
		4, 15, 16, 0, 7, 21, 33, 1,
		-33, -3, -14, -21, -13, -12, -39, -21,
		// Rook:
		32, 42, 32, 51, 63, 9, 31, 43,
		27, 32, 58, 62, 80, 67, 26, 44,
		-5, 19, 26, 36, 17, 45, 61, 16,
		-24, -11, 7, 26, 24, 35, -8, -20,
		-36, -26, -12, -1, 9, -7, 6, -23,
		-45, -25, -16, -17, 3, 0, -5, -33,
		-44, -16, -20, -9, -1, 11, -6, -71,
		-19, -13, 1, 17, 16, 7, -37, -26,
		// Queen:
		-28, 0, 29, 12, 59, 44, 43, 45,
		-24, -39, -5, 1, -16, 57, 28, 54,
		-13, -17, 7, 8, 29, 56, 47, 57,
		-27, -27, -16, -16, -1, 17, -2, 1,
		-9, -26, -9, -10, -2, -4, 3, -3,
		-14, 2, -11, -2, -5, 2, 14, 5,
		-35, -8, 11, 2, 8, 15, -3, 1,
		-1, -18, -9, 10, -15, -25, -31, -50,
		// King:
		-65, 23, 16, -15, -56, -34, 2, 13,
		29, -1, -20, -7, -8, -4, -38, -29,
		-9, 24, 2, -16, -20, 6, 22, -22,
		-17, -20, -12, -27, -30, -25, -14, -36,
		-49, -1, -27, -39, -46, -44, -33, -51,
		-14, -14, -22, -46, -44, -30, -15, -27,
		1, 7, -8, -64, -43, -16, 9, 8,
		-15, 36, 12, -54, 8, -28, 24, 14,
		/*
			ENDGAME POSITIONAL PIECE SCORES
		*/
		// Pawn:
		0, 0, 0, 0, 0, 0, 0, 0,
		178, 173, 158, 134, 147, 132, 165, 187,
		94, 100, 85, 67, 56, 53, 82, 84,
		32, 24, 13, 5, -2, 4, 17, 17,
		13, 9, -3, -7, -7, -8, 3, -1,
		4, 7, -6, 1, 0, -5, -1, -8,
		13, 8, 8, 10, 13, 0, 2, -7,
		0, 0, 0, 0, 0, 0, 0, 0,
		// Knight:
		-58, -38, -13, -28, -31, -27, -63, -99,
		-25, -8, -25, -2, -9, -25, -24, -52,
		-24, -20, 10, 9, -1, -9, -19, -41,
		-17, 3, 22, 22, 22, 11, 8, -18,
		-18, -6, 16, 25, 16, 17, 4, -18,
		-23, -3, -1, 15, 10, -3, -20, -22,
		-42, -20, -10, -5, -2, -20, -23, -44,
		-29, -51, -23, -15, -22, -18, -50, -64,
		// Bishop:
		-14, -21, -11, -8, -7, -9, -17, -24,
		-8, -4, 7, -12, -3, -13, -4, -14,
		2, -8, 0, -1, -2, 6, 0, 4,
		-3, 9, 12, 9, 14, 10, 3, 2,
		-6, 3, 13, 19, 7, 10, -3, -9,
		-12, -3, 8, 10, 13, 3, -7, -15,
		-14, -18, -7, -1, 4, -9, -15, -27,
		-23, -9, -23, -5, -9, -16, -5, -17,
		// Rook:
		13, 10, 18, 15, 12, 12, 8, 5,
		11, 13, 13, 11, -3, 3, 8, 3,
		7, 7, 7, 5, 4, -3, -5, -3,
		4, 3, 13, 1, 2, 1, -1, 2,
		3, 5, 8, 4, -5, -6, -8, -11,
		-4, 0, -5, -1, -7, -12, -8, -16,
		-6, -6, 0, 2, -9, -9, -11, -3,
		-9, 2, 3, -1, -5, -13, 4, -20,
		// Queen:
		-9, 22, 22, 27, 27, 19, 10, 20,
		-17, 20, 32, 41, 58, 25, 30, 0,
		-20, 6, 9, 49, 47, 35, 19, 9,
		3, 22, 24, 45, 57, 40, 57, 36,
		-18, 28, 19, 47, 31, 34, 39, 23,
		-16, -27, 15, 6, 9, 17, 10, 5,
		-22, -23, -30, -16, -16, -23, -36, -32,
		-33, -28, -22, -43, -5, -32, -20, -41,
		// King:
		-74, -35, -18, -18, -11, 15, 4, -17,
		-12, 17, 14, 17, 17, 38, 23, 11,
		10, 17, 23, 15, 20, 45, 44, 13,
		-8, 22, 24, 27, 26, 33, 26, 3,
		-18, -4, 21, 24, 27, 23, 9, -11,
		-19, -3, 11, 21, 23, 16, 7, -9,
		-27, -11, 4, 13, 14, 4, -5, -17,
		-53, -34, -21, -11, -28, -14, -24, -43};

// Mirror positional score tables for opposite side:
const int mirror_scores[128] = {
		a1, b1, c1, d1, e1, f1, g1, h1,
		a2, b2, c2, d2, e2, f2, g2, h2,
		a3, b3, c3, d3, e3, f3, g3, h3,
		a4, b4, c4, d4, e4, f4, g4, h4,
		a5, b5, c5, d5, e5, f5, g5, h5,
		a6, b6, c6, d6, e6, f6, g6, h6,
		a7, b7, c7, d7, e7, f7, g7, h7,
		a8, b8, c8, d8, e8, f8, g8, h8};

/*
					Rank mask            File mask           Isolated mask        Passed pawn mask
				for square a6        for square f2         for square g2          for square c4

		8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 1 0 0    8  0 0 0 0 0 1 0 1     8  0 1 1 1 0 0 0 0
		7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 1 0 0    7  0 0 0 0 0 1 0 1     7  0 1 1 1 0 0 0 0
		6  1 1 1 1 1 1 1 1    6  0 0 0 0 0 1 0 0    6  0 0 0 0 0 1 0 1     6  0 1 1 1 0 0 0 0
		5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 1 0 0    5  0 0 0 0 0 1 0 1     5  0 1 1 1 0 0 0 0
		4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 1 0 0    4  0 0 0 0 0 1 0 1     4  0 0 0 0 0 0 0 0
		3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 1 0 0    3  0 0 0 0 0 1 0 1     3  0 0 0 0 0 0 0 0
		2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 1 0 0    2  0 0 0 0 0 1 0 1     2  0 0 0 0 0 0 0 0
		1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 1 0 0    1  0 0 0 0 0 1 0 1     1  0 0 0 0 0 0 0 0

			 a b c d e f g h       a b c d e f g h       a b c d e f g h        a b c d e f g h
*/

// File masks [square]:
U64 file_masks[64];

// Rank masks [square]:
U64 rank_masks[64];

// Isolated pawn masks [square]:
U64 isolated_masks[64];

// White passed pawn masks [square]:
U64 white_passed_masks[64];

// Black passed pawn masks [square]:
U64 black_passed_masks[64];

// Extract rank from a given square [square]:
const int get_rank[64] =
		{
				7, 7, 7, 7, 7, 7, 7, 7,
				6, 6, 6, 6, 6, 6, 6, 6,
				5, 5, 5, 5, 5, 5, 5, 5,
				4, 4, 4, 4, 4, 4, 4, 4,
				3, 3, 3, 3, 3, 3, 3, 3,
				2, 2, 2, 2, 2, 2, 2, 2,
				1, 1, 1, 1, 1, 1, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0};

// Double pawns penalty:
const int double_pawn_penalty = -10;

// Isolated pawns penalty:
const int isolated_pawn_penalty = -10;

// Passed pawns bonus:
const int passed_pawn_bonus[8] = {0, 10, 30, 50, 75, 100, 150, 200};

// Semi open file score:
const int semi_open_file_scores = 10;

// Open file score:
const int open_file_score = 15;

// Kings shield bonus:
const int king_shield_bonus = 5;

// Set file or rank mask:
U64 set_file_rank_mask(int file_number, int rank_number)
{
	// Initialize file or rank mask:
	U64 mask = 0ULL;
	// Loop over the ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Loop over the files:
		for (int file = 0; file < 8; file++)
		{
			// Initialize the square:
			int square = rank * 8 + file;
			// If there is a given file:
			if (file_number != -1)
			{
				// On file match:
				if (file == file_number)
				{
					// Set bit on the mask:
					mask |= set_bit(mask, square);
				}
			}
			// If there is a given rank:
			else if (rank_number != -1)
			{
				// On rank match:
				if (rank == rank_number)
				{
					// Set bit on the mask:
					mask |= set_bit(mask, square);
				}
			}
		}
	}
	// Return the mask:
	return mask;
}

// Init evaluation masks:
void init_evaluation_masks()
{
	/*
		INITIALIZE FILE MASKS
	*/
	// Loop over ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Loop over files:
		for (int file = 0; file < 8; file++)
		{
			// Initialize square:
			int square = rank * 8 + file;
			// Initialize file mask for a current square:
			file_masks[square] |= set_file_rank_mask(file, -1);
		}
	}
	/*
		INITIALIZE RANK MASKS
	*/
	// Loop over ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Loop over files:
		for (int file = 0; file < 8; file++)
		{
			// Initialize square:
			int square = rank * 8 + file;
			// Initialize file mask for a current square:
			rank_masks[square] |= set_file_rank_mask(-1, rank);
		}
	}
	/*
		INITIALIZE ISOLATED MASKS
	*/
	// Loop over ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Loop over files:
		for (int file = 0; file < 8; file++)
		{
			// Initialize square:
			int square = rank * 8 + file;
			// Initialize file mask for a current square:
			isolated_masks[square] |= set_file_rank_mask(file - 1, -1);
			isolated_masks[square] |= set_file_rank_mask(file + 1, -1);
		}
	}
	/*
		INITIALIZE WHITE PASSED MASKS
	*/
	// Loop over ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Loop over files:
		for (int file = 0; file < 8; file++)
		{
			// Initialize square:
			int square = rank * 8 + file;
			// Initialize file mask for a current square:
			white_passed_masks[square] |= set_file_rank_mask(file - 1, -1);
			white_passed_masks[square] |= set_file_rank_mask(file, -1);
			white_passed_masks[square] |= set_file_rank_mask(file + 1, -1);
			// Loop over redudant ranks:
			for (int i = 0; i < (8 - rank); i++)
			{
				// Reset redundant bits:
				white_passed_masks[square] &= ~rank_masks[(7 - i) * 8 + file];
			}
		}
	}
	/*
		INITIALIZE BLACK PASSED MASKS
	*/
	// Loop over ranks:
	for (int rank = 0; rank < 8; rank++)
	{
		// Loop over files:
		for (int file = 0; file < 8; file++)
		{
			// Initialize square:
			int square = rank * 8 + file;
			// Initialize file mask for a current square:
			black_passed_masks[square] |= set_file_rank_mask(file - 1, -1);
			black_passed_masks[square] |= set_file_rank_mask(file, -1);
			black_passed_masks[square] |= set_file_rank_mask(file + 1, -1);
			// Loop over redudant ranks:
			for (int i = 0; i < rank + 1; i++)
			{
				// Reset redundant bits:
				black_passed_masks[square] &= ~rank_masks[i * 8 + file];
			}
		}
	}
}

// Get the game phase score:
static inline int get_game_phase_score()
{
	/*
		The game phase score of the game is derived from the pieces
		(not counting pawns and kings) that are still on the board.
		The full material starting position game phase score is:

		4 * knight material score in the opening +
		4 * bishop material score in the opening +
		4 * rook material score in the opening +
		2 * queen material score in the opening
	*/
	// Initializate the pieces counter:
	int white_piece_scores = 0;
	int black_piece_scores = 0;
	// Loop over white pieces:
	for (int piece = N; piece <= Q; piece++)
	{
		white_piece_scores += count_bits(bitboards[piece]) * material_score[opening][piece];
	}
	// Loop over black pieces:
	for (int piece = n; piece <= q; piece++)
	{
		black_piece_scores += count_bits(bitboards[piece]) * -material_score[opening][piece];
	}
	// Return the game phase score:
	return white_piece_scores + black_piece_scores;
}

// Position evaluation:
static inline int evaluate()
{
	// Get the game phase score:
	int game_phase_score = get_game_phase_score();
	// Initialize the game phase variable:
	int game_phase = -1;
	// Define the game phase based on game phase score:
	if (game_phase_score > opening_phase_score)
	{
		game_phase = opening;
	}
	else if (game_phase_score < endgame_phase_score)
	{
		game_phase = endgame;
	}
	else
	{
		game_phase = middlegame;
	}
	// Static evaluation score:
	int score = 0;
	int score_opening = 0;
	int score_endgame = 0;
	// Current pieces bitboard copy:
	U64 bitboard;
	// Initialize piece and square:
	int piece, square;
	// Penalties:
	int double_pawns = 0;
	// Loop over the pieces bitboards:
	for (int bb_piece = P; bb_piece <= k; bb_piece++)
	{
		// Initialize piece bitboard copy:
		bitboard = bitboards[bb_piece];
		// Loop over pieces within a bitboard:
		while (bitboard)
		{
			// Initialize the piece:
			piece = bb_piece;
			// Initialize square:
			square = get_ls1b_index(bitboard);
			// Calculate material (opening and endgame) scores:
			score_opening += material_score[opening][piece];
			score_endgame += material_score[endgame][piece];
			// Score positional weights:
			switch (piece)
			{
			// Evaluate white pieces:
			case P:
				// Calculate positional (opening and endgame) scores:
				score_opening += positional_score[opening][PAWN][square];
				score_endgame += positional_score[endgame][PAWN][square];
				/* // Double pawn panalty:
				double_pawns = count_bits(bitboards[P] & file_masks[square]);
				// On stacked pawns:
				if (double_pawns > 1)
				{
					// Applies the penalty:
					score += double_pawns * double_pawn_penalty;
				}
				// On isolated pawns:
				if ((bitboards[P] & isolated_masks[square]) == 0)
				{
					// Give an isolated pawn penalty:
					score += isolated_pawn_penalty;
				}
				// On passed pawns:
				if ((white_passed_masks[square] & bitboards[p]) == 0)
				{
					// Give an passed pawn bonus:
					score += passed_pawn_bonus[get_rank[square]];
				} */
				break;
			case N:
				// Calculate positional (opening and endgame) scores:
				score_opening += positional_score[opening][KNIGHT][square];
				score_endgame += positional_score[endgame][KNIGHT][square];
				break;
			case B:
				// Calculate positional (opening and endgame) scores:
				score_opening += positional_score[opening][BISHOP][square];
				score_endgame += positional_score[endgame][BISHOP][square];
				/* // Mobility:
				score += count_bits(get_bishop_attacks(square, occupancies[both])); */
				break;
			case R:
				// Calculate positional (opening and endgame) scores:
				score_opening += positional_score[opening][ROOK][square];
				score_endgame += positional_score[endgame][ROOK][square];
				/* // Semi open file bonus:
				if ((bitboards[P] & file_masks[square]) == 0)
				{
					// Give the semi open file bonus:
					score += semi_open_file_scores;
				}
				// Open file bonus:
				if (((bitboards[P] | bitboards[p]) & file_masks[square]) == 0)
				{
					// Give the semi open file bonus:
					score += open_file_score;
				} */
				break;
			case Q:
				// Calculate positional (opening and endgame) scores:
				score_opening += positional_score[opening][QUEEN][square];
				score_endgame += positional_score[endgame][QUEEN][square];
				/* // Mobility:
				score += count_bits(get_queen_attacks(square, occupancies[both])); */
				break;
			case K:
				// Calculate positional (opening and endgame) scores:
				score_opening += positional_score[opening][KING][square];
				score_endgame += positional_score[endgame][KING][square];
				/* // Semi open file penalty:
				if ((bitboards[P] & file_masks[square]) == 0)
				{
					// Give the semi open file penalty:
					score -= semi_open_file_scores;
				}
				// Open file penalty:
				if (((bitboards[P] | bitboards[p]) & file_masks[square]) == 0)
				{
					// Give the semi open file penalty:
					score -= open_file_score;
				}
				// King safety bonus:
				score += count_bits(king_attacks[square] & occupancies[white]) * king_shield_bonus; */
				break;
			// Evaluate black pieces:
			case p:
				// Calculate positional (opening and endgame) scores:
				score_opening -= positional_score[opening][PAWN][mirror_scores[square]];
				score_endgame -= positional_score[endgame][PAWN][mirror_scores[square]];
				/* // Double pawn panalty:
				double_pawns = count_bits(bitboards[p] & file_masks[square]);
				// On stacked pawns:
				if (double_pawns > 1)
				{
					// Applies the penalty:
					score -= double_pawns * double_pawn_penalty;
				}
				// On isolated pawns:
				if ((bitboards[p] & isolated_masks[square]) == 0)
				{
					// Give an isolated pawn penalty:
					score -= isolated_pawn_penalty;
				}
				// On passed pawns:
				if ((black_passed_masks[square] & bitboards[P]) == 0)
				{
					// Give an passed pawn bonus:
					score -= passed_pawn_bonus[get_rank[mirror_scores[square]]];
				} */
				break;
			case n:
				// Calculate positional (opening and endgame) scores:
				score_opening -= positional_score[opening][KNIGHT][mirror_scores[square]];
				score_endgame -= positional_score[endgame][KNIGHT][mirror_scores[square]];
				break;
			case b:
				// Calculate positional (opening and endgame) scores:
				score_opening -= positional_score[opening][BISHOP][mirror_scores[square]];
				score_endgame -= positional_score[endgame][BISHOP][mirror_scores[square]];
				/* // Mobility:
				score -= count_bits(get_bishop_attacks(square, occupancies[both])); */
				break;
			case r:
				// Calculate positional (opening and endgame) scores:
				score_opening -= positional_score[opening][ROOK][mirror_scores[square]];
				score_endgame -= positional_score[endgame][ROOK][mirror_scores[square]];
				/* // Semi open file bonus:
				if ((bitboards[p] & file_masks[square]) == 0)
				{
					// Give the semi open file bonus:
					score -= semi_open_file_scores;
				}
				// Open file bonus:
				if (((bitboards[p] | bitboards[P]) & file_masks[square]) == 0)
				{
					// Give the semi open file bonus:
					score -= open_file_score;
				} */
				break;
			case q:
				// Calculate positional (opening and endgame) scores:
				score_opening -= positional_score[opening][QUEEN][mirror_scores[square]];
				score_endgame -= positional_score[endgame][QUEEN][mirror_scores[square]];
				/* // Mobility:
				score -= count_bits(get_queen_attacks(square, occupancies[both])); */
				break;
			case k:
				// Calculate positional (opening and endgame) scores:
				score_opening -= positional_score[opening][KING][mirror_scores[square]];
				score_endgame -= positional_score[endgame][KING][mirror_scores[square]];
				/* // Semi open file penalty:
				if ((bitboards[p] & file_masks[square]) == 0)
				{
					// Give the semi open file penalty:
					score += semi_open_file_scores;
				}
				// Open file penalty:
				if (((bitboards[p] | bitboards[P]) & file_masks[square]) == 0)
				{
					// Give the semi open file penalty:
					score += open_file_score;
				}
				// King safety bonus:
				score -= count_bits(king_attacks[square] & occupancies[black]) * king_shield_bonus; */
				break;
			}
			// Pop LS1B:
			pop_bit(bitboard, square);
		}
	}
	/*
		Now in order to calculate interpolated score
		for a given game phase we use this formula
		(same for material and positional scores):

		(
			score_opening * game_phase_score +
			score_endgame * (opening_phase_score - game_phase_score)
		) / opening_phase_score

		E.g. the score for pawn on d4 at phase say 5000 would be
		interpolated_score = (12 * 5000 + (-7) * (6192 - 5000)) / 6192 = 8,342377261
	*/
	// Middlegame (interpolated) score:
	if (game_phase == middlegame)
	{
		// Interpolate the score:
		score = (score_opening * game_phase_score + score_endgame * (opening_phase_score - game_phase_score)) / opening_phase_score;
	}
	// Opening score:
	else if (game_phase == opening)
	{
		// Assign the score:
		score = score_opening;
	}
	// Endgame score:
	else
	{
		// Assign the score:
		score = score_endgame;
	}
	// Return final evaluation based on side:
	return (side == white) ? score : -score;
}

/******************************************************************************\
================================== SEARCH ======================================
\******************************************************************************/

/*

	Score layout:
	[-infinity, -mate_value ... -mate_score ... score ... mate_score ... mate_value, infinity]

*/

// Score bounds for the range of matting scores:
#define infinity 50000
#define mate_value 49000
#define mate_score 48000

/*

	Most valuable VICTIM & less valuable ATTACKER:

		(Victims) Pawn Knight Bishop   Rook  Queen   King
	(Attackers)
				Pawn   105    205    305    405    505    605
			Knight   104    204    304    404    504    604
			Bishop   103    203    303    403    503    603
				Rook   102    202    302    402    502    602
			 Queen   101    201    301    401    501    601
				King   100    200    300    400    500    600

*/

// MVV LVA [attacker][victim]
static int mvv_lva[12][12] = {
		105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605,
		104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604,
		103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603,
		102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602,
		101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601,
		100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600,

		105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605,
		104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604,
		103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603,
		102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602,
		101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601,
		100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600};

// Max reachable ply within a search:
#define max_ply 64

// Killer moves [id][ply]:
int killer_moves[2][max_ply];

// Hisotry moves [piece][square]:
int history_moves[12][64];

/*

	================================
				Triangular PV table
	--------------------------------
		PV line: e2e4 e7e5 g1f3 b8c6
	================================

				0    1    2    3    4    5

	0    m1   m2   m3   m4   m5   m6

	1    0    m2   m3   m4   m5   m6

	2    0    0    m3   m4   m5   m6

	3    0    0    0    m4   m5   m6

	4    0    0    0    0    m5   m6

	5    0    0    0    0    0    m6

*/

// PV length [ply]:
int pv_length[max_ply];

// PV table [ply][ply]:
int pv_table[max_ply][max_ply];

// Follow PV and score PV move:
int follow_pv, score_pv;

/******************************************************************************\
============================ TRANSPOSITION TABLE ===============================
\******************************************************************************/

// Number of hash table entries:
int hash_entries = 0;

// No hash entry found constant:
#define no_hash_entry 100000

// Transposition table hash flags:
#define hash_flag_exact 0
#define hash_flag_alpha 1
#define hash_flag_beta 2

// Transposition table data structure:
typedef struct
{
	U64 hash_key;
	int depth;
	int flag;
	int score;
} tt;

// Define transposition table instance:
tt *hash_table = NULL;

// Clear the transposition table:
void clear_hash_table()
{
	// Initialize hash table entry pointer:
	tt *hash_entry;
	// Loop over TT elements:
	for (hash_entry = hash_table; hash_entry < hash_table + hash_entries; hash_entry++)
	{
		// Initializate all the properties:
		hash_entry->hash_key = 0;
		hash_entry->depth = 0;
		hash_entry->flag = 0;
		hash_entry->score = 0;
	}
}

// Dynamically allocate memory for hash table:
void init_hash_table(int mb)
{
    // Initialize hash size:
    int hash_size = 0x100000 * mb;
    // Initialize number of hash entries:
    hash_entries =  hash_size / sizeof(tt);
    // Free hash table if not empty:
    if (hash_table != NULL)
    {      
        // Free hash table dynamic memory:
        free(hash_table);
    }
    // Allocate memory:
    hash_table = (tt *) malloc(hash_entries * sizeof(tt));
    // Memory allocation has failed:
    if (hash_table == NULL)
    {    
        // Try to allocate with half size:
        init_hash_table(mb / 2);
    }
    // Memory allocation succeeded:
    else
    {
        // Clear hash table:
        clear_hash_table();
    }    
}

// Write hash entry data:
static inline void write_hash_entry(int score, int depth, int hash_flag)
{
	/* Create a TT instance pointer to the hash entry
	responsible for storing a particular hash entry
	scoring data for the current board position if available:	*/
	tt *hash_entry = &hash_table[hash_key % hash_entries];
	// Store score independent from the actual path from
	// root node (position) to current node (position):
	if (score < -mate_score)
	{
		score -= ply;
	}
	if (score > mate_score)
	{
		score += ply;
	}
	// Fill the hash entry data:
	hash_entry->hash_key = hash_key;
	hash_entry->score = score;
	hash_entry->flag = hash_flag;
	hash_entry->depth = depth;
}

// Read hash entry data:
static inline int read_hash_entry(int alpha, int beta, int depth)
{
	/* Create a TT instance pointer to the hash entry
	responsible for storing a particular hash entry
	scoring data for the current board position if available:	*/
	tt *hash_entry = &hash_table[hash_key % hash_entries];
	// Make sure dealing with the exact position on the board:
	if (hash_entry->hash_key == hash_key)
	{
		// Make sure the depth matches exactly:
		if (hash_entry->depth >= depth)
		{
			// Extract stored score from TT entry:
			int score = hash_entry->score;
			// Retrieve score independent from the actual path from
			// root node (position) to current node (position):
			if (score < -mate_score)
			{
				score += ply;
			}
			if (score > mate_score)
			{
				score -= ply;
			}
			// Match the exact (PV node) score:
			if (hash_entry->flag == hash_flag_exact)
			{
				// Return the exact (PV node) score:
				return score;
			}
			// Match the alpha (fail-low node) score:
			if ((hash_entry->flag == hash_flag_alpha) && (score <= alpha))
			{
				// Return the alpha (fail-low node) score:
				return alpha;
			}
			// Match the beta (fail-high node) score:
			if ((hash_entry->flag == hash_flag_beta) && (score >= beta))
			{
				// Return the beta (fail-high node) score:
				return beta;
			}
		}
	}
	// Return the unkown value:
	return no_hash_entry;
}

// Enable PV move scoring:
static inline void enable_pv_scoring(moves *move_list)
{
	// Disable following PV:
	follow_pv = 0;
	// Loop over the moves within a move list:
	for (int count = 0; count < move_list->count; count++)
	{
		// Make sure we hit PV move:
		if (pv_table[0][ply] == move_list->moves[count])
		{
			// Enable move scoring:
			score_pv = 1;
			// Enable PV following:
			follow_pv = 1;
		}
	}
}

/*

	=======================
				Move ordering
	=======================

	1. PV move
	2. Captures in MVV/LVA
	3. 1st killer move
	4. 2nd killer move
	5. History moves
	6. Unsorted moves

*/

// Score moves function:
static inline int score_move(int move)
{
	// If PV move scoring is allowed:
	if (score_pv)
	{
		// Make sure we are dealing with PV move:
		if (pv_table[0][ply] == move)
		{
			// Disable PV score flag:
			score_pv = 0;
			// Give PV move the highest score to search it first:
			return 20000;
		}
	}
	// Score capture move:
	if (get_move_capture(move))
	{
		// Initialize target piece:
		int target_piece = P;
		// Pick up bitboard piece index ranges depending on side:
		int start_piece, end_piece;
		// White to move:
		if (side == white)
		{
			start_piece = p;
			end_piece = k;
		}
		// Black to move:
		else
		{
			start_piece = P;
			end_piece = K;
		}
		// Loop over the bitboards of the opposite side to move:
		for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
		{
			// If there is a piece on the target square:
			if (get_bit(bitboards[bb_piece], get_move_target(move)))
			{
				// Set up the target piece:
				target_piece = bb_piece;
				break;
			}
		}
		// Score move by MVV LVA lookup [source_piece][target_piece]:
		return mvv_lva[get_move_piece(move)][target_piece] + 10000;
	}
	// Score quiet move:
	else
	{
		// Score 1st killer move:
		if (killer_moves[0][ply] == move)
		{
			return 9000;
		}
		// Score 2nd killer move:
		else if (killer_moves[1][ply] == move)
		{
			return 8000;
		}
		// Score history moves:
		else
		{
			return history_moves[get_move_piece(move)][get_move_target(move)];
		}
	}
	// Return:
	return 0;
}

// Print the moves and its scores from a given move_list:
void print_moves_scores(moves *move_list)
{
	// Print the header:
	printf("+----------------+\n");
	printf("|  \033[0;33mMOVES SCORES\033[0;30m  |\n");
	printf("+--------+-------+\n");
	printf("|  \033[0;33mMove\033[0;30m  | \033[0;33mScore\033[0;30m |\n");
	printf("+--------+-------+\n");
	// Loop over the moves within a move list:
	for (int count = 0; count < move_list->count; count++)
	{
		// Print the move and score:
		printf("| \e[0m");
		if (!get_move_promoted(move_list->moves[count]))
		{
			printf(" ");
		}
		print_move(move_list->moves[count]);
		// Wich color to use for the move score:
		if (score_move(move_list->moves[count]) >= 500)
		{
			printf("\033[0;30m  | \033[0;31m%5d\033[0;30m |\n", score_move(move_list->moves[count]));
		}
		else if (score_move(move_list->moves[count]) >= 300)
		{
			printf("\033[0;30m  | \033[0;33m%5d\033[0;30m |\n", score_move(move_list->moves[count]));
		}
		else if (score_move(move_list->moves[count]) >= 100)
		{
			printf("\033[0;30m  | \033[0;32m%5d\033[0;30m |\n", score_move(move_list->moves[count]));
		}
		else
		{
			printf("\033[0;30m  | \033[0;30m%5d\033[0;30m |\n", score_move(move_list->moves[count]));
		}
		printf("+--------+-------+\e[0m\n");
	}
}

// Sort moves in descendent order:
static inline int sort_moves(moves *move_list)
{
	// Move scores array:
	int moves_scores[move_list->count];
	// Score all the moves on the moves list:
	for (int count = 0; count < move_list->count; count++)
	{
		// Score current move:
		moves_scores[count] = score_move(move_list->moves[count]);
	}
	// Loop over current move within a move list:
	for (int current_move = 0; current_move < move_list->count; current_move++)
	{
		// Loop over next move within a move list:
		for (int next_move = current_move + 1; next_move < move_list->count; next_move++)
		{
			// Compare current and next move scores:
			if (moves_scores[current_move] < moves_scores[next_move])
			{
				// Swap scores:
				int temp_score = moves_scores[current_move];
				moves_scores[current_move] = moves_scores[next_move];
				moves_scores[next_move] = temp_score;
				// Swap moves:
				int temp_move = move_list->moves[current_move];
				move_list->moves[current_move] = move_list->moves[next_move];
				move_list->moves[next_move] = temp_move;
			}
		}
	}
}

// Position repetition detection:
static inline int is_repetition()
{
	// Loop over repetition indices range:
	for (int index = 0; index < repetition_index; index++)
	{
		// If found the hash key equals the current:
		if (repetition_table[index] == hash_key)
		{
			// Return that a repetition was found:
			return 1;
		}
	}
	// If no repetition found:
	return 0;
}

// Quiescence serach:
static inline int quiescence(int alpha, int beta)
{
	// Every 2047 nodes:
	if ((nodes & 2047) == 0)
	{
		// "Listen" to the GUI/user input:
		communicate();
	}
	// Increment nodes count:
	nodes++;
	// Evaluate position:
	int evaluation = evaluate();
	// Fail-hard beta cutoff:
	if (evaluation >= beta)
	{
		// Node (moves) fails high:
		return beta;
	}
	// Found a better move:
	if (evaluation > alpha)
	{
		// PV node (move):
		alpha = evaluation;
	}
	// Create a move list instance:
	moves move_list[1];
	// Generate the moves:
	generate_moves(move_list);
	// Sort the moves in the move list:
	sort_moves(move_list);
	// Loop over moves within a movelist:
	for (int count = 0; count < move_list->count; count++)
	{
		// Preserve the board state:
		copy_board();
		// Increment the ply:
		ply++;
		// Increment repetition index and store hash key:
		repetition_index++;
		repetition_table[repetition_index] = hash_key;
		// Make sure to make only legal moves:
		if (make_move(move_list->moves[count], only_captures) == 0)
		{
			// Decrement ply:
			ply--;
			// Decrement repetition index:
			repetition_index--;
			// Skip to the next move:
			continue;
		}
		// Score current move:
		int score = -quiescence(-beta, -alpha);
		// Decrement ply:
		ply--;
		// Decrement repetition index:
		repetition_index--;
		// Take move back:
		restore_board();
		// If time is up:
		if (stopped == 1)
		{
			// Just return zero:
			return 0;
		}
		// Found a better move:
		if (score > alpha)
		{
			// PV node (move):
			alpha = score;
			// Fail-hard beta cutoff:
			if (score >= beta)
			{
				// Node (moves) fails high:
				return beta;
			}
		}
	}
	// Node (move) fails low:
	return alpha;
}

const int full_depth_moves = 4;
const int reduction_limit = 3;

// Negamax alpha beta search:
static inline int negamax(int alpha, int beta, int depth)
{
	// Initialize the current move score (from the static evaluation perspective):
	int score;
	// Define the hash flag:
	int hash_flag = hash_flag_alpha;
	// Position repetition occurs:
	if (ply && is_repetition())
	{
		// Return draw score:
		return 0;
	}
	// A hack from Pedro Catro to figure out if the current node is a PV node or not:
	int pv_node = beta - alpha > 1;
	// Reading the hash entry when not a root ply and not a PV node:
	if (ply && (score = read_hash_entry(alpha, beta, depth)) != no_hash_entry && pv_node == 0)
	{
		// The move has already been searched (hence has a value)
		// then return the score withou searching again:
		return score;
	}
	// Every 2047 nodes:
	if ((nodes & 2047) == 0)
	{
		// "Listen" to the GUI/user input:
		communicate();
	}
	// Initialize PV length:
	pv_length[ply] = ply;
	// Recursions escape condition:
	if (depth == 0)
	{
		// Run the quiescence search:
		return quiescence(alpha, beta);
	}
	// Too deep, hence there is an overflow of arrrays relying on max ply constant:
	if (ply > max_ply - 1)
	{
		// Evaluate position:
		return evaluate();
	}
	// Increment nodes count:
	nodes++;
	// Is king in check:
	int in_check = is_square_attacked(
			(side == white) ? get_ls1b_index(bitboards[K]) : get_ls1b_index(bitboards[k]),
			side ^ 1);
	// Increase search depth if the king has been exposed into a check:
	if (in_check)
	{
		// Increase the depth:
		depth++;
	}
	// Legal moves counter:
	int legal_moves = 0;
	// NULL move prunning:
	if (depth >= 3 && in_check == 0 && ply)
	{
		// Preserve board state:
		copy_board();
		// Increment ply:
		ply++;
		// Increment repetition index and store hash key:
		repetition_index++;
		repetition_table[repetition_index] = hash_key;
		// Hash enpassant if available:
		if (enpassant != no_sq)
		{
			hash_key ^= enpassant_keys[enpassant];
		}
		// Reset enpassant square:
		enpassant = no_sq;
		// Switch the side to move, literally giving the opponent an extra move:
		side ^= 1;
		// Hash the side:
		hash_key ^= side_key;
		// Search moves with reduced depth to find beta cutoffs:
		score = -negamax(-beta, -beta + 1, depth - 1 - 2);
		// Decrement ply:
		ply--;
		// Decrement repetition index:
		repetition_index--;
		// Restore the board state:
		restore_board();
		// If time is up:
		if (stopped == 1)
		{
			// Just return zero:
			return 0;
		}
		// Fail hard beta cutoff:
		if (score >= beta)
		{
			// Nove (move) fails high:
			return beta;
		}
	}
	// Create a move list instance:
	moves move_list[1];
	// Generate the moves:
	generate_moves(move_list);
	// If following PV line:
	if (follow_pv)
	{
		// Enable PV move scoring:
		enable_pv_scoring(move_list);
	}
	// Sort the moves in the move list:
	sort_moves(move_list);
	// Number of moves searched in a move list:
	int moves_searched = 0;
	// Loop over moves within a movelist:
	for (int count = 0; count < move_list->count; count++)
	{
		// Preserve the board state:
		copy_board();
		// Increment the ply:
		ply++;
		// Increment repetition index and store hash key:
		repetition_index++;
		repetition_table[repetition_index] = hash_key;
		// Make sure to make only legal moves:
		if (make_move(move_list->moves[count], all_moves) == 0)
		{
			// Decrement ply:
			ply--;
			// Decrement repetition index:
			repetition_index--;
			// Skip to the next move:
			continue;
		}
		// Increment legal moves:
		legal_moves++;
		// Full depth search:
		if (moves_searched == 0)
		{
			// Regular alpha beta search:
			score = -negamax(-beta, -alpha, depth - 1);
		}
		// LMR search:
		else
		{
			// Condition to consider LMR (late move reduction):
			if (moves_searched >= full_depth_moves && depth >= reduction_limit && in_check == 0 && get_move_capture(move_list->moves[count]) == 0 && get_move_promoted(move_list->moves[count]) == 0)
			{
				// Search current move with reduced depth:
				score = -negamax(-alpha - 1, -alpha, depth - 2);
			}
			// Hack to ensure that full-depth search is done:
			else
			{
				score = alpha + 1;
			}
			// Principle variation search PVS:
			if (score > alpha)
			{
				/* Once you've found a move with a score that is between alpha and beta,
				the rest of the moves are searched with the goal of proving that they are all bad.
				It's possible to do this a bit faster than a search that worries that one
				of the remaining moves might be good. */
				score = -negamax(-alpha - 1, -alpha, depth - 1);
				/* If the algorithm finds out that it was wrong, and that one of the
				subsequent moves was better than the first PV move, it has to search again,
				in the normal alpha-beta manner.  This happens sometimes, and it's a waste of time,
				but generally not often enough to counteract the savings gained from doing the
				"bad move proof" search referred to earlier. */
				if ((score > alpha) && (score < beta))
				{
					// Re-search the move that has failed to be proven bad:
					score = -negamax(-beta, -alpha, depth - 1);
				}
			}
		}
		// Decrement ply:
		ply--;
		// Decrement repetition index:
		repetition_index--;
		// Take move back:
		restore_board();
		// If time is up:
		if (stopped == 1)
		{
			// Just return zero:
			return 0;
		}
		// Increment the number of moves searched:
		moves_searched++;
		// Found a better move:
		if (score > alpha)
		{
			// Switch the hash flag from storing score for fail-low node
			// to the one storing score for PV node:
			hash_flag = hash_flag_exact;
			// On quiet moves:
			if (get_move_capture(move_list->moves[count]) == 0)
			{
				// Store history moves:
				history_moves[get_move_piece(move_list->moves[count])][get_move_target(move_list->moves[count])] += depth;
			}
			// PV node (move):
			alpha = score;
			// Write PV move:
			pv_table[ply][ply] = move_list->moves[count];
			// Loop over next ply line:
			for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
			{
				// Copy move from deeper ply into a current plys line:
				pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
			}
			// Adjust PV length:
			pv_length[ply] = pv_length[ply + 1];
			// Fail-hard beta cutoff:
			if (score >= beta)
			{
				// Store hash entry with the score equal to beta:
				write_hash_entry(beta, depth, hash_flag_beta);
				// On quiet moves:
				if (get_move_capture(move_list->moves[count]) == 0)
				{
					// Store killer moves:
					killer_moves[1][ply] = killer_moves[0][ply];
					killer_moves[0][ply] = move_list->moves[count];
				}
				// Node (moves) fails high:
				return beta;
			}
		}
	}
	// There is not any legal move to make in the current position:
	if (legal_moves == 0)
	{
		// King is in check:
		if (in_check)
		{
			// Return mating score (assuming closest distance to mate):
			return -mate_value + ply;
		}
		// King is not in check:
		else
		{
			// Return stalemate score:
			return 0;
		}
	}
	// Store hash entry with the score equal to alpha:
	write_hash_entry(alpha, depth, hash_flag);
	// Node (move) fails low:
	return alpha;
}

// Search position for the best move:
void search_position(int depth)
{
	// Define the best score variable:
	int score = 0;
	// Reset nodes counter:
	nodes = 0;
	// Reset "time is up" flag:
	stopped = 0;
	// Reset PV flags:
	follow_pv = 0;
	score_pv = 0;
	// Clear all the helper structures for search:
	memset(killer_moves, 0, sizeof(killer_moves));
	memset(history_moves, 0, sizeof(history_moves));
	memset(pv_table, 0, sizeof(pv_table));
	memset(pv_length, 0, sizeof(pv_length));
	// Define the initial alpha and beta window:
	int alpha = -infinity;
	int beta = infinity;
	// Iterative deepining:
	for (int current_depth = 1; current_depth <= depth; current_depth++)
	{
		// If time is up:
		if (stopped == 1)
		{
			// Stop calculating and return the best move so far:
			break;
		}
		// Enable follow PV flag:
		follow_pv = 1;
		// Find the best move with a given position:
		score = negamax(alpha, beta, current_depth);
		// Fell outside the window, so try again with a full-width window (and the same depth):
		if ((score <= alpha) || (score >= beta))
		{
			alpha = -infinity;
			beta = infinity;
			continue;
		}
		// Setup the window for the next iteration:
		alpha = score - 50;
		beta = score + 50;

		// PV is available:
		if (pv_length[0])
		{
			// Send the score to GUI through UCI command:
			if (score > -mate_value && score < -mate_score)
			{
				printf("info score mate %d depth %d nodes %lld time %d pv ", -(score + mate_value) / 2 - 1, current_depth, nodes, get_time_ms() - starttime);
			}
			else if (score > mate_score && score < mate_value)
			{
				printf("info score mate %d depth %d nodes %lld time %d pv ", (mate_value - score) / 2 + 1, current_depth, nodes, get_time_ms() - starttime);
			}
			else
			{
				printf("info score cp %d depth %d nodes %lld time %d pv ", score, current_depth, nodes, get_time_ms() - starttime);
			}
			// Loop over the moves within a PV line:
			for (int count = 0; count < pv_length[0]; count++)
			{
				// Print the move:
				print_move(pv_table[0][count]);
				printf(" ");
			}
			// Print a new line:
			printf("\n");
		}
	}
	// Best move command:
	printf("bestmove ");
	print_move(pv_table[0][0]);
	printf("\n");
}

/******************************************************************************\
==================================== UCI =======================================
\******************************************************************************/

// Parse the user/GUI move string input (e.g "e7e8q"):
int parse_move(char *move_string)
{
	// Inititalize a move list instance:
	moves move_list[1];
	// Generate the moves:
	generate_moves(move_list);
	// Parse the squares:
	int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;
	int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;
	// Loop over the moves within the move list:
	for (int move_count = 0; move_count < move_list->count; move_count++)
	{
		// Initialize move:
		int move = move_list->moves[move_count];
		// Make sure source and target squares are available within the generated move:
		if (source_square == get_move_source(move) && target_square == get_move_target(move))
		{
			// Initialize promoted piece:
			int promoted_piece = get_move_promoted(move);
			// Promoted piece is available:
			if (promoted_piece)
			{
				// Promoted to queen:
				if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q')
				{
					// Return legal move:
					return move;
				}
				// Promoted to rook:
				else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r')
				{
					// Return legal move:
					return move;
				}
				// Promoted to bishop:
				else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b')
				{
					// Return legal move:
					return move;
				}
				// Promoted to knight:
				else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n')
				{
					// Return legal move:
					return move;
				}
				// Continue the loop on possible wrong promotions (e.g "e7e8k"):
				continue;
			}
			// Return legal move:
			return move;
		}
	}
	// Return illegal move:
	return 0;
}

// Parse UCI <position> command:
void parse_position(char *command)
{
	// Shift pointer to the right where next token begins:
	command += 9;
	// Initialize pointer to the current char int the commmand string:
	char *current_char = command;
	// Parse UCI "startpos" command:
	if (strncmp(command, "startpos", 8) == 0)
	{
		// Initialize chess board with starting position:
		parse_fen(start_position);
	}
	// Parse UCI "fen" command:
	else
	{
		// Make sure FEN command is available whitin command string:
		current_char = strstr(command, "fen");
		// If FEN command is not available:
		if (current_char == NULL)
		{
			// Initialize chess board with starting position:
			parse_fen(start_position);
		}
		// If FEN command is available:
		else
		{
			// Shift pointer to the right where next token begins:
			current_char += 4;
			// Initialize chess board with fen string:
			parse_fen(current_char);
		}
	}
	// Parse moves after position:
	current_char = strstr(command, "moves");
	// If moves command is available:
	if (current_char != NULL)
	{
		// Shift pointer to the right where next token begins:
		current_char += 6;
		// Loop over the moves within a moves string:
		while (*current_char)
		{
			// Parse next move:
			int move = parse_move(current_char);
			// If no more moves:
			if (move == 0)
			{
				// Break out of the loop:
				break;
			}
			// Increment repetition index and store the hash key:
			repetition_index++;
			repetition_table[repetition_index] = hash_key;
			// Make move on the chess board:
			make_move(move, all_moves);
			// Move current char pointer to the end of current move:
			while (*current_char && *current_char != ' ')
			{
				current_char++;
			}
			// Go to the next move:
			current_char++;
		}
	}
	// Print the board:
	print_board();
}

// Reset time control variables:
void reset_time_control()
{
	// Reseting timing:
	quit = 0;
	movestogo = 30;
	movetime = -1;
	time = -1;
	inc = 0;
	starttime = 0;
	stoptime = 0;
	timeset = 0;
	stopped = 0;
}

// Parse UCI <go> command:
void parse_go(char *command)
{
	// Reset time control:
	reset_time_control();
	// Init parameters:
	int depth = -1;
	// Init argument:
	char *argument = NULL;
	// Infinite search:
	if ((argument = strstr(command, "infinite")))
	{
	}
	// Match UCI <binc> command:
	if ((argument = strstr(command, "binc")) && side == black)
	{
		// Parse black time increment:
		inc = atoi(argument + 5);
	}
	// Match UCI <winc> command:
	if ((argument = strstr(command, "winc")) && side == white)
	{
		// Parse white time increment:
		inc = atoi(argument + 5);
	}
	// Match UCI <wtime> command:
	if ((argument = strstr(command, "wtime")) && side == white)
	{
		// Parse white time limit:
		time = atoi(argument + 6);
	}
	// Match UCI <btime> command:
	if ((argument = strstr(command, "btime")) && side == black)
	{
		// Parse black time limit:
		time = atoi(argument + 6);
	}
	// Match UCI <movestogo> command:
	if ((argument = strstr(command, "movestogo")))
	{
		// Parse number of moves to go:
		movestogo = atoi(argument + 10);
	}
	// Match UCI <movetime> command:
	if ((argument = strstr(command, "movetime")))
	{
		// Parse amount of time allowed to spend to make a move:
		movetime = atoi(argument + 9);
	}
	// Match UCI <depth> command:
	if ((argument = strstr(command, "depth")))
	{
		// Parse search depth:
		depth = atoi(argument + 6);
	}
	// If move time is not available:
	if (movetime != -1)
	{
		// Set time equal to move time:
		time = movetime;
		// Set moves to go to 1:
		movestogo = 1;
	}
	// Initializate start time:
	starttime = get_time_ms();
	// Initializate search depth:
	depth = depth;
	// If time control is available:
	if (time != -1)
	{
		// Flag we're playing with time control:
		timeset = 1;
		// Set up timing:
		time /= movestogo;
		// "Illegal" (empty) move bug fix:
		if (time > 1500)
		{
			time -= 50;
		}
		// Initializate stotime:
		stoptime = starttime + time + inc;
	}
	// If depth is not available:
	if (depth == -1)
	{
		// Set depth to 64 plies (takes ages to complete...):
		depth = 64;
	}
	// Print debug info:
	printf("time:%d start:%u stop:%u depth:%d timeset:%d\n", time, starttime, stoptime, depth, timeset);
	// Search position:
	search_position(depth);
}

// Main UCI loop:
void uci_loop()
{
	// Max hash MB:
	int max_hash = 128;
	// Default MB size:
	int mb = 64;
	// Reset STDIN/STDOUT buffers:
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	// Define user/GUI input buffer:
	char input[2000];
	// Print engine information:
	printf("id name BBC %s\n", version);
	printf("id author CMK & Derlexy\n");
	printf("option name Hash type spin default 64 min 4 max %d\n", max_hash);
	printf("uciok\n");
	// Main loop:
	while (1)
	{
		// Reset user/GUI input:
		memset(input, 0, sizeof(input));
		// Make sure output reaches the GUI:
		fflush(stdout);
		// Get the user/GUI input:
		if (!fgets(input, 2000, stdin))
		{
			// Continue the loop:
			continue;
		}
		// Make sure input is available:
		if (input[0] == '\n')
		{
			// Continue the loop:
			continue;
		}
		// Parse UCI <isready> command:
		if (strncmp(input, "isready", 7) == 0)
		{
			// Print response:
			printf("readyok\n");
			// Continue the loop:
			continue;
		}
		// Parse UCI <position> command:
		else if (strncmp(input, "position", 8) == 0)
		{
			// Call parse position function:
			parse_position(input);
			// Clear hash table:
			clear_hash_table();
		}
		// Parse UCI <ucinewgame> command:
		else if (strncmp(input, "ucinewgame", 10) == 0)
		{
			// Call parse position function:
			parse_position("position startpos");
			// Clear hash table:
			clear_hash_table();
		}
		// Parse UCI <go> command:
		else if (strncmp(input, "go", 2) == 0)
		{
			// Call parse go function:
			parse_go(input);
		}
		// Parse UCI <quit> command:
		else if (strncmp(input, "quit", 4) == 0)
		{
			// Quit from the chess engine program execution:
			break;
		}
		// Parse UCI <uci> command:
		else if (strncmp(input, "uci", 3) == 0)
		{
			// Print engine information:
			printf("id name BBC %s\n", version);
			printf("id author CMK & Derlexy\n");
			printf("uciok\n");
		}
		// Setup the hash table MB size:
		else if (!strncmp(input, "setoption name Hash value ", 26))
		{
			// Initialize MB:
			sscanf(input, "%*s %*s %*s %*s %d", &mb);
			// Adjust MB if going under the allowed bound:
			if (mb < 4)
			{
				// Adjust MB:
				mb = 4;
			}
			// Adjust MB if going over the allowed bound:
			if (mb > max_hash)
			{
				// Adjust MB:
				mb = max_hash;
			}
			// Set hash table size in MB:
			printf("Set hash table size to %dMB\n", mb);
			// Initializate the hash table:
			init_hash_table(mb);
		}
	}
}

/******************************************************************************\
=============================== INITIALIZE ALL =================================
\******************************************************************************/

// Initialize all variables:
void init_all()
{
	// Initialize leaper pieces attacks:
	init_leapers_attacks();
	// Initialize sliders pieces attacks:
	init_sliders_attacks(bishop);
	init_sliders_attacks(rook);
	/*
	// Initialize magic numbers:
	init_magic_numbers();
	print_magic_numbers();
	*/
	// Initialize random keys for hashing purposes:
	init_random_keys();
	// Initalize evaluation masks:
	init_evaluation_masks();
	// Initializate hash table with 64 megabytes:
	init_hash_table(64);
}

/******************************************************************************\
================================= MAIN DRIVER ==================================
\******************************************************************************/

int main()
{
	// Initialize all variables:
	init_all();
	// Debug mode variable:
	int debug = 1;
	// If debug mode is enabled:
	if (debug)
	{
		// Parse FEN:
		parse_fen(start_position);
		// Print the board:
		print_board();
		// Print the score of current position:
		printf("Score: %d\n", evaluate());
		// Search position:
		// search_position(10);
	}
	// If debug mode is disabled:
	else
	{
		// Connect to the GUI:
		uci_loop();
	}
	// Free hash table memory on exit:
	free(hash_table);
	// Return:
	return 0;
}