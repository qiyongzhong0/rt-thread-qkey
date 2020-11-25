/*
 * qkey.c
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-10-14     qiyongzhong   first version
 */

#include <qkey.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>

#define DBG_TAG "drv.qkey"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

typedef struct{
    rt_int16_t pin;
    rt_uint8_t level   :1;
    rt_uint8_t evt_set :7;
    rt_uint8_t wave;
    qkey_evt_hook_t hook;
}qkey_inst_t;

static qkey_inst_t qkeys[QKEY_TOTAL];

static void qkey_pin_init(int pin, int level)
{
#if QKEY_PIN_PULL_EN
    if (level == PIN_LOW)
    {
        rt_pin_mode(pin, PIN_MODE_INPUT_PULLUP);
    }
    else
    {
        rt_pin_mode(pin, PIN_MODE_INPUT_PULLDOWN);
    }
#else
    rt_pin_mode(pin, PIN_MODE_INPUT);
#endif
}

static void qkey_pin_deinit(int pin)
{
    rt_pin_mode(pin, PIN_MODE_INPUT);
}

static void qkey_datas_init(void)
{
    memset((void *)qkeys, 0, sizeof(qkeys));
    
    for(int i; i<QKEY_TOTAL; i++)
    {
        qkeys[i].pin = -1;
    }
}

static int qkey_find(int pin)
{
    for (int i = 0; i < QKEY_TOTAL; i++)
    {
        if (qkeys[i].pin == pin)
        {
            return(i);
        }
    } 
    return(-1);
}

static int qkey_get_unused(void)
{
    for (int i = 0; i < QKEY_TOTAL; i++)
    {
        if (qkeys[i].pin == -1)
        {
            return(i);
        }
    } 
    return(-1);
}

static void qkey_pin_chk(qkey_inst_t *hinst)
{
    int val = hinst->wave;
    int evt_set = hinst->evt_set;
    int lvl = (rt_pin_read(hinst->pin) != PIN_LOW);

    val <<= 1;
    if (lvl == hinst->level)//press down
    {
        val |= 0x01;
    }
    val &= 0x07;

    switch(val)
    {
    case 0x00://LLL - key hold up
        if (evt_set & QKEY_EVT_UP)
        {
            (hinst->hook)(hinst->pin, QKEY_EVT_UP);
        }
        break;
        
    case 0x01://LLH - skip
        break;
    
    case 0x02://LHL - pulse disturb
        val = 0;
        break;
        
    case 0x03://LHH - key press down
        if (evt_set & QKEY_EVT_PUSH)
        {
            (hinst->hook)(hinst->pin, QKEY_EVT_PUSH);
        }
        break;
        
    case 0x04://HLL - key pop up
        if (evt_set & QKEY_EVT_POP)
        {
            (hinst->hook)(hinst->pin, QKEY_EVT_POP);
        }
        break;
    
    case 0x05://HLH - pulse disturb
        val = 0x07;
        break;

    case 0x06://HHL - skip
        break;
        
    case 0x07://HHH - key hold down
        if (evt_set & QKEY_EVT_DOWN)
        {
            (hinst->hook)(hinst->pin, QKEY_EVT_DOWN);
        }
        break;
        
    default:
        break;
    }

    hinst->wave = val;
}

static void qkey_scan(void)
{
    for (int i = 0; i < QKEY_TOTAL; i++)
    {
        if (qkeys[i].pin == -1)
        {
            continue;
        }
        
        qkey_pin_chk(qkeys + i);
    }
}

static void qkey_thread_entry(void *params)
{
    qkey_datas_init();

    while(1)
    {
        qkey_scan();
        rt_thread_mdelay(QKEY_SCAN_PRIOD_MS);
    }
}

static int qkey_init(void)
{
    rt_thread_t tid = rt_thread_create(QKEY_THREAD_NAME, qkey_thread_entry, RT_NULL, 
                                        QLED_THREAD_STACK_SIZE, QLED_THREAD_PRIO, 20);
    rt_thread_startup(tid);
    
    return(RT_EOK);
}
INIT_PREV_EXPORT(qkey_init);

int qkey_add(int pin, int level, int evt_set, qkey_evt_hook_t hook)
{
    int idx;

    if (pin < 0)
    {
        LOG_E("key add fail. param pin is error.");
        return(-RT_ERROR);
    }
    if ((evt_set & 0x0F) == 0)
    {
        LOG_E("key add fail. param evt_set is empty.");
        return(-RT_ERROR);
    }
    if (hook == NULL)
    {
        LOG_E("key add fail. param hook is NULL.");
        return(-RT_ERROR);
    }
    
    idx = qkey_find(pin);
    if (idx < 0)
    {
        idx = qkey_get_unused();
    }
    
    if (idx < 0)
    {
        LOG_E("led add fail. led space is full.");
        return(-RT_ERROR);
    }
    
    qkey_pin_init(pin, level);
    
    rt_enter_critical();
    
    qkeys[idx].pin = pin;
    qkeys[idx].level = (level != PIN_LOW);
    qkeys[idx].evt_set = evt_set;
    qkeys[idx].wave = 0;
    qkeys[idx].hook = hook;
    
    rt_exit_critical();

    return(RT_EOK);
}

void qkey_remove(int pin)
{
    int idx;

    if (pin < 0)
    {
        return;
    }
    
    idx = qkey_find(pin);
    if (idx < 0)
    {
        return;
    }
    
    qkey_pin_deinit(pin);
    
    qkeys[idx].pin = -1;
}

