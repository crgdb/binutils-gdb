/* Opcode table for the TXVU
   Copyright 1998 Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "ansidecl.h"
#include "dis-asm.h"
#include "opcode/txvu.h"

#ifndef NULL
#define NULL 0
#endif

#if defined (__STDC__) || defined (ALMOST_STDC)
#define XCONCAT2(a,b)	a##b
#else
#define XCONCAT2(a,b)	a/**/b
#endif
#define CONCAT2(a,b)	XCONCAT2(a,b)

/* ??? One can argue it's preferable to have the PARSE_FN support in tc-vxvu.c
   and the PRINT_FN support in txvu-dis.c.  For this project I like having
   them all in one place.  */

#define PARSE_FN(fn) \
static long CONCAT2 (parse_,fn) \
     PARAMS ((char **, const char **));
#define INSERT_FN(fn) \
static TXVU_INSN CONCAT2 (insert_,fn) \
     PARAMS ((TXVU_INSN, const struct txvu_operand *, \
	      int, long, const char **))
#define EXTRACT_FN(fn) \
static long CONCAT2 (extract_,fn) \
     PARAMS ((TXVU_INSN, const struct txvu_operand *, \
	      int, int *))
#define PRINT_FN(fn) \
static void CONCAT2 (print_,fn) \
     PARAMS ((disassemble_info *, TXVU_INSN, long));

PARSE_FN (dotdest);
INSERT_FN (dotdest);
EXTRACT_FN (dotdest);
PRINT_FN (dotdest);

PARSE_FN (dotdest1);

PARSE_FN (bc);
EXTRACT_FN (bc);
PRINT_FN (sdest);

PARSE_FN (vfreg);
PRINT_FN (vfreg);

PARSE_FN (bcftreg);
PRINT_FN (bcftreg);

PARSE_FN (accdest);
PRINT_FN (accdest);

INSERT_FN (xyz);

PARSE_FN (ireg);
PRINT_FN (ireg);

PARSE_FN (freg);
PRINT_FN (freg);

PARSE_FN (ffstreg);
INSERT_FN (ffstreg);
EXTRACT_FN (ffstreg);
PRINT_FN (ffstreg);

PARSE_FN (vi01);
PRINT_FN (vi01);

INSERT_FN (luimm12);
EXTRACT_FN (luimm12);

INSERT_FN (luimm15);
EXTRACT_FN (luimm15);

/* Various types of TXVU operands, including insn suffixes.

   Fields are:

   BITS SHIFT FLAGS PARSE_FN INSERT_FN EXTRACT_FN PRINT_FN

   Operand values are 128 + table index.  This allows ASCII chars to be
   included in the syntax spec.  */

