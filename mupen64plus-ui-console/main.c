/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-ui-console - main.c                                       *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2007-2010 Richard42                                     *
 *   Copyright (C) 2008 Ebenblues Nmn Okaygo Tillin9                       *
 *   Copyright (C) 2002 Hacktarux                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* This is the main application entry point for the console-only front-end
 * for Mupen64Plus v2.0. 
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

// The mac version of SDL requires inclusion of SDL_main in the executable
#ifdef __APPLE__
#include <SDL/SDL_main.h>
#endif

#include "cheat.h"
#include "main.h"
#include "plugin.h"
#include "version.h"
#include "core_interface.h"
#include "compare_core.h"
#include "osal_preproc.h"
#include "ui-sdl.h"
#include "SDL.h"
#include <screen/screen.h>

#include "../bbutil/bbutil.h"

/** global variables **/
int    g_Verbose = 1;

/** static (local) variables **/
static m64p_handle l_ConfigCore = NULL;
static m64p_handle l_ConfigVideo = NULL;
static m64p_handle l_ConfigUI = NULL;

static const char *l_CoreLibPath = NULL;
static const char *l_ConfigDirPath = NULL;
static const char *l_ROMFilepath = NULL;       // filepath of ROM to load & run at startup

#if defined(SHAREDIR)
  static const char *l_DataDirPath = SHAREDIR;
#else
  static const char *l_DataDirPath = NULL;
#endif

static int  *l_TestShotList = NULL;      // list of screenshots to take for regression test support
static int   l_TestShotIdx = 0;          // index of next screenshot frame in list
static int   l_SaveOptions = 0;          // save command-line options in configuration file
static int   l_CoreCompareMode = 0;      // 0 = disable, 1 = send, 2 = receive

static eCheatMode l_CheatMode = CHEAT_DISABLE;
static char      *l_CheatNumList = NULL;

screen_context_t screen_cxt;

/*********************************************************************************************************
 *  Callback functions from the core
 */

void DebugCallback(void *Context, int level, const char *message)
{
    if (level <= 1)
        printf("%s Error: %s\n", (const char *) Context, message);
    else if (level == 2)
        printf("%s Warning: %s\n", (const char *) Context, message);
    else if (level == 3 || (level == 5 && g_Verbose))
        printf("%s: %s\n", (const char *) Context, message);
    else if (level == 4)
        printf("%s Status: %s\n", (const char *) Context, message);
    /* ignore the verbose info for now */
}

static void FrameCallback(unsigned int FrameIndex)
{
    // take a screenshot if we need to
    if (l_TestShotList != NULL)
    {
        int nextshot = l_TestShotList[l_TestShotIdx];
        if (nextshot == FrameIndex)
        {
            (*CoreDoCommand)(M64CMD_TAKE_NEXT_SCREENSHOT, 0, NULL);  /* tell the core take a screenshot */
            // advance list index to next screenshot frame number.  If it's 0, then quit
            l_TestShotIdx++;
        }
        else if (nextshot == 0)
        {
            (*CoreDoCommand)(M64CMD_STOP, 0, NULL);  /* tell the core to shut down ASAP */
            free(l_TestShotList);
            l_TestShotList = NULL;
        }
    }
}
/*********************************************************************************************************
 *  Configuration handling
 */

static m64p_error OpenConfigurationHandles(void)
{
    m64p_error rval;

    /* Open Configuration sections for core library and console User Interface */
    rval = (*ConfigOpenSection)("Core", &l_ConfigCore);
    if (rval != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: failed to open 'Core' configuration section\n");
        return rval;
    }

    rval = (*ConfigOpenSection)("Video-General", &l_ConfigVideo);
    if (rval != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: failed to open 'Video-General' configuration section\n");
        return rval;
    }

    rval = (*ConfigOpenSection)("UI-Console", &l_ConfigUI);
    if (rval != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: failed to open 'UI-Console' configuration section\n");
        return rval;
    }

    /* Set default values for my Config parameters */
    (*ConfigSetDefaultString)(l_ConfigUI, "PluginDir", OSAL_CURRENT_DIR, "Directory in which to search for plugins");
    (*ConfigSetDefaultString)(l_ConfigUI, "VideoPlugin", "mupen64plus-video-rice" OSAL_DLL_EXTENSION, "Filename of video plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "AudioPlugin", "mupen64plus-audio-sdl" OSAL_DLL_EXTENSION, "Filename of audio plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "InputPlugin", "mupen64plus-input-sdl" OSAL_DLL_EXTENSION, "Filename of input plugin");
    (*ConfigSetDefaultString)(l_ConfigUI, "RspPlugin", "mupen64plus-rsp-hle" OSAL_DLL_EXTENSION, "Filename of RSP plugin");

    return M64ERR_SUCCESS;
}

