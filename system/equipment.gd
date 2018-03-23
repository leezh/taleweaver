tool
extends Node

const G = preload("res://system/defs.gd")

var display_name = ""
var description = ""
var icon = null
var battle_model = ""
var type = G.EQUIP_WEAPON
var light_weapon = false
var two_handed = false
var mod_add = {}
var mod_mult = {}

func _get_property_list():
	var list = [{
			"name": "display_name",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNATIONALIZED
		},{
			"name": "description",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_MULTILINE_TEXT,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNATIONALIZED
		},{
			"name": "icon",
			"type": TYPE_OBJECT,
			"hint": PROPERTY_HINT_RESOURCE_TYPE,
			"hint_string": "Texture",
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		},{
			"name": "battle_model",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_FILE,
			"hint_string": ResourceLoader.get_recognized_extensions_for_type("PackedScene").join(","),
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		},{
			"name": "equipment_type",
			"type": TYPE_INT,
			"hint": PROPERTY_HINT_ENUM,
			"hint_string": "Weapon,Offhand,Armour,Accessory",
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		},{
			"name": "light_weapon",
			"type": TYPE_BOOL,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		},{
			"name": "two_handed",
			"type": TYPE_BOOL,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		}]
	for s in range(G.STAT_NAMES.size()):
		list.append({
			"name": "add/" + G.STAT_NAMES[s],
			"type": TYPE_INT,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		})
		list.append({
			"name": "multiply/" + G.STAT_NAMES[s],
			"type": TYPE_REAL,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONONE
		})
	return list

func _get(property):
	var path = property.split("/")
	match path[0]:
		"display_name":
			return display_name
		"description":
			return description
		"icon":
			return icon
		"battle_model":
			return battle_model
		"equipment_type":
			return type
		"light_weapon":
			return light_weapon
		"two_handed":
			return two_handed
		"add":
			var s = G.STAT_NAMES.find(path[1])
			return mod_add[s] if s in mod_add else 0
		"multiply":
			var s = G.STAT_NAMES.find(path[1])
			return mod_mult[s] if s in mod_mult else 1.0

func _set(property, value):
	var path = property.split("/")
	match path[0]:
		"display_name":
			display_name = value
		"description":
			description = value
		"icon":
			icon = value
		"battle_model":
			battle_model = value
		"equipment_type":
			type = value
		"light_weapon":
			light_weapon = value
		"two_handed":
			two_handed = value
		"add":
			var s = G.STAT_NAMES.find(path[1])
			if value != 0:
				mod_add[s] = value
			else:
				mod_add.erase(s)
		"multiply":
			var s = G.STAT_NAMES.find(path[1])
			if value != 0:
				mod_mult[s] = value
			else:
				mod_mult.erase(s)

func on_equip(owner, slot, put_back):
	if slot == G.EQUIP_WEAPON and two_handed:
		owner.equip(G.EQUIP_OFFHAND, null, put_back)
	if slot == G.EQUIP_OFFHAND and G.EQUIP_WEAPON in owner.equipment:
		if G.item(owner.equipment[G.EQUIP_WEAPON]).two_handed:
			owner.equip(G.EQUIP_WEAPON, null, put_back)
	owner.apply_mods(mod_add, mod_mult)

func on_unequip(owner, slot, put_back):
	owner.remove_mods(mod_add, mod_mult)
