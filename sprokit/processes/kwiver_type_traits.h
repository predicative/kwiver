// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/**
 * \file
 * \brief definition of kwiver type traits
 */

#ifndef KWIVER_VITAL_TYPE_TRAITS_H
#define KWIVER_VITAL_TYPE_TRAITS_H

#include <vital/vital_types.h>

#include <vital/types/database_query.h>
#include <vital/types/descriptor_set.h>
#include <vital/types/descriptor_request.h>
#include <vital/types/detected_object_set.h>
#include <vital/types/feature_set.h>
#include <vital/types/feature_track_set.h>
#include <vital/types/homography_f2f.h>
#include <vital/types/geo_polygon.h>
#include <vital/types/homography.h>
#include <vital/types/image_container.h>
#include <vital/types/iqr_feedback.h>
#include <vital/types/matrix.h>
#include <vital/types/metadata.h>
#include <vital/types/image_container_set.h>
#include <vital/types/object_track_set.h>
#include <vital/types/query_result_set.h>
#include <vital/types/timestamp.h>
#include <vital/types/track_descriptor_set.h>
#include <vital/types/uid.h>

#include "trait_utils.h"

#include <memory>
#include <string>
#include <vector>

namespace kwiver {
namespace vital {

  typedef std::vector< double >  double_vector;
  typedef std::shared_ptr< double_vector > double_vector_sptr;
  typedef std::vector< std::string > string_vector;
  typedef std::shared_ptr< string_vector > string_vector_sptr;
  typedef std::vector< unsigned char > uchar_vector;
  typedef std::shared_ptr< uchar_vector > uchar_vector_sptr;
  using string_sptr =  std::shared_ptr< std::string >;

} }

// ==================================================================================
//
// Create type traits for common pipeline types.
// These are types that are passed through the pipeline.
// ( type-trait-name, "canonical_type_name", concrete-type )
//
create_type_trait( bounding_box, "kwiver:bounding_box", kwiver::vital::bounding_box_d );
create_type_trait( timestamp, "kwiver:timestamp", kwiver::vital::timestamp );
create_type_trait( time_interval_sec, "kwiver:time_interval_sec", double );
create_type_trait( gsd, "kwiver:gsd", double );
create_type_trait( corner_points, "kwiver:corner_points", kwiver::vital::geo_polygon );
create_type_trait( image, "kwiver:image", kwiver::vital::image_container_sptr );
create_type_trait( image_set, "kwiver:image_set", kwiver::vital::image_container_set_sptr );
create_type_trait( mask, "kwiver:mask", kwiver::vital::image_container_sptr );
create_type_trait( bool, "kwiver:bool", bool );
create_type_trait( feature_set, "kwiver:feature_set", kwiver::vital::feature_set_sptr );
create_type_trait( database_query, "kwiver:database_query", kwiver::vital::database_query_sptr );
create_type_trait( descriptor_set, "kwiver:descriptor_set", kwiver::vital::descriptor_set_sptr );
create_type_trait( descriptor_request, "kwiver:descriptor_request", kwiver::vital::descriptor_request_sptr );
create_type_trait( query_result, "kwiver:query_result", kwiver::vital::query_result_set_sptr );
create_type_trait( iqr_feedback, "kwiver:iqr_feedback", kwiver::vital::iqr_feedback_sptr );
create_type_trait( string, "kwiver:string", kwiver::vital::string_t );
create_type_trait( string_vector, "kwiver:string_vector", kwiver::vital::string_vector_sptr );
create_type_trait( track_set, "kwiver:track_set", kwiver::vital::track_set_sptr );
create_type_trait( feature_track_set, "kwiver:feature_track_set", kwiver::vital::feature_track_set_sptr );
create_type_trait( object_track_set, "kwiver:object_track_set", kwiver::vital::object_track_set_sptr );
create_type_trait( double_vector,  "kwiver:d_vector", kwiver::vital::double_vector_sptr );
create_type_trait( uchar_vector,  "kwiver:uchar_vector", kwiver::vital::uchar_vector_sptr );
create_type_trait( detected_object_set, "kwiver:detected_object_set", kwiver::vital::detected_object_set_sptr );
create_type_trait( track_descriptor_set, "kwiver:track_descriptor_set", kwiver::vital::track_descriptor_set_sptr );
create_type_trait( matrix_d, "kwiver:matrix_d", kwiver::vital::matrix_d );

