/**
  ******************************************************************************
  * @file    simplenn.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Fri Apr 30 10:44:22 2021
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


#include "simplenn.h"

#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "core_convert.h"

#include "layers.h"



/*
#define AI_TOOLS_VERSION_MAJOR 6
#define AI_TOOLS_VERSION_MINOR 0
#define AI_TOOLS_VERSION_MICRO 0
#define AI_TOOLS_VERSION_EXTRA "RC6"

*/

/*
#define AI_TOOLS_API_VERSION_MAJOR 1
#define AI_TOOLS_API_VERSION_MINOR 4
#define AI_TOOLS_API_VERSION_MICRO 0
*/

#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_simplenn
 
#undef AI_SIMPLENN_MODEL_SIGNATURE
#define AI_SIMPLENN_MODEL_SIGNATURE     "164e32791dd83822876da2e8eca7bd09"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "Fri Apr 30 10:44:22 2021"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_SIMPLENN_N_BATCHES
#define AI_SIMPLENN_N_BATCHES         (1)

/**  Forward network declaration section  *************************************/
AI_STATIC ai_network AI_NET_OBJ_INSTANCE;


/**  Forward network array declarations  **************************************/
AI_STATIC ai_array image_output_array;   /* Array #0 */
AI_STATIC ai_array node_10_output_array;   /* Array #1 */
AI_STATIC ai_array node_12_output_array;   /* Array #2 */
AI_STATIC ai_array node_14_output_array;   /* Array #3 */
AI_STATIC ai_array node_16_output_array;   /* Array #4 */
AI_STATIC ai_array node_18_output_array;   /* Array #5 */
AI_STATIC ai_array node_20_output_array;   /* Array #6 */
AI_STATIC ai_array node_22_output_array;   /* Array #7 */
AI_STATIC ai_array node_10_weights_array;   /* Array #8 */
AI_STATIC ai_array node_10_bias_array;   /* Array #9 */
AI_STATIC ai_array node_14_weights_array;   /* Array #10 */
AI_STATIC ai_array node_14_bias_array;   /* Array #11 */
AI_STATIC ai_array node_18_weights_array;   /* Array #12 */
AI_STATIC ai_array node_18_bias_array;   /* Array #13 */
AI_STATIC ai_array node_22_weights_array;   /* Array #14 */
AI_STATIC ai_array node_22_bias_array;   /* Array #15 */


/**  Forward network tensor declarations  *************************************/
AI_STATIC ai_tensor image_output;   /* Tensor #0 */
AI_STATIC ai_tensor node_10_output;   /* Tensor #1 */
AI_STATIC ai_tensor node_12_output;   /* Tensor #2 */
AI_STATIC ai_tensor node_14_output;   /* Tensor #3 */
AI_STATIC ai_tensor node_16_output;   /* Tensor #4 */
AI_STATIC ai_tensor node_18_output;   /* Tensor #5 */
AI_STATIC ai_tensor node_20_output;   /* Tensor #6 */
AI_STATIC ai_tensor node_22_output;   /* Tensor #7 */
AI_STATIC ai_tensor node_10_weights;   /* Tensor #8 */
AI_STATIC ai_tensor node_10_bias;   /* Tensor #9 */
AI_STATIC ai_tensor node_14_weights;   /* Tensor #10 */
AI_STATIC ai_tensor node_14_bias;   /* Tensor #11 */
AI_STATIC ai_tensor node_18_weights;   /* Tensor #12 */
AI_STATIC ai_tensor node_18_bias;   /* Tensor #13 */
AI_STATIC ai_tensor node_22_weights;   /* Tensor #14 */
AI_STATIC ai_tensor node_22_bias;   /* Tensor #15 */


