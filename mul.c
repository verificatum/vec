
/* Copyright 2008-2019 Douglas Wikstrom
 *
 * This file is part of Verificatum Elliptic Curve library (VEC).
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <gmp.h>

#include "vec.h"

void
vec_mul(mpz_t rx, mpz_t ry,
        vec_curve *curve,
        mpz_t x, mpz_t y,
        mpz_t scalar)
{

  int i;
  int bit_length;

  vec_scratch_mpz_t scratch;

  vec_scratch_init_mpz_t(scratch);

  /* Initialize with the unit element. */
  mpz_set_si(rx, -1);
  mpz_set_si(ry, -1);

  /* Determine bit length. */
  bit_length = (int)mpz_sizeinbase(scalar, 2);

  for (i = bit_length; i >= 0; i--)
    {

      /* Double. */
      vec_dbl(scratch,
              rx, ry,
              curve,
              rx, ry);

      /* Add. */
      if (mpz_tstbit(scalar, i))
        {

          vec_add(scratch,
                  rx, ry,
                  curve,
                  rx, ry,
                  x, y);
        }
    }

  vec_scratch_clear_mpz_t(scratch);
}
