import wx, wx.adv, wx.lib.newevent, os, pyautogui, re
import pyxhook

from params import p
from unicodeit.replace import replace
from unicodeit.data import REPLACEMENTS, COMBININGMARKS

APP_TITLE = 'Latexey'
TRAY_TOOLTIP = 'Latexey'
APP_ICON = os.path.dirname(os.path.realpath(__file__))+'/icon.png'
TRAY_ICON = os.path.dirname(os.path.realpath(__file__))+'/icon.png'
ENABLE_DICT = True

LATEX_COMMANDS = REPLACEMENTS + COMBININGMARKS

WIDTH_INIT = p['WIDTH_INIT']
VPAD = p['VPAD']

class TaskBarIcon(wx.adv.TaskBarIcon):

	def __init__(self, frame):
		self.frame = frame
		super(TaskBarIcon, self).__init__()
		self.setIcon(TRAY_ICON)
		self.Bind(wx.adv.EVT_TASKBAR_LEFT_DOWN, self.onLeftClick)

	def CreatePopupMenu(self):
		menu = wx.Menu()
		self.createMenuItem(menu, 'Show Frame', self.showFrame)
		self.createMenuItem(menu, 'Exit', self.exitApp)
		return menu

	def createMenuItem(self, menu, label, func):
		item = wx.MenuItem(menu, -1, label)
		menu.Bind(wx.EVT_MENU, func, id=item.GetId())
		menu.Append(item)
		return item

	def onLeftClick(self, event):
		if self.frame.IsShown(): self.frame.hideWindow(event=None)
		else:		   			 self.frame.showWindow(event=None)

	def setIcon(self, path):
		icon = wx.Icon(wx.Bitmap(path))
		self.SetIcon(icon, TRAY_TOOLTIP)

	def showFrame(self, event):
		self.frame.showWindow(event=None)

	def exitApp(self, event):
		closeApp()

class Frame(wx.Frame):

	def __init__(self, parent):
		wx.Frame.__init__(self, name='', parent=parent,
			pos=wx.Point(wx.GetMousePosition()[0], wx.GetMousePosition()[1]), 
			title=APP_TITLE)
		self.readyPasteEvent, self.EVT_READY_PASTE = wx.lib.newevent.NewEvent()
		self.lastMousePos = wx.Point(0, 0)
		self.screenheight = wx.SystemSettings.GetMetric(wx.SYS_SCREEN_Y)
		self.val = ''
		self.panel = wx.Panel(self)
		self.popup = Dictionary(self)
		
		self.text_input = wx.TextCtrl(self.panel, size=(WIDTH_INIT,-1))
		self.text_input.SetFont(wx.Font(wx.FontInfo(10)))

		self.text_output = wx.StaticText(self.panel, label='')
		self.text_output.SetFont(wx.Font(wx.FontInfo(11)))

		self.initSizer()
		self.refit()
		self.initBind()
		self.setIcon(APP_ICON)
		self.text_input.SetFocus()

	def initSizer(self):
		self.moduleSizer = wx.BoxSizer(wx.VERTICAL)
		self.panel.SetSizer(self.moduleSizer)
		self.moduleSizer.AddSpacer(VPAD)
		self.moduleSizer.Add(self.text_input, flag=wx.ALIGN_CENTER)
		self.moduleSizer.AddSpacer(VPAD) 
		self.moduleSizer.Add(self.text_output, flag=wx.ALIGN_CENTER)
		self.moduleSizer.AddSpacer(VPAD)

	def initBind(self):
		self.menu = wx.Menu()

		self.toPaste = False

		trayMenu = wx.MenuItem(self.menu, -1, 'tray')
		exitMenu = wx.MenuItem(self.menu, -1, 'exit')
		self.panel.Bind(wx.EVT_MENU, self.hideWindow, id=trayMenu.GetId())
		self.panel.Bind(wx.EVT_MENU, self.exitApp, id=exitMenu.GetId())

		entries = [(wx.ACCEL_CTRL,  ord('W'), exitMenu.GetId() )]
		accel = wx.AcceleratorTable(entries)
		self.panel.SetAcceleratorTable(accel)

		self.text_input.Bind(wx.EVT_KEY_UP, self.evaluate)
		self.text_input.Bind(wx.EVT_KEY_DOWN, self.onKeyDown)
		self.panel.Bind(wx.EVT_MOTION, self.dragWin)
		self.panel.Bind(wx.EVT_LEFT_DOWN, self.clickWin)
		self.text_output.Bind(wx.EVT_LEFT_DOWN, self.clickWin)
		self.Bind(self.EVT_READY_PASTE, self.writePaste)

	def setIcon(self, path):
		icon = wx.Icon(wx.Bitmap(path))
		self.SetIcon(icon)

	def dragWin(self, event):
		if event.LeftIsDown():
			windowX, windowY = self.lastMousePos
			screenX, screenY = wx.GetMousePosition()
			self.Move(wx.Point(screenX - windowX, screenY - windowY))
		event.Skip()
 
	def clickWin(self, event):
		pos = self.ScreenToClient(event.GetEventObject().ClientToScreen(event.GetPosition()))
		self.lastMousePos = pos
		event.Skip()

	def onKeyDown(self, event):
		key = event.GetKeyCode()
		if key == wx.WXK_ESCAPE: self.hideWindow(event)
		if key == wx.WXK_RETURN: 
			if self.popup.cursor == -1:
				self.write(event)
			else:
				wx.CallAfter(self.popup.requestInput)
		# if key == wx.WXK_DOWN  :	wx.CallAfter(self.popup.incCursor, 1)
		# if key == wx.WXK_UP    :	wx.CallAfter(self.popup.incCursor, -1)
		if key == wx.WXK_DOWN  : self.popup.incCursor(1)
		if key == wx.WXK_UP    : self.popup.incCursor(-1)
		event.Skip()

	def writeOnInput(self, text):
		self.text_input.Clear()
		self.text_input.write(text)

	def checkPopup(self):
		if not ENABLE_DICT:
			wx.CallAfter(self.popup.Hide)
		elif self.popup.listBox.GetCount() > 0 :
			wx.CallAfter(self.popup.Show)
			self.positionPopup()
		else:
			wx.CallAfter(self.popup.Hide)

	def positionPopup(self):
		left_x = self.text_input.GetScreenPosition().Get()[0]
		upper_y = self.GetScreenPosition().Get()[1]
		height = self.GetSize()[1]
		popup_width, popup_height = WIDTH_INIT, 100
		if upper_y + height + popup_height > self.screenheight:
			self.popdown = False
			wx.CallAfter(self.popup.SetPosition,(left_x, upper_y - popup_height))
		else:
			self.popdown = True
			wx.CallAfter(self.popup.SetPosition,(left_x, upper_y + height))

	def write(self, event):
		clipdata = wx.TextDataObject()
		clipdata.SetText(self.val)
		wx.TheClipboard.Open()
		wx.TheClipboard.SetData(clipdata)
		wx.TheClipboard.Close()
		self.text_input.Clear()
		self.hideWindow(event=None)
		wx.PostEvent(self, self.readyPasteEvent())

	def writePaste(self, event):
		pyautogui.hotkey("ctrl", "v")

	def hideWindow(self, event):
		wx.CallAfter(self.popup.Hide)
		self.Hide()

	def showWindow(self, event):
		self.Show()
 
	def exitApp(self, event):
		closeApp()

	def refit(self):
		self.moduleSizer.RepositionChildren(self.moduleSizer.MinSize)
		self.panel.SetSizer(self.moduleSizer)
		self.moduleSizer.Fit(self)

	def evaluate(self, event):
		getinput = self.text_input.GetValue()
		index = self.text_input.GetInsertionPoint()
		separated_input = [getinput[:index], getinput[index:]]
		if ENABLE_DICT:	
			wx.CallAfter(self.popup.update, separated_input)
			self.checkPopup()
		self.val = replace(getinput)
		self.text_output.SetLabel(self.val)
		self.refit()
		event.Skip()

