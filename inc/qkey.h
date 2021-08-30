/*
 * qkey.h
 *
 * Change Logs:
 * Date           Author            Notes
 * 2020-10-14     qiyongzhong       first version
 * 2021-08-30     qiyongzhong       add key array
 */

#ifndef __QKEY_H__
#define __QKEY_H__

#include <rtthread.h>

//#define QKEY_USING_TEST
//#define QKEY_USING_PULL
//#define QKEY_USING_ARRAY

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

#ifdef QKEY_USING_ARRAY

#define QKEY_ARRAY_PIN_FIRST    256 //按键阵列事件回调的第1个引脚号

#ifndef QKEY_ARRAY_ROW_TOTAL
#define QKEY_ARRAY_ROW_TOTAL    4   //按键阵列的行总数
#endif

#ifndef QKEY_ARRAY_COL_TOTAL
#define QKEY_ARRAY_COL_TOTAL    3   //按键阵列的列总数
#endif

typedef struct{
    rt_int16_t row[QKEY_ARRAY_ROW_TOTAL];
    rt_int16_t col[QKEY_ARRAY_COL_TOTAL];
}qkey_array_pins_t;

/* 
 * @brief   add key array to driver
 * @param   pin         - key input pin
 * @param   scan_level  - key column scan level
 * @param   level       - key push down level
 * @param   hook        - event callback function
 * @retval  0 - success, other - error
 */
int qkey_array_add(const qkey_array_pins_t *pins, int scan_level, int level, int evt_set, qkey_evt_hook_t hook);

/* 
 * @brief   remove key array from driver
 * @param   none
 * @retval  none
 */
void qkey_array_remove(void);
#endif

#endif

