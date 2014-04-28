/**************************************************************************
 *                                                                        *
 * This code has been developed by Juice. This software is an             *
 * implementation of a part of one or more MPEG-4 Video tools as          *
 * specified in ISO/IEC 14496-2 standard.  Those intending to use this    *
 * software module in hardware or software products are advised that its  *
 * use may infringe existing patents or copyrights, and any such use      *
 * would be at such party's own risk.  The original developer of this     *
 * software module and his/her company, and subsequent editors and their  *
 * companies (including Project Mayo), will have no liability for use of  *
 * this software or modifications or derivatives thereof.                 *
 *                                                                        *
 * Project Mayo gives users of the Codec a license to this software       *
 * module or modifications thereof for use in hardware or software        *
 * products claiming conformance to the MPEG-4 Video Standard as          *
 * described in the Open DivX license.                                    *
 *                                                                        *
 * The complete Open DivX license can be found at                         *
 * http://www.projectmayo.com/opendivx/license.php                        *
 *                                                                        *
 **************************************************************************/
/**
*  Copyright (C) 2001 - Project Mayo
 *
 * Juice
 *
 * DivX Advanced Research Center <darc@projectmayo.com>
*
**/

#include "basic_prediction.h"

// Purpose: specialized basic motion compensation routines
void CopyBlock(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy;

	long *lpSrc = (long *) Src;
	long *lpDst = (long *) Dst;
	int lpStride = Stride >> 2;

	for (dy = 0; dy < 8; dy++) {
		lpDst[0] = lpSrc[0];
		lpDst[1] = lpSrc[1];
		lpSrc += lpStride;
		lpDst += lpStride;
	}
}
/**/
void CopyBlockHor(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 sub ebx, 7
		 xor ecx, ecx
		 mov edx, 8
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again10:
		 // 0
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 1
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 2
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 3
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 4
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 5
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 6
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 7
		 mov al, [esi]
		 mov cl, [esi+1]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al

		 add esi, ebx
		 add edi, ebx
		 dec edx
		 jnz start_again10
	}
}

/**/
void CopyBlockVer(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 xor ecx, ecx
		 mov edx, 8
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again10:
		 // 0
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 1
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 2
		 //xor eax, eax
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 3
		 //xor eax, eax
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 4
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 5
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 6
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 7
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al

		 add esi, ebx
		 sub esi, 7
		 add edi, ebx
		 sub edi, 7
		 dec edx
		 jnz start_again10
	}
}
/**/
void CopyBlockHorVer(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 0; dy < 8; dy++) {
		for (dx = 0; dx < 8; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1] + 
								Src[dx+Stride] + Src[dx+Stride+1] +2) >> 2; // horver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
void CopyBlockHorRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 sub ebx, 7
		 xor ecx, ecx
		 mov edx, 8
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again1:
		 // 0
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 1
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 2
		 //xor eax, eax
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 3
		 //xor eax, eax
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 4
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 5
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 6
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 7
		 mov al, [esi]
		 mov cl, [esi+1]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al

		 add esi, ebx
		 add edi, ebx
		 dec edx
		 jnz start_again1
	} 
}
/**/
void CopyBlockVerRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 xor ecx, ecx
		 mov edx, 8
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again1:
		 // 0
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 1
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 2
		 //xor eax, eax
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 3
		 //xor eax, eax
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 4
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 5
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 6
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 7
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al

		 add esi, ebx
		 sub esi, 7
		 add edi, ebx
		 sub edi, 7
		 dec edx
		 jnz start_again1
	}
}
/**/
void CopyBlockHorVerRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 0; dy < 8; dy++) {
		for (dx = 0; dx < 8; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1] + 
								Src[dx+Stride] + Src[dx+Stride+1] +1) >> 2; // horver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/** *** **/