class HookMan(pyxhook.HookManager):
	def __init__(self,frame,parent=None):
		pyxhook.HookManager.__init__(self,parent)
		self.pressed = []
		self.topressed = set(['Control_L', 'apostrophe'])
		self.KeyDown = self.key_down
		self.KeyUp = self.key_up
		self.frame = frame
		self.HookKeyboard()

	def keyevent(self):
		if set(self.pressed) ==  self.topressed:
			wx.CallAfter(self.frame.showWindow,event=None)

	def key_down(self,event):
		key = event.Key
		if key not in self.pressed: self.pressed.append(key)
		self.keyevent()

	def key_up(self,event):
		key = event.Key
		if key in self.pressed: self.pressed.remove(key)
		self.keyevent()
		self.pressed = []

class Dictionary(wx.PopupWindow):

	def __init__(self, parent=None):
		wx.PopupWindow.__init__(self, parent)
		self.SetSize(WIDTH_INIT,100)
		self.head, self.tail= ['','']
		self.parent = parent
		self.theItems = []
		self.listBox = wx.ListBox(self, size=(WIDTH_INIT,self.GetSize()[1]))
		self.cursor = -1
		self.previnp = []

	def update(self, raw_separated_inp):
		head_raw, self.tail = raw_separated_inp
		processed_inp = self.process_input(head_raw)
		if self.previnp != raw_separated_inp:
			self.cursor = -1
		r = re.compile(processed_inp.replace('\\','\\\\'))
		thetuple = [tup for tup in LATEX_COMMANDS if r.match(tup[0])][:10]
		self.listBox.Clear()
		if(len(thetuple) > 0):
			self.theItems = [sym+' '+com for (com,sym) in thetuple]
			self.listBox.InsertItems(self.theItems,0)
			self.checkCursor()
		self.listBox.SetSelection(self.cursor)
		self.previnp = raw_separated_inp

	def incCursor(self, inc):
		self.cursor += inc
		
	def checkCursor(self):
		if self.cursor < -1 : self.cursor = -1
		maxval = self.listBox.GetCount()
		if self.cursor >= maxval : self.cursor = maxval-1

	def process_input(self, inp):
		raw_inp = inp
		self.head = inp
		flipped = raw_inp[::-1]
		if len(flipped) == 0 :
			processed_inp = ' '
		elif (flipped[0] == ' ') or ('\\' not in flipped) :
			processed_inp = ' '
		else:
			theid = flipped.index('\\')
			self.head = flipped[theid+1:][::-1]
			flipped_snip = flipped[:theid]
			processed_inp = '\\'+flipped_snip[::-1]
		return processed_inp

	def requestInput(self):
		selected = self.listBox.GetString(self.cursor)
		command = selected.split(' ')[-1]
		wx.CallAfter(self.parent.writeOnInput,self.head+command+self.tail)
		# print(self.head + command)


	def hideWindow(self):
		self.Hide()

def closeApp():
	hookman.cancel()
	wx.CallAfter(tray.Destroy)
	wx.CallAfter(frame.Destroy)

if __name__ == '__main__':
	app = wx.App()
	frame = Frame(None)
	hookman = HookMan(frame)
	tray = TaskBarIcon(frame)
	hookman.start()
	frame.Show()
	app.MainLoop()