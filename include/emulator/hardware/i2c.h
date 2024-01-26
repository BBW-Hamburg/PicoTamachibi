#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct i2c_inst {
    char pad;
} static *i2c0, *i2c1;

typedef struct i2c_inst i2c_inst_t;


inline static void i2c_init(i2c_inst_t *gpio, unsigned) {}

#ifdef __cplusplus
}
#endif
