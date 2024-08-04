#pragma once

#include "helper.h"

namespace Packet::IDs {
constexpr PacketId KeepAlive      = 0x00;
constexpr PacketId Login          = 0x01;
constexpr PacketId Handshake      = 0x02;
constexpr PacketId ChatMessage    = 0x03;
constexpr PacketId TimeUpdate     = 0x04;
constexpr PacketId EntityEquip    = 0x05;
constexpr PacketId SpawnPos       = 0x06;
constexpr PacketId EntityUse      = 0x07;
constexpr PacketId HealthUpdate   = 0x08;
constexpr PacketId Respawn        = 0x09;
constexpr PacketId Player         = 0x0a;
constexpr PacketId PlayerPos      = 0x0b;
constexpr PacketId PlayerLook     = 0x0c;
constexpr PacketId PlayerPnL      = 0x0d;
constexpr PacketId PlayerDig      = 0x0e;
constexpr PacketId BlockPlace     = 0x0f;
constexpr PacketId HoldChange     = 0x10;
constexpr PacketId UseBed         = 0x11;
constexpr PacketId Animation      = 0x12;
constexpr PacketId EntityAct      = 0x13;
constexpr PacketId PlayerSpawn    = 0x14;
constexpr PacketId PickupSpawn    = 0x15;
constexpr PacketId CollectItem    = 0x16;
constexpr PacketId SpawnVeh       = 0x17;
constexpr PacketId SpawnMob       = 0x18;
constexpr PacketId Painting       = 0x19;
constexpr PacketId UpdStance      = 0x1b;
constexpr PacketId EntityVel      = 0x1c;
constexpr PacketId EntityDestroy  = 0x1d;
constexpr PacketId EntityIdle     = 0x1e;
constexpr PacketId EntityRelMov   = 0x1f;
constexpr PacketId EntityLook     = 0x20;
constexpr PacketId EntityLookRM   = 0x21;
constexpr PacketId EntitySetPos   = 0x22;
constexpr PacketId EntityStatus   = 0x26;
constexpr PacketId EntityAttach   = 0x27;
constexpr PacketId EntityMeta     = 0x28;
constexpr PacketId PreChunk       = 0x32;
constexpr PacketId MapChunk       = 0x33;
constexpr PacketId MultiBlockChg  = 0x34;
constexpr PacketId BlockChg       = 0x35;
constexpr PacketId BlockAction    = 0x36;
constexpr PacketId Explosion      = 0x3c;
constexpr PacketId SoundEffect    = 0x3d;
constexpr PacketId SetState       = 0x46;
constexpr PacketId Thunderbolt    = 0x47;
constexpr PacketId NewWindow      = 0x64;
constexpr PacketId CloseWindow    = 0x65;
constexpr PacketId ClickWindow    = 0x66;
constexpr PacketId SetSlot        = 0x67;
constexpr PacketId ItemsWindow    = 0x68;
constexpr PacketId UpdateWindow   = 0x69;
constexpr PacketId TransactWindow = 0x6a;
constexpr PacketId SignUpdate     = 0x82;
constexpr PacketId ItemData       = 0x83;
constexpr PacketId IncrementStat  = 0xc8;
constexpr PacketId Disconnect     = 0xff;
} // namespace Packet::IDs
