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
	if slot in equip:
		var old_item = equip[slot]
		equip.erase(slot)
		remove_mods(old_item.mod_add, old_item.mod_mult)
		old_item.on_unequip()
		ItemDB.push_item(old_item)
	if item == null:
		return
	assert(item is G.Equip)
	if slot == G.EQUIP_WEAPON and item.two_handed:
		equip(G.EQUIP_OFFHAND, null)
	if slot == G.EQUIP_OFFHAND and G.EQUIP_WEAPON in equip:
		if equip[G.EQUIP_WEAPON].two_handed:
			equip(G.EQUIP_WEAPON, null)
	equip[slot] = item
	apply_mods(item.mod_add, item.mod_mult)
	item.on_equip()