const struct txvu_operand txvu_operands[] =
{
  /* place holder (??? not sure if needed) */
#define UNUSED 128
  { 0 },

  /* Operands that exist in the same place for essentially the same purpose
     in both upper and lower instructions.  These don't have a U or L prefix.
     Operands specific to the upper or lower instruction are so prefixed.  */

  /* Destination indicator attached to mnemonic, with leading '.'.
     After parsing this, the value is stored in global `dest' so that the
     register parser can verify the same choice of xyzw is used.  */
#define DOTDEST (UNUSED + 1)
  { 4, TXVU_SHIFT_DEST, TXVU_OPERAND_SUFFIX,
      parse_dotdest, insert_dotdest, extract_dotdest, print_dotdest },

  /* ft reg, with vector specification same as DOTDEST */
#define VFTREG (DOTDEST + 1)
  { 5, TXVU_SHIFT_TREG, 0, parse_vfreg, 0, 0, print_vfreg },

  /* fs reg, with vector specification same as DOTDEST */
#define VFSREG (VFTREG + 1)
  { 5, TXVU_SHIFT_SREG, 0, parse_vfreg, 0, 0, print_vfreg },

  /* fd reg, with vector specification same as DOTDEST */
#define VFDREG (VFSREG + 1)
  { 5, TXVU_SHIFT_DREG, 0, parse_vfreg, 0, 0, print_vfreg },

  /* Upper word operands.  */

  /* broadcast */
#define UBC (VFDREG + 1)
  { 2, 0, TXVU_OPERAND_SUFFIX, parse_bc, 0, extract_bc, print_sdest },

  /* ftreg in broadcast case */
#define UBCFTREG (UBC + 1)
  { 5, TXVU_SHIFT_TREG, 0, parse_bcftreg, 0, 0, print_bcftreg },

  /* accumulator dest */
#define UACCDEST (UBCFTREG + 1)
  { 0, 0, 0, parse_accdest, 0, 0, print_accdest },

  /* The XYZ operand is a fake one that is used to ensure only "xyz" is
     specified.  It simplifies the opmula and opmsub entries.  */
#define UXYZ (UACCDEST + 1)
  { 0, 0, TXVU_OPERAND_FAKE, 0, insert_xyz, 0, 0 },

  /* Lower word operands.  */

  /* 5 bit signed immediate.  */
#define LIMM5 (UXYZ + 1)
  { 5, 6, TXVU_OPERAND_SIGNED, 0, 0, 0, 0 },

  /* 11 bit signed immediate.  */
#define LIMM11 (LIMM5 + 1)
  { 11, 0, TXVU_OPERAND_SIGNED, 0, 0, 0, 0 },

  /* 15 bit unsigned immediate.  */
#define LUIMM15 (LIMM11 + 1)
  { 15, 0, 0, 0, insert_luimm15, extract_luimm15, 0 },

  /* ID register.  */
#define LIDREG (LUIMM15 + 1)
  { 5, 6, 0, parse_ireg, 0, 0, print_ireg },

  /* IS register.  */
#define LISREG (LIDREG + 1)
  { 5, 11, 0, parse_ireg, 0, 0, print_ireg },

  /* IT register.  */
#define LITREG (LISREG + 1)
  { 5, 16, 0, parse_ireg, 0, 0, print_ireg },

  /* FS reg, with FSF field selector.  */
#define LFSFFSREG (LITREG + 1)
  { 5, 11, 0, parse_ffstreg, insert_ffstreg, extract_ffstreg, print_ffstreg },

  /* FS reg, no selector (choice of x,y,z,w is provided by opcode).  */
#define LFSREG (LFSFFSREG + 1)
  { 5, 11, 0, parse_freg, 0, 0, print_freg },

  /* FT reg, with FTF field selector.  */
#define LFTFFTREG (LFSREG + 1)
  { 5, 16, 0, parse_ffstreg, insert_ffstreg, extract_ffstreg, print_ffstreg },

  /* VI01 register.  */
#define LVI01 (LFTFFTREG + 1)
  { 0, 0, 0, parse_vi01, 0, 0, print_vi01 },

  /* 24 bit unsigned immediate.  */
#define LUIMM24 (LVI01 + 1)
  { 24, 0, 0, 0, 0, 0, 0 },

  /* 12 bit unsigned immediate, split into 1 and 11 bit pieces.  */
#define LUIMM12 (LUIMM24 + 1)
  { 12, 0, 0, 0, insert_luimm12, extract_luimm12, 0 },

  /* 11 bit pc-relative signed immediate.  */
#define LPCREL11 (LUIMM12 + 1)
  { 11, 0, TXVU_OPERAND_SIGNED + TXVU_OPERAND_RELATIVE_BRANCH, 0, 0, 0, 0 },

  /* Destination indicator, single letter only, with leading '.'.  */
#define LDOTDEST1 (LPCREL11 + 1)
  { 4, TXVU_SHIFT_DEST, TXVU_OPERAND_SUFFIX,
      /* Note that we borrow the insert/extract/print functions from the
	 vector case.  */
      parse_dotdest1, insert_dotdest, extract_dotdest, print_dotdest },

/* end of list place holder */
  { 0 }
};

/* Macros to put a field's value into the right place.  */
/* ??? If assembler needs these, move to opcode/txvu.h.  */

/* value X, B bits, shift S */
#define V(x,b,s) (((x) & ((1 << (b)) - 1)) << (s))

/* Field value macros for both upper and lower instructions.
   These shift a value into the right place in the instruction.  */

/* [FI] T reg field (remember it's V for value, not vector, here).  */
#define VT(x) V ((x), 5, TXVU_SHIFT_TREG)
/* [FI] S reg field.  */
#define VS(x) V ((x), 5, TXVU_SHIFT_SREG)
/* [FI] D reg field.  */
#define VD(x) V ((x), 5, TXVU_SHIFT_DREG)
/* DEST field.  */
#define VDEST(x) V ((x), 4, 21)

/* Masks for fields in both upper and lower instructions.
   These mask out all bits but the ones for the field in the instruction.  */

#define MT VT (~0)
#define MS VS (~0)
#define MD VD (~0)
#define MDEST VDEST (~0)

/* Upper instruction Value macros.  */

/* Upper Flag bits.  */
#define VUF(x) V ((x), 5, 27)
/* Upper REServed two bits next to flag bits.  */
#define VURES(x) V ((x), 2, 25)
/* 4 bit opcode field.  */
#define VUOP4(x) V ((x), 4, 2)
/* 6 bit opcode field.  */
#define VUOP6(x) V ((x), 6, 0)
/* 9 bit opcode field.  */
#define VUOP9(x) V ((x), 9, 2)
/* 11 bit opcode field.  */
#define VUOP11(x) V ((x), 11, 0)
/* BroadCast field.  */
#define VUBC(x) V ((x), 2, 0)

/* Upper instruction field masks.  */
#define MUUBITS (VUF (~0) + VURES (~0))
#define MURES VURES (~0)
#define MUOP4 VUOP4 (~0)
#define MUOP6 VUOP6 (~0)
#define MUOP9 VUOP9 (~0)
#define MUOP11 VUOP11 (~0)

/* A space, separates instruction name (mnemonic + mnemonic operands) from
   operands.  */
#define SP ' '
/* Commas separate operands.  */
#define C ','
/* Special I,P,Q,R operands.  */
#define I 'i'
#define P 'p'
#define Q 'q'
#define R 'r'

