#include "string.h"
#include <stdarg.h>

DG_SYMBOL String8 string8(char *str, usize len)
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

DG_SYMBOL String8 string8_vfmt(DG_Arena *a, char *fmt, va_list args)
{

  va_list args2;
  va_copy(args2, args);

  int string_size = vsnprintf(0, 0, fmt, args2) + 1;

  char *string = dg_arena_alloc_size(a, string_size);

  vsnprintf(string, string_size, fmt, args2);

  va_end(args2);

  DG_ASSERT(string[string_size - 1] == 0);

  return string8(string, string_size);
}

DG_SYMBOL String8 string8_fmt(DG_Arena *a, char *fmt, ...)
{
  String8 result = {0};

  va_list args;
  va_start(args, fmt);
  result = string8_vfmt(a, fmt, args);
  va_end(args);

  return result;
}

DG_SYMBOL String8 string8_from_cstring_clamped(char *data, u32 max_len)
{
  String8 result = {0};
  result.len = DG_STRLEN_CLAMPED(data, max_len);
  result.data = (u8*)data;
  return result;
}

DG_SYMBOL String8 string8_from_cstring(char *data)
{
  String8 result = {0};
  result.len = DG_STRLEN(data);
  result.data = (u8*)data;
  return result;
}

DG_SYMBOL String8 string8_from_ncstring(char *data, u32 len)
{
  String8 result = {0};
  result.len = len;
  result.data = (u8*)data;
  return result;
}

DG_SYMBOL String8 string8_substring_copy(DG_Arena *a, String8 str, u32 low, u32 high)
{
  String8 alias = string8_substring_alias(str, low, high);
  return string8_copy(a, alias);
}

DG_SYMBOL String8 string8_substring_alias(String8 str, u32 low, u32 high)
{

  DG_ASSERT(low <= high);
  DG_ASSERT(low <= str.len);
  DG_ASSERT(high <= str.len);

  String8 result = {0};
  result.len = high - low;
  result.data = str.data + low;

  return result;
}

DG_SYMBOL usize string8_len(String8 str)
{
  return str.len;
}

DG_SYMBOL usize string8_size(String8 str)
{
  return (str.len * sizeof(u8));
}

DG_SYMBOL String8 string8_copy(DG_Arena *a, String8 str)
{
  String8 result = { 0 };
  usize size = string8_size(str);

  result.data = dg_arena_alloc_size(a, size + 1);
  DG_MEMCPY(result.data, str.data, size);

  result.data[size] = 0;

  result.len = str.len;
  return result;
}

DG_SYMBOL char *string8_copy_to_cstring(DG_Arena *a, String8 str)
{
  return (char*) string8_copy(a, str).data;
}

DG_SYMBOL void string8_to_string(String8 str, char *buffer, usize buffer_size)
{
  usize size = MIN(buffer_size, string8_len(str) + 1);
  DG_MEMCPY(buffer, str.data, size);

  *(buffer + (size - 1)) = '\0';
}


DG_SYMBOL String8 string_list_to_string8(DG_Arena *a, String_List *list)
{
  String8 result = {0};

  u8 *ptr = dg_arena_alloc_size(a, list->total_size + 1);

  result.data = ptr;
  result.len = list->total_size;

  for (String_Node *node = list->first; node != 0; node = node->next) {
    String8 str = node->data;
    DG_MEMCPY(ptr, str.data, str.len);

    // NOTE: já que (sizeof u8) == 1
    ptr += str.len;
  }
  *ptr = 0;

  DG_ASSERT(((uptr)result.data + result.len) == (uptr)ptr);

  return result;
}

DG_SYMBOL char *string_list_to_cstring(DG_Arena *a, String_List *list)
{
  return (char *)string_list_to_string8(a, list).data;
}

DG_SYMBOL String_Node *string_list_push_node(String_List *list, String_Node *node)
{
  dg_sll_push(&list->first, &list->last, node);

  list->total_size += node->data.len;
  list->node_count++;

  return node;
}

DG_SYMBOL String_Node *string_list_push_fmt(DG_Arena *a, String_List *list, char *fmt, ...)
{
  String_Node *result = dg_arena_alloc_size(a, sizeof *result);

  va_list args;
  va_start(args, fmt);
  result->data = string8_vfmt(a, fmt, args);
  va_end(args);

  return string_list_push_node(list, result);
}

