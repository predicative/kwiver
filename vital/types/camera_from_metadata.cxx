/*ckwg +29
 * Copyright 2018 by Kitware, Inc.
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
 * \brief Implementation of file IO functions for a \ref kwiver::vital::camera
 *
 * File format is the KRTD file.
 */

#include "camera_from_metadata.h"

#include <vital/types/metadata_traits.h>

namespace kwiver {
namespace vital {

/// Extract scale or offset metadata to a vector

Eigen::VectorXd
tags_to_vector( metadata_sptr const& md, std::vector<vital_metadata_tag> tags )
{
  metadata_traits md_traits;
  auto vec_length = tags.size();

  Eigen::VectorXd rslt(vec_length);

  for (int i=0; i<vec_length; ++i)
  {
   if (md->has(tags[i]))
    {
      rslt[0] = md->find(tags[i]).as_double();
    }
    else
    {
      throw metadata_exception("Missing RPC metadata: " +
                               md_traits.tag_to_name(tags[i]));
    }
  }

  return rslt;
}

Eigen::Matrix< double, 4, 20 >
tags_to_matrix( metadata_sptr const& md, std::vector<vital_metadata_tag> tags )
{
  metadata_traits md_traits;
  if (tags.size() != 4)
  {
    throw metadata_exception("Should have 4 metadata tags for RPC coefficients");
  }

  // Eigen::Matrix< double, 4, 20 > rslt;
  matrix_d rslt;

  for (int i=0; i<4; ++i)
  {
   if (md->has(tags[i]))
    {
      matrix_d currRow;
      md->find(tags[i]).data(currRow);
      rslt.row(i) = currRow;
    }
    else
    {
      throw metadata_exception("Missing RPC metadata: " +
                               md_traits.tag_to_name(tags[i]));
    }
  }

  return rslt;
}

/// Produce RPC camera from metadata
camera_rpc_sptr
VITAL_EXPORT camera_from_metadata( metadata_sptr const& md )
{
  vector_3d world_scale, world_offset;
  vector_2d image_scale, image_offset;
  Eigen::Matrix< double, 4, 20 > rpc_coeffs;

  metadata_traits md_traits;

  std::vector<vital_metadata_tag> world_scale_tags = {
    vital::VITAL_META_RPC_LONG_SCALE,
    vital::VITAL_META_RPC_LAT_SCALE,
    vital::VITAL_META_RPC_HEIGHT_SCALE
  };
  world_scale = tags_to_vector(md, world_scale_tags);

  std::vector<vital_metadata_tag> world_offset_tags = {
    vital::VITAL_META_RPC_LONG_OFFSET,
    vital::VITAL_META_RPC_LAT_OFFSET,
    vital::VITAL_META_RPC_HEIGHT_OFFSET
  };
  world_offset = tags_to_vector(md, world_offset_tags);

  std::vector<vital_metadata_tag> image_scale_tags = {
    vital::VITAL_META_RPC_ROW_SCALE,
    vital::VITAL_META_RPC_COL_SCALE
  };
  image_scale = tags_to_vector(md, image_scale_tags);

  std::vector<vital_metadata_tag> image_offset_tags = {
    vital::VITAL_META_RPC_ROW_OFFSET,
    vital::VITAL_META_RPC_COL_OFFSET
  };
  image_offset = tags_to_vector(md, image_offset_tags);

  std::vector<vital_metadata_tag> rpc_coeffs_tags = {
    vital::VITAL_META_RPC_ROW_NUM_COEFF,
    vital::VITAL_META_RPC_ROW_DEN_COEFF,
    vital::VITAL_META_RPC_COL_NUM_COEFF,
    vital::VITAL_META_RPC_COL_DEN_COEFF
  };
  rpc_coeffs = tags_to_matrix(md, rpc_coeffs_tags);

  simple_camera_rpc* cam = new simple_camera_rpc(world_scale, world_offset,
                                                 image_scale, image_offset,
                                                 rpc_coeffs);
  return camera_rpc_sptr(cam);
}

} } // end of namespace