/**  Forward network tensor chain declarations  *******************************/
AI_STATIC_CONST ai_tensor_chain node_10_chain;   /* Chain #0 */
AI_STATIC_CONST ai_tensor_chain node_12_chain;   /* Chain #1 */
AI_STATIC_CONST ai_tensor_chain node_14_chain;   /* Chain #2 */
AI_STATIC_CONST ai_tensor_chain node_16_chain;   /* Chain #3 */
AI_STATIC_CONST ai_tensor_chain node_18_chain;   /* Chain #4 */
AI_STATIC_CONST ai_tensor_chain node_20_chain;   /* Chain #5 */
AI_STATIC_CONST ai_tensor_chain node_22_chain;   /* Chain #6 */


/**  Forward network layer declarations  **************************************/
AI_STATIC ai_layer_dense node_10_layer; /* Layer #0 */
AI_STATIC ai_layer_nl node_12_layer; /* Layer #1 */
AI_STATIC ai_layer_dense node_14_layer; /* Layer #2 */
AI_STATIC ai_layer_nl node_16_layer; /* Layer #3 */
AI_STATIC ai_layer_dense node_18_layer; /* Layer #4 */
AI_STATIC ai_layer_nl node_20_layer; /* Layer #5 */
AI_STATIC ai_layer_dense node_22_layer; /* Layer #6 */




/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  image_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 1024, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  node_10_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  node_12_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  node_14_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  node_16_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  node_18_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  node_20_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  node_22_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 4, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  node_10_weights_array, AI_ARRAY_FORMAT_LUT8_FLOAT,
  NULL, NULL, 131072, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  node_10_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  node_14_weights_array, AI_ARRAY_FORMAT_LUT8_FLOAT,
  NULL, NULL, 16384, AI_STATIC)

/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  node_14_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#12 */
AI_ARRAY_OBJ_DECLARE(
  node_18_weights_array, AI_ARRAY_FORMAT_LUT8_FLOAT,
  NULL, NULL, 16384, AI_STATIC)

/* Array#13 */
AI_ARRAY_OBJ_DECLARE(
  node_18_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#14 */
AI_ARRAY_OBJ_DECLARE(
  node_22_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 512, AI_STATIC)

/* Array#15 */
AI_ARRAY_OBJ_DECLARE(
  node_22_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 4, AI_STATIC)

/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  image_output, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 1024, 1, 1), AI_STRIDE_INIT(4, 4, 4, 4096, 4096),
  1, &image_output_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  node_10_output, AI_STATIC,
  1, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_10_output_array, NULL)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  node_12_output, AI_STATIC,
  2, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_12_output_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  node_14_output, AI_STATIC,
  3, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_14_output_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  node_16_output, AI_STATIC,
  4, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_16_output_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  node_18_output, AI_STATIC,
  5, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_18_output_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  node_20_output, AI_STATIC,
  6, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_20_output_array, NULL)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  node_22_output, AI_STATIC,
  7, 0x0,
  AI_SHAPE_INIT(4, 1, 4, 1, 1), AI_STRIDE_INIT(4, 4, 4, 16, 16),
  1, &node_22_output_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  node_10_weights, AI_STATIC,
  8, 0x0,
  AI_SHAPE_INIT(4, 1024, 128, 1, 1), AI_STRIDE_INIT(4, 1, 1024, 131072, 131072),
  1, &node_10_weights_array, NULL)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  node_10_bias, AI_STATIC,
  9, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_10_bias_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  node_14_weights, AI_STATIC,
  10, 0x0,
  AI_SHAPE_INIT(4, 128, 128, 1, 1), AI_STRIDE_INIT(4, 1, 128, 16384, 16384),
  1, &node_14_weights_array, NULL)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  node_14_bias, AI_STATIC,
  11, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_14_bias_array, NULL)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  node_18_weights, AI_STATIC,
  12, 0x0,
  AI_SHAPE_INIT(4, 128, 128, 1, 1), AI_STRIDE_INIT(4, 1, 128, 16384, 16384),
  1, &node_18_weights_array, NULL)

/* Tensor #13 */
AI_TENSOR_OBJ_DECLARE(
  node_18_bias, AI_STATIC,
  13, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &node_18_bias_array, NULL)