DG_SYMBOL String_Node *string_list_push_string8(DG_Arena *a, String_List *list, String8 str)
{
  String_Node *result = dg_arena_alloc_size(a, sizeof *result);
  result->data = str;


  return string_list_push_node(list, result);
}

DG_SYMBOL String_Node *string_list_push_ncstring(DG_Arena *a, String_List *list, char *str, u32 length)
{
  String_Node *node = dg_arena_alloc_size(a, sizeof *node);
  node->data = string8(str, length);

  return string_list_push_node(list, node);
}

DG_SYMBOL String_Node *string_list_push_cstring(DG_Arena *a, String_List *list, char *str)
{
  return string_list_push_ncstring(a, list, str, DG_STRLEN(str));
}

DG_SYMBOL String_List string8_split_on_ncstring_pro(
  DG_Arena *a,
  String8 str,
  char *split_chars,
  u32 split_chars_count,
  u8 flags)
{
  String_List result = {0};

  b32 keep_empties = flags | STR_SPLIT_KEEP_EMPTIES;

  i32 substr_start = 0;
  i32 substr_end = 0;

  for (i32 i = 0; i < str.len; ++i) {
    b32 include_last = false;

    b32 is_split = false;
    for (u32 split_chars_idx = 0; split_chars_idx < split_chars_count; ++split_chars_idx) {
      char c = split_chars[split_chars_idx];
      is_split |= str.data[i] == c;
      if (i == str.len-1) {
        if (str.data[i] != c) {
          include_last = true;
        }
        is_split = true;
      }
    }


    if (is_split) {
      substr_end = i + include_last;
      if (substr_end - substr_start != 0 || keep_empties) {
        String8 substr = string8_substring_alias(str, substr_start, substr_end);
        string_list_push_string8(a, &result, substr);
      }
      substr_start = i + 1;
    }
  }

  return result;
}

DG_SYMBOL String_List string8_split_on_string8_impl(
  DG_Arena *a,
  String8 str,
  String8 split_str,
  struct string8_split_opt *params)
{
  return string8_split_on_ncstring_pro(a, str, (char *) split_str.data, split_str.len, params->flags);
}

DG_SYMBOL String_List string8_split_on_ncstring_impl(
  DG_Arena *a,
  String8 str,
  char *split_chars,
  u32 split_chars_count,
  struct string8_split_opt *params)
{
  return string8_split_on_ncstring_pro(a, str, split_chars, split_chars_count, params->flags);
}

DG_SYMBOL String_List string8_split_on_char_impl(DG_Arena *a, String8 str, char c, struct string8_split_opt *params)
{
  return string8_split_on_ncstring_pro(a, str, &c, 1, params->flags);
}


DG_SYMBOL String_List string8_split_lines(DG_Arena *a, String8 str)
{
  // FIXME: \r\n???
  return string8_split_on_char(a, str, '\n');
}


DG_SYMBOL String_Array string_list_to_array(DG_Arena *a, String_List *list)
{
  String_Array result;
  result.len   = list->node_count;
  result.data = dg_arena_alloc_size(a, sizeof(String8) * result.len);

  i32 idx = 0;
  for(String_Node *n = list->first; n != 0; n = n->next, idx += 1)
  {
    result.data[idx] = n->data;
  }
  return result;
}


DG_SYMBOL u32 string8_parse_u32_impl(String8 str, struct string8_parse_opt *params)
{
  if (params->base == 0) {
    params->base = 10;
  }

  DG_ASSERT(params->base >= 2 && params->base <= 36);
  u32 result = 0;

  DG_Scratch scratch = dg_scratch_get(0);
  char *cstr = string8_copy_to_cstring(scratch.arena, str);
  result = strtol(cstr, 0, params->base);
  dg_scratch_release(scratch);

  return result;
}


// TODO: string8_parse_params
DG_SYMBOL i32 string8_parse_i32(String8 str)
{
  i32 result = 0;
  DG_Scratch scratch = dg_scratch_get(0);
  result = atoi(string8_copy_to_cstring(scratch.arena, str));
  dg_scratch_release(scratch);

  return result;
}

