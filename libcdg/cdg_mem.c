#include "libcdg.h"

#ifndef DG_SCRATCH_ARENA_COUNT
#define DG_SCRATCH_ARENA_COUNT 2
#endif

thread_static DG_Arena *global_scratch[2] = {0};

void dg_scratch_memory_init(void)
{
  // TODO: reservar no lugar de alocar
  u64 memory_size = 4 * MEGABYTE;
  u8 *memory = malloc(memory_size);

  dg_scratch_memory_init_buffer(memory, memory_size);
}

DG_Temp_Arena dg_scratch_get(DG_Arena *conflict)
{
  if (!global_scratch[0]) {
    dg_scratch_memory_init();
  }

  DG_Arena *result = 0;

  DG_Arena **scratch_arena = global_scratch;

  for (u32 i = 0; i < DG_SCRATCH_ARENA_COUNT; ++i, ++scratch_arena) {
    bool is_non_conflicting = true;

    if (conflict == *scratch_arena) {
      is_non_conflicting = false;
    }

    if (is_non_conflicting) {
      result = *scratch_arena;
      break;
    }
  }

  return dg_temp_arena_begin(result);
}

void dg_scratch_memory_init_buffer(u8 *data, usize size)
{
  DG_ASSERT(data != 0);
  DG_ASSERT(size > sizeof(global_scratch));

  u32 non_reserved_size = size - sizeof(global_scratch);
  u32 individual_arena_size = non_reserved_size / DG_SCRATCH_ARENA_COUNT;

  DG_ASSERT(
      (individual_arena_size * DG_SCRATCH_ARENA_COUNT)
      <= size
  );

  uintptr curr_ptr = (uintptr)data;
  for (u32 i = 0; i < DG_SCRATCH_ARENA_COUNT; ++i) {
    global_scratch[i] = dg_arena_init_buffer((u8 *)curr_ptr, individual_arena_size);
    curr_ptr += individual_arena_size;
  }
}



DG_Arena *dg_arena_init_buffer(void *data, size_t size)
{
  DG_Arena *arena = data;
  DG_MEMSET(arena, 0, sizeof *arena);

  arena->data = (void *)((uintptr)data + (sizeof *arena));
  arena->size = size;

  return arena;
}


internal b32 dg_is_power_of_two(size_t x) {
  return ((x != 0) && ((x & (x - 1)) == 0));
}

// implementation from https://dylanfalconer.com/articles/the-arena-custom-memory-allocators
internal uintptr_t dg_align_forward(uintptr_t ptr, size_t alignment)
{
  uintptr_t p, a, modulo;
  if (!dg_is_power_of_two(alignment)) {
    return 0;
  }

  p = ptr;
  a = (uintptr_t)alignment;
  modulo = p & (a - 1);

  if (modulo) {
    p += a - modulo;
  }

  return p;
}

void *dg_arena_alloc_impl(DG_Arena *arena, size_t size, size_t alignment)
{
  uintptr curr_ptr = (uintptr)arena->data + (uintptr)arena->cursor;
  uintptr offset = dg_align_forward(curr_ptr, alignment);
  offset -= (uintptr)arena->data;

  // TODO: trocar para um if, se extrapolar o tamanho fazer um realloc
  // além disso olhar se preciso fazer uma função para arena estatica e outra para arena dinamica
  DG_ASSERT(offset + size < arena->size);

  void *ptr = (void *)((uintptr)arena->data + offset);
  arena->cursor = offset + size;

  arena->last_allocation = ptr;
  // FIXME: colocar uma versão onde o memset usa 0xFE no lugar de 0
  return DG_MEMSET(ptr, 0, size);
}

void *dg_arena_realloc(DG_Arena *a, void *old_ptr, usize new_size) {
  void *result = 0;
  if (old_ptr == a->last_allocation) {
    usize old_size = (uintptr)old_ptr - (uintptr)a->data + a->cursor;
    if (new_size > old_size) {
      dg_arena_alloc(a, new_size - old_size);
    } else {
      a->cursor -= old_size - new_size;
    }

    result = old_ptr;
  } else {
    result = dg_arena_alloc(a, new_size);
  }

  return result;
}

// TODO: olhar https://youtu.be/443UNeGrFoM?si=DBJXmKB_z8W8Yrrf&t=3074
void *dg_tracking_arena_alloc_impl(DG_Arena *arena, size_t size, size_t alignment, char *file, i32 line)
{
  // TODO: registrar onde foram todas as alocações
  void *ptr = dg_arena_alloc_impl(arena, size, alignment);
  if (ptr == 0) {
    DG_LOG_ERROR("%s:%d Could not allocate %zu bytes\n", file, line, size);
  }
  return ptr;
}

void *dg_arena_realloc_impl(DG_Arena *arena, void *ptr, size_t new_size, size_t alignment)
{
  void *result = 0;
  if (ptr == arena->last_allocation) {
    result = ptr;
    isize last_size = ((uintptr)arena->data + arena->cursor) - (uintptr)ptr;

    DG_ASSERT(last_size <= arena->size);

    if (last_size < new_size) {
      usize size_diff = new_size - last_size;
      arena->cursor += new_size;
    } else if (last_size > new_size) {
      usize size_diff = last_size - new_size;
      arena->cursor -= new_size;
    }

  } else {
    result = dg_arena_alloc_impl(arena, new_size, alignment);
  }

  return result;
}

DG_Temp_Arena dg_temp_arena_begin(DG_Arena *a)
{
  return (DG_Temp_Arena) {
    .arena = a,
    .cursor = a->cursor,
    .last_allocation = a->last_allocation,
  };
}

void dg_temp_arena_end(DG_Temp_Arena tmp_mem)
{
  tmp_mem.arena->cursor = tmp_mem.cursor;
  tmp_mem.arena->last_allocation = tmp_mem.last_allocation;
  tmp_mem.arena = 0;
}

// TODO:
void dg_arena_clear(DG_Arena *arena)
{
  arena->cursor = 0;
}
