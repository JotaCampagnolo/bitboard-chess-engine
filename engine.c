/******************************************************************************\
============================ BITBOARD CHESS ENGINE =============================

																			by

											 JotaCampagnolo & Code Monkey King

\******************************************************************************/

// System headers:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN64
#include <windows.h>
#else
#include <sys/time.h>
#endif

// Define bitboard data type:
#define U64 unsigned long long

// Defining FEN debug positions:
#define empty_board "8/8/8/8/8/8/8/8 b - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

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
	printf("+---------------------------------------+\n\n");
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
	printf("+---------------------------------------+\n\n");
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
	printf("+-------------------------------------------------------------------+\n");
}

// Preserve board state:
#define copy_board()                           \
	U64 bitboards_copy[12], occupancies_copy[3]; \
	int side_copy, enpassant_copy, castle_copy;  \
	memcpy(bitboards_copy, bitboards, 96);       \
	memcpy(occupancies_copy, occupancies, 24);   \
	side_copy = side, enpassant_copy = enpassant, castle_copy = castle;

// Restore board state:
#define restore_board()                      \
	memcpy(bitboards, bitboards_copy, 96);     \
	memcpy(occupancies, occupancies_copy, 24); \
	side = side_copy, enpassant = enpassant_copy, castle = castle_copy;

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
					break;
				}
			}
		}
		// Handling pawn promotions:
		if (promoted)
		{
			// Erase the pawn from the target square:
			pop_bit(bitboards[(side == white) ? P : p], target_square);
			// Set up promoted piece on chess board:
			set_bit(bitboards[promoted], target_square);
		}
		// Handling enpassant captures:
		if (enpassant_flag)
		{
			// Erase the pawn depending on side to move:
			(side == white) ? pop_bit(bitboards[p], target_square + 8) : pop_bit(bitboards[P], target_square - 8);
		}
		// Reseting the enpassant square:
		enpassant = no_sq;
		// Handling double pawn push:
		if (double_flag)
		{
			// Set the enpassant square depending on side to move:
			(side == white) ? (enpassant = target_square + 8) : (enpassant = target_square - 8);
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
				break;
			// White castles queen side:
			case (c1):
				// Move the H rook:
				pop_bit(bitboards[R], a1);
				set_bit(bitboards[R], d1);
				break;
			// Black castles king side:
			case (g8):
				// Move the H rook:
				pop_bit(bitboards[r], h8);
				set_bit(bitboards[r], f8);
				break;
			// Black castles queen side:
			case (c8):
				// Move the H rook:
				pop_bit(bitboards[r], a8);
				set_bit(bitboards[r], d8);
				break;
			}
		}
		// Update castling rights:
		castle &= castling_rights[source_square];
		castle &= castling_rights[target_square];
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

// Leaf nodes (number of positions reached during the test of the moves generator at a given depth):
long nodes;

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
	printf("Nodes: %ld\n", nodes);
	printf("Time: %ldms\n\n", get_time_ms() - start);
}

/******************************************************************************\
================================ EVALUATION ====================================
\******************************************************************************/

/*

	♟︎	=	 100			= ♟︎
	♞	 =	300			 = ♟︎ * 3
	♝	 =	301			 = ♟︎ * 3 + 1
	♜	 =	500			 = ♟︎ * 5
	♛	 =	900			 = ♟︎ * 9
	♚	 =	10000		 = ♟︎ * 100

*/

// Pieces material value:
int material_score[12] = {
		100,	 // white PAWN
		300,	 // white KNIGHT
		301,	 // white BISHOP
		500,	 // white ROOK
		900,	 // white QUEEN
		10000, // white KING
		-100,	 // black PAWN
		-300,	 // black KNIGHT
		-301,	 // black BISHOP
		-500,	 // black ROOK
		-900,	 // black QUEEN
		-10000 // black KING
};

// Pawn positional score:
const int pawn_scores[64] = {
		90, 90, 90, 90, 90, 90, 90, 90,
		30, 30, 30, 40, 40, 30, 30, 30,
		20, 20, 20, 30, 30, 30, 20, 20,
		10, 10, 10, 20, 20, 10, 10, 10,
		5, 5, 10, 20, 20, 5, 5, 5,
		0, 0, 0, 5, 5, 0, 0, 0,
		0, 0, 0, -10, -10, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};

