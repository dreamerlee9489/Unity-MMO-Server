// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto_id.proto

#include "proto_id.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
namespace Proto {
}  // namespace Proto
static constexpr ::PROTOBUF_NAMESPACE_ID::Metadata* file_level_metadata_proto_5fid_2eproto = nullptr;
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_proto_5fid_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_proto_5fid_2eproto = nullptr;
const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_proto_5fid_2eproto::offsets[1] = {};
static constexpr ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema* schemas = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::Message* const* file_default_instances = nullptr;

const char descriptor_table_protodef_proto_5fid_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\016proto_id.proto\022\005Proto*\314\026\n\005MsgId\022\010\n\004Non"
  "e\020\000\022\025\n\021MI_NetworkConnect\020\001\022\027\n\023MI_Network"
  "Connected\020\002\022\024\n\020MI_NetworkListen\020\003\022\027\n\023MI_"
  "NetworkListenKey\020\004\022\030\n\024MI_NetworkDisconne"
  "ct\020\n\022\032\n\026MI_NetworkDisconnectEx\020\013\022\037\n\033MI_N"
  "etworkRequestDisconnect\020\024\022\026\n\022MI_RemoveCo"
  "mponent\0202\022\026\n\022MI_CreateComponent\0203\022\023\n\017MI_"
  "CreateSystem\0204\022\013\n\007MI_Ping\020e\022\022\n\016MI_AppReg"
  "ister\020f\022\022\n\016MI_AppInfoSync\020g\022\026\n\022MI_AppInf"
  "oListSync\020h\022\025\n\020C2L_AccountCheck\020\350\007\022\027\n\022C2"
  "L_AccountCheckRs\020\351\007\022!\n\034MI_AccountQueryOn"
  "lineToRedis\020\352\007\022#\n\036MI_AccountQueryOnlineT"
  "oRedisRs\020\353\007\022\031\n\024L2DB_QueryPlayerList\020\362\007\022\033"
  "\n\026L2DB_QueryPlayerListRs\020\363\007\022\023\n\016L2C_Playe"
  "rList\020\364\007\022\025\n\020C2L_CreatePlayer\020\366\007\022\027\n\022C2L_C"
  "reatePlayerRs\020\367\007\022\026\n\021L2DB_CreatePlayer\020\370\007"
  "\022\030\n\023L2DB_CreatePlayerRs\020\371\007\022\025\n\020C2L_Select"
  "Player\020\372\007\022\027\n\022C2L_SelectPlayerRs\020\373\007\022\031\n\024MI"
  "_LoginTokenToRedis\020\376\007\022\033\n\026MI_LoginTokenTo"
  "RedisRs\020\377\007\022\022\n\rL2C_GameToken\020\200\010\022\025\n\020C2G_Lo"
  "ginByToken\020\314\010\022\027\n\022C2G_LoginByTokenRs\020\315\010\022\030"
  "\n\023MI_GameTokenToRedis\020\316\010\022\032\n\025MI_GameToken"
  "ToRedisRs\020\317\010\022\025\n\020G2DB_QueryPlayer\020\320\010\022\027\n\022G"
  "2DB_QueryPlayerRs\020\321\010\022\023\n\016G2C_SyncPlayer\020\322"
  "\010\022\025\n\020G2M_RequestWorld\020\324\010\022\024\n\017G2S_CreateWo"
  "rld\020\325\010\022\020\n\013MI_Teleport\020\326\010\022\025\n\020MI_TeleportA"
  "fter\020\327\010\022\023\n\016G2S_SyncPlayer\020\330\010\022\032\n\025G2S_Requ"
  "estSyncPlayer\020\331\010\022\024\n\017G2DB_SavePlayer\020\332\010\022\025"
  "\n\020G2S_RemovePlayer\020\333\010\022\023\n\016C2G_EnterWorld\020"
  "\334\010\022\023\n\016G2M_QueryWorld\020\335\010\022\025\n\020G2M_QueryWorl"
  "dRs\020\336\010\022\022\n\rMI_CreateTeam\020\261\t\022\022\n\rMI_GlobalC"
  "hat\020\262\t\022\021\n\014MI_WorldChat\020\263\t\022\020\n\013MI_TeamChat"
  "\020\264\t\022\023\n\016MI_PrivateChat\020\265\t\022\023\n\016S2C_EnterWor"
  "ld\020\335\013\022\023\n\016S2G_SyncPlayer\020\336\013\022\026\n\021S2C_AllRol"
  "eAppear\020\337\013\022\026\n\021S2C_RoleDisappear\020\340\013\022\r\n\010C2"
  "S_Move\020\341\013\022\r\n\010S2C_Move\020\342\013\022\024\n\017S2C_PlayerIt"
  "ems\020\343\013\022\024\n\017C2S_PlayerItems\020\344\013\022\034\n\027MI_Broad"
  "castCreateWorld\020\321\017\022!\n\034MI_BroadcastCreate"
  "WorldProxy\020\322\017\022\031\n\024MI_WorldSyncToGather\020\271\027"
  "\022\036\n\031MI_WorldProxySyncToGather\020\272\027\022 \n\033MI_A"
  "ccountSyncOnlineToRedis\020\241\037\022\"\n\035MI_Account"
  "DeleteOnlineToRedis\020\242\037\022\037\n\032MI_PlayerSyncO"
  "nlineToRedis\020\243\037\022!\n\034MI_PlayerDeleteOnline"
  "ToRedis\020\244\037\022\026\n\021MI_RobotSyncState\020\211\'\022\023\n\016MI"
  "_RobotCreate\020\212\'\022\021\n\014MI_HttpBegin\020\220N\022\031\n\024MI"
  "_HttpInnerResponse\020\221N\022\026\n\021MI_HttpRequestB"
  "ad\020\222N\022\030\n\023MI_HttpRequestLogin\020\223N\022\017\n\nMI_Ht"
  "tpEnd\020\203R\022\030\n\023MI_HttpOuterRequest\020\204R\022\031\n\024MI"
  "_HttpOuterResponse\020\205R\022\022\n\014MI_CmdThread\020\241\234"
  "\001\022\017\n\tMI_CmdApp\020\242\234\001\022\026\n\020MI_CmdWorldProxy\020\243"
  "\234\001\022\021\n\013MI_CmdWorld\020\244\234\001\022\022\n\014MI_CmdCreate\020\245\234"
  "\001\022\026\n\020MI_CmdEfficiency\020\246\234\001\022\023\n\rMI_Efficien"
  "cy\020\247\234\001\022\027\n\021C2S_SyncPlayerPos\020\261\352\001\022\027\n\021S2C_S"
  "yncPlayerPos\020\262\352\001\022\027\n\021C2S_SyncPlayerCmd\020\263\352"
  "\001\022\027\n\021S2C_SyncPlayerCmd\020\264\352\001\022\024\n\016C2S_SyncNp"
  "cPos\020\265\352\001\022\024\n\016S2C_SyncNpcPos\020\266\352\001\022\032\n\024S2C_Sy"
  "ncEntityStatus\020\267\352\001\022\026\n\020S2C_SyncFsmState\020\270"
  "\352\001\022\026\n\020C2S_SyncFsmState\020\271\352\001\022\024\n\016C2S_ReqSyn"
  "cNpc\020\272\352\001\022\024\n\016S2C_ReqSyncNpc\020\273\352\001\022\027\n\021C2S_Re"
  "qSyncPlayer\020\274\352\001\022\027\n\021S2C_ReqLinkPlayer\020\275\352\001"
  "\022\030\n\022C2S_PlayerAtkEvent\020\276\352\001\022\030\n\022S2C_Player"
  "AtkEvent\020\277\352\001\022\025\n\017C2S_NpcAtkEvent\020\300\352\001\022\025\n\017S"
  "2C_NpcAtkEvent\020\301\352\001\022\026\n\020S2C_DropItemList\020\302"
  "\352\001\022\026\n\020S2C_SyncBtAction\020\303\352\001\022\026\n\020C2S_SyncBt"
  "Action\020\304\352\001\022\030\n\022C2S_UpdateKnapItem\020\301\270\002\022\030\n\022"
  "C2S_UpdateKnapGold\020\302\270\002\022\027\n\021C2S_GetPlayerK"
  "nap\020\303\270\002\022\027\n\021S2C_GetPlayerKnap\020\304\270\002\022\025\n\017C2C_"
  "ReqJoinTeam\020\321\206\003\022\025\n\017C2C_JoinTeamRes\020\322\206\003\022\031"
  "\n\023C2C_ReqEnterDungeon\020\323\206\003\022\031\n\023C2C_EnterDu"
  "ngeonRes\020\324\206\003\022\020\n\nC2C_ReqPvp\020\325\206\003\022\020\n\nC2C_Pv"
  "pRes\020\326\206\003\022\022\n\014C2C_ReqTrade\020\327\206\003\022\022\n\014C2C_Trad"
  "eRes\020\330\206\003\022\031\n\023C2C_UpdateTradeItem\020\331\206\003\022\023\n\rS"
  "2C_TradeOpen\020\332\206\003\022\024\n\016S2C_TradeClose\020\333\206\003b\006"
  "proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_proto_5fid_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_proto_5fid_2eproto_sccs[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_proto_5fid_2eproto_once;
