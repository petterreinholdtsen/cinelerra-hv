/*
 * Copyright (c) 2003 Matteo Frigo
 * Copyright (c) 2003 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sat Jul  5 22:11:12 EDT 2003 */

#include "codelet-rdft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_hc2r -compact -variables 4 -sign 1 -n 15 -name hc2r_15 -include hc2r.h */

/*
 * This function contains 64 FP additions, 31 FP multiplications,
 * (or, 47 additions, 14 multiplications, 17 fused multiply/add),
 * 44 stack variables, and 30 memory accesses
 */
/*
 * Generator Id's : 
 * $Id: algsimp.ml,v 1.7 2003/03/15 20:29:42 stevenj Exp $
 * $Id: fft.ml,v 1.2 2003/03/15 20:29:42 stevenj Exp $
 * $Id: gen_hc2r.ml,v 1.14 2003/04/17 19:25:50 athena Exp $
 */

#include "hc2r.h"

static void hc2r_15(const R *ri, const R *ii, R *O, stride ris, stride iis, stride os, int v, int ivs, int ovs)
{
     DK(KP1_118033988, +1.118033988749894848204586834365638117720309180);
     DK(KP1_902113032, +1.902113032590307144232878666758764286811397268);
     DK(KP1_175570504, +1.175570504584946258337411909278145537195304875);
     DK(KP500000000, +0.500000000000000000000000000000000000000000000);
     DK(KP866025403, +0.866025403784438646763723170752936183471402627);
     DK(KP2_000000000, +2.000000000000000000000000000000000000000000000);
     DK(KP1_732050807, +1.732050807568877293527446341505872366942805254);
     int i;
     for (i = v; i > 0; i = i - 1, ri = ri + ivs, ii = ii + ivs, O = O + ovs) {
	  E T3, Tu, Ti, TB, TZ, T10, TE, TG, TJ, Tn, Tv, Ts, Tw, T8, Td;
	  E Te;
	  {
	       E Th, T1, T2, Tf, Tg;
	       Tg = ii[WS(iis, 5)];
	       Th = KP1_732050807 * Tg;
	       T1 = ri[0];
	       T2 = ri[WS(ris, 5)];
	       Tf = T1 - T2;
	       T3 = FMA(KP2_000000000, T2, T1);
	       Tu = Tf - Th;
	       Ti = Tf + Th;
	  }
	  {
	       E T4, TD, T9, TI, T5, T6, T7, Ta, Tb, Tc, Tr, TH, Tm, TC, Tj;
	       E To;
	       T4 = ri[WS(ris, 3)];
	       TD = ii[WS(iis, 3)];
	       T9 = ri[WS(ris, 6)];
	       TI = ii[WS(iis, 6)];
	       T5 = ri[WS(ris, 7)];
	       T6 = ri[WS(ris, 2)];
	       T7 = T5 + T6;
	       Ta = ri[WS(ris, 4)];
	       Tb = ri[WS(ris, 1)];
	       Tc = Ta + Tb;
	       {
		    E Tp, Tq, Tk, Tl;
		    Tp = ii[WS(iis, 4)];
		    Tq = ii[WS(iis, 1)];
		    Tr = KP866025403 * (Tp + Tq);
		    TH = Tp - Tq;
		    Tk = ii[WS(iis, 7)];
		    Tl = ii[WS(iis, 2)];
		    Tm = KP866025403 * (Tk - Tl);
		    TC = Tk + Tl;
	       }
	       TB = KP866025403 * (T5 - T6);
	       TZ = TD - TC;
	       T10 = TI - TH;
	       TE = FMA(KP500000000, TC, TD);
	       TG = KP866025403 * (Ta - Tb);
	       TJ = FMA(KP500000000, TH, TI);
	       Tj = FNMS(KP500000000, T7, T4);
	       Tn = Tj - Tm;
	       Tv = Tj + Tm;
	       To = FNMS(KP500000000, Tc, T9);
	       Ts = To - Tr;
	       Tw = To + Tr;
	       T8 = T4 + T7;
	       Td = T9 + Tc;
	       Te = T8 + Td;
	  }
	  O[0] = FMA(KP2_000000000, Te, T3);
	  {
	       E T11, T13, TY, T12, TW, TX;
	       T11 = FNMS(KP1_902113032, T10, KP1_175570504 * TZ);
	       T13 = FMA(KP1_902113032, TZ, KP1_175570504 * T10);
	       TW = FNMS(KP500000000, Te, T3);
	       TX = KP1_118033988 * (T8 - Td);
	       TY = TW - TX;
	       T12 = TX + TW;
	       O[WS(os, 12)] = TY - T11;
	       O[WS(os, 9)] = T12 + T13;
	       O[WS(os, 3)] = TY + T11;
	       O[WS(os, 6)] = T12 - T13;
	  }
	  {
	       E TP, Tt, TO, TT, TV, TR, TS, TU, TQ;
	       TP = KP1_118033988 * (Tn - Ts);
	       Tt = Tn + Ts;
	       TO = FNMS(KP500000000, Tt, Ti);
	       TR = TE - TB;
	       TS = TJ - TG;
	       TT = FNMS(KP1_902113032, TS, KP1_175570504 * TR);
	       TV = FMA(KP1_902113032, TR, KP1_175570504 * TS);
	       O[WS(os, 5)] = FMA(KP2_000000000, Tt, Ti);
	       TU = TP + TO;
	       O[WS(os, 11)] = TU - TV;
	       O[WS(os, 14)] = TU + TV;
	       TQ = TO - TP;
	       O[WS(os, 2)] = TQ - TT;
	       O[WS(os, 8)] = TQ + TT;
	  }
	  {
	       E Tz, Tx, Ty, TL, TN, TF, TK, TM, TA;
	       Tz = KP1_118033988 * (Tv - Tw);
	       Tx = Tv + Tw;
	       Ty = FNMS(KP500000000, Tx, Tu);
	       TF = TB + TE;
	       TK = TG + TJ;
	       TL = FNMS(KP1_902113032, TK, KP1_175570504 * TF);
	       TN = FMA(KP1_902113032, TF, KP1_175570504 * TK);
	       O[WS(os, 10)] = FMA(KP2_000000000, Tx, Tu);
	       TM = Tz + Ty;
	       O[WS(os, 1)] = TM - TN;
	       O[WS(os, 4)] = TM + TN;
	       TA = Ty - Tz;
	       O[WS(os, 7)] = TA - TL;
	       O[WS(os, 13)] = TA + TL;
	  }
     }
}

static const khc2r_desc desc = { 15, "hc2r_15", {47, 14, 17, 0}, &GENUS, 0, 0, 0, 0, 0 };

void X(codelet_hc2r_15) (planner *p) {
     X(khc2r_register) (p, hc2r_15, &desc);
}
