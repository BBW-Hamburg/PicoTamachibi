#include <stdbool.h>
#define FONT_HEIGHT 16
#define FONT_WIDTH  8
#define FONT_SIZE   FONT_HEIGHT * FONT_WIDTH


extern const bool fontBitmapUnk[FONT_SIZE];


#ifdef __cplusplus
extern "C" {
#endif
const bool *font_get_character(const char character);
#ifdef __cplusplus
}
#endif