/* TXVU instructions.
   [??? some of these comments are left over from the ARC port from which
   this code is borrowed, delete in time]

   Longer versions of insns must appear before shorter ones (if gas sees
   "lsr r2,r3,1" when it's parsing "lsr %a,%b" it will think the ",1" is
   junk).  This isn't necessary for `ld' because of the trailing ']'.

   Instructions that are really macros based on other insns must appear
   before the real insn so they're chosen when disassembling.  Eg: The `mov'
   insn is really the `and' insn.

   This table is best viewed on a wide screen (161 columns).  I'd prefer to
   keep it this way.  The rest of the file, however, should be viewable on an
   80 column terminal.  */

/* ??? This table also includes macros: asl, lsl, and mov.  The ppc port has
   a more general facility for dealing with macros which could be used if
   we need to.  */

/* These tables can't be `const' because members `next_asm' and `next_dis' are
   computed at run-time.  We could split this into two, as that would put the
   constant stuff into a readonly section.  */

struct txvu_opcode txvu_upper_opcodes[] = {

  /* Macros appear first, so the disassembler will try them first.  */
  /* ??? Any aliases?  */
  /* ??? When close to being finished, clean up by aligning fields.  */

  /* The rest of these needn't be sorted, but it helps to find them if they are.  */
  { "abs", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x1fd) },
  { "add", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x28) },
  { "addi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x22) },
  { "addq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x20) },
  { "add", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + VUOP4 (~0), VUOP4 (0) },
  { "adda", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2bc) },
  { "addai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x23e) },
  { "addaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x23c) },
  { "adda", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0xf) },
  { "clip", { DOTDEST, SP, VFSREG }, MURES + MDEST + MT + MUOP11, VDEST (0xf) + VUOP11 (0x1ff) },
  { "ftoi0", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x17c) },
  { "ftoi4", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x17d) },
  { "ftoi12", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x17e) },
  { "ftoi15", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x17f) },
  { "itof0", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x13c) },
  { "itof4", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x13d) },
  { "itof12", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x13e) },
  { "itof15", { DOTDEST, SP, VFTREG, C, VFSREG }, MURES + MUOP11, VUOP11 (0x13f) },
  { "madd", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x29) },
  { "maddi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x23) },
  { "maddq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x21) },
  { "madd", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + MUOP4, VUOP4 (0x2) },
  { "madda", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2bd) },
  { "maddai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x23f) },
  { "maddaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x23d) },
  { "madda", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0x2f) },
  { "max", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2b) },
  { "maxi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x2d) },
  { "max", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + MUOP4, VUOP4 (0x4) },
  /* ??? mini or min? */
  { "mini", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2f) },
  { "minii", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x1f) },
  { "mini", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + MUOP4, VUOP4 (0x5) },
  { "msub", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2d) },
  { "msubi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x27) },
  { "msubq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x25) },
  { "msub", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + MUOP4, VUOP4 (0x3) },
  { "msuba", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2fd) },
  { "msubai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x27f) },
  { "msubaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x27d) },
  { "msuba", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0x3f) },
  { "mul", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2a) },
  { "muli", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x1e) },
  { "mulq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x1c) },
  { "mul", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + VUOP4 (~0), VUOP4 (6) },
  { "mula", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2be) },
  { "mulai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x1fe) },
  { "mulaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x1fc) },
  { "mula", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0x6f) },
  { "nop", { 0 }, MURES + MDEST + MT + MS + MUOP11, VUOP11 (0x2ff) },
  { "opmula", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG, UXYZ }, MURES + MUOP11, VUOP11 (0x2fe) },
  { "opmsub", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG, UXYZ }, MURES + MUOP6, VUOP6 (0x2e) },
  { "sub", { DOTDEST, SP, VFDREG, C, VFSREG, C, VFTREG }, MURES + MUOP6, VUOP6 (0x2c) },
  { "subi", { DOTDEST, SP, VFDREG, C, VFSREG, C, I }, MURES + MT + MUOP6, VUOP6 (0x26) },
  { "subq", { DOTDEST, SP, VFDREG, C, VFSREG, C, Q }, MURES + MT + MUOP6, VUOP6 (0x24) },
  { "sub", { UBC, DOTDEST, SP, VFDREG, C, VFSREG, C, UBCFTREG }, MURES + VUOP4 (~0), VUOP4 (1) },
  { "suba", { DOTDEST, SP, UACCDEST, C, VFSREG, C, VFTREG }, MURES + MUOP11, VUOP11 (0x2fc) },
  { "subai", { DOTDEST, SP, UACCDEST, C, VFSREG, C, I }, MURES + MT + MUOP11, VUOP11 (0x27e) },
  { "subaq", { DOTDEST, SP, UACCDEST, C, VFSREG, C, Q }, MURES + MT + MUOP11, VUOP11 (0x27c) },
  { "suba", { UBC, DOTDEST, SP, UACCDEST, C, VFSREG, C, UBCFTREG }, MURES + MUOP9, VUOP9 (0x1f) }
};
const int txvu_upper_opcodes_count = sizeof (txvu_upper_opcodes) / sizeof (txvu_upper_opcodes[0]);

/* Lower instruction Value macros.  */