static m64p_error SaveConfigurationOptions(void)
{
    /* if shared data directory was given on the command line, write it into the config file */
    if (l_DataDirPath != NULL)
        (*ConfigSetParameter)(l_ConfigCore, "SharedDataPath", M64TYPE_STRING, l_DataDirPath);

    /* if any plugin filepaths were given on the command line, write them into the config file */
    if (g_PluginDir != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "PluginDir", M64TYPE_STRING, g_PluginDir);
    if (g_GfxPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "VideoPlugin", M64TYPE_STRING, g_GfxPlugin);
    if (g_AudioPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "AudioPlugin", M64TYPE_STRING, g_AudioPlugin);
    if (g_InputPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "InputPlugin", M64TYPE_STRING, g_InputPlugin);
    if (g_RspPlugin != NULL)
        (*ConfigSetParameter)(l_ConfigUI, "RspPlugin", M64TYPE_STRING, g_RspPlugin);

    return (*ConfigSaveFile)();
}

/*********************************************************************************************************
 *  Command-line parsing
 */

static void printUsage(const char *progname)
{
    printf("Usage: %s [parameters] [romfile]\n"
           "\n"
           "Parameters:\n"
           "    --noosd               : disable onscreen display\n"
           "    --osd                 : enable onscreen display\n"
           "    --fullscreen          : use fullscreen display mode\n"
           "    --windowed            : use windowed display mode\n"
           "    --resolution (res)    : display resolution (640x480, 800x600, 1024x768, etc)\n"
           "    --cheats (cheat-spec) : enable or list cheat codes for the given rom file\n"
           "    --corelib (filepath)  : use core library (filepath) (can be only filename or full path)\n"
           "    --configdir (dir)     : force configation directory to (dir); should contain mupen64plus.conf\n"
           "    --datadir (dir)       : search for shared data files (.ini files, languages, etc) in (dir)\n"
           "    --plugindir (dir)     : search for plugins in (dir)\n"
           "    --sshotdir (dir)      : set screenshot directory to (dir)\n"
           "    --gfx (plugin-spec)   : use gfx plugin given by (plugin-spec)\n"
           "    --audio (plugin-spec) : use audio plugin given by (plugin-spec)\n"
           "    --input (plugin-spec) : use input plugin given by (plugin-spec)\n"
           "    --rsp (plugin-spec)   : use rsp plugin given by (plugin-spec)\n"
           "    --emumode (mode)      : set emu mode to: 0=Pure Interpreter 1=Interpreter 2=DynaRec\n"
           "    --testshots (list)    : take screenshots at frames given in comma-separated (list), then quit\n"
           "    --set (param-spec)    : set a configuration variable, format: ParamSection[ParamName]=Value\n"
           "    --core-compare-send   : use the Core Comparison debugging feature, in data sending mode\n"
           "    --core-compare-recv   : use the Core Comparison debugging feature, in data receiving mode\n"
           "    --saveoptions         : save the given command-line options in configuration file for future\n"
           "    --verbose             : print lots of information\n"
           "    --help                : see this help message\n\n"
           "(plugin-spec):\n"
           "    (pluginname)          : filename (without path) of plugin to find in plugin directory\n"
           "    (pluginpath)          : full path and filename of plugin\n"
           "    'dummy'               : use dummy plugin\n\n"
           "(cheat-spec):\n"
           "    'list'                : show all of the available cheat codes\n"
           "    'all'                 : enable all of the available cheat codes\n"
           "    (codelist)            : a comma-separated list of cheat code numbers to enable,\n"
           "                            with dashes to use code variables (ex 1-2 to use cheat 1 option 2)\n"
           "\n", progname);

    return;
}

