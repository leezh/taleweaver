tool
extends Node

var display_name = ""
var description = ""
var icon = null
var use_in_battle = false
var use_in_field = false

func _get_property_list():
	var list = [{
			"name": "display_name",
			"type": TYPE_STRING,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNATIONALIZED
		},{
			"name": "description",
			"type": TYPE_STRING,
			"hint": PROPERTY_HINT_MULTILINE_TEXT,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNATIONALIZED
		},{
			"name": "icon",
			"type": TYPE_OBJECT,
			"hint": PROPERTY_HINT_RESOURCE_TYPE,
			"hint_string": "Texture",
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		},{
			"name": "use_in_battle",
			"type": TYPE_BOOL,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		},{
			"name": "use_in_field",
			"type": TYPE_BOOL,
			"usage": PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_STORE_IF_NONZERO
		}]
	return list

func _get(property):
	match property:
		"display_name":
			return display_name
		"description":
			return description
		"icon":
			return icon
		"use_in_battle":
			return use_in_battle
		"use_in_field":
			return use_in_field

func _set(property, value):
	match property:
		"display_name":
			display_name = value
		"description":
			description = value
		"icon":
			icon = value
		"use_in_battle":
			use_in_battle = value
		"use_in_field":
			use_in_field = value

func on_battle_use():
	pass

func on_field_use():
	pass