/* 6 bit opcode.  */
#define VLOP6(x) V ((x), 6, 0)
/* 7 bit opcode.  */
#define VLOP7(x) V ((x), 7, 25)
/* 11 bit opcode.  */
#define VLOP11(x) V ((x), 11, 0)
/* 11 bit immediate.  */
#define VLIMM11(x) V ((x), 11, 0)
/* FTF field.  */
#define VLFTF(x) V ((x), 2, 23)
/* FSF field.  */
#define VLFSF(x) V ((x), 2, 21)
/* Upper bit of 12 bit unsigned immediate.  */
#define VLUIMM12TOP(x) V ((x), 1, 21)
/* Upper 4 bits of 15 bit unsigned immediate.  */
#define VLUIMM15TOP(x) VDEST (x)

/* Lower instruction field masks.  */
#define MLOP6 VLOP6 (~0)
#define MLOP7 VLOP7 (~0)
#define MLOP11 VLOP11 (~0)
#define MLIMM11 VLIMM11 (~0)
#define MLB24 V (1, 1, 24)
#define MLUIMM12TOP VLUIMM12TOP (~0)
/* 12 bit unsigned immediates are split into two parts, 1 bit and 11 bits.
   The upper 1 bit is part of the `dest' field.  This mask is for the
   other 3 bits of the dest field.  */
#define MLUIMM12UNUSED V (7, 3, 22)
#define MLUIMM15TOP MDEST

struct txvu_opcode txvu_lower_opcodes[] = {

  /* Macros appear first, so the disassembler will try them first.  */
  /* ??? Any aliases?  */
  /* ??? There isn't an explicit nop.  Apparently it's "move vf0,vf0".  */
  { "nop", { 0 }, 0xffffffff, VLOP7 (0x40) + VLIMM11 (0x33c) },