static int SetConfigParameter(const char *ParamSpec)
{
    char *ParsedString, *VarName, *VarValue;
    m64p_handle ConfigSection;
    m64p_type VarType;
    m64p_error rval;

    if (ParamSpec == NULL)
    {
        fprintf(stderr, "UI-Console Error: ParamSpec is NULL in SetConfigParameter()\n");
        return 1;
    }

    /* make a copy of the input string */
    ParsedString = (char *) malloc(strlen(ParamSpec) + 1);
    if (ParsedString == NULL)
    {
        fprintf(stderr, "UI-Console Error: SetConfigParameter() couldn't allocate memory for temporary string.\n");
        return 2;
    }
    strcpy(ParsedString, ParamSpec);

    /* parse it for the simple section[name]=value format */
    VarName = strchr(ParsedString, '[');
    if (VarName != NULL)
    {
        *VarName++ = 0;
        VarValue = strchr(VarName, ']');
        if (VarValue != NULL)
        {
            *VarValue++ = 0;
        }
    }
    if (VarName == NULL || VarValue == NULL || *VarValue != '=')
    {
        fprintf(stderr, "UI-Console Error: invalid (param-spec) '%s'\n", ParamSpec);
        free(ParsedString);
        return 3;
    }
    VarValue++;

    /* then set the value */
    rval = (*ConfigOpenSection)(ParsedString, &ConfigSection);
    if (rval != M64ERR_SUCCESS)
    {
        fprintf(stderr, "UI-Console Error: SetConfigParameter failed to open config section '%s'\n", ParsedString);
        free(ParsedString);
        return 4;
    }
    if ((*ConfigGetParameterType)(ConfigSection, VarName, &VarType) == M64ERR_SUCCESS)
    {
        switch(VarType)
        {
            int ValueInt;
            float ValueFloat;
            case M64TYPE_INT:
                ValueInt = atoi(VarValue);
                ConfigSetParameter(ConfigSection, VarName, M64TYPE_INT, &ValueInt);
                break;
            case M64TYPE_FLOAT:
                ValueFloat = (float) atof(VarValue);
                ConfigSetParameter(ConfigSection, VarName, M64TYPE_FLOAT, &ValueFloat);
                break;
            case M64TYPE_BOOL:
                ValueInt = (int) (osal_insensitive_strcmp(VarValue, "true") == 0);
                ConfigSetParameter(ConfigSection, VarName, M64TYPE_BOOL, &ValueInt);
                break;
            case M64TYPE_STRING:
                ConfigSetParameter(ConfigSection, VarName, M64TYPE_STRING, VarValue);
                break;
            default:
                fprintf(stderr, "UI-Console Error: invalid VarType in SetConfigParameter()\n");
                return 5;
        }
    }
    else
    {
        ConfigSetParameter(ConfigSection, VarName, M64TYPE_STRING, VarValue);
    }

    free(ParsedString);
    return 0;
}

static int *ParseNumberList(const char *InputString, int *ValuesFound)
{
    const char *str;
    int *OutputList;

    /* count the number of integers in the list */
    int values = 1;
    str = InputString;
    while ((str = strchr(str, ',')) != NULL)
    {
        str++;
        values++;
    }

    /* create a list and populate it with the frame counter values at which to take screenshots */
    if ((OutputList = (int *) malloc(sizeof(int) * (values + 1))) != NULL)
    {
        int idx = 0;
        str = InputString;
        while (str != NULL)
        {
            OutputList[idx++] = atoi(str);
            str = strchr(str, ',');
            if (str != NULL) str++;
        }
        OutputList[idx] = 0;
    }

    if (ValuesFound != NULL)
        *ValuesFound = values;
    return OutputList;
}

static int ParseCommandLineInitial(int argc, const char **argv)
{
    int i;

    /* look through commandline options */
    for (i = 1; i < argc; i++)
    {
        int ArgsLeft = argc - i - 1;

        if (strcmp(argv[i], "--corelib") == 0 && ArgsLeft >= 1)
        {
            l_CoreLibPath = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "--configdir") == 0 && ArgsLeft >= 1)
        {
            l_ConfigDirPath = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "--datadir") == 0 && ArgsLeft >= 1)
        {
            l_DataDirPath = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            printUsage(argv[0]);
            return 1;
        }
    }

    return 0;
}

