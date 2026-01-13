#ifndef LIBCDG_H
#define LIBCDG_H

// types.h {{{

// context cracking {{{

#if !defined(DG_CONTEXT_UNKNOWN) || !DG_CONTEXT_UNKNOWN

# if defined(__clang__) // compiler switch

#  define DG_COMPILER_CLANG 1

#  if defined(_WIN32)
#   define DG_OS_WINDOWS 1
#  elif defined(__gnu_linux__) || defined(__linux__)
#   define DG_OS_LINUX 1
#  else
#   error "This Compiler/OS combination is not supported"
#  endif

#  if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#   define DG_ARCH_X64 1
#  elif defined(i386) || defined(__i386) || defined(__i386__)
#   define DG_ARCH_X86 1
#  elif defined(__aarch64__)
#   define DG_ARCH_ARM64 1
#  elif defined(__arm__)
#   define DG_ARCH_ARM32 1
#  else
#   error "Architecture not supported."
#  endif

# elif defined(__GNUC__) || defined(__GNUG__) // compiler switch

#  define DG_COMPILER_GCC 1

#  if defined(__gnu_linux__) || defined(__linux__)
#   define DG_OS_LINUX 1
#  else
#   error "This Compiler/OS combination is not supported"
#  endif

#  if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#   define DG_ARCH_X64 1
#  elif defined(i386) || defined(__i386) || defined(__i386__)
#   define DG_ARCH_X86 1
#  elif defined(__aarch64__)
#   define DG_ARCH_ARM64 1
#  elif defined(__arm__)
#   define DG_ARCH_ARM32 1
#  else
#   error "Architecture not supported."
#  endif

# elif defined(__TINYC__)
# define DG_COMPILER_TINYC 1

#  if defined(_WIN32)
#   define DG_OS_WINDOWS 1
#  elif defined(__gnu_linux__) || defined(__linux__)
#   define DG_OS_LINUX 1
#  else
#   error "This Compiler/OS combination is not supported"
#  endif

#  if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#   define DG_ARCH_X64 1
#  elif defined(i386) || defined(__i386) || defined(__i386__)
#   define DG_ARCH_X86 1
#  elif defined(__aarch64__)
#   define DG_ARCH_ARM64 1
#  elif defined(__arm__)
#   define DG_ARCH_ARM32 1
#  else
#   error "Architecture not supported."
#  endif


# else // compiler switch
#  error "Compiler not supported."
# endif // compiler switch

# if defined(DG_ARCH_X64)
#  define DG_ARCH_64BIT 1
# elif defined(DG_ARCH_X86)
#  define DG_ARCH_32BIT 1
# endif

# if DG_ARCH_ARM32 || DG_ARCH_ARM64 || DG_ARCH_X64 || DG_ARCH_X86
#  define DG_ARCH_LITTLE_ENDIAN 1
# else
#  error "Endianness of this architecture not understood by context cracker."
# endif

#else

# define DG_ARCH_UNKNOWN 1
# define DG_OS_UNKNOWN 1
# define DG_COMPILER_UNKNOWN 1

#endif


#if defined(__cplusplus)
# define DG_LANG_CPP 1
#else
# define DG_LANG_C 1
#endif

// clear undefined variables
#if !defined(DG_ARCH_UNKNOWN)
#define DG_ARCH_UNKNOWN 0
#endif
#if !defined(DG_OS_UNKNOWN)
#define DG_OS_UNKNOWN 0
#endif
#if !defined(DG_COMPILER_UNKNOWN)
#define DG_COMPILER_UNKNOWN 0
#endif
#if !defined(DG_ARCH_32BIT)
# define DG_ARCH_32BIT 0
#endif
#if !defined(DG_ARCH_64BIT)
# define DG_ARCH_64BIT 0
#endif
#if !defined(DG_ARCH_X64)
# define DG_ARCH_X64 0
#endif
#if !defined(DG_ARCH_X86)
# define DG_ARCH_X86 0
#endif
#if !defined(DG_ARCH_ARM64)
# define DG_ARCH_ARM64 0
#endif
#if !defined(DG_ARCH_ARM32)
# define DG_ARCH_ARM32 0
#endif
#if !defined(DG_COMPILER_MSVC)
# define DG_COMPILER_MSVC 0
#endif
#if !defined(DG_COMPILER_GCC)
# define DG_COMPILER_GCC 0
#endif
#if !defined(DG_COMPILER_TINYC)
# define DG_COMPILER_TINYC 0
#endif
#if !defined(DG_COMPILER_CLANG)
# define DG_COMPILER_CLANG 0
#endif
#if !defined(DG_OS_WINDOWS)
# define DG_OS_WINDOWS 0
#endif
#if !defined(DG_OS_LINUX)
# define DG_OS_LINUX 0
#endif
#if !defined(DG_OS_MAC)
# define DG_OS_MAC 0
#endif
#if !defined(DG_LANG_CPP)
# define DG_LANG_CPP 0
#endif
#if !defined(DG_LANG_C)
# define DG_LANG_C 0
#endif

