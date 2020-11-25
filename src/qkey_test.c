/*
 * qkey_test.c
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-10-14     qiyongzhong   first version
 */

#include <qkey.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>

#define DBG_TAG "drv.qkey.test"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#ifdef QKEY_TEST
static const char *cmd_info[] = 
{
    "Usages:\n",
    "qkey add pin lvl evt_set   - add key to driver, evt_set - 0x01~0x0F.\n",
    "qkey remove pin            - remove key from driver.\n",
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
        rt_kprintf("add remove success.\n");
        return;
    }
    
    rt_kprintf("No supported command.\n");
}
MSH_CMD_EXPORT_ALIAS(qkey_test, qkey, test qkey driver function)
#endif

