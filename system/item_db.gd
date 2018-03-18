extends Node

func load_item(name, count=1):
	for child in get_children():
		if child.source == name:
			if not child.stackable:
				break
			child.count += count
			return [child]
	var list = []
	var ItemClass = load("res://world/items/" + name + ".gd")
	while count > 0:
		var new_item = ItemClass.new()
		assert(new_item is G.Item)
		new_item.source = name
		if new_item.stackable:
			new_item.count = count
		count -= new_item.count
		list.append(new_item)
		add_child(new_item)
	return list

func remove_item(name, count=1):
	for child in get_children():
		if child.source == name:
			if child.stackable:
				child.count -= count
				if child.count <= 0:
					remove_child(child)
					child.queue_free()
				return
			remove_child(child)
			child.queue_free()
			count -= 1
			if count == 0:
				break

func push_item(item):
	assert(item is G.Item)
	if item.stackable:
		for child in ItemDB.get_children():
			if child.source == item.source:
				child.count += item.count
				item.queue_free()
				return
	add_child(item)

func pop_item(item):
	assert(item.parent == self)
	if item.count == 1:
		remove_child(item)
		return item
	var new_item = get_script().new()
	new_item.source = item.source
	item.count -= 1
	return new_item