void CopyMBlock(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 mov ebx, Stride
		 sub ebx, 12
		 mov edx, 16
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again:
		 mov eax, [esi]
		 mov [edi], eax
		 add esi, 4
		 add edi, 4

		 mov eax, [esi]
		 mov [edi], eax
		 add esi, 4
		 add edi, 4

		 mov eax, [esi]
		 mov [edi], eax
		 add esi, 4
		 add edi, 4

		 mov eax, [esi]
		 mov [edi], eax

		 add esi, ebx
		 add edi, ebx
		 dec edx
		 jnz start_again
	}
}
/**/
void CopyMBlockHor(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 sub ebx, 15
		 xor ecx, ecx
		 mov edx, 16
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again0:
		 // 0
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 1
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 2
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 3
		 //xor eax, eax
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 4
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 5
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 6
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 7
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 8
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 9
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 10
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 11
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 //12
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 //13 
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 //14
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 //15
		 mov al, [esi]
		 mov cl, [esi+1]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al

		 add esi, ebx
		 add edi, ebx
		 dec edx
		 jnz start_again0
	}
}
/**/
void CopyMBlockVer(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 xor ecx, ecx
		 mov edx, 16
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again0:
		 // 0
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 1
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 2
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 3
		 //xor eax, eax
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 4
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 5
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 6
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 7
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 8
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 9
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 10
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 11
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 //12
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 //13 
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 //14
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 //15
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 inc eax
		 shr eax, 1
		 mov [edi], al

		 add esi, ebx
		 sub esi, 15
		 add edi, ebx
		 sub edi, 15
		 dec edx
		 jnz start_again0
	}
}
/**/
void CopyMBlockHorVer(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 xor ecx, ecx
		 mov edx, 16
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again1:
		 // 0
		 //xor eax, eax
		 mov al, [esi]
		 mov cl, [esi+1]
		 add eax, ecx
		 mov cl, [esi+ebx]
		 add eax, ecx
		 mov cl, [esi+ebx+1]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi], al

		 // 1
		 //xor eax, eax
		 mov al, [esi+1]
		 mov cl, [esi+2]
		 add eax, ecx
		 mov cl, [esi+ebx+1]
		 add eax, ecx
		 mov cl, [esi+ebx+2]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+1], al

		 // 2
		 mov al, [esi+2]
		 mov cl, [esi+3]
		 add eax, ecx
		 mov cl, [esi+ebx+2]
		 add eax, ecx
		 mov cl, [esi+ebx+3]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+2], al

		 // 3
		 mov al, [esi+3]
		 mov cl, [esi+4]
		 add eax, ecx
		 mov cl, [esi+ebx+3]
		 add eax, ecx
		 mov cl, [esi+ebx+4]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+3], al

		 // 4
		 //xor eax, eax
		 mov al, [esi+4]
		 mov cl, [esi+5]
		 add eax, ecx
		 mov cl, [esi+ebx+4]
		 add eax, ecx
		 mov cl, [esi+ebx+5]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+4], al

		 // 5
		 //xor eax, eax
		 mov al, [esi+5]
		 mov cl, [esi+6]
		 add eax, ecx
		 mov cl, [esi+ebx+5]
		 add eax, ecx
		 mov cl, [esi+ebx+6]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+5], al

		 // 6
		 mov al, [esi+6]
		 mov cl, [esi+7]
		 add eax, ecx
		 mov cl, [esi+ebx+6]
		 add eax, ecx
		 mov cl, [esi+ebx+7]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+6], al

		 // 7
		 mov al, [esi+7]
		 mov cl, [esi+8]
		 add eax, ecx
		 mov cl, [esi+ebx+7]
		 add eax, ecx
		 mov cl, [esi+ebx+8]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+7], al

		 // 8
		 mov al, [esi+8]
		 mov cl, [esi+9]
		 add eax, ecx
		 mov cl, [esi+ebx+8]
		 add eax, ecx
		 mov cl, [esi+ebx+9]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+8], al

		 // 9
		 mov al, [esi+9]
		 mov cl, [esi+10]
		 add eax, ecx
		 mov cl, [esi+ebx+9]
		 add eax, ecx
		 mov cl, [esi+ebx+10]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+9], al

		 // 10
		 mov al, [esi+10]
		 mov cl, [esi+11]
		 add eax, ecx
		 mov cl, [esi+ebx+10]
		 add eax, ecx
		 mov cl, [esi+ebx+11]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+10], al

		 // 11
		 mov al, [esi+11]
		 mov cl, [esi+12]
		 add eax, ecx
		 mov cl, [esi+ebx+11]
		 add eax, ecx
		 mov cl, [esi+ebx+12]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+11], al

		 //12
		 mov al, [esi+12]
		 mov cl, [esi+13]
		 add eax, ecx
		 mov cl, [esi+ebx+12]
		 add eax, ecx
		 mov cl, [esi+ebx+13]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+12], al

		 //13 
		 //xor eax, eax
		 mov al, [esi+13]
		 mov cl, [esi+14]
		 add eax, ecx
		 mov cl, [esi+ebx+13]
		 add eax, ecx
		 mov cl, [esi+ebx+14]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+13], al

		 //14
		 mov al, [esi+14]
		 mov cl, [esi+15]
		 add eax, ecx
		 mov cl, [esi+ebx+14]
		 add eax, ecx
		 mov cl, [esi+ebx+15]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+14], al

		 //15
		 mov al, [esi+15]
		 mov cl, [esi+16]
		 add eax, ecx
		 mov cl, [esi+ebx+15]
		 add eax, ecx
		 mov cl, [esi+ebx+16]
		 add eax, ecx
		 add eax, 2
		 shr eax, 2
		 mov [edi+15], al

		 add esi, ebx
		 add edi, ebx
		 dec edx
		 jnz start_again1
	}
}
/**/
void CopyMBlockHorRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 sub ebx, 15
		 xor ecx, ecx
		 mov edx, 16
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again:
		 // 0
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 1
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 2
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 3
		 //xor eax, eax
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 4
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 5
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 6
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 7
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 8
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 9
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 10
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 // 11
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 //12
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 //13 
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 //14
		 mov al, [esi]
		 inc esi
		 mov cl, [esi]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc edi

		 //15
		 mov al, [esi]
		 mov cl, [esi+1]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al

		 add esi, ebx
		 add edi, ebx
		 dec edx
		 jnz start_again
	}
}
/**/
void CopyMBlockVerRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 xor ecx, ecx
		 mov edx, 16
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again:
		 // 0
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 1
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 2
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 3
		 //xor eax, eax
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 4
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 5
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 6
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 7
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 8
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 9
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 10
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 // 11
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 //12
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 //13 
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 //14
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al
		 inc esi
		 inc edi

		 //15
		 mov al, [esi]
		 mov cl, [esi+ebx]
		 add eax, ecx
		 shr eax, 1
		 mov [edi], al

		 add esi, ebx
		 sub esi, 15
		 add edi, ebx
		 sub edi, 15
		 dec edx
		 jnz start_again
	}
}
/**/
void CopyMBlockHorVerRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	_asm {
		 xor eax, eax
		 mov ebx, Stride
		 xor ecx, ecx
		 mov edx, 16
		 mov esi, dword ptr [Src]
		 mov edi, dword ptr [Dst]

start_again:
		 // 0
		 mov al, [esi]
		 mov cl, [esi+1]
		 add eax, ecx
		 mov cl, [esi+ebx]
		 add eax, ecx
		 mov cl, [esi+ebx+1]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi], al

		 // 1
		 mov al, [esi+1]
		 mov cl, [esi+2]
		 add eax, ecx
		 mov cl, [esi+ebx+1]
		 add eax, ecx
		 mov cl, [esi+ebx+2]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+1], al

		 // 2
		 mov al, [esi+2]
		 mov cl, [esi+3]
		 add eax, ecx
		 mov cl, [esi+ebx+2]
		 add eax, ecx
		 mov cl, [esi+ebx+3]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+2], al

		 // 3
		 mov al, [esi+3]
		 mov cl, [esi+4]
		 add eax, ecx
		 mov cl, [esi+ebx+3]
		 add eax, ecx
		 mov cl, [esi+ebx+4]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+3], al

		 // 4
		 mov al, [esi+4]
		 mov cl, [esi+5]
		 add eax, ecx
		 mov cl, [esi+ebx+4]
		 add eax, ecx
		 mov cl, [esi+ebx+5]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+4], al

		 // 5
		 mov al, [esi+5]
		 mov cl, [esi+6]
		 add eax, ecx
		 mov cl, [esi+ebx+5]
		 add eax, ecx
		 mov cl, [esi+ebx+6]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+5], al

		 // 6
		 mov al, [esi+6]
		 mov cl, [esi+7]
		 add eax, ecx
		 mov cl, [esi+ebx+6]
		 add eax, ecx
		 mov cl, [esi+ebx+7]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+6], al

		 // 7
		 mov al, [esi+7]
		 mov cl, [esi+8]
		 add eax, ecx
		 mov cl, [esi+ebx+7]
		 add eax, ecx
		 mov cl, [esi+ebx+8]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+7], al

		 // 8
		 mov al, [esi+8]
		 mov cl, [esi+9]
		 add eax, ecx
		 mov cl, [esi+ebx+8]
		 add eax, ecx
		 mov cl, [esi+ebx+9]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+8], al

		 // 9
		 mov al, [esi+9]
		 mov cl, [esi+10]
		 add eax, ecx
		 mov cl, [esi+ebx+9]
		 add eax, ecx
		 mov cl, [esi+ebx+10]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+9], al

		 // 10
		 mov al, [esi+10]
		 mov cl, [esi+11]
		 add eax, ecx
		 mov cl, [esi+ebx+10]
		 add eax, ecx
		 mov cl, [esi+ebx+11]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+10], al

		 // 11
		 mov al, [esi+11]
		 mov cl, [esi+12]
		 add eax, ecx
		 mov cl, [esi+ebx+11]
		 add eax, ecx
		 mov cl, [esi+ebx+12]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+11], al

		 //12
		 mov al, [esi+12]
		 mov cl, [esi+13]
		 add eax, ecx
		 mov cl, [esi+ebx+12]
		 add eax, ecx
		 mov cl, [esi+ebx+13]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+12], al

		 //13 
		 mov al, [esi+13]
		 mov cl, [esi+14]
		 add eax, ecx
		 mov cl, [esi+ebx+13]
		 add eax, ecx
		 mov cl, [esi+ebx+14]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+13], al

		 //14
		 mov al, [esi+14]
		 mov cl, [esi+15]
		 add eax, ecx
		 mov cl, [esi+ebx+14]
		 add eax, ecx
		 mov cl, [esi+ebx+15]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+14], al

		 //15
		 //xor eax, eax
		 mov al, [esi+15]
		 mov cl, [esi+16]
		 add eax, ecx
		 mov cl, [esi+ebx+15]
		 add eax, ecx
		 mov cl, [esi+ebx+16]
		 add eax, ecx
		 inc eax
		 shr eax, 2
		 mov [edi+15], al

		 add esi, ebx
		 add edi, ebx
		 dec edx
		 jnz start_again
	}
}
