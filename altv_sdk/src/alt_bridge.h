#pragma once

#define ALT_SERVER_API

#include <memory>
#include <utility>
#include "shared.h"
#include "runtime.h"

using ResourceOnRemoveBaseObjectCallback = shared::ResourceOnRemoveBaseObjectCallback;
using ResourceStartCallback = shared::ResourceStartCallback;

using StdStringPtr = std::unique_ptr<std::string>;
using StdStringVector = std::vector<std::string>;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

void set_alt_core(alt::ICore* core) {
    alt::ICore::SetInstance(core);
}

alt::ICore* get_alt_core() {
    return &alt::ICore::Instance();
}

alt::IScriptRuntime* create_script_runtime() {
    return new RustRuntime();
}

void register_script_runtime(
    alt::ICore* core,
    std::string resource_type,
    alt::IScriptRuntime* runtime
) {
    core->RegisterScriptRuntime(resource_type, runtime);
    RustRuntime::set_instance(static_cast<RustRuntime*>(runtime));
}

// mvalue

class MValueWrapper {
public:
    std::shared_ptr<alt::MValueConst> ptr;

    MValueWrapper clone() {
        MValueWrapper instance;
        instance.ptr = this->ptr;

        return instance;
    }
};

class MValueMutWrapper {
public:
    std::shared_ptr<alt::MValue> ptr;

    MValueMutWrapper clone() {
        MValueMutWrapper instance;
        instance.ptr = this->ptr;

        return instance;
    }
};

MValueWrapper convert_mvalue_mut_wrapper_to_const(MValueMutWrapper mut_wrapper) {
    MValueWrapper wrapper;
    // is this even legal?
    wrapper.ptr = std::make_shared<alt::MValueConst>(*mut_wrapper.ptr);
    mut_wrapper.ptr = nullptr;
    return wrapper;
}

using MValueDictPair = std::pair<std::string, MValueWrapper>;

class MValueDictPairWrapper {
public:
    std::shared_ptr<MValueDictPair> ptr;

    MValueDictPairWrapper clone() {
        MValueDictPairWrapper instance;
        instance.ptr = this->ptr;

        return instance;
    }
};

class PlayerPtrWrapper {
public:
    std::shared_ptr<alt::IPlayer*> ptr;

    PlayerPtrWrapper clone() {
        PlayerPtrWrapper instance;
        instance.ptr = this->ptr;

        return instance;
    }
};

using PlayerVector = std::vector<PlayerPtrWrapper>;

PlayerVector create_player_vec() {
    PlayerVector vec;
    return vec;
}

void push_to_player_vec(PlayerVector& player_vec, alt::IPlayer* player) {
    PlayerPtrWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::IPlayer*>(player);
    player_vec.push_back(wrapper.clone());
}

using MValueWrapperVec = std::vector<MValueWrapper>;

MValueWrapperVec create_mvalue_vec() {
    MValueWrapperVec vec;
    return vec;
}

void push_to_mvalue_vec(MValueWrapperVec& mvalue_vec, MValueWrapper mvalue) {
    mvalue_vec.push_back(mvalue.clone());
}

u8 get_mvalue_type(MValueWrapper mvalue) {
    return static_cast<u8>(mvalue.ptr->Get()->GetType());
}

bool get_mvalue_bool(MValueWrapper mvalue) {
    assert(mvalue.ptr->Get()->GetType() == alt::IMValue::Type::BOOL);
    return mvalue.ptr->As<alt::IMValueBool>().Get()->Value();
}

f64 get_mvalue_double(MValueWrapper mvalue) {
    assert(mvalue.ptr->Get()->GetType() == alt::IMValue::Type::DOUBLE);
    return mvalue.ptr->As<alt::IMValueDouble>().Get()->Value();
}

std::string get_mvalue_string(MValueWrapper mvalue) {
    assert(mvalue.ptr->Get()->GetType() == alt::IMValue::Type::STRING);
    return mvalue.ptr->As<alt::IMValueString>().Get()->Value();
}

i64 get_mvalue_int(MValueWrapper mvalue) {
    assert(mvalue.ptr->Get()->GetType() == alt::IMValue::Type::INT);
    return mvalue.ptr->As<alt::IMValueInt>().Get()->Value();
}

