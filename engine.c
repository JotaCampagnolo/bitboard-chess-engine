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

// Enumerate sides to move (colors):
enum { white, black };

/******************************************************************************\
============================== BIT MANIPULATION ================================
\******************************************************************************/

// Bit set/get/pop macros:
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)

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

// Printing bitboard:
void print_bitboard(U64 bitboard)
{
	// Print horizontal top border:
	printf("\n");
	printf("\033[0;30m+---------------------------------------+\n");
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
	printf("\e[0mBitboard state: %llu\n\n", bitboard);
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

// Pawn attacks table [side][square]:
U64 pawn_attacks[2][64];

// Knight attacks table [square]:
U64 knight_attacks[64];

// King attacks table [square]:
U64 king_attacks[64];

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

/******************************************************************************\
================================= MAIN DRIVER ==================================
\******************************************************************************/

int main()
{
	// Initialize leaper pieces attacks:
	init_leapers_attacks();
	// Initialize occupancy bitboard:
	U64 block = 0ULL;
	set_bit(block, d7);
	set_bit(block, d2);
	set_bit(block, g4);
	print_bitboard(block);
	printf("Index: %d\nCoordinate: %s\n", get_ls1b_index(block), square_to_coordinates[get_ls1b_index(block)]);

	return 0;
}