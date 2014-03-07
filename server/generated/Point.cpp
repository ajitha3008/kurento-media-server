/* Autogenerated with Kurento Idl */

#include "Point.hpp"
#include <JsonSerializer.hpp>

namespace kurento {

Point::Point (const Json::Value &value) {
  Json::Value aux;

  if (value.isMember ("x")) {
    aux = value["x"];

    if (!aux.isInt ()) {
      /* param 'x' has invalid type value, raise exception */
      JsonRpc::CallException e (JsonRpc::ErrorCode::SERVER_ERROR_INIT,
                                "'x' parameter should be a integer");
      throw e;
    } else {
      JsonSerializer s(false);
      s.JsonValue = value;
      s.SerializeNVP(x);
    }
  } else {
    /* Requiered property 'x' not present, raise exception */
    JsonRpc::CallException e (JsonRpc::ErrorCode::SERVER_ERROR_INIT,
                              "'x' property is requiered");
    throw e;
  }

  if (value.isMember ("y")) {
    aux = value["y"];

    if (!aux.isInt ()) {
      /* param 'y' has invalid type value, raise exception */
      JsonRpc::CallException e (JsonRpc::ErrorCode::SERVER_ERROR_INIT,
                                "'y' parameter should be a integer");
      throw e;
    } else {
      JsonSerializer s(false);
      s.JsonValue = value;
      s.SerializeNVP(y);
    }
  } else {
    /* Requiered property 'y' not present, raise exception */
    JsonRpc::CallException e (JsonRpc::ErrorCode::SERVER_ERROR_INIT,
                              "'y' property is requiered");
    throw e;
  }

}

} /* kurento */

void
Serialize(std::shared_ptr<kurento::Point>& object, JsonSerializer& s)
{
  if (!s.IsWriter && !object) {
    object.reset(new kurento::Point());
  }

  if (object) {
    s.Serialize("x", object->x);
    s.Serialize("y", object->y);
  }

  if (!s.IsWriter) {

  }

}
