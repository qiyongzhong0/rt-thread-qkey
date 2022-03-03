/*
 * qkey_test.c
 *
 * Change Logs:
 * Date           Author            Notes
 * 2020-10-14     qiyongzhong       first version
 * 2021-08-30     qiyongzhong       add key array
 */

#include <qkey.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>

#define DBG_TAG "drv.qkey.test"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#ifdef QKEY_USING_TEST
static const char *cmd_info[] = 
{
    "Usages:\n",
    "qkey add pin lvl evt_set               - add key to driver, evt_set - 0x01~0x0F.\n",
    "qkey remove pin                        - remove key from driver.\n",
    #ifdef QKEY_USING_ARRAY
    "qkey add_array scan_lvl lvl evt_set    - add key array to driver, evt_set - 0x01~0x0F.\n",
    "qkey remove_array                      - remove key array from driver.\n",
    #endif
    "\n"
}; 
static void show_cmd_info(void)
{
    for(int i=0; i<sizeof(cmd_info)/sizeof(char*); i++)
    {
        rt_kprintf(cmd_info[i]);
    }
}
static void qkey_test_hook(int pin, qkey_evt_t evt)
{
    rt_kprintf("pin - %d, evt - %d\n", pin, evt);
}
#ifdef QKEY_USING_ARRAY
#include <drv_common.h>
#define KEY_PIN_ROW1        GET_PIN(A, 0)
#define KEY_PIN_ROW2        GET_PIN(A, 1)
#define KEY_PIN_ROW3        GET_PIN(A, 2)
#define KEY_PIN_ROW4        GET_PIN(A, 3)

#define KEY_PIN_COL1        GET_PIN(A, 8)
#define KEY_PIN_COL2        GET_PIN(A, 9)
#define KEY_PIN_COL3        GET_PIN(A, 10)

static const qkey_array_pins_t qkey_array = {
    {KEY_PIN_ROW1, KEY_PIN_ROW2, KEY_PIN_ROW3, KEY_PIN_ROW4},
    {KEY_PIN_COL1, KEY_PIN_COL2, KEY_PIN_COL3}
};
#endif
static void qkey_test(int argc, char **argv)
{
    if (argc < 2)
    {
        show_cmd_info();
        return;
    }
    
    if (strcmp(argv[1], "add") == 0)
    {
        int pin, lvl, evt_set;
        
        if (argc < 5)
        {
            rt_kprintf(cmd_info[1]);
            return;
        }

        pin = atoi(argv[2]);
        lvl = atoi(argv[3]);
        evt_set = atoi(argv[4]);
        if (qkey_add(pin, lvl, evt_set, qkey_test_hook) < 0)
        {
            rt_kprintf("add key fail.\n");
            return;
        }
        
        rt_kprintf("add key success.\n");
        return;
    }
    
    if (strcmp(argv[1], "remove") == 0)
    {
        int pin;
        
        if (argc < 3)
        {
            rt_kprintf(cmd_info[2]);
            return;
        }

        pin = atoi(argv[2]);
        qkey_remove(pin);
        rt_kprintf("remove key success.\n");
        return;
    }

    #ifdef QKEY_USING_ARRAY
    if (strcmp(argv[1], "add_array") == 0)
    {
        int scan_lvl, lvl, evt_set;
        
        if (argc < 5)
        {
            rt_kprintf(cmd_info[3]);
            return;
        }

        scan_lvl = atoi(argv[2]);
        lvl = atoi(argv[3]);
        evt_set = atoi(argv[4]);
        if (qkey_array_add(&qkey_array, scan_lvl, lvl, evt_set, qkey_test_hook) < 0)
        {
            rt_kprintf("add key array fail.\n");
            return;
        }
        
        rt_kprintf("add key array success.\n");
        return;
    }
    
    if (strcmp(argv[1], "remove_array") == 0)
    {
        qkey_array_remove();
        rt_kprintf("remove key array success.\n");
        return;
    }
    #endif
    
    rt_kprintf("No supported command.\n");
}
MSH_CMD_EXPORT_ALIAS(qkey_test, qkey, test qkey driver function)
#endif

