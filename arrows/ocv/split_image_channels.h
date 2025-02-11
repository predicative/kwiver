/*ckwg +29
 * Copyright 2017-2019 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief Header for OCV split_image_channels algorithm
 */

#ifndef KWIVER_ARROWS_OCV_SPLIT_IMAGE_CHANNELS_H_
#define KWIVER_ARROWS_OCV_SPLIT_IMAGE_CHANNELS_H_

#include <arrows/ocv/kwiver_algo_ocv_export.h>

#include <vital/algo/split_image.h>

namespace kwiver {
namespace arrows {
namespace ocv {

/// Split an image into multiple channel images, 1 per channel
class KWIVER_ALGO_OCV_EXPORT split_image_channels
  : public vital::algorithm_impl< split_image_channels, vital::algo::split_image >
{
public:
  PLUGIN_INFO( "ocv_channels",
               "Split an image into multiple channel images (also known as planes)" )

  /// Constructor
  split_image_channels();

  /// Destructor
  virtual ~split_image_channels();

  virtual void set_configuration( kwiver::vital::config_block_sptr ) { }
  virtual bool check_configuration( kwiver::vital::config_block_sptr config ) const { return true; }

  /// Split image
  virtual std::vector< kwiver::vital::image_container_sptr >
  split( kwiver::vital::image_container_sptr img ) const;
};

} // end namespace ocv
} // end namespace arrows
} // end namespace kwiver

#endif // KWIVER_ARROWS_OCV_SPLIT_IMAGE_CHANNELS_H_