  /* The rest of these needn't be sorted, but it helps to find them if they are.  */
  { "b", { SP, LPCREL11 }, MLOP7 + MDEST + MT + MS, VLOP7 (0x20) },
  { "bal", { SP, LITREG, C, LPCREL11 }, MLOP7 + MDEST + MS, VLOP7 (0x21) },
  { "div", { SP, Q, C, LFSFFSREG, C, LFTFFTREG }, MLOP7 + MLOP11, VLOP7 (0x40) + VLOP11 (0x3bc) },
  { "eatan", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7fd) },
  { "eatanxy", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x77c) },
  { "eatanxz", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x77d) },
  { "eexp", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7fe) },
  { "eleng", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x74e) },
  { "ercpr", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7be) },
  { "erleng", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x73f) },
  { "ersadd", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x73d) },
  { "ersqrt", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7bd) },
  { "esadd", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x73c) },
  { "esin", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7fc) },
  { "esqrt", { SP, P, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLOP11, VLOP7 (0x40) + VLOP11 (0x7bc) },
  { "esum", { SP, P, C, LFSREG }, MLOP7 + MDEST + MT + MLOP11, VLOP7 (0x40) + VDEST (0xf) + VLOP11 (0x77e) },
  { "fcand", { SP, LVI01, C, LUIMM24 }, MLOP7 + MLB24, VLOP7 (0x12) },
  { "fceq", { SP, LVI01, C, LUIMM24 }, MLOP7 + MLB24, VLOP7 (0x10) },
  { "fcget", { SP, LITREG }, MLOP7 + MDEST + MS + MLIMM11, VLOP7 (0x1c) },
  { "fcor", { SP, LVI01, C, LUIMM24 }, MLOP7 + MLB24, VLOP7 (0x13) },
  { "fcset", { SP, LVI01, C, LUIMM24 }, MLOP7 + MLB24, VLOP7 (0x11) },
  { "fmand", { SP, LITREG, C, LISREG }, MLOP7 + MDEST + MLIMM11, VLOP7 (0x1a) },
  { "fmeq", { SP, LITREG, C, LISREG }, MLOP7 + MDEST + MLIMM11, VLOP7 (0x18) },
  { "fmor", { SP, LITREG, C, LISREG }, MLOP7 + MDEST + MLIMM11, VLOP7 (0x1b) },
  { "fsand", { SP, LITREG, C, LUIMM12 }, MLOP7 + MLUIMM12UNUSED + MS, VLOP7 (0x16) },
  { "fseq", { SP, LITREG, C, LUIMM12 }, MLOP7 + MLUIMM12UNUSED + MS, VLOP7 (0x14) },
  { "fsor", { SP, LITREG, C, LUIMM12 }, MLOP7 + MLUIMM12UNUSED + MS, VLOP7 (0x17) },
  { "fsset", { SP, LITREG, C, LUIMM12 }, MLOP7 + MLUIMM12UNUSED + MS, VLOP7 (0x15) },
  { "iadd", { SP, LIDREG, C, LISREG, C, LITREG }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x30) },
  { "iaddi", { SP, LITREG, C, LISREG, C, LIMM5 }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x32) },
  { "iaddiu", { SP, LITREG, C, LISREG, C, LUIMM15 }, MLOP7, VLOP7 (0x08) },
  { "iand", { SP, LIDREG, C, LISREG, C, LITREG }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x34) },
  { "ibeq", { SP, LITREG, C, LISREG, C, LPCREL11 }, MLOP7 + MDEST, VLOP7 (0x28) },
  { "ibgez", { SP, LISREG, C, LPCREL11 }, MLOP7 + MDEST + MT, VLOP7 (0x2f) },
  { "iblez", { SP, LISREG, C, LPCREL11 }, MLOP7 + MDEST + MT, VLOP7 (0x2e) },
  { "ibltz", { SP, LISREG, C, LPCREL11 }, MLOP7 + MDEST + MT, VLOP7 (0x2c) },
  { "ibne", { SP, LITREG, C, LISREG, C, LPCREL11 }, MLOP7 + MDEST, VLOP7 (0x29) },
  /* FIXME: Need to not require commas around parens.  */
  { "ilw", { LDOTDEST1, SP, LITREG, C, LIMM11, '(', LISREG, ')' }, MLOP7, VLOP7 (0x04) },
  { "ilwr", { LDOTDEST1, SP, LITREG, C, '(', LISREG, ')' }, MLOP7, VLOP7 (0x40) + VLIMM11 (0x3fe) },
  { "ior", { SP, LIDREG, C, LISREG, C, LITREG }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x34) },
  { "isub", { SP, LIDREG, C, LISREG, C, LITREG }, MLOP7 + MDEST + MLOP6, VLOP7 (0x40) + VLOP6 (0x31) },
  { "isubiu", { SP, LITREG, C, LISREG, C, LUIMM15 }, MLOP7, VLOP7 (0x09) },
  { "isw", { LDOTDEST1, SP, LITREG, C, LIMM11, '(', LISREG, ')' }, MLOP7, VLOP7 (0x05) },
  { "iswr", { LDOTDEST1, SP, LITREG, C, '(', LISREG, ')' }, MLOP7, VLOP7 (0x40) + VLIMM11 (0x3ff) },
  { "jalr", { SP, LITREG, C, LISREG }, MLOP7 + MDEST + MLIMM11, VLOP7 (0x25) },
  { "jr", { SP, LISREG }, MLOP7 + MDEST + MT + MLIMM11, VLOP7 (0x24) },
  { "lq", { DOTDEST, SP, VFTREG, C, LIMM11, '(', LISREG, ')' }, MLOP7, VLOP7 (0x00) },
  /* FIXME: No commas around -/+.  */
  { "lqd", { DOTDEST, SP, VFTREG, C, LIMM11, '(', '-', '-', LISREG, ')' }, MLOP7, VLOP7 (0x40) + VLIMM11 (0x37e) },
  { "lqi", { DOTDEST, SP, VFTREG, C, LIMM11, '(', LISREG, '+', '+', ')' }, MLOP7, VLOP7 (0x40) + VLIMM11 (0x37c) },
  /* Only a single VF reg is allowed here.  We can use VFTREG because LDOTDEST1
     handles verifying only a single choice of xyzw is present.  */
  { "mfir", { LDOTDEST1, SP, VFTREG, C, LISREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3fc) },
  { "mfp", { DOTDEST, SP, VFTREG, C, P }, MLOP7 + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x67c) },
  { "move", { DOTDEST, SP, VFTREG, C, VFSREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x33c) },
  { "mr32", { DOTDEST, SP, VFTREG, C, VFSREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x33d) },
  { "mtir", { LDOTDEST1, SP, LITREG, C, LFSREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3fd) },
  { "rget", { DOTDEST, SP, VFTREG, C, R }, MLOP7 + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x43d) },
  { "rinit", { SP, R, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x43e) },
  { "rnext", { DOTDEST, SP, VFTREG, C, R }, MLOP7 + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x43c) },
  { "rsqrt", { SP, Q, C, LFSFFSREG, C, LFTFFTREG }, MLOP7 + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3be) },
  { "rxor", { R, C, LFSFFSREG }, MLOP7 + VLFTF (~0) + MT + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x43f) },
  { "sq", { DOTDEST, SP, VFTREG, C, LIMM11, '(', LISREG, ')' }, MLOP7, VLOP7 (0x01) },
  /* FIXME: No commas around -/+.  */
  { "sqd", { DOTDEST, SP, VFTREG, C, LIMM11, '(', '-', '-', LISREG, ')' }, MLOP7, VLOP7 (0x40) + VLIMM11 (0x37f) },
  { "sqi", { DOTDEST, SP, VFTREG, C, LIMM11, '(', LISREG, '+', '+', ')' }, MLOP7, VLOP7 (0x40) + VLIMM11 (0x37d) },
  { "sqrt", { SP, Q, C, LFTFFTREG }, MLOP7 + VLFSF (~0) + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x3bd) },
  { "waitp", { 0 }, 0xffffffff, VLOP7 (0x40) + VLIMM11 (0x7bf) },
  { "waitq", { 0 }, 0xffffffff, VLOP7 (0x40) + VLIMM11 (0x3bf) },
  { "xgkick", { LISREG }, MLOP7 + MDEST + MT + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x6fc) },
  { "xitop", { LITREG }, MLOP7 + MDEST + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x6bd) },
  { "xtop", { LITREG }, MLOP7 + MDEST + MS + MLIMM11, VLOP7 (0x40) + VLIMM11 (0x6bc) }
};
const int txvu_lower_opcodes_count = sizeof (txvu_lower_opcodes) / sizeof (txvu_lower_opcodes[0]);

