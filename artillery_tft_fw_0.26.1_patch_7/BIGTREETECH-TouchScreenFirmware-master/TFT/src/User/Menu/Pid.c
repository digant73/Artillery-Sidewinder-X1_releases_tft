#include "Pid.h"
#include "includes.h"

// 1 title, ITEM_PER_PAGE items (icon + label)
MENUITEMS pidItems = {
// title
LABEL_PID_TITLE,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_PID_START,            LABEL_PID_START},
  {ICON_BACK,                 LABEL_BACK},}
};

const ITEM pidItemTool[] = {
// icon                       label
  {ICON_BED,                  LABEL_BED},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
  {ICON_NOZZLE,               LABEL_NOZZLE},
};

#define ITEM_DEGREE_NUM 3

const ITEM pidItemDegree[ITEM_DEGREE_NUM] = {
// icon                       label
  {ICON_1_DEGREE,             LABEL_1_DEGREE},
  {ICON_5_DEGREE,             LABEL_5_DEGREE},
  {ICON_10_DEGREE,            LABEL_10_DEGREE},
};

const u8 pid_item_degree[ITEM_DEGREE_NUM] = {1, 5, 10};
u8       pid_item_degree_i = 1;

const char* const pidDisplayID[] = HEAT_DISPLAY_ID;
const char*       pidCmd[] = PID_CMD;

HEATER pidHeater = {{}, NOZZLE0, NOZZLE0};
bool pidInitialized = false;

/* Initialize target temperatures */
void pidInitTargetTemps(void)
{
  if(!pidInitialized)
  {
    for(TOOL i = BED; i < HEATER_COUNT; i++)
    {
      pidHeater.T[i].target = 0;
    }

    pidInitialized = true;
  }
}

/* Decrease target temperature */
void pidDecTargetTemp(void)
{
  if(pidHeater.T[pidHeater.tool].target > 0)
  {
    pidHeater.T[pidHeater.tool].target =
      limitValue(0, pidHeater.T[pidHeater.tool].target - pid_item_degree[pid_item_degree_i], infoSettings.max_temp[pidHeater.tool]);
  }
}

/* Increase target temperature */
void pidIncTargetTemp(void)
{
  if(pidHeater.T[pidHeater.tool].target < infoSettings.max_temp[pidHeater.tool])
  {
    pidHeater.T[pidHeater.tool].target =
      limitValue(0, pidHeater.T[pidHeater.tool].target + pid_item_degree[pid_item_degree_i], infoSettings.max_temp[pidHeater.tool]);
  }
}

/* Set next tool */
void pidSetNextTool(void)
{
  pidHeater.tool = (TOOL)((pidHeater.tool + 1) % HEATER_COUNT);
  pidItems.items[KEY_ICON_4] = pidItemTool[pidHeater.tool];
}

/* Set next degree */
void pidSetNextDegree(void)
{
  pid_item_degree_i = (pid_item_degree_i + 1) % ITEM_DEGREE_NUM;
  pidItems.items[KEY_ICON_5] = pidItemDegree[pid_item_degree_i];
}

void pidTemperatureReDraw(bool skip_header)
{
  char tempstr[20];

  if (!skip_header)
  {
    sprintf(tempstr, "%s    ", pidDisplayID[pidHeater.tool]);

    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  }

  sprintf(tempstr, "  %d  ", pidHeater.T[pidHeater.tool].target);

  setLargeFont(true);

  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);

  setLargeFont(false);
}

void menuPidStart(void)
{
  u16 key_num = IDLE_TOUCH;

  popupDrawPage(DIALOG_TYPE_ALERT, bottomDoubleBtn, textSelect(LABEL_WARNING), textSelect(LABEL_PID_START_DIALOG), textSelect(LABEL_CONFIRM), textSelect(LABEL_CANCEL));

  while(infoMenu.menu[infoMenu.cur] == menuPidStart)
  {
    key_num = KEY_GetValue(2, doubleBtnRect);
    switch(key_num)
    {
      case KEY_POPUP_CONFIRM:
        storeCmd("M42 P4 S0\nM42 P5 S255\nM42 P6 S0\n");                       // set LED light to RED
        storeCmd("M106 S255\n");                                               // set fan speed to max
        storeCmd("G4 S1\n");                                                   // wait 1 sec

        for(TOOL i = BED; i < HEATER_COUNT; i++)                               // bed + nozzles
        {
          if (pidHeater.T[i].target > 0)
          {
            storeCmd("%s S%d\n", pidCmd[i], (int) pidHeater.T[i].target);      // start PID autotune
            storeCmd("G4 S1\n");                                               // wait 1 sec
          }
        }

        storeCmd("M107\n");                                                    // stop fan
        storeCmd("M42 P4 S255\nM42 P5 S0\nM42 P6 S0\n");                       // set LED light to GREEN

        GUI_Clear(BACKGROUND_COLOR);
        GUI_DispStringInRect(20, 0, LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_PID_START_INFO));
        GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT * 2), LCD_WIDTH - 20, LCD_HEIGHT, textSelect(LABEL_PID_START_INFO_EXTRA));

        while(!isPress())
        {
          loopProcess();
        }
        while(isPress())
        {
          loopProcess();
        }

        infoMenu.cur--;
        break;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        break;
    }

    loopProcess();
  }
}

void menuPid(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  pidInitTargetTemps();

  menuDrawPage(&pidItems);
  pidTemperatureReDraw(false);

  while(infoMenu.menu[infoMenu.cur] == menuPid)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        pidDecTargetTemp();
        pidTemperatureReDraw(true);
        break;

      case KEY_ICON_3:
        pidIncTargetTemp();
        pidTemperatureReDraw(true);
        break;

      case KEY_ICON_4:
        pidSetNextTool();
        menuDrawItem(&pidItems.items[key_num], key_num);
        pidTemperatureReDraw(false);
        break;

      case KEY_ICON_5:
        pidSetNextDegree();
        menuDrawItem(&pidItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuPidStart;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        break;
    }

    loopProcess();
  }
}
