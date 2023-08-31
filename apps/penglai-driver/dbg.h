#ifndef __dbg_h__
#define __dbg_h__

#include <linux/kernel.h>
#include <linux/string.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) printk("[DEBUG] (%s:%d,%s) " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#endif

#define log_err(M, ...) printk("[ERROR] (%s:%d,%s) " M " \n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define log_warn(M, ...) printk("[WARN] (%s:%d,%s) " M " \n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define log_info(M, ...) printk("[INFO] (%s:%d,%s) " M " \n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define check(A, M, ...)           \
    if (!(A))                      \
    {                              \
        log_err(M, ##__VA_ARGS__); \
        goto error;                \
    }

#define sentinel(M, ...)           \
    {                              \
        log_err(M, ##__VA_ARGS__); \
        goto error;                \
    }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...)   \
    if (!(A))                    \
    {                            \
        debug(M, ##__VA_ARGS__); \
        goto error;              \
    }

#endif