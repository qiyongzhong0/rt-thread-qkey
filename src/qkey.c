/*
 * qkey.c
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-10-14     qiyongzhong   first version
 * 2021-08-30     qiyongzhong       add key array
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
    rt_uint8_t evt_set :7;
    rt_uint8_t level   :1;
    rt_uint8_t wave;
    qkey_evt_hook_t hook;
}qkey_inst_t;

static qkey_inst_t qkeys[QKEY_TOTAL];

#ifdef QKEY_USING_ARRAY
typedef struct{
    const qkey_array_pins_t *pins;
    rt_uint8_t waves[QKEY_ARRAY_ROW_TOTAL][QKEY_ARRAY_COL_TOTAL];
    rt_uint8_t evt_set      :6;
    rt_uint8_t level        :1;
    rt_uint8_t scan_level   :1;
    qkey_evt_hook_t hook;
}qkey_array_t;
static qkey_array_t qkey_array = {0};
#endif

static void qkey_pin_init(int pin, int level)
{
#ifdef QKEY_USING_PULL
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
    
    for(int i = 0; i < QKEY_TOTAL; i++)
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

#ifdef QKEY_USING_ARRAY
static void qkey_array_pin_chk(qkey_array_t *hinst, int row, int col)
{
    int val = hinst->waves[row][col];
    int evt_set = hinst->evt_set;
    int lvl = (rt_pin_read(hinst->pins->row[row]) != PIN_LOW);
    int evt_pin = QKEY_ARRAY_PIN_FIRST + row * QKEY_ARRAY_COL_TOTAL + col;

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
            (hinst->hook)(evt_pin, QKEY_EVT_UP);
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
            (hinst->hook)(evt_pin, QKEY_EVT_PUSH);
        }
        break;
        
    case 0x04://HLL - key pop up
        if (evt_set & QKEY_EVT_POP)
        {
            (hinst->hook)(evt_pin, QKEY_EVT_POP);
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
            (hinst->hook)(evt_pin, QKEY_EVT_DOWN);
        }
        break;
        
    default:
        break;
    }

    hinst->waves[row][col] = val;
}

static void qkey_array_scan(void)
{
    if (qkey_array.pins == RT_NULL)
    {
        rt_thread_mdelay(2);
        return;
    }
    
    for(int col = 0; col < QKEY_ARRAY_COL_TOTAL; col++)
    {
        int col_pin = qkey_array.pins->col[col];
        rt_pin_mode(col_pin, PIN_MODE_OUTPUT);
        rt_pin_write(col_pin, qkey_array.scan_level ? PIN_HIGH : PIN_LOW);
        rt_thread_mdelay(1);
        
        for (int row = 0; row < QKEY_ARRAY_ROW_TOTAL; row++)
        {
            qkey_array_pin_chk(&qkey_array, row, col);
        }
        
        rt_pin_write(col_pin, qkey_array.scan_level ? PIN_LOW : PIN_HIGH);
        rt_thread_mdelay(1);
        
        rt_pin_mode(col_pin, PIN_MODE_INPUT);
    }
}
#endif

static void qkey_thread_entry(void *params)
{
    while(1)
    {
        qkey_scan();
        #ifdef QKEY_USING_ARRAY
        qkey_array_scan();
        rt_thread_mdelay(QKEY_SCAN_PRIOD_MS - 2);
        #else
        rt_thread_mdelay(QKEY_SCAN_PRIOD_MS);
        #endif
    }
}

static int qkey_init(void)
{
    rt_thread_t tid = rt_thread_create(QKEY_THREAD_NAME, qkey_thread_entry, RT_NULL, 
                                        QKEY_THREAD_STACK_SIZE, QKEY_THREAD_PRIO, 20);
    RT_ASSERT(tid != RT_NULL);
    qkey_datas_init();
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
    if (hook == RT_NULL)
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
        LOG_E("key add fail. key space is full.");
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

#ifdef QKEY_USING_ARRAY
int qkey_array_add(const qkey_array_pins_t *pins, int scan_level, int level, int evt_set, qkey_evt_hook_t hook)
{
    if (pins == RT_NULL)
    {
        LOG_E("key array add fail. param pins is NULL.");
        return(-RT_ERROR);
    }
    if ((evt_set & 0x0F) == 0)
    {
        LOG_E("key add fail. param evt_set is empty.");
        return(-RT_ERROR);
    }
    if (hook == RT_NULL)
    {
        LOG_E("key add fail. param hook is NULL.");
        return(-RT_ERROR);
    }
    if (qkey_array.pins != RT_NULL)
    {
        LOG_E("key add fail. key array is working. please remove it first");
        return(-RT_ERROR);
    }

    rt_enter_critical();

    qkey_array.pins = pins;
    qkey_array.scan_level = scan_level;
    qkey_array.level = level;
    qkey_array.evt_set = evt_set;
    qkey_array.hook = hook;
    
    rt_exit_critical();
    
    return(RT_EOK);
}

void qkey_array_remove(void)
{
    qkey_array.pins = RT_NULL;
}
#endif

