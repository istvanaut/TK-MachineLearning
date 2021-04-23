/**
  ******************************************************************************
  * @file    mcu_nn_data.h
  * @author  AST Embedded Analytics Research Platform
  * @date    Fri Apr 23 10:43:23 2021
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef MCU_NN_DATA_H
#define MCU_NN_DATA_H
#pragma once

#include "mcu_nn_config.h"
#include "ai_platform.h"

#define AI_MCU_NN_DATA_CONFIG           AI_HANDLE_NULL

#define AI_MCU_NN_DATA_ACTIVATIONS_SIZE     (1024)

#define AI_MCU_NN_DATA_WEIGHTS_SIZE         (170512)

#define AI_MCU_NN_DATA_ACTIVATIONS(ptr_)  \
  AI_BUFFER_OBJ_INIT( \
    AI_BUFFER_FORMAT_U8, \
    1, 1, AI_MCU_NN_DATA_ACTIVATIONS_SIZE, 1, \
    AI_HANDLE_PTR(ptr_) )

#define AI_MCU_NN_DATA_WEIGHTS(ptr_)  \
  AI_BUFFER_OBJ_INIT( \
    AI_BUFFER_FORMAT_U8|AI_BUFFER_FMT_FLAG_CONST, \
    1, 1, AI_MCU_NN_DATA_WEIGHTS_SIZE, 1, \
    AI_HANDLE_PTR(ptr_) )


AI_API_DECLARE_BEGIN

/*!
 * @brief Get network weights array pointer as a handle ptr.
 * @ingroup mcu_nn_data
 * @return a ai_handle pointer to the weights array
 */
AI_API_ENTRY
ai_handle ai_mcu_nn_data_weights_get(void);


AI_API_DECLARE_END

#endif /* MCU_NN_DATA_H */

