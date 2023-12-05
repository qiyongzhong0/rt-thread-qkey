# Quick key

## 1.简介

**Quick key** 旨在提供一个快捷易用的按键驱动包。

### 1.1目录结构

`Quick key` 软件包目录结构如下所示：

``` 
key
├───inc                         // 头文件目录
│   └───qkey.h                  // API 接口头文件
├───src                         // 源码目录
│   |   qkey.c                  // 主模块
│   └───qkey_test.c             // 测试模块
│   license                     // 软件包许可证
│   readme.md                   // 软件包使用说明
└───SConscript                  // RT-Thread 默认的构建脚本
```

### 1.2许可证

Quick key package 遵循 LGPLv2.1 许可，详见 `LICENSE` 文件。

### 1.3依赖

- RT_Thread 4.0

## 2.使用

### 2.1接口函数说明

#### 按键事件定义：
```c
typedef enum{
    QKEY_EVT_PUSH   = (1<<0),//键按下
    QKEY_EVT_POP    = (1<<1),//键抬起
    QKEY_EVT_DOWN   = (1<<2),//键持续按下
    QKEY_EVT_UP     = (1<<3) //键持续抬起
}qkey_evt_t;
```

#### 按键事件回调函数定义：
```c
typedef void (*qkey_evt_hook_t)(int pin, qkey_evt_t evt);
```

#### int qkey_add(int pin, int level, int evt_set, qkey_evt_hook_t hook);
- 功能 ：加一个key到驱动
- 参数 ：pin--key使用的引脚号
- 参数 ：level--key按下后的电平
- 参数 ：hook--key事件回调函数
- 返回 ：0--成功，其它--错误

#### void qkey_remove(int pin);
- 功能 ：从驱动移除key
- 参数 ：pin--key使用的引脚号
- 返回 ：无

#### int qkey_array_add(const qkey_array_pins_t *pins, int scan_level, int level, int evt_set, qkey_evt_hook_t hook);
- 功能 ：加按键阵列到驱动
- 参数 ：pins--按键阵列的行列引脚数据
- 参数 ：scan_level--列扫描电平, 注:键阵列是采用列扫描模式设计的
- 参数 ：level--键按下后的电平
- 参数 ：hook--key事件回调函数
- 返回 ：0--成功，其它--错误

#### void qkey_array_remove(void);
- 功能 ：从驱动移除按键阵列
- 参数 ：无
- 返回 ：无

### 2.2测试功能

#### 在启用测试功能情况下，可使用shell命令行测试按键功能。

- ##### `qkey` 列表帮助信息
- ##### `qkey add pin lvl evt_set` 将按键加入到驱动，参数pin、lvl、evt_set分别为引脚号、按下电平、关心的事件集，如：`qkey add 16 0 3` 加按键，引脚为PB0，按下电平低，关心事件为按下、抬起。
- ##### `qkey remove pin` 将按键从驱动中移除，参数pin为引脚号。
- ##### `qkey add_array scan_lvl lvl evt_set` 将按键阵列加入到驱动，参数scan_lvl、lvl、evt_set分别为列扫描电平、键按下电平、关心的事件集，如：`qkey add_array 0 0 3` 加按键阵列，扫描电平低，键按下电平低，关心事件为按下、抬起。
- ##### `qkey remove_array` 将按键阵列从驱动中移除。

### 2.3获取组件

- **方式1：**
通过 *Env配置工具* 或 *RT-Thread studio* 开启软件包，根据需要配置各项参数；配置路径为 *RT-Thread online packages -> peripherals packages -> quick key* 


### 2.4配置参数说明

| 参数宏 | 说明 |
| ---- | ---- |
| QKEY_USING_TEST 			| 使用测试功能
| QKEY_USING_PULL			| 使用引脚上下拉功能 
| QKEY_TOTAL 				| 支持的按键总数
| QKEY_SCAN_PRIOD_MS 		| 按键扫描周期
| QKEY_THREAD_NAME 			| 按键驱动线程名称
| QKEY_THREAD_STACK_SIZE 	| 按键驱动线程堆栈尺寸
| QKEY_THREAD_PRIO 			| 按键驱动线程优先级
| QKEY_USING_ARRAY 			| 使用按键阵列
| QKEY_ARRAY_ROW_TOTAL 		| 按键阵列的行总数
| QKEY_ARRAY_COL_TOTAL 		| 按键阵列的列总数
| QKEY_ARRAY_PIN_FIRST 		| 按键阵列事件回调的第1个引脚号

## 3. 联系方式

* 维护：qiyongzhong
* 主页：https://github.com/qiyongzhong0/rt-thread-qkey
* 主页：https://gitee.com/qiyongzhong0/rt-thread-qkey
* 邮箱：917768104@qq.com
