#include "lib.h"
#include <stdio.h>

#define END(parser) (&(parser)->buffer[StringBuffer_len((parser)->buffer)])
#define REMAINING(parser) (END(parser) - (parser)->buffer)
#define try(expr)                                                              \
  do {                                                                         \
    status = (expr);                                                           \
    if (status != PPMS_OK)                                                     \
      goto catch;                                                              \
  } while (0)
#define throw_if(cond, _status)                                                \
  do {                                                                         \
    if ((cond)) {                                                              \
      status = PPMS_##_status;                                                 \
      goto catch;                                                              \
    }                                                                          \
  } while (0)

static inline int is_whitespace(int c) {
  return c == ' ' || c == '\t' || c == '\n';
}

static inline int is_numerical(int c) { return c >= '0' && c <= '9'; }

static int parse_int(char const **offset_p, char const *end) {
  register char const *offset = *offset_p;
  int status = 0;
  for (; offset < end; ++offset) {
    int c = *offset;
    if (!is_numerical(c))
      break;
    status *= 10;
    status += c - '0';
  }
  *offset_p = offset;
  return status;
}

void ppm_skip_irrevelant(PPMParser *parser) {
  register char const *offset = parser->offset;
  char const *const end = END(parser);
  for (; offset < end;) {
    int c = *offset;
    if (c == '#') {
      for (; offset < end && c != '\n'; ++offset) {
      }
    }
    if (!is_whitespace(c))
      break;
  }
  parser->offset = offset;
}

PPMStatus ppm_match_str(PPMParser *parser, String str) {
  PPMStatus status = PPMS_OK;
  register char const *offset = parser->offset;
  char const *const str_end = &str[String_len(str)];
  throw_if(REMAINING(parser) < String_len(str), EOF);
  for (register char const *str_offset = &str[0]; str_offset < str_end;
       ++str_offset, ++offset) {
    throw_if(*offset != *str_offset, MISMATCH);
  }
  catch : parser->offset = offset;
  return status;
}

PPMStatus ppm_cell_stream(PPMParser *parser,
                          PPMStatus (*fn)(void *arg, Cell cell), void *arg,
                          int n_cells) {
  PPMStatus status = PPMS_OK;
  char const *const end = END(parser);
  for (register int i = 0; i < n_cells && parser->offset < end; ++i) {
    Cell cell;
    for (int j = 0; j < 3; ++j) {
      ppm_skip_irrevelant(parser);
      throw_if(!is_numerical(*parser->offset), NUMBER);
      int val = parse_int(&parser->offset, end);
      throw_if(val > parser->max_val, VAL_OVERFLOW);
      cell.arr[j] = val;
    }
    fn(arg, cell);
  }
  ppm_skip_irrevelant(parser);
  // these cases can't intercept, so instead of an else if, the separate ifs
  // with only assignments in their bodies can be optimized to conditional
  // move instructions instead of jumps
  if (n_cells > 0)
    status = PPMS_PIXELS_FEW;
  if (REMAINING(parser) != 0)
    status = PPMS_PIXELS_MANY;
  catch : return status;
}

struct ppm_buffer_fn_arg {
  Cell *cell;
  Cell const *end_cell;
};

__attribute__((noinline)) static PPMStatus ppm_buffer_fn(void *arg, Cell cell) {
  struct ppm_buffer_fn_arg *state = arg;
  *state->cell = cell;
  ++state->cell;
  return PPMS_OK;
}

PPMStatus ppm_cell_buffer(PPMParser *parser, Cell *cells, int n_cells) {
  PPMStatus status = PPMS_OK;
  struct ppm_buffer_fn_arg state = {cells, &cells[n_cells]};
  try(ppm_cell_stream(parser, ppm_buffer_fn, &state, n_cells));
  throw_if(state.cell < state.end_cell, PIXELS_FEW);
  catch : return status;
}
