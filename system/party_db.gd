extends Node

var characters = {}
var party = [null, null, null]

const PARTY_LIMIT = 3

func add_character(name):
	if name in characters:
		return characters[name]
	var Class = load("res://world/characters/" + name + ".gd")
	var c = Class.new()
	c.name = name
	characters[name] = c
	add_child(c)
	return c
