extends Node

var items = {}

func load_item(name, count=1):
	if name in items:
		items[name].count += count
		return items[name]
	var ItemClass = load("res://world/items/" + name + ".gd")
	assert(ItemClass is Script)
	var new_item = ItemClass.new()
	assert(new_item is G.Item)
	new_item.source = name
	new_item.count = count
	items[name] = new_item
	return new_item

func remove_item(name, count=1):
	if not name in items:
		return
	if items[name].count > count:
		items[name].count -= count
	else:
		items[name].queue_free()
		items.erase(name)

func push_item(item):
	assert(item is G.Item)
	assert(not item.is_inside_tree())
	if item.source in items:
		items[item.source].count += item.count
		item.queue_free()
	else:
		items[item.source] = item

func pop_item(name, count=1):
	if not name in items:
		return null
	var item = items[name]
	if item.count <= count:
		items.erase(name)
		return item
	var new_item = item.get_script().new()
	new_item.source = name
	new_item.count = count
	item.count -= count
	return new_item
