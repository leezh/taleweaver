extends Node

class Stat:
	var base = 0
	var add = 0
	var mult = 1.0
	var value setget , get_value
	func get_value():
		return (base + add) * mult

const STAT_NAMES = ["max_hp", "max_sp", "p_attack", "p_defence", "m_attack", "m_defence", "speed"]
var stats = {}
var hp = 0
var sp = 0

func _init():
	for s in STAT_NAMES:
		stats[s] = Stat.new()
