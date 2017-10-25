#include <stdio.h>
#include <unistd.h>           // close
#include <fcntl.h>            // O_RDWR, O_SYNC
#include <sys/mman.h>         // PROT_READ, PROT_WRITE

#include "socal/socal.h"      // alt_write
#include "socal/hps.h"
#include "socal/alt_gpio.h"

#include "hps_0.h"            // system configuration for HPS_LED_HEX

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

volatile unsigned long *h2p_lw_hex_addr=NULL;

#define SEG7_SET(index, seg_mask)   alt_write_word(h2p_lw_hex_addr+index,seg_mask)

static unsigned char szMap[] = {
  63, 6, 91, 79, 102, 109, 125, 7, 
  127, 111, 119, 124, 57, 94, 121, 113
};  // 0,1,2,....9, a, b, c, d, e, f

// This function write to the HEX displays using a PIO configured in the FPGA
void printhex(unsigned j) {
  alt_write_word(h2p_lw_hex_addr+0,szMap[(j      ) & 0xf]);
  alt_write_word(h2p_lw_hex_addr+1,szMap[(j >>  4) & 0xf]);
  alt_write_word(h2p_lw_hex_addr+2,szMap[(j >>  8) & 0xf]);
  alt_write_word(h2p_lw_hex_addr+3,szMap[(j >> 12) & 0xf]);
  alt_write_word(h2p_lw_hex_addr+4,szMap[(j >> 16) & 0xf]);
  alt_write_word(h2p_lw_hex_addr+5,szMap[(j >> 20) & 0xf]);
}

// This function is identical to the previous one, but writes to a global variable in memory
static unsigned memhex[6];
void printhexmem(unsigned j) {
  memhex[0] = szMap[(j      ) & 0xf];
  memhex[1] = szMap[(j >>  4) & 0xf];
  memhex[2] = szMap[(j >>  8) & 0xf];
  memhex[3] = szMap[(j >> 12) & 0xf];
  memhex[4] = szMap[(j >> 16) & 0xf];
  memhex[5] = szMap[(j >> 20) & 0xf];
}

int main(int argc, char **argv) {
  void *virtual_base;
  int fd;
  unsigned i;

  if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
    printf( "ERROR: could not open \"/dev/mem\"...\n" );
    return( 1 );
  }
  virtual_base = mmap( NULL,
		       HW_REGS_SPAN,
		       ( PROT_READ | PROT_WRITE ),
		       MAP_SHARED,
		       fd,
		       HW_REGS_BASE );	
  if( virtual_base == MAP_FAILED ) {
    printf( "ERROR: mmap() failed...\n" );
    close( fd );
    return(1);
  }
  
  h2p_lw_hex_addr=virtual_base +
    ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + SEG7_IF_BASE ) & ( unsigned long)( HW_REGS_MASK ) );

  for (i = 0; i<0xFFFFFF; i++) {
    printhex(i);
    printhexmem(i);
  }

  if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
    printf( "ERROR: munmap() failed...\n" );
    close( fd );
    return( 1 );
    
  }
  close( fd );
  return 0;
}
