extends Node

var party = {}
var inventory = {}
var inventory_sort = []
var equipment = {}
var equipment_sort = []

func set_party_member(index, character):
	if index > 0 and index in party:
		party[index].position = -1
		party.erase(index)
	if character == null:
		return
	assert(character is G.Character)
	if index < 0:
		index = 0
		while index in party:
			index += 1
	if character.position >= 0:
		party.erase(index)
	party[index] = character
	character.position = index

func get_members():
	return party.values()

func get_random_member():
	var list = []
	for index in party:
		if party[index].hp > 0:
			list.append(index)
	return list[rand_range(0, list.size())]

func add_item(name, count=1):
	if name in inventory:
		inventory[name] += count
	else:
		inventory[name] = count
		inventory_sort.append(name)

func remove_item(name, count=1):
	if name in inventory:
		if inventory[name] > count:
			inventory[name] -= count
		else:
			inventory.erase(name)
			inventory_sort.erase(name)

func add_equipment(name, count=1):
	if name in equipment:
		equipment[name] += count
	else:
		equipment[name] = count
		equipment_sort.append(name)

func remove_equipment(name, count=1):
	if name in equipment:
		if equipment[name] > count:
			equipment[name] -= count
		else:
			equipment.erase(name)
			equipment_sort.erase(name)
