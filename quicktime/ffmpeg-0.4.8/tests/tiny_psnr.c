/*
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <inttypes.h>

#define F 100
#define SIZE 2048

static uint64_t int_sqrt(uint64_t a)
{
    uint64_t ret=0;
    int s;
    uint64_t ret_sq=0;

    for(s=31; s>=0; s--){
        uint64_t b= ret_sq + (1ULL<<(s*2)) + (ret<<s)*2;
        if(b<=a){
            ret_sq=b;
            ret+= 1ULL<<s;
        }
    }
    return ret;
}

int main(int argc,char* argv[]){
    int i, j;
    uint64_t sse=0;
    uint64_t dev;
    FILE *f[2];
    uint8_t buf[2][SIZE];
    
    if(argc!=3){
        printf("tiny_psnr <file1> <file2>\n");
        return -1;
    }
    
    f[0]= fopen(argv[1], "r");
    f[1]= fopen(argv[2], "r");

    for(i=0;;){
        if( fread(buf[0], SIZE, 1, f[0]) != 1) break;
        if( fread(buf[1], SIZE, 1, f[1]) != 1) break;
        
        for(j=0; j<SIZE; i++,j++){
            const int a= buf[0][j];
            const int b= buf[1][j];
            sse += (a-b) * (a-b);
        }
    }
    
    dev= int_sqrt((sse*F*F)/i);
    
    //FIXME someone should write a integer fixpoint log() function for bitexact PSNR scores ...
    printf("stddev:%3d.%02d bytes:%d\n", (int)(dev/F), (int)(dev%F), i);
    
    return 0;
}
