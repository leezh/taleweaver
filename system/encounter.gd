extends "res://system/party.gd"

export(float) var encounter_weight = 1.0
export(String, MULTILINE) var loot_list = ""
export(String, MULTILINE) var inventory_list = ""

func _ready():
	for line in inventory_list.split("/n"):
		line = line.strip_edges()
		if line.empty() or line.begins_with("#"):
			continue
		add_item(line)
	var index = 0
	for child in get_children():
		set_party_member(index, child)
		index += 1
