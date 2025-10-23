#include "../libcdg/libcdg.h"
#include <stdarg.h>

typedef DG_Make_Slice_Type(u8) String8;

typedef DG_Make_Sll_Node_Type(string_node, String8) String_Node;

typedef struct {
  String_Node *first;
  String_Node *last;
  u32 node_count;
  u32 total_size;
} String_List;

String8 string8(char *str, usize len)
{
  String8 result = { 0 };

  if (str[len - 1] == 0) {
    result.len = len - 1;
  } else {
    result.len = len;
  }

  result.data = (u8*)str;
  return result;
}

String8 string8_vfmt(DG_Arena *a, char *fmt, va_list args){

  va_list args2;
  va_copy(args2, args);

  int string_size = vsnprintf(0, 0, fmt, args2) + 1;

  char *string = dg_arena_alloc(a, string_size);

  vsnprintf(string, string_size, fmt, args2);

  va_end(args2);

  DG_ASSERT(string[string_size - 1] == 0);

  return string8(string, string_size);
}

String8 string8_fmt(DG_Arena *a, char *fmt, ...){
  String8 result = {0};

  va_list args;
  va_start(args, fmt);
  result = string8_vfmt(a, fmt, args);
  va_end(args);

  return result;
}


String8 string8_from_cstring(char *data, u32 len) {
  String8 result = {0};
  result.len = len;
  result.data = (u8*)data;
  return result;
}

#define string_ensure_string_literal(str) ("" str "")
#define string8_literal(str) string8_from_cstring(string_ensure_string_literal(str), sizeof(str) - 1)

String8 string8_sub_slice(String8 str, u32 low, u32 high) {

  DG_ASSERT(low < high);
  DG_ASSERT(low < str.len);
  DG_ASSERT(high <= str.len);

  String8 result = {0};
  result.len = high - low;
  result.data = str.data + low;

  return result;
}

usize string8_len(String8 str) {
  return str.len;
}

usize string8_size(String8 str) {
  return (str.len * sizeof(u8));
}

String8 string_slice_copy(DG_Arena *a, String8 str)
{
  String8 result = { 0 };
  usize size = string8_size(str);

  result.data = dg_arena_alloc(a, size + 1);
  DG_MEMCPY(result.data, str.data, size);

  result.data[size] = 0;

  result.len = str.len;
  return result;
}

char *string8_copy_to_cstring(DG_Arena *a, String8 str)
{
  return (char*) string_slice_copy(a, str).data;
}

void string8_to_string(String8 str, char *buffer, usize buffer_size) {
  usize size = MIN(buffer_size, string8_len(str) + 1);
  DG_MEMCPY(buffer, str.data, size);

  *(buffer + (size - 1)) = '\0';
}


String8 string_list_to_string8(DG_Arena *a, String_List *list)
{
  String8 result = {0};

  u8 *ptr = dg_arena_alloc(a, list->total_size + 1);

  result.data = ptr;
  result.len = list->total_size;

  for (String_Node *node = list->first; node != 0; node = node->next) {
    String8 str = node->data;
    DG_MEMCPY(ptr, str.data, str.len);

    // NOTE: já que (sizeof u8) == 1
    ptr += str.len;
  }
  *ptr = 0;

  DG_ASSERT(((uintptr)result.data + result.len) == (uintptr)ptr);

  return result;
}

char *string_list_to_cstring(DG_Arena *a, String_List *list)
{
  return (char *)string_list_to_string8(a, list).data;
}

String_Node *string_list_push_node(String_List *list, String_Node *node)
{
  dg_sll_push(&list->first, &list->last, node);

  list->total_size += node->data.len;
  list->node_count++;

  return node;
}

String_Node *string_list_push_fmt(DG_Arena *a, String_List *list, char *fmt, ...)
{
  String_Node *result = dg_arena_alloc(a, sizeof *result);

  va_list args;
  va_start(args, fmt);
  result->data = string8_vfmt(a, fmt, args);
  va_end(args);

  return string_list_push_node(list, result);
}

String_Node *string_list_push_string_slice(DG_Arena *a, String_List *list, String8 str)
{
  String_Node *result = dg_arena_alloc(a, sizeof *result);
  result->data = string_slice_copy(a, str);


  return string_list_push_node(list, result);
}

usize cstring_len(char *str)
{
  char *ptr = str;
  for (; ptr != 0; ptr++){
    // NOTE: will be optimized away?
  }
  return (usize)(ptr - str);
}

String_Node *string_list_push_ncstring(DG_Arena *a, String_List *list, char *str, u32 length)
{
  String_Node *node = dg_arena_alloc(a, sizeof *node);
  node->data = string8(str, length);

  return string_list_push_node(list, node);
}

String_Node *string_list_push_cstring(DG_Arena *a, String_List *list, char *str)
{
  return string_list_push_ncstring(a, list, str, cstring_len(str));
}

#define string_list_push_string_literal(a, list, str) string_list_push_ncstring(a, list, ensure_string_literal(str), sizeof str);
