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
#ifndef   QCURSES_FWDQCURSES_H
#define   QCURSES_FWDQCURSES_H

#include <stdint.h> // integral typedefs
#include <stdlib.h> // size_t typedef

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// QCurses Defines
////////////////////////////////////////////////////////////////////////////////

#define QCURSES_DECLARE_STRUCT(name)    typedef struct name name
#define QCURSES_DECLARE_ENUM(name)      typedef enum name name
#define QCURSES_DECLARE_FLAG_TYPE(name) typedef qcurses_flags_t name
#define QCURSES_DECLARE_FLAG_BITS(name) typedef enum name name
#define QCURSES_DECLARE_FLAGS(bitsName, flagName)                               \
  QCURSES_DECLARE_FLAG_BITS(bitsName); QCURSES_DECLARE_FLAG_TYPE(flagName)

////////////////////////////////////////////////////////////////////////////////
// QCurses Typedefs
////////////////////////////////////////////////////////////////////////////////

typedef int       qcurses_bool_t;
typedef uint32_t  qcurses_flags_t;

////////////////////////////////////////////////////////////////////////////////
// QCurses Declarations
////////////////////////////////////////////////////////////////////////////////

// Enums
QCURSES_DECLARE_ENUM(qcurses_keycode_t);

// Flags
QCURSES_DECLARE_FLAGS(qcurses_align_bits_t, qcurses_align_t);
QCURSES_DECLARE_FLAGS(qcurses_modifier_bits_t, qcurses_modifier_t);
QCURSES_DECLARE_FLAGS(qcurses_policy_bits_t, qcurses_policy_t);
QCURSES_DECLARE_FLAGS(qcurses_state_bits_t, qcurses_state_t);

// Structs
QCURSES_DECLARE_STRUCT(qcurses_alloc_t);
QCURSES_DECLARE_STRUCT(qcurses_bounds_t);
QCURSES_DECLARE_STRUCT(qcurses_application_t);
QCURSES_DECLARE_STRUCT(qcurses_application_info_t);
QCURSES_DECLARE_STRUCT(qcurses_coord_t);
QCURSES_DECLARE_STRUCT(qcurses_region_t);
QCURSES_DECLARE_STRUCT(qcurses_slot_t);
QCURSES_DECLARE_STRUCT(qcurses_widget_t);
QCURSES_DECLARE_STRUCT(qcurses_widget_config_t);
QCURSES_DECLARE_STRUCT(qcurses_widget_parent_t);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_FWDQCURSES_H
