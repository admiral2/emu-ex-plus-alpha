#pragma once

#include "MenuView.hh"
#include <TextEntry.hh>

static CollectTextInputView cheatInputView;
static void refreshCheatViews();
void EncodeGG(char *str, int a, int v, int c);

static const uint maxCheats = 254;

class EditCheatView : public BaseMenuView
{
private:
	TextMenuItem name;
	DualTextMenuItem addr, value, comp;
	DualTextMenuItem ggCode;
	TextMenuItem remove;
	uint idx = 0;
	int type = 0;
	char *nameStr = nullptr;
	MenuItem *item[5] = {nullptr};
	char addrStr[5] = {0}, valueStr[3] = {0}, compStr[3] = {0};
	char ggCodeStr[9] = {0};

	void syncCheat(const char *newName = 0)
	{
		if(type)
		{
			int a, v, c;
			if(!FCEUI_DecodeGG(ggCodeStr, &a, &v, &c))
			{
				logWarn("error decoding GG code %s", ggCodeStr);
				a = 0; v = 0; c = -1;
			}
			if(!FCEUI_SetCheat(idx, newName, a, v, c, -1, 1))
			{
				logWarn("error setting cheat %d", idx);
			}
		}
		else
		{
			logMsg("setting comp %d", strlen(compStr) ? (int)strtoul(compStr, nullptr, 16) : -1);
			if(!FCEUI_SetCheat(idx,
					newName, strtoul(addrStr, nullptr, 16), strtoul(valueStr, nullptr, 16),
					strlen(compStr) ? strtoul(compStr, nullptr, 16) : -1, -1, 0))
			{
				logWarn("error setting cheat %d", idx);
			}
		}
	}

	uint handleGgCodeFromTextInput(const char *str)
	{
		if(str)
		{
			if(strlen(str) != 6 && strlen(str) != 8)
			{
				popup.postError("Invalid, must be 6 or 8 digits");
				Base::displayNeedsUpdate();
				return 1;
			}
			string_copy(ggCodeStr, str, sizeof(ggCodeStr));
			syncCheat();
			ggCode.compile();
			Base::displayNeedsUpdate();
		}
		//viewStack.popAndShow();
		removeModalView();
		return 0;
	}

	void ggCodeHandler(TextMenuItem &item, const InputEvent &e)
	{
		cheatInputView.init("Input Game Genie code", ggCodeStr);
		cheatInputView.onTextDelegate().bind<EditCheatView, &EditCheatView::handleGgCodeFromTextInput>(this);
		cheatInputView.place(Gfx::viewportRect());
		modalView = &cheatInputView;
		//viewStack.pushAndShow(&cheatInputView);
	}

	uint handleNameFromTextInput(const char *str)
	{
		if(str)
		{
			logMsg("setting cheat name %s", str);
			syncCheat(str);
			FCEUI_GetCheat(idx, &nameStr, nullptr, nullptr, nullptr, nullptr, nullptr);
			name.t.setString(nameStr);
			name.compile();
			Base::displayNeedsUpdate();
		}
		//viewStack.popAndShow();
		removeModalView();
		return 0;
	}

	void nameHandler(TextMenuItem &item, const InputEvent &e)
	{
		cheatInputView.init("Input name", name.t.str);
		cheatInputView.onTextDelegate().bind<EditCheatView, &EditCheatView::handleNameFromTextInput>(this);
		cheatInputView.place(Gfx::viewportRect());
		modalView = &cheatInputView;
		//viewStack.pushAndShow(&cheatInputView);
	}

	uint handleAddrFromTextInput(const char *str)
	{
		if(str)
		{
			uint a = strtoul(str, nullptr, 16);
			if(a > 0xFFFF)
			{
				logMsg("addr 0x%X too large", a);
				popup.postError("Invalid input");
				Base::displayNeedsUpdate();
				return 1;
			}
			string_copy(addrStr, a ? str : "0", sizeof(addrStr));
			syncCheat();
			addr.compile();
			Base::displayNeedsUpdate();
		}
		//viewStack.popAndShow();
		removeModalView();
		return 0;
	}

	void addrHandler(TextMenuItem &item, const InputEvent &e)
	{
		cheatInputView.init("Input 4-digit hex", addrStr);
		cheatInputView.onTextDelegate().bind<EditCheatView, &EditCheatView::handleAddrFromTextInput>(this);
		cheatInputView.place(Gfx::viewportRect());
		modalView = &cheatInputView;
		//viewStack.pushAndShow(&cheatInputView);
	}

