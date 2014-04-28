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
/* Generated on Sat Jul  5 21:40:29 EDT 2003 */

#include "codelet-dft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_notw_c -simd -compact -variables 4 -n 7 -name n2fv_7 -with-ostride 2 -include n2f.h */

/*
 * This function contains 30 FP additions, 18 FP multiplications,
 * (or, 18 additions, 6 multiplications, 12 fused multiply/add),
 * 24 stack variables, and 14 memory accesses
 */
/*
 * Generator Id's : 
 * $Id: algsimp.ml,v 1.7 2003/03/15 20:29:42 stevenj Exp $
 * $Id: fft.ml,v 1.2 2003/03/15 20:29:42 stevenj Exp $
 * $Id: gen_notw_c.ml,v 1.9 2003/04/16 21:21:53 athena Exp $
 */

#include "n2f.h"

static void n2fv_7(const R *ri, const R *ii, R *ro, R *io, stride is, stride os, int v, int ivs, int ovs)
{
     DVK(KP900968867, +0.900968867902419126236102319507445051165919162);
     DVK(KP222520933, +0.222520933956314404288902564496794759466355569);
     DVK(KP623489801, +0.623489801858733530525004884004239810632274731);
     DVK(KP781831482, +0.781831482468029808708444526674057750232334519);
     DVK(KP974927912, +0.974927912181823607018131682993931217232785801);
     DVK(KP433883739, +0.433883739117558120475768332848358754609990728);
     int i;
     const R *xi;
     R *xo;
     xi = ri;
     xo = ro;
     BEGIN_SIMD();
     for (i = v; i > 0; i = i - VL, xi = xi + (VL * ivs), xo = xo + (VL * ovs)) {
	  V T1, Ta, Td, T4, Tc, T7, Te, T8, T9, Tj, Ti;
	  T1 = LD(&(xi[0]), ivs, &(xi[0]));
	  T8 = LD(&(xi[WS(is, 3)]), ivs, &(xi[WS(is, 1)]));
	  T9 = LD(&(xi[WS(is, 4)]), ivs, &(xi[0]));
	  Ta = VADD(T8, T9);
	  Td = VSUB(T9, T8);
	  {
	       V T2, T3, T5, T6;
	       T2 = LD(&(xi[WS(is, 1)]), ivs, &(xi[WS(is, 1)]));
	       T3 = LD(&(xi[WS(is, 6)]), ivs, &(xi[0]));
	       T4 = VADD(T2, T3);
	       Tc = VSUB(T3, T2);
	       T5 = LD(&(xi[WS(is, 2)]), ivs, &(xi[0]));
	       T6 = LD(&(xi[WS(is, 5)]), ivs, &(xi[WS(is, 1)]));
	       T7 = VADD(T5, T6);
	       Te = VSUB(T6, T5);
	  }
	  ST(&(xo[0]), VADD(T1, VADD(T4, VADD(T7, Ta))), ovs, &(xo[0]));
	  Tj = VBYI(VFMA(LDK(KP433883739), Tc, VFNMS(LDK(KP781831482), Te, VMUL(LDK(KP974927912), Td))));
	  Ti = VFMA(LDK(KP623489801), T7, VFNMS(LDK(KP222520933), Ta, VFNMS(LDK(KP900968867), T4, T1)));
	  ST(&(xo[8]), VSUB(Ti, Tj), ovs, &(xo[0]));
	  ST(&(xo[6]), VADD(Ti, Tj), ovs, &(xo[2]));
	  {
	       V Tf, Tb, Th, Tg;
	       Tf = VBYI(VFNMS(LDK(KP781831482), Td, VFNMS(LDK(KP433883739), Te, VMUL(LDK(KP974927912), Tc))));
	       Tb = VFMA(LDK(KP623489801), Ta, VFNMS(LDK(KP900968867), T7, VFNMS(LDK(KP222520933), T4, T1)));
	       ST(&(xo[10]), VSUB(Tb, Tf), ovs, &(xo[2]));
	       ST(&(xo[4]), VADD(Tb, Tf), ovs, &(xo[0]));
	       Th = VBYI(VFMA(LDK(KP781831482), Tc, VFMA(LDK(KP974927912), Te, VMUL(LDK(KP433883739), Td))));
	       Tg = VFMA(LDK(KP623489801), T4, VFNMS(LDK(KP900968867), Ta, VFNMS(LDK(KP222520933), T7, T1)));
	       ST(&(xo[12]), VSUB(Tg, Th), ovs, &(xo[0]));
	       ST(&(xo[2]), VADD(Tg, Th), ovs, &(xo[2]));
	  }
     }
     END_SIMD();
}

static const kdft_desc desc = { 7, "n2fv_7", {18, 6, 12, 0}, &GENUS, 0, 2, 0, 0 };
void X(codelet_n2fv_7) (planner *p) {
     X(kdft_register) (p, n2fv_7, &desc);
}
