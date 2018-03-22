tool
extends Node

const G = preload("res://system/defs.gd")

class Stat:
	var base = 0
	var add = 0
	var mult = 1.0
	var value setget , get_value
	func get_value():
		return (base + add) * mult

var display_name = ""
var player_enabled = false
var portrait = null
var item_db
var battle_actor = ""
var stats = {}
var hp = 0
var sp = 0
var equipment = {}
var position = -1

func _init():
	for s in range(G.STAT_NAMES.size()):
		stats[s] = Stat.new()

func _get_property_list():
	var list = [{
			"name": "display_name",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNATIONALIZED
		},{
			"name": "player_enabled",
			"type": TYPE_BOOL,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		},{
			"name": "portrait",
			"type": TYPE_OBJECT,
			"hint": PROPERTY_HINT_RESOURCE_TYPE,
			"hint_string": "Texture",
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		},{
			"name": "battle_actor",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_FILE,
			"hint_string": ResourceLoader.get_recognized_extensions_for_type("PackedScene").join(","),
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		}]
	for s in range(G.STAT_NAMES.size()):
		list.append({
			"name": "base/" + G.STAT_NAMES[s],
			"type": TYPE_INT,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		})
	for e in range(G.EQUIP_SLOTS.size()):
		list.append({
			"name": "equip/" + G.EQUIP_SLOTS[e],
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		})
	return list

func _get(property):
	var path = property.split("/")
	match path[0]:
		"display_name":
			return display_name
		"player_enabled":
			return player_enabled
		"portrait":
			return portrait
		"battle_actor":
			return battle_actor
		"base":
			var s = G.STAT_NAMES.find(path[1])
			return stats[s].base
		"equip":
			var e = G.EQUIP_SLOTS.find(path[1])
			return equipment[e] if e in equipment else ""

func _set(property, value):
	var path = property.split("/")
	match path[0]:
		"display_name":
			display_name = value
		"player_enabled":
			player_enabled = value
		"portrait":
			portrait = value
		"battle_actor":
			battle_actor = value
		"base":
			var s = G.STAT_NAMES.find(path[1])
			stats[s].base = value
		"equip":
			var e = G.EQUIP_SLOTS.find(path[1])
			if Engine.editor_hint or not is_inside_tree():
				if value:
					equipment[e] = value
				else:
					equipment.erase(e)
			else:
				equip(e, value, false)

func _ready():
	item_db = get_tree().root.get_node("G")
	for slot in equipment:
		item_db.item(equipment[slot]).on_equip(self, slot, false)

func apply_mods(add, mult):
	for s in add:
		assert(s in stats)
		stats[s].add += add[s]
	for s in mult:
		assert(s in stats)
		stats[s].mult += mult[s] - 1

func remove_mods(add, mult):
	for s in add:
		assert(s in stats)
		stats[s].add -= add[s]
	for s in mult:
		assert(s in stats)
		stats[s].mult -= mult[s] - 1

func equip(slot, item, put_back=true):
	if slot in equipment:
		var current = equipment[slot]
		equipment.erase(slot)
		item_db.item(current).on_unequip(self, slot, put_back)
		if put_back:
			get_parent().add_item(current)
	if not item or not item_db.item(item) is G.Equipment:
		return
	equipment[slot] = item
	item_db.item(item).on_equip(self, slot, put_back)