static m64p_error ParseCommandLineFinal(int argc, const char **argv)
{
    int i;

    /* parse commandline options */
    for (i = 1; i < argc; i++)
    {
        int ArgsLeft = argc - i - 1;
        if (strcmp(argv[i], "--noosd") == 0)
        {
            int Osd = 0;
            (*ConfigSetParameter)(l_ConfigCore, "OnScreenDisplay", M64TYPE_BOOL, &Osd);
        }
        else if (strcmp(argv[i], "--osd") == 0)
        {
            int Osd = 1;
            (*ConfigSetParameter)(l_ConfigCore, "OnScreenDisplay", M64TYPE_BOOL, &Osd);
        }
        else if (strcmp(argv[i], "--fullscreen") == 0)
        {
            int Fullscreen = 1;
            (*ConfigSetParameter)(l_ConfigVideo, "Fullscreen", M64TYPE_BOOL, &Fullscreen);
        }
        else if (strcmp(argv[i], "--windowed") == 0)
        {
            int Fullscreen = 0;
            (*ConfigSetParameter)(l_ConfigVideo, "Fullscreen", M64TYPE_BOOL, &Fullscreen);
        }
        else if ((strcmp(argv[i], "--corelib") == 0 || strcmp(argv[i], "--configdir") == 0 ||
                  strcmp(argv[i], "--datadir") == 0) && ArgsLeft >= 1)
        {   /* these are handled in ParseCommandLineInitial */
            i++;
        }
        else if (strcmp(argv[i], "--resolution") == 0 && ArgsLeft >= 1)
        {
            const char *res = argv[i+1];
            int xres, yres;
            i++;
            if (sscanf(res, "%ix%i", &xres, &yres) != 2)
                fprintf(stderr, "Warning: couldn't parse resolution '%s'\n", res);
            else
            {
                (*ConfigSetParameter)(l_ConfigVideo, "ScreenWidth", M64TYPE_INT, &xres);
                (*ConfigSetParameter)(l_ConfigVideo, "ScreenHeight", M64TYPE_INT, &yres);
            }
        }
        else if (strcmp(argv[i], "--cheats") == 0 && ArgsLeft >= 1)
        {
            if (strcmp(argv[i+1], "all") == 0)
                l_CheatMode = CHEAT_ALL;
            else if (strcmp(argv[i+1], "list") == 0)
                l_CheatMode = CHEAT_SHOW_LIST;
            else
            {
                l_CheatMode = CHEAT_LIST;
                l_CheatNumList = (char*) argv[i+1];
            }
            i++;
        }
        else if (strcmp(argv[i], "--plugindir") == 0 && ArgsLeft >= 1)
        {
            g_PluginDir = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "--sshotdir") == 0 && ArgsLeft >= 1)
        {
            (*ConfigSetParameter)(l_ConfigCore, "ScreenshotPath", M64TYPE_STRING, argv[i+1]);
            i++;
        }
        else if (strcmp(argv[i], "--gfx") == 0 && ArgsLeft >= 1)
        {
            g_GfxPlugin = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "--audio") == 0 && ArgsLeft >= 1)
        {
            g_AudioPlugin = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "--input") == 0 && ArgsLeft >= 1)
        {
            g_InputPlugin = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "--rsp") == 0 && ArgsLeft >= 1)
        {
            g_RspPlugin = argv[i+1];
            i++;
        }
        else if (strcmp(argv[i], "--emumode") == 0 && ArgsLeft >= 1)
        {
            int emumode = atoi(argv[i+1]);
            i++;
            if (emumode < 0 || emumode > 2)
            {
                fprintf(stderr, "Warning: invalid --emumode value '%i'\n", emumode);
                continue;
            }
            if (emumode == 2 && !(g_CoreCapabilities & M64CAPS_DYNAREC))
            {
                fprintf(stderr, "Warning: Emulator core doesn't support Dynamic Recompiler.\n");
                emumode = 1;
            }
            (*ConfigSetParameter)(l_ConfigCore, "R4300Emulator", M64TYPE_INT, &emumode);
        }
        else if (strcmp(argv[i], "--testshots") == 0 && ArgsLeft >= 1)
        {
            l_TestShotList = ParseNumberList(argv[i+1], NULL);
            i++;
        }
        else if (strcmp(argv[i], "--set") == 0 && ArgsLeft >= 1)
        {
            if (SetConfigParameter(argv[i+1]) != 0)
                return M64ERR_INPUT_INVALID;
            i++;
        }
        else if (strcmp(argv[i], "--core-compare-send") == 0)
        {
            l_CoreCompareMode = 1;
        }
        else if (strcmp(argv[i], "--core-compare-recv") == 0)
        {
            l_CoreCompareMode = 2;
        }
        else if (strcmp(argv[i], "--saveoptions") == 0)
        {
            l_SaveOptions = 1;
        }
        else if (ArgsLeft == 0)
        {
            /* this is the last arg, it should be a ROM filename */
            l_ROMFilepath = argv[i];
            return M64ERR_SUCCESS;
        }
        else if (strcmp(argv[i], "--verbose") == 0)
        {
            g_Verbose = 1;
        }
        else
        {
            fprintf(stderr, "Warning: unrecognized command-line parameter '%s'\n", argv[i]);
        }
        /* continue argv loop */
    }

    /* missing ROM filepath */
    fprintf(stderr, "Error: no ROM filepath given\n");
    return M64ERR_INPUT_INVALID;
}