/* Indexed by first letter of opcode.  Points to chain of opcodes with same
   first letter.  */
/* ??? One can certainly use a better hash.  Later.  */
static struct txvu_opcode *upper_opcode_map[26 + 1];
static struct txvu_opcode *lower_opcode_map[26 + 1];

/* Indexed by insn code.  Points to chain of opcodes with same insn code.  */
static struct txvu_opcode *upper_icode_map[(1 << TXVU_ICODE_HASH_SIZE) - 1];
static struct txvu_opcode *lower_icode_map[(1 << TXVU_ICODE_HASH_SIZE) - 1];

/* Initialize any tables that need it.
   Must be called once at start up (or when first needed).

   FLAGS is currently unused but is intended to control initialization.  */

void
txvu_opcode_init_tables (flags)
     int flags;
{
  static int init_p = 0;

  /* We may be intentionally called more than once (for example gdb will call
     us each time the user switches cpu).  These tables only need to be init'd
     once though.  */
  /* ??? We can remove the need for txvu_opcode_supported by taking it into
     account here, but I'm not sure I want to do that yet (if ever).  */
  if (!init_p)
    {
      int i,n;

      memset (upper_opcode_map, 0, sizeof (upper_opcode_map));
      memset (upper_icode_map, 0, sizeof (upper_icode_map));

      /* Scan the table backwards so macros appear at the front.  */
      for (i = txvu_upper_opcodes_count - 1; i >= 0; --i)
	{
	  int opcode_hash = TXVU_HASH_UPPER_OPCODE (txvu_upper_opcodes[i].mnemonic);
	  int icode_hash = TXVU_HASH_UPPER_ICODE (txvu_upper_opcodes[i].value);

	  txvu_upper_opcodes[i].next_asm = upper_opcode_map[opcode_hash];
	  upper_opcode_map[opcode_hash] = &txvu_upper_opcodes[i];

	  txvu_upper_opcodes[i].next_dis = upper_icode_map[icode_hash];
	  upper_icode_map[icode_hash] = &txvu_upper_opcodes[i];
	}

      memset (lower_opcode_map, 0, sizeof (lower_opcode_map));
      memset (lower_icode_map, 0, sizeof (lower_icode_map));

      /* Scan the table backwards so macros appear at the front.  */
      for (i = txvu_lower_opcodes_count - 1; i >= 0; --i)
	{
	  int opcode_hash = TXVU_HASH_LOWER_OPCODE (txvu_lower_opcodes[i].mnemonic);
	  int icode_hash = TXVU_HASH_LOWER_ICODE (txvu_lower_opcodes[i].value);

	  txvu_lower_opcodes[i].next_asm = lower_opcode_map[opcode_hash];
	  lower_opcode_map[opcode_hash] = &txvu_lower_opcodes[i];

	  txvu_lower_opcodes[i].next_dis = lower_icode_map[icode_hash];
	  lower_icode_map[icode_hash] = &txvu_lower_opcodes[i];
	}

      init_p = 1;
    }
}

/* Return the first insn in the chain for assembling upper INSN.  */

const struct txvu_opcode *
txvu_upper_opcode_lookup_asm (insn)
     const char *insn;
{
  return upper_opcode_map[TXVU_HASH_UPPER_OPCODE (insn)];
}

/* Return the first insn in the chain for assembling lower INSN.  */

const struct txvu_opcode *
txvu_lower_opcode_lookup_asm (insn)
     const char *insn;
{
  return lower_opcode_map[TXVU_HASH_LOWER_OPCODE (insn)];
}

/* Return the first insn in the chain for disassembling upper INSN.  */

const struct txvu_opcode *
txvu_upper_opcode_lookup_dis (insn)
     TXVU_INSN insn;
{
  return upper_icode_map[TXVU_HASH_UPPER_ICODE (insn)];
}

/* Return the first insn in the chain for disassembling lower INSN.  */

const struct txvu_opcode *
txvu_lower_opcode_lookup_dis (insn)
     TXVU_INSN insn;
{
  return lower_icode_map[TXVU_HASH_LOWER_ICODE (insn)];
}

/* Value of DEST in use.
   Each of the registers must specify the same value as the opcode.
   ??? Perhaps remove the duplication?  */
static int dest;

/* Value of BC to use.
   The register specified for the ftreg must match the broadcast register
   specified in the opcode.  */
static int bc;

/* Init fns.
   These are called before doing each of the respective activities.  */

/* Called by the assembler before parsing an instruction.  */

void
txvu_opcode_init_parse ()
{
  dest = -1;
  bc = -1;
}

/* Called by the disassembler before printing an instruction.  */

void
txvu_opcode_init_print ()
{
  dest = -1;
  bc = -1;
}

/* Multiple destination choice support.
   The "dest" string selects any combination of x,y,z,w.
   [The letters are ordered that way to follow the manual's style.]  */

/* Utility to parse a `dest' spec.
   Return the found value.
   *PSTR is set to the character that terminated the parsing.
   It is up to the caller to do any error checking.  */

