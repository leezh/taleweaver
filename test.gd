extends Node

func _ready():
	var godette = PartyDB.add_character("godette")
	ItemDB.load_item("basic_blaster", 3)
	godette.equip(G.EQUIP_WEAPON, ItemDB.pop_item("basic_blaster"))
	print(godette.stats[G.STAT_PATTACK].value)