void zeldaSubscreenHack()
{
	static unsigned int crc1[] = {0xEC7011B7,0xD43DA81F,0x693BA2AE,0xB044B569,0xB2055FBD,0xEC7011B7,0xF034001A};
	static unsigned int crc2[] = {0x7616D72B,0x021E1E19,0xB7F14E9F,0x373C1985,0x0BAB4E0C,0x7616D72B,0xAE47ED06};

	m64p_rom_header header;

	int i;
    if ((*CoreDoCommand)(M64CMD_ROM_GET_HEADER, sizeof(header), &header) == M64ERR_SUCCESS)
    {
    	printf("Looking for zelda rom.\n");
    	for(i = 0;i < SDL_arraysize(crc1);i++)
    	{
    		if(SDL_Swap32(header.CRC1) == crc1[i] && SDL_Swap32(header.CRC2) == crc2[i])
    		{
    			printf("Found Zelda! using subscreen cheat!\n");
				l_CheatMode = CHEAT_LIST;
				l_CheatNumList = "0";
				return;
    		}
    	}
    }

}

/*********************************************************************************************************
* main function
*/
int main(int argc, char *argv[])
{
    int i;

    printf(" __  __                            __   _  _   ____  _             \n");
    printf("|  \\/  |_   _ _ __    ___ _ __  / /_ | || | |  _ \\| |_   _ ___ \n");
    printf("| |\\/| | | | | '_ \\ / _ \\ '_ \\| '_ \\| || |_| |_) | | | | / __|  \n");
    printf("| |  | | |_| | |_) |  __/ | | | (_) |__   _|  __/| | |_| \\__ \\  \n");
    printf("|_|  |_|\\__,_| .__/ \\___|_| |_|\\___/  |_| |_|   |_|\\__,_|___/  \n");
    printf("              |_|         http://code.google.com/p/mupen64plus/  \n");
    printf("%s Version %i.%i.%i\n\n", CONSOLE_UI_NAME, VERSION_PRINTF_SPLIT(CONSOLE_UI_VERSION));

    if(access("shared/misc/n64/", F_OK) != 0){
    	mkdir("shared/misc/n64/", S_IRWXU | S_IRWXG);
    	mkdir("shared/misc/n64/data", S_IRWXU | S_IRWXG);
    	mkdir("shared/misc/n64/roms", S_IRWXU | S_IRWXG);
    }

#ifdef TOUCHPAD_UI
    char* arguments[20];
    int argi = 0;

load_new_rom:
    argi = 0;
    arguments[argi++] = argv[0];
    arguments[argi++] = "--saveoptions";
    arguments[argi++] = "--emumode";
    arguments[argi++] = "2";
    arguments[argi++] = "--configdir";
    arguments[argi++] = "shared/misc/n64/data/";
    arguments[argi++] = "--datadir";
    //arguments[argi++] = "shared/misc/n64/data/";
    arguments[argi++] = "app/native/";
    arguments[argi++] = "--plugindir";
	arguments[argi++] = "app/native/lib";
	argc = argi;
    argv = arguments;
#endif
    /* bootstrap some special parameters from the command line */
    if (ParseCommandLineInitial(argc, (const char **) argv) != 0)
        return 1;
    l_CoreLibPath = "app/native/lib/libmupen64plus.so.2";
    /* load the Mupen64Plus core library */
    if (AttachCoreLib(l_CoreLibPath) != M64ERR_SUCCESS)
        return 2;

    /* start the Mupen64Plus core library, load the configuration file */
    m64p_error rval = (*CoreStartup)(CONSOLE_API_VERSION, l_ConfigDirPath, l_DataDirPath, "Core", DebugCallback, NULL, NULL);
    if (rval != M64ERR_SUCCESS)
    {
        printf("UI-console: error starting Mupen64Plus core library.\n");
        DetachCoreLib();
        return 3;
    }

    /* Open configuration sections */
    rval = OpenConfigurationHandles();
    if (rval != M64ERR_SUCCESS)
    {
        (*CoreShutdown)();
        DetachCoreLib();
        return 4;
    }

#ifdef TOUCHPAD_UI
    if(romName[0] == NULL){
    const char* currentVideo = ConfigGetParamString(l_ConfigUI,"VideoPlugin");
    videoPlugin = strstr(currentVideo,"rice") == NULL ? VIDEO_PLUGIN_GLES2N64 : VIDEO_PLUGIN_RICE;

    int rc = -1;

    	printf("One time init!\n");fflush(stdout);
		screen_create_context(&screen_cxt, 0);
		bbutil_init_egl(screen_cxt);

		if( BPS_SUCCESS != bps_initialize() ){
			printf("Error Initializing BPS...\n");
		}

		if( BPS_SUCCESS != dialog_request_events(0) ){
			printf("Error Initializing Dialog events...\n");
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		PB_eglSwapBuffers();

		//char* romName = UIChooseRom(&disableSound,&videoPlugin);

    	rc = dialog_select_game(romName, "shared/misc/n64/roms/", &videoPlugin, &disableSound);

		if(rc < 0){
			printf("Dialog Canceled or Wrong...\n");
			(*CoreShutdown)();
			DetachCoreLib();
			return 5;
		}
    } else {
    	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT);
    	PB_eglSwapBuffers();
    }

    if(!disableSound)
    {
    	arguments[argi++] = "--rsp";
    	arguments[argi++] = "libmupen64plus-rsp-hle";
    	arguments[argi++] = "--audio";
    	arguments[argi++] = "mupen64plus-audio-sdl";
    }
    else
    {
    	arguments[argi++] = "--audio";
    	arguments[argi++] = "dummy";
    	arguments[argi++] = "--rsp";
    	//arguments[argi++] = "rsp-no-sound";
    	arguments[argi++] = "libmupen64plus-rsp-hle";
    }

    if(videoPlugin == VIDEO_PLUGIN_RICE)
    {
    	printf("Using Rice...\n");fflush(stdout);
    	arguments[argi++] = "--gfx";
    	arguments[argi++] = "libmupen64plus-video-rice";
    	//arguments[argi++] = "dummy";
    }
    else
    {
    	printf("Using gles2n64...\n");fflush(stdout);
    	arguments[argi++] = "--gfx";
    	arguments[argi++] = "gles2n64";
    }

    arguments[argi++] = "--input";
    arguments[argi++] = "libmupen64plus-input-sdl";
    //arguments[argi++] = "dummy";

	arguments[argi++] = romName;
	argc = argi;
    argv = arguments;

    if(argv[argc-1] == NULL) //If no rom was chosen
    	argc--;
#endif

    /* parse command-line options */
    rval = ParseCommandLineFinal(argc, (const char **) argv);
    if (rval != M64ERR_SUCCESS)
    {
        (*CoreShutdown)();
        DetachCoreLib();
        return 5;
    }

    /* Handle the core comparison feature */
    if (l_CoreCompareMode != 0 && !(g_CoreCapabilities & M64CAPS_CORE_COMPARE))
    {
        printf("UI-console: can't use --core-compare feature with this Mupen64Plus core library.\n");
        DetachCoreLib();
        return 6;
    }
    compare_core_init(l_CoreCompareMode);

    /* save the given command-line options in configuration file if requested */
    if (l_SaveOptions)
        SaveConfigurationOptions();

    /* load ROM image */
    FILE *fPtr = fopen(l_ROMFilepath, "rb");
    if (fPtr == NULL)
    {
        fprintf(stderr, "Error: couldn't open ROM file '%s' for reading.\n", l_ROMFilepath);
        (*CoreShutdown)();
        DetachCoreLib();
        return 7;
    }

    /* get the length of the ROM, allocate memory buffer, load it from disk */
    long romlength = 0;
    fseek(fPtr, 0L, SEEK_END);
    romlength = ftell(fPtr);
    fseek(fPtr, 0L, SEEK_SET);
    unsigned char *ROM_buffer = (unsigned char *) malloc(romlength);
    if (ROM_buffer == NULL)
    {
        fprintf(stderr, "Error: couldn't allocate %li-byte buffer for ROM image file '%s'.\n", romlength, l_ROMFilepath);
        fclose(fPtr);
        (*CoreShutdown)();
        DetachCoreLib();
        return 8;
    }
    else if (fread(ROM_buffer, 1, romlength, fPtr) != romlength)
    {
        fprintf(stderr, "Error: couldn't read %li bytes from ROM image file '%s'.\n", romlength, l_ROMFilepath);
        free(ROM_buffer);
        fclose(fPtr);
        (*CoreShutdown)();
        DetachCoreLib();
        return 9;
    }
    fclose(fPtr);

    /* Try to load the ROM image into the core */
    if ((*CoreDoCommand)(M64CMD_ROM_OPEN, (int) romlength, ROM_buffer) != M64ERR_SUCCESS)
    {
        fprintf(stderr, "Error: core failed to open ROM image file '%s'.\n", l_ROMFilepath);
        free(ROM_buffer);
        (*CoreShutdown)();
        DetachCoreLib();
        return 10;
    }
    free(ROM_buffer); /* the core copies the ROM image, so we can release this buffer immediately */



    /* handle the cheat codes */
    zeldaSubscreenHack();
    CheatStart(l_CheatMode, l_CheatNumList);
    if (l_CheatMode == CHEAT_SHOW_LIST)
    {
        (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
        (*CoreShutdown)();
        DetachCoreLib();
        return 11;
    }

    /* search for and load plugins */
    rval = PluginSearchLoad(l_ConfigUI);
    if (rval != M64ERR_SUCCESS)
    {
        (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
        (*CoreShutdown)();
        DetachCoreLib();
        return 12;
    }

    /* attach plugins to core */
    for (i = 0; i < 4; i++)
    {
        if ((*CoreAttachPlugin)(g_PluginMap[i].type, g_PluginMap[i].handle) != M64ERR_SUCCESS)
        {
            fprintf(stderr, "UI-Console: error from core while attaching %s plugin.\n", g_PluginMap[i].name);
            (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);
            (*CoreShutdown)();
            DetachCoreLib();
            return 13;
        }
    }

    /* set up Frame Callback if --testshots is enabled */
    if (l_TestShotList != NULL)
    {
        if ((*CoreDoCommand)(M64CMD_SET_FRAME_CALLBACK, 0, FrameCallback) != M64ERR_SUCCESS)
        {
            fprintf(stderr, "UI-Console: warning: couldn't set frame callback, so --testshots won't work.\n");
        }
    }

    /* run the game */
    (*CoreDoCommand)(M64CMD_EXECUTE, 0, NULL);

    /* detach plugins from core and unload them */
    for (i = 0; i < 4; i++)
        (*CoreDetachPlugin)(g_PluginMap[i].type);
    PluginUnload();

    /* close the ROM image */
    (*CoreDoCommand)(M64CMD_ROM_CLOSE, 0, NULL);

    /* save the configuration file again if --saveoptions was specified, to keep any updated parameters from the core/plugins */
    if (l_SaveOptions)
        SaveConfigurationOptions();

    /* Shut down and release the Core library */
    (*CoreShutdown)();
    DetachCoreLib();

    /* free allocated memory */
    if (l_TestShotList != NULL)
        free(l_TestShotList);

    if(romName[0] != NULL){
		goto load_new_rom;
	}

    return 0;
}