/* Tensor #14 */
AI_TENSOR_OBJ_DECLARE(
  node_22_weights, AI_STATIC,
  14, 0x0,
  AI_SHAPE_INIT(4, 128, 4, 1, 1), AI_STRIDE_INIT(4, 4, 512, 2048, 2048),
  1, &node_22_weights_array, NULL)

/* Tensor #15 */
AI_TENSOR_OBJ_DECLARE(
  node_22_bias, AI_STATIC,
  15, 0x0,
  AI_SHAPE_INIT(4, 1, 4, 1, 1), AI_STRIDE_INIT(4, 4, 4, 16, 16),
  1, &node_22_bias_array, NULL)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_10_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &image_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_10_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &node_10_weights, &node_10_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_10_layer, 1,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &node_10_chain,
  &AI_NET_OBJ_INSTANCE, &node_12_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_float node_12_nl_params_data[] = { 0.0, 0.009999999776482582, -1 };
AI_ARRAY_OBJ_DECLARE(
    node_12_nl_params, AI_ARRAY_FORMAT_FLOAT,
    node_12_nl_params_data, node_12_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_12_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_10_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_12_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_12_layer, 2,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &node_12_chain,
  &AI_NET_OBJ_INSTANCE, &node_14_layer, AI_STATIC, 
  .nl_params = &node_12_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_14_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_12_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_14_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &node_14_weights, &node_14_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_14_layer, 4,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &node_14_chain,
  &AI_NET_OBJ_INSTANCE, &node_16_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_float node_16_nl_params_data[] = { 0.0, 0.009999999776482582, -1 };
AI_ARRAY_OBJ_DECLARE(
    node_16_nl_params, AI_ARRAY_FORMAT_FLOAT,
    node_16_nl_params_data, node_16_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_16_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_14_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_16_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_16_layer, 5,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &node_16_chain,
  &AI_NET_OBJ_INSTANCE, &node_18_layer, AI_STATIC, 
  .nl_params = &node_16_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_18_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_16_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_18_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &node_18_weights, &node_18_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_18_layer, 7,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &node_18_chain,
  &AI_NET_OBJ_INSTANCE, &node_20_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_float node_20_nl_params_data[] = { 0.0, 0.009999999776482582, -1 };
AI_ARRAY_OBJ_DECLARE(
    node_20_nl_params, AI_ARRAY_FORMAT_FLOAT,
    node_20_nl_params_data, node_20_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_20_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_18_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_20_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_20_layer, 8,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &node_20_chain,
  &AI_NET_OBJ_INSTANCE, &node_22_layer, AI_STATIC, 
  .nl_params = &node_20_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_22_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_20_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_22_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &node_22_weights, &node_22_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_22_layer, 10,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &node_22_chain,
  &AI_NET_OBJ_INSTANCE, &node_22_layer, AI_STATIC, 
)


AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 170512, 1,
                     NULL),
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 1024, 1,
                     NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_SIMPLENN_IN_NUM, &image_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_SIMPLENN_OUT_NUM, &node_22_output),
  &node_10_layer, 0, NULL)



AI_DECLARE_STATIC
ai_bool simplenn_configure_activations(
  ai_network* net_ctx, const ai_buffer* activation_buffer)
{
  AI_ASSERT(net_ctx &&  activation_buffer && activation_buffer->data)

  ai_ptr activations = AI_PTR(AI_PTR_ALIGN(activation_buffer->data, 4));
  AI_ASSERT(activations)
  AI_UNUSED(net_ctx)

  {
    /* Updating activations (byte) offsets */
    image_output_array.data = AI_PTR(NULL);
    image_output_array.data_start = AI_PTR(NULL);
    node_10_output_array.data = AI_PTR(activations + 0);
    node_10_output_array.data_start = AI_PTR(activations + 0);
    node_12_output_array.data = AI_PTR(activations + 0);
    node_12_output_array.data_start = AI_PTR(activations + 0);
    node_14_output_array.data = AI_PTR(activations + 512);
    node_14_output_array.data_start = AI_PTR(activations + 512);
    node_16_output_array.data = AI_PTR(activations + 0);
    node_16_output_array.data_start = AI_PTR(activations + 0);
    node_18_output_array.data = AI_PTR(activations + 512);
    node_18_output_array.data_start = AI_PTR(activations + 512);
    node_20_output_array.data = AI_PTR(activations + 0);
    node_20_output_array.data_start = AI_PTR(activations + 0);
    node_22_output_array.data = AI_PTR(NULL);
    node_22_output_array.data_start = AI_PTR(NULL);
    
  }
  return true;
}



