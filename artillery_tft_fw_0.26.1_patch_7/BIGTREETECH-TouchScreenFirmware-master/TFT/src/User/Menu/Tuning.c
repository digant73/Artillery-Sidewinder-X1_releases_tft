#include "Tuning.h"
#include "includes.h"

// 1 title, ITEM_PER_PAGE items(icon + label)
MENUITEMS TuningItems = {
// title
LABEL_TUNING,
// icon                       label
 {{ICON_PID,                  LABEL_PID},
  {ICON_EXTRUDE_100,          LABEL_EXTRUDE_100},
  {ICON_LEVELING,             LABEL_ABL},
  {ICON_MANUAL_LEVEL,         LABEL_LEVELING},
  {ICON_PARAMETER,            LABEL_PARAMETER_SETTING},
  {ICON_GCODE,                LABEL_TERMINAL},
  {ICON_EEPROM_SAVE,          LABEL_EEPROM_SAVE},
  {ICON_BACK,                 LABEL_BACK},}
};

/* Initialize menu */
void tunInitMenu(void)
{
  if(infoMachineSettings.autoLevel == 1)
  {
    TuningItems.items[KEY_ICON_2].icon = ICON_LEVELING;
    TuningItems.items[KEY_ICON_2].label.index = LABEL_ABL;
    TuningItems.items[KEY_ICON_3].icon = ICON_MANUAL_LEVEL;
    TuningItems.items[KEY_ICON_3].label.index = LABEL_LEVELING;
  }
  else
  {
    TuningItems.items[KEY_ICON_2].icon = ICON_MANUAL_LEVEL;
    TuningItems.items[KEY_ICON_2].label.index = LABEL_LEVELING;
    TuningItems.items[KEY_ICON_3].icon = ICON_BACKGROUND;
    TuningItems.items[KEY_ICON_3].label.index = LABEL_BACKGROUND;
  }
}

void menuTuning(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  tunInitMenu();

  menuDrawPage(&TuningItems);

  while(infoMenu.menu[infoMenu.cur] == menuTuning)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuPid;
        break;

      case KEY_ICON_1:
        storeCmd("G90\nG0 F3000 X0 Y0 Z100\n");            // present extruder
        storeCmd("M83\nG1 F50\nG1 E100\nM82\n");           // extrude
        break;

      case KEY_ICON_2:
        if(infoMachineSettings.autoLevel == 1)
        {
          infoMenu.menu[++infoMenu.cur] = menuAutoLeveling;
        }
        else
        {
          infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
        }
        break;

      case KEY_ICON_3:
        if(infoMachineSettings.autoLevel == 1)
        {
          infoMenu.menu[++infoMenu.cur] = menuManualLeveling;
        }
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuParameterSettings;
        break;

      case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuSendGcode;
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuEepromSave;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
