export CROSS_AS=fcc -m6800 -c
export CROSS_LD=ld6800
export CROSS_CC=fcc
export CROSS_CCOPTS= -m6800 -Os -X -c -I$(ROOT_DIR)/cpu-6800 -I$(ROOT_DIR)/platform/platform-$(TARGET) -I$(ROOT_DIR)/include
export CROSS_CC_SEG1=
export CROSS_CC_SEG2=
export CROSS_CC_SEG3=
export CROSS_CC_SEGDISC= -Tdiscard
export CROSS_CC_VIDEO=
export CROSS_CC_FONT=
export CROSS_CC_NETWORK=
export ASOPTS=
export ASMEXT = .s
export BINEXT = .o
export BITS=16
export EXECFORMAT=16
