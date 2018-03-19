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
var equip = {}

func _init():
	for s in range(G.STAT_NAMES.size()):
		stats[s] = Stat.new()

func apply_mods(add, mult):
	if add != null:
		for s in add:
			assert(s in stats)
			stats[s].add += add[s]
	if mult != null:
		for s in mult:
			assert(s in stats)
			stats[s].mult += mult[s] - 1

func remove_mods(add, mult):
	if add != null:
		for s in add:
			assert(s in stats)
			stats[s].add -= add[s]
	if mult != null:
		for s in mult:
			assert(s in stats)
			stats[s].mult -= mult[s] - 1

func equip(slot, item):
	if slot in equip:
		var old_item = equip[slot]
		remove_child(old_item)
		equip.erase(slot)
		ItemDB.push_item(old_item)
	if item == null:
		return
	assert(item is G.Equip)
	assert(slot in item.types)
	if slot == G.EQUIP_PRIMARY and item.two_handed:
		equip(G.EQUIP_OFFHAND, null)
	if slot == G.EQUIP_OFFHAND and G.EQUIP_PRIMARY in equip:
		if equip[G.EQUIP_PRIMARY].two_handed:
			equip(G.EQUIP_PRIMARY, null)
	equip[slot] = item
	add_child(item)
