/*
 * qkey_sample.h
 *
 * Change Logs:
 * Date           Author            Notes
 * 2022-06-10     qiyongzhong       first version
 */

#ifndef __QKEY_SAMPLE_H__
#define __QKEY_SAMPLE_H__

typedef enum{
    VK_NONE = 0,
    VK_UP,
    VK_DOWN,
    VK_LEFT,
    VK_RIGHT,
    VK_ENTER,
    VK_CANCEL,
    VK_0 = '0',
    VK_1 = '1',
    VK_2 = '2',
    VK_3 = '3',
    VK_4 = '4',
    VK_5 = '5',
    VK_6 = '6',
    VK_7 = '7',
    VK_8 = '8',
    VK_9 = '9',
    VK_XING = '*',
    VK_JING = '#',
}vk_t;

vk_t kb_key_get(int tmo_ms);//读取键盘按键, 超时无按键返回 VK_NONE

#endif

