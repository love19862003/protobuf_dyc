/************************************************
 * \file protobuf-dyc.cpp
 * \date 2019/01/25 16:30
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

#include "ProtobufDyc.h"
#include <iostream>
#include <fstream>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/compiler/parser.h>
namespace pb = google::protobuf;
namespace DycSpace{

  DycProtoBase::DycProtoBase() : m_pool(nullptr){
  
  }
  DycProtoBase::~DycProtoBase(){
    m_pool.reset();
  }

  bool DycProtoBase::registerPbBin(const std::string& file){
    if (!m_pool) return false;
    std::ifstream ifile(file, std::ios::binary | std::ios::in);
    if (!ifile.is_open()) return false;
    pb::FileDescriptorSet fset;
    bool r = fset.ParseFromIstream(&ifile);
    assert(r);
    ifile.close();
    if (!r){
      return false;
    }
    for (auto& desc : fset.file()){
      if (!m_pool->BuildFile(desc)) return false;
    }
    return true;
  }

  bool DycProtoBase::dumpProtoDesc(const std::string& in, std::string& out) const{
    std::ifstream ifile(in, std::ios::binary | std::ios::in);
    if (!ifile.is_open()) return false;
    pb::FileDescriptorSet fset;
    if (fset.ParseFromIstream(&ifile)){
      std::string str;
      if (pb::util::MessageToJsonString(fset, &str).ok()){
        std::ofstream ofile(out, std::ios::binary | std::ios::out | std::ios::trunc);
        ofile << str;
        ofile.close();
      }
    }
    ifile.close();
    return true;
  }

  void DycProtoBase::resetPool(){
    m_pool.reset(new pb::DescriptorPool());
  }

  PbMsgPtr DycProtoBase::genMessage(const std::string& name) const{
    if (nullptr == m_pool){
      assert(false);
      return nullptr;
    }
    const pb::Descriptor* desc = m_pool->FindMessageTypeByName(name);
    if (!desc){
      assert(false);
      return nullptr;
    }

    pb::DynamicMessageFactory factory;
    const pb::Message* m = factory.GetPrototype(desc);
    assert(nullptr != m);
    if (!m) return nullptr;

    PbMsgPtr ptr(m->New());
    return ptr;
  }

  bool DycProtoBase::jsonFromProtobuf(const std::string& str, const std::string& msg, std::string& out) const{
    return jsonFromProtobufArray(str.data(), str.length(), msg, out);
  }

  bool DycProtoBase::jsonFromProtobufArray(const char* buf, size_t len, const std::string& msg, std::string& out) const{
    PbMsgPtr ptr = genMessage(msg);
    if(!ptr){
      assert(false);
      return false;
    }

    if (ptr->ParseFromArray(buf, static_cast<int>(len))){
      return pb::util::MessageToJsonString(*ptr, &out).ok();
    }else{
      assert(false);
      return false;
    }
  }

  bool DycProtoBase::jsonToProtobuf(const std::string& json, const std::string& msg, std::string& out) const{
    PbMsgPtr ptr = genMessage(msg);
    if (!ptr){
      assert(false);
      return false;
    }
    if (pb::util::JsonStringToMessage(json, ptr.get()).ok()){
      return ptr->SerializeToString(&out);
    }else{
      assert(false);
      return false;
    }
  }

  const pb::EnumDescriptor* DycProtoBase::getEnumDesc(const std::string& name) const{
    auto res = m_pool->FindEnumTypeByName(name);
    assert(res);
    return res;
  }

}