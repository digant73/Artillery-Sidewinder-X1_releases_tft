#include "includes.h"
#include "parseACK.h"

char dmaL2Cache[ACK_MAX_SIZE];
static u16 ack_index=0;
static u8 ack_cur_src = SERIAL_PORT;

bool portSeen[_UART_CNT] = {false, false, false, false, false, false};

struct HOST_ACTION
{
  char prompt_begin[20];
  char prompt_button1[20];
  char prompt_button2[20];
  bool prompt_show;         //Show popup reminder or not
  uint8_t button;           //Number of buttons
} hostAction;

// notify or ignore messages starting with following text
const ECHO knownEcho[] = {
//  {ECHO_NOTIFY_NONE, "enqueueing \"M117\""},
  {ECHO_NOTIFY_NONE, "busy: paused for user"},
  {ECHO_NOTIFY_NONE, "busy: processing"},
  {ECHO_NOTIFY_NONE, "Now fresh file:"},
  {ECHO_NOTIFY_NONE, "Now doing file:"},
  {ECHO_NOTIFY_NONE, "Probe Offset"},
  {ECHO_NOTIFY_NONE, "Flow:"},
  {ECHO_NOTIFY_NONE, "echo:;"},                 //M503
  {ECHO_NOTIFY_NONE, "echo:  G"},               //M503
  {ECHO_NOTIFY_NONE, "echo:  M"},               //M503
  {ECHO_NOTIFY_NONE, "Cap:"},                   //M115
  {ECHO_NOTIFY_NONE, "Config:"},                //M360
  {ECHO_NOTIFY_TOAST, "Settings Stored"},       //M500
  {ECHO_NOTIFY_TOAST, "echo:Soft endstops"},    //M211
  {ECHO_NOTIFY_TOAST, "echo:Bed"},              //M420
  {ECHO_NOTIFY_TOAST, "echo:Fade"},             //M420
  {ECHO_NOTIFY_TOAST, "echo:Active Extruder"},  //Tool Change
};

uint8_t forceIgnore[ECHO_ID_COUNT] = {0};

void setCurrentAckSrc(uint8_t src)
{
  ack_cur_src = src;
  portSeen[src] = true;
}

static char ack_seen(const char *str)
{
  u16 i;
  for(ack_index=0; ack_index<ACK_MAX_SIZE && dmaL2Cache[ack_index]!=0; ack_index++)
  {
    for(i=0; str[i]!=0 && dmaL2Cache[ack_index+i]!=0 && dmaL2Cache[ack_index+i]==str[i]; i++)
    {}
    if(str[i]==0)
    {
      ack_index += i;
      return true;
    }
  }
  return false;
}

static char ack_cmp(const char *str)
{
  u16 i;
  for(i=0; i<ACK_MAX_SIZE && str[i]!=0 && dmaL2Cache[i]!=0; i++)
  {
    if(str[i] != dmaL2Cache[i])
    return false;
  }
  if(dmaL2Cache[i] != 0) return false;
  return true;
}

static float ack_value()
{
  return (strtod(&dmaL2Cache[ack_index], NULL));
}

// Read the value after the / if exists
static float ack_second_value()
{
  char *secondValue = strchr(&dmaL2Cache[ack_index],'/');
  if(secondValue != NULL)
  {
    return (strtod(secondValue+1, NULL));
  }
  else
  {
    return -0.5;
  }
}

void ackPopupInfo(const char *info)
{
  DIALOG_TYPE d_type = DIALOG_TYPE_ERROR;

  // play notification sound if buzzer for ACK is enabled

  if (info == errormagic)
    BUZZER_PLAY(sound_error);
  else if (info == echomagic && infoSettings.ack_notification == 1)
    BUZZER_PLAY(sound_notify);

  // set echo message in status screen
  if (info == echomagic)
  {
    //ignore all messages if parameter settings is open
    if (infoMenu.menu[infoMenu.cur] == menuParameterSettings)
      return;

    d_type = DIALOG_TYPE_INFO;
    statusScreen_setMsg((u8 *)info, (u8 *)dmaL2Cache + ack_index);
  }

  if (infoMenu.menu[infoMenu.cur] == menuTerminal ||
      (infoMenu.menu[infoMenu.cur] == menuStatus && info == echomagic))
    return;

 //show notification based on notificaiton settings
  if (infoSettings.ack_notification == 1 ||  info == errormagic)
  {
    popupReminder(d_type, (u8 *) info, (u8 *) dmaL2Cache + ack_index);
  }
  else if(infoSettings.ack_notification == 2)
    addToast(DIALOG_TYPE_INFO, dmaL2Cache); //show toast notificaion if turned on
}