u64 get_mvalue_uint(MValueWrapper mvalue) {
    assert(mvalue.ptr->Get()->GetType() == alt::IMValue::Type::UINT);
    return mvalue.ptr->As<alt::IMValueUInt>().Get()->Value();
}

MValueWrapperVec get_mvalue_list(MValueWrapper mvalue) {
    assert(mvalue.ptr->Get()->GetType() == alt::IMValue::Type::LIST);

    auto list = mvalue.ptr->As<alt::IMValueList>().Get();
    auto size = list->GetSize();

    auto mvalue_vec = create_mvalue_vec();

    for (alt::Size i = 0; i < size; ++i) {
        MValueWrapper wrapper;
        wrapper.ptr = std::make_shared<alt::MValueConst>(list->Get(i));
        mvalue_vec.push_back(wrapper.clone());
    }

    return mvalue_vec;
}

std::vector<MValueDictPairWrapper> get_mvalue_dict(MValueWrapper mvalue) {
    assert(mvalue.ptr->Get()->GetType() == alt::IMValue::Type::DICT);
    auto dict = mvalue.ptr->As<alt::IMValueDict>().Get();

    std::vector<MValueDictPairWrapper> vec;

    for (auto it = dict->Begin(); it; it = dict->Next()) {
        MValueWrapper value_wrapper;
        value_wrapper.ptr = std::make_shared<alt::MValueConst>(it->GetValue());

        MValueDictPairWrapper pair;
        pair.ptr = std::make_shared<MValueDictPair>(std::pair { it->GetKey(), value_wrapper.clone() });
        vec.push_back(pair.clone());
    }

    return vec;
}

std::string get_mvalue_dict_pair_key(const MValueDictPairWrapper& pair) {
    return pair.ptr->first;
}

MValueWrapper get_mvalue_dict_pair_value(const MValueDictPairWrapper& pair) {
    return pair.ptr->second.clone();
}

alt::IBaseObject* get_mvalue_base_object(MValueWrapper mvalue) {
    assert(mvalue.ptr->Get()->GetType() == alt::IMValue::Type::BASE_OBJECT);
    return mvalue.ptr->As<alt::IMValueBaseObject>().Get()->RawValue();
}

MValueWrapper create_mvalue_bool(bool value) {
    MValueWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValueConst>(alt::ICore::Instance().CreateMValueBool(value));
    return wrapper;
}

MValueWrapper create_mvalue_double(f64 value) {
    MValueWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValueConst>(alt::ICore::Instance().CreateMValueDouble(value));
    return wrapper;
}

MValueWrapper create_mvalue_string(std::string value) {
    MValueWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValueConst>(alt::ICore::Instance().CreateMValueString(value));
    return wrapper;
}

MValueWrapper create_mvalue_nil() {
    MValueWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValueConst>(alt::ICore::Instance().CreateMValueNil());
    return wrapper;
}

MValueWrapper create_mvalue_int(i64 value) {
    MValueWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValueConst>(alt::ICore::Instance().CreateMValueInt(value));
    return wrapper;
}

MValueWrapper create_mvalue_uint(u64 value) {
    MValueWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValueConst>(alt::ICore::Instance().CreateMValueUInt(value));
    return wrapper;
}

MValueWrapper create_mvalue_list(MValueWrapperVec mvalue_vec) {
    auto mvalue_list = alt::ICore::Instance().CreateMValueList();
    auto size = mvalue_vec.size();

    for (size_t i = 0; i < size; ++i) {
        mvalue_list->PushConst(*(mvalue_vec[i].ptr));
    }

    MValueWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValueConst>(mvalue_list);
    return wrapper;
}

MValueMutWrapper create_mvalue_dict() {
    MValueMutWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValue>(alt::ICore::Instance().CreateMValueDict());
    return wrapper;
}

void push_to_mvalue_dict(MValueMutWrapper& dict, std::string key, MValueWrapper mvalue) {
    assert(dict.ptr->Get()->GetType() == alt::IMValue::Type::DICT);
    dict.ptr->As<alt::IMValueDict>().Get()->SetConst(key, *(mvalue.ptr));
}

MValueWrapper create_mvalue_base_object(alt::IBaseObject* value) {
    MValueWrapper wrapper;
    wrapper.ptr = std::make_shared<alt::MValueConst>(alt::ICore::Instance().CreateMValueBaseObject(value));
    return wrapper;
}

// events

