extends Node

class Stat:
	var base = 0
	var add = 0
	var mult = 1.0
	var value setget , get_value
	func get_value():
		return (base + add) * mult

var stats = {}
var hp = 0
var sp = 0
var equipment = {}

func _init():
	for s in range(G.STAT_NAMES.size()):
		stats[s] = Stat.new()

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

func equip(slot, item):
	if slot in equipment:
		var current = equipment[slot]
		equipment.erase(slot)
		G.item(current).on_unequip(self, slot)
		get_parent().add_item(current)
	if item == null:
		return
	assert(G.item(item) is G.Equipment)
	equipment[slot] = item
	G.item(item).on_equip(self, slot)
