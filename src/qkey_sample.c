/*
 * qkey_sample.c
 *
 * Change Logs:
 * Date           Author            Notes
 * 2022-06-10     qiyongzhong       first version
 */

#include <qkey.h>

#ifdef QKEY_USING_SAMPLE

#include <qkey_sample.h>
#include <drv_common.h>


#define KEY_PIN_ACT         GET_PIN(C, 9)
#define KEY_PIN_REC         GET_PIN(C, 8)
#define KEY_PIN_EXIT        GET_PIN(C, 7)
#define KEY_PIN_START       GET_PIN(C, 6)
#define KEY_PIN_ESC         GET_PIN(D, 15)
#define KEY_PIN_ENTER       GET_PIN(D, 14)
#define KEY_PIN_DOWN        GET_PIN(D, 13)
#define KEY_PIN_RIGHT       GET_PIN(D, 12)
#define KEY_PIN_LEFT        GET_PIN(D, 11)
#define KEY_PIN_UP          GET_PIN(D, 10)

#ifdef QKEY_USING_ARRAY
#define KEY_PIN_ROW1        GET_PIN(A, 12)
#define KEY_PIN_ROW2        GET_PIN(A, 15)
#define KEY_PIN_ROW3        GET_PIN(C, 10)
#define KEY_PIN_ROW4        GET_PIN(D, 1)

#define KEY_PIN_COL1        GET_PIN(D, 0)
#define KEY_PIN_COL2        GET_PIN(C, 12)
#define KEY_PIN_COL3        GET_PIN(C, 11)

static const qkey_array_pins_t kb_array_pins = {
    {KEY_PIN_ROW1, KEY_PIN_ROW2, KEY_PIN_ROW3, KEY_PIN_ROW4},
    {KEY_PIN_COL1, KEY_PIN_COL2, KEY_PIN_COL3}
};
#endif
static rt_mailbox_t kb_mb = RT_NULL;

static void kb_evt_hook(int pin, qkey_evt_t evt)
{
    //rt_kprintf("key push on pin - %d\n", pin);

    if (evt != QKEY_EVT_PUSH)
    {
        return;
    }

    switch(pin)
    {
    case KEY_PIN_UP:
        rt_mb_send(kb_mb, VK_UP);
        break;
    case KEY_PIN_DOWN:
        rt_mb_send(kb_mb, VK_DOWN);
        break;
    case KEY_PIN_LEFT:
        rt_mb_send(kb_mb, VK_LEFT);
        break;
    case KEY_PIN_RIGHT:
        rt_mb_send(kb_mb, VK_RIGHT);
        break;
    case KEY_PIN_ENTER:
        rt_mb_send(kb_mb, VK_CANCEL);
        break;
    case KEY_PIN_ESC:
        rt_mb_send(kb_mb, VK_ENTER);
        break;
    #ifdef QKEY_USING_ARRAY
    case (QKEY_ARRAY_PIN_FIRST + 0):
        rt_mb_send(kb_mb, VK_1);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 1):
        rt_mb_send(kb_mb, VK_2);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 2):
        rt_mb_send(kb_mb, VK_3);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 3):
        rt_mb_send(kb_mb, VK_4);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 4):
        rt_mb_send(kb_mb, VK_5);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 5):
        rt_mb_send(kb_mb, VK_6);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 6):
        rt_mb_send(kb_mb, VK_7);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 7):
        rt_mb_send(kb_mb, VK_8);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 8):
        rt_mb_send(kb_mb, VK_9);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 9):
        rt_mb_send(kb_mb, VK_XING);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 10):
        rt_mb_send(kb_mb, VK_0);
        break;
    case (QKEY_ARRAY_PIN_FIRST + 11):
        rt_mb_send(kb_mb, VK_JING);
        break;
    #endif
    default:
        break;
    }
}

static int kb_init(void)
{
    kb_mb = rt_mb_create("kb_mb", 8, RT_IPC_FLAG_FIFO);
    RT_ASSERT(kb_mb != RT_NULL);
        
    qkey_add(KEY_PIN_ACT,   0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_REC,   0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_EXIT,  0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_START, 0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_ESC,   0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_ENTER, 0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_DOWN,  0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_RIGHT, 0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_LEFT,  0, QKEY_EVT_PUSH, kb_evt_hook);
    qkey_add(KEY_PIN_UP,    0, QKEY_EVT_PUSH, kb_evt_hook);

    #ifdef QKEY_USING_ARRAY
    qkey_array_add(&kb_array_pins, 0, 0, QKEY_EVT_PUSH, kb_evt_hook);
    #endif

    return(RT_EOK);
}
INIT_APP_EXPORT(kb_init);

vk_t kb_key_get(int tmo_ms)
{
    rt_ubase_t key;
    rt_err_t rst = rt_mb_recv(kb_mb, &key, rt_tick_from_millisecond(tmo_ms));
    if (rst != RT_EOK)
    {
        return(0);
    }

    return((vk_t)key);
}

#endif
