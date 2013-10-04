/*
 * (C) Copyright 2013 Kurento (http://kurento.org/)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 */

#include "server_test_base.hpp"
#include <boost/test/unit_test.hpp>

#include "KmsMediaServer_constants.h"
#include "KmsMediaDataType_constants.h"
#include "KmsMediaErrorCodes_constants.h"

#include "KmsMediaUriEndPointType_constants.h"
#include "KmsMediaPlayerEndPointType_constants.h"
#include "KmsMediaRecorderEndPointType_constants.h"

#include "utils/marshalling.hpp"

#include <gst/gst.h>

#define GST_CAT_DEFAULT _server_test_
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);
#define GST_DEFAULT_NAME "server_test"

using namespace kurento;

BOOST_FIXTURE_TEST_SUITE ( server_test_suite,  F)

static void
check_version (boost::shared_ptr<kurento::KmsMediaServerServiceClient> client)
{
  int32_t gotVersion;

  gotVersion = client->getVersion();
  BOOST_CHECK_EQUAL (gotVersion, g_KmsMediaServer_constants.VERSION);
}

#if 0 /* Temporally disabled */
static void
check_no_handler (boost::shared_ptr<kurento::MediaServerServiceClient> client)
{
  MediaObjectId mediaPipeline = MediaObjectId();

  BOOST_CHECK_THROW (client->createMediaPipeline (mediaPipeline, 0), HandlerNotFoundException);
}

static void
check_add_handler_address (boost::shared_ptr<kurento::MediaServerServiceClient> client)
{
  client->addHandlerAddress (0, "localhost", 2323);
  client->addHandlerAddress (0, "localhost", 3434);
}

static void
check_type (boost::shared_ptr<kurento::MediaServerServiceClient> client)
{
  MediaObjectId mediaPipeline = MediaObjectId();
  MediaObjectId mo = MediaObjectId();

  client->createMediaPipeline (mediaPipeline, 0);
  BOOST_CHECK (mediaPipeline.type.__isset.mediaObject);
  BOOST_CHECK_EQUAL (mediaPipeline.type.mediaObject, MediaObjectType::type::MEDIA_PIPELINE);

  client->createSdpEndPoint (mo, mediaPipeline, SdpEndPointType::type::RTP_END_POINT);
  BOOST_CHECK (mo.type.__isset.sdpEndPoint);
  BOOST_CHECK_EQUAL (mo.type.sdpEndPoint, SdpEndPointType::type::RTP_END_POINT);

  client->createSdpEndPoint (mo, mediaPipeline, SdpEndPointType::type::WEBRTC_END_POINT);
  BOOST_CHECK (mo.type.__isset.sdpEndPoint);
  BOOST_CHECK_EQUAL (mo.type.sdpEndPoint, SdpEndPointType::type::WEBRTC_END_POINT);

  client->createUriEndPoint (mo, mediaPipeline, UriEndPointType::type::PLAYER_END_POINT, "");
  BOOST_CHECK (mo.type.__isset.uriEndPoint);
  BOOST_CHECK_EQUAL (mo.type.uriEndPoint, UriEndPointType::type::PLAYER_END_POINT);

  client->createUriEndPoint (mo, mediaPipeline, UriEndPointType::type::RECORDER_END_POINT, "");
  BOOST_CHECK (mo.type.__isset.uriEndPoint);
  BOOST_CHECK_EQUAL (mo.type.uriEndPoint, UriEndPointType::type::RECORDER_END_POINT);

  client->createHttpEndPoint (mo, mediaPipeline);
  BOOST_CHECK (mo.type.__isset.endPoint);
  BOOST_CHECK_EQUAL (mo.type.endPoint, EndPointType::type::HTTP_END_POINT);

  client->createMixer (mo, mediaPipeline, MixerType::type::MAIN_MIXER);
  BOOST_CHECK (mo.type.__isset.mixerType);
  BOOST_CHECK_EQUAL (mo.type.mixerType, MixerType::type::MAIN_MIXER);

  client->release (mediaPipeline);
}
#endif

static void
check_use_released_media_pipeline (boost::shared_ptr<kurento::KmsMediaServerServiceClient> client)
{
  KmsMediaObjectRef mediaPipeline = KmsMediaObjectRef();
  KmsMediaObjectRef mo = KmsMediaObjectRef();
  KmsMediaParams params = KmsMediaParams ();

  params.__set_dataType (g_KmsMediaDataType_constants.STRING_DATA_TYPE);
  params.__set_data (marshalString ("file:///tmp/f.webm") );

  client->createMediaPipeline (mediaPipeline);
  client->release (mediaPipeline);

  try {
    client->createMediaElementWithParams (mo, mediaPipeline, g_KmsMediaPlayerEndPointType_constants.TYPE_NAME, params);
    BOOST_FAIL ("Use a released MediaPipeline must throw a KmsMediaServerException");
  } catch (const KmsMediaServerException &e) {
    BOOST_CHECK_EQUAL (g_KmsMediaErrorCodes_constants.MEDIA_OBJECT_NOT_FOUND, e.errorCode);
  }
}

