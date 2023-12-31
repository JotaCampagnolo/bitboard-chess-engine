/******************************************************************************\
============================ BITBOARD CHESS ENGINE =============================

                                      by

                       JotaCampagnolo & Code Monkey King

\******************************************************************************/

// System headers:
#include <stdio.h>
#include <string.h>

// Define bitboard data type:
#define U64 unsigned long long

// Defining FEN debug positions:
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

// Enumerate board squares:
enum
{
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// Enumerate sides to move (colors):
enum { white, black, both };

// Enumerate rook and bishop:
enum { rook, bishop };

// Castle rights representation:
enum { wk = 1, wq = 2, bk = 4, bq = 8 };

// Encode pieces:
enum { P, N, B, R, Q, K, p, n, b, r, q, k };

const char *square_to_coordinates[] = {
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

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
  ['k'] = k
};

// Promoted pieces:
char promoted_pieces[] = {
	[Q] = 'q',
	[R] = 'r',
	[B] = 'b',
	[N] = 'n',
	[q] = 'q',
	[r] = 'r',
	[b] = 'b',
	[n] = 'n'
};

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
	n1 = (U64) (get_random_U32_number()) & 0xFFFF;
	n2 = (U64) (get_random_U32_number()) & 0xFFFF;
	n3 = (U64) (get_random_U32_number()) & 0xFFFF;
	n4 = (U64) (get_random_U32_number()) & 0xFFFF;
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
			if (!file) printf("| \e[0m%1d\033[0;30m |", 8 - rank);
			// Print bit state (either 1 or 0):
			get_bit(bitboard, square) ? printf(" \033[0;33m%d\033[0;30m |", 1) : printf(" %d\033[0;30m |", 0);
			if (file == 7) printf("   |");
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
          if(bb_piece >= p)
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
			if (file == 7) printf("   |");
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
			case 'K': castle |= wk; break;
			case 'Q': castle |= wq; break;
			case 'k': castle |= bk; break;
			case 'q': castle |= bq; break;
			case '-': break;
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
	6, 5, 5, 5, 5, 5, 5, 6
};
const int rook_relevant_bits[64] = {
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

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
	0x1004081002402ULL
};
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
	0x4010011029020020ULL
};

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
	if(!side)
	{
		// Generate pawn attacks:
		if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
		if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
	}
	// Black pawns:
	else
	{
		// Generate pawn attacks:
		if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
		if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
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
	if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
	if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
	if ((bitboard >> 10) & not_hg_file) attacks |= (bitboard >> 10);
	if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);
	if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
	if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
	if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
	if ((bitboard << 6) & not_hg_file) attacks |= (bitboard << 6);
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
	if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1);
	if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
	if (bitboard >> 8) attacks |= (bitboard >> 8);
	if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
	if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1);
	if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
	if (bitboard << 8) attacks |= (bitboard << 8);
	if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
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
	for(r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
	for(r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
	for(r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
	for(r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));
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
	for(r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
	for(r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
	for(f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
	for(f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));
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
	for(r = tr + 1, f = tf + 1; r < 8 && f < 8; r++, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & block) break;
	}
	for(r = tr - 1, f = tf + 1; r >= 0 && f < 8; r--, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & block) break;
	}
	for(r = tr + 1, f = tf - 1; r < 8 && f >= 0; r++, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & block) break;
	}
	for(r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & block) break;
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
	for(r = tr + 1; r < 8; r++)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if ((1ULL << (r * 8 + tf)) & block) break; 
	}
	for(r = tr - 1; r >= 0; r--)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if ((1ULL << (r * 8 + tf)) & block) break; 
	}
	for(f = tf + 1; f < 8; f++)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if ((1ULL << (tr * 8 + f)) & block) break; 
	}
	for(f = tf - 1; f >= 0; f--)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if ((1ULL << (tr * 8 + f)) & block) break;
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
		attacks[index] = bishop ?
			bishop_attacks_on_the_fly(square, occupancies[index])
			: rook_attacks_on_the_fly(square, occupancies[index]);
	}
	// Test magic numbers loop:
	for (int random_count = 0; random_count < 100000000; random_count++)
	{
		// Generate magic number candidate:
		U64 magic_number = generate_magic_number();
		// Skip invalid magic numbers:
		if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;
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
void print_magic_numbers(){
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
	if ((side == white) && (pawn_attacks[black][square] & bitboards[P])) return 1;
	// Attacked by black pawns:
	if ((side == black) && (pawn_attacks[white][square] & bitboards[p])) return 1;
	// Attacked by knights:
	if (knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n])) return 1;
	// Attacked by bishops:
	if (get_bishop_attacks(square, occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])) return 1;
	// Attacked by rooks:
	if (get_rook_attacks(square, occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])) return 1;
	// Attacked by queens:
	if (get_queen_attacks(square, occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])) return 1;
	// Attacked by kings:
	if (king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k])) return 1;
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
			if (!file) printf("| \e[0m%1d\033[0;30m |", 8 - rank);
			// Check wheter current square is attacked or not:
			is_square_attacked(square, side) ? printf(" \033[0;31m%s\033[0;30m |", "⚔") : printf(" %d\033[0;30m |", 0);
			if (file == 7) printf("   |");
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
	(source) |			\
	(target << 6) |		\
	(piece << 12) |		\
	(promoted << 16) |	\
	(capture << 20) |	\
	(double << 21) |	\
	(enpassant << 22) |	\
	(castling << 23)	\

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
typedef struct {
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
	printf("%s%s%c\n", square_to_coordinates[get_move_source(move)], square_to_coordinates[get_move_target(move)], promoted_pieces[get_move_promoted(move)]);
}

