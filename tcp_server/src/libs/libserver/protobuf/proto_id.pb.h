// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto_id.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_proto_5fid_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_proto_5fid_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3009000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3009001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_proto_5fid_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_proto_5fid_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_proto_5fid_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace Proto {

enum MsgId : int {
  None = 0,
  MI_NetworkConnect = 1,
  MI_NetworkConnected = 2,
  MI_NetworkListen = 3,
  MI_NetworkListenKey = 4,
  MI_NetworkDisconnect = 10,
  MI_NetworkDisconnectEx = 11,
  MI_NetworkRequestDisconnect = 20,
  MI_RemoveComponent = 50,
  MI_CreateComponent = 51,
  MI_CreateSystem = 52,
  MI_Ping = 101,
  MI_AppRegister = 102,
  MI_AppInfoSync = 103,
  MI_AppInfoListSync = 104,
  C2L_AccountCheck = 1000,
  C2L_AccountCheckRs = 1001,
  MI_AccountQueryOnlineToRedis = 1002,
  MI_AccountQueryOnlineToRedisRs = 1003,
  L2DB_QueryPlayerList = 1010,
  L2DB_QueryPlayerListRs = 1011,
  L2C_PlayerList = 1012,
  C2L_CreatePlayer = 1014,
  C2L_CreatePlayerRs = 1015,
  L2DB_CreatePlayer = 1016,
  L2DB_CreatePlayerRs = 1017,
  C2L_SelectPlayer = 1018,
  C2L_SelectPlayerRs = 1019,
  MI_LoginTokenToRedis = 1022,
  MI_LoginTokenToRedisRs = 1023,
  L2C_GameToken = 1024,
  C2G_LoginByToken = 1100,
  C2G_LoginByTokenRs = 1101,
  MI_GameTokenToRedis = 1102,
  MI_GameTokenToRedisRs = 1103,
  G2DB_QueryPlayer = 1104,
  G2DB_QueryPlayerRs = 1105,
  G2C_SyncPlayer = 1106,
  G2M_RequestWorld = 1108,
  G2S_CreateWorld = 1109,
  MI_Teleport = 1110,
  MI_TeleportAfter = 1111,
  G2S_SyncPlayer = 1112,
  G2S_RequestSyncPlayer = 1113,
  G2DB_SavePlayer = 1114,
  G2S_RemovePlayer = 1115,
  C2G_EnterWorld = 1116,
  G2M_QueryWorld = 1117,
  G2M_QueryWorldRs = 1118,
  S2C_EnterWorld = 1501,
  S2G_SyncPlayer = 1502,
  S2C_AllRoleAppear = 1503,
  S2C_RoleDisappear = 1504,
  C2S_Move = 1505,
  S2C_Move = 1506,
  S2C_PlayerItems = 1507,
  C2S_PlayerItems = 1508,
  MI_BroadcastCreateWorld = 2001,
  MI_BroadcastCreateWorldProxy = 2002,
  MI_WorldSyncToGather = 3001,
  MI_WorldProxySyncToGather = 3002,
  MI_AccountSyncOnlineToRedis = 4001,
  MI_AccountDeleteOnlineToRedis = 4002,
  MI_PlayerSyncOnlineToRedis = 4003,
  MI_PlayerDeleteOnlineToRedis = 4004,
  MI_RobotSyncState = 5001,
  MI_RobotCreate = 5002,
  MI_HttpBegin = 10000,
  MI_HttpInnerResponse = 10001,
  MI_HttpRequestBad = 10002,
  MI_HttpRequestLogin = 10003,
  MI_HttpEnd = 10499,
  MI_HttpOuterRequest = 10500,
  MI_HttpOuterResponse = 10501,
  MI_CmdThread = 20001,
  MI_CmdApp = 20002,
  MI_CmdWorldProxy = 20003,
  MI_CmdWorld = 20004,
  MI_CmdCreate = 20005,
  MI_CmdEfficiency = 20006,
  MI_Efficiency = 20007,
  C2S_PlayerPushPos = 30001,
  S2C_PlayerPushPos = 30002,
  C2S_PlayerSyncCmd = 30003,
  S2C_PlayerSyncCmd = 30004,
  C2S_EnemyPushPos = 30005,
  S2C_EnemyPushPos = 30006,
  S2C_FsmSyncState = 30007,
  C2S_FsmSyncState = 30008,
  C2S_RequestSyncEnemy = 30009,
  S2C_RequestSyncEnemy = 30010,
  S2C_RequestLinkPlayer = 30011,
  C2S_AtkAnimEvent = 30012,
  S2C_AtkAnimEvent = 30013,
  S2C_DropItemList = 30014,
  C2S_AddItemToKnap = 40001,
  C2S_GetPlayerKnap = 40002,
  S2C_GetPlayerKnap = 40003,
  MsgId_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  MsgId_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool MsgId_IsValid(int value);
constexpr MsgId MsgId_MIN = None;
constexpr MsgId MsgId_MAX = S2C_GetPlayerKnap;
constexpr int MsgId_ARRAYSIZE = MsgId_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MsgId_descriptor();
template<typename T>
inline const std::string& MsgId_Name(T enum_t_value) {
  static_assert(::std::is_same<T, MsgId>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function MsgId_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    MsgId_descriptor(), enum_t_value);
}
inline bool MsgId_Parse(
    const std::string& name, MsgId* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<MsgId>(
    MsgId_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Proto

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::Proto::MsgId> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Proto::MsgId>() {
  return ::Proto::MsgId_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_proto_5fid_2eproto
