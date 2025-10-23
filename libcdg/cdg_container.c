#include "libcdg.h"

// Dynamic arrays

internal inline void dg_dynamic_array_make_buffer_unchecked(_DG_Any_Dynamic_Array *arr, u32 capacity, u8 *data) {
  _DG_Any_Dynamic_Array replica = {0};
  replica.cap = capacity;
  replica.data = data;
  DG_MEMCPY(arr, &replica, sizeof(replica));
}

// TODO: colocar em libcdg.h
void dg_dynamic_array_make_buffer_impl(u8 *data, usize data_size, _DG_Any_Dynamic_Array *arr, u32 capacity, u32 item_size) {
  DG_ASSERT((usize)(capacity * item_size) <= data_size);
  dg_dynamic_array_make_buffer_unchecked(arr, capacity, data);
}

void dg_dynamic_array_make_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, u32 capacity, u32 item_size) {
  u8 *data = dg_arena_alloc(a, item_size * capacity);
  dg_dynamic_array_make_buffer_unchecked(arr, capacity, data);
}


// FIXME: utilizar DG_Allocator_Header no lugar de DG_Arena,
// pra isso preciso criar uma especie de função de realloc
internal void dg_dynamic_array_grow(DG_Arena *a, _DG_Any_Dynamic_Array *arr, u32 item_size) {
  _DG_Any_Dynamic_Array replica = {0};
  DG_MEMCPY(&replica, arr, sizeof(replica));

  if (!replica.data) {
    // TODO: default capacity
    replica.cap = 8;
    replica.data = dg_arena_alloc(a, 2 * item_size * replica.cap);
  } else {
    replica.data = dg_arena_realloc(a, replica.data, 2 * item_size * replica.cap);
  }

  replica.cap *= 2;
  DG_MEMCPY(arr, &replica, sizeof(replica));
}

void dg_dynamic_array_pop_impl(_DG_Any_Dynamic_Array *arr, void *dst, u32 item_size) {
  DG_ASSERT(arr->len > 0);
  if (dst) {
    void *last_item_start = (void *)((uintptr)arr->data + ((arr->len - 1) * item_size));
    DG_MEMCPY(dst, last_item_start, item_size);
  }
  arr->len -= 1;
}

void dg_dynamic_array_push_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size) {
  void *dst = (void *)(((uintptr)arr->data) + (arr->len * item_size));
  arr->len++;
  DG_MEMCPY(dst, src, item_size);
}

bool dg_dynamic_array_try_push_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size) {
  if (arr->len < arr->cap) {
    dg_dynamic_array_push_impl(arr, src, item_size);
    return true;
  }

  return false;
}

void dg_dynamic_array_push_or_error_impl(_DG_Any_Dynamic_Array *arr, void *src, u32 item_size) {
  DG_ASSERT(arr->len < arr->cap);
  dg_dynamic_array_push_impl(arr, src, item_size);
}

void dg_dynamic_array_push_or_grow_impl(DG_Arena *a, _DG_Any_Dynamic_Array *arr, void *src, u32 item_size) {
  if (arr->len >= arr->cap) {
    dg_dynamic_array_grow(a, arr, item_size);
  }
  dg_dynamic_array_push_impl(arr, src, item_size);
}

void dg_dynamic_array_clear_impl(_DG_Any_Dynamic_Array *arr) {
  arr->len = 0;
}

// Slices

void dg_slice_make_impl(DG_Arena *a, _DG_Any_Slice *slice, u64 len, u64 item_size){
  _DG_Any_Slice res = {0};

  void *data = dg_arena_alloc(a, len * item_size);

  if (data) {
    res.len = len;
    res.data = data;
  }

  *slice = res;
}