void setIgnoreEcho(ECHO_ID msgId, bool state)
{
  forceIgnore[msgId] = state;
}

bool processKnownEcho(void)
{
  bool isKnown = false;
  u8 i;

  for (i = 0; i < COUNT(knownEcho); i++)
  {
    if (strstr(dmaL2Cache, knownEcho[i].msg))
    {
      isKnown = true;
      break;
    }
  }

  // display the busy indicator
  busyIndicator(STATUS_BUSY);

  if (isKnown)
  {
    if (knownEcho[i].notifyType == ECHO_NOTIFY_NONE)
      return isKnown;
    if (forceIgnore[i] == 0)
    {
      if (knownEcho[i].notifyType == ECHO_NOTIFY_TOAST)
        addToast(DIALOG_TYPE_INFO, dmaL2Cache);
      else if (knownEcho[i].notifyType == ECHO_NOTIFY_DIALOG)
      {
        BUZZER_PLAY(sound_notify);
        popupReminder(DIALOG_TYPE_INFO, (u8 *)echomagic, (u8 *)dmaL2Cache + ack_index);
      }
      // display the echo message in the status bar
      statusScreen_setMsg((u8 *)echomagic, (u8 *)dmaL2Cache + ack_index);
    }
  }
  return isKnown;
}

bool dmaL1NotEmpty(uint8_t port)
{
  return dmaL1Data[port].rIndex != dmaL1Data[port].wIndex;
}

void syncL2CacheFromL1(uint8_t port)
{
  uint16_t i = 0;
  while (dmaL1NotEmpty(port))
  {
    dmaL2Cache[i] = dmaL1Data[port].cache[dmaL1Data[port].rIndex];
    dmaL1Data[port].rIndex = (dmaL1Data[port].rIndex + 1) % DMA_TRANS_LEN;
    if (dmaL2Cache[i++] == '\n') break;
  }
  dmaL2Cache[i] = 0; // End character
}

void hostActionCommands(void)
{
  char *find = strchr(dmaL2Cache + ack_index, '\n'); 
  *find = '\0';
  if(ack_seen("prompt_begin "))
  {
    hostAction.button = 0;
    hostAction.prompt_show = 1;
    strcpy(hostAction.prompt_begin, dmaL2Cache + ack_index);
    if(ack_seen("Resuming SD"))
    {
      hostAction.prompt_show = 0;
    }
    else if(ack_seen("Resuming"))
    {
      infoPrinting.pause = false;
      hostAction.prompt_show = 0;
    }
    else if(ack_seen("Reheating"))
    {
      hostAction.prompt_show = 0;
    }
    else if(ack_seen("Nozzle Parked"))
    {
      infoPrinting.pause = true;
    }
  }
  else if(ack_seen("prompt_button "))
  {
    hostAction.button++;
    if(hostAction.button == 1)
    {
      strcpy(hostAction.prompt_button1, dmaL2Cache + ack_index);
    }
    else
    {
      strcpy(hostAction.prompt_button2, dmaL2Cache + ack_index);
    }
  }

  if(ack_seen("prompt_show") && hostAction.prompt_show)
  {
    switch(hostAction.button)
    {
      case 0:
        BUZZER_PLAY(sound_notify);
        popupReminder(DIALOG_TYPE_ALERT,(u8 *)"Message", (u8 *)hostAction.prompt_begin);
        break;
      case 1:
        BUZZER_PLAY(sound_notify);
        showDialog(DIALOG_TYPE_ALERT, (u8*)"Action command", (u8 *)hostAction.prompt_begin, (u8 *)hostAction.prompt_button1, NULL, breakAndContinue, NULL, NULL);
        break;
      case 2:
        BUZZER_PLAY(sound_notify);
        showDialog(DIALOG_TYPE_ALERT, (u8*)"Action command", (u8 *)hostAction.prompt_begin, (u8 *)hostAction.prompt_button1, (u8 *)hostAction.prompt_button2, resumeAndPurge, resumeAndContinue, NULL);
        break;
    }
  }
}

