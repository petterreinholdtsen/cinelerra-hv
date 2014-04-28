/**************************************************************************
 *                                                                        *
 * This code has been developed by John Funnell. This software is an      *
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
 * John Funnell
 * Andrea Graziani
 *
 * DivX Advanced Research Center <darc@projectmayo.com>
*
**/
// mp4_vld.h //

#ifndef _MP4_VLD_H_
#define _MP4_VLD_H_

#define ESCAPE 7167

/*** *** ***/

typedef struct {
	int val, len;
} tab_type;

/***/

typedef struct {
	int last;
	int run;
	int level;
} event_t;

/*** *** ***/

static tab_type tableB16_1[] = // tables to decode Table B16 VLC - 112 values
{ 
	{4353, 7},  {4289, 7},  {385, 7},  {4417, 7},  {449, 7},  {130, 7},  {67, 7},  {9, 7},  {4098, 6},  {4098, 6},  
	{321, 6},  {321, 6},  {4225, 6},  {4225, 6},  {4161, 6},  {4161, 6},  {257, 6},  {257, 6},  {193, 6},  {193, 6},  
	{8, 6},  {8, 6},  {7, 6},  {7, 6},  {66, 6},  {66, 6},  {6, 6},  {6, 6},  {129, 5},  {129, 5},  
	{129, 5},  {129, 5},  {5, 5},  {5, 5},  {5, 5},  {5, 5},  {4, 5},  {4, 5},  {4, 5},  {4, 5},  
	{4097, 4},  {4097, 4},  {4097, 4},  {4097, 4},  {4097, 4},  {4097, 4},  {4097, 4},  {4097, 4},  {1, 2},  {1, 2},  
	{1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  
	{1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  
	{1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  {1, 2},  
	{2, 3},  {2, 3},  {2, 3},  {2, 3},  {2, 3},  {2, 3},  {2, 3},  {2, 3},  {2, 3},  {2, 3},  
	{2, 3},  {2, 3},  {2, 3},  {2, 3},  {2, 3},  {2, 3},  {65, 4},  {65, 4},  {65, 4},  {65, 4},  
	{65, 4},  {65, 4},  {65, 4},  {65, 4},  {3, 4},  {3, 4},  {3, 4},  {3, 4},  {3, 4},  {3, 4},  
	{3, 4},  {3, 4} 
};
static tab_type tableB16_2[] = { {18, 10},  {17, 10},  {4993, 9},  {4993, 9},  {4929, 9},  {4929, 9},  {4865, 9},  {4865, 9},  {4801, 9},  {4801, 9},  {4737, 9},  {4737, 9},  {4162, 9},  {4162, 9},  {4100, 9},  {4100, 9},  {769, 9},  {769, 9},  {705, 9},  {705, 9},  {450, 9},  {450, 9},  {386, 9},  {386, 9},  {322, 9},  {322, 9},  {195, 9},  {195, 9},  {131, 9},  {131, 9},  {70, 9},  {70, 9},  {69, 9},  {69, 9},  {16, 9},  {16, 9},  {258, 9},  {258, 9},  {15, 9},  {15, 9},  {14, 9},  {14, 9},  {13, 9},  {13, 9},  {4609, 8},  {4609, 8},  {4609, 8},  {4609, 8},  {4545, 8},  {4545, 8},  {4545, 8},  {4545, 8},  {4481, 8},  {4481, 8},  {4481, 8},  {4481, 8},  {4099, 8},  {4099, 8},  {4099, 8},  {4099, 8},  {641, 8},  {641, 8},  {641, 8},  {641, 8},  {577, 8},  {577, 8},  {577, 8},  {577, 8},  {513, 8},  {513, 8},  {513, 8},  {513, 8},  {4673, 8},  {4673, 8},  {4673, 8},  {4673, 8},  {194, 8},  {194, 8},  {194, 8},  {194, 8},  {68, 8},  {68, 8},  {68, 8},  {68, 8},  {12, 8},  {12, 8},  {12, 8},  {12, 8},  {11, 8},  {11, 8},  {11, 8},  {11, 8},  {10, 8},  {10, 8},  {10, 8},  {10, 8} };
static tab_type tableB16_3[] = { {4103, 11},  {4103, 11},  {4102, 11},  {4102, 11},  {22, 11},  {22, 11},  {21, 11},  {21, 11},  {4226, 10},  {4226, 10},  {4226, 10},  {4226, 10},  {4163, 10},  {4163, 10},  {4163, 10},  {4163, 10},  {4101, 10},  {4101, 10},  {4101, 10},  {4101, 10},  {833, 10},  {833, 10},  {833, 10},  {833, 10},  {323, 10},  {323, 10},  {323, 10},  {323, 10},  {514, 10},  {514, 10},  {514, 10},  {514, 10},  {259, 10},  {259, 10},  {259, 10},  {259, 10},  {196, 10},  {196, 10},  {196, 10},  {196, 10},  {132, 10},  {132, 10},  {132, 10},  {132, 10},  {71, 10},  {71, 10},  {71, 10},  {71, 10},  {20, 10},  {20, 10},  {20, 10},  {20, 10},  {19, 10},  {19, 10},  {19, 10},  {19, 10},  {23, 11},  {23, 11},  {24, 11},  {24, 11},  {72, 11},  {72, 11},  {578, 11},  {578, 11},  {4290, 11},  {4290, 11},  {4354, 11},  {4354, 11},  {5057, 11},  {5057, 11},  {5121, 11},  {5121, 11},  {25, 12},  {26, 12},  {27, 12},  {73, 12},  {387, 12},  {74, 12},  {133, 12},  {451, 12},  {897, 12},  {4104, 12},  {4418, 12},  {4482, 12},  {5185, 12},  {5249, 12},  {5313, 12},  {5377, 12},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7},  {7167, 7} };

/* tables to decode Table B17 VLC */
static tab_type tableB17_1[] = { {4225,7}, {4209,7}, {4193,7}, {4177,7}, {193,7}, {177,7}, {161,7}, {4,7}, {4161,6}, {4161,6}, {4145,6}, {4145,6}, {4129,6}, {4129,6}, {4113,6}, {4113,6}, {145,6}, {145,6}, {129,6}, {129,6}, {113,6}, {113,6}, {97,6}, {97,6}, {18,6}, {18,6}, {3,6}, {3,6}, {81,5}, {81,5}, {81,5}, {81,5}, {65,5}, {65,5}, {65,5}, {65,5}, {49,5}, {49,5}, {49,5}, {49,5}, {4097,4}, {4097,4}, {4097,4}, {4097,4}, {4097,4}, {4097,4}, {4097,4}, {4097,4}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {17,3}, {33,4}, {33,4}, {33,4}, {33,4}, {33,4}, {33,4}, {33,4}, {33,4}, {2,4}, {2,4},{2,4},{2,4}, {2,4}, {2,4},{2,4},{2,4} };
static tab_type tableB17_2[] = { {9,10}, {8,10}, {4481,9}, {4481,9}, {4465,9}, {4465,9}, {4449,9}, {4449,9}, {4433,9}, {4433,9}, {4417,9}, {4417,9}, {4401,9}, {4401,9}, {4385,9}, {4385,9}, {4369,9}, {4369,9}, {4098,9}, {4098,9}, {353,9}, {353,9}, {337,9}, {337,9}, {321,9}, {321,9}, {305,9}, {305,9}, {289,9}, {289,9}, {273,9}, {273,9}, {257,9}, {257,9}, {241,9}, {241,9}, {66,9}, {66,9}, {50,9}, {50,9}, {7,9}, {7,9}, {6,9}, {6,9}, {4353,8}, {4353,8}, {4353,8}, {4353,8}, {4337,8}, {4337,8}, {4337,8}, {4337,8}, {4321,8}, {4321,8}, {4321,8}, {4321,8}, {4305,8}, {4305,8}, {4305,8}, {4305,8}, {4289,8}, {4289,8}, {4289,8}, {4289,8}, {4273,8}, {4273,8}, {4273,8}, {4273,8}, {4257,8}, {4257,8}, {4257,8}, {4257,8}, {4241,8}, {4241,8}, {4241,8}, {4241,8}, {225,8}, {225,8}, {225,8}, {225,8}, {209,8}, {209,8}, {209,8}, {209,8}, {34,8}, {34,8}, {34,8}, {34,8}, {19,8}, {19,8}, {19,8}, {19,8}, {5,8}, {5,8}, {5,8}, {5,8}, };
static tab_type tableB17_3[] = { {4114,11}, {4114,11}, {4099,11}, {4099,11}, {11,11}, {11,11}, {10,11}, {10,11}, {4545,10}, {4545,10}, {4545,10}, {4545,10}, {4529,10}, {4529,10}, {4529,10}, {4529,10}, {4513,10}, {4513,10}, {4513,10}, {4513,10}, {4497,10}, {4497,10}, {4497,10}, {4497,10}, {146,10}, {146,10}, {146,10}, {146,10}, {130,10}, {130,10}, {130,10}, {130,10}, {114,10}, {114,10}, {114,10}, {114,10}, {98,10}, {98,10}, {98,10}, {98,10}, {82,10}, {82,10}, {82,10}, {82,10}, {51,10}, {51,10}, {51,10}, {51,10}, {35,10}, {35,10}, {35,10}, {35,10}, {20,10}, {20,10}, {20,10}, {20,10}, {12,11}, {12,11}, {21,11}, {21,11}, {369,11}, {369,11}, {385,11}, {385,11}, {4561,11}, {4561,11}, {4577,11}, {4577,11}, {4593,11}, {4593,11}, {4609,11}, {4609,11}, {22,12}, {36,12}, {67,12}, {83,12}, {99,12}, {162,12}, {401,12}, {417,12}, {4625,12}, {4641,12}, {4657,12}, {4673,12}, {4689,12}, {4705,12}, {4721,12}, {4737,12}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, {7167,7}, };

/***/

#endif // _MP4_VLD_H_

/***/

extern tab_type tableB16_1[];
extern tab_type tableB16_2[];
extern tab_type tableB16_3[];

extern tab_type tableB17_1[];
extern tab_type tableB17_2[];
extern tab_type tableB17_3[];

/***/

int vldTableB19(int last, int run);
int vldTableB20(int last, int run);
int vldTableB21(int last, int level);
int vldTableB22(int last, int level);
tab_type *vldTableB16(int code);
tab_type *vldTableB17(int code);

/***/

extern event_t vld_event(int intraFlag);
extern event_t vld_intra_dct();
extern event_t vld_inter_dct();

/***/
