#ifndef WEB_TOKEN
#define WEB_TOKEN

#include <windows.data.json.h>
#include <QDataStream>
#include <iostream>
#include <string>
#include "../Encryption/speck_cypher.h"
#include "base64.h"
#include "json\single_include\nlohmann\json.hpp"

using json = nlohmann::json;

//@This class generate JWT token
//@All fields of class are private, so if you want to
//@generate token make next things:
//Code example:
//  WebToken wt;
//  wt.SetHeader("SPEK");
//  wt.SetPayload(1);
//  QString token = wt.GetToken("key");

class WebToken {
 public:
  explicit WebToken() = default;
  virtual ~WebToken() = default;
  std::string GetToken(const std::string& key);  // static?
  void SetPayload(const unsigned int id);//static?
  void SetHeader(const std::string& algorithm_name,
                 const std::string& type = "JWT");  // static?
  std::string signature_;
 private:
  std::string token_;
  json header_;
  json payload_;
  
  SpeckCypher cypher;
};

#endif  // !WEB_TOKEN