// Print move list:
void print_move_list(moves *move_list)
{
	printf("\n\033[0;30m+-------------------------------------------------------------------+\n");
	printf("|                       \033[0;33mPRINTING THE MOVE LIST\033[0;30m                      |\n");
	printf("+----------+----------+-----------+----------+-----------+----------+\n");
	printf("| \033[0;33mmove\033[0;30m     | \033[0;33mpiece\033[0;30m    | \033[0;33mcapture\033[0;30m   | \033[0;33mdouble\033[0;30m   | \033[0;33menpassant\033[0;30m | \033[0;33mcastling\033[0;30m |\n");
	printf("+----------+----------+-----------+----------+-----------+----------+\n");
	// Loop over moves within a move list:
	for (int move_count = 0; move_count < move_list->count; move_count++)
	{
		// Initialize move:
		int move = move_list->moves[move_count];
		// Print move:
		printf("| \e[0m%s%s%c\033[0;30m    | \e[0m%c → %s\033[0;30m    | %s       | %s      | %s       | %s      |\n", 
			square_to_coordinates[get_move_source(move)],
			square_to_coordinates[get_move_target(move)],
			promoted_pieces[get_move_promoted(move)],
			ascii_pieces[get_move_piece(move)], unicode_pieces[get_move_piece(move)],
			(get_move_capture(move) ? "\033[0;32myes\033[0;30m" : "no "),
			(get_move_double(move) ? "\033[0;32myes\033[0;30m" : "no "),
			(get_move_enpassant(move) ? "\033[0;32myes\033[0;30m" : "no "),
			(get_move_castling(move) ? "\033[0;32myes\033[0;30m" : "no ")
		);
	}
	printf("+----------+----------+-----------+----------+-----------+----------+\n");
	printf("|                        \033[0;33mMOVE LIST SIZE: \033[0;32m%3d\033[0;30m                        |\n", move_list->count);
	printf("+-------------------------------------------------------------------+\n");
}

// Generate all moves:
static inline void generate_moves()
{
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
							printf("%s%sQ → pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sR → pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sB → pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sN → pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
						}
						else
						{
							// One square pawn move:
							printf("%s%s → pawn move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							// Two squares pawn move:
							if ((source_square >= a2 && source_square <= h2) && !get_bit(occupancies[both], target_square - 8))
							{
								printf("%s%s → pawn rush\n", square_to_coordinates[source_square], square_to_coordinates[target_square - 8]);
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
							printf("%s%sQ → pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sR → pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sB → pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sN → pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
						}
						else
						{
							// One square pawn move:
							printf("%s%s → pawn capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
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
							printf("%s%s → pawn enpassant capture\n", square_to_coordinates[source_square], square_to_coordinates[target_enpassant]);
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
							printf("e1g1 → castling move\n");
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
							printf("e1c1 → castling move\n");
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
							printf("%s%sQ → pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sR → pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sB → pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sN → pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
						}
						else
						{
							// One square pawn move:
							printf("%s%s → pawn move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							// Two squares pawn move:
							if ((source_square >= a7 && source_square <= h7) && !get_bit(occupancies[both], target_square + 8))
							{
								printf("%s%s → pawn rush\n", square_to_coordinates[source_square], square_to_coordinates[target_square + 8]);
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
							printf("%s%sQ → pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sR → pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sB → pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
							printf("%s%sN → pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
						}
						else
						{
							// One square pawn move:
							printf("%s%s → pawn capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
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
							printf("%s%s → pawn enpassant capture\n", square_to_coordinates[source_square], square_to_coordinates[target_enpassant]);
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
							printf("e8g8 → castling move\n");
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
							printf("e8c8 → castling move\n");
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
						printf("%s%s → piece move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
					}
					// Capture moves:
					else
					{
						printf("%s%s → piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
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
						printf("%s%s → piece move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
					}
					// Capture moves:
					else
					{
						printf("%s%s → piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
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
						printf("%s%s → piece move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
					}
					// Capture moves:
					else
					{
						printf("%s%s → piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
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
						printf("%s%s → piece move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
					}
					// Capture moves:
					else
					{
						printf("%s%s → piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
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
						printf("%s%s → piece move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
					}
					// Capture moves:
					else
					{
						printf("%s%s → piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
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
	// Create move list:
	moves move_list[1];
	move_list->count = 0;
	// Add move:
	add_move(move_list, encode_move(e7, f8, P, Q, 1, 0, 0, 0));
	// Print move list:
	print_move_list(move_list);
	// Return:
	return 0;
}