static bool descriptor_table_proto_5fid_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_proto_5fid_2eproto = {
  &descriptor_table_proto_5fid_2eproto_initialized, descriptor_table_protodef_proto_5fid_2eproto, "proto_id.proto", 2926,
  &descriptor_table_proto_5fid_2eproto_once, descriptor_table_proto_5fid_2eproto_sccs, descriptor_table_proto_5fid_2eproto_deps, 0, 0,
  schemas, file_default_instances, TableStruct_proto_5fid_2eproto::offsets,
  file_level_metadata_proto_5fid_2eproto, 0, file_level_enum_descriptors_proto_5fid_2eproto, file_level_service_descriptors_proto_5fid_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_proto_5fid_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_proto_5fid_2eproto), true);
namespace Proto {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MsgId_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_proto_5fid_2eproto);
  return file_level_enum_descriptors_proto_5fid_2eproto[0];
}
bool MsgId_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 10:
    case 11:
    case 20:
    case 50:
    case 51:
    case 52:
    case 101:
    case 102:
    case 103:
    case 104:
    case 1000:
    case 1001:
    case 1002:
    case 1003:
    case 1010:
    case 1011:
    case 1012:
    case 1014:
    case 1015:
    case 1016:
    case 1017:
    case 1018:
    case 1019:
    case 1022:
    case 1023:
    case 1024:
    case 1100:
    case 1101:
    case 1102:
    case 1103:
    case 1104:
    case 1105:
    case 1106:
    case 1108:
    case 1109:
    case 1110:
    case 1111:
    case 1112:
    case 1113:
    case 1114:
    case 1115:
    case 1116:
    case 1117:
    case 1118:
    case 1201:
    case 1202:
    case 1203:
    case 1204:
    case 1205:
    case 1501:
    case 1502:
    case 1503:
    case 1504:
    case 1505:
    case 1506:
    case 1507:
    case 1508:
    case 2001:
    case 2002:
    case 3001:
    case 3002:
    case 4001:
    case 4002:
    case 4003:
    case 4004:
    case 5001:
    case 5002:
    case 10000:
    case 10001:
    case 10002:
    case 10003:
    case 10499:
    case 10500:
    case 10501:
    case 20001:
    case 20002:
    case 20003:
    case 20004:
    case 20005:
    case 20006:
    case 20007:
    case 30001:
    case 30002:
    case 30003:
    case 30004:
    case 30005:
    case 30006:
    case 30007:
    case 30008:
    case 30009:
    case 30010:
    case 30011:
    case 30012:
    case 30013:
    case 30014:
    case 30015:
    case 30016:
    case 30017:
    case 30018:
    case 30019:
    case 30020:
    case 40001:
    case 40002:
    case 40003:
    case 40004:
    case 50001:
    case 50002:
    case 50003:
    case 50004:
    case 50005:
    case 50006:
    case 50007:
    case 50008:
    case 50009:
    case 50010:
    case 50011:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace Proto
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
