extends Node

var party = [null, null, null]
const PARTY_LIMIT = 3
var inventory = {}

func add_character(name):
	if has_node(name):
		return get_node(name)
	var Class = load("res://world/characters/" + name + ".gd")
	var new_char = Class.new()
	new_char.name = name
	add_child(new_char)
	return new_char

func add_item(name, count=1):
	if name in inventory:
		inventory[name] += count
	else:
		G.item(name)
		inventory[name] = count

func remove_item(name, count=1):
	if name in inventory:
		if inventory[name] > count:
			inventory[name] -= count
		else:
			inventory.erase(name)
