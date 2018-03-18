extends Node

var party = [null, null, null]
const PARTY_LIMIT = 3

func add_character(name):
	if has_node(name):
		return get_node(name)
	var Class = load("res://world/characters/" + name + ".gd")
	var new_char = Class.new()
	new_char.name = name
	add_child(new_char)
	return new_char
