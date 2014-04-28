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
/* Generated on Sat Jul  5 21:56:40 EDT 2003 */

#include "codelet-rdft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_r2hc -compact -variables 4 -n 5 -name r2hc_5 -include r2hc.h */

/*
 * This function contains 12 FP additions, 6 FP multiplications,
 * (or, 9 additions, 3 multiplications, 3 fused multiply/add),
 * 17 stack variables, and 10 memory accesses
 */
/*
 * Generator Id's : 
 * $Id: algsimp.ml,v 1.7 2003/03/15 20:29:42 stevenj Exp $
 * $Id: fft.ml,v 1.2 2003/03/15 20:29:42 stevenj Exp $
 * $Id: gen_r2hc.ml,v 1.13 2003/04/17 19:25:50 athena Exp $
 */

#include "r2hc.h"

static void r2hc_5(const R *I, R *ro, R *io, stride is, stride ros, stride ios, int v, int ivs, int ovs)
{
     DK(KP250000000, +0.250000000000000000000000000000000000000000000);
     DK(KP559016994, +0.559016994374947424102293417182819058860154590);
     DK(KP587785252, +0.587785252292473129168705954639072768597652438);
     DK(KP951056516, +0.951056516295153572116439333379382143405698634);
     int i;
     for (i = v; i > 0; i = i - 1, I = I + ivs, ro = ro + ovs, io = io + ovs) {
	  E Ta, T7, T8, T3, Tb, T6, T9, Tc;
	  Ta = I[0];
	  {
	       E T1, T2, T4, T5;
	       T1 = I[WS(is, 4)];
	       T2 = I[WS(is, 1)];
	       T7 = T2 + T1;
	       T4 = I[WS(is, 2)];
	       T5 = I[WS(is, 3)];
	       T8 = T4 + T5;
	       T3 = T1 - T2;
	       Tb = T7 + T8;
	       T6 = T4 - T5;
	  }
	  io[WS(ios, 1)] = FNMS(KP587785252, T6, KP951056516 * T3);
	  ro[0] = Ta + Tb;
	  io[WS(ios, 2)] = FMA(KP587785252, T3, KP951056516 * T6);
	  T9 = KP559016994 * (T7 - T8);
	  Tc = FNMS(KP250000000, Tb, Ta);
	  ro[WS(ros, 1)] = T9 + Tc;
	  ro[WS(ros, 2)] = Tc - T9;
     }
}

static const kr2hc_desc desc = { 5, "r2hc_5", {9, 3, 3, 0}, &GENUS, 0, 0, 0, 0, 0 };

void X(codelet_r2hc_5) (planner *p) {
     X(kr2hc_register) (p, r2hc_5, &desc);
}