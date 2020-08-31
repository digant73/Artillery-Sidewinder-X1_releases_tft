#include "ConnectionSettings.h"
#include "includes.h"

// 1 title, ITEM_PER_PAGE items (icon + label)
MENUITEMS connectionSettingsItems = {
// title
LABEL_CONNECTION_SETTINGS,
// icon                       label
 {{ICON_BAUD_RATE,            LABEL_BACKGROUND},
  {ICON_DISCONNECT,           LABEL_DISCONNECT},
  {ICON_STOP,                 LABEL_EMERGENCYSTOP},
  {ICON_SHUT_DOWN,            LABEL_SHUT_DOWN},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};

const ITEM bauItemBaudrate[ITEM_BAUDRATE_NUM] = {
// icon                       label
  {ICON_BAUD_RATE,            {.address = "2400"}},
  {ICON_BAUD_RATE,            {.address = "9600"}},
  {ICON_BAUD_RATE,            {.address = "19200"}},
  {ICON_BAUD_RATE,            {.address = "38400"}},
  {ICON_BAUD_RATE,            {.address = "57600"}},
  {ICON_BAUD_RATE,            {.address = "115200"}},
  {ICON_BAUD_RATE,            {.address = "250000"}},
  {ICON_BAUD_RATE,            {.address = "500000"}},
  {ICON_BAUD_RATE,            {.address = "1000000"}},
};

const u32 item_baudrate[ITEM_BAUDRATE_NUM] = {2400, 9600, 19200, 38400, 57600, 115200, 250000, 500000, 1000000};
u8        bau_item_baudrate_i = 0;

/* Initialize baudrate menu */
void bauInitMenuBaudrate(void)
{
  for(u8 i = 0; i < ITEM_BAUDRATE_NUM; i++)
  {
    if(infoSettings.baudrate == item_baudrate[i])
    {
      bau_item_baudrate_i = i;
      connectionSettingsItems.items[KEY_ICON_0] = bauItemBaudrate[bau_item_baudrate_i];
    }
  }
}

/* Set next baudrate */
void bauSetNextBaudrate(void)
{
  bau_item_baudrate_i = (bau_item_baudrate_i + 1) % ITEM_BAUDRATE_NUM;
  infoSettings.baudrate = item_baudrate[bau_item_baudrate_i];

  connectionSettingsItems.items[KEY_ICON_0] = bauItemBaudrate[bau_item_baudrate_i];

  Serial_ReSourceDeInit();             // Serial_Init() will malloc a dynamic memory, so Serial_DeInit() first to free, then malloc again.
  Serial_ReSourceInit();
  reminderMessage(LABEL_UNCONNECTED, STATUS_UNCONNECT);
}

// Set uart pins to input, free uart
void menuDisconnect(void)
{
  GUI_Clear(infoSettings.bg_color);
  GUI_DispStringInRect(20, 0, LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_DISCONNECT_INFO));
  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  Serial_ReSourceDeInit();
  while(!isPress());
  while(isPress());
  Serial_ReSourceInit();

  infoMenu.cur--;
}

void menuConnectionSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  bauInitMenuBaudrate();

  menuDrawPage(&connectionSettingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuConnectionSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        bauSetNextBaudrate();    
        menuDrawItem(&connectionSettingsItems.items[key_num], key_num);
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
        break;

      case KEY_ICON_2:                 // Emergency Stop : Used for emergency stopping, a reset is required to return to operational mode.
        storeCmd("M112\n");            // it may need to wait for a space to open up in the command queue.
        break;                         // Enable EMERGENCY_PARSER in Marlin Firmware for an instantaneous M112 command.

      case KEY_ICON_3:
        storeCmd("M81\n");
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default :
        break;
    }

    loopProcess();
  }

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
