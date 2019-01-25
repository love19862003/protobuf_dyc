/************************************************
 * \file protobuf-dyc.h
 * \date 2019/01/25 16:21
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
#include <string>
#include <memory>
namespace google {
namespace protobuf{
  class DescriptorPool;
  class EnumDescriptor;
  class Message;
}
}
namespace DycSpace{
   typedef std::shared_ptr<google::protobuf::Message> PbMsgPtr;
   class DycProtoBase
   {
   public:
     explicit DycProtoBase();
     virtual ~DycProtoBase();
   protected:
     bool registerPbBin(const std::string& file);
     bool dumpProtoDesc(const std::string& in, std::string& out) const;
     void resetPool();
     bool jsonFromProtobuf(const std::string& str, const std::string& msg, std::string& out) const;
     bool jsonFromProtobufArray(const char* buf, size_t len, const std::string& msg, std::string& out) const;
     bool jsonToProtobuf(const std::string& json, const std::string& msg, std::string& out) const;
     const google::protobuf::EnumDescriptor* getEnumDesc(const std::string& name) const;
     PbMsgPtr genMessage(const std::string& name) const;
   protected:
   private:
     DycProtoBase(const DycProtoBase &) = delete;
     DycProtoBase& operator = (const DycProtoBase&) = delete;

   private:
     std::unique_ptr<google::protobuf::DescriptorPool> m_pool;
   };
}