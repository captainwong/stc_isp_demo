/**
 * @file ringbuf.h
 * @brief Ring buffer implementation.
 * This file provides a simple ring buffer structure and its associated operations.
 * It allows for efficient reading and writing of data in a circular buffer manner.
 *
 * This is the left-value version of the ring buffer, all ringbuf_t object used by macros are left-value.
 * @author captainwong (1281261856#qq.com)
 * @date 2025-08-02
 */

#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#ifdef __RINGBUF_POINTER_H__
#error "ringbuf_p.h and ringbuf.h are not compatible. Please include only one of them."
#endif

#include "emb_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RINGBUF_DATA_TYPE
#define RINGBUF_DATA_TYPE uint8_t
#endif

#ifndef RINGBUF_SIZE_TYPE
#define RINGBUF_SIZE_TYPE uint8_t
#endif

typedef struct {
    RINGBUF_DATA_TYPE *buf;
    RINGBUF_SIZE_TYPE size;
    RINGBUF_SIZE_TYPE r, w;
} ringbuf_t;

#define ringbuf_valid(rb) ((rb).buf && (rb).size)

#define ringbuf_init(rb, buf_, size_) \
    do {                              \
        (rb).buf = buf_;              \
        (rb).size = size_;            \
        (rb).r = (rb).w = 0;          \
    } while (0)

#define ringbuf_clear(rb)      \
    do {                       \
        ((rb).r = (rb).w = 0); \
    } while (0)

#define ringbuf_writable(rb) ((rb).w >= (rb).r ? (rb).size - (rb).w + (rb).r - 1 : (rb).r - (rb).w - 1)

#define ringbuf_readable(rb) ((rb).w >= (rb).r ? (rb).w - (rb).r : (rb).size - (rb).r + (rb).w)

#define ringbuf_linear_readable(rb) ((rb).w >= (rb).r ? (rb).w - (rb).r : (rb).size - (rb).r)

#define ringbuf_linear_addr(rb) (&(rb).buf[(rb).r])

#define ringbuf_write(rb, b)         \
    do {                             \
        (rb).buf[(rb).w] = b;        \
        if (++(rb).w == (rb).size) { \
            (rb).w = 0;              \
        }                            \
    } while (0)

#define ringbuf_write_n(rb, buf_, len_)                                        \
    do {                                                                       \
        RINGBUF_SIZE_TYPE EMB_DATA_MODIFIER i, tosend = len_;                  \
        RINGBUF_SIZE_TYPE EMB_DATA_MODIFIER writable = ringbuf_writable((rb)); \
        if (tosend > writable) {                                               \
            tosend = writable;                                                 \
        }                                                                      \
        for (i = 0; i < tosend; i++) {                                         \
            ringbuf_write((rb), (buf_)[i]);                                    \
        }                                                                      \
    } while (0)

#define ringbuf_read(rb, res)        \
    do {                             \
        (res) = (rb).buf[(rb).r];    \
        if (++(rb).r == (rb).size) { \
            (rb).r = 0;              \
        }                            \
    } while (0)

#define ringbuf_read_clear(rb, res)  \
    do {                             \
        (res) = (rb).buf[(rb).r];    \
        (rb).buf[(rb).r] = 0;        \
        if (++(rb).r == (rb).size) { \
            (rb).r = 0;              \
        }                            \
    } while (0)

#define ringbuf_read_n(rb, buf_, len_)                                       \
    do {                                                                     \
        RINGBUF_SIZE_TYPE EMB_DATA_MODIFIER i, toread = len_;                \
        RINGBUF_SIZE_TYPE EMB_DATA_MODIFIER readable = ringbuf_readable(rb); \
        if (toread > readable) {                                             \
            toread = readable;                                               \
        }                                                                    \
        for (i = 0; i < toread; i++) {                                       \
            (buf_)[i] = (rb).buf[(rb).r];                                    \
            if (++(rb).r == (rb).size) {                                     \
                (rb).r = 0;                                                  \
            }                                                                \
        }                                                                    \
    } while (0)

#define ringbuf_read_n_clear(rb, buf_, len_)                                 \
    do {                                                                     \
        RINGBUF_SIZE_TYPE EMB_DATA_MODIFIER i, toread = len_;                \
        RINGBUF_SIZE_TYPE EMB_DATA_MODIFIER readable = ringbuf_readable(rb); \
        if (toread > readable) {                                             \
            toread = readable;                                               \
        }                                                                    \
        for (i = 0; i < toread; i++) {                                       \
            (buf_)[i] = (rb).buf[(rb).r];                                    \
            (rb).buf[(rb).r] = 0;                                            \
            if (++(rb).r == (rb).size) {                                     \
                (rb).r = 0;                                                  \
            }                                                                \
        }                                                                    \
    } while (0)

#define ringbuf_peek(rb, res)     \
    do {                          \
        (res) = (rb).buf[(rb).r]; \
    } while (0)

#define ringbuf_skip(rb)             \
    do {                             \
        if (++(rb).r == (rb).size) { \
            (rb).r = 0;              \
        }                            \
    } while (0)

#define ringbuf_skip_clear(rb)       \
    do {                             \
        (rb).buf[(rb).r] = 0;        \
        if (++(rb).r == (rb).size) { \
            (rb).r = 0;              \
        }                            \
    } while (0)

#define ringbuf_skip_n(rb, n)                        \
    do {                                             \
        if ((rb).r + (size_t)n >= (rb).size) {       \
            (rb).r = (rb).r + (size_t)n - (rb).size; \
        } else {                                     \
            (rb).r += (n);                           \
        }                                            \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif  // __RINGBUF_H__
