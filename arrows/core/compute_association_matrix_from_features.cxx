// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Implementation of compute_association_matrix_from_features

#include "compute_association_matrix_from_features.h"

#include <vital/algo/detected_object_filter.h>
#include <vital/types/object_track_set.h>
#include <vital/exceptions/algorithm.h>
#include <vital/vital_config.h>

#include <string>
#include <vector>
#include <atomic>
#include <algorithm>

namespace kwiver {
namespace arrows {
namespace core {

using namespace kwiver::vital;

/// Private implementation class
class compute_association_matrix_from_features::priv
{
public:
  /// Constructor
  priv()
    : m_max_distance( -1.0 )
    , m_logger( vital::get_logger( "compute_association_matrix_from_features" ) )
  {
  }

  /// Raw pixel distance threshold
  double m_max_distance;

  /// The feature matching algorithm to use
  vital::algo::detected_object_filter_sptr m_filter;

  /// Logger handle
  vital::logger_handle_t m_logger;
};

/// Constructor
compute_association_matrix_from_features
::compute_association_matrix_from_features()
  : d_( new priv )
{
}

/// Destructor
compute_association_matrix_from_features
::~compute_association_matrix_from_features() noexcept
{
}

/// Get this alg's \link vital::config_block configuration block \endlink
vital::config_block_sptr
compute_association_matrix_from_features
::get_configuration() const
{
  // get base config from base class
  vital::config_block_sptr config = algorithm::get_configuration();

  // Maximum allowed pixel distance for matches
  config->set_value( "max_distance", d_->m_max_distance,
    "Maximum allowed pixel distance for matches. Is expressed "
    "in raw pixel distance." );

  // Sub-algorithm implementation name + sub_config block
  // - Feature filter algorithm
  algo::detected_object_filter::get_nested_algo_configuration(
    "filter", config, d_->m_filter );

  return config;
}

/// Set this algo's properties via a config block
void
compute_association_matrix_from_features
::set_configuration( vital::config_block_sptr in_config )
{
  vital::config_block_sptr config = this->get_configuration();
  config->merge_config( in_config );

  algo::detected_object_filter::set_nested_algo_configuration( "filter",
    config, d_->m_filter );

  d_->m_max_distance = config->get_value< double >( "max_distance" );
}

bool
compute_association_matrix_from_features
::check_configuration( vital::config_block_sptr config ) const
{
  return (
    algo::detected_object_filter::check_nested_algo_configuration( "filter", config )
  );
}

/// Compute an association matrix given detections and tracks
bool
compute_association_matrix_from_features
::compute( VITAL_UNUSED kwiver::vital::timestamp ts,
           VITAL_UNUSED kwiver::vital::image_container_sptr image,
           kwiver::vital::object_track_set_sptr tracks,
           kwiver::vital::detected_object_set_sptr detections,
           kwiver::vital::matrix_d& matrix,
           kwiver::vital::detected_object_set_sptr& considered ) const
{
  considered = d_->m_filter->filter( detections );

  auto filtered_dets = considered;
  auto filtered_tracks = tracks->tracks();

  const double invalid_value = std::numeric_limits< double >::max();

  if( filtered_tracks.empty() || filtered_dets->empty() )
  {
    matrix = kwiver::vital::matrix_d();
  }
  else
  {
    matrix = kwiver::vital::matrix_d( filtered_tracks.size(), filtered_dets->size() );

    for( unsigned t = 0; t < filtered_tracks.size(); ++t )
    {
      for( unsigned d = 0; d < filtered_dets->size(); ++d )
      {
        track_sptr trk = filtered_tracks[t];
        detected_object_sptr det = filtered_dets->at(d);

        auto det_features = det->descriptor();
        auto trk_features = decltype(det_features){};

        if( !trk->empty() )
        {
          object_track_state* trk_state =
            dynamic_cast< object_track_state* >( trk->back().get() );

          if( trk_state->detection() )
          {
            double dist = d_->m_max_distance;

            if( d_->m_max_distance > 0.0 )
            {
              auto center1 = trk_state->detection()->bounding_box().center();
              auto center2 = det->bounding_box().center();

              dist = ( center1[0] - center2[0] ) * ( center1[0] - center2[0] );
              dist += ( ( center1[1] - center2[1] ) * ( center1[1] - center2[1] ) );
              dist = std::sqrt( dist );
            }

            if( d_->m_max_distance <= 0.0 || dist < d_->m_max_distance )
            {
              trk_features = trk_state->detection()->descriptor();
            }
          }
        }

        if( det_features && trk_features )
        {
          if( det_features->size() != trk_features->size() )
          {
            throw std::runtime_error( "Invalid descriptor dimensions" );
          }

          double sum_sqr = 0.0;

          for( auto pos1 = det_features->raw_data(),
                    pos2 = trk_features->raw_data(),
                    end = pos1 + det_features->size();
               pos1 != end; ++pos1, ++pos2 )
          {
            sum_sqr += ( ( *pos1 - *pos2 ) * ( *pos1 - *pos2 ) );
          }

          matrix( t, d ) = std::sqrt( sum_sqr );
        }
        else
        {
          matrix( t, d ) = invalid_value;
        }
      }
    }
  }

  return ( matrix.size() > 0 );
}

} // end namespace core
} // end namespace arrows
} // end namespace kwiver
