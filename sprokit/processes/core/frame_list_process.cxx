// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include "frame_list_process.h"

#include <vital/vital_types.h>
#include <vital/types/timestamp.h>
#include <vital/types/image_container.h>
#include <vital/types/image.h>
#include <vital/algo/image_io.h>
#include <vital/exceptions.h>
#include <vital/util/data_stream_reader.h>
#include <vital/util/tokenize.h>

#include <kwiver_type_traits.h>

#include <sprokit/pipeline/process_exception.h>
#include <sprokit/pipeline/datum.h>

#include <kwiversys/SystemTools.hxx>

#include <vector>
#include <stdint.h>
#include <fstream>

// -- DEBUG
#if defined DEBUG
#include <arrows/algorithms/ocv/image_container.h>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
#endif

//+ TODO this process is obsoleted by the image_list_reader
// implementation of the video_input algorithm

namespace algo = kwiver::vital::algo;

namespace kwiver {

// (config-key, value-type, default-value, description )
create_config_trait( image_list_file, std::string, "",
  "Name of file that contains list of image file names.\n\n"
  "Each line in the file specifies the name of a single image file.");

create_config_trait( path, std::string, "",
  "Path to search for image file.\n\n"
  " The format is the same as the standard "
  "path specification, a set of directories separated by a colon (':')" );

create_config_trait( frame_time, double, "0.03333333", "Inter frame time in seconds.\n\n "
  "The generated timestamps will have the specified number of seconds in the generated "
  "timestamps for sequential frames. This can be used to simulate a frame rate in a "
  "video stream application.");

create_config_trait( zero_based_id, bool, "true",
  "Should the first frame be labeled with frame ID 0 instead of frame 1." );

create_config_trait( no_path_in_name, bool, "true",
  "Set to true if the output image file path should not contain a full path to"
  "the image file and just contain the file name for the image." );

create_config_trait( disable_file_load, bool, "false",
  "Debug option to disable file loading and just pass the filename downstream." );

create_algorithm_name_config_trait( image_reader );

// -----------------------------------------------------------------------------
// Private implementation class
class frame_list_process::priv
{
public:
  priv();
  ~priv();

  // Configuration values
  std::string m_config_image_list_filename;
  kwiver::vital::time_usec_t m_config_frame_time;
  std::vector< std::string > m_config_path;

  // process local data
  std::vector < kwiver::vital::path_t > m_files;
  std::vector < kwiver::vital::path_t >::const_iterator m_current_file;
  kwiver::vital::frame_id_t m_frame_number;
  kwiver::vital::time_usec_t m_frame_time;
  bool m_zero_based_id;
  bool m_no_path_in_name;
  bool m_disable_file_load;

