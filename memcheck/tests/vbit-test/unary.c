/* -*- mode: C; c-basic-offset: 3; -*- */

/*
   This file is part of MemCheck, a heavyweight Valgrind tool for
   detecting memory errors.

   Copyright (C) 2012-2015  Florian Krohm

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   The GNU General Public License is contained in the file COPYING.
*/

#include <assert.h>
#include "vtest.h"


/* Check the result of a unary operation. */
static void
check_result_for_unary(const irop_t *op, const test_data_t *data)
{
   const opnd_t *result = &data->result;
   const opnd_t *opnd   = &data->opnds[0];
   unsigned num_bits = result->vbits.num_bits;
   vbits_t expected_vbits;

   /* Only handle those undef-kinds that actually occur. */
   switch (op->undef_kind) {
   case UNDEF_ALL:
      expected_vbits = undefined_vbits(num_bits);
      break;

   case UNDEF_SAME:
      expected_vbits = opnd->vbits;
      break;

   case UNDEF_TRUNC:
      expected_vbits = truncate_vbits(opnd->vbits, num_bits);
      break;

   case UNDEF_LEFT:
      expected_vbits = left_vbits(opnd->vbits, num_bits);
      break;

   case UNDEF_UPPER:
      assert(num_bits * 2 == opnd->vbits.num_bits);
      expected_vbits = upper_vbits(opnd->vbits);
      break;

   case UNDEF_SEXT:
      expected_vbits = sextend_vbits(opnd->vbits, num_bits);
      break;

   case UNDEF_ZEXT:
      expected_vbits = zextend_vbits(opnd->vbits, num_bits);
      break;

   default:
      panic(__func__);
   }

   if (! equal_vbits(result->vbits, expected_vbits))
      complain(op, data, expected_vbits);
}


int
test_unary_op(const irop_t *op, test_data_t *data)
{
   unsigned num_input_bits, bitpos;
   int tests_done = 0;

   num_input_bits = bitsof_irtype(data->opnds[0].type);

   for (bitpos = 0; bitpos < num_input_bits; ++bitpos) {
      data->opnds[0].vbits = onehot_vbits(bitpos, num_input_bits);

      valgrind_execute_test(op, data);

      check_result_for_unary(op, data);
      tests_done++;
   }
   return tests_done;
}
