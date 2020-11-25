/*
 * qkey.h
 *
 * Change Logs:
 * Date           Author            Notes
 * 2020-10-14     qiyongzhong       first version
 */

#ifndef __QKEY_H__
#define __QKEY_H__

#include <rtconfig.h>

//#define QKEY_USING_TEST

#ifndef QKEY_PIN_PULL_EN
#define QKEY_PIN_PULL_EN        1
#endif

#ifndef QKEY_TOTAL
#define QKEY_TOTAL              5       //support key maximum total
#endif

#ifndef QKEY_SCAN_PRIOD_MS
#define QKEY_SCAN_PRIOD_MS      10      //key scan time unit
#endif

#ifndef QKEY_THREAD_NAME
#define QKEY_THREAD_NAME        "qkey"   //key driver thread name
#endif

#ifndef QKEY_THREAD_STACK_SIZE
#define QKEY_THREAD_STACK_SIZE  1024    //key driver thread stack size
#endif

#ifndef QKEY_THREAD_PRIO
#define QKEY_THREAD_PRIO        3       //key driver thread priority
#endif

typedef enum{
    QKEY_EVT_PUSH   = (1<<0),//push down
    QKEY_EVT_POP    = (1<<1),//pop up
    QKEY_EVT_DOWN   = (1<<2),//hold push down
    QKEY_EVT_UP     = (1<<3) //hold pop up
}qkey_evt_t;

typedef void (*qkey_evt_hook_t)(int pin, qkey_evt_t evt);

/* 
 * @brief   add key to driver
 * @param   pin      - key input pin
 * @param   level    - key push down level
 * @param   hook     - event callback function
 * @retval  0 - success, other - error
 */
int qkey_add(int pin, int level, int evt_set, qkey_evt_hook_t hook);

/* 
 * @brief   remove key from driver
 * @param   pin      - key input pin
 * @retval  none
 */
void qkey_remove(int pin);

#endif

