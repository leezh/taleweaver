extends Node

func _ready():
	print(PlayerParty.get_node("godette").stats[G.STATS.p_attack].value)
