/*
 *  PCM Interface - misc routines
 *  Copyright (c) 1998 by Jaroslav Kysela <perex@suse.cz>
 *
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation; either version 2.1 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <byteswap.h>
#include "pcm_local.h"


/**
 * \brief Return sign info for a PCM sample linear format
 * \param format Format
 * \return 0 unsigned, 1 signed, a negative error code if format is not linear
 */
int snd_pcm_format_signed(snd_pcm_format_t format)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_S8:
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S16_BE:
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S24_BE:
	case SNDRV_PCM_FORMAT_S32_LE:
	case SNDRV_PCM_FORMAT_S32_BE:
		return 1;
	case SNDRV_PCM_FORMAT_U8:
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_U16_BE:
	case SNDRV_PCM_FORMAT_U24_LE:
	case SNDRV_PCM_FORMAT_U24_BE:
	case SNDRV_PCM_FORMAT_U32_LE:
	case SNDRV_PCM_FORMAT_U32_BE:
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * \brief Return sign info for a PCM sample linear format
 * \param format Format
 * \return 0 signed, 1 unsigned, a negative error code if format is not linear
 */
int snd_pcm_format_unsigned(snd_pcm_format_t format)
{
	int val;

	val = snd_pcm_format_signed(format);
	if (val < 0)
		return val;
	return !val;
}

/**
 * \brief Return linear info for a PCM sample format
 * \param format Format
 * \return 0 non linear, 1 linear
 */
int snd_pcm_format_linear(snd_pcm_format_t format)
{
	return snd_pcm_format_signed(format) >= 0;
}

/**
 * \brief Return float info for a PCM sample format
 * \param format Format
 * \return 0 non float, 1 float
 */
int snd_pcm_format_float(snd_pcm_format_t format)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_FLOAT_LE:
	case SNDRV_PCM_FORMAT_FLOAT_BE:
	case SNDRV_PCM_FORMAT_FLOAT64_LE:
	case SNDRV_PCM_FORMAT_FLOAT64_BE:
		return 1;
	default:
		return 0;
	}
}

/**
 * \brief Return endian info for a PCM sample format
 * \param format Format
 * \return 0 big endian, 1 little endian, a negative error code if endian independent
 */
int snd_pcm_format_little_endian(snd_pcm_format_t format)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_U24_LE:
	case SNDRV_PCM_FORMAT_S32_LE:
	case SNDRV_PCM_FORMAT_U32_LE:
	case SNDRV_PCM_FORMAT_FLOAT_LE:
	case SNDRV_PCM_FORMAT_FLOAT64_LE:
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_LE:
		return 1;
	case SNDRV_PCM_FORMAT_S16_BE:
	case SNDRV_PCM_FORMAT_U16_BE:
	case SNDRV_PCM_FORMAT_S24_BE:
	case SNDRV_PCM_FORMAT_U24_BE:
	case SNDRV_PCM_FORMAT_S32_BE:
	case SNDRV_PCM_FORMAT_U32_BE:
	case SNDRV_PCM_FORMAT_FLOAT_BE:
	case SNDRV_PCM_FORMAT_FLOAT64_BE:
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_BE:
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * \brief Return endian info for a PCM sample format
 * \param format Format
 * \return 0 little endian, 1 big endian, a negative error code if endian independent
 */
int snd_pcm_format_big_endian(snd_pcm_format_t format)
{
	int val;

	val = snd_pcm_format_little_endian(format);
	if (val < 0)
		return val;
	return !val;
}

/**
 * \brief Return endian info for a PCM sample format
 * \param format Format
 * \return 0 swapped, 1 CPU endian, a negative error code if endian independent
 */
int snd_pcm_format_cpu_endian(snd_pcm_format_t format)
{
#ifdef SNDRV_LITTLE_ENDIAN
	return snd_pcm_format_little_endian(format);
#else
	return snd_pcm_format_big_endian(format);
#endif
}

/**
 * \brief Return nominal bits per a PCM sample
 * \param format Sample format
 * \return bits per sample, a negative error code if not applicable
 */
