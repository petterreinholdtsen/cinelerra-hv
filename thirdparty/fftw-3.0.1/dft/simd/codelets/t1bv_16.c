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
/* Generated on Sat Jul  5 21:43:55 EDT 2003 */

#include "codelet-dft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_twiddle_c -simd -compact -variables 4 -n 16 -name t1bv_16 -include t1b.h -sign 1 */

/*
 * This function contains 87 FP additions, 42 FP multiplications,
 * (or, 83 additions, 38 multiplications, 4 fused multiply/add),
 * 36 stack variables, and 32 memory accesses
 */
/*
 * Generator Id's : 
 * $Id: algsimp.ml,v 1.7 2003/03/15 20:29:42 stevenj Exp $
 * $Id: fft.ml,v 1.2 2003/03/15 20:29:42 stevenj Exp $
 * $Id: gen_twiddle_c.ml,v 1.7 2003/04/16 19:51:27 athena Exp $
 */

#include "t1b.h"

static const R *t1bv_16(R *ri, R *ii, const R *W, stride ios, int m, int dist)
{
     DVK(KP382683432, +0.382683432365089771728459984030398866761344562);
     DVK(KP923879532, +0.923879532511286756128183189396788286822416626);
     DVK(KP707106781, +0.707106781186547524400844362104849039284835938);
     int i;
     R *x;
     x = ii;
     BEGIN_SIMD();
     for (i = m; i > 0; i = i - VL, x = x + (VL * dist), W = W + (TWVL * 30)) {
	  V TJ, T1b, TD, T1c, T17, T18, Ty, TK, T10, T11, T12, Tb, TM, T13, T14;
	  V T15, Tm, TN, TG, TI, TH;
	  TG = LD(&(x[0]), dist, &(x[0]));
	  TH = LD(&(x[WS(ios, 8)]), dist, &(x[0]));
	  TI = BYTW(&(W[TWVL * 14]), TH);
	  TJ = VSUB(TG, TI);
	  T1b = VADD(TG, TI);
	  {
	       V TA, TC, Tz, TB;
	       Tz = LD(&(x[WS(ios, 4)]), dist, &(x[0]));
	       TA = BYTW(&(W[TWVL * 6]), Tz);
	       TB = LD(&(x[WS(ios, 12)]), dist, &(x[0]));
	       TC = BYTW(&(W[TWVL * 22]), TB);
	       TD = VSUB(TA, TC);
	       T1c = VADD(TA, TC);
	  }
	  {
	       V Tp, Tw, Tr, Tu, Ts, Tx;
	       {
		    V To, Tv, Tq, Tt;
		    To = LD(&(x[WS(ios, 2)]), dist, &(x[0]));
		    Tp = BYTW(&(W[TWVL * 2]), To);
		    Tv = LD(&(x[WS(ios, 6)]), dist, &(x[0]));
		    Tw = BYTW(&(W[TWVL * 10]), Tv);
		    Tq = LD(&(x[WS(ios, 10)]), dist, &(x[0]));
		    Tr = BYTW(&(W[TWVL * 18]), Tq);
		    Tt = LD(&(x[WS(ios, 14)]), dist, &(x[0]));
		    Tu = BYTW(&(W[TWVL * 26]), Tt);
	       }
	       T17 = VADD(Tp, Tr);
	       T18 = VADD(Tu, Tw);
	       Ts = VSUB(Tp, Tr);
	       Tx = VSUB(Tu, Tw);
	       Ty = VMUL(LDK(KP707106781), VSUB(Ts, Tx));
	       TK = VMUL(LDK(KP707106781), VADD(Ts, Tx));
	  }
	  {
	       V T2, T9, T4, T7, T5, Ta;
	       {
		    V T1, T8, T3, T6;
		    T1 = LD(&(x[WS(ios, 1)]), dist, &(x[WS(ios, 1)]));
		    T2 = BYTW(&(W[0]), T1);
		    T8 = LD(&(x[WS(ios, 13)]), dist, &(x[WS(ios, 1)]));
		    T9 = BYTW(&(W[TWVL * 24]), T8);
		    T3 = LD(&(x[WS(ios, 9)]), dist, &(x[WS(ios, 1)]));
		    T4 = BYTW(&(W[TWVL * 16]), T3);
		    T6 = LD(&(x[WS(ios, 5)]), dist, &(x[WS(ios, 1)]));
		    T7 = BYTW(&(W[TWVL * 8]), T6);
	       }
	       T10 = VADD(T2, T4);
	       T11 = VADD(T7, T9);
	       T12 = VSUB(T10, T11);
	       T5 = VSUB(T2, T4);
	       Ta = VSUB(T7, T9);
	       Tb = VFNMS(LDK(KP382683432), Ta, VMUL(LDK(KP923879532), T5));
	       TM = VFMA(LDK(KP382683432), T5, VMUL(LDK(KP923879532), Ta));
	  }
	  {
	       V Td, Tk, Tf, Ti, Tg, Tl;
	       {
		    V Tc, Tj, Te, Th;
		    Tc = LD(&(x[WS(ios, 15)]), dist, &(x[WS(ios, 1)]));
		    Td = BYTW(&(W[TWVL * 28]), Tc);
		    Tj = LD(&(x[WS(ios, 11)]), dist, &(x[WS(ios, 1)]));
		    Tk = BYTW(&(W[TWVL * 20]), Tj);
		    Te = LD(&(x[WS(ios, 7)]), dist, &(x[WS(ios, 1)]));
		    Tf = BYTW(&(W[TWVL * 12]), Te);
		    Th = LD(&(x[WS(ios, 3)]), dist, &(x[WS(ios, 1)]));
		    Ti = BYTW(&(W[TWVL * 4]), Th);
	       }
	       T13 = VADD(Td, Tf);
	       T14 = VADD(Ti, Tk);
	       T15 = VSUB(T13, T14);
	       Tg = VSUB(Td, Tf);
	       Tl = VSUB(Ti, Tk);
	       Tm = VFMA(LDK(KP923879532), Tg, VMUL(LDK(KP382683432), Tl));
	       TN = VFNMS(LDK(KP382683432), Tg, VMUL(LDK(KP923879532), Tl));
	  }
	  {
	       V T1a, T1g, T1f, T1h;
	       {
		    V T16, T19, T1d, T1e;
		    T16 = VMUL(LDK(KP707106781), VSUB(T12, T15));
		    T19 = VSUB(T17, T18);
		    T1a = VBYI(VSUB(T16, T19));
		    T1g = VBYI(VADD(T19, T16));
		    T1d = VSUB(T1b, T1c);
		    T1e = VMUL(LDK(KP707106781), VADD(T12, T15));
		    T1f = VSUB(T1d, T1e);
		    T1h = VADD(T1d, T1e);
	       }
	       ST(&(x[WS(ios, 6)]), VADD(T1a, T1f), dist, &(x[0]));
	       ST(&(x[WS(ios, 14)]), VSUB(T1h, T1g), dist, &(x[0]));
	       ST(&(x[WS(ios, 10)]), VSUB(T1f, T1a), dist, &(x[0]));
	       ST(&(x[WS(ios, 2)]), VADD(T1g, T1h), dist, &(x[0]));
	  }
	  {
	       V T1k, T1o, T1n, T1p;
	       {
		    V T1i, T1j, T1l, T1m;
		    T1i = VADD(T1b, T1c);
		    T1j = VADD(T17, T18);
		    T1k = VSUB(T1i, T1j);
		    T1o = VADD(T1i, T1j);
		    T1l = VADD(T10, T11);
		    T1m = VADD(T13, T14);
		    T1n = VBYI(VSUB(T1l, T1m));
		    T1p = VADD(T1l, T1m);
	       }
	       ST(&(x[WS(ios, 12)]), VSUB(T1k, T1n), dist, &(x[0]));
	       ST(&(x[0]), VADD(T1o, T1p), dist, &(x[0]));
	       ST(&(x[WS(ios, 4)]), VADD(T1k, T1n), dist, &(x[0]));
	       ST(&(x[WS(ios, 8)]), VSUB(T1o, T1p), dist, &(x[0]));
	  }
	  {
	       V TF, TQ, TP, TR;
	       {
		    V Tn, TE, TL, TO;
		    Tn = VSUB(Tb, Tm);
		    TE = VSUB(Ty, TD);
		    TF = VBYI(VSUB(Tn, TE));
		    TQ = VBYI(VADD(TE, Tn));
		    TL = VSUB(TJ, TK);
		    TO = VSUB(TM, TN);
		    TP = VSUB(TL, TO);
		    TR = VADD(TL, TO);
	       }
	       ST(&(x[WS(ios, 5)]), VADD(TF, TP), dist, &(x[WS(ios, 1)]));
	       ST(&(x[WS(ios, 13)]), VSUB(TR, TQ), dist, &(x[WS(ios, 1)]));
	       ST(&(x[WS(ios, 11)]), VSUB(TP, TF), dist, &(x[WS(ios, 1)]));
	       ST(&(x[WS(ios, 3)]), VADD(TQ, TR), dist, &(x[WS(ios, 1)]));
	  }
	  {
	       V TU, TY, TX, TZ;
	       {
		    V TS, TT, TV, TW;
		    TS = VADD(TJ, TK);
		    TT = VADD(Tb, Tm);
		    TU = VADD(TS, TT);
		    TY = VSUB(TS, TT);
		    TV = VADD(TD, Ty);
		    TW = VADD(TM, TN);
		    TX = VBYI(VADD(TV, TW));
		    TZ = VBYI(VSUB(TW, TV));
	       }
	       ST(&(x[WS(ios, 15)]), VSUB(TU, TX), dist, &(x[WS(ios, 1)]));
	       ST(&(x[WS(ios, 7)]), VADD(TY, TZ), dist, &(x[WS(ios, 1)]));
	       ST(&(x[WS(ios, 1)]), VADD(TU, TX), dist, &(x[WS(ios, 1)]));
	       ST(&(x[WS(ios, 9)]), VSUB(TY, TZ), dist, &(x[WS(ios, 1)]));
	  }
     }
     END_SIMD();
     return W;
}

static const tw_instr twinstr[] = {
     VTW(1),
     VTW(2),
     VTW(3),
     VTW(4),
     VTW(5),
     VTW(6),
     VTW(7),
     VTW(8),
     VTW(9),
     VTW(10),
     VTW(11),
     VTW(12),
     VTW(13),
     VTW(14),
     VTW(15),
     {TW_NEXT, VL, 0}
};

static const ct_desc desc = { 16, "t1bv_16", twinstr, {83, 38, 4, 0}, &GENUS, 0, 0, 0 };

void X(codelet_t1bv_16) (planner *p) {
     X(kdft_dit_register) (p, t1bv_16, &desc);
}
