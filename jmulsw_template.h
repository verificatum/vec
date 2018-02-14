
/*
 * Copyright 2008-2018 Douglas Wikstrom
 *
 * This file is part of Verificatum Elliptic Curve library (VEC).
 *
 * VEC is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * VEC is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General
 * Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with VEC. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef FIELD_ELEMENT

#include "templates.h"

/*
 * Multiplication with sliding window. In theory this could be
 * slightly faster using wNAFs, but the difference would be very
 * small.
 */
void
FUNCTION_NAME(vec_jmulsw, POSTFIX)
     (FIELD_ELEMENT RX, FIELD_ELEMENT RY, FIELD_ELEMENT RZ,
      CURVE *curve,
      FIELD_ELEMENT_VAR X, FIELD_ELEMENT_VAR Y, FIELD_ELEMENT_VAR Z,
      mpz_t scalar)
{

  int i;
  int j;
  int b;
  int size;
  int width;
  float cost;
  float new_cost;
  int bit_length;
  int block;

  FIELD_ELEMENT_VAR *xtab;
  FIELD_ELEMENT_VAR *ytab;
  FIELD_ELEMENT_VAR *ztab;

  SCRATCH(scratch);

  VEC_UNUSED(curve);

  SCRATCH_INIT(scratch);

  /* Determine bit length. */
  bit_length = (int)mpz_sizeinbase(scalar, 2);

  /* Determine optimal width of table. */
  width = 1;
  new_cost = bit_length / 2;

  do {

    width++;
    cost = new_cost;
    new_cost = (1 << (width - 1)) +
      ((float)((1 << width) - 1) * bit_length) / ((1 << width) * width);

  } while (new_cost < cost);

  width--;

  size = (1 << (width - 1));

  /* Precompute table. */
  xtab = ARRAY_MALLOC_INIT(size);
  ytab = ARRAY_MALLOC_INIT(size);
  ztab = ARRAY_MALLOC_INIT(size);

  /* Double (X, Y, Z) to compute table. */
  JDBL_VAR(scratch,
           xtab[size - 1], ytab[size - 1], ztab[size - 1],
           curve,
           X, Y, Z);

  /* Initialize with basis. */
  FIELD_ELEMENT_VAR_SET(xtab[0], ytab[0], ztab[0], X, Y, Z);

  /* Build table */
  for (i = 1; i < size; i++) {

    JADD_VAR(scratch,
             xtab[i], ytab[i], ztab[i],
             curve,
             xtab[i - 1], ytab[i - 1], ztab[i - 1],
             xtab[size - 1], ytab[size - 1], ztab[size - 1]);
  }

  /* Initialize with basis. */
  FIELD_ELEMENT_UNIT(RX, RY, RZ);

  /* Compute output. */
  i = bit_length;

  while (i >= 0)
    {

      /* Double until we encounter a one, i.e., the starting block of
         a block of bits. */
      while (i >= 0 && !mpz_tstbit(scalar, i))
        {
          JDBL(scratch,
               RX, RY, RZ,
               curve,
               RX, RY, RZ);
          i--;
        }

      /* Check if we did not encounter any one and should return. */
      if (i < 0)
        {
          break;
        }

      /* Find end point of block of bits. We know that we will find a
         one now, since i >= 0. */
      j = i - width + 1;
      if (j < 0)
        {
          j = 0;
        }
      while (j < i && !mpz_tstbit(scalar, j))
        {
          j++;
        }

      /* Extract block of bits starting and ending with ones. Note
         that we drop the least significant bit, since this is not
         used in the table lookup. */
      block = 0;
      for (b = i; b > j; b--)
        {
          block = (block << 1) | mpz_tstbit(scalar, b);
        }

      /* Double for the block. */
      while (i >= j)
        {
          JDBL(scratch,
               RX, RY, RZ,
               curve,
               RX, RY, RZ);
          i--;
        }

      /* Add with block. */
      JADD(scratch,
           RX, RY, RZ,
           curve,
           RX, RY, RZ,
           xtab[block], ytab[block], ztab[block]);

    }

  ARRAY_CLEAR_FREE(ztab, size);
  ARRAY_CLEAR_FREE(ytab, size);
  ARRAY_CLEAR_FREE(xtab, size);

  SCRATCH_CLEAR(scratch);
}

#endif