int snd_pcm_format_width(snd_pcm_format_t format)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_S8:
	case SNDRV_PCM_FORMAT_U8:
		return 8;
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S16_BE:
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_U16_BE:
		return 16;
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S24_BE:
	case SNDRV_PCM_FORMAT_U24_LE:
	case SNDRV_PCM_FORMAT_U24_BE:
		return 24;
	case SNDRV_PCM_FORMAT_S32_LE:
	case SNDRV_PCM_FORMAT_S32_BE:
	case SNDRV_PCM_FORMAT_U32_LE:
	case SNDRV_PCM_FORMAT_U32_BE:
	case SNDRV_PCM_FORMAT_FLOAT_LE:
	case SNDRV_PCM_FORMAT_FLOAT_BE:
		return 32;
	case SNDRV_PCM_FORMAT_FLOAT64_LE:
	case SNDRV_PCM_FORMAT_FLOAT64_BE:
		return 64;
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_LE:
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_BE:
		return 24;
	case SNDRV_PCM_FORMAT_MU_LAW:
	case SNDRV_PCM_FORMAT_A_LAW:
		return 8;
	case SNDRV_PCM_FORMAT_IMA_ADPCM:
		return 4;
	default:
		return -EINVAL;
	}
}

/**
 * \brief Return bits needed to store a PCM sample
 * \param format Sample format
 * \return bits per sample, a negative error code if not applicable
 */
int snd_pcm_format_physical_width(snd_pcm_format_t format)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_S8:
	case SNDRV_PCM_FORMAT_U8:
		return 8;
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S16_BE:
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_U16_BE:
		return 16;
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S24_BE:
	case SNDRV_PCM_FORMAT_U24_LE:
	case SNDRV_PCM_FORMAT_U24_BE:
	case SNDRV_PCM_FORMAT_S32_LE:
	case SNDRV_PCM_FORMAT_S32_BE:
	case SNDRV_PCM_FORMAT_U32_LE:
	case SNDRV_PCM_FORMAT_U32_BE:
	case SNDRV_PCM_FORMAT_FLOAT_LE:
	case SNDRV_PCM_FORMAT_FLOAT_BE:
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_LE:
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_BE:
		return 32;
	case SNDRV_PCM_FORMAT_FLOAT64_LE:
	case SNDRV_PCM_FORMAT_FLOAT64_BE:
		return 64;
	case SNDRV_PCM_FORMAT_MU_LAW:
	case SNDRV_PCM_FORMAT_A_LAW:
		return 8;
	case SNDRV_PCM_FORMAT_IMA_ADPCM:
		return 4;
	default:
		return -EINVAL;
	}
}

/**
 * \brief Return bytes needed to store a quantity of PCM sample
 * \param format Sample format
 * \param samples Samples count
 * \return bytes needed, a negative error code if not integer or unknown
 */
ssize_t snd_pcm_format_size(snd_pcm_format_t format, size_t samples)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_S8:
	case SNDRV_PCM_FORMAT_U8:
		return samples;
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S16_BE:
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_U16_BE:
		return samples * 2;
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S24_BE:
	case SNDRV_PCM_FORMAT_U24_LE:
	case SNDRV_PCM_FORMAT_U24_BE:
	case SNDRV_PCM_FORMAT_S32_LE:
	case SNDRV_PCM_FORMAT_S32_BE:
	case SNDRV_PCM_FORMAT_U32_LE:
	case SNDRV_PCM_FORMAT_U32_BE:
	case SNDRV_PCM_FORMAT_FLOAT_LE:
	case SNDRV_PCM_FORMAT_FLOAT_BE:
		return samples * 4;
	case SNDRV_PCM_FORMAT_FLOAT64_LE:
	case SNDRV_PCM_FORMAT_FLOAT64_BE:
		return samples * 8;
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_LE:
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_BE:
		return samples * 4;
	case SNDRV_PCM_FORMAT_MU_LAW:
	case SNDRV_PCM_FORMAT_A_LAW:
		return samples;
	case SNDRV_PCM_FORMAT_IMA_ADPCM:
		if (samples & 1)
			return -EINVAL;
		return samples / 2;
	default:
		assert(0);
		return -EINVAL;
	}
}

