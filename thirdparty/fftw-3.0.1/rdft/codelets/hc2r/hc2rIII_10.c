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
/* Generated on Sat Jul  5 22:12:05 EDT 2003 */

#include "codelet-rdft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_hc2r -compact -variables 4 -sign 1 -n 10 -name hc2rIII_10 -dft-III -include hc2rIII.h */

/*
 * This function contains 32 FP additions, 16 FP multiplications,
 * (or, 26 additions, 10 multiplications, 6 fused multiply/add),
 * 22 stack variables, and 20 memory accesses
 */
/*
 * Generator Id's : 
 * $Id: algsimp.ml,v 1.7 2003/03/15 20:29:42 stevenj Exp $
 * $Id: fft.ml,v 1.2 2003/03/15 20:29:42 stevenj Exp $
 * $Id: gen_hc2r.ml,v 1.14 2003/04/17 19:25:50 athena Exp $
 */

#include "hc2rIII.h"

static void hc2rIII_10(const R *ri, const R *ii, R *O, stride ris, stride iis, stride os, int v, int ivs, int ovs)
{
     DK(KP500000000, +0.500000000000000000000000000000000000000000000);
     DK(KP1_902113032, +1.902113032590307144232878666758764286811397268);
     DK(KP1_175570504, +1.175570504584946258337411909278145537195304875);
     DK(KP2_000000000, +2.000000000000000000000000000000000000000000000);
     DK(KP1_118033988, +1.118033988749894848204586834365638117720309180);
     int i;
     for (i = v; i > 0; i = i - 1, ri = ri + ivs, ii = ii + ivs, O = O + ovs) {
	  E T1, To, T8, Tq, Ta, Tp, Te, Ts, Th, Tn;
	  T1 = ri[WS(ris, 2)];
	  To = ii[WS(iis, 2)];
	  {
	       E T2, T3, T4, T5, T6, T7;
	       T2 = ri[WS(ris, 4)];
	       T3 = ri[0];
	       T4 = T2 + T3;
	       T5 = ri[WS(ris, 3)];
	       T6 = ri[WS(ris, 1)];
	       T7 = T5 + T6;
	       T8 = T4 + T7;
	       Tq = T5 - T6;
	       Ta = KP1_118033988 * (T7 - T4);
	       Tp = T2 - T3;
	  }
	  {
	       E Tc, Td, Tm, Tf, Tg, Tl;
	       Tc = ii[WS(iis, 4)];
	       Td = ii[0];
	       Tm = Tc + Td;
	       Tf = ii[WS(iis, 1)];
	       Tg = ii[WS(iis, 3)];
	       Tl = Tg + Tf;
	       Te = Tc - Td;
	       Ts = KP1_118033988 * (Tl + Tm);
	       Th = Tf - Tg;
	       Tn = Tl - Tm;
	  }
	  O[0] = KP2_000000000 * (T1 + T8);
	  O[WS(os, 5)] = KP2_000000000 * (Tn - To);
	  {
	       E Ti, Tj, Tb, Tk, T9;
	       Ti = FNMS(KP1_902113032, Th, KP1_175570504 * Te);
	       Tj = FMA(KP1_175570504, Th, KP1_902113032 * Te);
	       T9 = FNMS(KP2_000000000, T1, KP500000000 * T8);
	       Tb = T9 - Ta;
	       Tk = T9 + Ta;
	       O[WS(os, 2)] = Tb + Ti;
	       O[WS(os, 6)] = Tk + Tj;
	       O[WS(os, 8)] = Ti - Tb;
	       O[WS(os, 4)] = Tj - Tk;
	  }
	  {
	       E Tr, Tv, Tu, Tw, Tt;
	       Tr = FMA(KP1_902113032, Tp, KP1_175570504 * Tq);
	       Tv = FNMS(KP1_175570504, Tp, KP1_902113032 * Tq);
	       Tt = FMA(KP500000000, Tn, KP2_000000000 * To);
	       Tu = Ts + Tt;
	       Tw = Tt - Ts;
	       O[WS(os, 1)] = -(Tr + Tu);
	       O[WS(os, 7)] = Tw - Tv;
	       O[WS(os, 9)] = Tr - Tu;
	       O[WS(os, 3)] = Tv + Tw;
	  }
     }
}

static const khc2r_desc desc = { 10, "hc2rIII_10", {26, 10, 6, 0}, &GENUS, 0, 0, 0, 0, 0 };

void X(codelet_hc2rIII_10) (planner *p) {
     X(khc2rIII_register) (p, hc2rIII_10, &desc);
}
