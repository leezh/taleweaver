extends Node

var party = {}
var inventory = {}
var inventory_sort = []

func add_character(name):
	if has_node(name):
		return get_node(name)
	var Class = load("res://world/characters/" + name + ".gd")
	assert(Class is Script)
	var new_char = Class.new()
	assert(new_char is G.Character)
	new_char.name = name
	add_child(new_char)
	return new_char

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

func add_item(name, count=1):
	if name in inventory:
		inventory[name] += count
	else:
		G.item(name)
		inventory[name] = count
		inventory_sort.append(name)

func remove_item(name, count=1):
	if name in inventory:
		if inventory[name] > count:
			inventory[name] -= count
		else:
			inventory.erase(name)
			inventory_sort.erase(name)