static void
check_parent (boost::shared_ptr<kurento::KmsMediaServerServiceClient> client)
{
  KmsMediaObjectRef mediaPipeline = KmsMediaObjectRef();
  KmsMediaObjectRef mo = KmsMediaObjectRef();
  KmsMediaObjectRef parent = KmsMediaObjectRef();
  KmsMediaParams params = KmsMediaParams ();

  params.__set_dataType (g_KmsMediaDataType_constants.STRING_DATA_TYPE);
  params.__set_data (marshalString ("file:///tmp/f.webm") );

  client->createMediaPipeline (mediaPipeline);
  client->createMediaElementWithParams (mo, mediaPipeline, g_KmsMediaPlayerEndPointType_constants.TYPE_NAME, params);
  client->getParent (parent, mo);
  BOOST_CHECK_EQUAL (mediaPipeline.id, parent.id);

  client->release (mediaPipeline);
}

static void
check_getMediaPipeline (boost::shared_ptr<kurento::KmsMediaServerServiceClient> client)
{
  KmsMediaObjectRef mediaPipeline = KmsMediaObjectRef();
  KmsMediaObjectRef mo = KmsMediaObjectRef();
  KmsMediaObjectRef mediaPipelineGot = KmsMediaObjectRef();
  KmsMediaParams params = KmsMediaParams ();

  params.__set_dataType (g_KmsMediaDataType_constants.STRING_DATA_TYPE);
  params.__set_data (marshalString ("file:///tmp/f.webm") );

  client->createMediaPipeline (mediaPipeline);
  client->getMediaPipeline (mediaPipelineGot, mediaPipeline);
  BOOST_CHECK_EQUAL (mediaPipeline.id, mediaPipelineGot.id);

  client->createMediaElementWithParams (mo, mediaPipeline, g_KmsMediaPlayerEndPointType_constants.TYPE_NAME, params);
  client->getMediaPipeline (mediaPipelineGot, mo);
  BOOST_CHECK_EQUAL (mediaPipeline.id, mediaPipelineGot.id);

  client->release (mediaPipeline);
}

static void
check_same_token (boost::shared_ptr<kurento::KmsMediaServerServiceClient> client)
{
  KmsMediaObjectRef mediaPipeline = KmsMediaObjectRef();
  KmsMediaObjectRef mo = KmsMediaObjectRef();
  KmsMediaParams params = KmsMediaParams ();

  params.__set_dataType (g_KmsMediaDataType_constants.STRING_DATA_TYPE);
  params.__set_data (marshalString ("file:///tmp/f.webm") );

  client->createMediaPipeline (mediaPipeline);
  client->createMediaElementWithParams (mo, mediaPipeline, g_KmsMediaPlayerEndPointType_constants.TYPE_NAME, params);
  BOOST_CHECK_EQUAL (mediaPipeline.token, mo.token);

  client->release (mediaPipeline);
}

#if 0 /* Temporally disabled */
static void
check_get_parent_in_released_media_pipeline (boost::shared_ptr<kurento::MediaServerServiceClient> client)
{
  MediaObjectId mediaPipeline = MediaObjectId();
  MediaObjectId mo = MediaObjectId();
  MediaObjectId parent = MediaObjectId();

  client->createMediaPipeline (mediaPipeline, 0);
  client->createMixer (mo, mediaPipeline, MixerType::type::MAIN_MIXER);
  client->release (mediaPipeline);

  BOOST_CHECK_THROW (client->getParent (parent, mo), MediaObjectNotFoundException);
}

static void
check_media_pipeline_no_parent (boost::shared_ptr<kurento::MediaServerServiceClient> client)
{
  MediaObjectId mediaPipeline = MediaObjectId();
  MediaObjectId parent = MediaObjectId();

  GST_DEBUG ("check_media_pipeline_no_parent test");
  client->createMediaPipeline (mediaPipeline, 0);
  BOOST_CHECK_THROW (client->getParent (parent, mediaPipeline), NoParentException);

  client->release (mediaPipeline);
}
#endif