// Knight positional score:
const int knight_scores[64] = {
		-5, 0, 0, 0, 0, 0, 0, -5,
		-5, 0, 0, 10, 10, 0, 0, -5,
		-5, 5, 20, 20, 20, 20, 5, -5,
		-5, 10, 20, 30, 30, 20, 10, -5,
		-5, 10, 20, 30, 30, 20, 10, -5,
		-5, 5, 20, 10, 10, 20, 5, -5,
		-5, 0, 0, 0, 0, 0, 0, -5,
		-5, -10, 0, 0, 0, 0, -10, -5};

// Bishop positional score:
const int bishop_scores[64] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 10, 10, 0, 0, 0,
		0, 0, 10, 20, 20, 10, 0, 0,
		0, 0, 10, 20, 20, 10, 0, 0,
		0, 10, 0, 0, 0, 0, 10, 0,
		0, 30, 0, 0, 0, 0, 30, 0,
		0, 0, -10, 0, 0, -10, 0, 0};

// Rook positional score:
const int rook_scores[64] = {
		50, 50, 50, 50, 50, 50, 50, 50,
		50, 50, 50, 50, 50, 50, 50, 50,
		0, 0, 10, 20, 20, 10, 0, 0,
		0, 0, 10, 20, 20, 10, 0, 0,
		0, 0, 10, 20, 20, 10, 0, 0,
		0, 0, 10, 20, 20, 10, 0, 0,
		0, 0, 10, 20, 20, 10, 0, 0,
		0, 0, 0, 20, 20, 0, 0, 0};

// King positional score:
const int king_scores[64] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 5, 5, 5, 5, 0, 0,
		0, 5, 5, 10, 10, 5, 5, 0,
		0, 5, 10, 20, 20, 10, 5, 0,
		0, 5, 10, 20, 20, 10, 5, 0,
		0, 0, 5, 10, 10, 5, 0, 0,
		0, 5, 5, -5, -5, 0, 5, 0,
		0, 0, 5, 0, -15, 0, 10, 0};

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

// Position evaluation:
static inline int evaluate()
{
	// Static evaluation score:
	int score = 0;
	// Current pieces bitboard copy:
	U64 bitboard;
	// Initialize piece and square:
	int piece, square;
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
			// Score material weights:
			score += material_score[piece];
			// Score positional weights:
			switch (piece)
			{
			// Evaluate white pieces:
			case P:
				score += pawn_scores[square];
				break;
			case N:
				score += knight_scores[square];
				break;
			case B:
				score += bishop_scores[square];
				break;
			case R:
				score += rook_scores[square];
				break;
			case K:
				score += king_scores[square];
				break;
			// Evaluate black pieces:
			case p:
				score -= pawn_scores[mirror_scores[square]];
				break;
			case n:
				score -= knight_scores[mirror_scores[square]];
				break;
			case b:
				score -= bishop_scores[mirror_scores[square]];
				break;
			case r:
				score -= rook_scores[mirror_scores[square]];
				break;
			case k:
				score -= king_scores[mirror_scores[square]];
				break;
			}
			// Pop LS1B:
			pop_bit(bitboard, square);
		}
	}
	// Return final evaluation based on side:
	return (side == white) ? score : -score;
}

/******************************************************************************\
================================== SEARCH ======================================
\******************************************************************************/

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

// Half move counter:
int ply;

// Best move:
int best_move;

// Score moves function:
static inline int score_move(int move)
{
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
		return mvv_lva[get_move_piece(move)][target_piece];
	}
	// Score quiet move:
	else
	{
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
	for (int count = 0; count <= move_list->count; count++)
	{
		// Print the move and score:
		printf("| \e[0m");
		if (!get_move_promoted(move_list->moves[count]))
		{
			printf(" ");
		}
		print_move(move_list->moves[count]);
		printf("\033[0;30m  | %s%5d\033[0;30m |\n", (score_move(move_list->moves[count]) > 0) ? "\033[0;32m" : "", score_move(move_list->moves[count]));
		printf("+--------+-------+\e[0m\n");
	}
}

