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

func _init():
	for s in range(G.STAT_NAMES.size()):
		stats[s] = Stat.new()