void trigger_local_event(std::string event_name, MValueWrapperVec mvalue_vec) {
    alt::MValueArgs args;
    auto size = mvalue_vec.size();

    for (alt::Size i = 0; i < size; ++i) {
        args.Push(*(mvalue_vec[i].ptr));
    }

    alt::ICore::Instance().TriggerLocalEvent(event_name, args);
}

void trigger_client_event(alt::IPlayer* player, std::string event_name, MValueWrapperVec mvalue_vec) {
    alt::MValueArgs args;
    auto size = mvalue_vec.size();

    for (alt::Size i = 0; i < size; ++i) {
        args.Push(*(mvalue_vec[i].ptr));
    }

    alt::ICore::Instance().TriggerClientEvent(player, event_name, args);
}

void trigger_client_event_for_some(PlayerVector players, std::string event_name, MValueWrapperVec mvalue_vec) {
    alt::MValueArgs args;
    auto size = mvalue_vec.size();

    for (alt::Size i = 0; i < size; ++i) {
        args.Push(*(mvalue_vec[i].ptr));
    }

    std::vector<alt::IPlayer*> normal_players_vec;
    for (auto& player : players)
    {
        normal_players_vec.push_back(*player.ptr);
    }

    alt::ICore::Instance().TriggerClientEvent(normal_players_vec, event_name, args);
}

void trigger_client_event_for_all(std::string event_name, MValueWrapperVec mvalue_vec) {
    alt::MValueArgs args;
    auto size = mvalue_vec.size();

    for (alt::Size i = 0; i < size; ++i) {
        args.Push(*(mvalue_vec[i].ptr));
    }

    alt::ICore::Instance().TriggerClientEventForAll(event_name, args);
}

void toggle_event_type(u16 event_type, bool state) {
    alt::ICore::Instance().ToggleEvent(static_cast<alt::CEvent::Type>(event_type), state);
}

u16 get_event_type(const alt::CEvent* event) {
    return static_cast<u16>(event->GetType());
}

alt::IPlayer* get_event_player_target(const alt::CEvent* event) {
    auto type = event->GetType();
    switch (type) {
    case alt::CEvent::Type::PLAYER_CONNECT:
        return static_cast<const alt::CPlayerConnectEvent*>(event)->GetTarget();
    case alt::CEvent::Type::PLAYER_DISCONNECT:
        return static_cast<const alt::CPlayerDisconnectEvent*>(event)->GetTarget();
    case alt::CEvent::Type::CLIENT_SCRIPT_EVENT:
        return static_cast<const alt::CClientScriptEvent*>(event)->GetTarget();
    default:
        alt::ICore::Instance().LogError(
            "get_event_player_target unknown event type: " +
            std::to_string(static_cast<u16>(type))
        );
        return nullptr;
    }
}

StdStringPtr get_event_reason(const alt::CEvent* event) {
    auto type = event->GetType();

    switch (type) {
    case alt::CEvent::Type::PLAYER_DISCONNECT:
        return std::make_unique<std::string>(std::string{
            static_cast<const alt::CPlayerDisconnectEvent*>(event)->GetReason()
        });
    default:
        alt::ICore::Instance().LogError(
            "get_event_reason unknown event type: " +
            std::to_string(static_cast<u16>(type))
        );
        return std::make_unique<std::string>(std::string{ "" });
        break;
    }
}

StdStringPtr get_event_console_command_name(const alt::CEvent* event) {
    assert(event->GetType() == alt::CEvent::Type::CONSOLE_COMMAND_EVENT);
    return std::make_unique<std::string>(std::string{
        static_cast<const alt::CConsoleCommandEvent*>(event)->GetName()
    });
}

std::unique_ptr<StdStringVector> get_event_console_command_args(const alt::CEvent* event) {
    assert(event->GetType() == alt::CEvent::Type::CONSOLE_COMMAND_EVENT);
    return std::make_unique<StdStringVector>(
        static_cast<const alt::CConsoleCommandEvent*>(event)->GetArgs()
    );
}

StdStringPtr get_any_script_event_name(const alt::CEvent* event) {
    auto type = event->GetType();
    switch (type) {
    case alt::CEvent::Type::SERVER_SCRIPT_EVENT:
        return std::make_unique<std::string>(std::string {
            static_cast<const alt::CServerScriptEvent*>(event)->GetName()
        });
    case alt::CEvent::Type::CLIENT_SCRIPT_EVENT:
        return std::make_unique<std::string>(std::string {
            static_cast<const alt::CClientScriptEvent*>(event)->GetName()
        });
    default:
        assert(("get_any_script_event_name expected server or client script event", false));
    }
}

