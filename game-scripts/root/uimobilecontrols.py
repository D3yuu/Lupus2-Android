import app
import math
import ui
import wndMgr


CONTROL_ROOT = "d:/ymir work/ui/mobile/"
BUTTON_NORMAL = CONTROL_ROOT + "button_normal.tga"
BUTTON_PRESSED = CONTROL_ROOT + "button_pressed.tga"
ORIGINAL_UI_COMPENSATION = 1.0 / 1.12


class MobileControlsOverlay(ui.Window):
	"""Visual-only mobile HUD rendered below the regular PC UI layer."""

	def __init__(self, parent):
		ui.Window.__init__(self, "GAME")
		self.SetParent(parent)
		self.SetWindowName("MobileControlsOverlay")
		self.SetPosition(0, 0)
		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.AddFlag("not_pick")

		self.screenWidth = wndMgr.GetScreenWidth()
		self.screenHeight = wndMgr.GetScreenHeight()
		self.lastMask = -1
		self.lastPage = -1
		self.buttons = []
		self.quickButtons = []

		self.__BuildJoystick()
		self.__BuildActionButtons()
		self.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def __Center(self, xRatio, yRatio):
		return int(self.screenWidth * xRatio), int(self.screenHeight * yRatio)

	def __KeepMobileSize(self, size):
		# The original PC interface is enlarged globally. These controls were
		# already touch-sized, so preserve their current physical dimensions.
		return max(1, int(size * ORIGINAL_UI_COMPENSATION + 0.5))

	def __MakeImage(self, imageName, centerX, centerY, size, sourceSize=128):
		image = ui.ExpandedImageBox("GAME")
		image.SetParent(self)
		image.AddFlag("not_pick")
		image.LoadImage(imageName)
		scale = float(size) / float(sourceSize)
		image.SetScale(scale, scale)
		image.SetPosition(int(centerX - size / 2), int(centerY - size / 2))
		image.Show()
		return image

	def __MakeLabel(self, text, centerX, centerY, large=False):
		label = ui.TextLine("GAME")
		label.SetParent(self)
		label.AddFlag("not_pick")
		label.SetPosition(centerX, centerY)
		label.SetHorizontalAlignCenter()
		label.SetVerticalAlignCenter()
		label.SetOutline()
		label.SetFontName("Tahoma:10")
		if large:
			label.SetFontName("Tahoma:14")
		label.SetFontColor(0.86, 0.89, 0.91)
		label.SetText(text)
		label.Show()
		return label

	def __MakeButton(self, center, size, keyIndex, label=None):
		centerX, centerY = center
		background = self.__MakeImage(BUTTON_NORMAL, centerX, centerY, size)
		entry = {
			"background": background,
			"center": center,
			"size": size,
			"key": keyIndex,
			"pressed": False,
			"label": None,
		}
		if label is not None:
			entry["label"] = self.__MakeLabel(label, centerX, centerY, size >= 90)
		self.buttons.append(entry)
		return entry

	def __MakeIcon(self, entry, imageName, sourcePixels, displayPixels):
		centerX, centerY = entry["center"]
		icon = ui.ExpandedImageBox("GAME")
		icon.SetParent(self)
		icon.AddFlag("not_pick")
		icon.LoadImage(imageName)
		scale = float(displayPixels) / float(sourcePixels)
		icon.SetScale(scale, scale)
		icon.SetPosition(int(centerX - displayPixels / 2), int(centerY - displayPixels / 2))
		icon.Show()
		entry["icon"] = icon

	def __BuildJoystick(self):
		centerX, centerY = self.__Center(0.13, 0.70)
		self.joystickCenter = (centerX, centerY)
		self.joystickBase = self.__MakeImage(
			CONTROL_ROOT + "joystick_base.tga", centerX, centerY, self.__KeepMobileSize(116))
		self.joystickKnob = self.__MakeImage(
			CONTROL_ROOT + "joystick_knob.tga", centerX, centerY, self.__KeepMobileSize(48))
		self.joystickKnobSize = self.__KeepMobileSize(48)
		self.joystickTravel = self.__KeepMobileSize(20)

	def __BuildActionButtons(self):
		# Measured from the supplied reference: the five slots form a compact,
		# asymmetric arc around the attack button rather than a mathematical circle.
		attackCenter = self.__Center(0.880, 0.720)
		attack = self.__MakeButton(attackCenter, self.__KeepMobileSize(136), 4)
		self.__MakeIcon(
			attack,
			"d:/ymir work/ui/game/taskbar/mouse_button_attack_01.sub",
			32,
			self.__KeepMobileSize(72),
		)

		# Exact centre layout from the reference, scaled uniformly to the
		# existing 64 px quick buttons (reference radius/button = 596/158).
		layoutScale = float(self.screenHeight) / 900.0
		quickOffsets = (
			(-235,   96), # 1
			(-210,  -31), # 2
			(-137, -137), # 3
			( -31, -210), # 4
			(  96, -235), # 5
		)
		quickCenters = []
		for offsetX, offsetY in quickOffsets:
			quickCenters.append((
				int(attackCenter[0] + offsetX * layoutScale),
				int(attackCenter[1] + offsetY * layoutScale),
			))
		for index, center in enumerate(quickCenters):
			entry = self.__MakeButton(center, self.__KeepMobileSize(64), 5 + index, str(index + 1))
			self.quickButtons.append(entry)

		utilityX = int(attackCenter[0] - 331 * layoutScale)
		horse = self.__MakeButton((utilityX, quickCenters[1][1]), self.__KeepMobileSize(58), 15)
		self.__MakeIcon(
			horse,
			CONTROL_ROOT + "horse_icon.tga",
			32,
			self.__KeepMobileSize(34),
		)

		pickup = self.__MakeButton((utilityX, quickCenters[0][1]), self.__KeepMobileSize(58), 16)
		self.__MakeIcon(
			pickup,
			CONTROL_ROOT + "pickup_icon.tga",
			48,
			self.__KeepMobileSize(44),
		)

		self.pageButton = self.__MakeButton(self.__Center(0.850, 0.855), self.__KeepMobileSize(52), -1, ">")

	def __SetButtonPressed(self, entry, pressed):
		if entry["pressed"] == pressed:
			return
		entry["pressed"] = pressed
		entry["background"].LoadImage(BUTTON_PRESSED if pressed else BUTTON_NORMAL)
		scale = float(entry["size"]) / 128.0
		entry["background"].SetScale(scale, scale)

	def __SetQuickPage(self, page):
		labels = ("1", "2", "3", "4", "5") if page == 0 else ("TAB", "F1", "F2", "F3", "F4")
		firstKey = 5 if page == 0 else 10
		for index, entry in enumerate(self.quickButtons):
			entry["key"] = firstKey + index
			entry["label"].SetText(labels[index])
		self.pageButton["label"].SetText(">" if page == 0 else "<")

	def OnUpdate(self):
		controlMask, quickPage = app.GetMobileControlState()

		if quickPage != self.lastPage:
			self.__SetQuickPage(quickPage)
			self.lastPage = quickPage

		if controlMask != self.lastMask:
			for entry in self.buttons:
				key = entry["key"]
				pressed = key >= 0 and (controlMask & (1 << key)) != 0
				self.__SetButtonPressed(entry, pressed)
			self.lastMask = controlMask

		centerX, centerY = self.joystickCenter
		directionX = (1 if controlMask & (1 << 3) else 0) - (1 if controlMask & (1 << 2) else 0)
		directionY = (1 if controlMask & (1 << 1) else 0) - (1 if controlMask & (1 << 0) else 0)
		self.joystickKnob.SetPosition(
			centerX - self.joystickKnobSize / 2 + directionX * self.joystickTravel,
			centerY - self.joystickKnobSize / 2 + directionY * self.joystickTravel)