// Quiescence serach:
static inline int quiescence(int alpha, int beta)
{
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
	// Loop over moves within a movelist:
	for (int count = 0; count < move_list->count; count++)
	{
		// Preserve the board state:
		copy_board();
		// Increment the ply:
		ply++;
		// Make sure to make only legal moves:
		if (make_move(move_list->moves[count], only_captures) == 0)
		{
			// Decrement ply:
			ply--;
			// Skip to the next move:
			continue;
		}
		// Score current move:
		int score = -quiescence(-beta, -alpha);
		// Decrement ply:
		ply--;
		// Take move back:
		restore_board();
		// Fail-hard beta cutoff:
		if (score >= beta)
		{
			// Node (moves) fails high:
			return beta;
		}
		// Found a better move:
		if (score > alpha)
		{
			// PV node (move):
			alpha = score;
		}
	}
	// Node (move) fails low:
	return alpha;
}

// Negamax alpha beta search:
static inline int negamax(int alpha, int beta, int depth)
{
	// Recursions escape condition:
	if (depth == 0)
	{
		// Run the quiescence search:
		return quiescence(alpha, beta);
	}
	// Increment nodes count:
	nodes++;
	// Is king in check:
	int in_check = is_square_attacked(
			(side == white) ? get_ls1b_index(bitboards[K]) : get_ls1b_index(bitboards[k]),
			side ^ 1);
	// Legal moves counter:
	int legal_moves = 0;
	// Best move so far:
	int best_so_far;
	// Old value of alpha:
	int old_alpha = alpha;
	// Create a move list instance:
	moves move_list[1];
	// Generate the moves:
	generate_moves(move_list);
	// Loop over moves within a movelist:
	for (int count = 0; count < move_list->count; count++)
	{
		// Preserve the board state:
		copy_board();
		// Increment the ply:
		ply++;
		// Make sure to make only legal moves:
		if (make_move(move_list->moves[count], all_moves) == 0)
		{
			// Decrement ply:
			ply--;
			// Skip to the next move:
			continue;
		}
		// Increment legal moves:
		legal_moves++;
		// Score current move:
		int score = -negamax(-beta, -alpha, depth - 1);
		// Decrement ply:
		ply--;
		// Take move back:
		restore_board();
		// Fail-hard beta cutoff:
		if (score >= beta)
		{
			// Node (moves) fails high:
			return beta;
		}
		// Found a better move:
		if (score > alpha)
		{
			// PV node (move):
			alpha = score;
			// If its a root move:
			if (ply == 0)
			{
				// Update the best move so far:
				best_so_far = move_list->moves[count];
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
			return -49000 + ply;
		}
		// King is not in check:
		else
		{
			// Return stalemate score:
			return 0;
		}
	}
	// Found a better move:
	if (old_alpha != alpha)
	{
		// Initialize best move:
		best_move = best_so_far;
	}
	// Node (move) fails low:
	return alpha;
}

// Search position for the best move:
void search_position(int depth)
{
	// Find the best move with a given position:
	int score = negamax(-50000, 50000, depth);
	// If there is a best move:
	if (best_move)
	{
		// Send the score to GUI through UCI command:
		printf("info score cp %d depth %d nodes %ld\n", score, depth, nodes);
		// Best move command:
		printf("bestmove ");
		print_move(best_move);
		printf("\n");
	}
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

// Parse UCI <go> command:
void parse_go(char *command)
{
	// Initialize depth:
	int depth = -1;
	// Initialize char pointer to the current depth argument:
	char *current_depth = NULL;
	// Handle fixed depth search:
	if (current_depth = strstr(command, "depth"))
	{
		// Convert string to integer:
		depth = atoi(current_depth + 6);
	}
	// Diferent time controls placeholder:
	else
	{
		depth = 6;
	}
	// Search position:
	search_position(depth);
}

// Main UCI loop:
void uci_loop()
{
	// Reset STDIN/STDOUT buffers:
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	// Define user/GUI input buffer:
	char input[2000];
	// Print engine information:
	printf("id name BBC\n");
	printf("id name JMC\n");
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
		}
		// Parse UCI <ucinewgame> command:
		else if (strncmp(input, "ucinewgame", 10) == 0)
		{
			// Call parse position function:
			parse_position("position startpos");
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
			printf("id name BBC\n");
			printf("id name JMC\n");
			printf("uciok\n");
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
		parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ");
		// Print the board:
		print_board();
		// Search position:
		// search_position(3);
		// Create a move list:
		moves move_list[1];
		// Generate moves:
		generate_moves(move_list);
		// Print moves scores:
		print_moves_scores(move_list);
	}
	// If debug mode is disabled:
	else
	{
		// Connect to the GUI:
		uci_loop();
	}
	// Return:
	return 0;
}