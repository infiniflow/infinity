/**
 * Autogenerated by Thrift Compiler (0.20.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef PeerService_H
#define PeerService_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include <memory>
#include "peer_server_types.h"

namespace infinity_peer_server {

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class PeerServiceIf {
 public:
  virtual ~PeerServiceIf() {}
  virtual void Register(RegisterResponse& _return, const RegisterRequest& request) = 0;
  virtual void UnRegister(UnRegisterResponse& _return, const UnRegisterRequest& request) = 0;
  virtual void SendHeartBeat(HeartBeatResponse& _return, const HeartBeatRequest& request) = 0;
  virtual void SyncLog(SyncLogResponse& _return, const SyncLogRequest& request) = 0;
  virtual void ChangeRole(ChangeRoleResponse& _return, const ChangeRoleRequest& request) = 0;
  virtual void NewLeader(NewLeaderResponse& _return, const NewLeaderRequest& request) = 0;
};

class PeerServiceIfFactory {
 public:
  typedef PeerServiceIf Handler;

  virtual ~PeerServiceIfFactory() {}

  virtual PeerServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(PeerServiceIf* /* handler */) = 0;
  };

class PeerServiceIfSingletonFactory : virtual public PeerServiceIfFactory {
 public:
  PeerServiceIfSingletonFactory(const ::std::shared_ptr<PeerServiceIf>& iface) : iface_(iface) {}
  virtual ~PeerServiceIfSingletonFactory() {}

  virtual PeerServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) override {
    return iface_.get();
  }
  virtual void releaseHandler(PeerServiceIf* /* handler */) override {}

 protected:
  ::std::shared_ptr<PeerServiceIf> iface_;
};

class PeerServiceNull : virtual public PeerServiceIf {
 public:
  virtual ~PeerServiceNull() {}
  void Register(RegisterResponse& /* _return */, const RegisterRequest& /* request */) override {
    return;
  }
  void UnRegister(UnRegisterResponse& /* _return */, const UnRegisterRequest& /* request */) override {
    return;
  }
  void SendHeartBeat(HeartBeatResponse& /* _return */, const HeartBeatRequest& /* request */) override {
    return;
  }
  void SyncLog(SyncLogResponse& /* _return */, const SyncLogRequest& /* request */) override {
    return;
  }
  void ChangeRole(ChangeRoleResponse& /* _return */, const ChangeRoleRequest& /* request */) override {
    return;
  }
  void NewLeader(NewLeaderResponse& /* _return */, const NewLeaderRequest& /* request */) override {
    return;
  }
};

typedef struct _PeerService_Register_args__isset {
  _PeerService_Register_args__isset() : request(false) {}
  bool request :1;
} _PeerService_Register_args__isset;

class PeerService_Register_args {
 public:

  PeerService_Register_args(const PeerService_Register_args&);
  PeerService_Register_args& operator=(const PeerService_Register_args&);
  PeerService_Register_args() noexcept {
  }

  virtual ~PeerService_Register_args() noexcept;
  RegisterRequest request;

  _PeerService_Register_args__isset __isset;

  void __set_request(const RegisterRequest& val);

