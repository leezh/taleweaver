extends "res://system/party.gd"

export(float) var encounter_weight = 1.0
export(String, MULTILINE) var loot_list = ""
export(String, MULTILINE) var inventory_list = ""

func _ready():
	for line in inventory_list.split("/n"):
		line = line.strip()
		if line.empty() or line.begins_with("#"):
			continue
		add_item(line)
	for index in range(get_child_count()):
		if child.position >= 0 and not child.position in party:
			set_party_member(child.position, child)
		elif not index in party:
			set_party_member(index, child)
		else:
			set_party_member(-1, child)
