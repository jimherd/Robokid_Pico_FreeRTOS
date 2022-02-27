/**
 * @file    common.h
 * @author  Jim Herd (you@domain.com)
 * @brief   Prototypes for common.c
 * @date    2022-02-26
 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include "system.h"

void update_task_execution_time(task_t task, uint32_t start_time, uint32_t end_time) ;

#endif