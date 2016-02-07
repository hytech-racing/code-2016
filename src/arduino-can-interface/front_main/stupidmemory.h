  #include <stddef.h> 

  
void memcpyInt (short int *dstpp, unsigned char *srcpp) {// dst is destination, src is source
  
  *dstpp &= int(0);
  int MSB = int(srcpp[0]);
  *dstpp |= MSB<<(8);
  *dstpp |= srcpp[1];
 
}
    

void memcpyLong (unsigned long *dstpp, unsigned char *srcpp) {// dst is destination, src is source
  
  *dstpp &= int(0);
  unsigned long MSB = long(srcpp[0]);
  unsigned long M2SB = long(srcpp[1]);
  unsigned long L2SB = long(srcpp[2]);
  *dstpp |= MSB<<(24);
  *dstpp |= MSB<<(16);
  *dstpp |= MSB<<(8);
  *dstpp |= srcpp[3];

 
}    