/**
 * \brief Return 64 bit expressing silence for a PCM sample format
 * \param format Sample format
 * \return silence 64 bit word
 */
u_int64_t snd_pcm_format_silence_64(snd_pcm_format_t format)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_S8:
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S16_BE:
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S24_BE:
	case SNDRV_PCM_FORMAT_S32_LE:
	case SNDRV_PCM_FORMAT_S32_BE:
		return 0;
	case SNDRV_PCM_FORMAT_U8:
		return 0x8080808080808080ULL;
#ifdef SNDRV_LITTLE_ENDIAN
	case SNDRV_PCM_FORMAT_U16_LE:
		return 0x8000800080008000ULL;
	case SNDRV_PCM_FORMAT_U24_LE:
		return 0x0080000000800000ULL;
	case SNDRV_PCM_FORMAT_U32_LE:
		return 0x8000000080000000ULL;
	case SNDRV_PCM_FORMAT_U16_BE:
		return 0x0080008000800080ULL;
	case SNDRV_PCM_FORMAT_U24_BE:
		return 0x0000800000008000ULL;
	case SNDRV_PCM_FORMAT_U32_BE:
		return 0x0000008000000080ULL;
#else
	case SNDRV_PCM_FORMAT_U16_LE:
		return 0x0080008000800080ULL;
	case SNDRV_PCM_FORMAT_U24_LE:
		return 0x0000800000008000ULL;
	case SNDRV_PCM_FORMAT_U32_LE:
		return 0x0000008000000080ULL;
	case SNDRV_PCM_FORMAT_U16_BE:
		return 0x8000800080008000ULL;
	case SNDRV_PCM_FORMAT_U24_BE:
		return 0x0080000000800000ULL;
	case SNDRV_PCM_FORMAT_U32_BE:
		return 0x8000000080000000ULL;
#endif
	case SNDRV_PCM_FORMAT_FLOAT_LE:
	{
		union {
			float f;
			u_int32_t i;
		} u;
		u.f = 0.0;
#ifdef SNDRV_LITTLE_ENDIAN
		return u.i;
#else
		return bswap_32(u.i);
#endif
	}
	case SNDRV_PCM_FORMAT_FLOAT64_LE:
	{
		union {
			double f;
			u_int64_t i;
		} u;
		u.f = 0.0;
#ifdef SNDRV_LITTLE_ENDIAN
		return u.i;
#else
		return bswap_64(u.i);
#endif
	}
	case SNDRV_PCM_FORMAT_FLOAT_BE:		
	{
		union {
			float f;
			u_int32_t i;
		} u;
		u.f = 0.0;
#ifdef SNDRV_LITTLE_ENDIAN
		return bswap_32(u.i);
#else
		return u.i;
#endif
	}
	case SNDRV_PCM_FORMAT_FLOAT64_BE:
	{
		union {
			double f;
			u_int64_t i;
		} u;
		u.f = 0.0;
#ifdef SNDRV_LITTLE_ENDIAN
		return bswap_64(u.i);
#else
		return u.i;
#endif
	}
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_LE:
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME_BE:
		return 0;	
	case SNDRV_PCM_FORMAT_MU_LAW:
		return 0x7f7f7f7f7f7f7f7fULL;
	case SNDRV_PCM_FORMAT_A_LAW:
		return 0x5555555555555555ULL;
	case SNDRV_PCM_FORMAT_IMA_ADPCM:	/* special case */
	case SNDRV_PCM_FORMAT_MPEG:
	case SNDRV_PCM_FORMAT_GSM:
	case SNDRV_PCM_FORMAT_SPECIAL:
		return 0;
	default:
		assert(0);
		return 0;
	}
	return 0;
}

/**
 * \brief Return 32 bit expressing silence for a PCM sample format
 * \param format Sample format
 * \return silence 32 bit word
 */