  bool operator == (const PeerService_Register_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const PeerService_Register_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_Register_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class PeerService_Register_pargs {
 public:


  virtual ~PeerService_Register_pargs() noexcept;
  const RegisterRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_Register_result__isset {
  _PeerService_Register_result__isset() : success(false) {}
  bool success :1;
} _PeerService_Register_result__isset;

class PeerService_Register_result {
 public:

  PeerService_Register_result(const PeerService_Register_result&);
  PeerService_Register_result& operator=(const PeerService_Register_result&);
  PeerService_Register_result() noexcept {
  }

  virtual ~PeerService_Register_result() noexcept;
  RegisterResponse success;

  _PeerService_Register_result__isset __isset;

  void __set_success(const RegisterResponse& val);

  bool operator == (const PeerService_Register_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const PeerService_Register_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_Register_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_Register_presult__isset {
  _PeerService_Register_presult__isset() : success(false) {}
  bool success :1;
} _PeerService_Register_presult__isset;

class PeerService_Register_presult {
 public:


  virtual ~PeerService_Register_presult() noexcept;
  RegisterResponse* success;

  _PeerService_Register_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _PeerService_UnRegister_args__isset {
  _PeerService_UnRegister_args__isset() : request(false) {}
  bool request :1;
} _PeerService_UnRegister_args__isset;

class PeerService_UnRegister_args {
 public:

  PeerService_UnRegister_args(const PeerService_UnRegister_args&);
  PeerService_UnRegister_args& operator=(const PeerService_UnRegister_args&);
  PeerService_UnRegister_args() noexcept {
  }

  virtual ~PeerService_UnRegister_args() noexcept;
  UnRegisterRequest request;

  _PeerService_UnRegister_args__isset __isset;

  void __set_request(const UnRegisterRequest& val);

  bool operator == (const PeerService_UnRegister_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const PeerService_UnRegister_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_UnRegister_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class PeerService_UnRegister_pargs {
 public:


  virtual ~PeerService_UnRegister_pargs() noexcept;
  const UnRegisterRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_UnRegister_result__isset {
  _PeerService_UnRegister_result__isset() : success(false) {}
  bool success :1;
} _PeerService_UnRegister_result__isset;

class PeerService_UnRegister_result {
 public:

  PeerService_UnRegister_result(const PeerService_UnRegister_result&);
  PeerService_UnRegister_result& operator=(const PeerService_UnRegister_result&);
  PeerService_UnRegister_result() noexcept {
  }

  virtual ~PeerService_UnRegister_result() noexcept;
  UnRegisterResponse success;

  _PeerService_UnRegister_result__isset __isset;

  void __set_success(const UnRegisterResponse& val);

  bool operator == (const PeerService_UnRegister_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const PeerService_UnRegister_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_UnRegister_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_UnRegister_presult__isset {
  _PeerService_UnRegister_presult__isset() : success(false) {}
  bool success :1;
} _PeerService_UnRegister_presult__isset;

class PeerService_UnRegister_presult {
 public:


  virtual ~PeerService_UnRegister_presult() noexcept;
  UnRegisterResponse* success;

  _PeerService_UnRegister_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _PeerService_SendHeartBeat_args__isset {
  _PeerService_SendHeartBeat_args__isset() : request(false) {}
  bool request :1;
} _PeerService_SendHeartBeat_args__isset;

class PeerService_SendHeartBeat_args {
 public:

  PeerService_SendHeartBeat_args(const PeerService_SendHeartBeat_args&);
  PeerService_SendHeartBeat_args& operator=(const PeerService_SendHeartBeat_args&);
  PeerService_SendHeartBeat_args() noexcept {
  }

  virtual ~PeerService_SendHeartBeat_args() noexcept;
  HeartBeatRequest request;

  _PeerService_SendHeartBeat_args__isset __isset;

  void __set_request(const HeartBeatRequest& val);

  bool operator == (const PeerService_SendHeartBeat_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const PeerService_SendHeartBeat_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_SendHeartBeat_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class PeerService_SendHeartBeat_pargs {
 public:


  virtual ~PeerService_SendHeartBeat_pargs() noexcept;
  const HeartBeatRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_SendHeartBeat_result__isset {
  _PeerService_SendHeartBeat_result__isset() : success(false) {}
  bool success :1;
} _PeerService_SendHeartBeat_result__isset;

class PeerService_SendHeartBeat_result {
 public:

  PeerService_SendHeartBeat_result(const PeerService_SendHeartBeat_result&);
  PeerService_SendHeartBeat_result& operator=(const PeerService_SendHeartBeat_result&);
  PeerService_SendHeartBeat_result() noexcept {
  }

  virtual ~PeerService_SendHeartBeat_result() noexcept;
  HeartBeatResponse success;

  _PeerService_SendHeartBeat_result__isset __isset;

  void __set_success(const HeartBeatResponse& val);

  bool operator == (const PeerService_SendHeartBeat_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const PeerService_SendHeartBeat_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_SendHeartBeat_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_SendHeartBeat_presult__isset {
  _PeerService_SendHeartBeat_presult__isset() : success(false) {}
  bool success :1;
} _PeerService_SendHeartBeat_presult__isset;

class PeerService_SendHeartBeat_presult {
 public:


  virtual ~PeerService_SendHeartBeat_presult() noexcept;
  HeartBeatResponse* success;

  _PeerService_SendHeartBeat_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _PeerService_SyncLog_args__isset {
  _PeerService_SyncLog_args__isset() : request(false) {}
  bool request :1;
} _PeerService_SyncLog_args__isset;

class PeerService_SyncLog_args {
 public:

  PeerService_SyncLog_args(const PeerService_SyncLog_args&);
  PeerService_SyncLog_args& operator=(const PeerService_SyncLog_args&);
  PeerService_SyncLog_args() noexcept {
  }

  virtual ~PeerService_SyncLog_args() noexcept;
  SyncLogRequest request;

  _PeerService_SyncLog_args__isset __isset;

  void __set_request(const SyncLogRequest& val);

  bool operator == (const PeerService_SyncLog_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const PeerService_SyncLog_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_SyncLog_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class PeerService_SyncLog_pargs {
 public:


  virtual ~PeerService_SyncLog_pargs() noexcept;
  const SyncLogRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_SyncLog_result__isset {
  _PeerService_SyncLog_result__isset() : success(false) {}
  bool success :1;
} _PeerService_SyncLog_result__isset;

class PeerService_SyncLog_result {
 public:

  PeerService_SyncLog_result(const PeerService_SyncLog_result&);
  PeerService_SyncLog_result& operator=(const PeerService_SyncLog_result&);
  PeerService_SyncLog_result() noexcept {
  }

  virtual ~PeerService_SyncLog_result() noexcept;
  SyncLogResponse success;

  _PeerService_SyncLog_result__isset __isset;

  void __set_success(const SyncLogResponse& val);

  bool operator == (const PeerService_SyncLog_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const PeerService_SyncLog_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_SyncLog_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_SyncLog_presult__isset {
  _PeerService_SyncLog_presult__isset() : success(false) {}
  bool success :1;
} _PeerService_SyncLog_presult__isset;

class PeerService_SyncLog_presult {
 public:


  virtual ~PeerService_SyncLog_presult() noexcept;
  SyncLogResponse* success;

  _PeerService_SyncLog_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _PeerService_ChangeRole_args__isset {
  _PeerService_ChangeRole_args__isset() : request(false) {}
  bool request :1;
} _PeerService_ChangeRole_args__isset;

class PeerService_ChangeRole_args {
 public:

  PeerService_ChangeRole_args(const PeerService_ChangeRole_args&);
  PeerService_ChangeRole_args& operator=(const PeerService_ChangeRole_args&);
  PeerService_ChangeRole_args() noexcept {
  }

  virtual ~PeerService_ChangeRole_args() noexcept;
  ChangeRoleRequest request;

  _PeerService_ChangeRole_args__isset __isset;

  void __set_request(const ChangeRoleRequest& val);

  bool operator == (const PeerService_ChangeRole_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const PeerService_ChangeRole_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_ChangeRole_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class PeerService_ChangeRole_pargs {
 public:


  virtual ~PeerService_ChangeRole_pargs() noexcept;
  const ChangeRoleRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_ChangeRole_result__isset {
  _PeerService_ChangeRole_result__isset() : success(false) {}
  bool success :1;
} _PeerService_ChangeRole_result__isset;

class PeerService_ChangeRole_result {
 public:

  PeerService_ChangeRole_result(const PeerService_ChangeRole_result&);
  PeerService_ChangeRole_result& operator=(const PeerService_ChangeRole_result&);
  PeerService_ChangeRole_result() noexcept {
  }

  virtual ~PeerService_ChangeRole_result() noexcept;
  ChangeRoleResponse success;

  _PeerService_ChangeRole_result__isset __isset;

  void __set_success(const ChangeRoleResponse& val);

  bool operator == (const PeerService_ChangeRole_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const PeerService_ChangeRole_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_ChangeRole_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_ChangeRole_presult__isset {
  _PeerService_ChangeRole_presult__isset() : success(false) {}
  bool success :1;
} _PeerService_ChangeRole_presult__isset;

class PeerService_ChangeRole_presult {
 public:


  virtual ~PeerService_ChangeRole_presult() noexcept;
  ChangeRoleResponse* success;

  _PeerService_ChangeRole_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _PeerService_NewLeader_args__isset {
  _PeerService_NewLeader_args__isset() : request(false) {}
  bool request :1;
} _PeerService_NewLeader_args__isset;

class PeerService_NewLeader_args {
 public:

  PeerService_NewLeader_args(const PeerService_NewLeader_args&);
  PeerService_NewLeader_args& operator=(const PeerService_NewLeader_args&);
  PeerService_NewLeader_args() noexcept {
  }

  virtual ~PeerService_NewLeader_args() noexcept;
  NewLeaderRequest request;

  _PeerService_NewLeader_args__isset __isset;

  void __set_request(const NewLeaderRequest& val);

  bool operator == (const PeerService_NewLeader_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const PeerService_NewLeader_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_NewLeader_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class PeerService_NewLeader_pargs {
 public:


  virtual ~PeerService_NewLeader_pargs() noexcept;
  const NewLeaderRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_NewLeader_result__isset {
  _PeerService_NewLeader_result__isset() : success(false) {}
  bool success :1;
} _PeerService_NewLeader_result__isset;

class PeerService_NewLeader_result {
 public:

  PeerService_NewLeader_result(const PeerService_NewLeader_result&);
  PeerService_NewLeader_result& operator=(const PeerService_NewLeader_result&);
  PeerService_NewLeader_result() noexcept {
  }

  virtual ~PeerService_NewLeader_result() noexcept;
  NewLeaderResponse success;

  _PeerService_NewLeader_result__isset __isset;

  void __set_success(const NewLeaderResponse& val);

  bool operator == (const PeerService_NewLeader_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const PeerService_NewLeader_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PeerService_NewLeader_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _PeerService_NewLeader_presult__isset {
  _PeerService_NewLeader_presult__isset() : success(false) {}
  bool success :1;
} _PeerService_NewLeader_presult__isset;

class PeerService_NewLeader_presult {
 public:


  virtual ~PeerService_NewLeader_presult() noexcept;
  NewLeaderResponse* success;

  _PeerService_NewLeader_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class PeerServiceClient : virtual public PeerServiceIf {
 public:
  PeerServiceClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  PeerServiceClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void Register(RegisterResponse& _return, const RegisterRequest& request) override;
  void send_Register(const RegisterRequest& request);
  void recv_Register(RegisterResponse& _return);
  void UnRegister(UnRegisterResponse& _return, const UnRegisterRequest& request) override;
  void send_UnRegister(const UnRegisterRequest& request);
  void recv_UnRegister(UnRegisterResponse& _return);
  void SendHeartBeat(HeartBeatResponse& _return, const HeartBeatRequest& request) override;
  void send_SendHeartBeat(const HeartBeatRequest& request);
  void recv_SendHeartBeat(HeartBeatResponse& _return);
  void SyncLog(SyncLogResponse& _return, const SyncLogRequest& request) override;
  void send_SyncLog(const SyncLogRequest& request);
  void recv_SyncLog(SyncLogResponse& _return);
  void ChangeRole(ChangeRoleResponse& _return, const ChangeRoleRequest& request) override;
  void send_ChangeRole(const ChangeRoleRequest& request);
  void recv_ChangeRole(ChangeRoleResponse& _return);
  void NewLeader(NewLeaderResponse& _return, const NewLeaderRequest& request) override;
  void send_NewLeader(const NewLeaderRequest& request);
  void recv_NewLeader(NewLeaderResponse& _return);
 protected:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class PeerServiceProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  ::std::shared_ptr<PeerServiceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext) override;
 private:
  typedef  void (PeerServiceProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_Register(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_UnRegister(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_SendHeartBeat(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_SyncLog(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_ChangeRole(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_NewLeader(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  PeerServiceProcessor(::std::shared_ptr<PeerServiceIf> iface) :
    iface_(iface) {
    processMap_["Register"] = &PeerServiceProcessor::process_Register;
    processMap_["UnRegister"] = &PeerServiceProcessor::process_UnRegister;
    processMap_["SendHeartBeat"] = &PeerServiceProcessor::process_SendHeartBeat;
    processMap_["SyncLog"] = &PeerServiceProcessor::process_SyncLog;
    processMap_["ChangeRole"] = &PeerServiceProcessor::process_ChangeRole;
    processMap_["NewLeader"] = &PeerServiceProcessor::process_NewLeader;
  }

  virtual ~PeerServiceProcessor() {}
};

class PeerServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  PeerServiceProcessorFactory(const ::std::shared_ptr< PeerServiceIfFactory >& handlerFactory) noexcept :
      handlerFactory_(handlerFactory) {}

  ::std::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo) override;

 protected:
  ::std::shared_ptr< PeerServiceIfFactory > handlerFactory_;
};

class PeerServiceMultiface : virtual public PeerServiceIf {
 public:
  PeerServiceMultiface(std::vector<std::shared_ptr<PeerServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~PeerServiceMultiface() {}
 protected:
  std::vector<std::shared_ptr<PeerServiceIf> > ifaces_;
  PeerServiceMultiface() {}
  void add(::std::shared_ptr<PeerServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void Register(RegisterResponse& _return, const RegisterRequest& request) override {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->Register(_return, request);
    }
    ifaces_[i]->Register(_return, request);
    return;
  }

  void UnRegister(UnRegisterResponse& _return, const UnRegisterRequest& request) override {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->UnRegister(_return, request);
    }
    ifaces_[i]->UnRegister(_return, request);
    return;
  }

  void SendHeartBeat(HeartBeatResponse& _return, const HeartBeatRequest& request) override {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->SendHeartBeat(_return, request);
    }
    ifaces_[i]->SendHeartBeat(_return, request);
    return;
  }

  void SyncLog(SyncLogResponse& _return, const SyncLogRequest& request) override {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->SyncLog(_return, request);
    }
    ifaces_[i]->SyncLog(_return, request);
    return;
  }

  void ChangeRole(ChangeRoleResponse& _return, const ChangeRoleRequest& request) override {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->ChangeRole(_return, request);
    }
    ifaces_[i]->ChangeRole(_return, request);
    return;
  }

  void NewLeader(NewLeaderResponse& _return, const NewLeaderRequest& request) override {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->NewLeader(_return, request);
    }
    ifaces_[i]->NewLeader(_return, request);
    return;
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class PeerServiceConcurrentClient : virtual public PeerServiceIf {
 public:
  PeerServiceConcurrentClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot, std::shared_ptr< ::apache::thrift::async::TConcurrentClientSyncInfo> sync) : sync_(sync)
{
    setProtocol(prot);
  }
  PeerServiceConcurrentClient(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot, std::shared_ptr< ::apache::thrift::async::TConcurrentClientSyncInfo> sync) : sync_(sync)
{
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(std::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, std::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void Register(RegisterResponse& _return, const RegisterRequest& request) override;
  int32_t send_Register(const RegisterRequest& request);
  void recv_Register(RegisterResponse& _return, const int32_t seqid);
  void UnRegister(UnRegisterResponse& _return, const UnRegisterRequest& request) override;
  int32_t send_UnRegister(const UnRegisterRequest& request);
  void recv_UnRegister(UnRegisterResponse& _return, const int32_t seqid);
  void SendHeartBeat(HeartBeatResponse& _return, const HeartBeatRequest& request) override;
  int32_t send_SendHeartBeat(const HeartBeatRequest& request);
  void recv_SendHeartBeat(HeartBeatResponse& _return, const int32_t seqid);
  void SyncLog(SyncLogResponse& _return, const SyncLogRequest& request) override;
  int32_t send_SyncLog(const SyncLogRequest& request);
  void recv_SyncLog(SyncLogResponse& _return, const int32_t seqid);
  void ChangeRole(ChangeRoleResponse& _return, const ChangeRoleRequest& request) override;
  int32_t send_ChangeRole(const ChangeRoleRequest& request);
  void recv_ChangeRole(ChangeRoleResponse& _return, const int32_t seqid);
  void NewLeader(NewLeaderResponse& _return, const NewLeaderRequest& request) override;
  int32_t send_NewLeader(const NewLeaderRequest& request);
  void recv_NewLeader(NewLeaderResponse& _return, const int32_t seqid);
 protected:
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  std::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
  std::shared_ptr< ::apache::thrift::async::TConcurrentClientSyncInfo> sync_;
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

} // namespace

#endif