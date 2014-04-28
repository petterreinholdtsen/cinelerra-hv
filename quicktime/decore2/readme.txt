decore2

unless otherwise stated, all code is licensed under gpl (refer to gpl.txt)
(c)2001 peter ross <pross@cs.rmit.edu.au>


stuff, etc.
-----------
- only supports iframe decoding. handles a subset of the mpeg-4 format
  (but works with xvid & divx4.02 encoded video)
- uyvy iframe decoding on my machine is 42 fps; compared to 37 using divx4.02
- low level modules (quant,idct,transfer) are taken directly from encore2
- higher level structures (MACROBLOCK, VECTOR, IMAGE) are a bit different


api
---
decore2 is simular to encore2: create, decode and destroy.


code overview
-------------
decoding steps for a single frame are as follows:

decoder_decode()
{
   read VOL header (continue if not found)
   read VOP header

   if I_VOP, call decoder_iframe()
   if P_VOP, call decoder_iframe()
   if N_VOP, swap ref & cur image pointer      (N_VOP is when "vop_coded==0")

   color conversion (yuv->whatever)

   swap ref & cur image pointers
}



decoder_iframe()
{
   for each macroblock

      read in mb header stuff (quant, cbp)

      for each block

         read dc coeff
         dc prediction & ac_direction

         if block coded
            read ac coeffs

         ac prediction (& backup for future prediction)

         dequant
         idct
         transfer 16bit to 8bit
}



decoder_pframe()
{
   for each macroblock

      read in mb header stuff (quant, cbp, mvs)
      motion prediction

      for each block

         ... todo
}
