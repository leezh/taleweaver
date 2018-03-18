extends Node

const Character = preload("res://system/character.gd")
const Item = preload("res://system/item.gd")

const STAT_MAXHP = 0
const STAT_MAXSP = 1
const STAT_PATTACK = 2
const STAT_PDEFENCE = 3
const STAT_MATTACK = 4
const STAT_MDEFENCE = 5
const STAT_SPEED = 6

const STAT_NAMES = ["max_hp", "max_sp", "p_attack", "p_defence", "m_attack", "m_defence", "speed"]
