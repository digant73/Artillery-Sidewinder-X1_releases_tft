#include "XYOffset.h"
#include "includes.h"

// 1 title, ITEM_PER_PAGE items (icon + label)
MENUITEMS levItems = {
// title
LABEL_XY_OFFSET,
// icon                       label
 {{ICON_DEC,                  LABEL_DEC},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_INC,                  LABEL_INC},
  {ICON_XY_UNLOCK,            LABEL_XY_UNLOCK},
  {ICON_E_5_MM,               LABEL_5_MM},
  {ICON_DEFAULT,              LABEL_DEFAULT},
  {ICON_BACK,                 LABEL_BACK},}
};

#define ITEM_LEN_NUM 3

const ITEM levItemLen[ITEM_LEN_NUM] = {
// icon                       label
  {ICON_E_1_MM,               LABEL_1_MM},
  {ICON_E_5_MM,               LABEL_5_MM},
  {ICON_E_10_MM,              LABEL_10_MM},
};

const u8 lev_item_len[ITEM_LEN_NUM] = {1, 5, 10};
u8       lev_item_len_i = 1;

#define LEVELING_DISPLAY_ID     "X/Y"
#define LEVELING_MIN_OFFSET     10
#define LEVELING_MAX_OFFSET     100
#define LEVELING_DEFAULT_OFFSET LEVELING_EDGE_DISTANCE

s16 levOffset[2];

/* Initialize offset */
void levInitOffset(void)
{
  infoSettings.level_edge =
    limitValue(LEVELING_MIN_OFFSET, infoSettings.level_edge, LEVELING_MAX_OFFSET);

  levOffset[0] = infoSettings.level_edge;
  levOffset[1] = infoSettings.level_edge;
}

/* Decrease offset */
void levDecOffset(u8 axis)
{
  if (axis > 1)
    return;

  if(levOffset[axis] > LEVELING_MIN_OFFSET)
  {
    levOffset[axis] =
      limitValue(LEVELING_MIN_OFFSET, levOffset[axis] - lev_item_len[lev_item_len_i], LEVELING_MAX_OFFSET);

    infoSettings.level_edge = levOffset[axis];
  }
}

/* Increase offset */
void levIncOffset(u8 axis)
{
  if (axis > 1)
    return;

  if(levOffset[axis] < LEVELING_MAX_OFFSET)
  {
    levOffset[axis] =
      limitValue(LEVELING_MIN_OFFSET, levOffset[axis] + lev_item_len[lev_item_len_i], LEVELING_MAX_OFFSET);

    infoSettings.level_edge = levOffset[axis];
  }
}

/* Unlock XY axis */
void levUnlockXY(void)
{
  storeCmd("M84 X Y E\n");
}

/* Set next lenght */
void levSetNextLen(void)
{
  lev_item_len_i = (lev_item_len_i + 1) % ITEM_LEN_NUM;
  levItems.items[KEY_ICON_5] = levItemLen[lev_item_len_i];
}

/* Set default offset*/
void levSetDefaultOffset(void)
{
  for(int i = 0; i < 2; i++)
  {
    levOffset[i] = LEVELING_DEFAULT_OFFSET;

    infoSettings.level_edge = levOffset[i];
  }
}

void levOffsetReDraw(bool skip_header)
{
  char tempstr[20];

  if (!skip_header)
  {
    sprintf(tempstr, "%s    ", LEVELING_DISPLAY_ID);

    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (u8 *)tempstr);
  }

  sprintf(tempstr, "  %d%s%d  ", levOffset[0], "/", levOffset[1]);

  setLargeFont(true);

  GUI_DispStringInPrect(&exhibitRect, (u8 *)tempstr);

  setLargeFont(false);
}

void menuXYOffset(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  while(infoCmd.count != 0)
  {
    loopProcess();
  }

  levInitOffset();

  menuDrawPage(&levItems);
  levOffsetReDraw(false);

  while(infoMenu.menu[infoMenu.cur] == menuXYOffset)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        levDecOffset(0);
        levDecOffset(1);
        levOffsetReDraw(true);
        break;

      case KEY_ICON_3:
        levIncOffset(0);
        levIncOffset(1);
        levOffsetReDraw(true);
        break;

      case KEY_ICON_4:
        levUnlockXY();
        break;

      case KEY_ICON_5:
        levSetNextLen();
        menuDrawItem(&levItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        levSetDefaultOffset();
        levOffsetReDraw(true);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