// }}}

#if __STDC_VERSION__ < 199901L /* older than c99 */
# ifndef inline
#  define inline
# endif
#endif

#if DG_COMPILER_MSVC
# define thread_static __declspec(thread)
#elif DG_COMPILER_CLANG || DG_COMPILER_GCC
# define thread_static __thread
#else
// TODO: make simple to know when this doesn't apply
# define thread_static
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uintptr_t uptr;
typedef intptr_t iptr;

typedef size_t usize;
typedef ptrdiff_t isize;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float  f32;
typedef double f64;

typedef u32 b32;
typedef u8  b8;

typedef u8 byte;

#define global_variable static
#define local_persist static
#define internal static

#if defined(DG_OS_WINDOWS)
# if defined(DG_EXPORT_FUNCTIONS)
#  define DG_EXTERN __declspec(dllexport)
# elif defined(DG_IMPORT_FUNCTIONS)
#  define DG_EXTERN __declspec(dllimport)
# else
#  define DG_EXTERN __declspec(dllexport)
# endif
#elif defined(DG_OS_LINUX)
# define DG_EXTERN __attribute__((visibility("default")))
#endif

#if defined(DG_UNITY_BUILD)
# define DG_SYMBOL internal
#elif defined(DG_EXPORT_FUNCTIONS) || defined(DG_IMPORT_FUNCTIONS)
# define DG_SYMBOL DG_EXTERN
#else
# define DG_SYMBOL
#endif

#define DG_STATEMENT(x) do { x } while (0)

#define DG_ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define STR(x) #x
#define GLUE_IMPL(a,b) a##b
#define GLUE(a,b) GLUE_IMPL(a,b)
#define GLUE_2(a,b) GLUE(a,b)
#define GLUE_3(a,b,c) GLUE(GLUE(a,b), c)
#define GLUE_4(a,b,c,d) GLUE(GLUE_3(a,b,c), d)

#define ELEVENTH_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define DG_NARGS(...) ELEVENTH_ARGUMENT(dummy, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define DG_BIT_MASK_REMOVE(mask, flag)  ((mask)  & ~(flag))
#define DG_BIT_MASK_ADD(mask, flag)     ((mask)  |  (flag))
#define DG_BIT_MASK_HAS(mask, flag)     (((mask) & (flag)) == (flag))

#if !defined(DG_STATIC_ASSERT) // {{{
# define DG_STATIC_ASSERT(...) DG_STATIC_ASSERT_IMPL(DG_NARGS(__VA_ARGS__), __VA_ARGS__)
# define DG_STATIC_ASSERT_IMPL(n, ...) GLUE(DG_STATIC_ASSERT_, n)(__VA_ARGS__)
# define DG_STATIC_ASSERT_1(expr) _Static_assert(expr, "")
# define DG_STATIC_ASSERT_2(expr, msg) _Static_assert(expr, msg)
#endif // }}} DG_STATIC_ASSERT

// NOTE: vou deixar sem as parenteses para ver se não tenho erros básicos de macros
#define ABS(a)   ((a) > (0) ? (a) : -(a))
#define MAX(a,b) ((a) > (b) ? (a) :  (b))
#define MIN(a,b) ((a) < (b) ? (a) :  (b))
#define CLAMP(val, min, max) (val < min ? min : (val > max ? max : val))

#define CLAMP_TOP MIN
#define CLAMP_BOTTOM MAX

#define KILOBYTE 1024
#define MEGABYTE 1048576L
#define GIGABYTE (MEGABYTE * 1024L)

#define DG_REINTERPRET_CAST(type, val) \
  (*((type *)&(val)))

#define DG_OFFSET_OF(type, field) \
  ((uptr)&(((type *) 0)->field))

#define DG_DYNAMIC_ACCESS(type, offset) \
  (((void *)(type))+offset)

#define DG_SWAP(type, a, b) ({ \
  type _tmp = *(a); \
  *(a) = *(b); \
  *(b) = _tmp; \
  (void)0; \
})

