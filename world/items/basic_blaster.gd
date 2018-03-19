extends "res://system/equip.gd"

var add_mods = {
	G.STAT_PATTACK: 20
	}

func _init():
	types = [G.EQUIP_PRIMARY]
	two_handed = true

func _enter_tree():
	assert(get_parent() is G.Character)
	get_parent().apply_mods(add_mods, null)

func _exit_tree():
	get_parent().remove_mods(add_mods, null)
