extends Node

func _ready():
	print(PlayerParty.get_node("godette").stats[G.STAT_PATTACK].value)
