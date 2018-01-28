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
#ifndef   QSTATUS_BAR_H
#define   QSTATUS_BAR_H

#include "qcurses.h"
#include "qwidget.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Status Bar Definition
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QWIDGET_BEGIN(qstatus_bar_t)
  // Intentionally Empty
QWIDGET_END

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QSTATUS_BAR_H