void parseACK(void)
{
  if (infoHost.rx_ok[SERIAL_PORT] != true) return; //not get response data

  while (dmaL1NotEmpty(SERIAL_PORT))
  {
    bool avoid_terminal = false;
    syncL2CacheFromL1(SERIAL_PORT);
    infoHost.rx_ok[SERIAL_PORT] = false;

    if (infoHost.connected == false) //not connected to Marlin
    {
      // Parse error information even though not connected to printer
      if (ack_seen(errormagic)) {
        ackPopupInfo(errormagic);
      }
      if (!(ack_seen("@") && ack_seen("T:")) && !ack_seen("T0:"))  goto parse_end;  //the first response should be such as "T:25/50\n"
      if (ack_seen(heaterID[BED])) infoSettings.bed_en = ENABLED;
      if (ack_seen(heaterID[CHAMBER])) infoSettings.chamber_en = ENABLED;
      uint8_t i;
      for (i = NOZZLE0; i < MAX_HOTEND_COUNT; i++) {
        if(!ack_seen(heaterID[i])) break;
      }
      infoSettings.hotend_count = i ? i : 1;
      if (infoSettings.ext_count < infoSettings.hotend_count) infoSettings.ext_count = infoSettings.hotend_count;
      updateNextHeatCheckTime();
      infoHost.connected = true;
      storeCmd("M503\n");// Query detailed printer capabilities
      storeCmd("M92\n"); // Steps/mm of extruder is an important parameter for Smart filament runout
                         // Avoid can't getting this parameter due to disabled M503 in Marlin
      storeCmd("M115\n");
    }

    // Onboard sd Gcode command response

    if(requestCommandInfo.inWaitResponse)
    {
      if (ack_seen(requestCommandInfo.startMagic))
      {
        requestCommandInfo.inResponse = true;
        requestCommandInfo.inWaitResponse = false;
      }
      else if ((requestCommandInfo.error_num > 0 && ack_seen(requestCommandInfo.errorMagic[0]))
            || (requestCommandInfo.error_num > 1 && ack_seen(requestCommandInfo.errorMagic[1]))
            || (requestCommandInfo.error_num > 2 && ack_seen(requestCommandInfo.errorMagic[2])))
      { //parse onboard sd error
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        requestCommandInfo.inError = true;
        requestCommandInfo.inWaitResponse = false;

        strcpy(requestCommandInfo.cmd_rev_buf, dmaL2Cache);
        BUZZER_PLAY(sound_error);
        goto parse_end;
      }
    }
    if(requestCommandInfo.inResponse)
    {
      if(strlen(requestCommandInfo.cmd_rev_buf)+strlen(dmaL2Cache) < CMD_MAX_REV)
      {
        strcat(requestCommandInfo.cmd_rev_buf, dmaL2Cache);
        if (ack_seen(requestCommandInfo.stopMagic))
        {
          requestCommandInfo.done = true;
          requestCommandInfo.inResponse = false;
        }
      }
      else
      {
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        ackPopupInfo(errormagic);
      }
      infoHost.wait = false;
      goto parse_end;
    }
    // Onboard sd Gcode command response end

    if(ack_cmp("ok\n"))
    {
      infoHost.wait = false;
    }
    else
    {
      if(ack_seen("ok"))
      {
        infoHost.wait = false;
      }
      // parse temperature
      if((ack_seen("@") && ack_seen("T:")) || ack_seen("T0:"))
      {
        heatSetCurrentTemp(NOZZLE0, ack_value()+0.5f);
        if(!heatGetSendWaiting(NOZZLE0)) {
          heatSyncTargetTemp(NOZZLE0, ack_second_value()+0.5f);
        }
        for(uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
        {
          if(!heaterIsValid(i)) continue;
          if(ack_seen(heaterID[i]))
          {
            heatSetCurrentTemp(i, ack_value()+0.5f);
            if(!heatGetSendWaiting(i)) {
              heatSyncTargetTemp(i, ack_second_value()+0.5f);
            }
          }
        }
        avoid_terminal = !infoSettings.terminalACK;
        updateNextHeatCheckTime();
      }
      else if((ack_seen("X:") && ack_index == 2) || ack_seen("C: X:")) // Smoothieware axis position starts with "C: X:"
      {
        coordinateSetAxisActual(X_AXIS, ack_value());
        if (ack_seen("Y:"))
        {
          coordinateSetAxisActual(Y_AXIS, ack_value());
          if (ack_seen("Z:"))
          {
            coordinateSetAxisActual(Z_AXIS, ack_value());
          }
        }
        coordinateQuerySetWait(false);
      }
      else if(ack_seen("Count E:")) // Parse actual extruder position, response of "M114 E\n", required "M114_DETAIL" in Marlin
      {
        coordinateSetAxisActualSteps(E_AXIS, ack_value());
      }
      else if(infoMachineSettings.onboard_sd_support == ENABLED && ack_seen("File opened: "))
      {
        // File opened: 1A29A~1.GCO Size: 6974
        uint16_t start_index = ack_index;
        uint16_t end_index = ack_seen("Size: ") ? (ack_index - sizeof("Size: ")) : start_index;
        infoFile.source = BOARD_SD;
        strcpy(infoFile.title, getCurFileSource());
        strcat(infoFile.title,"/");
        uint16_t path_len = MIN(end_index - start_index, MAX_PATH_LEN - strlen(getCurFileSource()) - 1);
        strncat(infoFile.title, dmaL2Cache + start_index, path_len);
        infoFile.title[path_len + strlen(getCurFileSource()) + 1] = 0;

        infoPrinting.pause = false;
        infoHost.printing = true;
        infoPrinting.time = 0;
        infoPrinting.cur = 0;
        infoPrinting.size = ack_value();
      }
      else if(infoMachineSettings.onboard_sd_support == ENABLED && infoFile.source == BOARD_SD && ack_seen("Not SD printing"))
      {
        infoHost.printing = false;
        if (infoPrinting.printing)
          infoPrinting.pause = true;
      }
      else if(infoMachineSettings.onboard_sd_support == ENABLED && infoFile.source == BOARD_SD && ack_seen("SD printing byte"))
      {
        infoPrinting.pause = false;
        // Parsing printing data
        // Example: SD printing byte 123/12345
        infoPrinting.cur = ack_value();
  //      powerFailedCache(position);
      }
      else if(infoMachineSettings.onboard_sd_support == ENABLED && infoFile.source == BOARD_SD && ack_seen("Done printing file"))
      {
        infoPrinting.printing = false;
        infoPrinting.cur = infoPrinting.size;
      }

    //parse and store stepper steps/mm values
      else if(ack_seen("M92 X"))
      {
                          setParameter(P_STEPS_PER_MM, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_STEPS_PER_MM, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_STEPS_PER_MM, Z_STEPPER, ack_value());
        if(ack_seen("E")) setParameter(P_STEPS_PER_MM, E_STEPPER, ack_value());
      }
      else if(ack_seen("M92 T0 E")){
        setParameter(P_STEPS_PER_MM, E_STEPPER, ack_value());
      }
      else if(ack_seen("M92 T1 E")){
        setParameter(P_STEPS_PER_MM, E2_STEPPER, ack_value());
        setDualStepperStatus(E_STEPPER, true);
      }
    //parse and store Max Feed Rate values
     else if(ack_seen("M203 X")){
                          setParameter(P_MAX_FEED_RATE, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_MAX_FEED_RATE, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_MAX_FEED_RATE, Z_STEPPER, ack_value());
        if(ack_seen("E")) setParameter(P_MAX_FEED_RATE, E_STEPPER, ack_value());
      }
      else if(ack_seen("M203 T0 E")){
        setParameter(P_MAX_FEED_RATE, E_STEPPER, ack_value());
      }
      else if(ack_seen("M203 T1 E")){
        setParameter(P_MAX_FEED_RATE, E2_STEPPER, ack_value());
        setDualStepperStatus(E_STEPPER, true);
      }
    //parse and store Max Acceleration values
      else if(ack_seen("M201 X")){
                          setParameter(P_MAX_ACCELERATION, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_MAX_ACCELERATION, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_MAX_ACCELERATION, Z_STEPPER, ack_value());
        if(ack_seen("E")) setParameter(P_MAX_ACCELERATION, E_STEPPER, ack_value());

      }
      else if(ack_seen("M201 T0 E")){
        setParameter(P_MAX_ACCELERATION, E_STEPPER, ack_value());
      }
      else if(ack_seen("M201 T1 E")){
        setParameter(P_MAX_ACCELERATION, E2_STEPPER, ack_value());
        setDualStepperStatus(E_STEPPER, true);
      }
    //parse and store Acceleration values
      else if(ack_seen("M204 P")){
                          setParameter(P_ACCELERATION, 0, ack_value());
        if(ack_seen("R")) setParameter(P_ACCELERATION, 1, ack_value());
        if(ack_seen("T")) setParameter(P_ACCELERATION, 2, ack_value());
      }
    //parse and store FW retraction values
      else if(ack_seen("M207 S")){
                          setParameter(P_FWRETRACT, 0, ack_value());
        if(ack_seen("W")) setParameter(P_FWRETRACT, 1, ack_value());
        if(ack_seen("F")) setParameter(P_FWRETRACT, 2, ack_value());
        if(ack_seen("Z")) setParameter(P_FWRETRACT, 3, ack_value());
      }
    //parse and store FW recover values
      else if(ack_seen("M208 S")){
                          setParameter(P_FWRECOVER, 0, ack_value());
        if(ack_seen("W")) setParameter(P_FWRECOVER, 1, ack_value());
        if(ack_seen("F")) setParameter(P_FWRECOVER, 2, ack_value());
        if(ack_seen("R")) setParameter(P_FWRECOVER, 3, ack_value());
      }
    //parse and store auto FW retract state (M209 - Set Auto Retract)
      else if(ack_seen("M209 S")){
                          setParameter(P_AUTO_RETRACT, 0, ack_value());
      }
    //parse and store Offset 2nd Nozzle
      else if(ack_seen("M218 T1 X")){
                          setParameter(P_OFFSET_TOOL, 0, ack_value());
        if(ack_seen("Y")) setParameter(P_OFFSET_TOOL, 1, ack_value());
        if(ack_seen("Z")) setParameter(P_OFFSET_TOOL, 2, ack_value());
      }
    //parse and store Probe Offset values
      else if(ack_seen("M851 X")){
                          setParameter(P_PROBE_OFFSET, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_PROBE_OFFSET, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_PROBE_OFFSET, Z_STEPPER, ack_value());
      }
    //parse and store linear advance values
      else if(ack_seen("M900 K")){
                          setParameter(P_LIN_ADV, 0, ack_value());
      }
      else if(ack_seen("M900 T0 K")){
                          setParameter(P_LIN_ADV, 0, ack_value());
      }
      else if(ack_seen("M900 T1 K")){
                          setParameter(P_LIN_ADV, 1, ack_value());
      }
    //parse and store stepper driver current values
      else if(ack_seen("M906 X")){
                          setParameter(P_CURRENT, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_CURRENT, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_CURRENT, Z_STEPPER, ack_value());
      }
    //parse and store TMC Bump sensitivity values
      else if(ack_seen("M914 X")){
                          setParameter(P_BUMPSENSITIVITY, X_STEPPER, ack_value());
        if(ack_seen("Y")) setParameter(P_BUMPSENSITIVITY, Y_STEPPER, ack_value());
        if(ack_seen("Z")) setParameter(P_BUMPSENSITIVITY, Z_STEPPER, ack_value());
      }
      else if(ack_seen("M906 I1")){
        if(ack_seen("X")) setDualStepperStatus(X_STEPPER, true);
        if(ack_seen("Y")) setDualStepperStatus(Y_STEPPER, true);
        if(ack_seen("Z")) setDualStepperStatus(Z_STEPPER, true);
      }
      else if(ack_seen("M906 T0 E")){
        setParameter(P_CURRENT, E_STEPPER, ack_value());
      }
      else if(ack_seen("M906 T1 E")){
        setParameter(P_CURRENT, E2_STEPPER, ack_value());
        setDualStepperStatus(E_STEPPER, true);
      }
    // Parse and store ABL type
      else if(ack_seen("echo:; Unified Bed Leveling")){
        if(ENABLE_UBL_VALUE==2) infoMachineSettings.enableubl = ENABLED;
      }
    // Parse and store ABL on/off state & Z fade value on M503
      else if(ack_seen("M420 S")) {
        infoSettings.autoLevelState = ack_value();
        if(ack_seen("S")) setParameter(P_ABL_STATE, 0, ack_value());
        if(ack_seen("Z")) setParameter(P_ABL_STATE, 1, ack_value());
      }
    // Parse M115 capability report
      else if(ack_seen("FIRMWARE_NAME:"))
      {
        uint8_t *string = (uint8_t *)&dmaL2Cache[ack_index];
        uint16_t string_start = ack_index;
        uint16_t string_end = string_start;
        if (ack_seen("Marlin"))
        {
          infoMachineSettings.isMarlinFirmware = 1;
        }
        else
        {
          infoMachineSettings.isMarlinFirmware = 0;
          setupMachine();
        }
        if (ack_seen("FIRMWARE_URL:")) // For Smoothieware and Repetier
          string_end = ack_index - sizeof("FIRMWARE_URL:");
        else if (ack_seen("SOURCE_CODE_URL:")) // For Marlin
          string_end = ack_index - sizeof("SOURCE_CODE_URL:");
        infoSetFirmwareName(string, string_end - string_start); // Set firmware name

        if (ack_seen("MACHINE_TYPE:"))
        {
          string = (uint8_t *)&dmaL2Cache[ack_index];
          string_start = ack_index;
          if (ack_seen("EXTRUDER_COUNT:"))
          {
            infoSettings.ext_count = ack_value();
            string_end = ack_index - sizeof("EXTRUDER_COUNT:");
          }
          infoSetMachineType(string, string_end - string_start); // Set firmware name
        }
      }
      else if(ack_seen("Cap:EEPROM:"))
      {
        infoMachineSettings.EEPROM = ack_value();
      }
      else if(ack_seen("Cap:AUTOREPORT_TEMP:"))
      {
        infoMachineSettings.autoReportTemp = ack_value();
      }
      else if(ack_seen("Cap:AUTOLEVEL:"))
      {
        infoMachineSettings.autoLevel = ack_value();
      }
      else if(ack_seen("Cap:Z_PROBE:"))
      {
        infoMachineSettings.zProbe = ack_value();
      }
      else if(ack_seen("Cap:LEVELING_DATA:"))
      {
        infoMachineSettings.levelingData = ack_value();
      }
      else if(ack_seen("Cap:SOFTWARE_POWER:"))
      {
        infoMachineSettings.softwarePower = ack_value();
      }
      else if(ack_seen("Cap:TOGGLE_LIGHTS:"))
      {
        infoMachineSettings.toggleLights = ack_value();
      }
      else if(ack_seen("Cap:CASE_LIGHT_BRIGHTNESS:"))
      {
        infoMachineSettings.caseLightsBrightness = ack_value();
      }
      else if(ack_seen("Cap:PROMPT_SUPPORT:"))
      {
        infoMachineSettings.promptSupport = ack_value();
      }
      else if(ack_seen("Cap:SDCARD:") && infoSettings.onboardSD == AUTO)
      {
        infoMachineSettings.onboard_sd_support = ack_value();
      }
      else if(ack_seen("Cap:AUTOREPORT_SD_STATUS:"))
      {
        infoMachineSettings.autoReportSDStatus = ack_value();
      }
      else if(ack_seen("Cap:LONG_FILENAME:") && infoSettings.longFileName == AUTO)
      {
        infoMachineSettings.long_filename_support = ack_value();
      }
      else if(ack_seen("Cap:CHAMBER_TEMPERATURE:"))
      {
        setupMachine();
      }
    //parse Repeatability Test
      else if(ack_seen("Mean:"))
      {
        popupReminder(DIALOG_TYPE_INFO, (u8* )"Repeatability Test", (u8 *)dmaL2Cache + ack_index-5);
      }
      else if(ack_seen("Probe Offset"))
      {
        if(ack_seen("Z:") || (ack_seen("Z")))
        {
          setParameter(P_PROBE_OFFSET,Z_STEPPER, ack_value());
        }
      }
    // parse and store feed rate percentage
      else if(ack_seen("FR:"))
      {
        speedSetPercent(0,ack_value());
        speedQuerySetWait(false);
      }
    // parse and store flow rate percentage
      else if(ack_seen("Flow: "))
      {
        speedSetPercent(1,ack_value());
        speedQuerySetWait(false);
      }
    // parse fan speed
      else if(ack_seen("M106 P"))
      {
        u8 i = ack_value();
        if (ack_seen("S"))
          fanSetSpeed(i, ack_value());
      }
    // Parse pause message
      else if(!infoMachineSettings.promptSupport && ack_seen("paused for user"))
      {
        showDialog(DIALOG_TYPE_QUESTION, (u8*)"Printer is Paused",(u8*)"Paused for user\ncontinue?",
                   textSelect(LABEL_CONFIRM), NULL, breakAndContinue, NULL,NULL);
      }
    // Parse UBL Complete message
      else if(ack_seen("UBL Complete"))
      {
        BUZZER_PLAY(sound_notify);

        showDialog(DIALOG_TYPE_INFO, textSelect(LABEL_ABL_COMPLETE), textSelect(LABEL_ABL_SMART_FILL),
                   textSelect(LABEL_CONFIRM), NULL, NULL, NULL, NULL);
      }
    // Parse PID Autotune finished message
      else if(ack_seen("PID Autotune finished"))
      {
        pidUpdateStatus(true);
      }
    // Parse PID Autotune failed message
      else if(ack_seen("PID Autotune failed"))
      {
        pidUpdateStatus(false);
      }
    // Parse "HOST_ACTION_COMMANDS"
      else if(ack_seen("//action:"))
      {
        hostActionCommands();
      }
    //Parse error messages & Echo messages
      else if(ack_seen(errormagic))
      {
        ackPopupInfo(errormagic);
      }
    // if no known echo was found and processed, then popup the echo message
      else if(ack_seen(echomagic))
      {
        if (!processKnownEcho())
        {
          ackPopupInfo(echomagic);
        }
      }
    }

  parse_end:
    if(ack_cur_src != SERIAL_PORT)
    {
      Serial_Puts(ack_cur_src, dmaL2Cache);
    }
    else if (!ack_seen("ok") || ack_seen("T:") || ack_seen("T0:"))
    {
      // make sure we pass on spontaneous messages to all connected ports (since these can come unrequested)
      for (int port = 0; port < _UART_CNT; port++)
      {
        if (port != SERIAL_PORT && portSeen[port])
        {
          // pass on this one to anyone else who might be listening
          Serial_Puts(port, dmaL2Cache);
        }
      }
    }

    if (avoid_terminal != true){
      sendGcodeTerminalCache(dmaL2Cache, TERMINAL_ACK);
    }
  }
}

void parseRcvGcode(void)
{
  #ifdef SERIAL_PORT_2
    uint8_t i = 0;
    for(i = 0; i < _UART_CNT; i++)
    {
      if(i != SERIAL_PORT && infoHost.rx_ok[i] == true)
      {
        infoHost.rx_ok[i] = false;
        while(dmaL1NotEmpty(i))
        {
          syncL2CacheFromL1(i);
          storeCmdFromUART(i, dmaL2Cache);
        }
      }
    }
  #endif
}