#ifndef DG_BREAKPOINT
 #define DG_BREAKPOINT \
  volatile usize GLUE(_dg_breakpoint_val_, __LINE__) = 0; GLUE(_dg_breakpoint_val_, __LINE__) += 0;
#endif

#ifndef DG_LOG_ERROR
# include <stdio.h>
# define DG_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)
#endif // DG_LOG_ERROR

// TODO: DG_LOG rework

#ifndef DG_LOG
# include <stdio.h>
# define DG_LOG(...) fprintf(stdout, __VA_ARGS__)
#endif // DG_LOG

#ifndef DG_CRASH
# if defined(DG_PLATFORM_WASM)
#  define DG_CRASH() __builtin_trap()
# else
#  define DG_CRASH() (*((volatile int *)0) = 69)
# endif
#endif // DG_CRASH


#if !defined(DG_ENSURE_MSG_PASS_LOC)
# define DG_ENSURE_MSG_PASS_LOC(exp, msg, file, line) \
  DG_STATEMENT({ \
    if ((exp) == false) { \
    DG_LOG_ERROR("%s:%d: assertion '%s' failed\n", file, line, msg); \
    DG_CRASH(); \
    } \
    })
# define DG_ENSURE_MSG(exp, msg) DG_ENSURE_MSG_PASS_LOC(exp, msg, __FILE__, __LINE__)
# define DG_ENSURE(exp) DG_ENSURE_MSG(exp, STR(exp))
#endif

#if defined(DG_NO_ASSERT)
# define DG_ASSERT_MSG_PASS_LOC(exp, msg, file, line)
# define DG_ASSERT_MSG(exp, msg)
# define DG_ASSERT(exp)
#else
# if !defined(DG_ASSERT_MSG_PASS_LOC) // {{{
#  define DG_ASSERT_MSG_PASS_LOC(exp, msg, file, line) DG_ENSURE_MSG_PASS_LOC(exp, msg, file, line)
#  define DG_ASSERT_MSG(exp, msg) DG_ASSERT_MSG_PASS_LOC(exp, msg, __FILE__, __LINE__)
#  define DG_ASSERT(exp) DG_ASSERT_MSG(exp, STR(exp))
# endif // }}}
#endif

#define DG_WIP \
  DG_ASSERT_MSG(false, "not implemented")

#define DG_CRASH_WHEN_N_CALLS(num) \
{\
  local_persist usize GLUE(_dg_breakpoint_val_, __LINE__) = 0;\
  if(++GLUE(_dg_breakpoint_val_, __LINE__) >= num) {\
    DG_CRASH();\
  }\
}

// }}}

// alloc.c {{{


#include <stddef.h>
// #include <string.h> // for memset

#define DG_DEFAULT_ALIGNMENT 8

#ifndef DG_STRLEN
internal inline usize strlen_clamped(char *data, usize max_len) {
  usize result = 0;
  for (usize i = 0; i <= max_len; ++i) {
    if (data[i] == '\0') {
      result = i;
      break;
    }
  }
  return result;
}
#define DG_STRLEN(str) strlen_clamped(str, (u32)(-1)) // max 4gb
#define DG_STRLEN_CLAMPED(str, max_len) strlen_clamped(str, max_len)
#endif

#ifndef DG_MEMCPY
internal inline void *memcpy(void *dst, const void *src, usize size)
{
  u8 *src_ptr = (u8 *)src;
  u8 *dst_ptr = (u8 *)dst;

  for (i32 i = 0; i < size; ++i){
    *dst_ptr = *src_ptr;
    src_ptr+=1;
    dst_ptr+=1;
  }

  return dst;
}
#define DG_MEMCPY memcpy
#endif

#ifndef DG_MEMSET
#include <string.h>
void *memset(void *dst, int val, usize size);
#define DG_MEMSET memset
#endif

#ifndef DG_REALLOC
#include <stdlib.h>
void *realloc(void *ptr, usize new_size);
#define DG_REALLOC realloc
#endif

#ifndef DG_MALLOC
#include <stdlib.h>
void *malloc(usize size);
#define DG_MALLOC malloc
#endif

#ifndef DG_FREE
#include <stdlib.h>
void free(void *ptr);
#define DG_FREE free
#endif

#ifndef DG_MEMZERO_ZERO
# if DG_DEBUG
#  define DG_MEMZERO_ZERO 0xcd
# else
#  define DG_MEMZERO_ZERO 0
# endif
#endif