u_int32_t snd_pcm_format_silence_32(snd_pcm_format_t format)
{
	assert(snd_pcm_format_physical_width(format) <= 32);
	return (u_int32_t)snd_pcm_format_silence_64(format);
}

/**
 * \brief Return 16 bit expressing silence for a PCM sample format
 * \param format Sample format
 * \return silence 16 bit word
 */
u_int16_t snd_pcm_format_silence_16(snd_pcm_format_t format)
{
	assert(snd_pcm_format_physical_width(format) <= 16);
	return (u_int16_t)snd_pcm_format_silence_64(format);
}

/**
 * \brief Return 8 bit expressing silence for a PCM sample format
 * \param format Sample format
 * \return silence 8 bit word
 */
u_int8_t snd_pcm_format_silence(snd_pcm_format_t format)
{
	assert(snd_pcm_format_physical_width(format) <= 8);
	return (u_int8_t)snd_pcm_format_silence_64(format);
}

/**
 * \brief Silence a PCM samples buffer
 * \param format Sample format
 * \param data Buffer
 * \return samples Samples count
 */
int snd_pcm_format_set_silence(snd_pcm_format_t format, void *data, unsigned int samples)
{
	if (samples == 0)
		return 0;
	switch (snd_pcm_format_physical_width(format)) {
	case 4: {
		u_int8_t silence = snd_pcm_format_silence_64(format);
		unsigned int samples1;
		if (samples % 2 != 0)
			return -EINVAL;
		samples1 = samples / 2;
		memset(data, silence, samples1);
		break;
	}
	case 8: {
		u_int8_t silence = snd_pcm_format_silence_64(format);
		memset(data, silence, samples);
		break;
	}
	case 16: {
		u_int16_t silence = snd_pcm_format_silence_64(format);
		while (samples-- > 0)
			*((u_int16_t *)data)++ = silence;
		break;
	}
	case 32: {
		u_int32_t silence = snd_pcm_format_silence_64(format);
		while (samples-- > 0)
			*((u_int32_t *)data)++ = silence;
		break;
	}
	case 64: {
		u_int64_t silence = snd_pcm_format_silence_64(format);
		while (samples-- > 0)
			*((u_int64_t *)data)++ = silence;
		break;
	}
	default:
		assert(0);
		return -EINVAL;
	}
	return 0;
}

static int linear_formats[4*2*2] = {
	SNDRV_PCM_FORMAT_S8,
	SNDRV_PCM_FORMAT_S8,
	SNDRV_PCM_FORMAT_U8,
	SNDRV_PCM_FORMAT_U8,
	SNDRV_PCM_FORMAT_S16_LE,
	SNDRV_PCM_FORMAT_S16_BE,
	SNDRV_PCM_FORMAT_U16_LE,
	SNDRV_PCM_FORMAT_U16_BE,
	SNDRV_PCM_FORMAT_S24_LE,
	SNDRV_PCM_FORMAT_S24_BE,
	SNDRV_PCM_FORMAT_U24_LE,
	SNDRV_PCM_FORMAT_U24_BE,
	SNDRV_PCM_FORMAT_S32_LE,
	SNDRV_PCM_FORMAT_S32_BE,
	SNDRV_PCM_FORMAT_U32_LE,
	SNDRV_PCM_FORMAT_U32_BE
};

/**
 * \brief Compose a PCM sample linear format
 * \param width Nominal bits per sample
 * \param unsignd Sign: 0 signed, 1 unsigned
 * \return big_endian Endian: 0 little endian, 1 big endian
 */
snd_pcm_format_t snd_pcm_build_linear_format(int width, int unsignd, int big_endian)
{
	switch (width) {
	case 8:
		width = 0;
		break;
	case 16:
		width = 1;
		break;
	case 24:
		width = 2;
		break;
	case 32:
		width = 3;
		break;
	default:
		return SND_PCM_FORMAT_UNKNOWN;
	}
	return ((int(*)[2][2])linear_formats)[width][!!unsignd][!!big_endian];
}