	uint handleValFromTextInput(const char *str)
	{
		if(str)
		{
			uint a = strtoul(str, nullptr, 16);
			if(a > 0xFF)
			{
				logMsg("val 0x%X too large", a);
				popup.postError("Invalid input");
				Base::displayNeedsUpdate();
				return 1;
			}
			string_copy(valueStr, a ? str : "0", sizeof(valueStr));
			syncCheat();
			value.compile();
			Base::displayNeedsUpdate();
		}
		//viewStack.popAndShow();
		removeModalView();
		return 0;
	}

	void valHandler(TextMenuItem &item, const InputEvent &e)
	{
		cheatInputView.init("Input 2-digit hex", valueStr);
		cheatInputView.onTextDelegate().bind<EditCheatView, &EditCheatView::handleValFromTextInput>(this);
		cheatInputView.place(Gfx::viewportRect());
		modalView = &cheatInputView;
		//viewStack.pushAndShow(&cheatInputView);
	}

	uint handleCompFromTextInput(const char *str)
	{
		if(str)
		{
			if(strlen(str))
			{
				uint a = strtoul(str, nullptr, 16);
				if(a > 0xFF)
				{
					logMsg("val 0x%X too large", a);
					popup.postError("Invalid input");
					Base::displayNeedsUpdate();
					return 1;
				}
				string_copy(compStr, str, sizeof(compStr));
			}
			else
			{
				compStr[0] = 0;
			}
			syncCheat();
			comp.compile();
			Base::displayNeedsUpdate();
		}
		//viewStack.popAndShow();
		removeModalView();
		return 0;
	}

	void compHandler(TextMenuItem &item, const InputEvent &e)
	{
		cheatInputView.init("Input 2-digit hex or blank", compStr);
		cheatInputView.onTextDelegate().bind<EditCheatView, &EditCheatView::handleCompFromTextInput>(this);
		cheatInputView.place(Gfx::viewportRect());
		modalView = &cheatInputView;
		//viewStack.pushAndShow(&cheatInputView);
	}

	void removeHandler(TextMenuItem &item, const InputEvent &e)
	{
		assert(fceuCheats != 0);
		FCEUI_DelCheat(idx);
		fceuCheats--;
		refreshCheatViews();
		viewStack.popAndShow();
	}

public:
	constexpr EditCheatView(): BaseMenuView("")	{ }

	void init(bool highlightFirst, int cheatIdx)
	{
		idx = cheatIdx;
		uint32 a;
		uint8 v;
		int compare;
		int gotCheat = FCEUI_GetCheat(cheatIdx, &nameStr, &a, &v, &compare, 0, &type);
		logMsg("got cheat with addr 0x%.4x val 0x%.2x comp %d", a, v, compare);

		uint i = 0;
		name.init(nameStr); item[i++] = &name;
		name.selectDelegate().bind<EditCheatView, &EditCheatView::nameHandler>(this);
		if(type)
		{
			name_ = "Edit Code";
			if(a == 0 && v == 0 && compare == -1)
				ggCodeStr[0] = 0;
			else
				EncodeGG(ggCodeStr, a, v, compare);
			ggCode.init("GG Code", ggCodeStr); item[i++] = &ggCode;
			ggCode.selectDelegate().bind<EditCheatView, &EditCheatView::ggCodeHandler>(this);
		}
		else
		{
			name_ = "Edit RAM Patch";
			snprintf(addrStr, sizeof(addrStr), "%x", a);
			addr.init("Address", addrStr); item[i++] = &addr;
			addr.selectDelegate().bind<EditCheatView, &EditCheatView::addrHandler>(this);
			snprintf(valueStr, sizeof(valueStr), "%x", v);
			value.init("Value", valueStr); item[i++] = &value;
			value.selectDelegate().bind<EditCheatView, &EditCheatView::valHandler>(this);
			if(compare == -1)
				compStr[0] = 0;
			else
				snprintf(compStr, sizeof(compStr), "%x", compare);
			comp.init("Compare", compStr); item[i++] = &comp;
			comp.selectDelegate().bind<EditCheatView, &EditCheatView::compHandler>(this);
		}
		remove.init("Delete Cheat"); item[i++] = &remove;
		remove.selectDelegate().bind<EditCheatView, &EditCheatView::removeHandler>(this);
		assert(i <= sizeofArray(item));
		BaseMenuView::init(item, i, highlightFirst);
	}
};

static EditCheatView editCheatView;

