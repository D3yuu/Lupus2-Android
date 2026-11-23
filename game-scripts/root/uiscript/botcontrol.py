import uiScriptLocale

WINDOW_WIDTH = 425
WINDOW_HEIGHT = 225

window = {
	"name": "BotControlWindow",
	"style": ("movable", "float",),
	"x": 0,
	"y": 0,
	"width": WINDOW_WIDTH,
	"height": WINDOW_HEIGHT,
	"children":
	(
		{
			"name": "Board",
			"type": "board",
			"x": 0,
			"y": 0,
			"width": WINDOW_WIDTH,
			"height": WINDOW_HEIGHT,
			"children":
			(
				{
					"type": "horizontalbar",
					"x": 0,
					"y": 0,
					"width": WINDOW_WIDTH,
					"children":
					(
						{
							"type": "text",
							"x": 0,
							"y": 3,
							"text": uiScriptLocale.BOT_CONTROL_TITLE,
							"horizontal_align": "center",
							"text_horizontal_align": "center",
							"bold": True,
						},
					),
				},
				{
					"type": "text",
					"x": 0,
					"y": 28,
					"text": uiScriptLocale.BOT_CONTROL_INSTRUCTION,
					"horizontal_align": "center",
					"text_horizontal_align": "center",
				},
				{
					"name": "ItemName",
					"type": "text",
					"x": 0,
					"y": 47,
					"text": uiScriptLocale.BOT_CONTROL_ITEM,
					"horizontal_align": "center",
					"text_horizontal_align": "center",
					"color": 0xfffee3ae,
					"bold": True,
				},
				{
					"name": "ItemSlot",
					"type": "grid_table",
					"x": 32,
					"y": 68,
					"start_index": 0,
					"x_count": 9,
					"y_count": 3,
					"x_step": 40,
					"y_step": 32,
					"image": "d:/ymir work/ui/public/Slot_Base.sub",
				},
				{
					"name": "AttemptText",
					"type": "text",
					"x": 0,
					"y": 174,
					"text": uiScriptLocale.BOT_CONTROL_ONE_ATTEMPT,
					"horizontal_align": "center",
					"text_horizontal_align": "center",
					"color": 0xffffc04d,
				},
				{
					"type": "text",
					"x": 0,
					"y": 199,
					"text": uiScriptLocale.BOT_CONTROL_WRONG_KICK,
					"horizontal_align": "center",
					"text_horizontal_align": "center",
					"color": 0xffff5a5a,
				},
			),
		},
	),
}
