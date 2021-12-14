/**
  ******************************************************************************
  * @file    simplenn.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Thu Dec  9 16:01:28 2021
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
#define AI_SIMPLENN_MODEL_SIGNATURE     "ab098fad95d8aaf84ceabca81e3650c7"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "Thu Dec  9 16:01:28 2021"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_SIMPLENN_N_BATCHES
#define AI_SIMPLENN_N_BATCHES         (1)

/**  Forward network declaration section  *************************************/
AI_STATIC ai_network AI_NET_OBJ_INSTANCE;


/**  Forward network array declarations  **************************************/
AI_STATIC ai_array image_output_array;   /* Array #0 */
AI_STATIC ai_array node_9_output_array;   /* Array #1 */
AI_STATIC ai_array node_10_output_array;   /* Array #2 */
AI_STATIC ai_array node_11_output_array;   /* Array #3 */
AI_STATIC ai_array node_12_output_array;   /* Array #4 */
AI_STATIC ai_array action_output_array;   /* Array #5 */
AI_STATIC ai_array dense3bias_array;   /* Array #6 */
AI_STATIC ai_array dense3weight_array;   /* Array #7 */
AI_STATIC ai_array dense2bias_array;   /* Array #8 */
AI_STATIC ai_array dense2weight_array;   /* Array #9 */
AI_STATIC ai_array dense1bias_array;   /* Array #10 */
AI_STATIC ai_array dense1weight_array;   /* Array #11 */


/**  Forward network tensor declarations  *************************************/
AI_STATIC ai_tensor image_output;   /* Tensor #0 */
AI_STATIC ai_tensor image_output0;   /* Tensor #1 */
AI_STATIC ai_tensor node_9_output;   /* Tensor #2 */
AI_STATIC ai_tensor node_10_output;   /* Tensor #3 */
AI_STATIC ai_tensor node_11_output;   /* Tensor #4 */
AI_STATIC ai_tensor node_12_output;   /* Tensor #5 */
AI_STATIC ai_tensor action_output;   /* Tensor #6 */
AI_STATIC ai_tensor dense3bias;   /* Tensor #7 */
AI_STATIC ai_tensor dense3weight;   /* Tensor #8 */
AI_STATIC ai_tensor dense2bias;   /* Tensor #9 */
AI_STATIC ai_tensor dense2weight;   /* Tensor #10 */
AI_STATIC ai_tensor dense1bias;   /* Tensor #11 */
AI_STATIC ai_tensor dense1weight;   /* Tensor #12 */


/**  Forward network tensor chain declarations  *******************************/
AI_STATIC_CONST ai_tensor_chain node_9_chain;   /* Chain #0 */
AI_STATIC_CONST ai_tensor_chain node_10_chain;   /* Chain #1 */
AI_STATIC_CONST ai_tensor_chain node_11_chain;   /* Chain #2 */
AI_STATIC_CONST ai_tensor_chain node_12_chain;   /* Chain #3 */
AI_STATIC_CONST ai_tensor_chain action_chain;   /* Chain #4 */


/**  Forward network layer declarations  **************************************/
AI_STATIC ai_layer_gemm node_9_layer; /* Layer #0 */
AI_STATIC ai_layer_nl node_10_layer; /* Layer #1 */
AI_STATIC ai_layer_gemm node_11_layer; /* Layer #2 */
AI_STATIC ai_layer_nl node_12_layer; /* Layer #3 */
AI_STATIC ai_layer_gemm action_layer; /* Layer #4 */




/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  image_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 9216, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  node_9_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 6, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  node_10_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 6, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  node_11_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  node_12_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  action_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 4, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  dense3bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 4, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  dense3weight_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  dense2bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 32, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  dense2weight_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 192, AI_STATIC)

/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  dense1bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 6, AI_STATIC)

/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  dense1weight_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 55296, AI_STATIC)

/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  image_output, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 1, 1, 9216), AI_STRIDE_INIT(4, 4, 4, 4, 4),
  1, &image_output_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  image_output0, AI_STATIC,
  1, 0x0,
  AI_SHAPE_INIT(4, 1, 9216, 1, 1), AI_STRIDE_INIT(4, 4, 4, 36864, 36864),
  1, &image_output_array, NULL)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  node_9_output, AI_STATIC,
  2, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 4, 4, 24, 24),
  1, &node_9_output_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  node_10_output, AI_STATIC,
  3, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 4, 4, 24, 24),
  1, &node_10_output_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  node_11_output, AI_STATIC,
  4, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &node_11_output_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  node_12_output, AI_STATIC,
  5, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &node_12_output_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  action_output, AI_STATIC,
  6, 0x0,
  AI_SHAPE_INIT(4, 1, 4, 1, 1), AI_STRIDE_INIT(4, 4, 4, 16, 16),
  1, &action_output_array, NULL)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  dense3bias, AI_STATIC,
  7, 0x0,
  AI_SHAPE_INIT(4, 1, 4, 1, 1), AI_STRIDE_INIT(4, 4, 4, 16, 16),
  1, &dense3bias_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  dense3weight, AI_STATIC,
  8, 0x0,
  AI_SHAPE_INIT(4, 1, 4, 1, 32), AI_STRIDE_INIT(4, 4, 4, 16, 16),
  1, &dense3weight_array, NULL)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  dense2bias, AI_STATIC,
  9, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &dense2bias_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  dense2weight, AI_STATIC,
  10, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 6), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &dense2weight_array, NULL)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  dense1bias, AI_STATIC,
  11, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 1), AI_STRIDE_INIT(4, 4, 4, 24, 24),
  1, &dense1bias_array, NULL)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  dense1weight, AI_STATIC,
  12, 0x0,
  AI_SHAPE_INIT(4, 1, 6, 1, 9216), AI_STRIDE_INIT(4, 4, 4, 24, 24),
  1, &dense1weight_array, NULL)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_9_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &image_output0, &dense1weight, &dense1bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_9_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_9_layer, 1,
  GEMM_TYPE, 0x0, NULL,
  gemm, forward_gemm,
  &node_9_chain,
  &AI_NET_OBJ_INSTANCE, &node_10_layer, AI_STATIC, 
  .alpha = 1.0, 
  .beta = 1.0, 
  .tA = 1, 
  .tB = 1, 
)


