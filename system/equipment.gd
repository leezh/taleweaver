tool
extends Node

const G = preload("res://system/defs.gd")

export(Texture) var icon = null
export(String, FILE) var battle_model = ""
export(G.EQUIP) var type = G.EQUIP.weapon
export(bool) var light_weapon = false
export(bool) var two_handed = false
var mod_add = {}
var mod_mult = {}

func _get_property_list():
	var list = []
	for stat in G.STATS:
		list.append({
			"name": "add/" + stat,
			"type": TYPE_INT,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		})
		list.append({
			"name": "multiply/" + stat,
			"type": TYPE_REAL,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONONE
		})
	return list

func _get(property):
	var path = property.split("/")
	match path[0]:
		"add":
			var s = G.STATS[path[1]]
			return mod_add[s] if s in mod_add else 0
		"multiply":
			var s = G.STATS[path[1]]
			return mod_mult[s] if s in mod_mult else 1.0

func _set(property, value):
	var path = property.split("/")
	match path[0]:
		"add":
			var s = G.STATS[path[1]]
			if value != 0:
				mod_add[s] = value
			else:
				mod_add.erase(s)
		"multiply":
			var s = G.STATS[path[1]]
			if value != 0:
				mod_mult[s] = value
			else:
				mod_mult.erase(s)

func on_equip(owner, slot):
	owner.apply_mods(mod_add, mod_mult)

func on_unequip(owner, slot):
	owner.remove_mods(mod_add, mod_mult)

func on_check_equip(owner, slot, party):
	match slot:
		G.EQUIP.weapon:
			if two_handed and G.EQUIP.offhand in owner.equipment:
				party.add_item(owner.equipment[G.EQUIP.offhand])
				owner.equip(G.EQUIP.offhand, null)
		G.EQUIP.offhand:
			if G.EQUIP.weapon in owner.equipment:
				if owner.get_equip(G.EQUIP.weapon).two_handed:
					party.add_item(owner.equipment[G.EQUIP.weapon])
					owner.equip(G.EQUIP.weapon, null)
