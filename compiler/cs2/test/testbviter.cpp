/*******************************************************************************
 * Copyright (c) 2000, 2016 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *******************************************************************************/

#include <cstdio>
#include <stdlib.h>
#include <iostream>

#include "cs2/bitvectr.h"
#include "cs2/sparsrbit.h"



template <class ABitVector, class Allocator>
int foo(int seed, Allocator &a) {
  ABitVector v1(a),v2(a),v3(a);
  int count;
  srand(seed);

  count =rand()%1000000;
  for (int i=0; i<count; i++)
    v1[rand()%0xFFFFFF]=1;

  count =rand()%1000000;
  for (int i=0; i<count; i++)
    v2[rand()%0xFFFFFF]=(bool)v1[rand()%0xFFFFFF];

  count =rand()%1000000;
  for (int i=0; i<count; i++)
    v3[rand()%0xFFFFFF]=(bool)v2[rand()%0xFFFFFF];

  v2.Or(v3);
  v1.And(v3);

  v1.Or(v2,v3);
  v3.And(v2,v1);

  if (v1.IsZero()) return 0;

  auto i = v1.begin();
  typename ABitVector::Cursor c(v1);
  c.SetToFirstOne();
  for (; (i != v1.end()) && c.Valid()  ; ++i, c.SetToNextOne()) {
    if (c != i) {
      printf("%d: seed = %d ERROR: iterator index %d does not match cursor index %d.\n", __LINE__, seed, (uint32_t) i, (uint32_t) c);
      return 1;
    }
    if (c != *i) {
      printf("%d: seed = %d ERROR: deref iterator index %d does not match cursor index %d.\n", __LINE__, seed,  (uint32_t) *i, (uint32_t) c);
      return 1;
    }
 }
  if ((i != v1.end()) != c.Valid()) {
    printf("%d: seed = %d ERROR: iterator has %sterminated, whereas cursor has %sterminated.\n",
        __LINE__, seed, (i == v1.end()) ? "" : "not ", !c.Valid() ? "" : "not ");
    return 1;
  }
  return 0;
}

typedef CS2::shared_allocator<CS2::heap_allocator<8192,10> > allocator;
typedef CS2::ABitVector<allocator> dense;

int main() {
  CS2::heap_allocator<8192,10> h;
  allocator a(h);

  int seed = 2;
  return foo<dense,allocator>(seed,a);
}
