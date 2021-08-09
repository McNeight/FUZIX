#include <kernel.h>
#include <blkdev.h>
#include <devsd.h>
#include <z80softspi.h>

#include "rc2014.h"

#ifdef CONFIG_SD

/*
 *	SD card bit bang. For now just a single card to get us going. We
 *	should fix the cs asm to allow for multiple cards
 *
 *	Bits
 *	7: MISO
 *	4: CLK
 *	3: \CS card 0
 *	0: MOSI
 */

uint8_t pio_c;

void pio_setup(void)
{
    spi_piostate = 0xE0;

    if (kio_present) {
      pio_c = 0x83;
      spi_port = 0x82;
    } else {
      pio_c = 0x6B;
      spi_port = 0x69;
    }
    spi_data = 0x01;
    spi_clock = 0x10;

    out(pio_c, 0xCF);		/* Mode 3 */
    out(pio_c, 0xE6);		/* MISO input, unused as input (so high Z) */
    out(pio_c, 0x07);		/* No interrupt, no mask */
}

void sd_spi_raise_cs(void)
{
    out(spi_port, spi_piostate |= 0x08);
}

void sd_spi_lower_cs(void)
{
    spi_piostate &= ~0x08;
    out(spi_port, spi_piostate);
}

void sd_spi_clock(bool go_fast) __z88dk_fastcall
{
  used(go_fast);
}

COMMON_MEMORY

bool sd_spi_receive_sector(void) __naked
{
  __asm
    ld a, (_blk_op+BLKPARAM_IS_USER_OFFSET)
    ld hl, (_blk_op+BLKPARAM_ADDR_OFFSET)
    push af
#ifdef SWAPDEV
    cp #2
    jr nz, not_swapin
    ld a,(_blk_op+BLKPARAM_SWAP_PAGE)
    call map_for_swap
    jr doread
not_swapin:
#endif
    or a
    call nz,map_process_always
doread:
    call _sd_spi_rx_sector
    pop af
    or a
    jp nz,map_kernel
    ret
  __endasm;
}

bool sd_spi_transmit_sector(void) __naked
{
  __asm
    ld a, (_blk_op+BLKPARAM_IS_USER_OFFSET)
    ld hl, (_blk_op+BLKPARAM_ADDR_OFFSET)
    push af
#ifdef SWAPDEV
    cp #2
    jr nz, not_swapout
    ld a, (_blk_op+BLKPARAM_SWAP_PAGE)
    call map_for_swap
    jr dowrite
not_swapout:
#endif
    or a
    call nz,map_process_always
dowrite:
    call _sd_spi_tx_sector
    pop af
    or a
    jp nz,map_kernel
    ret
  __endasm;
}

#endif