static void
check_player_end_point (boost::shared_ptr<kurento::KmsMediaServerServiceClient> client)
{
  KmsMediaObjectRef mediaPipeline = KmsMediaObjectRef();
  KmsMediaObjectRef playerEndPoint = KmsMediaObjectRef();
  KmsMediaParams params = KmsMediaParams ();
  KmsMediaCommand command;
  KmsMediaCommandResult result;
  std::string originalUri = "file:///tmp/player_end_point_test.webm";
  std::string resultUri;

  params.__set_dataType (g_KmsMediaDataType_constants.STRING_DATA_TYPE);
  params.__set_data (marshalString (originalUri) );

  client->createMediaPipeline (mediaPipeline);
  client->createMediaElementWithParams (playerEndPoint, mediaPipeline, g_KmsMediaPlayerEndPointType_constants.TYPE_NAME, params);

  command.__set_name (g_KmsMediaUriEndPointType_constants.GET_URI);
  client->sendCommand (result, playerEndPoint, command);

  BOOST_CHECK_EQUAL (g_KmsMediaDataType_constants.STRING_DATA_TYPE, result.dataType);

  BOOST_REQUIRE_NO_THROW (resultUri = unmarshalString (result.data) );
  BOOST_CHECK_EQUAL (0, originalUri.compare (resultUri) );

  client->release (mediaPipeline);
}

static void
check_recorder_end_point (boost::shared_ptr<kurento::KmsMediaServerServiceClient> client)
{
  KmsMediaObjectRef mediaPipeline = KmsMediaObjectRef();
  KmsMediaObjectRef recorderEndPoint = KmsMediaObjectRef();
  KmsMediaParams params = KmsMediaParams ();
  KmsMediaCommand command;
  KmsMediaCommandResult result;
  std::string originalUri = "file:///tmp/player_end_point_test.webm";
  std::string resultUri;

  params.__set_dataType (g_KmsMediaDataType_constants.STRING_DATA_TYPE);
  params.__set_data (marshalString (originalUri) );

  client->createMediaPipeline (mediaPipeline);
  client->createMediaElementWithParams (recorderEndPoint, mediaPipeline, g_KmsMediaRecorderEndPointType_constants.TYPE_NAME, params);

  command.__set_name (g_KmsMediaUriEndPointType_constants.GET_URI);
  client->sendCommand (result, recorderEndPoint, command);

  BOOST_CHECK_EQUAL (g_KmsMediaDataType_constants.STRING_DATA_TYPE, result.dataType);

  BOOST_REQUIRE_NO_THROW (resultUri = unmarshalString (result.data) );
  BOOST_CHECK_EQUAL (0, originalUri.compare (resultUri) );

  client->release (mediaPipeline);
}

#if 0 /* Temporally disabled */
static void
check_http_end_point (boost::shared_ptr<kurento::MediaServerServiceClient> client)
{
  MediaObjectId mediaPipeline = MediaObjectId();
  MediaObjectId httpEp = MediaObjectId();
  std::string out;

  client->createMediaPipeline (mediaPipeline, 0);

  client->createHttpEndPoint (httpEp, mediaPipeline);
  client->getUrl (out, httpEp);

  client->release (mediaPipeline);
}

static void
check_zbar_filter (boost::shared_ptr<kurento::MediaServerServiceClient> client)
{
  MediaObjectId mediaPipeline = MediaObjectId();
  MediaObjectId zbarFilter = MediaObjectId();
  std::string out;

  client->createMediaPipeline (mediaPipeline, 0);
  client->createFilter (zbarFilter, mediaPipeline, FilterType::type::ZBAR_FILTER);
  client->release (mediaPipeline);
}
#endif

static void
client_side (boost::shared_ptr<kurento::KmsMediaServerServiceClient> client)
{
  check_version (client);

#if 0 /* Temporally disabled */
  check_no_handler (client);
  check_add_handler_address (client);
#endif

  check_use_released_media_pipeline (client);
  check_parent (client);
  check_getMediaPipeline (client);
  check_same_token (client);

#if 0 /* Temporally disabled */
  check_type (client);
  check_get_parent_in_released_media_pipeline (client);
  check_media_pipeline_no_parent (client);
#endif

  check_player_end_point (client);
  check_recorder_end_point (client);

#if 0 /* Temporally disabled */
  check_http_end_point (client);
  check_zbar_filter (client);
#endif
}

BOOST_AUTO_TEST_CASE ( server_test )
{
  BOOST_REQUIRE_MESSAGE (initialized, "Cannot connect to the server");
  GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, GST_DEFAULT_NAME, 0, GST_DEFAULT_NAME);
  client_side (client);
}

BOOST_AUTO_TEST_SUITE_END()