class EditCheatListView : public BaseMenuView
{
private:
	TextMenuItem addGG;
	TextMenuItem addRAM;
	TextMenuItem cheat[maxCheats];
	uchar addCheatType = 0;
	MenuItem *item[maxCheats + 2] = {nullptr};

	uint handleNameFromTextInput(const char *str)
	{
		if(str)
		{
			if(!FCEUI_AddCheat(str, 0, 0, -1, addCheatType))
			{
				logErr("error adding new cheat");
				viewStack.popAndShow();
				return 0;
			}
			fceuCheats++;
			FCEUI_ToggleCheat(fceuCheats-1);
			logMsg("added new cheat, %d total", fceuCheats);
			//viewStack.pop();
			removeModalView();
			refreshCheatViews();
			editCheatView.init(0, fceuCheats-1);
			viewStack.pushAndShow(&editCheatView);
		}
		else
		{
			removeModalView();
			//viewStack.popAndShow();
		}
		return 0;
	}

	void addGGHandler(TextMenuItem &item, const InputEvent &e)
	{
		addCheatType = 1;
		cheatInputView.init("Input name");
		cheatInputView.onTextDelegate().bind<EditCheatListView, &EditCheatListView::handleNameFromTextInput>(this);
		cheatInputView.place(Gfx::viewportRect());
		modalView = &cheatInputView;
		//viewStack.pushAndShow(&cheatInputView);
	}

	void addRAMHandler(TextMenuItem &item, const InputEvent &e)
	{
		addCheatType = 0;
		cheatInputView.init("Input name");
		cheatInputView.onTextDelegate().bind<EditCheatListView, &EditCheatListView::handleNameFromTextInput>(this);
		cheatInputView.place(Gfx::viewportRect());
		modalView = &cheatInputView;
		//viewStack.pushAndShow(&cheatInputView);
	}

public:
	constexpr EditCheatListView(): BaseMenuView("Edit Cheats") { }

	void onSelectElement(const GuiTable1D *table, const InputEvent &e, uint i)
	{
		if(i < 2)
			item[i]->select(this, e);
		else
		{
			editCheatView.init(!e.isPointer(), i - 2);
			viewStack.pushAndShow(&editCheatView);
		}
	}

	void init(bool highlightFirst)
	{
		uint i = 0;
		addGG.init("Add Game Genie Code"); item[i++] = &addGG;
		addGG.selectDelegate().bind<EditCheatListView, &EditCheatListView::addGGHandler>(this);
		addRAM.init("Add RAM Patch"); item[i++] = &addRAM;
		addRAM.selectDelegate().bind<EditCheatListView, &EditCheatListView::addRAMHandler>(this);
		int cheats = IG::min(fceuCheats, (uint)sizeofArray(cheat));
		iterateTimes(cheats, c)
		{
			char *name;
			int gotCheat = FCEUI_GetCheat(c, &name, 0, 0, 0, 0, 0);
			assert(gotCheat);
			if(!gotCheat) continue;
			cheat[c].init(name); item[i++] = &cheat[c];
		}
		assert(i <= sizeofArray(item));
		BaseMenuView::init(item, i, highlightFirst);
	}
};

static EditCheatListView editCheatListView;

class CheatsView : public BaseMenuView
{
private:
	TextMenuItem edit;

	void editHandler(TextMenuItem &item, const InputEvent &e)
	{
		editCheatListView.init(!e.isPointer());
		viewStack.pushAndShow(&editCheatListView);
	}

	struct CheatMenuItem : public BoolMenuItem
	{
		constexpr CheatMenuItem(): idx(0) { }
		int idx;
		void init(int idx, const char *name, bool on) { BoolMenuItem::init(name, on); var_selfs(idx); }

		void select(View *view, const InputEvent &e)
		{
			toggle();
			FCEUI_ToggleCheat(idx);
		}
	} cheat[maxCheats];

	MenuItem *item[maxCheats + 1] = {nullptr};
public:
	constexpr CheatsView(): BaseMenuView("Cheats") { }
	void init(bool highlightFirst)
	{
		uint i = 0;
		edit.init("Add/Edit"); item[i++] = &edit;
		edit.selectDelegate().bind<CheatsView, &CheatsView::editHandler>(this);
		int cheats = IG::min(fceuCheats, (uint)sizeofArray(cheat));
		iterateTimes(cheats, c)
		{
			char *name;
			int status;
			int gotCheat = FCEUI_GetCheat(c, &name, 0, 0, 0, &status, 0);
			assert(gotCheat);
			if(!gotCheat) continue;
			cheat[c].init(c, name, status); item[i++] = &cheat[c];
		}
		assert(i <= sizeofArray(item));
		BaseMenuView::init(item, i, highlightFirst);
	}
};

