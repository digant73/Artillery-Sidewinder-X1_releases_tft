#include "FeatureSettings.h"
#include "includes.h"

static uint16_t fe_cur_page = 0;

//
//parameter values
//

#define ITEM_TOGGLE_AUTO_NUM 3
const LABEL itemToggleAuto[ITEM_TOGGLE_AUTO_NUM] =
{
  LABEL_OFF,
  LABEL_ON,
  LABEL_AUTO
};

#define ITEM_TOGGLE_SMART_NUM 2
const LABEL itemToggleSmart[ITEM_TOGGLE_SMART_NUM] =
{
  LABEL_ON,
  LABEL_SMART
};

#define ITEM_NOTIFICATION_TYPE_NUM 3
const char *const itemNotificationType[ITEM_NOTIFICATION_TYPE_NUM] =
{
  //item value text(only for custom value)
  "OFF",
  "POPUP",
  "TOAST"
};

const char *const itemSortBy[SORT_BY_COUNT] =
{
  //item value text(only for custom value)
  "Date ▼",
  "Date ▲",
  "Name ▲",
  "Name ▼",
};

//
//add key number index of the items
//
typedef enum
{
  SKEY_TERMINAL_ACK = 0,
  SKEY_PERSISTENT_INFO,
  SKEY_FAN_SPEED_PERCENT,
  SKEY_FILE_LIST_MODE,
  SKEY_FILE_SORT_BY,
  SKEY_ACK_NOTIFICATION,
  SKEY_EMULATE_M600,
  SKEY_SERIAL_ALWAYS_ON,
  SKEY_SPEED,
  SKEY_AUTO_LOAD_LEVELING,
  SKEY_XY_OFFSET_PROBING,
  SKEY_Z_STEPPERS_ALIGNMENT,

  #ifdef PS_ON_PIN
    SKEY_PS_ON,
  #endif

  #ifdef FIL_RUNOUT_PIN
    SKEY_FIL_RUNOUT,
  #endif

  SKEY_PL_RECOVERY_EN,
  SKEY_PL_RECOVERY_HOME,
  SKEY_BTT_MINI_UPS,

  #ifdef LED_COLOR_PIN
    SKEY_KNOB_LED_COLOR,

    #ifdef LCD_LED_PWM_CHANNEL
      SKEY_KNOB_LED_IDLE,
    #endif
  #endif

  SKEY_START_GCODE_ENABLED,
  SKEY_END_GCODE_ENABLED,
  SKEY_CANCEL_GCODE_ENABLED,
  SKEY_RESET_SETTINGS,        // Keep reset always at the bottom of the settings menu list.
  SKEY_COUNT                  // keep this always at the end
} SKEY_LIST;