  // processing classes
  algo::image_io_sptr m_image_reader;

}; // end priv class


// =============================================================================
frame_list_process
::frame_list_process( kwiver::vital::config_block_sptr const& config )
  : process( config ),
    d( new frame_list_process::priv )
{
  make_ports();
  make_config();
}

frame_list_process
::~frame_list_process()
{
}


// -----------------------------------------------------------------------------
void frame_list_process
::_configure()
{
  scoped_configure_instrumentation();

  // Examine the configuration
  d->m_config_image_list_filename = config_value_using_trait( image_list_file );
  d->m_config_frame_time          = config_value_using_trait( frame_time ) * 1e6; // in usec
  d->m_zero_based_id              = config_value_using_trait( zero_based_id );
  d->m_no_path_in_name            = config_value_using_trait( no_path_in_name );
  d->m_disable_file_load          = config_value_using_trait( disable_file_load );

  std::string path = config_value_using_trait( path );
  kwiver::vital::tokenize( path, d->m_config_path, ":", kwiver::vital::TokenizeTrimEmpty );
  d->m_config_path.push_back( "." ); // add current directory

  kwiver::vital::config_block_sptr algo_config = get_config(); // config for process

  algo::image_io::set_nested_algo_configuration_using_trait( image_reader, algo_config, d->m_image_reader);
  if ( ! d->m_image_reader )
  {
    VITAL_THROW( sprokit::invalid_configuration_exception, name(),
             "Unable to create image_reader." );
  }

  algo::image_io::get_nested_algo_configuration_using_trait( image_reader, algo_config, d->m_image_reader);

  // instantiate image reader and converter based on config type
  if ( ! algo::image_io::check_nested_algo_configuration_using_trait( image_reader, algo_config ) )
  {
    VITAL_THROW( sprokit::invalid_configuration_exception, name(), "Configuration check failed." );
  }
}


// -----------------------------------------------------------------------------
// Post connection initialization
void frame_list_process
::_init()
{
  scoped_init_instrumentation();

  // open file and read lines
  std::ifstream ifs( d->m_config_image_list_filename.c_str() );
  if ( ! ifs )
  {
    std::stringstream msg;
    msg <<  "Could not open image list \"" << d->m_config_image_list_filename << "\"";
    VITAL_THROW( sprokit::invalid_configuration_exception, this->name(), msg.str() );
  }

  kwiver::vital::data_stream_reader stream_reader( ifs );

  // verify and get file names in a list
  for ( std::string line; stream_reader.getline( line ); /* null */ )
  {
    std::string resolved_file = line;
    if ( ! kwiversys::SystemTools::FileExists( line ) )
    {
      // Resolve against specified path
      resolved_file = kwiversys::SystemTools::FindFile( line, d->m_config_path, true );
      if ( resolved_file.empty() )
      {
        VITAL_THROW( kwiver::vital::file_not_found_exception, line, "could not locate file in path" );
      }
    }

    d->m_files.push_back( resolved_file );
  } // end for

  d->m_current_file = d->m_files.begin();
  d->m_frame_number = ( !d->m_zero_based_id ? 1 : 0 );
}

//------------------------------------------------------------------------------
void frame_list_process
::_step()
{
  if ( d->m_current_file != d->m_files.end() )
  {
    scoped_step_instrumentation();

    // still have an image to read
    std::string a_file = *d->m_current_file;

    LOG_DEBUG( logger(), "reading image from file \"" << a_file << "\"" );

    // read image file
    //
    // This call returns a *new* image container. This is good since
    // we are going to pass it downstream using the sptr.
    kwiver::vital::image_container_sptr img_c;

    if( !d->m_disable_file_load )
    {
      img_c = d->m_image_reader->load( a_file );
    }

    // --- debug
#if defined DEBUG
    cv::Mat image = algorithms::ocv::image_container::vital_to_ocv( img_c->get_image() );
    namedWindow( "Display window", cv::WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    waitKey(0);                 // Wait for a keystroke in the window
#endif
    // -- end debug

    kwiver::vital::timestamp frame_ts( d->m_frame_time, d->m_frame_number );

    // update timestamp
    ++d->m_frame_number;
    d->m_frame_time += d->m_config_frame_time;

    // update filename
    if ( d->m_no_path_in_name )
    {
      const size_t last_slash_idx = a_file.find_last_of("\\/");
      if ( std::string::npos != last_slash_idx )
      {
        a_file.erase( 0, last_slash_idx + 1 );
      }
    }

    double output_rate = ( d->m_config_frame_time ? 1.0 / d->m_config_frame_time : 1.0 );

    push_to_port_using_trait( timestamp, frame_ts );
    push_to_port_using_trait( image, img_c );
    push_to_port_using_trait( image_file_name, a_file );
    push_to_port_using_trait( frame_rate, output_rate  );

    ++d->m_current_file;
  }
  else
  {
    LOG_DEBUG( logger(), "End of input reached, process terminating" );

    // indicate done
    mark_process_as_complete();
    const sprokit::datum_t dat = sprokit::datum::complete_datum();

    push_datum_to_port_using_trait( timestamp, dat );
    push_datum_to_port_using_trait( image, dat );
    push_datum_to_port_using_trait( image_file_name, dat );
    push_datum_to_port_using_trait( frame_rate, dat );
  }
}


// -----------------------------------------------------------------------------
void frame_list_process
::make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t optional;
  sprokit::process::port_flags_t shared;
  shared.insert( flag_output_shared );

  declare_output_port_using_trait( timestamp, optional );
  declare_output_port_using_trait( image, shared );
  declare_output_port_using_trait( image_file_name, optional );
  declare_output_port_using_trait( frame_rate, optional );
}


// -----------------------------------------------------------------------------
void frame_list_process
::make_config()
{
  declare_config_using_trait( image_list_file );
  declare_config_using_trait( frame_time );
  declare_config_using_trait( image_reader );
  declare_config_using_trait( path );
  declare_config_using_trait( zero_based_id );
  declare_config_using_trait( no_path_in_name );
  declare_config_using_trait( disable_file_load );
}


// =============================================================================
frame_list_process::priv
::priv()
  : m_frame_number( 1 )
  , m_frame_time( 0 )
{
}

frame_list_process::priv
::~priv()
{
}

} // end namespace