AI_DECLARE_STATIC
ai_bool simplenn_configure_weights(
  ai_network* net_ctx, const ai_buffer* weights_buffer)
{
  AI_ASSERT(net_ctx &&  weights_buffer && weights_buffer->data)

  ai_ptr weights = AI_PTR(weights_buffer->data);
  AI_ASSERT(weights)
  AI_UNUSED(net_ctx)

  {
    /* Updating weights (byte) offsets */
    
    node_10_weights_array.format |= AI_FMT_FLAG_CONST;
    node_10_weights_array.data = AI_PTR(weights + 1024);
    node_10_weights_array.data_start = AI_PTR(weights + 0);
    node_10_bias_array.format |= AI_FMT_FLAG_CONST;
    node_10_bias_array.data = AI_PTR(weights + 132096);
    node_10_bias_array.data_start = AI_PTR(weights + 132096);
    node_14_weights_array.format |= AI_FMT_FLAG_CONST;
    node_14_weights_array.data = AI_PTR(weights + 133632);
    node_14_weights_array.data_start = AI_PTR(weights + 132608);
    node_14_bias_array.format |= AI_FMT_FLAG_CONST;
    node_14_bias_array.data = AI_PTR(weights + 150016);
    node_14_bias_array.data_start = AI_PTR(weights + 150016);
    node_18_weights_array.format |= AI_FMT_FLAG_CONST;
    node_18_weights_array.data = AI_PTR(weights + 151552);
    node_18_weights_array.data_start = AI_PTR(weights + 150528);
    node_18_bias_array.format |= AI_FMT_FLAG_CONST;
    node_18_bias_array.data = AI_PTR(weights + 167936);
    node_18_bias_array.data_start = AI_PTR(weights + 167936);
    node_22_weights_array.format |= AI_FMT_FLAG_CONST;
    node_22_weights_array.data = AI_PTR(weights + 168448);
    node_22_weights_array.data_start = AI_PTR(weights + 168448);
    node_22_bias_array.format |= AI_FMT_FLAG_CONST;
    node_22_bias_array.data = AI_PTR(weights + 170496);
    node_22_bias_array.data_start = AI_PTR(weights + 170496);
  }

  return true;
}


/**  PUBLIC APIs SECTION  *****************************************************/

AI_API_ENTRY
ai_bool ai_simplenn_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if ( report && net_ctx )
  {
    ai_network_report r = {
      .model_name        = AI_SIMPLENN_MODEL_NAME,
      .model_signature   = AI_SIMPLENN_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 165892,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .activations       = AI_STRUCT_INIT,
      .params            = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x0,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }

  return false;
}

AI_API_ENTRY
ai_error ai_simplenn_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}

AI_API_ENTRY
ai_error ai_simplenn_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    &AI_NET_OBJ_INSTANCE,
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}

AI_API_ENTRY
ai_handle ai_simplenn_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}

AI_API_ENTRY
ai_bool ai_simplenn_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = ai_platform_network_init(network, params);
  if ( !net_ctx ) return false;

  ai_bool ok = true;
  ok &= simplenn_configure_weights(net_ctx, &params->params);
  ok &= simplenn_configure_activations(net_ctx, &params->activations);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_simplenn_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}

AI_API_ENTRY
ai_i32 ai_simplenn_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}



#undef AI_SIMPLENN_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

