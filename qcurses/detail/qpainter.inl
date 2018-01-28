/*******************************************************************************
 * Copyright 2017 Trent Reed
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
#ifndef   QPAINTER_INL
#define   QPAINTER_INL

#include "../qmath.h"
#include <ncurses.h>

////////////////////////////////////////////////////////////////////////////////
// Painter Definition
////////////////////////////////////////////////////////////////////////////////

struct qpainter_t
{
  WINDOW *                              pWindow;
  qbounds_t                             boundary;
  qbounds_t                             maxBounds;
  char *                                pClearBrush;
};

#endif // QPAINTER_INL