#ifndef DG_MEMZERO_SIZE
#define DG_MEMZERO_SIZE(ptr, size) DG_MEMSET(ptr, DG_MEMZERO_ZERO, size)
#endif

#ifndef DG_MEMZERO
#define DG_MEMZERO(ptr) DG_MEMZERO_SIZE(ptr, sizeof (*ptr))
#endif

typedef struct DG_Arena DG_Arena;
struct DG_Arena {
  DG_Arena *prev;
  DG_Arena *current;
  // TODO: add temp_count
  usize size;

  usize base_pos;  // base position on the chain
  usize pos;       // current position relative to the arena
};

typedef struct {
  DG_Arena *arena;
  usize cursor;
} DG_Temp_Arena;

typedef DG_Temp_Arena DG_Scratch;

DG_SYMBOL DG_Arena *dg_arena_init(void);
DG_SYMBOL DG_Arena *dg_arena_init_buffer(void *data, size_t size);

DG_SYMBOL void *dg_arena_alloc_impl(DG_Arena *arena, size_t size, size_t alignment, char *file, int line);

DG_SYMBOL DG_Temp_Arena dg_temp_arena_begin(DG_Arena *a);
DG_SYMBOL void dg_temp_arena_end(DG_Temp_Arena tmp_mem);

DG_SYMBOL void dg_arena_clear(DG_Arena *arena);
DG_SYMBOL void dg_arena_pop_to(DG_Arena *a, usize pos);


#define dg_arena_alloc_size_aligned(arena, size, alignment) dg_arena_alloc_impl(arena, size, alignment, __FILE__, __LINE__)
#define dg_arena_alloc_size(arena, size)                    dg_arena_alloc_size_aligned((arena), (size), DG_DEFAULT_ALIGNMENT)
#define dg_arena_alloc_arr(arena, type, count)              dg_arena_alloc_size_aligned((arena), sizeof(type) * (count), DG_DEFAULT_ALIGNMENT)
#define dg_arena_alloc(arena, type)                         dg_arena_alloc_arr((arena), type, 1)

#define DG_Temp_Guard(arena) \
  for (DG_Temp_Arena GLUE(_dg_tam_, __LINE__) = dg_temp_arena_begin(arena) \
  ; GLUE(_dg_tam_, __LINE__).arena \
  ; dg_temp_arena_end(GLUE(_dg_tam_, __LINE__)))

DG_SYMBOL void dg_scratch_memory_init(void);
DG_SYMBOL void dg_scratch_memory_init_buffer(u8 *data, usize size);

DG_SYMBOL DG_Temp_Arena dg_scratch_get(DG_Arena *conflict);
#define dg_scratch_release(scratch_arena) dg_temp_arena_end(scratch_arena)

DG_SYMBOL u8 most_significant_bit(u64 value);

// }}}
// containers {{{

typedef i8 Ordering_Kind;
enum {
  ORDERING_LT = -1,
  ORDERING_EQ = 0,
  ORDERING_GT = 1,
};

//
// dynamic arrays
//

// NOTE: implementation from https://nullprogram.com/blog/2023/10/05/

#define DG_Make_Dynamic_Array_Type(type) \
  struct {                               \
    type *data;                          \
    i32 len;                             \
    i32 cap;                             \
}

#define DG_DYNAMIC_ARRAY_GET(arr, index) ((arr).data[(index)])
#define DG_DYNAMIC_ARRAY_LEN(arr)        ((arr).len)
#define DG_DYNAMIC_ARRAY_CAP(arr)        ((arr).cap)
#define DG_DYNAMIC_ARRAY_ACCESS(arr, index) (&(arr).data[(index)])

#define DG_DA_FOREACH_WITH_IDX(item_decl, idx_name, da) \
  for (u32 idx_name = 0, item_decl = {0}; idx_name < DG_DYNAMIC_ARRAY_LEN(da); item_decl = DG_DYNAMIC_ARRAY_GET((da), idx_name), ++idx_name)

#define DG_DA_FOREACH(item_decl, da) DG_DA_FOREACH_WITH_IDX(item_decl, GLUE(__idx_,__LINE__),da)

#if !defined(DG_NO_ABBREVIATIONS) || !DG_NO_ABBREVIATIONS
 #define DA_ACCESS DG_DYNAMIC_ARRAY_ACCESS
 #define DA_GET DG_DYNAMIC_ARRAY_GET
 #define DA_LEN DG_DYNAMIC_ARRAY_LEN
 #define DA_CAP DG_DYNAMIC_ARRAY_CAP
