#ifndef PPM_PARSER_H
#define PPM_PARSER_H
#include <c-string/lib.h>
#include <stdint.h>

typedef union {
  struct {
    uint16_t r;
    uint16_t g;
    uint16_t b;
  };
  uint16_t arr[3];
} Cell;

#define CVECTOR_POINTERMODE
#include <c-vector/lib.h>
CVECTOR_WITH_NAME(Cell, GridBuffer);

typedef struct {
  int rows;
  int cols;
  uint16_t max_val;
  GridBuffer buffer;
} Grid;

typedef struct {
  uint16_t max_val;
  StringBuffer buffer;
  char const *offset;
} PPMParser;

typedef enum {
  PPMS_OK = 0,
  PPMS_MISMATCH,
  PPMS_NUMBER,
  PPMS_VAL_OVERFLOW,
  PPMS_PIXELS_MANY,
  PPMS_PIXELS_FEW,
  PPMS_INVALID,
  PPMS_EOF,
} PPMStatus;

#endif /* PPM_PARSER_H */
