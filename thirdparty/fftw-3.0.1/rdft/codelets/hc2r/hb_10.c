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
/* Generated on Sat Jul  5 22:11:33 EDT 2003 */

#include "codelet-rdft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_hc2hc -compact -variables 4 -sign 1 -n 10 -dif -name hb_10 -include hb.h */

/*
 * This function contains 102 FP additions, 60 FP multiplications,
 * (or, 72 additions, 30 multiplications, 30 fused multiply/add),
 * 39 stack variables, and 40 memory accesses
 */
/*
 * Generator Id's : 
 * $Id: algsimp.ml,v 1.7 2003/03/15 20:29:42 stevenj Exp $
 * $Id: fft.ml,v 1.2 2003/03/15 20:29:42 stevenj Exp $
 * $Id: gen_hc2hc.ml,v 1.9 2003/04/17 19:25:50 athena Exp $
 */

#include "hb.h"

static const R *hb_10(R *rio, R *iio, const R *W, stride ios, int m, int dist)
{
     DK(KP250000000, +0.250000000000000000000000000000000000000000000);
     DK(KP951056516, +0.951056516295153572116439333379382143405698634);
     DK(KP587785252, +0.587785252292473129168705954639072768597652438);
     DK(KP559016994, +0.559016994374947424102293417182819058860154590);
     int i;
     for (i = m - 2; i > 0; i = i - 2, rio = rio + dist, iio = iio - dist, W = W + 18) {
	  E T3, Tk, Tw, T1w, TO, TP, T1E, T1D, Tr, TX, Ti, T1l, TZ, T10, T1s;
	  E T1p, T1z, T1B, TL, TS;
	  {
	       E T1, T2, Tu, Tv;
	       T1 = rio[0];
	       T2 = iio[-WS(ios, 5)];
	       T3 = T1 + T2;
	       Tk = T1 - T2;
	       Tu = iio[0];
	       Tv = rio[WS(ios, 5)];
	       Tw = Tu + Tv;
	       T1w = Tu - Tv;
	  }
	  {
	       E T6, Tl, Tg, Tp, T9, Tm, Td, To;
	       {
		    E T4, T5, Te, Tf;
		    T4 = rio[WS(ios, 2)];
		    T5 = iio[-WS(ios, 7)];
		    T6 = T4 + T5;
		    Tl = T4 - T5;
		    Te = iio[-WS(ios, 6)];
		    Tf = rio[WS(ios, 1)];
		    Tg = Te + Tf;
		    Tp = Te - Tf;
	       }
	       {
		    E T7, T8, Tb, Tc;
		    T7 = iio[-WS(ios, 8)];
		    T8 = rio[WS(ios, 3)];
		    T9 = T7 + T8;
		    Tm = T7 - T8;
		    Tb = rio[WS(ios, 4)];
		    Tc = iio[-WS(ios, 9)];
		    Td = Tb + Tc;
		    To = Tb - Tc;
	       }
	       TO = Tl - Tm;
	       TP = To - Tp;
	       T1E = Td - Tg;
	       T1D = T6 - T9;
	       {
		    E Tn, Tq, Ta, Th;
		    Tn = Tl + Tm;
		    Tq = To + Tp;
		    Tr = Tn + Tq;
		    TX = KP559016994 * (Tn - Tq);
		    Ta = T6 + T9;
		    Th = Td + Tg;
		    Ti = Ta + Th;
		    T1l = KP559016994 * (Ta - Th);
	       }
	  }
	  {
	       E Tz, T1n, TJ, T1r, TC, T1o, TG, T1q;
	       {
		    E Tx, Ty, TH, TI;
		    Tx = iio[-WS(ios, 2)];
		    Ty = rio[WS(ios, 7)];
		    Tz = Tx + Ty;
		    T1n = Tx - Ty;
		    TH = rio[WS(ios, 6)];
		    TI = iio[-WS(ios, 1)];
		    TJ = TH + TI;
		    T1r = TI - TH;
	       }
	       {
		    E TA, TB, TE, TF;
		    TA = rio[WS(ios, 8)];
		    TB = iio[-WS(ios, 3)];
		    TC = TA + TB;
		    T1o = TB - TA;
		    TE = iio[-WS(ios, 4)];
		    TF = rio[WS(ios, 9)];
		    TG = TE + TF;
		    T1q = TE - TF;
	       }
	       TZ = Tz + TC;
	       T10 = TG + TJ;
	       T1s = T1q - T1r;
	       T1p = T1n - T1o;
	       {
		    E T1x, T1y, TD, TK;
		    T1x = T1n + T1o;
		    T1y = T1q + T1r;
		    T1z = T1x + T1y;
		    T1B = KP559016994 * (T1x - T1y);
		    TD = Tz - TC;
		    TK = TG - TJ;
		    TL = TD + TK;
		    TS = KP559016994 * (TD - TK);
	       }
	  }
	  rio[0] = T3 + Ti;
	  iio[-WS(ios, 9)] = T1w + T1z;
	  {
	       E Ts, TM, Tj, Tt;
	       Ts = Tk + Tr;
	       TM = Tw + TL;
	       Tj = W[8];
	       Tt = W[9];
	       rio[WS(ios, 5)] = FNMS(Tt, TM, Tj * Ts);
	       iio[-WS(ios, 4)] = FMA(Tt, Ts, Tj * TM);
	  }
	  {
	       E T1t, T1F, T1Q, T1N, T1C, T1R, T1m, T1M, T1A, T1k;
	       T1t = FNMS(KP951056516, T1s, KP587785252 * T1p);
	       T1F = FNMS(KP951056516, T1E, KP587785252 * T1D);
	       T1Q = FMA(KP951056516, T1D, KP587785252 * T1E);
	       T1N = FMA(KP951056516, T1p, KP587785252 * T1s);
	       T1A = FNMS(KP250000000, T1z, T1w);
	       T1C = T1A - T1B;
	       T1R = T1B + T1A;
	       T1k = FNMS(KP250000000, Ti, T3);
	       T1m = T1k - T1l;
	       T1M = T1l + T1k;
	       {
		    E T1u, T1G, T1j, T1v;
		    T1u = T1m + T1t;
		    T1G = T1C - T1F;
		    T1j = W[14];
		    T1v = W[15];
		    rio[WS(ios, 8)] = FNMS(T1v, T1G, T1j * T1u);
		    iio[-WS(ios, 1)] = FMA(T1v, T1u, T1j * T1G);
	       }
	       {
		    E T1U, T1W, T1T, T1V;
		    T1U = T1M + T1N;
		    T1W = T1R - T1Q;
		    T1T = W[6];
		    T1V = W[7];
		    rio[WS(ios, 4)] = FNMS(T1V, T1W, T1T * T1U);
		    iio[-WS(ios, 5)] = FMA(T1V, T1U, T1T * T1W);
	       }
	       {
		    E T1I, T1K, T1H, T1J;
		    T1I = T1m - T1t;
		    T1K = T1F + T1C;
		    T1H = W[2];
		    T1J = W[3];
		    rio[WS(ios, 2)] = FNMS(T1J, T1K, T1H * T1I);
		    iio[-WS(ios, 7)] = FMA(T1J, T1I, T1H * T1K);
	       }
	       {
		    E T1O, T1S, T1L, T1P;
		    T1O = T1M - T1N;
		    T1S = T1Q + T1R;
		    T1L = W[10];
		    T1P = W[11];
		    rio[WS(ios, 6)] = FNMS(T1P, T1S, T1L * T1O);
		    iio[-WS(ios, 3)] = FMA(T1P, T1O, T1L * T1S);
	       }
	  }
	  {
	       E TQ, T11, T1c, T19, TY, T18, TT, T1d, TW, TR;
	       TQ = FNMS(KP951056516, TP, KP587785252 * TO);
	       T11 = FNMS(KP951056516, T10, KP587785252 * TZ);
	       T1c = FMA(KP951056516, TO, KP587785252 * TP);
	       T19 = FMA(KP951056516, TZ, KP587785252 * T10);
	       TW = FNMS(KP250000000, Tr, Tk);
	       TY = TW - TX;
	       T18 = TX + TW;
	       TR = FNMS(KP250000000, TL, Tw);
	       TT = TR - TS;
	       T1d = TS + TR;
	       {
		    E TU, T12, TN, TV;
		    TU = TQ + TT;
		    T12 = TY - T11;
		    TN = W[12];
		    TV = W[13];
		    iio[-WS(ios, 2)] = FMA(TN, TU, TV * T12);
		    rio[WS(ios, 7)] = FNMS(TV, TU, TN * T12);
	       }
	       {
		    E T1g, T1i, T1f, T1h;
		    T1g = T1d - T1c;
		    T1i = T18 + T19;
		    T1f = W[16];
		    T1h = W[17];
		    iio[0] = FMA(T1f, T1g, T1h * T1i);
		    rio[WS(ios, 9)] = FNMS(T1h, T1g, T1f * T1i);
	       }
	       {
		    E T14, T16, T13, T15;
		    T14 = TY + T11;
		    T16 = TT - TQ;
		    T13 = W[4];
		    T15 = W[5];
		    rio[WS(ios, 3)] = FNMS(T15, T16, T13 * T14);
		    iio[-WS(ios, 6)] = FMA(T13, T16, T15 * T14);
	       }
	       {
		    E T1a, T1e, T17, T1b;
		    T1a = T18 - T19;
		    T1e = T1c + T1d;
		    T17 = W[0];
		    T1b = W[1];
		    rio[WS(ios, 1)] = FNMS(T1b, T1e, T17 * T1a);
		    iio[-WS(ios, 8)] = FMA(T17, T1e, T1b * T1a);
	       }
	  }
     }
     return W;
}

static const tw_instr twinstr[] = {
     {TW_FULL, 0, 10},
     {TW_NEXT, 1, 0}
};

static const hc2hc_desc desc = { 10, "hb_10", twinstr, {72, 30, 30, 0}, &GENUS, 0, 0, 0 };

void X(codelet_hb_10) (planner *p) {
     X(khc2hc_dif_register) (p, hb_10, &desc);
}
