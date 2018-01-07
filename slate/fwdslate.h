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
 *------------------------------------------------------------------------------
 * Slate Terminal UI Include Header
 ******************************************************************************/
#ifndef   SLATE_FWDSLATE_H
#define   SLATE_FWDSLATE_H

#include <stdint.h> // integral typedefs
#include <stdlib.h> // size_t typedef

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Slate Global Defines
////////////////////////////////////////////////////////////////////////////////

#define SLATE_DECLARE_STRUCT(name)    typedef struct name name
#define SLATE_DECLARE_ENUM(name)      typedef enum name name
#define SLATE_DECLARE_FLAG_TYPE(name) typedef slate_flags_t name
#define SLATE_DECLARE_FLAG_BITS(name) typedef enum name name
#define SLATE_DECLARE_FLAGS(bitsName, flagName)                                 \
  SLATE_DECLARE_FLAG_BITS(bitsName); SLATE_DECLARE_FLAG_TYPE(flagName)

////////////////////////////////////////////////////////////////////////////////
// Slate Global Typedefs
////////////////////////////////////////////////////////////////////////////////

typedef int       slate_bool_t;
typedef uint32_t  slate_flags_t;

////////////////////////////////////////////////////////////////////////////////
// Slate Forward Declarations
////////////////////////////////////////////////////////////////////////////////

// Enums
SLATE_DECLARE_ENUM(slate_keycode_t);

// Flags
SLATE_DECLARE_FLAGS(slate_align_bits_t, slate_align_t);
SLATE_DECLARE_FLAGS(slate_modifier_bits_t, slate_modifier_t);
SLATE_DECLARE_FLAGS(slate_policy_bits_t, slate_policy_t);
SLATE_DECLARE_FLAGS(slate_state_bits_t, slate_state_t);

// Structs
SLATE_DECLARE_STRUCT(slate_alloc_t);
SLATE_DECLARE_STRUCT(slate_bounds_t);
SLATE_DECLARE_STRUCT(slate_application_t);
SLATE_DECLARE_STRUCT(slate_application_info_t);
SLATE_DECLARE_STRUCT(slate_coord_t);
SLATE_DECLARE_STRUCT(slate_region_t);
SLATE_DECLARE_STRUCT(slate_slot_t);
SLATE_DECLARE_STRUCT(slate_widget_t);
SLATE_DECLARE_STRUCT(slate_widget_config_t);
SLATE_DECLARE_STRUCT(slate_widget_parent_t);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // SLATE_FWDSLATE_H