static long
_parse_dest (pstr)
     char **pstr;
{
  long dest = 0;

  while (**pstr)
    {
      switch (**pstr)
	{
	case 'x' : case 'X' : dest |= TXVU_DEST_X; break;
	case 'y' : case 'Y' : dest |= TXVU_DEST_Y; break;
	case 'z' : case 'Z' : dest |= TXVU_DEST_Z; break;
	case 'w' : case 'W' : dest |= TXVU_DEST_W; break;
	default : return dest;
	}
      ++*pstr;
    }

  return dest;
}

static long
parse_dotdest (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  long dest;

  if (**pstr != '.')
    {
      *errmsg = "missing `.'";
      return 0;
    }

  ++*pstr;
  dest = _parse_dest (pstr);
  if (dest == 0 || isalnum (**pstr))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }

  *errmsg = NULL;
  return dest;
}

/* Parse a `dest' spec where only a single letter is allowed,
   but the encoding handles all four.  */

static long
parse_dotdest1 (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char c;
  long dest;

  if (**pstr != '.')
    {
      *errmsg = "missing `.'";
      return 0;
    }

  ++*pstr;
  switch (**pstr)
    {
    case 'x' : case 'X' : dest |= TXVU_DEST_X; break;
    case 'y' : case 'Y' : dest |= TXVU_DEST_Y; break;
    case 'z' : case 'Z' : dest |= TXVU_DEST_Z; break;
    case 'w' : case 'W' : dest |= TXVU_DEST_W; break;
    default : *errmsg = "invalid `dest'"; return 0;
    }
  ++*pstr;
  c == tolower (**pstr);
  if (c == 'x' || c == 'y' || c == 'z' || c == 'w')
    {
      *errmsg = "only one of x,y,z,w can be specified";
      return 0;
    }
  if (isalnum (**pstr))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }

  *errmsg = NULL;
  return dest;
}

static TXVU_INSN
insert_dotdest (insn, operand, mods, value, errmsg)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     long value;
     const char **errmsg;
{
  /* Record the DEST value in use so the register parser can use it.  */
  dest = value;
  if (errmsg)
    *errmsg = NULL;
  return insn |= value << operand->shift;
}

static long
extract_dotdest (insn, operand, mods, pinvalid)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     int *pinvalid;
{
  /* Record the DEST value in use so the register printer can use it.  */
  dest = (insn >> operand->shift) & ((1 << operand->bits) - 1);
  return dest;
}

/* Utility to print a multiple dest spec.  */

static void
_print_dest (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  if (value & TXVU_DEST_X)
    (*info->fprintf_func) (info->stream, "x");
  if (value & TXVU_DEST_Y)
    (*info->fprintf_func) (info->stream, "y");
  if (value & TXVU_DEST_Z)
    (*info->fprintf_func) (info->stream, "z");
  if (value & TXVU_DEST_W)
    (*info->fprintf_func) (info->stream, "w");
}

static void
print_dotdest (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  (*info->fprintf_func) (info->stream, ".");
  _print_dest (info, insn, value);
}

/* Utilities for single destination choice handling.  */

static long
_parse_sdest (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char c;
  long dest = 0;

  switch (**pstr)
    {
    case 'x' : case 'X' : dest = TXVU_SDEST_X; break;
    case 'y' : case 'Y' : dest = TXVU_SDEST_Y; break;
    case 'z' : case 'Z' : dest = TXVU_SDEST_Z; break;
    case 'w' : case 'W' : dest = TXVU_SDEST_W; break;
    default : *errmsg = "only one of x,y,z,w can be specified"; return 0;
    }
  ++*pstr;
  c == tolower (**pstr);
  if (c == 'x' || c == 'y' || c == 'z' || c == 'w')
    {
      *errmsg = "only one of x,y,z,w can be specified";
      return 0;
    }
  if (isalnum (**pstr))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }

  *errmsg = NULL;
  return dest;
}

static void
print_sdest (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  char c;

  switch (value)
    {
    case TXVU_SDEST_X : c = 'x'; break;
    case TXVU_SDEST_Y : c = 'y'; break;
    case TXVU_SDEST_Z : c = 'z'; break;
    case TXVU_SDEST_W : c = 'w'; break;
    }

  (*info->fprintf_func) (info->stream, "%c", c);
}

/* Broadcase field.  */

static long
parse_bc (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  long value = _parse_sdest (pstr, errmsg);

  if (*errmsg)
    return 0;
  /* Save value for later verification in register parsing.  */
  bc = value;
  return value;
}

/* During the extraction process, save the bc field for use in
   printing the bc register.  */

static long
extract_bc (insn, operand, mods, pinvalid)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     int *pinvalid;
{
  bc = insn & 3;
  return bc;
}

static long
parse_vfreg (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;
  int reg_dest;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'f')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  reg_dest = _parse_dest (&str);
  if (reg_dest == 0 || isalnum (*str))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }
  if (reg_dest != dest)
    {
      *errmsg = "register `dest' does not match instruction `dest'";
      return 0;
    }
  *pstr = str;
  *errmsg = NULL;
  return reg;
}

static void
print_vfreg (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "vf%02ld", value);
  _print_dest (info, insn, dest);
}

/* FT register in broadcast case.  */

