/* See LICENSE file for copyright and license details. */
#define BARPADDING_PATCH 1

/* appearance */
static const unsigned int borderpx          = 1;        /* border pixel of windows */
static const Gap default_gap                = {.isgap = 1, .realgap = 10, .gappx = 10};
static const unsigned int snap              = 32;       /* snap pixel */
static const int scalepreview               = 8;        /* preview scaling (display w and h / scalepreview) */
static const int previewbar                 = 0;        /* show the bar in the preview window */
static const int swallowfloating            = 0;        /* 1 means swallow floating windows by default */
static const int swterminheritfs            = 1;        /* 1 terminal inherits fullscreen on unswallow, 0 otherwise */
static const int showbar                    = 1;        /* 0 means no bar */
static const int topbar                     = 1;        /* 0 means bottom bar */
static const unsigned int systraypinning    = 3;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing    = 2;   /* systray spacing */
static const int systraypinningfailfirst    = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray                = 1;   /* 0 means no systray */
static const int vertpad                    = 10;       /* vertical padding of bar */
static const int sidepad                    = 10;       /* horizontal padding of bar */
static const int horizpadbar                = 8;        /* horizontal padding for statusbar */
static const int vertpadbar                 = 8;        /* vertical padding for statusbar */
static const int showtab                    = 1;        /* 0 means no tabbar */
static const int alltab                     = 0;        /* 0 means no tabbar for all layouts */
static const int toptab                     = 0;        /* 1 means top tab bar */
static const char *fonts[]                  = { "monofur:size=12", "monofur nerd font:style:medium:size=10" };
static const char dmenufont[]               = "monofur:size=12";
static const char col_bg[]                  = "#1a1b26";
static const char col_fg[]                  = "#787c99";
static const char col_sel[]                 = "#449dab";
static const unsigned int baralpha          = 0xd0;
static const unsigned int borderalpha       = OPAQUE;

static const char *colors[][3]              = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_fg, col_bg, col_bg },
	[SchemeSel]  = { col_sel, col_bg,  col_bg  },
};

static const unsigned int alphas[][3]       = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging: refer to https://github.com/bakkeby/patches/wiki/tagicons */
static char *tagicons[][NUMTAGS] = {
	[IconsDefault]        = { "" },
	[IconsVacant]         = { "1", "2", "3", "4", "5", "6", "7", "8", "9" },
	[IconsOccupied]       = { "1", "2", "3", "4", "5", "6", "7", "8", "9" },
};

static const unsigned int ulinepad		= 5;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 2;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static const int ulineall				= 0;	/* 1 to show underline on all tags, 0 for just the active ones */

static const char *tagsel[][2] = {
	{ col_sel,     col_bg },
	{ "#ff7a93", col_bg },
	{ "#bb9af7", col_bg },
	{ "#b9f27c", col_bg },
	{ "#e0af68", col_bg },
	{ "#ff9e64", col_bg },
	{ "#7aa2f7", col_bg },
	{ "#0db9d7", col_bg },
	{ "#ad8ee6", col_bg },
};

static const unsigned int tagalpha[] = { OPAQUE, baralpha };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                instance    title            tags mask  isfloating  isterminal  noswallow  monitor */
    { "mpv",                NULL,       NULL,            0,         0,          0,           1,        -1 },
	{ "librewolf",          NULL,       NULL,            0,         0,          0,           -1,        -1 },
    { "st-256color",        NULL,       NULL,            0,         0,          1,           0,        -1 },
	{ NULL,                 NULL,       "Event Tester",  0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact        = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster        = 1;    /* number of clients in master area */
static const int resizehints    = 1;    /* 1 means respect size hints in tiled resizals */
static const int decorhints     = 1;    /* 1 means respect decoration hints */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate    = 60;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "󰙀 " ,      tile },    /* first entry is default */
	{ "󰀽 ",      NULL },    /* no layout function means floating behavior */
	{ "󰩨 ",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      previewtag,     {.ui = TAG } },     \

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/fish", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", "#000000", "-nf", col_fg, "-sb", "#000000", "-sf", col_sel, NULL };
static const char *termcmd[]  = { "st", NULL };

static const Key keys[] = {
	/* modifier                     key        function         argument */
    { MODKEY|ShiftMask,				XK_s,	   spawn, 	   		SHCMD("maim --select --hidecursor | xclip -selection clipboard -t image/png")},
	{ MODKEY,						XK_Print,  spawn,	   		SHCMD("maim | xclip -selection clipboard -t image/png")},
	{ MODKEY|ShiftMask,				XK_Print,  spawn,	   		SHCMD("maim -i $(xdotool getactivewindow) --hidecursor | xclip -selection clipboard -t image/png")},
	{ MODKEY|ShiftMask, 			XK_f,	   spawn,           SHCMD("pcmanfm ~ -n")},
	{ MODKEY,                       XK_s,      spawn,           {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,           {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,       {0} },
	{ MODKEY,                       XK_w,      toggletab,       {0} }, // tabmodes
	{ MODKEY,                       XK_j,      focusstack,      {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,      {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,      {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,      {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,        {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,        {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,            {0} },
	{ MODKEY,                       XK_Tab,    view,            {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,      {0} },
	{ MODKEY,                       XK_t,      setlayout,       {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,       {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,       {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,       {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating,  {0} },
	{ MODKEY,                       XK_0,      view,            {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,             {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,        {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,          {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,          {.i = +1 } },
//	{ MODKEY|ShiftMask,             XK_a,      seticonset,      {.i = 0 } },
//	{ MODKEY|ShiftMask,             XK_b,      seticonset,      {.i = 1 } },
    { MODKEY,                       XK_n,      nametag,         {0} },
	{ MODKEY,                       XK_minus,  setgaps,         {.i = -5 } },
	{ MODKEY,                       XK_equal,  setgaps,         {.i = +5 } },
	{ MODKEY|ShiftMask,             XK_minus,  setgaps,         {.i = GAP_RESET } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,         {.i = GAP_TOGGLE} },
	TAGKEYS(                        XK_1,                       0)
	TAGKEYS(                        XK_2,                       1)
	TAGKEYS(                        XK_3,                       2)
	TAGKEYS(                        XK_4,                       3)
	TAGKEYS(                        XK_5,                       4)
	TAGKEYS(                        XK_6,                       5)
	TAGKEYS(                        XK_7,                       6)
	TAGKEYS(                        XK_8,                       7)
	TAGKEYS(                        XK_9,                       8)
	{ MODKEY|ShiftMask,             XK_q,      quit,            {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTagBar,            0,              Button4,        cycleiconset,   {.i = +1 } },
	{ ClkTagBar,            0,              Button5,        cycleiconset,   {.i = -1 } },
	{ ClkTabBar,            0,              Button1,        focuswin,       {0} }, // tab mode
};

