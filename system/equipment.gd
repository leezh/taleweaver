extends "res://system/item.gd"

var type = G.EQUIP_WEAPON
var light_weapon = false
var two_handed = false
var mod_add = {}
var mod_mult = {}

func on_equip(owner, slot, put_back):
	if slot == G.EQUIP_WEAPON and two_handed:
		owner.equip(G.EQUIP_OFFHAND, null, put_back)
	if slot == G.EQUIP_OFFHAND and G.EQUIP_WEAPON in owner.equipment:
		if G.item(owner.equipment[G.EQUIP_WEAPON]).two_handed:
			owner.equip(G.EQUIP_WEAPON, null, put_back)
	owner.apply_mods(mod_add, mod_mult)

func on_unequip(owner, slot, put_back):
	owner.remove_mods(mod_add, mod_mult)
