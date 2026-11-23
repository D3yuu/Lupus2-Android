import ui
import app
import random
import re
import net
import item
import uiScriptLocale


class BotControlWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self, layer="TOP_MOST")
		self.items = []
		self.shuffleTime = 0.0
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		self.items = []
		self.shuffleTime = 0.0

	def LoadWindow(self):
		try:
			loader = ui.PythonScriptLoader()
			loader.LoadScriptFile(self, "UIScript/botcontrol.py")
		except:
			import exception
			exception.Abort("BotControlWindow.LoadWindow.LoadScript")

		try:
			self.itemSlot = self.GetChild("ItemSlot")
			self.itemName = self.GetChild("ItemName")
			self.attemptText = self.GetChild("AttemptText")
		except:
			import exception
			exception.Abort("BotControlWindow.LoadWindow.BindObject")

		self.itemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.ClickSlot))

	def Open(self):
		self.Refresh()
		self.shuffleTime = app.GetTime() + 5.0
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		return True

	def OnPressExitKey(self):
		return True

	def ClickSlot(self, selectedSlotPos):
		if selectedSlotPos < 0 or selectedSlotPos >= len(self.items):
			return

		for slot in xrange(self.itemSlot.GetSlotCount()):
			self.itemSlot.DeactivateSlot(slot)

		self.itemSlot.ActivateSlot(selectedSlotPos)
		net.SendChatPacket("/b0t_c4tr9 %d" % int(self.items[selectedSlotPos]))

	def UpdateItems(self, vnums):
		self.items = list(vnums)

	def SetRealItemName(self, text):
		cleanName = re.sub(r"[0-9]", "", text).replace("+", "").replace("_", " ").strip()
		words = cleanName.split()
		coloredWords = []
		letterCount = sum(len(word) for word in words)
		symbolPositions = {}
		candidatePositions = []
		globalPosition = 0

		# Pastram prima si ultima litera din fiecare cuvant pentru lizibilitate.
		for word in words:
			for localPosition in xrange(1, max(1, len(word) - 1)):
				candidatePositions.append(globalPosition + localPosition)
			globalPosition += len(word)

		if candidatePositions:
			random.shuffle(candidatePositions)
			symbolCount = 1 if letterCount < 9 else 2
			selectedPositions = []

			for position in candidatePositions:
				if any(abs(position - selected) <= 1 for selected in selectedPositions):
					continue

				selectedPositions.append(position)
				if len(selectedPositions) >= symbolCount:
					break

			for position in selectedPositions:
				symbolPositions[position] = random.choice("$%&^*")

		letterIndex = 0
		for word in words:
			coloredLetters = []
			for letter in word:
				if letterIndex in symbolPositions:
					shownLetter = symbolPositions[letterIndex]
				else:
					shownLetter = random.choice((letter.lower(), letter.upper()))
				letterIndex += 1

				if random.randint(0, 1):
					color = "|cff%02x%02x%02x" % (random.randint(80, 255), random.randint(80, 255), random.randint(80, 255))
					shownLetter = color + shownLetter + "|r"
				coloredLetters.append(shownLetter)
			coloredWords.append("-".join(coloredLetters))

		self.itemName.SetText("   ".join(coloredWords))

	def SetRealItemVnum(self, vnum):
		try:
			item.SelectItem(int(vnum))
			self.SetRealItemName(item.GetItemName())
		except:
			self.SetRealItemName("Obiect")

	def SetAttempts(self, attempts):
		if attempts == 1:
			self.attemptText.SetText(uiScriptLocale.BOT_CONTROL_ONE_ATTEMPT)
		else:
			self.attemptText.SetText(uiScriptLocale.BOT_CONTROL_ATTEMPTS_LEFT % attempts)

	def Refresh(self):
		for slot in xrange(self.itemSlot.GetSlotCount()):
			self.itemSlot.ClearSlot(slot)

		for slot, vnum in enumerate(self.items):
			self.itemSlot.SetItemSlot(slot, vnum, 0)

		self.itemSlot.RefreshSlot()

	def OnUpdate(self):
		if not self.IsShow() or len(self.items) < 2:
			return

		if self.shuffleTime <= app.GetTime():
			random.shuffle(self.items)
			self.Refresh()
			self.shuffleTime = app.GetTime() + 5.0
