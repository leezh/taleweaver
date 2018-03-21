extends Node

func _ready():
	var godette = PartyDB.add_character("godette")
	godette.equip(G.EQUIP_WEAPON, "basic_blaster")
	print(godette.stats[G.STAT_PATTACK].value)