MValueWrapperVec get_any_script_event_args(const alt::CEvent* event) {
    auto type = event->GetType();
    alt::MValueArgs args;
    switch (type) {
    case alt::CEvent::Type::SERVER_SCRIPT_EVENT:
        args = static_cast<const alt::CServerScriptEvent*>(event)->GetArgs();
        break;
    case alt::CEvent::Type::CLIENT_SCRIPT_EVENT:
        args = static_cast<const alt::CClientScriptEvent*>(event)->GetArgs();
        break;
    default:
        assert(("get_any_script_event_args expected server or client script event", false));
    }

    auto mvalue_vec = create_mvalue_vec();
    auto size = args.GetSize();

    for (alt::Size i = 0; i < size; ++i) {
        MValueWrapper wrapper;
        wrapper.ptr = std::make_shared<alt::MValueConst>(args[i]);
        mvalue_vec.push_back(wrapper.clone());
    }

    return mvalue_vec;
}

void log_colored(std::string str) {
    return alt::ICore::Instance().LogColored(str);
}

void log_error(std::string str) {
    return alt::ICore::Instance().LogError(str);
}

void log_warn(std::string str) {
    return alt::ICore::Instance().LogWarning(str);
}

// entity conversions
alt::IEntity* convert_base_object_to_entity(alt::IBaseObject* base_object) {
    return dynamic_cast<alt::IEntity*>(base_object);
}

// vehicle conversions
alt::IBaseObject* convert_vehicle_to_base_object(alt::IVehicle* vehicle) {
    return static_cast<alt::IBaseObject*>(vehicle);
}

alt::IVehicle* convert_base_object_to_vehicle(alt::IBaseObject* base_object) {
    return dynamic_cast<alt::IVehicle*>(base_object);
}

alt::IEntity* convert_vehicle_to_entity(alt::IVehicle* vehicle) {
    return static_cast<alt::IEntity*>(vehicle);
}

// player conversions
alt::IBaseObject* convert_player_to_base_object(alt::IPlayer* player) {
    return static_cast<alt::IBaseObject*>(player);
}

alt::IPlayer* convert_base_object_to_player(alt::IBaseObject* base_object) {
    return dynamic_cast<alt::IPlayer*>(base_object);
}

alt::IEntity* convert_player_to_entity(alt::IPlayer* player) {
    return static_cast<alt::IEntity*>(player);
}

// vehicle
alt::IVehicle* create_vehicle(
    u32 model,
    f32 x, f32 y, f32 z,
    f32 rx, f32 ry, f32 rz
) {
    return alt::ICore::Instance().CreateVehicle(model, { x, y, z }, { rx, ry, rz });
}

void set_vehicle_primary_color(alt::IVehicle* vehicle, u8 color) {
    vehicle->SetPrimaryColor(color);
}

u8 get_vehicle_primary_color(const alt::IVehicle* vehicle) {
    return vehicle->GetPrimaryColor();
}

u16 get_entity_id(alt::IEntity* entity) {
    assert(entity != nullptr);
    return entity->GetID();
}

u32 get_entity_model(const alt::IEntity* entity) {
    return entity->GetModel();
}

void destroy_base_object(alt::IBaseObject* base_object) {
    if (!base_object) {
        alt::ICore::Instance().LogError("destroy_base_object nullptr base_object");
        return;
    }

    auto type = base_object->GetType();

    alt::ICore::Instance().DestroyBaseObject(base_object);
}

u8 get_base_object_type(const alt::IBaseObject* base_object) {
    if (!base_object) {
        alt::ICore::Instance().LogError("get_base_object_type nullptr base_object");
        return 255;
    }
    return static_cast<u8>(base_object->GetType());
}

// player
StdStringPtr get_player_name(const alt::IPlayer* player) {
    return std::make_unique<std::string>(player->GetName());
}

void spawn_player(alt::IPlayer* player, f32 x, f32 y, f32 z) {
    player->Spawn({ x, y, z }, 0);
}

void set_player_model(alt::IPlayer* player, u32 model) {
    player->SetModel(model);
}