create_type_trait( homography, "kwiver:homography", kwiver::vital::homography_sptr );
create_type_trait( homography_src_to_ref, "kwiver:s2r_homography", kwiver::vital::f2f_homography );
create_type_trait( homography_ref_to_src, "kwiver:r2s_homography", kwiver::vital::f2f_homography );
create_type_trait( file_name, "kwiver:file_name", kwiver::vital::path_t );
create_type_trait( metadata, "kwiver:metadata", kwiver::vital::metadata_vector );
create_type_trait( video_uid, "kwiver:video_uuid", kwiver::vital::uid );
create_type_trait( frame_rate, "kwiver:frame_rate", double );
create_type_trait( kwiver_logical, "kwiver:logical", bool );

create_type_trait( serialized_message, "kwiver:serialized_message", kwiver::vital::string_sptr );

// ==================================================================================
//
// Create port traits for common port types.
// ( port-name, type-trait-name, "port-description" )
//
create_port_trait( bounding_box, bounding_box, "Bounding box" );
create_port_trait( timestamp, timestamp, "Timestamp for input image." );
create_port_trait( corner_points, corner_points, "Four corner points for image in lat/lon units, ordering ul ur lr ll." );
create_port_trait( gsd, gsd, "GSD for image in meters per pixel." );
create_port_trait( success_flag, bool, "A flag inticating an operation is successful." );
create_port_trait( image, image, "Single frame image." );
create_port_trait( image_set, image_set, "A collection of images." );
create_port_trait( left_image, image, "Single frame left image." );
create_port_trait( right_image, image, "Single frame right image." );
create_port_trait( depth_map, image, "Depth map stored in image form." );
create_port_trait( feature_set, feature_set, "Set of detected image features." );
create_port_trait( database_query, database_query, "A database query." );
create_port_trait( descriptor_set, descriptor_set, "Set of descriptors." );
create_port_trait( descriptor_request, descriptor_request, "A request to compute descriptors." );
create_port_trait( iqr_feedback, iqr_feedback, "IQR feedback." );
create_port_trait( iqr_model, uchar_vector, "IQR model." );
create_port_trait( query_result, query_result, "Set of query results." );
create_port_trait( string_vector, string_vector, "Vector of strings." );
create_port_trait( track_set, track_set, "Set of arbitrary tracks." );
create_port_trait( feature_track_set, feature_track_set, "Set of feature tracks." );
create_port_trait( object_track_set, object_track_set, "Set of object tracks." );
create_port_trait( detected_object_set, detected_object_set, "Set of detected objects." );
create_port_trait( track_descriptor_set, track_descriptor_set, "Set of track descriptors." );
create_port_trait( matrix_d, matrix_d, "2-dimensional double matrix." );

create_port_trait( homography_src_to_ref, homography_src_to_ref, "Source image to ref image homography." );
create_port_trait( file_name, file_name, "Name of an arbitrary data file." );
create_port_trait( image_file_name, file_name, "Name of an image file." );
create_port_trait( video_file_name, file_name, "Name of a video file." );
create_port_trait( metadata, metadata, "Video metadata vector for a frame." );
create_port_trait( video_uid, video_uid, "Video UID value." );
create_port_trait( frame_rate, frame_rate, "Video frame rate." );
create_port_trait( detection_time, time_interval_sec, "Elapsed time for this detection.\n\n"
    "This port produces the number of seconds that it took to perform the detection operation on the current frame." );


create_port_trait( serialized_message, serialized_message, "serialized data type" );

create_port_trait( coordinate_system_updated, kwiver_logical, "Set to true if new reference frame is established." );
create_port_trait( motion_heat_map, image, "Motion heat map." );

#endif // KWIVER_VITAL_TYPE_TRAITS_H
