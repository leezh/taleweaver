extends "res://system/item.gd"

var type = G.EQUIP_WEAPON
var light_weapon = false
var two_handed = false
var mod_add = {}
var mod_mult = {}

func on_equip(owner, slot):
	if slot == G.EQUIP_WEAPON and two_handed:
		owner.equip(G.EQUIP_OFFHAND, null)
	if slot == G.EQUIP_OFFHAND and G.EQUIP_WEAPON in owner.equipment:
		if G.item(owner.equipment[G.EQUIP_WEAPON]).two_handed:
			owner.equip(G.EQUIP_WEAPON, null)
	owner.apply_mods(mod_add, mod_mult)

func on_unequip(owner, slot):
	owner.remove_mods(mod_add, mod_mult)
