#include <dllentry.h>
#include <playerdb.h>
#include <hook.h>
#include <log.h>
#include <Net/NetworkIdentifier.h>
#include <Packet/MobEquipmentPacket.h>
#include <Item/ItemStack.h>
#include <Item/Item.h>

DEF_LOGGER("BAC");

void dllenter() {}
void dllexit() {}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
    NetworkIdentifier *netid, void *packet) {
  auto &db = Mod::PlayerDatabase::GetInstance().GetData().get<NetworkIdentifier>();
  auto it  = db.find(*netid);
  if (it != db.end()) {
    auto level = it->player->getCommandPermissionLevel();
    if (level > CommandPermissionLevel::Normal) {
      original(this, netid, packet);
    } else {
      LOGI("%s try to edit command block") % it->name;
    }
  }
}

TClasslessInstanceHook(
    void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVMobEquipmentPacket@@@Z",
    NetworkIdentifier *netid, MobEquipmentPacket *packet) {
  // is offhand container
  if (packet->containerId == 119) {
    // detect if it can be offhand
    if (auto item = packet->item.getItem(); item && !item->getAllowOffhand()) {
      auto &db = Mod::PlayerDatabase::GetInstance().GetData().get<NetworkIdentifier>();
      auto it  = db.find(*netid);
      if (it != db.end()) { LOGI("%s try to set illegal item to offhand") % it->name; }
      return;
    }
  }
  original(this, netid, packet);
}