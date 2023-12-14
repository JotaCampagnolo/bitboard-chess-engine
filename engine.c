/******************************************************************************\
============================ BITBOARD CHESS ENGINE =============================

                                      by

                       JotaCampagnolo & Code Monkey King

\******************************************************************************/

// System headers:
#include <stdio.h>

// Define bitboard data type:
#define U64 unsigned long long

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
	a1, b1, c1, d1, e1, f1, g1, h1
};

/******************************************************************************\
============================== BIT MANIPULATION ================================
\******************************************************************************/

// Bit set/get/pop macros:
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)

// Printing bitboard:
void print_bitboard(U64 bitboard)
{
	// Print horizontal top border:
	printf("\n");
	printf("+---------------------------------------+\n");
	printf("|                                       |\n");
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
			if (!file) printf("| %1d |", 8 - rank);
			// Print bit state (either 1 or 0):
			printf(" %d |", get_bit(bitboard, square) ? 1 : 0 );
			if (file == 7) printf("   |");
		}
		// Print a new line after every rank:
		printf("\n");
	}
	// Print horizontal bottom border:
	printf("|   +---+---+---+---+---+---+---+---+   |\n");
	printf("|     a   b   c   d   e   f   g   h     |\n");
	printf("+---------------------------------------+\n\n");
	// Print the bitboard as unsigned decimal number:
	printf("Bitboard state: %llu\n\n", bitboard);
}

/******************************************************************************\
================================= MAIN DRIVER ==================================
\******************************************************************************/

int main()
{
	// Defining the bitboard:
	U64 bitboard = 0ULL;
	// Adding some bits:
	set_bit(bitboard, d2);
	set_bit(bitboard, e2);
	set_bit(bitboard, f2);
	print_bitboard(bitboard);
	// Removing some bits:
	pop_bit(bitboard, e2);
	print_bitboard(bitboard);
	return 0;
}