class FDSControlView : public BaseMenuView
{
private:
	struct SetSideMenuItem : public TextMenuItem
	{
		constexpr SetSideMenuItem() { }
		int side = 0;
		void init(const char *sideStr, int side)
		{
			TextMenuItem::init(sideStr, side < FCEU_FDSSides());
			this->side = side;
		}

		void select(View *view, const InputEvent &e)
		{
			if(side < FCEU_FDSSides())
			{
				FCEU_FDSSetDisk(side);
				viewStack.popAndShow();
			}
		}
	} setSide[4];

	struct InsertEjectMenuItem : public TextMenuItem
	{
		constexpr InsertEjectMenuItem() { }
		void init()
		{
			TextMenuItem::init("Eject", FCEU_FDSInserted());
		}

		void select(View *view, const InputEvent &e)
		{
			if(FCEU_FDSInserted())
			{
				FCEU_FDSInsert();
				viewStack.popAndShow();
			}
		}
	} insertEject;

	MenuItem *item[5] = {nullptr}; //sizeofArrayConst(setSide) + 2 not accepted by older GCC
public:
	constexpr FDSControlView(): BaseMenuView("FDS Control") { }

	void init(bool highlightFirst)
	{
		uint i = 0;
		setSide[0].init("Set Disk 1 Side A", 0); item[i++] = &setSide[0];
		setSide[1].init("Set Disk 1 Side B", 1); item[i++] = &setSide[1];
		setSide[2].init("Set Disk 2 Side A", 2); item[i++] = &setSide[2];
		setSide[3].init("Set Disk 2 Side B", 3); item[i++] = &setSide[3];
		insertEject.init(); item[i++] = &insertEject;
		assert(i <= sizeofArray(item));
		BaseMenuView::init(item, i, highlightFirst);
	}
};

static FDSControlView fdsMenu;
static CheatsView cheatsMenu;

static void refreshCheatViews()
{
	editCheatListView.deinit();
	editCheatListView.init(0);
	cheatsMenu.deinit();
	cheatsMenu.init(0);
}

class NesMenuView : public MenuView
{
private:
	struct FDSControlMenuItem : public TextMenuItem
	{
		char label[sizeof("FDS Control (Disk 1:A)")];
		void init()
		{
			strcpy(label, "");
			TextMenuItem::init(label);
		}

		void refreshActive()
		{
			active = isFDS;
			if(!isFDS)
				strcpy(label, "FDS Control");
			else if(!FCEU_FDSInserted())
				strcpy(label, "FDS Control (No Disk)");
			else
				sprintf(label, "FDS Control (Disk %d:%c)", (FCEU_FDSCurrentSide()>>1)+1, (FCEU_FDSCurrentSide() & 1)? 'B' : 'A');
			compile();
		}

		void select(View *view, const InputEvent &e)
		{
			if(EmuSystem::gameIsRunning() && isFDS)
			{
				fdsMenu.init(!e.isPointer());
				viewStack.pushAndShow(&fdsMenu);
			}
			else
				popup.post("Disk System not in use", 2);
		}
	} fdsControl;

	struct CheatsMenuItem : public TextMenuItem
	{
		void init()
		{
			TextMenuItem::init("Cheats");
		}

		void refreshActive()
		{
			active = EmuSystem::gameIsRunning() /*&& fceuCheats*/;
		}

		void select(View *view, const InputEvent &e)
		{
			if(EmuSystem::gameIsRunning())
			{
				//if(fceuCheats)
				{
					cheatsMenu.init(!e.isPointer());
					viewStack.pushAndShow(&cheatsMenu);
				}
				//else
				//	popup.printf(5, 1, "Place a valid .cht file in the same directory as the game named %s.cht", EmuSystem::gameName);
			}
		}
	} cheats;

	MenuItem *item[STANDARD_ITEMS + 2];

public:

	void onShow()
	{
		MenuView::onShow();
		cheats.refreshActive();
		fdsControl.refreshActive();
	}

	void init(bool highlightFirst)
	{
		uint items = 0;
		loadFileBrowserItems(item, items);
		fdsControl.init(); item[items++] = &fdsControl;
		cheats.init(); item[items++] = &cheats;
		loadStandardItems(item, items);
		assert(items <= sizeofArray(item));
		BaseMenuView::init(item, items, highlightFirst);
	}
};
