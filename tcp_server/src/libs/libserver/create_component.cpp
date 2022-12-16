#include "create_component.h"
#include "common.h"
#include "component_factory.h"
#include "packet.h"
#include "entity.h"
#include "system_manager.h"
#include "thread.h"

template <typename... TArgs, size_t... Index>
IComponent* ComponentFactoryEx(EntitySystem* pEntitySystem, std::string className, uint64 sn, const std::tuple<TArgs...>& args, std::index_sequence<Index...>)
{
	return pEntitySystem->AddComponentByName(className, sn, std::get<Index>(args)...);
}

template<size_t ICount>
struct DynamicCall
{
	template <typename... TArgs>
	static IComponent* Invoke(EntitySystem* pEntitySystem, const std::string classname, uint64 sn, std::tuple<TArgs...> t1, google::protobuf::RepeatedPtrField<::Net::CreateComponentParam>& params)
	{
		if (params.empty())
		{
			return ComponentFactoryEx(pEntitySystem, classname, sn, t1, std::make_index_sequence<sizeof...(TArgs)>());
		}

		Net::CreateComponentParam param = (*(params.begin()));
		params.erase(params.begin());

		if (param.type() == Net::CreateComponentParam::Int)
		{
			auto t2 = std::tuple_cat(t1, std::make_tuple(param.int_param()));
			return DynamicCall<ICount - 1>::Invoke(pEntitySystem, classname, sn, t2, params);
		}

		if (param.type() == Net::CreateComponentParam::String)
		{
			auto t2 = std::tuple_cat(t1, std::make_tuple(param.string_param()));
			return DynamicCall<ICount - 1>::Invoke(pEntitySystem, classname, sn, t2, params);
		}

		if (param.type() == Net::CreateComponentParam::UInt64)
		{
			auto t2 = std::tuple_cat(t1, std::make_tuple(param.uint64_param()));
			return DynamicCall<ICount - 1>::Invoke(pEntitySystem, classname, sn, t2, params);
		}

		return nullptr;
	}
};

template<>
struct DynamicCall<0>
{
	template <typename... TArgs>
	static IComponent* Invoke(EntitySystem* pEntitySystem, const std::string classname, uint64 sn, std::tuple<TArgs...> t1, google::protobuf::RepeatedPtrField<::Net::CreateComponentParam>& params)
	{
		return nullptr;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CreateComponentC::Awake()
{
	auto pMsgSystem = GetSystemManager()->GetMessageSystem();

	pMsgSystem->RegisterFunction(this, Net::MsgId::MI_CreateComponent, BindFunP1(this, &CreateComponentC::HandleCreateComponent));
	pMsgSystem->RegisterFunction(this, Net::MsgId::MI_RemoveComponent, BindFunP1(this, &CreateComponentC::HandleRemoveComponent));

	pMsgSystem->RegisterFunction(this, Net::MsgId::MI_CreateSystem, BindFunP1(this, &CreateComponentC::HandleCreateSystem));

}

void CreateComponentC::BackToPool()
{

}

#define MaxDynamicCall 4
void CreateComponentC::HandleCreateComponent(Packet* pPacket) const
{
	Net::CreateComponent proto = pPacket->ParseToProto<Net::CreateComponent>();
	const std::string className = proto.class_name();
	uint64 sn = proto.sn();

	if (proto.params_size() > MaxDynamicCall)
	{
		LOG_ERROR(" !!!! CreateComponent failed. className:" << className.c_str() << " params size > " << MaxDynamicCall);
		return;
	}

	auto params = proto.params();
	const auto pObj = DynamicCall<MaxDynamicCall>::Invoke(GetSystemManager()->GetEntitySystem(), className, sn, std::make_tuple(), params);
	if (pObj == nullptr)
	{
		LOG_ERROR(" !!!! CreateComponent failed. className:" << className.c_str());
	}

	//std::cout << "CreateComponent. name:" << className << std::endl;
}

void CreateComponentC::HandleRemoveComponent(Packet* pPacket)
{
	Net::RemoveComponent proto = pPacket->ParseToProto<Net::RemoveComponent>();
	//GetEntitySystem( )->RemoveFromSystem( proto.sn( ) );
}

void CreateComponentC::HandleCreateSystem(Packet* pPacket)
{
	Net::CreateSystem proto = pPacket->ParseToProto<Net::CreateSystem>();
	const std::string systemName = proto.system_name();

	const auto pThread = static_cast<Thread*>(GetSystemManager());
	if (int(pThread->GetThreadType()) != proto.thread_type())
		return;

	GetSystemManager()->AddSystem(systemName);
}
