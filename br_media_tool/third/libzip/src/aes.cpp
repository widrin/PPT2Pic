
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "aes.h"



static BYTE Sbox[16*16]=
{// populate the Sbox matrix
	/* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
	/*0*/  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	/*1*/  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	/*2*/  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	/*3*/  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	/*4*/  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	/*5*/  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	/*6*/  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	/*7*/  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	/*8*/  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	/*9*/  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	/*a*/  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	/*b*/  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	/*c*/  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	/*d*/  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	/*e*/  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	/*f*/  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static BYTE iSbox[16*16]=
{
	// populate the iSbox matrix
	/* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
	/*0*/  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	/*1*/  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	/*2*/  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	/*3*/  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	/*4*/  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	/*5*/  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	/*6*/  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	/*7*/  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	/*8*/  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	/*9*/  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	/*a*/  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	/*b*/  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	/*c*/  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	/*d*/  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	/*e*/  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	/*f*/  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};
static BYTE Rcon[11*4]=
{
		0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00,
		0x02, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x00, 0x00,
		0x08, 0x00, 0x00, 0x00,
		0x10, 0x00, 0x00, 0x00,
		0x20, 0x00, 0x00, 0x00,
		0x40, 0x00, 0x00, 0x00,
		0x80, 0x00, 0x00, 0x00,
		0x1b, 0x00, 0x00, 0x00,
		0x36, 0x00, 0x00, 0x00
};


CAes::CAes()
{

}

CAes::~CAes()
{

}


CAes::CAes(INT keySize, BYTE* keyBytes)
{
	SetNbNkNr(keySize);//��ʼ��
	memcpy(key,keyBytes,keySize);
	KeyExpansion();
	// expand the seed key INTo a key schedule and store in w
}  // Aes constructor


/* convert hex string to int */
INT CAes::htoi(CHAR s[3])
{
	const char *digits="0123456789ABCDEF";
	if (islower(s[0]))
		s[0]=toupper(s[0]);
	if (islower(s[1]))
		s[1]=toupper(s[1]);
	return (INT)(16*(strchr(digits, s[0]) - strchr(digits,'0')) + (strchr(digits,s[1])-strchr(digits,'0')));
}


VOID CAes::Cipher(BYTE* input, BYTE* output)  // encipher 16-bit input
{
	// state = input
	memset(&State[0][0],0,16);
	INT i;
	for (i = 0; i < (4 * Nb); i++)//
	{
        State[i % 4][ i / 4] = input[i];
	}

	AddRoundKey(0);

	for (INT round = 1; round <= (Nr - 1); round++)  // main round loop
	{
        SubBytes();
        ShiftRows();
        MixColumns();
        AddRoundKey(round);
	}  // main round loop

	SubBytes();
	ShiftRows();
	AddRoundKey(Nr);

	// output = state
	for (i = 0; i < (4 * Nb); i++)
	{
        output[i] =  State[i % 4][ i / 4];
	}

}  // Cipher()

VOID CAes::InvCipher(BYTE * input, BYTE * output)  // decipher 16-bit input
{
	// state = input
	INT i;
	memset(&State[0][0],0,16);
	for (i = 0; i < (4 * Nb); i++)
	{
		State[i % 4][ i / 4] = input[i];
	}

	AddRoundKey(Nr);

	for (INT round = Nr-1; round >= 1; round--)  // main round loop
	{
        InvShiftRows();
        InvSubBytes();
        AddRoundKey(round);
        InvMixColumns();
	}  // end main round loop for InvCipher

	InvShiftRows();
	InvSubBytes();
	AddRoundKey(0);

	// output = state
	for (i = 0; i < (4 * Nb); i++)
	{
        output[i] =  State[i % 4][ i / 4];
	}

}  // InvCipher()

VOID CAes::SetNbNkNr(INT keyS)
{
	Nb = 4;     // block size always = 4 words = 16 BYTEs = 128 bits for AES
	Nk = 4;
	if (keyS == Bits128)
	{
		Nk = 4;   // key size = 4 words = 16 BYTEs = 128 bits
		Nr = 10;  // rounds for algorithm = 10
	}
	else if (keyS == Bits192)
	{
		Nk = 6;   // 6 words = 24 BYTEs = 192 bits
		Nr = 12;
	}
	else if (keyS == Bits256)
	{
		Nk = 8;   // 8 words = 32 BYTEs = 256 bits
		Nr = 14;
	}
}  // SetNbNkNr()



VOID CAes::AddRoundKey(INT round)
{
	INT r,c;
	for (r = 0; r < 4; r++)
	{
        for (c = 0; c < 4; c++)
        {//w:    4*x+y
			State[r][c]=(BYTE)((INT)State[r][c]^(INT)w[4*((round*4)+c)+r]);
        }
	}
}  // AddRoundKey()

VOID CAes::SubBytes()
{
	INT r,c;
	for (r = 0; r < 4; r++)
	{
        for (c = 0; c < 4; c++)
        {
			State[r][c] =  Sbox[ 16*(State[r][c] >> 4)+ ( State[r][c] & 0x0f) ];
        }
	}
}  // SubBytes

VOID CAes::InvSubBytes()
{
	INT r,c;
	for (r = 0; r < 4; r++)
	{
        for (c = 0; c < 4; c++)
        {
			State[r][c] =  iSbox[ 16*( State[r][c] >> 4)+( State[r][c] & 0x0f) ];
        }
	}
}  // InvSubBytes

VOID CAes::ShiftRows()
{
	BYTE temp[4*4];
	INT r,c;
	for (r = 0; r < 4; r++)  // copy State INTo temp[]
	{
        for (c = 0; c < 4; c++)
        {
			temp[4*r+c] =  State[r][c];
        }
	}
	//??
	for (r = 1; r < 4; r++)  // shift temp INTo State
	{
        for (c = 0; c < 4; c++)
        {
			State[r][c] = temp[ 4*r+ (c + r) % Nb ];
        }
	}
}  // ShiftRows()

VOID CAes::InvShiftRows()
{
	BYTE temp[4*4];
	INT r,c;
	for (r = 0; r < 4; r++)  // copy State INTo temp[]
	{
        for (c = 0; c < 4; c++)
        {
			temp[4*r+c] =  State[r][c];
        }
	}
	for (r = 1; r < 4; r++)  // shift temp INTo State
	{
        for (c = 0; c < 4; c++)
        {
			State[r][ (c + r) % Nb ] = temp[4*r+c];
        }
	}
}  // InvShiftRows()

VOID CAes::MixColumns()
{
	BYTE temp[4*4];
	INT r,c;
	for (r = 0; r < 4; r++)  // copy State INTo temp[]
	{
        for (c = 0; c < 4; c++)
        {
			temp[4*r+c] =  State[r][c];
        }
	}

	for (c = 0; c < 4; c++)
	{
		State[0][c] = (BYTE) ( (INT)gfmultby02(temp[0+c]) ^ (INT)gfmultby03(temp[4*1+c]) ^
			(INT)gfmultby01(temp[4*2+c]) ^ (INT)gfmultby01(temp[4*3+c]) );
		State[1][c] = (BYTE) ( (INT)gfmultby01(temp[0+c]) ^ (INT)gfmultby02(temp[4*1+c]) ^
			(INT)gfmultby03(temp[4*2+c]) ^ (INT)gfmultby01(temp[4*3+c]) );
		State[2][c] = (BYTE) ( (INT)gfmultby01(temp[0+c]) ^ (INT)gfmultby01(temp[4*1+c]) ^
			(INT)gfmultby02(temp[4*2+c]) ^ (INT)gfmultby03(temp[4*3+c]) );
		State[3][c] = (BYTE) ( (INT)gfmultby03(temp[0+c]) ^ (INT)gfmultby01(temp[4*1+c]) ^
			(INT)gfmultby01(temp[4*2+c]) ^ (INT)gfmultby02(temp[4*3+c]) );
	}
}  // MixColumns

VOID CAes::InvMixColumns()
{
	BYTE temp[4*4];
	INT r,c;
	for (r = 0; r < 4; r++)  // copy State INTo temp[]
	{
        for (c = 0; c < 4; c++)
        {
			temp[4*r+c] =  State[r][c];
        }
	}

	for (c = 0; c < 4; c++)
	{
		State[0][c] = (BYTE) ( (INT)gfmultby0e(temp[c]) ^ (INT)gfmultby0b(temp[4+c]) ^
			(INT)gfmultby0d(temp[4*2+c]) ^ (INT)gfmultby09(temp[4*3+c]) );
		State[1][c] = (BYTE) ( (INT)gfmultby09(temp[c]) ^ (INT)gfmultby0e(temp[4+c]) ^
			(INT)gfmultby0b(temp[4*2+c]) ^ (INT)gfmultby0d(temp[4*3+c]) );
		State[2][c] = (BYTE) ( (INT)gfmultby0d(temp[c]) ^ (INT)gfmultby09(temp[4+c]) ^
			(INT)gfmultby0e(temp[4*2+c]) ^ (INT)gfmultby0b(temp[4*3+c]) );
		State[3][c] = (BYTE) ( (INT)gfmultby0b(temp[c]) ^ (INT)gfmultby0d(temp[4+c]) ^
			(INT)gfmultby09(temp[4*2+c]) ^ (INT)gfmultby0e(temp[4*3+c]) );
	}
}  // InvMixColumns

BYTE CAes::gfmultby01(BYTE b)
{
	return b;
}

BYTE CAes::gfmultby02(BYTE b)
{
	if (b < 0x80)
        return (BYTE)(INT)(b <<1);
	else
        return (BYTE)( (INT)(b << 1) ^ (INT)(0x1b) );
}

BYTE CAes::gfmultby03(BYTE b)
{
	return (BYTE) ( (INT)gfmultby02(b) ^ (INT)b );
}

BYTE CAes::gfmultby09(BYTE b)
{
	return (BYTE)( (INT)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(INT)b );
}

BYTE CAes::gfmultby0b(BYTE b)
{
	return (BYTE)( (INT)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(INT)gfmultby02(b) ^
		(INT)b );
}

BYTE CAes::gfmultby0d(BYTE b)
{
	return (BYTE)( (INT)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(INT)gfmultby02(gfmultby02(b)) ^
		(INT)(b) );
}

BYTE CAes::gfmultby0e(BYTE b)
{
	return (BYTE)( (INT)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(INT)gfmultby02(gfmultby02(b)) ^
		(INT)gfmultby02(b) );
}

VOID CAes::KeyExpansion()
{
	BYTE result[4],result2[4];
    memset(w,0,16*15);
	INT row;
	for (row = 0; row < Nk; row++)//Nk=4,6,8
	{
		w[4*row+0] =  key[4*row];
		w[4*row+1] =  key[4*row+1];
		w[4*row+2] =  key[4*row+2];
		w[4*row+3] =  key[4*row+3];
	}

	BYTE temp[4];

	for (row = Nk; row < Nb * (Nr+1); row++)
	{
        temp[0] =  w[4*(row-1)+0];
		temp[1] =  w[4*(row-1)+1];
        temp[2] =  w[4*(row-1)+2];
		temp[3] =  w[4*(row-1)+3];

        if (row % Nk == 0)
        {
			RotWord(temp,result);
			SubWord(result,result2);
			memcpy(temp,result2,4);//

			temp[0] = (BYTE)( (INT)temp[0] ^ (INT) Rcon[4*(row/Nk)+0] );
			temp[1] = (BYTE)( (INT)temp[1] ^ (INT) Rcon[4*(row/Nk)+1] );
			temp[2] = (BYTE)( (INT)temp[2] ^ (INT) Rcon[4*(row/Nk)+2] );
			temp[3] = (BYTE)( (INT)temp[3] ^ (INT) Rcon[4*(row/Nk)+3] );
        }
        else if ( Nk > 6 && (row % Nk == 4) )
        {
			SubWord(temp,result);
			memcpy(temp,result,4);
        }
        // w[row] = w[row-Nk] xor temp
		w[4*row+0] = (BYTE) ( (INT) w[4*(row-Nk)+0] ^ (INT)temp[0] );
		w[4*row+1] = (BYTE) ( (INT) w[4*(row-Nk)+1] ^ (INT)temp[1] );
		w[4*row+2] = (BYTE) ( (INT) w[4*(row-Nk)+2] ^ (INT)temp[2] );
		w[4*row+3] = (BYTE) ( (INT) w[4*(row-Nk)+3] ^ (INT)temp[3] );
	}  // for loop
}  // KeyExpansion()

VOID CAes::SubWord(BYTE * word,BYTE* result)
{
	result[0] =  Sbox[ 16*(word[0] >> 4)+ (word[0] & 0x0f) ];
	result[1] =  Sbox[ 16*(word[1] >> 4)+ (word[1] & 0x0f) ];
	result[2] =  Sbox[ 16*(word[2] >> 4)+ (word[2] & 0x0f) ];
	result[3] =  Sbox[ 16*(word[3] >> 4)+ (word[3] & 0x0f) ];
}

VOID CAes::RotWord(BYTE * word,BYTE *result)
{
	result[0] = word[1];
	result[1] = word[2];
	result[2] = word[3];
	result[3] = word[0];
}