AI_STATIC_CONST ai_float node_10_nl_params_data[] = { 0.0, 0.009999999776482582, -1 };
AI_ARRAY_OBJ_DECLARE(
    node_10_nl_params, AI_ARRAY_FORMAT_FLOAT,
    node_10_nl_params_data, node_10_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_10_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_9_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_10_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_10_layer, 2,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &node_10_chain,
  &AI_NET_OBJ_INSTANCE, &node_11_layer, AI_STATIC, 
  .nl_params = &node_10_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_11_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &node_10_output, &dense2weight, &dense2bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_11_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_11_layer, 3,
  GEMM_TYPE, 0x0, NULL,
  gemm, forward_gemm,
  &node_11_chain,
  &AI_NET_OBJ_INSTANCE, &node_12_layer, AI_STATIC, 
  .alpha = 1.0, 
  .beta = 1.0, 
  .tA = 1, 
  .tB = 1, 
)


AI_STATIC_CONST ai_float node_12_nl_params_data[] = { 0.0, 0.009999999776482582, -1 };
AI_ARRAY_OBJ_DECLARE(
    node_12_nl_params, AI_ARRAY_FORMAT_FLOAT,
    node_12_nl_params_data, node_12_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  node_12_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_11_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &node_12_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  node_12_layer, 4,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &node_12_chain,
  &AI_NET_OBJ_INSTANCE, &action_layer, AI_STATIC, 
  .nl_params = &node_12_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  action_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &node_12_output, &dense3weight, &dense3bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &action_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  action_layer, 5,
  GEMM_TYPE, 0x0, NULL,
  gemm, forward_gemm,
  &action_chain,
  &AI_NET_OBJ_INSTANCE, &action_layer, AI_STATIC, 
  .alpha = 1.0, 
  .beta = 1.0, 
  .tA = 1, 
  .tB = 1, 
)


AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 222632, 1,
                     NULL),
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 152, 1,
                     NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_SIMPLENN_IN_NUM, &image_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_SIMPLENN_OUT_NUM, &action_output),
  &node_9_layer, 0, NULL)



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
    node_9_output_array.data = AI_PTR(activations + 0);
    node_9_output_array.data_start = AI_PTR(activations + 0);
    node_10_output_array.data = AI_PTR(activations + 0);
    node_10_output_array.data_start = AI_PTR(activations + 0);
    node_11_output_array.data = AI_PTR(activations + 24);
    node_11_output_array.data_start = AI_PTR(activations + 24);
    node_12_output_array.data = AI_PTR(activations + 24);
    node_12_output_array.data_start = AI_PTR(activations + 24);
    action_output_array.data = AI_PTR(NULL);
    action_output_array.data_start = AI_PTR(NULL);
    
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
    
    dense3bias_array.format |= AI_FMT_FLAG_CONST;
    dense3bias_array.data = AI_PTR(weights + 0);
    dense3bias_array.data_start = AI_PTR(weights + 0);
    dense3weight_array.format |= AI_FMT_FLAG_CONST;
    dense3weight_array.data = AI_PTR(weights + 16);
    dense3weight_array.data_start = AI_PTR(weights + 16);
    dense2bias_array.format |= AI_FMT_FLAG_CONST;
    dense2bias_array.data = AI_PTR(weights + 528);
    dense2bias_array.data_start = AI_PTR(weights + 528);
    dense2weight_array.format |= AI_FMT_FLAG_CONST;
    dense2weight_array.data = AI_PTR(weights + 656);
    dense2weight_array.data_start = AI_PTR(weights + 656);
    dense1bias_array.format |= AI_FMT_FLAG_CONST;
    dense1bias_array.data = AI_PTR(weights + 1424);
    dense1bias_array.data_start = AI_PTR(weights + 1424);
    dense1weight_array.format |= AI_FMT_FLAG_CONST;
    dense1weight_array.data = AI_PTR(weights + 1448);
    dense1weight_array.data_start = AI_PTR(weights + 1448);
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
      
      .n_macc            = 55730,
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

