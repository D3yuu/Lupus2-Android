import net
import app
import ui
import uiOption

import uiSystemOption
import uiGameOption
import uiScriptLocale
import networkModule
import constInfo
import localeInfo
import uiMoveChannel
import uiItemShop
import eventManager
import utils
from _weakref import proxy

SYSTEM_MENU_FOR_PORTAL = False

###################################################################################################
## System
class SystemDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.eventOpenHelpWindow = None
		self.systemOptionDlg = None
		self.gameOptionDlg = None
		self.interface = None
		self.toolTip = None

	def SetToolTip(self, tooltip):
		self.toolTip = proxy(tooltip)

	def BindInterface(self, interface):
		self.interface = interface

	def LoadDialog(self):
		if SYSTEM_MENU_FOR_PORTAL:
			self.__LoadSystemMenu_ForPortal()
		else:
			self.__LoadSystemMenu_Default()

	def __LoadSystemMenu_Default(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/systemdialog.py")

		# Butoanele ramase active
		self.GetChild("system_option_button").SAFE_SetEvent(self.__ClickSystemOptionButton)
		self.GetChild("game_option_button").SAFE_SetEvent(self.__ClickGameOptionButton)
		self.GetChild("change_button").SAFE_SetEvent(self.__ClickChangeCharacterButton)
		self.GetChild("logout_button").SAFE_SetEvent(self.__ClickLogOutButton)
		self.GetChild("exit_button").SAFE_SetEvent(self.__ClickExitButton)
		self.GetChild("cancel_button").SAFE_SetEvent(self.Close)

		# Butoanele STERSE (Help, Support, Escape, Mall, Version) au fost eliminate de aici
		# pentru a preveni KeyError.

		if app.ENABLE_MOVE_CHANNEL:
			self.GetChild("movechannel_button").SAFE_SetEvent(self.__ClickMoveChannelButton)

	def __LoadSystemMenu_ForPortal(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/systemdialog_forportal.py")

		self.GetChild("system_option_button").SAFE_SetEvent(self.__ClickSystemOptionButton)
		self.GetChild("game_option_button").SAFE_SetEvent(self.__ClickGameOptionButton)
		self.GetChild("change_button").SAFE_SetEvent(self.__ClickChangeCharacterButton)
		self.GetChild("exit_button").SAFE_SetEvent(self.__ClickExitButton)
		self.GetChild("cancel_button").SAFE_SetEvent(self.Close)

	@ui.WindowDestroy
	def Destroy(self):
		self.ClearDictionary()

		if self.gameOptionDlg:
			self.gameOptionDlg.Destroy()

		if self.systemOptionDlg:
			self.systemOptionDlg.Destroy()

		self.__Initialize()

	def SetOpenHelpWindowEvent(self, event):
		self.eventOpenHelpWindow = event

	def OpenDialog(self):
		self.Show()

	def __ClickChangeCharacterButton(self):
		self.Close()
		net.ExitGame()

	def __OnClosePopupDialog(self):
		self.popup = None

	def __ClickLogOutButton(self):
		if SYSTEM_MENU_FOR_PORTAL:
			if app.loggined:
				self.Close()
				net.ExitApplication()
			else:
				self.Close()
				net.LogOutGame()
		else:
			self.Close()
			net.LogOutGame()


	def __ClickExitButton(self):
		self.Close()
		net.ExitApplication()

	def __ClickSystemOptionButton(self):
		self.Close()

		if not self.systemOptionDlg:
			self.systemOptionDlg = uiSystemOption.OptionDialog()

		self.systemOptionDlg.Show()

	def __ClickGameOptionButton(self):
		self.Close()

		if not self.gameOptionDlg:
			self.gameOptionDlg = uiGameOption.OptionDialog()
			self.gameOptionDlg.SetToolTip(self.toolTip)

		self.gameOptionDlg.Show()

	if app.ENABLE_MOVE_CHANNEL:
		def __ClickMoveChannelButton(self):
			self.Close()
			if self.interface:
				self.interface.ToggleMoveChannelWindow()

	def Close(self):
		self.Hide()
		self.__OnMouseOverOut()
		return True

	def OnBlockMode(self, mode):
		uiGameOption.blockMode = mode
		if self.gameOptionDlg:
			self.gameOptionDlg.OnBlockMode(mode)

	def OnChangePKMode(self):
		if self.gameOptionDlg:
			self.gameOptionDlg.OnChangePKMode()

	def __OnMouseOverOut(self):
		if self.toolTip:
			self.toolTip.HideToolTip()

	def OnPressExitKey(self):
		self.Close()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True