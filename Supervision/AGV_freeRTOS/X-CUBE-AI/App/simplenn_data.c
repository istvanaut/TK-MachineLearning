/**
******************************************************************************
* @file    simplenn_data.c
* @author  AST Embedded Analytics Research Platform
* @date    Fri Apr 23 10:53:18 2021
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

#include "simplenn_data.h"

ai_handle ai_simplenn_data_weights_get(void)
{

  return AI_HANDLE_PTR(s_simplenn_weights);
}
