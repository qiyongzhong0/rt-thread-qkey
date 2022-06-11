/*
 * qkey_sample_super.c
 *
 * Change Logs:
 * Date           Author            Notes
 * 2022-06-11     qiyongzhong       first version
 */

#include <qkey.h>

#ifdef QKEY_USING_SAMPLE_SUPER

#include <rtthread.h>
#include <drv_common.h>

#define DBG_TAG "qkey.sample.super"
#define DBG_LVL DBG_LOG //DBG_INFO
#include <rtdbg.h>

#define KEY_CLICK_SGL_CON_MS        300     //单击确认阈值, 键抬起后确认为单击的超时时间
#define KEY_CLICK_SGL_MAX_MS        1000    //单击许可最大阈值, 不超过此时长时才判单击有效
#define KEY_CLICK_LONG_MS           5000    //长按阈值, 按下到抬起之间超过此值, 判为长按

#define KEY_PIN_CFG {       \
    {GET_PIN(C, 9), 0},     \
}

typedef struct{
    int pin;
    int lvl;
}key_cfg_t;

static const key_cfg_t keys_cfg[] = KEY_PIN_CFG;

#define KEY_TOTAL   (sizeof(keys_cfg) / sizeof(keys_cfg[0]))

typedef struct{
    rt_tick_t prev_tick;
    int  click_cnt;
    bool start_con;
}key_data_t;

static key_data_t keys_data[KEY_TOTAL];

static void keys_click_single_deal(int pin)//键单击处理
{
    LOG_D("pin - %d : single click.", pin);
}

static void keys_click_mult_deal(int pin, int times)//键多击处理
{
    LOG_D("pin - %d : %d times click.", pin, times);
}

static void keys_click_long_deal(int pin)//键长按处理
{
    LOG_D("pin - %d : long push.", pin);
}

static int keys_idx_get(int pin)
{
    for(int i=0; i<KEY_TOTAL; i++)
    {
        if (pin == keys_cfg[i].pin)
        {
            return(i);
        }
    }
    return(-1);
}

static void keys_evt_hook(int pin, qkey_evt_t evt)
{
    int idx = keys_idx_get(pin);
    if (idx < 0)
    {
        return;
    }
    
    rt_tick_t curr_tick = rt_tick_get();
    switch (evt)
    {
    case QKEY_EVT_PUSH://按下
        keys_data[idx].prev_tick = curr_tick;
        if (keys_data[idx].start_con)//已开始确认单击, 即在被确认前再次按下
        {
            keys_data[idx].click_cnt++;
        }
        else//新的一次按键
        {
            keys_data[idx].click_cnt = 0;
        }
        break;
    case QKEY_EVT_POP://抬起
        //按下时长超过长按阈值
        if (curr_tick - keys_data[idx].prev_tick >= rt_tick_from_millisecond(KEY_CLICK_LONG_MS))
        {
            keys_click_long_deal(pin);//长按处理
            keys_data[idx].start_con = false;//关闭确认单击
            break;
        }
        //按下时长不超过单击许可最大阈值
        if (curr_tick - keys_data[idx].prev_tick <= rt_tick_from_millisecond(KEY_CLICK_SGL_MAX_MS))
        {
            keys_data[idx].prev_tick = curr_tick;
            keys_data[idx].start_con = true;//开启确认单击
            break;
        }
        //按下时长超过单击许可最大阈值, 按键无效
        keys_data[idx].start_con = false;//关闭确认单击
        break;
    case QKEY_EVT_UP://持续抬起
        if (keys_data[idx].start_con)//已开始确认单击
        {
            //抬起时长达到单击确认阈值, 即得到确认
            if (curr_tick - keys_data[idx].prev_tick >= rt_tick_from_millisecond(KEY_CLICK_SGL_CON_MS))
            {
                keys_data[idx].click_cnt++;
                if (keys_data[idx].click_cnt == 1)
                {
                    keys_click_single_deal(pin);//单击处理
                }
                else
                {
                    keys_click_mult_deal(pin, keys_data[idx].click_cnt);//多击处理
                }
                keys_data[idx].start_con = false;//关闭确认单击
            }
        }
        break;
    default:
        break;
    }
}


int keys_init(void)
{
    for (int i=0; i<KEY_TOTAL; i++)
    {
        qkey_add(keys_cfg[i].pin, keys_cfg[i].lvl, (QKEY_EVT_PUSH | QKEY_EVT_POP | QKEY_EVT_UP), keys_evt_hook);
    }
    return(0);
}
INIT_APP_EXPORT(keys_init);

#endif

