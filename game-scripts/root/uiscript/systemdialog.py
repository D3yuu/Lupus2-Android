import app
import uiScriptLocale
from utils import ElementAddBefore, FindElementRef

ROOT = "d:/ymir work/ui/public/"

window = {
	"name" : "SystemDialog",
	"style" : ("float",),

	# Pozitionare initiala (se va recalcula inaltimea automat mai jos)
	"x" : (SCREEN_WIDTH - 200) / 2,
	"y" : (SCREEN_HEIGHT - 200) / 2, # Valoare temporara

	"width" : 200,
	"height" : 200, # Valoare temporara

	"children" :
	[
		{
			"name" : "board",
			"type" : "thinboard",

			"x" : 0,
			"y" : 0,

			"width" : 200,
			"height" : 200, # Se recalculeaza automat

			"children" :
			[
				# --- Butoane Ramase ---
				{
					"name" : "system_option_button",
					"type" : "button",

					"x" : 10,
					"y" : 0, # Y se calculeaza automat

					"text" : uiScriptLocale.SYSTEMOPTION_TITLE,

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
				{
					"name" : "game_option_button",
					"type" : "button",

					"x" : 10,
					"y" : 0,

					"text" : uiScriptLocale.GAMEOPTION_TITLE,

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
				{
					"name" : "change_button",
					"type" : "button",

					"x" : 10,
					"y" : 0,

					"text" : uiScriptLocale.SYSTEM_CHANGE,

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
				{
					"name" : "logout_button",
					"type" : "button",

					"x" : 10,
					"y" : 0,

					"text" : uiScriptLocale.SYSTEM_LOGOUT,

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
				{
					"name" : "exit_button",
					"type" : "button",

					"x" : 10,
					"y" : 0,

					"text" : uiScriptLocale.SYSTEM_EXIT,

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
				{
					"name" : "cancel_button",
					"type" : "button",

					"x" : 10,
					"y" : 0,

					"text" : uiScriptLocale.CANCEL,

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				},
			],
		},
	],
}

#############################################################################
# Logica de adaugare dinamica a butonului Move Channel
MAIN_BOARD = FindElementRef(window["children"], "board")

if app.ENABLE_MOVE_CHANNEL:
	ElementAddBefore(MAIN_BOARD["children"], "logout_button",
				{
					"name" : "movechannel_button",
					"type" : "button",

					"x" : 10,
					"y" : -1, # Se va seta in Recalculate

					"text" : uiScriptLocale.SYSTEM_MOVE_CHANNEL,

					"default_image" : ROOT + "XLarge_Button_01.sub",
					"over_image" : ROOT + "XLarge_Button_02.sub",
					"down_image" : ROOT + "XLarge_Button_03.sub",
				})

# Functia care aranjeaza totul frumos
def Recalculate():
	global MAIN_BOARD
	
	INIT_Y = 10 # Spatiul de sus (marginea)
	BUTTON_HEIGHT = 30 # Distanta intre butoane
	
	def Next():
		# Functie interna pentru a genera urmatoarea pozitie Y
		ret = getattr(Recalculate, "NEXT_Y", INIT_Y)
		setattr(Recalculate, "NEXT_Y", ret + BUTTON_HEIGHT)
		return ret

	# Resetam contorul in caz ca se apeleaza de mai multe ori
	setattr(Recalculate, "NEXT_Y", INIT_Y)

	# Actualizam Y pentru fiecare element ramas in lista
	for elem in MAIN_BOARD["children"]:
		elem["y"] = Next()

	# Calculam inaltimea totala a ferestrei
	# Ultimul Y + o mica marja jos (10px)
	TOTAL_HEIGHT = getattr(Recalculate, "NEXT_Y", INIT_Y) + 10
	
	window["height"] = TOTAL_HEIGHT
	MAIN_BOARD["height"] = TOTAL_HEIGHT
	
	# Centram fereastra pe ecran pe baza noii inaltimi
	window["y"] = (SCREEN_HEIGHT - TOTAL_HEIGHT) / 2

# Executam rearanjarea
Recalculate()