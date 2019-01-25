/************************************************
 * \file ProtobufClient.h
 * \date 2019/01/25 17:03
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
#pragma once

#include "ProtobufDyc.h"
namespace DycSpace{
  class ProtobufClient : public DycProtoBase
  {
  public:
    explicit ProtobufClient(const std::string& package, const std::string& enName);
    virtual ~ProtobufClient();

    bool init(const std::string& path, const std::string& regFile);
    bool packJson(const std::string& msg, const std::string& json, std::string& out) const;
    bool pack(const google::protobuf::Message* msg, std::string& out) const;
    bool unPackJson(const std::string& content, std::string& json, std::string& msg) const;
    bool unPackJsonArray(const char* buf, size_t len, std::string& json, std::string& msg) const;
    PbMsgPtr unPack(const std::string& content) const;
    PbMsgPtr unPackArray(const char* buf, size_t len) const;
    bool dumpEnum();
  protected:
  private:
    typedef uint16_t IdType;
    static constexpr IdType ErrorId(){ return 0; }
    IdType getMessageId(const std::string& name)const;
    std::string messageType(IdType id) const;
    std::string enSubName(const std::string& name) const;
    std::string enFullName(const std::string& name) const;
    std::string getMessageName(const char* buf, size_t len) const;
  private:
    const std::string m_package;
    const std::string m_idEnumName;
    const google::protobuf::EnumDescriptor* m_idEnum = nullptr;
  };
}
