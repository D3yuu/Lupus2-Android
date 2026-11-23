#include "StdAfx.h"
static PyObject* ikashopNoop(PyObject*, PyObject*) { return Py_BuildNone(); }
static PyObject* ikashopFalse(PyObject*, PyObject*) { return Py_BuildValue("i", 0); }
static PyObject* ikashopList(PyObject*, PyObject*) { return PyList_New(0); }
static PyObject* ikashopTuple(PyObject*, PyObject*) { return Py_BuildValue("(ii)", 0, 0); }
void initikashop() {
 static PyMethodDef methods[] = {
  { "CheckShopSpace", ikashopFalse, METH_VARARGS },
  { "ClearFoundShopMap", ikashopNoop, METH_VARARGS },
  { "GetBusinessBoard", ikashopNoop, METH_VARARGS },
  { "GetFilterHistory", ikashopList, METH_VARARGS },
  { "GetFirstEmptyCell", ikashopFalse, METH_VARARGS },
  { "GetItemHash", ikashopFalse, METH_VARARGS },
  { "GetItemNames", ikashopList, METH_VARARGS },
  { "GetItemPriceCache", ikashopFalse, METH_VARARGS },
  { "GetNotifications", ikashopList, METH_VARARGS },
  { "GetProjectPosition", ikashopTuple, METH_VARARGS },
  { "IsFoundShopFromSearchItem", ikashopFalse, METH_VARARGS },
  { "LoadNotifications", ikashopNoop, METH_VARARGS },
  { "RegisterFilterHistory", ikashopNoop, METH_VARARGS },
  { "RegisterItemPriceCache", ikashopNoop, METH_VARARGS },
  { "RegisterNotification", ikashopNoop, METH_VARARGS },
  { "SendAddItem", ikashopNoop, METH_VARARGS },
  { "SendAuctionAddOffer", ikashopNoop, METH_VARARGS },
  { "SendAuctionCreate", ikashopNoop, METH_VARARGS },
  { "SendAuctionExitFrom", ikashopNoop, METH_VARARGS },
  { "SendAuctionListRequest", ikashopNoop, METH_VARARGS },
  { "SendAuctionOpenAuction", ikashopNoop, METH_VARARGS },
  { "SendBuyItem", ikashopNoop, METH_VARARGS },
  { "SendChangeName", ikashopNoop, METH_VARARGS },
  { "SendCloseMyShopBoard", ikashopNoop, METH_VARARGS },
  { "SendCloseShopGuestBoard", ikashopNoop, METH_VARARGS },
  { "SendEditItem", ikashopNoop, METH_VARARGS },
  { "SendFilterRequest", ikashopNoop, METH_VARARGS },
  { "SendFindMyShop", ikashopNoop, METH_VARARGS },
  { "SendForceCloseShop", ikashopNoop, METH_VARARGS },
  { "SendMoveShopEntity", ikashopNoop, METH_VARARGS },
  { "SendMyAuctionCancel", ikashopNoop, METH_VARARGS },
  { "SendMyAuctionClose", ikashopNoop, METH_VARARGS },
  { "SendMyAuctionOpen", ikashopNoop, METH_VARARGS },
  { "SendNotificationListClose", ikashopNoop, METH_VARARGS },
  { "SendNotificationListRequest", ikashopNoop, METH_VARARGS },
  { "SendOfferAccept", ikashopNoop, METH_VARARGS },
  { "SendOfferCancel", ikashopNoop, METH_VARARGS },
  { "SendOfferCreate", ikashopNoop, METH_VARARGS },
  { "SendOfferListRequest", ikashopNoop, METH_VARARGS },
  { "SendOffersListClose", ikashopNoop, METH_VARARGS },
  { "SendOnClickPacket", ikashopNoop, METH_VARARGS },
  { "SendOpenShopOwner", ikashopNoop, METH_VARARGS },
  { "SendRandomSearchFillRequest", ikashopNoop, METH_VARARGS },
  { "SendRemoveAllItem", ikashopNoop, METH_VARARGS },
  { "SendRemoveItem", ikashopNoop, METH_VARARGS },
  { "SendRequestEdit", ikashopNoop, METH_VARARGS },
  { "SendSafeboxClose", ikashopNoop, METH_VARARGS },
  { "SendSafeboxGetItem", ikashopNoop, METH_VARARGS },
  { "SendSafeboxGetValutes", ikashopNoop, METH_VARARGS },
  { "SendSafeboxOpen", ikashopNoop, METH_VARARGS },
  { "SendSearchItem", ikashopNoop, METH_VARARGS },
  { "SendShopCreate", ikashopNoop, METH_VARARGS },
  { "SendShopReopen", ikashopNoop, METH_VARARGS },
  { "SetBusinessBoard", ikashopNoop, METH_VARARGS },
  { "SetSearchShopBoard", ikashopNoop, METH_VARARGS },
  { "txt", ikashopNoop, METH_VARARGS },
  { NULL, NULL, 0 }
 };
 PyObject* m=Py_InitModule("ikashop", methods);
 PyModule_AddIntConstant(m, "FILTER_ATTRIBUTE_NUM", 0);
 PyModule_AddIntConstant(m, "FILTER_NAME_MAX_LEN", 1);
 PyModule_AddIntConstant(m, "NOTIFICATION_BUYER_AUCTION_BUY", 2);
 PyModule_AddIntConstant(m, "NOTIFICATION_BUYER_AUCTION_LOST", 3);
 PyModule_AddIntConstant(m, "NOTIFICATION_BUYER_AUCTION_RAISE", 4);
 PyModule_AddIntConstant(m, "NOTIFICATION_BUYER_OFFER_ACCEPTED", 5);
 PyModule_AddIntConstant(m, "NOTIFICATION_BUYER_OFFER_DENIED", 6);
 PyModule_AddIntConstant(m, "NOTIFICATION_BUYER_OFFER_EXPIRED", 7);
 PyModule_AddIntConstant(m, "NOTIFICATION_SELLER_AUCTION_EXPIRED", 8);
 PyModule_AddIntConstant(m, "NOTIFICATION_SELLER_AUCTION_NEW_OFFER", 9);
 PyModule_AddIntConstant(m, "NOTIFICATION_SELLER_AUCTION_SOLD", 10);
 PyModule_AddIntConstant(m, "NOTIFICATION_SELLER_ITEM_EXPIRED", 11);
 PyModule_AddIntConstant(m, "NOTIFICATION_SELLER_NEW_OFFER_ON_ITEM", 12);
 PyModule_AddIntConstant(m, "NOTIFICATION_SELLER_OFFER_CANCELLED", 13);
 PyModule_AddIntConstant(m, "NOTIFICATION_SELLER_SELLER_SOLD_ITEM", 14);
 PyModule_AddIntConstant(m, "NOTIFICATION_SELLER_SHOP_EXPIRED", 15);
 PyModule_AddIntConstant(m, "SAFEBOX_GRID_HEIGHT", 16);
 PyModule_AddIntConstant(m, "SAFEBOX_GRID_WIDTH", 17);
 PyModule_AddIntConstant(m, "SAFEBOX_PAGE_COUNT", 18);
 PyModule_AddIntConstant(m, "SHOP_CATEGORY_MAX_SUB", 19);
 PyModule_AddIntConstant(m, "SHOP_GRID_HEIGHT", 20);
 PyModule_AddIntConstant(m, "SHOP_GRID_WIDTH", 21);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_ARMOR", 22);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_BOOKS", 23);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_DRAGON_VOCHER", 24);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_FISHING", 25);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_HERBALISM", 26);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_HORSE", 27);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_JEWELRY", 28);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_MINING", 29);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_POLYMORPH", 30);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_REFINE", 31);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_SOULSTONE", 32);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_SPECIAL", 33);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_CATEGORY_WEAPON", 34);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_ARMOR_BODY", 35);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_ARMOR_HEAD", 36);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_ARMOR_SHIELD", 37);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_ASSASSIN_0", 38);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_ASSASSIN_1", 39);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_FISHING_FISH", 40);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_FISHING_FISH_COOK", 41);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_FISHING_FISH_OTHER", 42);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_AUTOPOTION", 43);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_DEW_DEFENSIVE", 44);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_DEW_OFFENSIVE", 45);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_DEW_POWER", 46);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_JUICE_DEFENSIVE", 47);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_JUICE_OFFENSIVE", 48);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_JUICE_POWER", 49);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_OTHER_POTION", 50);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_PRIMARY", 51);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_RECIPE_DEFENSIVE", 52);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_RECIPE_OFFENSIVE", 53);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_RECIPE_OTHER", 54);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_RECIPE_POWER", 55);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_SPECIAL", 56);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_WATER_DEFENSIVE", 57);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_WATER_OFFENSIVE", 58);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HERB_WATER_POWER", 59);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HORSE_LEARN", 60);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_HORSE_OTHER", 61);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_JEWELRY_BOOTS", 62);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_JEWELRY_EAR", 63);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_JEWELRY_NECK", 64);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_JEWELRY_WRIST", 65);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_MINING_MELT", 66);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_MINING_ORE", 67);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_PASSIVE_SKILL", 68);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_DESERT1", 69);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_DESERT2", 70);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_END", 71);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_HWANG", 72);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_M1", 73);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_M2", 74);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_OATH", 75);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_ORC", 76);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_PEARL", 77);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_SNOW", 78);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_REFINE_SPECIAL", 79);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SHAMAN_0", 80);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SHAMAN_1", 81);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SOULSTONE_0", 82);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SOULSTONE_1", 83);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SOULSTONE_2", 84);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SOULSTONE_3", 85);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SOULSTONE_4", 86);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SPECIAL_CHARACTER", 87);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SPECIAL_DRAGON_VOUCHER", 88);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SPECIAL_LOOTBOX", 89);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SPECIAL_OTHER", 90);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SPECIAL_QUEST", 91);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SPECIAL_REFINE", 92);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SPECIAL_TOITEM", 93);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SURA_0", 94);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_SURA_1", 95);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_WARRIOR_0", 96);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_WARRIOR_1", 97);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_WEAPON_BELL", 98);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_WEAPON_BOW", 99);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_WEAPON_DAGGER", 100);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_WEAPON_FAN", 101);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_WEAPON_ONEHAND", 102);
 PyModule_AddIntConstant(m, "SHOP_SEARCH_SUB_WEAPON_TWOHAND", 103);
 PyModule_AddIntConstant(m, "YANG_PER_CHEQUE", 104);
}
