#pragma once

#include "helper.h"

namespace Packet::IDs {
constexpr PacketId KeepAlive      = 0x00; // xS +
constexpr PacketId Login          = 0x01; // CS +
constexpr PacketId Handshake      = 0x02; // CS +
constexpr PacketId ChatMessage    = 0x03; // CS +
constexpr PacketId TimeUpdate     = 0x04; // xS +
constexpr PacketId EntityEquip    = 0x05; // xS ~
constexpr PacketId SpawnPos       = 0x06; // xS +
constexpr PacketId EntityUse      = 0x07; // Cx ~
constexpr PacketId PlayerHealth   = 0x08; // xS +
constexpr PacketId PlayerRespawn  = 0x09; // CS +
constexpr PacketId PlayerFall     = 0x0a; // Cx ~
constexpr PacketId PlayerPos      = 0x0b; // Cx +
constexpr PacketId PlayerLook     = 0x0c; // Cx +
constexpr PacketId PlayerPnL      = 0x0d; // Cx +
constexpr PacketId PlayerDig      = 0x0e; // Cx ~
constexpr PacketId BlockPlace     = 0x0f; // Cx +
constexpr PacketId HoldChange     = 0x10; // Cx +
constexpr PacketId PlayerSleep    = 0x11; // Cx -
constexpr PacketId PlayerAnim     = 0x12; // CS +
constexpr PacketId PlayerAction   = 0x13; // Cx ~
constexpr PacketId PlayerSpawn    = 0x14; // xS +
constexpr PacketId PickupSpawn    = 0x15; // xS -
constexpr PacketId CollectItem    = 0x16; // xS -
constexpr PacketId SpawnVeh       = 0x17; // xS -
constexpr PacketId SpawnMob       = 0x18; // xS -
constexpr PacketId SpawnPainting  = 0x19; // xS -
constexpr PacketId UpdStance      = 0x1b; // xx -
constexpr PacketId EntityVel      = 0x1c; // xS +
constexpr PacketId EntityDestroy  = 0x1d; // xS +
constexpr PacketId EntityIdle     = 0x1e; // xx -
constexpr PacketId EntityRelMove  = 0x1f; // xS +
constexpr PacketId EntityLook     = 0x20; // xS +
constexpr PacketId EntityLookRM   = 0x21; // xS +
constexpr PacketId EntitySetPos   = 0x22; // xS +
constexpr PacketId EntityStatus   = 0x26; // xS +
constexpr PacketId EntityAttach   = 0x27; // xS -
constexpr PacketId EntityMeta     = 0x28; // xS +
constexpr PacketId PreChunk       = 0x32; // xS +
constexpr PacketId MapChunk       = 0x33; // xS +
constexpr PacketId MultiBlockChg  = 0x34; // xS -
constexpr PacketId BlockChg       = 0x35; // xS +
constexpr PacketId BlockAction    = 0x36; // xS +
constexpr PacketId Explosion      = 0x3c; // xS -
constexpr PacketId SoundEffect    = 0x3d; // xS ~
constexpr PacketId SetState       = 0x46; // xS -
constexpr PacketId Thunderbolt    = 0x47; // xS ~
constexpr PacketId NewWindow      = 0x64; // xS ~
constexpr PacketId CloseWindow    = 0x65; // CS +
constexpr PacketId ClickWindow    = 0x66; // Cx ~
constexpr PacketId SetSlot        = 0x67; // xS +
constexpr PacketId ItemsWindow    = 0x68; // xS +
constexpr PacketId UpdateWindow   = 0x69; // xS ~
constexpr PacketId TransactWindow = 0x6a; // xS ~
constexpr PacketId SignUpdate     = 0x82; // CS +
constexpr PacketId ItemData       = 0x83; // xS -
constexpr PacketId IncrementStat  = 0xc8; // xS -
constexpr PacketId ConnectionFin  = 0xff; // CS +
} // namespace Packet::IDs
