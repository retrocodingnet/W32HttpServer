#include "server_http.h"

struct http_method_map {
  const char *method_str; 
  enum http_method_t method_num;
};

static struct http_method_map method_map[] = {
  {"GET", HTTP_GET},
  {"POST", HTTP_POST},
  {"PUT", HTTP_PUT},
  {"PATCH", HTTP_PATCH},
};

static const size_t http_method_map_n = sizeof(method_map) / sizeof(struct http_method_map);

static BOOL ascii_compare (u8 c1, u8 c2) {
  return (c1 == c2 || (c1-32) == c2 || c1 == (c2-32)) ? TRUE : FALSE;
}

static BOOL nocase_compare(const char *str1, const char *str2, const size_t n) {
  u8 *ptr1 = (u8*)str1;
  u8 *ptr2 = (u8*)str2;
  size_t i; 
  
  if ( 0 == n ) {
    return FALSE;
  }
  

  for (i = 0; i < n; ++i, ++ptr1, ++ptr2) {
    if (!ascii_compare(*ptr1, *ptr2)) {
      return FALSE;
    }
  }

  return TRUE;
}

static enum http_method_t parse_method(const u8 *payload, u8 **path_ptr) {
  u8 *method_end;
  u32 compare_len;
  enum http_method_t method;
  size_t i;

  for(method_end = (u8*) payload; *method_end != ' '; ++method_end) {
    // just go through
  }

  if (' ' != *method_end ) {
    return -1;
  }

  compare_len = method_end - payload;

  for(i = 0; i < http_method_map_n; ++i) {
    if (strnicmp(method_map[i].method_str, (const char*)payload, compare_len) == 0) {
      method =  method_map[i].method_num;
	  break;
    }
  }

  if (NULL != path_ptr) {
    *path_ptr = method_end + 1; 
  }
  
  return method;
}

static struct http_path_t parse_path(const u8 *method_ptr, u8 **http_version_ptr) {
  struct http_path_t path = { 0, 0 }; 
  u8 *ptr;
  path.start = method_ptr;
  
  for(ptr = (u8 *) path.start; *ptr != ' '; ++ptr);
  path.end = ptr; 

  if (NULL != http_version_ptr) {
    *http_version_ptr = (u8*)path.end + 1;
  }
  
  return path;
}

int parse_http_request(
    u8 *payload, 
    size_t payload_len, 
    struct http_request_t *req_output)
{
  u8 *ptr, *next_ptr;
  if (NULL == req_output) {
    return -1; 
  }

  memset(req_output, 0, sizeof(struct http_request_t));

  req_output->payload = payload;
  req_output->payload_len = payload_len;
  req_output->method = parse_method(payload, &ptr);
  req_output->path = parse_path(ptr, &next_ptr);

  ptr = next_ptr;

  /* find header start */
  for (ptr = payload; *ptr != '\n'; ++ptr);
  req_output->header.start = ++ptr;

  /* find double newlines */
  for (; *(u32*)ptr != 0x0A0D0A0D; ++ptr); // 0x0A0D0A0D = "\r\n\r\n"
  req_output->header.end = ptr;

  return 0;
}

struct http_header_item_t find_header(struct http_header_t *hdr, const char *header_name) {
  size_t hdr_key_len = strlen(header_name);
  struct http_header_item_t item;
  int ret, i;
  const u8 *ptr;

  memset(&item, 0, sizeof(struct http_header_item_t));

  item.start = hdr->start;
  do {

    ret = strnicmp((const char*) item.start, header_name, hdr_key_len);

    if (ret != 0) {
       // skip the whole line
       for(ptr = item.start; *ptr != '\n'; ++ptr); // find '\r\n'
       item.start = ptr + 1;
       continue;
    }
    
    for(ptr = item.start; *ptr != ':'; ++ptr); // find ':'
    for(; *ptr == '\t' || *ptr == ' '; ++ptr); // skip spaces 
    item.value = ptr;
    for(; *ptr != '\n'; ++ptr); // find '\r\n'      item.end   = ptr - 1;
 
  } while (ret != 0 || ptr == hdr->end || ptr + 1 == hdr->end);

  if (item.value == 0) {
    item.start = 0;
  }

  return item;
}



