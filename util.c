#include "mcc.h"

List* new_list() {
  List* l = malloc(sizeof(List));
  l->first = NULL;
  l->last = NULL;
  l->len = 0;
  return l;
}

void list_offer(List* l, void* elem) {
  Node* n = malloc(sizeof(Node));
  n->data = elem;
  n->link = NULL;
  if (l->len == 0) {
    l->first = l->last = n;
  } else {
    l->last->link = n;
    l->last = n;
  }
  l->len++;
}

void* list_poll(List* l) {
  assert(l->len);
  Node* tmp = l->first;
  l->first = l->first->link;
  l->len--;
  return tmp->data;
}

List* list_append(List* a, List* b) {
  if (a->len == 0) {
    return b;
  } else if (b->len == 0) {
    return a;
  } else {
    a->last->link = b->first;
    a->last = b->last;
    a->len += b->len;
    return a;
  }
}

bool list_empty(List* l) { return l->len == 0 ? true : false; }

Vector* new_vec() {
  Vector* v = malloc(sizeof(Vector));
  v->data = malloc(sizeof(void*) * 16);
  v->capacity = 16;
  v->len = 0;
  return v;
}

void vec_push(Vector* v, void* elem) {
  if (v->len == v->capacity) {
    v->capacity *= 2;
    v->data = realloc(v->data, sizeof(void*) * v->capacity);
  }
  v->data[v->len++] = elem;
}

void vec_pushi(Vector* v, int val) { vec_push(v, (void*)(long long)val); }

void* vec_pop(Vector* v) {
  assert(v->len);
  return v->data[--v->len];
}

void* vec_last(Vector* v) {
  assert(v->len);
  return v->data[v->len - 1];
}

bool vec_contains(Vector* v, void* elem) {
  for (int i = 0; i < v->len; i++)
    if (v->data[i] == elem) return true;
  return false;
}

bool vec_union1(Vector* v, void* elem) {
  if (vec_contains(v, elem)) return false;
  vec_push(v, elem);
  return true;
}

bool vec_empty(Vector* v) { return v->len == 0 ? true : false; }

Map* new_map(void) {
  Map* map = malloc(sizeof(Map));
  map->keys = new_vec();
  map->vals = new_vec();
  return map;
}

void map_put(Map* map, char* key, void* val) {
  vec_push(map->keys, key);
  vec_push(map->vals, val);
}

void map_puti(Map* map, char* key, int val) {
  map_put(map, key, (void*)(long long)val);
}

void* map_get(Map* map, char* key) {
  for (int i = map->keys->len - 1; i >= 0; i--)
    if (!strcmp(map->keys->data[i], key)) return map->vals->data[i];
  return NULL;
}

int map_geti(Map* map, char* key, int default_) {
  for (int i = map->keys->len - 1; i >= 0; i--)
    if (!strcmp(map->keys->data[i], key)) return (int)map->vals->data[i];
  return default_;
}

StringBuilder* new_sb(void) {
  StringBuilder* sb = malloc(sizeof(StringBuilder));
  sb->data = malloc(8);
  sb->capacity = 8;
  sb->len = 0;
  return sb;
}

static void sb_grow(StringBuilder* sb, int len) {
  if (sb->len + len <= sb->capacity) return;

  while (sb->len + len > sb->capacity) sb->capacity *= 2;
  sb->data = realloc(sb->data, sb->capacity);
}

void sb_add(StringBuilder* sb, char c) {
  sb_grow(sb, 1);
  sb->data[sb->len++] = c;
}

void sb_append(StringBuilder* sb, char* s) { sb_append_n(sb, s, strlen(s)); }

void sb_append_n(StringBuilder* sb, char* s, int len) {
  sb_grow(sb, len);
  memcpy(sb->data + sb->len, s, len);
  sb->len += len;
}

char* sb_get(StringBuilder* sb) {
  if (sb->data[sb->len-1] != '\0') {
    sb_add(sb, '\0');
  }
  return sb->data;
}

char *int_to_string(int val) {
  int len = 0, tmp = val;
  if (tmp <= 0) {
    tmp = -tmp;
    len++;
  }
  while(tmp > 0) {
    len++;
    tmp /= 10;
  }
  char *ans = malloc((len+1)*sizeof(char));
  sprintf(ans,"%d",val);
  return ans;
}