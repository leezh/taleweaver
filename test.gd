extends Node

func _ready():
	var godette = PlayerParty.add_character("godette")
	godette.equip(G.EQUIP_WEAPON, "basic_blaster")
	PlayerParty.set_party_member(0, godette)
	print(godette.stats[G.STAT_PATTACK].value)
