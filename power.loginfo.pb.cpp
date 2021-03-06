// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: power.loginfo.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "power.loginfo.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace power {

namespace {

const ::google::protobuf::Descriptor* loginfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  loginfo_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_power_2eloginfo_2eproto() {
  protobuf_AddDesc_power_2eloginfo_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "power.loginfo.proto");
  GOOGLE_CHECK(file != NULL);
  loginfo_descriptor_ = file->message_type(0);
  static const int loginfo_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(loginfo, log_name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(loginfo, log_content_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(loginfo, log_len_),
  };
  loginfo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      loginfo_descriptor_,
      loginfo::default_instance_,
      loginfo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(loginfo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(loginfo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(loginfo));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_power_2eloginfo_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    loginfo_descriptor_, &loginfo::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_power_2eloginfo_2eproto() {
  delete loginfo::default_instance_;
  delete loginfo_reflection_;
}

void protobuf_AddDesc_power_2eloginfo_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\023power.loginfo.proto\022\005power\"A\n\007loginfo\022"
    "\020\n\010log_name\030\001 \002(\t\022\023\n\013log_content\030\002 \002(\014\022\017"
    "\n\007log_len\030\003 \002(\005", 95);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "power.loginfo.proto", &protobuf_RegisterTypes);
  loginfo::default_instance_ = new loginfo();
  loginfo::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_power_2eloginfo_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_power_2eloginfo_2eproto {
  StaticDescriptorInitializer_power_2eloginfo_2eproto() {
    protobuf_AddDesc_power_2eloginfo_2eproto();
  }
} static_descriptor_initializer_power_2eloginfo_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int loginfo::kLogNameFieldNumber;
const int loginfo::kLogContentFieldNumber;
const int loginfo::kLogLenFieldNumber;
#endif  // !_MSC_VER

loginfo::loginfo()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:power.loginfo)
}

void loginfo::InitAsDefaultInstance() {
}

loginfo::loginfo(const loginfo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:power.loginfo)
}

void loginfo::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  log_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  log_content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  log_len_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

loginfo::~loginfo() {
  // @@protoc_insertion_point(destructor:power.loginfo)
  SharedDtor();
}

void loginfo::SharedDtor() {
  if (log_name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete log_name_;
  }
  if (log_content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete log_content_;
  }
  if (this != default_instance_) {
  }
}

void loginfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* loginfo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return loginfo_descriptor_;
}

const loginfo& loginfo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_power_2eloginfo_2eproto();
  return *default_instance_;
}

loginfo* loginfo::default_instance_ = NULL;

loginfo* loginfo::New() const {
  return new loginfo;
}

void loginfo::Clear() {
  if (_has_bits_[0 / 32] & 7) {
    if (has_log_name()) {
      if (log_name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        log_name_->clear();
      }
    }
    if (has_log_content()) {
      if (log_content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        log_content_->clear();
      }
    }
    log_len_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool loginfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:power.loginfo)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string log_name = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_log_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->log_name().data(), this->log_name().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "log_name");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_log_content;
        break;
      }

      // required bytes log_content = 2;
      case 2: {
        if (tag == 18) {
         parse_log_content:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_log_content()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_log_len;
        break;
      }

      // required int32 log_len = 3;
      case 3: {
        if (tag == 24) {
         parse_log_len:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &log_len_)));
          set_has_log_len();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:power.loginfo)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:power.loginfo)
  return false;
#undef DO_
}

void loginfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:power.loginfo)
  // required string log_name = 1;
  if (has_log_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->log_name().data(), this->log_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "log_name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->log_name(), output);
  }

  // required bytes log_content = 2;
  if (has_log_content()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      2, this->log_content(), output);
  }

  // required int32 log_len = 3;
  if (has_log_len()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->log_len(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:power.loginfo)
}

::google::protobuf::uint8* loginfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:power.loginfo)
  // required string log_name = 1;
  if (has_log_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->log_name().data(), this->log_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "log_name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->log_name(), target);
  }

  // required bytes log_content = 2;
  if (has_log_content()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->log_content(), target);
  }

  // required int32 log_len = 3;
  if (has_log_len()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->log_len(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:power.loginfo)
  return target;
}

int loginfo::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string log_name = 1;
    if (has_log_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->log_name());
    }

    // required bytes log_content = 2;
    if (has_log_content()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->log_content());
    }

    // required int32 log_len = 3;
    if (has_log_len()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->log_len());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void loginfo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const loginfo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const loginfo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void loginfo::MergeFrom(const loginfo& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_log_name()) {
      set_log_name(from.log_name());
    }
    if (from.has_log_content()) {
      set_log_content(from.log_content());
    }
    if (from.has_log_len()) {
      set_log_len(from.log_len());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void loginfo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void loginfo::CopyFrom(const loginfo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool loginfo::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000007) != 0x00000007) return false;

  return true;
}

void loginfo::Swap(loginfo* other) {
  if (other != this) {
    std::swap(log_name_, other->log_name_);
    std::swap(log_content_, other->log_content_);
    std::swap(log_len_, other->log_len_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata loginfo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = loginfo_descriptor_;
  metadata.reflection = loginfo_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace power

// @@protoc_insertion_point(global_scope)