#endif

#define DG_DYNAMIC_ARRAY_ITEM_SIZE(arr)  (sizeof(*(arr)->data))

typedef DG_Make_Dynamic_Array_Type(byte) _DG_Any_Dynamic_Array;

DG_SYMBOL void dg_dynamic_array_make_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, u32 capacity, u32 item_size);
#define dg_dynamic_array_make(arena, arr, capacity) dg_dynamic_array_make_impl((arena), (_DG_Any_Dynamic_Array *)(arr), (capacity), DG_DYNAMIC_ARRAY_ITEM_SIZE(arr))

DG_SYMBOL void dg_dynamic_array_pop_impl(_DG_Any_Dynamic_Array *arr, void *dst, u32 item_size);
#define dg_dynamic_array_pop_discard(arr) dg_dynamic_array_pop_impl((_DG_Any_Dynamic_Array *)(arr), NULL, 0)
#define dg_dynamic_array_pop_size(arr, item, size) dg_dynamic_array_pop_impl((_DG_Any_Dynamic_Array *)(arr), &(item), size)
#define dg_dynamic_array_pop(arr, item) dg_dynamic_array_pop_size((arr), (item), DG_DYNAMIC_ARRAY_ITEM_SIZE(arr))

DG_SYMBOL bool dg_dynamic_array_try_push_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size);
#define dg_dynamic_array_try_push_size(arr, item, size) dg_dynamic_array_try_push_impl((_DG_Any_Dynamic_Array *)(arr), &(item), size)
#define dg_dynamic_array_try_push(arr, item) dg_dynamic_array_try_push_size((arr), (item), DG_DYNAMIC_ARRAY_ITEM_SIZE(arr))

DG_SYMBOL void dg_dynamic_array_push_or_error_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size);
#define dg_dynamic_array_push_size_or_error(arr, item, size) dg_dynamic_array_push_or_error_impl((_DG_Any_Dynamic_Array *)(arr), &(item), (size))
#define dg_dynamic_array_push_or_error(arr, item) dg_dynamic_array_push_size_or_error((arr), (item), DG_DYNAMIC_ARRAY_ITEM_SIZE(arr))

DG_SYMBOL void dg_dynamic_array_push_or_grow_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, void *data, u32 item_size);
#define dg_dynamic_array_push_size_or_grow(a, arr, item, size) dg_dynamic_array_push_or_grow_impl((a), (_DG_Any_Dynamic_Array *)(arr), &(item), (size))
#define dg_dynamic_array_push_or_grow(a, arr, item) dg_dynamic_array_push_size_or_grow((a), (arr), (item), DG_DYNAMIC_ARRAY_ITEM_SIZE(arr))

#define dg_dynamic_array_push dg_dynamic_array_push_or_grow

DG_SYMBOL void dg_dynamic_array_clear_impl(_DG_Any_Dynamic_Array *arr);
#define dg_dynamic_array_clear(arr) dg_dynamic_array_clear_impl((_DG_Any_Dynamic_Array *) (arr))

DG_SYMBOL void dg_dynamic_array_mergesort_impl(
  _DG_Any_Dynamic_Array *arr,
  Ordering_Kind (*compare_fn)(void *, void *),
  usize item_size
);

#define dg_dynamic_array_mergesort(arr, fn)  \
  dg_dynamic_array_mergesort_impl(           \
    (_DG_Any_Dynamic_Array *)(arr),          \
    (Ordering_Kind (*)(void *, void *))(fn), \
    DG_DYNAMIC_ARRAY_ITEM_SIZE(arr)          \
  )


DG_SYMBOL void dg_dynamic_array_resize_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, i32 new_cap, u32 item_size);
#define dg_dynamic_array_resize(arena, arr, len) dg_dynamic_array_resize_impl((arena), (_DG_Any_Dynamic_Array*)(arr), (len), DG_DYNAMIC_ARRAY_ITEM_SIZE(arr));
#define dg_dynamic_array_resize_to_len(arena, arr) dg_dynamic_array_resize((arena), (arr), (arr)->len)

//
// slices
//

#define DG_Make_Slice_Type(type) \
  struct {                       \
    type *data;                  \
    i32 len;                     \
  }

#define DG_SLICE_GET(slice, index) ((slice).data[(index)])
#define DG_SLICE_LEN(slice)        ((slice).len)

