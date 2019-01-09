#include "CoinPos.h"

Vec2 CoinPos::vectors[17] = { Vec2::ZERO };

int CoinPos::getSize(std::string str)
{
	char c = str.at(0);

	switch (c)
	{
	case 'O': return 12;
	case 'V': return 9;
	case 'E': return 17;
	case 'N': return 13;
	case 'B': return 16;
	case 'R': return 14;
	case 'A': return 12;
	case 'K': return 10;
	}
	return 0;
}


// Alphabet Positions

void CoinPos::getPos(std::string str)
{
	char c = str.at(0);

	switch (c)
	{
	case 'O':
		vectors[0] = { 0, 35 };		vectors[1] = { 0, 70 };		vectors[2] = { 0, 105 };
		vectors[3] = { 35, 0 };		vectors[4] = { 70, 0 };		vectors[5] = { 105, 0 };
		vectors[6] = { 140, 35 };	vectors[7] = { 140, 70 };	vectors[8] = { 140, 105 };
		vectors[9] = { 35, 140 };	vectors[10] = { 70, 140 };	vectors[11] = { 105, 140 };
		break;
	case 'V':
		vectors[0] = { 0, 140 };	vectors[1] = { 17.5, 105 };		vectors[2] = { 35, 70 };
		vectors[3] = { 52.5, 35 };	vectors[4] = { 70, 0 };			vectors[5] = { 87.5, 35 };
		vectors[6] = { 105, 70 };	vectors[7] = { 122.5, 105 };	vectors[8] = { 140, 140 };
		break;
	case 'E':
		vectors[0] = { 0, 0 };		vectors[1] = { 35, 0 };		vectors[2] = { 70, 0 };
		vectors[3] = { 105, 0 };	vectors[4] = { 140, 0 };	vectors[5] = { 0, 35 };
		vectors[6] = { 0, 70 };		vectors[7] = { 35, 70 };	vectors[8] = { 70, 70 };
		vectors[9] = { 105, 70 };	vectors[10] = { 140, 70 };	vectors[11] = { 0, 105 };
		vectors[12] = { 0, 140 };	vectors[13] = { 35, 140 };	vectors[14] = { 70, 140 };
		vectors[15] = { 105, 140 };	vectors[16] = { 140, 140 };
		break;
	case 'N':
		vectors[0] = { 0, 0 };		vectors[1] = { 0, 35 };		vectors[2] = { 0, 70 };
		vectors[3] = { 0, 105 };	vectors[4] = { 0, 140 };	vectors[5] = { 35, 105 };
		vectors[6] = { 70, 70 };	vectors[7] = { 105, 35 };	vectors[8] = { 140, 0 };
		vectors[9] = { 140, 35 };	vectors[10] = { 140, 70 };	vectors[11] = { 140, 105 };
		vectors[12] = { 140, 140 };
		break;
	case 'B':
		vectors[0] = { 0, 0 };		vectors[1] = { 35, 0 };		vectors[2] = { 70, 0 };
		vectors[3] = { 105, 0 };	vectors[4] = { 140, 35 };	vectors[5] = { 0, 35 };
		vectors[6] = { 0, 70 };		vectors[7] = { 35, 70 };	vectors[8] = { 70, 70 };
		vectors[9] = { 105, 70 };	vectors[10] = { 140, 105 };	vectors[11] = { 0, 105 };
		vectors[12] = { 0, 140 };	vectors[13] = { 35, 140 };	vectors[14] = { 70, 140 };
		vectors[15] = { 105, 140 };
		break;
	case 'R':
		vectors[0] = { 0, 0 };		vectors[1] = { 140, 0 };		vectors[2] = { 140, 35 };	
		vectors[3] = { 0, 35 };	
		vectors[4] = { 0, 70 };		vectors[5] = { 35, 70 };	vectors[6] = { 70, 70 };
		vectors[7] = { 105, 70 };	vectors[8] = { 140, 105 };	vectors[9] = { 0, 105 };
		vectors[10] = { 0, 140 };	vectors[11] = { 35, 140 };	vectors[12] = { 70, 140 };
		vectors[13] = { 105, 140 };
		break;
	case 'A':
		vectors[0] = { 0, 0 };		vectors[1] = { 140, 0 };		vectors[2] = { 0, 35 };
		vectors[3] = { 140, 35 };	vectors[4] = { 0, 70 };		vectors[5] = { 35, 70 };
		vectors[6] = { 70, 70 };	vectors[7] = { 105, 70 };	vectors[8] = { 140, 70 };
		vectors[9] = { 35, 105 };	vectors[10] = { 105, 105 };	vectors[11] = { 70, 140 };
		break;
	case 'K':
		vectors[0] = { 0, 0 };		vectors[1] = { 0, 35 };		vectors[2] = { 0, 70 };
		vectors[3] = { 0, 105 };	vectors[4] = { 0, 140 };	vectors[5] = { 35, 70 };
		vectors[6] = { 70, 35 };	vectors[7] = { 70, 105 };	vectors[8] = { 105, 0 };
		vectors[9] = { 105, 140 };
		break;
	}

}


