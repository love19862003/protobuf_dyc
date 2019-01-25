/************************************************
 * \file ProtobufClient.cpp
 * \date 2019/01/25 17:16
 *
 * \author wufan
 * Contact: love19862003@163.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*************************************************/
#include "ProtobufClient.h"
#include <fstream>
#include <assert.h>
#include <google/protobuf/message.h>


namespace DycSpace{

  

  ProtobufClient::ProtobufClient(const std::string& package, const std::string& enName) 
    : DycProtoBase()
    , m_package(package)
    , m_idEnumName(enName)
    , m_idEnum(nullptr){
  
  }

  ProtobufClient::~ProtobufClient(){
    m_idEnum = nullptr;
  }

  bool ProtobufClient::init(const std::string& path, const std::string& regFile){
    resetPool();
    std::ifstream ifile(path + "/" + regFile, std::ios::in | std::ios::binary);
    if (!ifile.is_open()) return false;
    bool res = true;

    std::string line;
    while (getline(ifile, line)){
      if (!line.empty() && !registerPbBin(path + "/" + line)){
        assert(false);
        res = false; 
      }
    }

    m_idEnum = getEnumDesc(m_idEnumName);
    assert(m_idEnum);
    ifile.close();
    return res && nullptr != m_idEnum;
  }

  bool ProtobufClient::packJson(const std::string& msg, const std::string& json, std::string& out) const{
    IdType id = getMessageId(msg);
    assert(id > ErrorId());
    if (id <= ErrorId()) return false;
    out.clear();
    if (jsonFromProtobuf(json, enFullName(msg), out)){
      out.insert(0, (const char*)&id, sizeof(IdType));
      return true;
    }else{
      assert(false);
      return false;
    }
  }

  bool ProtobufClient::pack(const google::protobuf::Message* msg, std::string& out)const{
    if (!msg) return false;
    IdType id = getMessageId(msg->GetDescriptor()->full_name());
    assert(id > ErrorId());
    if (id <= ErrorId()) return false;
    out.clear();
    if (msg->SerializeToString(&out)){
      out.insert(0, (const char*)&id, sizeof(IdType));
      return true;
    }else{
      assert(false);
      return false;
    }
  }

  bool ProtobufClient::unPackJsonArray(const char* buf, size_t len, std::string& json, std::string& msg) const{
    if (len <= sizeof(IdType)) return false;
    msg = getMessageName(buf, len);
    if (msg.empty()){
      assert(false);
      return false;
    }

    if (jsonFromProtobufArray(buf + sizeof(IdType), len - sizeof(IdType), msg, json)){
      return true;
    }else{
      return false;
    }
  }

  bool ProtobufClient::unPackJson(const std::string& content, std::string& json, std::string& msg) const{
    return unPackJsonArray(content.data(), content.length(), json, msg);
  }

  PbMsgPtr ProtobufClient::unPackArray(const char* buf, size_t len) const{
    std::string msg = getMessageName(buf, len);
    if (msg.empty()){
      assert(false);
      return nullptr;
    }

    PbMsgPtr ptr = genMessage(msg);
    if (!ptr || !ptr->ParseFromArray(buf + sizeof(IdType), len - sizeof(IdType))){
      assert(false); 
      return nullptr;
    }
    return ptr;
  }

  PbMsgPtr ProtobufClient::unPack(const std::string& content) const{
    return unPackArray(content.data(), content.length());
  }


  bool ProtobufClient::dumpEnum(){
    if (!m_idEnum){
      return false;
    }

    std::cout << "full name " << m_idEnum->full_name() << std::endl;
    std::cout << "info" << m_idEnum->DebugString() << std::endl;
    return true;
  }

  ProtobufClient::IdType ProtobufClient::getMessageId(const std::string& name) const{
    if (name.empty() || !m_idEnum){
      return ErrorId();
    }
    auto ev = m_idEnum->FindValueByName(enSubName(name));
    if (!ev){
      return ErrorId();
    }
    return ev->number();
  }

  std::string ProtobufClient::messageType(IdType id) const{
    if (nullptr == m_idEnum || id == ErrorId()){
      return "";
    }
    auto ev = m_idEnum->FindValueByNumber(id);
    if (!ev){
      return "";
    }
    return ev->name();
  }

  std::string ProtobufClient::enSubName(const std::string& name) const{
    if (name.empty()){
      return name;
    }

    size_t pos = name.find(m_package);
    if (pos != std::string::npos){
      return name.substr(pos + m_package.length());
    } else{
      return name;
    }
  }

  std::string ProtobufClient::enFullName(const std::string& name) const{
    if (name.empty()){
      return name;
    }

    if (name.find(m_package) != std::string::npos){
      return name;
    }else{
      return m_package + name;
    }
  }

  std::string ProtobufClient::getMessageName(const char* buf, size_t len) const{
    if (len <= sizeof(IdType)){
      return "";
    }
    IdType id = ErrorId();
    memcpy(&id, buf, sizeof(IdType));

    if (id  == ErrorId()){
      return "";
    }
    return enFullName(messageType(id));
  }


}