#define DG_SLICE_ITEM_SIZE(slice) sizeof(*(slice)->data)

typedef DG_Make_Slice_Type(void) _DG_Any_Slice;

DG_SYMBOL void dg_slice_make_impl(DG_Arena *a, _DG_Any_Slice *slice, u32 len, u32 item_size);
#define dg_slice_make(arena, slice, len) dg_slice_make_impl((arena), (_DG_Any_Slice *)(slice), (len), DG_SLICE_ITEM_SIZE(slice))

DG_SYMBOL void *dg_slice_access_impl(_DG_Any_Slice *slice, i32 index, u32 item_size);
#define dg_slice_access(slice, index) dg_slice_access_impl((slice), (index), DG_SLICE_ITEM_SIZE(slice));

DG_SYMBOL void dg_slice_subslice_impl(_DG_Any_Slice *slice, i32 start, i32 finish, _DG_Any_Slice *out_subslice, usize item_size);
#define dg_slice_subslice(slice, start, finish, out_subslice) \
  dg_slice_subslice_impl((_DG_Any_Slice *)(slice), (start), (finish), (_DG_Any_Slice *)(out_subslice), DG_SLICE_ITEM_SIZE(slice))


DG_SYMBOL void dg_slice_mergesort_impl(
  _DG_Any_Slice *slice,
  Ordering_Kind (*compare_fn)(void *, void *),
  usize item_size
);
#define dg_slice_mergesort(slice, fn)        \
    dg_slice_mergesort_impl(                 \
    (_DG_Any_Slice *)(slice),                \
    (Ordering_Kind (*)(void *, void *))(fn), \
    DG_SLICE_ITEM_SIZE(slice)                \
  )

DG_SYMBOL void dg_slice_copy_impl(DG_Arena *a, _DG_Any_Slice *slice, _DG_Any_Slice *out_slice, usize item_size);
#define dg_slice_copy(arena, in_slice, out_slice) \
  dg_slice_copy_impl((arena), (_DG_Any_Slice *)(in_slice), (_DG_Any_Slice *) (out_slice), DG_SLICE_ITEM_SIZE(out_slice))

DG_SYMBOL void dg_slice_make_from_dynamic_array_impl (
  DG_Arena *arena,
  _DG_Any_Slice *dest,
  _DG_Any_Dynamic_Array *source,
  u32 _slice_item_size
);
#define dg_slice_make_from_dynamic_array(arena, dest, source) \
  dg_slice_make_from_dynamic_array_impl (                     \
    (arena),                                                  \
    (_DG_Any_Slice *) (dest),                                 \
    (_DG_Any_Dynamic_Array *) (source),                       \
    DG_SLICE_ITEM_SIZE(dest)                                  \
  )                                                           \

DG_SYMBOL void dg_dynamic_array_as_slice_impl(_DG_Any_Dynamic_Array *arr, _DG_Any_Slice *out_slice);
#define dg_dynamic_array_as_slice(arr, out_slice) dg_dynamic_array_as_slice_impl((_DG_Any_Dynamic_Array *)(arr), (_DG_Any_Slice *)(out_slice))

//
// Exponential Array (Xar)
//

// TODO: !!!


//
// Singly Linked Lists (Sll)
//

#define DG_Make_Sll_Node_Type(struct_name, type) \
struct struct_name { \
  struct struct_name *next; \
  type data; \
}

typedef DG_Make_Sll_Node_Type(_dg_any_node, void *) _DG_Any_Sll_Node;

internal void dg_sll_push_impl(_DG_Any_Sll_Node **first, _DG_Any_Sll_Node **last, _DG_Any_Sll_Node *node)
{
  if (!*first) {
    *first = node;
  } else if (*last) {
    (*last)->next = node;
  }
  *last = node;
  node->next = 0;
}

#define dg_sll_push(first, last, node) dg_sll_push_impl( \
    (_DG_Any_Sll_Node **) (first), \
    (_DG_Any_Sll_Node **) (last), \
    (_DG_Any_Sll_Node *)  (node))

internal void dg_sll_pop_front_impl(_DG_Any_Sll_Node **first, _DG_Any_Sll_Node **last)
{
  if (*first == *last) {
    *first = 0;
    *last = 0;
  } else {
    *first = (*first)->next;
  }
}

#define dg_sll_pop_front(first, last) \
  dg_sll_pop_impl( \
    (_DG_Any_Sll_Node **) (first), \
    (_DG_Any_Sll_Node **) (last))


// }}}

#endif /* LIBCDG_H */
