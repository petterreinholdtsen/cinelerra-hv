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
/* Generated on Sat Jul  5 22:11:11 EDT 2003 */

#include "codelet-rdft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_hc2r -compact -variables 4 -sign 1 -n 10 -name hc2r_10 -include hc2r.h */

/*
 * This function contains 34 FP additions, 14 FP multiplications,
 * (or, 26 additions, 6 multiplications, 8 fused multiply/add),
 * 26 stack variables, and 20 memory accesses
 */
/*
 * Generator Id's : 
 * $Id: algsimp.ml,v 1.7 2003/03/15 20:29:42 stevenj Exp $
 * $Id: fft.ml,v 1.2 2003/03/15 20:29:42 stevenj Exp $
 * $Id: gen_hc2r.ml,v 1.14 2003/04/17 19:25:50 athena Exp $
 */

#include "hc2r.h"

static void hc2r_10(const R *ri, const R *ii, R *O, stride ris, stride iis, stride os, int v, int ivs, int ovs)
{
     DK(KP500000000, +0.500000000000000000000000000000000000000000000);
     DK(KP1_902113032, +1.902113032590307144232878666758764286811397268);
     DK(KP1_175570504, +1.175570504584946258337411909278145537195304875);
     DK(KP2_000000000, +2.000000000000000000000000000000000000000000000);
     DK(KP1_118033988, +1.118033988749894848204586834365638117720309180);
     int i;
     for (i = v; i > 0; i = i - 1, ri = ri + ivs, ii = ii + ivs, O = O + ovs) {
	  E T3, Tb, Tn, Tv, Tk, Tu, Ta, Ts, Te, Tg, Ti, Tj;
	  {
	       E T1, T2, Tl, Tm;
	       T1 = ri[0];
	       T2 = ri[WS(ris, 5)];
	       T3 = T1 - T2;
	       Tb = T1 + T2;
	       Tl = ii[WS(iis, 4)];
	       Tm = ii[WS(iis, 1)];
	       Tn = Tl - Tm;
	       Tv = Tl + Tm;
	  }
	  Ti = ii[WS(iis, 2)];
	  Tj = ii[WS(iis, 3)];
	  Tk = Ti - Tj;
	  Tu = Ti + Tj;
	  {
	       E T6, Tc, T9, Td;
	       {
		    E T4, T5, T7, T8;
		    T4 = ri[WS(ris, 2)];
		    T5 = ri[WS(ris, 3)];
		    T6 = T4 - T5;
		    Tc = T4 + T5;
		    T7 = ri[WS(ris, 4)];
		    T8 = ri[WS(ris, 1)];
		    T9 = T7 - T8;
		    Td = T7 + T8;
	       }
	       Ta = T6 + T9;
	       Ts = KP1_118033988 * (T6 - T9);
	       Te = Tc + Td;
	       Tg = KP1_118033988 * (Tc - Td);
	  }
	  O[WS(os, 5)] = FMA(KP2_000000000, Ta, T3);
	  O[0] = FMA(KP2_000000000, Te, Tb);
	  {
	       E To, Tq, Th, Tp, Tf;
	       To = FNMS(KP1_902113032, Tn, KP1_175570504 * Tk);
	       Tq = FMA(KP1_902113032, Tk, KP1_175570504 * Tn);
	       Tf = FNMS(KP500000000, Te, Tb);
	       Th = Tf - Tg;
	       Tp = Tg + Tf;
	       O[WS(os, 2)] = Th - To;
	       O[WS(os, 4)] = Tp + Tq;
	       O[WS(os, 8)] = Th + To;
	       O[WS(os, 6)] = Tp - Tq;
	  }
	  {
	       E Tw, Ty, Tt, Tx, Tr;
	       Tw = FNMS(KP1_902113032, Tv, KP1_175570504 * Tu);
	       Ty = FMA(KP1_902113032, Tu, KP1_175570504 * Tv);
	       Tr = FNMS(KP500000000, Ta, T3);
	       Tt = Tr - Ts;
	       Tx = Ts + Tr;
	       O[WS(os, 7)] = Tt - Tw;
	       O[WS(os, 9)] = Tx + Ty;
	       O[WS(os, 3)] = Tt + Tw;
	       O[WS(os, 1)] = Tx - Ty;
	  }
     }
}

static const khc2r_desc desc = { 10, "hc2r_10", {26, 6, 8, 0}, &GENUS, 0, 0, 0, 0, 0 };

void X(codelet_hc2r_10) (planner *p) {
     X(khc2r_register) (p, hc2r_10, &desc);
}