static long
parse_bcftreg (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;
  int reg_bc;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'f')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  reg_bc = _parse_sdest (&str, errmsg);
  if (*errmsg)
    return 0;
  if (reg_bc != bc)
    {
      *errmsg = "register `bc' does not match instruction `bc'";
      return 0;
    }
  *pstr = str;
  *errmsg = NULL;
  return reg;
}

static void
print_bcftreg (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "vf%02ld", value);
  print_sdest (info, insn, bc);
}

/* ACC handling.  */

static long
parse_accdest (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  long acc_dest = 0;

  if (strncasecmp (str, "acc", 3) != 0)
    {
      *errmsg = "expecting `acc'";
      return 0;
    }
  str += 3;
  acc_dest = _parse_dest (&str);
  if (acc_dest == 0 || isalnum (*str))
    {
      *errmsg = "invalid `dest'";
      return 0;
    }
  if (acc_dest != dest)
    {
      *errmsg = "acc `dest' does not match instruction `dest'";
      return 0;
    }
  *pstr = str;
  *errmsg = NULL;
  /* Value isn't used, but we must return something.  */
  return 0;
}

static void
print_accdest (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "acc");
  _print_dest (info, insn, dest);
}

/* XYZ operand handling.
   This simplifies the opmula,opmsub entries by keeping them equivalent to
   the others.  */

static TXVU_INSN
insert_xyz (insn, operand, mods, value, errmsg)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     long value;
     const char **errmsg;
{
  if (dest != (TXVU_DEST_X | TXVU_DEST_Y | TXVU_DEST_Z))
    {
      *errmsg = "expecting `xyz' for `dest' value";
      return insn;
    }
  return insn;
}

/* F[ST] register using selector in F[ST]F field.
   Internally, the value is encoded in 7 bits: the 2 bit xyzw indicator
   followed by the 5 bit register number.  */

static long
parse_ffstreg (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  int reg, xyzw;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'f')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  xyzw = _parse_sdest (&str, errmsg);
  if (*errmsg)
    return 0;
  *pstr = str;
  *errmsg = NULL;
  return reg | (xyzw << 5);
}

static void
print_ffstreg (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "vf%ld", value & TXVU_MASK_REG);
  print_sdest (info, insn, (value >> 5) & 3);
}

static TXVU_INSN
insert_ffstreg (insn, operand, mods, value, errmsg)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     long value;
     const char **errmsg;
{
  if (operand->shift == TXVU_SHIFT_SREG)
    return insn | VLFSF (value >> 5) | VS (value);
  else
    return insn | VLFTF (value >> 5) | VT (value);
}

static long
extract_ffstreg (insn, operand, mods, pinvalid)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     int *pinvalid;
{
  if (operand->shift == TXVU_SHIFT_SREG)
    return (((insn & VLFSF (~0)) >> 21) << 5) | VS (insn);
  else
    return (((insn & VLFTF (~0)) >> 21) << 5) | VT (insn);
}

/* F register.  */

static long
parse_freg (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;
  int reg_bc;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'f')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  *pstr = str;
  *errmsg = NULL;
  return reg;
}

static void
print_freg (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "vf%ld", value);
}

/* I register.  */

static long
parse_ireg (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;
  int reg_bc;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'i')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg < 0 || reg > 31)
    {
      *errmsg = "invalid register number";
      return 0;
    }
  *pstr = str;
  *errmsg = NULL;
  return reg;
}

static void
print_ireg (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "vi%ld", value);
}

/* VI01 register.  */

static long
parse_vi01 (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  char *str = *pstr;
  char *start;
  long reg;
  int reg_bc;

  if (tolower (str[0]) != 'v'
      || tolower (str[1]) != 'i')
    {
      *errmsg = "unknown register";
      return 0;
    }

  /* FIXME: quick hack until the framework works.  */
  start = str = str + 2;
  while (*str && isdigit (*str))
    ++str;
  reg = atoi (start);
  if (reg != 1)
    {
      *errmsg = "vi01 required here";
      return 0;
    }
  *pstr = str;
  *errmsg = NULL;
  return reg;
}

static void
print_vi01 (info, insn, value)
     disassemble_info *info;
     TXVU_INSN insn;
     long value;
{
  (*info->fprintf_func) (info->stream, "vi01");
}

/* Lower instruction 12 bit unsigned immediate.  */

static TXVU_INSN
insert_luimm12 (insn, operand, mods, value, errmsg)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     long value;
     const char **errmsg;
{
  return insn | VLUIMM12TOP ((value & (1 << 11)) != 0) | VLIMM11 (value);
}

static long
extract_luimm12 (insn, operand, mods, pinvalid)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     int *pinvalid;
{
  return (((insn & MLUIMM12TOP) != 0) << 11) | VLIMM11 (insn);
}

/* Lower instruction 15 bit unsigned immediate.  */

static TXVU_INSN
insert_luimm15 (insn, operand, mods, value, errmsg)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     long value;
     const char **errmsg;
{
  return insn | VLUIMM15TOP (value >> 11) | VLIMM11 (value);
}

static long
extract_luimm15 (insn, operand, mods, pinvalid)
     TXVU_INSN insn;
     const struct txvu_operand *operand;
     int mods;
     int *pinvalid;
{
  return (((insn & MLUIMM15TOP) >> 21) << 11) | VLIMM11 (insn);
}
