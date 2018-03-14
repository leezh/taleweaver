extends Node

var characters = {}
var party = [null, null, null]

const PARTY_LIMIT = 3

func add_character(name):
	if name in characters:
		return characters[name]
	var Class = load("res://world/characters/" + name + ".gd")
	var new_char = Class.new()
	new_char.name = name
	characters[name] = new_char
	add_child(new_char)
	return new_char
