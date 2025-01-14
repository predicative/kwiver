// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "image_object_detector_process.h"

#include <vital/algo/image_object_detector.h>

#include <sprokit/processes/kwiver_type_traits.h>
#include <sprokit/pipeline/process_exception.h>

namespace kwiver {


create_config_trait( frame_downsample, unsigned, "0",
  "If non-zero, only process every 1 in these frames" );
create_config_trait( frame_offset, unsigned, "0",
  "Frame downsampling offset factor, if enabled" );

create_algorithm_name_config_trait( detector );


// -----------------------------------------------------------------------------
// Private implementation class
class image_object_detector_process::priv
{
public:
  priv();
  ~priv();

  unsigned frame_downsample;
  unsigned frame_offset;
  unsigned frame_counter;

  vital::algo::image_object_detector_sptr m_detector;

}; // end priv class


// =============================================================================
image_object_detector_process
::image_object_detector_process( kwiver::vital::config_block_sptr const& config )
  : process( config ),
    d( new image_object_detector_process::priv )
{
  make_ports();
  make_config();
}


// -----------------------------------------------------------------------------
image_object_detector_process
::~image_object_detector_process()
{
}


// -----------------------------------------------------------------------------
void
image_object_detector_process
::_configure()
{
  scoped_configure_instrumentation();

  vital::config_block_sptr algo_config = get_config();

  // Check config so it will give run-time diagnostic of config problems

  if( !vital::algo::image_object_detector::check_nested_algo_configuration_using_trait(
         detector,
         algo_config ) )
  {
    VITAL_THROW( sprokit::invalid_configuration_exception, name(),
                 "Configuration check failed." );
  }

  vital::algo::image_object_detector::set_nested_algo_configuration_using_trait(
    detector,
    algo_config,
    d->m_detector );

  if( !d->m_detector )
  {
    VITAL_THROW( sprokit::invalid_configuration_exception, name(),
                 "Unable to create detector" );
  }

  d->frame_downsample = config_value_using_trait( frame_downsample );
  d->frame_offset = config_value_using_trait( frame_offset );

  d->frame_counter = 0;
}


// -----------------------------------------------------------------------------
void
image_object_detector_process
::_step()
{
  vital::image_container_sptr input = grab_from_port_using_trait( image );

  vital::detected_object_set_sptr result;

  if( d->frame_downsample > 0 &&
     ( d->frame_counter++ + d->frame_offset ) % d->frame_downsample != 0 )
  {
    result = std::make_shared< vital::detected_object_set >();
  }
  else
  {
    scoped_step_instrumentation();

    // Get detections from detector on image
    if( input )
    {
      result = d->m_detector->detect( input );
    }
  }

  push_to_port_using_trait( detected_object_set, result );
}


// -----------------------------------------------------------------------------
void
image_object_detector_process
::make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t required;
  sprokit::process::port_flags_t optional;

  required.insert( flag_required );

  // -- input --
  declare_input_port_using_trait( image, required );

  // -- output --
  declare_output_port_using_trait( detected_object_set, optional );
}


// -----------------------------------------------------------------------------
void
image_object_detector_process
::make_config()
{
  declare_config_using_trait( detector );
  declare_config_using_trait( frame_downsample );
  declare_config_using_trait( frame_offset );
}


// =============================================================================
image_object_detector_process::priv
::priv()
 : frame_downsample( 0 )
 , frame_offset( 0 )
 , frame_counter( 0 )
{
}

image_object_detector_process::priv
::~priv()
{
}

} // end namespace kwiver
