tool
extends Node

const G = preload("res://system/defs.gd")

export(Texture) var icon = null
export(int) var cost = 0
export(G.TARGETS) var target_type = G.TARGETS.single
export(bool) var use_in_battle = false
export(bool) var use_in_field = false

func on_battle_use(mode, user, targets):
	pass

func on_field_use(mode, targets):
	pass
