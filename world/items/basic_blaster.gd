extends "res://system/equip.gd"

func _init():
	type = G.EQUIP_WEAPON
	two_handed = true
	mod_add[G.STAT_PATTACK] = 20
