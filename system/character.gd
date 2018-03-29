tool
extends Node

const G = preload("res://system/defs.gd")
var equip_db

class Stat:
	var base = 0
	var add = 0
	var mult = 1.0
	var value setget , get_value
	func get_value():
		return (base + add) * mult

class Attack:
	var power = 0
	var element = 0

export(String) var display_name = ""
export(bool) var player_enabled = false
export(Texture) var portrait = null
export(String, FILE) var battle_actor = ""
var stats = {}
var equipment = {}
var hp = 0
var sp = 0
var position = -1

func _init():
	for s in G.STATS:
		stats[G.STATS[s]] = Stat.new()

func _get_property_list():
	var list = []
	for stat in G.STATS:
		list.append({
			"name": "base/" + stat,
			"type": TYPE_INT
		})
	for slot in G.EQUIP:
		list.append({
			"name": "equip/" + slot,
			"type": TYPE_STRING
		})
	return list

func _get(property):
	var path = property.split("/")
	match path[0]:
		"base":
			var s = G.STATS[path[1]]
			return stats[s].base
		"equip":
			var e = G.EQUIP[path[1]]
			return equipment[e] if e in equipment else ""

func _set(property, value):
	var path = property.split("/")
	match path[0]:
		"base":
			var s = G.STATS[path[1]]
			stats[s].base = value
		"equip":
			var e = G.EQUIP[path[1]]
			if Engine.editor_hint or not is_inside_tree():
				if value:
					equipment[e] = value
				else:
					equipment.erase(e)
			else:
				equip(e, value)

func _ready():
	if Engine.editor_hint:
		return
	equip_db = get_node("/root/EquipDB")
	for slot in equipment:
		equip_db.get_node(equipment[slot]).on_equip(self, slot)

func apply_mods(add, mult):
	for s in add:
		stats[s].add += add[s]
	for s in mult:
		stats[s].mult += mult[s] - 1

func remove_mods(add, mult):
	for s in add:
		stats[s].add -= add[s]
	for s in mult:
		stats[s].mult -= mult[s] - 1

func equip(slot, item):
	if slot in equipment:
		var current = equipment[slot]
		equipment.erase(slot)
		equip_db.get_node(current).on_unequip(self, slot)
	if not item or not equip_db.has_node(item):
		return
	equipment[slot] = item
	equip_db.get_node(item).on_equip(self, slot)

func get_equip(slot):
	if slot in equipment:
		return equip_db.get_node(equipment[slot])
	return null

func check_equip(slot, party):
	if slot in equipment:
		equip_db.get_node(equipment[slot]).on_check_equip(self, slot, party)

func get_attacks():
	var list = []
	for slot in [G.EQUIP.weapon, G.EQUIP.offhand]:
		if not slot in equipment:
			continue
		var item = equip_db.get_node(equipment[slot])
		if item.type != G.EQUIP.weapon:
			continue
		var attack = Attack.new()
		attack.power = stats[G.STAT.p_attack].value
		list.append(attack)
	return list