//
//perform action on button press
//
void updateFeatureSettings(uint8_t item_index)
{
  switch (item_index)
  {
    case SKEY_TERMINAL_ACK:
      infoSettings.terminalACK = (infoSettings.terminalACK + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_PERSISTENT_INFO:
      infoSettings.persistent_info = (infoSettings.persistent_info + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_FAN_SPEED_PERCENT:
      infoSettings.fan_percentage = (infoSettings.fan_percentage + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_FILE_LIST_MODE:
      infoSettings.file_listmode = (infoSettings.file_listmode + 1) % ITEM_TOGGLE_NUM;
      break;

      case SKEY_FILE_SORT_BY:
      infoSettings.files_sort_by = (infoSettings.files_sort_by + 1) % SORT_BY_COUNT;
      break;

    case SKEY_ACK_NOTIFICATION:
      infoSettings.ack_notification = (infoSettings.ack_notification + 1) % ITEM_NOTIFICATION_TYPE_NUM;
      break;

    case SKEY_EMULATE_M600:
      infoSettings.emulate_m600 = (infoSettings.emulate_m600 + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_SERIAL_ALWAYS_ON:
      infoSettings.serial_alwaysOn = (infoSettings.serial_alwaysOn + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_SPEED:
      infoSettings.move_speed = (infoSettings.move_speed + 1) % ITEM_SPEED_NUM;
      break;

    case SKEY_AUTO_LOAD_LEVELING:
      infoSettings.auto_load_leveling = (infoSettings.auto_load_leveling + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_XY_OFFSET_PROBING:
      infoSettings.xy_offset_probing = (infoSettings.xy_offset_probing + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_Z_STEPPERS_ALIGNMENT:
      infoSettings.z_steppers_alignment = (infoSettings.z_steppers_alignment + 1) % ITEM_TOGGLE_NUM;
      break;

    #ifdef PS_ON_PIN
      case SKEY_PS_ON:
        infoSettings.auto_off = (infoSettings.auto_off + 1) % ITEM_TOGGLE_AUTO_NUM;
        break;
    #endif

    #ifdef FIL_RUNOUT_PIN
      case SKEY_FIL_RUNOUT:
        infoSettings.runout ^= (1U << 0);
        break;
    #endif

    case SKEY_PL_RECOVERY_EN:
      infoSettings.powerloss_en = (infoSettings.powerloss_en + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_PL_RECOVERY_HOME:
      infoSettings.powerloss_home = (infoSettings.powerloss_home + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_BTT_MINI_UPS:
      infoSettings.btt_ups = (infoSettings.btt_ups + 1) % ITEM_TOGGLE_NUM;
      break;

    #ifdef LED_COLOR_PIN
      case SKEY_KNOB_LED_COLOR:
        infoSettings.knob_led_color = (infoSettings.knob_led_color + 1 ) % LED_COLOR_NUM;
        WS2812_Send_DAT(led_color[infoSettings.knob_led_color]);
        break;

      #ifdef LCD_LED_PWM_CHANNEL
        case SKEY_KNOB_LED_IDLE:
          infoSettings.knob_led_idle = (infoSettings.knob_led_idle + 1) % ITEM_TOGGLE_NUM;
          break;
      #endif  //LCD_LED_PWM_CHANNEL
    #endif

    case SKEY_START_GCODE_ENABLED:
      infoSettings.send_start_gcode = (infoSettings.send_start_gcode + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_END_GCODE_ENABLED:
      infoSettings.send_end_gcode = (infoSettings.send_end_gcode + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_CANCEL_GCODE_ENABLED:
      infoSettings.send_cancel_gcode = (infoSettings.send_cancel_gcode + 1) % ITEM_TOGGLE_NUM;
      break;

    case SKEY_RESET_SETTINGS:
      setDialogText(LABEL_SETTINGS_RESET, LABEL_SETTINGS_RESET_INFO, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_ALERT, resetSettings, NULL, NULL);
      break;

    default:
      return;
  }
}  //updateFeatureSettings

//
//load values on page change and reload
//
void loadFeatureSettings(LISTITEM * item, uint16_t item_index, uint8_t itemPos)
{
  if (item_index < SKEY_COUNT)
  {
    switch (item_index)
    {
      case SKEY_TERMINAL_ACK:
        item->icon = iconToggle[infoSettings.terminalACK];
        break;

      case SKEY_PERSISTENT_INFO:
        item->icon = iconToggle[infoSettings.persistent_info];
        break;

      case SKEY_FAN_SPEED_PERCENT:
        item->icon = iconToggle[infoSettings.fan_percentage];
        break;

      case SKEY_FILE_LIST_MODE:
        item->icon = iconToggle[infoSettings.file_listmode];
        break;

      case SKEY_FILE_SORT_BY:
        setDynamicTextValue(itemPos, (char *)itemSortBy[infoSettings.files_sort_by]);
        break;

      case SKEY_ACK_NOTIFICATION:
        setDynamicTextValue(itemPos, (char *)itemNotificationType[infoSettings.ack_notification]);
        break;

      case SKEY_EMULATE_M600:
        item->icon = iconToggle[infoSettings.emulate_m600];
        break;

      case SKEY_SERIAL_ALWAYS_ON:
        item->icon = iconToggle[infoSettings.serial_alwaysOn];
        break;

      case SKEY_SPEED:
        item->valueLabel = itemSpeed[infoSettings.move_speed].label;
        break;

      case SKEY_AUTO_LOAD_LEVELING:
        item->icon = iconToggle[infoSettings.auto_load_leveling];
        break;

      case SKEY_XY_OFFSET_PROBING:
        item->icon = iconToggle[infoSettings.xy_offset_probing];
        break;

      case SKEY_Z_STEPPERS_ALIGNMENT:
        item->icon = iconToggle[infoSettings.z_steppers_alignment];
        break;

      #ifdef PS_ON_PIN
        case SKEY_PS_ON:
          item->valueLabel = itemToggleAuto[infoSettings.auto_off];
          break;
      #endif

      #ifdef FIL_RUNOUT_PIN
        case SKEY_FIL_RUNOUT:
        {
          LABEL sensorLabel = itemToggleSmart[(infoSettings.runout >> 1) & 1];
          item->valueLabel.index = (infoSettings.runout & 1) ? sensorLabel.index : LABEL_OFF ;
          break;
        }
      #endif

      case SKEY_PL_RECOVERY_EN:
        item->icon = iconToggle[infoSettings.powerloss_en];
        break;

      case SKEY_PL_RECOVERY_HOME:
        item->icon = iconToggle[infoSettings.powerloss_home];
        break;

      case SKEY_BTT_MINI_UPS:
        item->icon = iconToggle[infoSettings.btt_ups];
        break;

      #ifdef LED_COLOR_PIN
        case SKEY_KNOB_LED_COLOR:
          item->valueLabel = itemLedcolor[infoSettings.knob_led_color];
          break;

        #ifdef LCD_LED_PWM_CHANNEL
          case SKEY_KNOB_LED_IDLE:
            item->icon = iconToggle[infoSettings.knob_led_idle];
            break;
        #endif
      #endif

      case SKEY_START_GCODE_ENABLED:
        item->icon = iconToggle[infoSettings.send_start_gcode];
        break;

      case SKEY_END_GCODE_ENABLED:
        item->icon = iconToggle[infoSettings.send_end_gcode];
        break;

      case SKEY_CANCEL_GCODE_ENABLED:
        item->icon = iconToggle[infoSettings.send_cancel_gcode];
        break;

      case SKEY_RESET_SETTINGS:
        break;

      default:
        break;
    }
  }
}  //loadFeatureSettings


void resetSettings(void)
{
  infoSettingsReset();
  storePara();
  popupReminder(DIALOG_TYPE_SUCCESS, LABEL_INFO, LABEL_SETTINGS_RESET_DONE);
}

void menuFeatureSettings(void)
{
  //
  //set item types
  //
  LISTITEM settingPage[SKEY_COUNT] = {
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_TERMINAL_ACK,           LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PERSISTENT_INFO,        LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_FAN_SPEED_PERCENT,      LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_FILE_LIST_MODE,         LABEL_BACKGROUND},
    {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_FILE_SORT_BY,           LABEL_DYNAMIC},
    {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_ACK_NOTIFICATION,       LABEL_DYNAMIC},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_EMULATE_M600,           LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_SERIAL_ALWAYS_ON,       LABEL_BACKGROUND},
    {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_MOVE_SPEED,             LABEL_NORMAL},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_AUTO_LOAD_LEVELING,     LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_XY_OFFSET_PROBING,      LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_Z_STEPPERS_ALIGNMENT,   LABEL_BACKGROUND},

    #ifdef PS_ON_PIN
      {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_PS_ON,                  LABEL_OFF},
    #endif

    #ifdef FIL_RUNOUT_PIN
      {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_FIL_RUNOUT,             LABEL_OFF},
    #endif

    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PL_RECOVERY_EN,         LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_PL_RECOVERY_HOME,       LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_BTT_MINI_UPS,           LABEL_BACKGROUND},

    #ifdef LED_COLOR_PIN
      {CHARICON_BLANK,       LIST_CUSTOMVALUE,   LABEL_KNOB_LED_COLOR,         LABEL_OFF},

      #ifdef LCD_LED_PWM_CHANNEL
        {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_KNOB_LED_IDLE,          LABEL_BACKGROUND},
      #endif
    #endif

    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_START_GCODE_ENABLED,    LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_END_GCODE_ENABLED,      LABEL_BACKGROUND},
    {CHARICON_TOGGLE_ON,   LIST_TOGGLE,        LABEL_CANCEL_GCODE_ENABLED,   LABEL_BACKGROUND},
    // Keep reset settings always at the bottom of the settings menu list.
    {CHARICON_BLANK,       LIST_MOREBUTTON,    LABEL_SETTINGS_RESET,         LABEL_BACKGROUND}
  };

  SETTINGS now = infoSettings;
  LABEL title = {LABEL_FEATURE_SETTINGS};
  uint16_t index = KEY_IDLE;
  listViewCreate(title, settingPage, SKEY_COUNT, &fe_cur_page, true, NULL, loadFeatureSettings);

  while (infoMenu.menu[infoMenu.cur] == menuFeatureSettings)
  {
    index = listViewGetSelectedIndex();

      if (index < SKEY_COUNT)
      {
        updateFeatureSettings(index);
        listViewRefreshItem(index);
      }

    loopProcess();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
