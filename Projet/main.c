#define _WIN32_IE 0x0600
#define _WIN32_WINNT 0x0501
#define WINVER 0x501
#define _WIN32_WINDOWS 0x501
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <commctrl.h>
#include "ressources.h"

// Variables globales
int joker_used = 0;
static HICON hIcon = NULL;
static HBRUSH bgBrush = NULL;
static HBITMAP hBitmap = NULL;
int j;
int i = 0;
int k = 0;
int c[CODE_LENGTH];
int g[CODE_LENGTH];
int usedU[CODE_LENGTH] = {0};
int usedJ[CODE_LENGTH] = {0};
char *p;

// Structures
typedef enum {
    MODE_FACILE,
    MODE_DIFFICILE
} GameMode;

GameMode g_mode;

typedef enum {
    STATE_PLAYER1_SETUP,
    STATE_PLAYER2_GUESS
} GameState;

typedef struct {
	char nom[50];
	int score;
}PS;

GameState g_currentState = STATE_PLAYER1_SETUP;

// Fonction de réinitialisation du jeu
void ResetGame() {
    joker_used = 0;
    g_currentState = STATE_PLAYER1_SETUP;
    memset(c, 0, sizeof(c));
    memset(g, 0, sizeof(g));
    memset(usedU, 0, sizeof(usedU));
    memset(usedJ, 0, sizeof(usedJ));
}

// Fonctions usuelles
int good(int u[], int j[])
{
    int bien = 0;
    for (i = 0; i < CODE_LENGTH; i++)
    {
        if (u[i] == j[i])
        {
            bien++;
            usedU[i] = 1;
            usedJ[i] = 1;
        }
    }
    return bien;
}

int bad(int u[], int j[])
{
    int mal = 0;
    for (i = 0; i < CODE_LENGTH; i++)
    {
        if (usedU[i]==0) {
            for (k = 0; k < CODE_LENGTH; k++)
            {
                if (u[i] == j[k] && i != k && usedJ[k]==0)
                {
                    mal++;
                    usedJ[k] = 1;
                    break;
                }
            }
        }
    }
    return mal;
}

int jone(int u[], int num)
{
    if (num < 0 || num >= CODE_LENGTH) {
        return -1; // Valeur invalide
    }
    return u[num];
}

int jtwo(int u[])
{
    int nbr = 1;
    int found;
    while (nbr < 10) {
        found = 0;
        for (i = 0; i < CODE_LENGTH; i++)
        {
            if (nbr == u[i])
            {
                found = 1;
                break;
            }
        }
        if (found==0) {
            return nbr;
        }
        nbr++;
    }
    return 0; // Tous les nombres sont dans le code (cas improbable)
}

static int pt = MAX_ATTEMPTS;

//Fonctions random number generator

void Generate(int code[])
{
	int w;
	for (w=0; w<CODE_LENGTH; w++)
	{
		code[w]=rand()%10;
	}
}

void Generateunique(int code[])
{
	int us[10]={0};
	int q;
	for (q=0;q<CODE_LENGTH;q++)
	{
		int n;
		do
		{
			n= rand()%10;
		}
		while (us[n]!=0);
		code[q]=n;
		us[n]=1;
	}
}

//Procedures Vitales

void Update(HWND hDlg)
{
    char mis[64];
    snprintf(mis, sizeof(mis), "Tentatives restantes : %d", pt);
    SetDlgItemText(hDlg, IDC_TENTATIVES, mis);
}

void chg(HWND hDlg, HINSTANCE hInstance, HWND *phBtnJone, HWND *phBtnJtwo, HWND *phBtnC)
{
    g_currentState = STATE_PLAYER2_GUESS;
    joker_used = 0;
    pt = MAX_ATTEMPTS;

    ShowWindow(GetDlgItem(hDlg, IDC_TENTATIVES), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_RANDOM_BUTTON),SW_HIDE);
    SetDlgItemText(hDlg, IDC_NUMBER_INPUT, "");
    SetWindowText(hDlg, "Joueur 2 ");
    SetDlgItemText(hDlg, -1, "Essayer de deviner le code");

    char buffer2[50];
    snprintf(buffer2, sizeof(buffer2), "Tentatives restantes : %d", pt);
    SetDlgItemText(hDlg, IDC_TENTATIVES, buffer2);

    SendDlgItemMessage(hDlg, IDC_NUMBER_INPUT, EM_SETPASSWORDCHAR, 0, 0);
    ShowWindow(GetDlgItem(hDlg, IDC_SHOW_PWD), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDOK), SW_HIDE);
    SetDlgItemText(hDlg, IDCANCEL, "Abandonner");
    EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);

    if (*phBtnC == NULL) {
        *phBtnC = CreateWindowEx(0, "BUTTON", "Vérifier",
            WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
            180, 130, 60, 25, hDlg, (HMENU)IDC_C_BUTTON, hInstance, NULL);
    } else {
        ShowWindow(*phBtnC, SW_SHOW);
    }

    if (*phBtnJone == NULL) {
        *phBtnJone = CreateWindowEx(0, "BUTTON", "Joker 1",
            WS_CHILD | BS_PUSHBUTTON, 10, 95, 60, 25,
            hDlg, (HMENU)IDC_Jone_BUTTON, hInstance, NULL);
    }

    if (*phBtnJtwo == NULL) {
        *phBtnJtwo = CreateWindowEx(0, "BUTTON", "Joker 2",
            WS_CHILD | BS_PUSHBUTTON, 10, 130, 60, 25,
            hDlg, (HMENU)IDC_Jtwo_BUTTON, hInstance, NULL);
    }

    ShowWindow(*phBtnJone, SW_HIDE);
    EnableWindow(*phBtnJone, FALSE);
	ShowWindow(*phBtnJtwo, SW_HIDE);
    EnableWindow(*phBtnJtwo, FALSE);
}
//Fonctions score
int scorefs(PS player[], int max)
{
    FILE *f = fopen("scores.txt", "r");
    int hsb = 0;

    if (!f) {
        return 0;
    }

    char line[128];

    while (hsb < max && fgets(line, sizeof(line), f) != NULL)
    {

        line[strcspn(line, "\n")] = 0;


        char *dash = strstr(line, " - ");

        if (dash != NULL)
        {

            int nameLen = dash - line;
            if (nameLen > 0 && nameLen < 50) {
                strncpy(player[hsb].nom, line, nameLen);
                player[hsb].nom[nameLen] = '\0';
            }


            player[hsb].score = atoi(dash + 3);
            hsb++;
        }
    }

    fclose(f);
    return hsb;
}

int scorefs2(PS player[], int max)
{
    FILE *f = fopen("scores2.txt", "r");
    int hsb = 0;

    if (!f) {
        return 0;
    }

    char line[128];

    while (hsb < max && fgets(line, sizeof(line), f) != NULL)
    {
        line[strcspn(line, "\n")] = 0;

        char *dash = strstr(line, " - ");

        if (dash != NULL)
        {
            int nameLen = dash - line;
            if (nameLen > 0 && nameLen < 50) {
                strncpy(player[hsb].nom, line, nameLen);
                player[hsb].nom[nameLen] = '\0';
            }

            player[hsb].score = atoi(dash + 3);
            hsb++;
        }
    }

    fclose(f);
    return hsb;
}
int scorefm(const char *nom, int nscore)
{
    int e;
    PS player[100];
    int hsb = scorefs(player, 100);
    int hsslti = 0;
    int totalScore = 0;

    for (e = 0; e < hsb; e++)
    {
        if (strcmp(player[e].nom, nom) == 0)
        {
            player[e].score += nscore;
            totalScore = player[e].score;
            hsslti = 1;
            break;
        }
    }

    if (hsslti==0)
    {
        strcpy(player[hsb].nom, nom);
        player[hsb].score = nscore;
        totalScore = nscore;
        hsb++;
    }

    FILE *f = fopen("scores.txt", "w");
    if (!f) {
        return totalScore;
    }

    for (e = 0; e < hsb; e++)
    {
        fprintf(f, "%s - %d\n", player[e].nom, player[e].score);
    }

    fclose(f);

    return totalScore;
}
int scoremax(const char *nom)
{
    int v;
    PS player[100];
    int hsb = scorefs2(player, 100);
    int max = 0;
    int trv = 0;
    for (v = 0; v < hsb; v++)
    {
        if (strcmp(player[v].nom, nom) == 0)
        {
            trv = 1;

            if (player[v].score > max)
            {
                max = player[v].score;
            }
        }
    }

    if (trv == 0) {
        return 0;
    }

    return max;
}

INT_PTR CALLBACK NameDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
            SetFocus(GetDlgItem(hDlg, IDC_NAME_EDIT));
            return FALSE;

        case WM_CLOSE:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_NAME_OK || LOWORD(wParam) == IDOK)
            {
                char* playerName = (char*)GetWindowLongPtr(hDlg, GWLP_USERDATA);

                if (playerName != NULL)
                {
                    GetDlgItemText(hDlg, IDC_NAME_EDIT, playerName, 32);

                    if (strlen(playerName) > 0) {
                        EndDialog(hDlg, IDOK);
                        return TRUE;
                    } else {
                        MessageBox(hDlg, "Veuillez entrer un nom!", "Erreur", MB_OK | MB_ICONWARNING);
                        SetFocus(GetDlgItem(hDlg, IDC_NAME_EDIT));
                        return TRUE;
                    }
                }
                else
                {
                    MessageBox(hDlg, "Erreur interne!", "Erreur", MB_OK | MB_ICONERROR);
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }
            }
            else if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, IDCANCEL);
                return TRUE;
            }
            break;
    }
    return FALSE;
}
// Interface graphique joueur1
INT_PTR CALLBACK InputDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hBtnJone = NULL, hBtnJtwo = NULL, hBtnC = NULL, hBtnR=NULL;
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);

    switch (message) {
        case WM_INITDIALOG:
            pt = MAX_ATTEMPTS;
            ResetGame();
            SendDlgItemMessage(hDlg, IDC_NUMBER_INPUT, EM_SETLIMITTEXT, CODE_LENGTH, 0);
            ShowWindow(GetDlgItem(hDlg, IDC_TENTATIVES), SW_HIDE);
CreateWindowEx(
		    0,
		    "BUTTON",
		    "Code alératoire",
		    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		    180, 95, 120, 25,
		    hDlg,
		    (HMENU)IDC_RANDOM_BUTTON,
		    hInstance,
		    NULL
			);
            hBtnJone = NULL;
            hBtnJtwo = NULL;
            hBtnC = NULL;
            hBtnR = NULL;
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_SHOW_PWD: {
                    BOOL isChecked = IsDlgButtonChecked(hDlg, IDC_SHOW_PWD);

                    if (isChecked) {
                        SendDlgItemMessage(hDlg, IDC_NUMBER_INPUT, EM_SETPASSWORDCHAR, 0, 0);
                    } else {
                        SendDlgItemMessage(hDlg, IDC_NUMBER_INPUT, EM_SETPASSWORDCHAR, '*', 0);
                    }
                    SetFocus(GetDlgItem(hDlg, IDC_NUMBER_INPUT));
                    InvalidateRect(GetDlgItem(hDlg, IDC_NUMBER_INPUT), NULL, TRUE);
                    return TRUE;
                }

                // Option joker 1
                case IDC_Jone_BUTTON: {
                    if (pt > JOKER_AVAILABLE_THRESHOLD) {
                        MessageBox(hDlg, "Les jokers sont disponibles après la 10ème tentative", "Info", MB_OK);
                        return TRUE;
                    }

                    if (joker_used) {
                        MessageBox(hDlg, "Vous avez déjà utilisé un joker", "Info", MB_OK);
                        return TRUE;
                    }

                    char input[16] = {0};
                    GetDlgItemText(hDlg, IDC_NUMBER_INPUT, input, sizeof(input));

                    if (strlen(input) != 1 || input[0] < '0' || input[0] > '4') {
                        MessageBox(hDlg, "Entrez un chiffre entre 0 et 4", "Erreur", MB_OK);
                        return TRUE;
                    }

                    int index = input[0] - '0';
                    int number = jone(c, index);

                    if (number == -1) {
                        MessageBox(hDlg, "Index invalide", "Erreur", MB_OK);
                        return TRUE;
                    }


                    char msg[64];
                    snprintf(msg, sizeof(msg), "Le chiffre à l'index %d est %d", index, number);
                    MessageBox(hDlg, msg, "Joker 1", MB_OK);

                    pt -= JOKER1_COST;
                    if (pt < 0) pt = 0;
                    Update(hDlg);

                    joker_used = 1;

                    ShowWindow(hBtnJone, SW_HIDE);
                    ShowWindow(hBtnJtwo, SW_HIDE);

                    return TRUE;
                }

                // Option joker 2
                case IDC_Jtwo_BUTTON: {
                    if (pt > JOKER_AVAILABLE_THRESHOLD) {
                        MessageBox(hDlg, "Les jokers sont disponibles après la 10ème tentative", "Info", MB_OK);
                        return TRUE;
                    }

                    if (joker_used) {
                        MessageBox(hDlg, "Vous avez déjà utilisé un joker", "Info", MB_OK);
                        return TRUE;
                    }

                    int joker = jtwo(c);

                    char msg[64];
                    snprintf(msg, sizeof(msg), "Pas dans le code : %d", joker);
                    MessageBox(hDlg, msg, "Joker 2", MB_OK);

                    pt -= JOKER2_COST;
                    if (pt < 0) pt = 0;
                    Update(hDlg);

                    joker_used = 1;

                    ShowWindow(hBtnJone, SW_HIDE);
                    ShowWindow(hBtnJtwo, SW_HIDE);

                    return TRUE;
                }

                //Generateur de nombre
                case IDC_RANDOM_BUTTON:
                	{
                		if (g_mode==MODE_FACILE)
                		{
                			Generateunique(c);
						}
						else
						{
							Generate(c);
						}
						MessageBox(hDlg,"Code généré automatiquement\nAu tour du Joueur 2.","Information",MB_OK|MB_ICONINFORMATION);
						ShowWindow(GetDlgItem(hDlg, IDC_RANDOM_BUTTON),SW_HIDE);
						chg(hDlg, hInstance, &hBtnJone, &hBtnJtwo, &hBtnC);
                        return TRUE;
					}

                // Interface graphique joueur2
                case IDC_C_BUTTON: {
                    int l = 0;
                    int m = 0;
                    char tmp[16] = {0};
                    GetDlgItemText(hDlg, IDC_NUMBER_INPUT, tmp, sizeof(tmp));
                    p = tmp;

                    if (strlen(p) != CODE_LENGTH) {
                        MessageBox(hDlg, "Entrez exactement 5 chiffres.", "SVP", MB_OK);
                        return TRUE;
                    }
                     if (g_mode == MODE_FACILE) {
                        for (i = 0; i < CODE_LENGTH; i++) {
                            for (j = i + 1; j < CODE_LENGTH; j++) {
                                if (*(p+i) == *(p+j)) {
                                    MessageBox(hDlg, "Pas de doublons en mode facile", "Attention", MB_ICONERROR);
                                    return TRUE;                                }
                            }
                        }
                    }


                    // Validation des chiffres
                    for (i = 0; i < CODE_LENGTH; i++) {
                        if (p[i] < '0' || p[i] > '9') {
                            MessageBox(hDlg, "Entrée invalide. Utilisez uniquement des chiffres.", "Erreur", MB_OK);
                            return TRUE;
                        }
                    }

                    for (i = 0; i < CODE_LENGTH; i++) {
                        g[i] = *(p + i) - '0';
                    }

                    memset(usedU, 0, sizeof(usedU));
                    memset(usedJ, 0, sizeof(usedJ));
                    l = good(c, g);
                    m = bad(c, g);

                    if (l != CODE_LENGTH && pt != 0) {
                        pt--;
                        Update(hDlg);
                        char resultat[100];
                        snprintf(resultat, sizeof(resultat), "Bien placés : %d\nMal placés : %d", l, m);
                        MessageBox(hDlg, resultat, "Résultat de l'essai", MB_OK);
                    }

                     if (pt <= JOKER_AVAILABLE_THRESHOLD && joker_used == 0) {

        if (pt >= JOKER1_COST) {
            ShowWindow(hBtnJone, SW_SHOW);
            EnableWindow(hBtnJone, TRUE);
        } else {
            ShowWindow(hBtnJone, SW_HIDE);
            EnableWindow(hBtnJone, FALSE);
        }

        if (pt >= JOKER2_COST) {
            ShowWindow(hBtnJtwo, SW_SHOW);
            EnableWindow(hBtnJtwo, TRUE);
        } else {
            ShowWindow(hBtnJtwo, SW_HIDE);
            EnableWindow(hBtnJtwo, FALSE);
        }
		InvalidateRect(hDlg, NULL, TRUE);
    }


                   if (l == CODE_LENGTH)
{

                int score = pt * SCORE_MULTIPLIER;
                char playerName[32] = {0};

                int ret = DialogBoxParam(
                hInstance,
                MAKEINTRESOURCE(IDD_NAME_DIALOG),
                hDlg,
                NameDlgProc,
                (LPARAM)playerName
                );

                   if (ret == -1)
            {
               MessageBox(hDlg, "Erreur lors de l'ouverture de la boîte de dialogue!", "Erreur", MB_OK | MB_ICONERROR);
               EndDialog(hDlg, IDCANCEL);
                return TRUE;
            }
            char message[256];
            int cumul, meilleur;


            if (ret == IDOK && strlen(playerName) > 0)
			{
			   cumul=scorefm(playerName, score);
			   FILE *f = fopen("scores2.txt", "a");
	        if (f)
	   {
	        fprintf(f, "%s - %d\n", playerName, score);
	        fclose(f);
       }
			   meilleur=scoremax(playerName);
               snprintf(message, sizeof(message), "Félicitations %s!\nVous avez trouvé le code!\nScore: %d XP\nScore cumulée: %d XP\nMeilleur Score: %d XP", playerName, score,cumul, meilleur);
            }
	        else
	        {
               snprintf(message, sizeof(message), "Vous avez trouvé le code!\nScore: %d XP", score);
            }

            MessageBox(hDlg, message, "Victoire!", MB_OK | MB_ICONINFORMATION);




            EndDialog(hDlg, IDCANCEL);
            return TRUE;
}

                    if (pt == 0) {
                        char topsecret[32];
                        snprintf(topsecret, sizeof(topsecret), "%d%d%d%d%d", c[0], c[1], c[2], c[3], c[4]);
                        char mskin[128];
                        snprintf(mskin, sizeof(mskin), "Hélas, vous avez perdu!\nLe code était: %s", topsecret);
                        MessageBox(hDlg, mskin, "Game over", MB_OK | MB_ICONINFORMATION);
                        EndDialog(hDlg, IDCANCEL);
                        return TRUE;
                    }
                    return TRUE;
                }

                case IDOK: {
                    char buffer[16] = {0};
                    GetDlgItemText(hDlg, IDC_NUMBER_INPUT, buffer, sizeof(buffer));
                    p = buffer;

                    if (strlen(p) != CODE_LENGTH) {
                        MessageBox(hDlg, "Entrez exactement 5 chiffres.", "SVP", MB_ICONERROR);
                        return TRUE;
                    }

                    for (i = 0; i < CODE_LENGTH; i++) {
                        if (*(p + i) < '0' || *(p + i) > '9') {
                            MessageBox(hDlg, "Entrée invalide.", "Refaire", MB_ICONERROR);
                            return TRUE;
                        }
                    }

                    // Conversion en tableau entier
                    for (i = 0; i < CODE_LENGTH; i++) {
                        c[i] = *(p + i) - '0';
                    }

                    // Mode Facile - vérification des doublons
                    if (g_mode == MODE_FACILE) {
                        for (i = 0; i < CODE_LENGTH; i++) {
                            for (j = i + 1; j < CODE_LENGTH; j++) {
                                if (c[i] == c[j]) {
                                    MessageBox(hDlg, "Veuillez ne pas entrer deux nombres égaux dans le mode facile", "Condition obligatoire", MB_ICONERROR);
                                    return TRUE;
                                }
                            }
                        }
                    }

                    // Passage du joueur 1 au joueur 2
                    if (g_currentState != STATE_PLAYER2_GUESS) {
                        MessageBox(hDlg, "Le code saisi est bien enregistré ! Au tour du Joueur 2.", "Début du jeu", MB_OK);
                        chg(hDlg, hInstance, &hBtnJone, &hBtnJtwo, &hBtnC);
                        return TRUE;
                    }
                    return TRUE;
                }

                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
           }
            break;
    }
    return FALSE;
}
//Interface graphique initiale
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;
    static HWND hTooltip = NULL;

    switch(Message) {
        case WM_ERASEBKGND:
            return 1;

        case WM_CREATE:
        {
            // Initialisation
            InitCommonControls();

            // creation du tooltip
            hTooltip = CreateWindowEx(
                0,
                TOOLTIPS_CLASS,
                NULL,
                WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
                CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, CW_USEDEFAULT,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL
            );

            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)hTooltip);

            return 0;
        }

        case WM_PAINT: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            hdc = BeginPaint(hwnd, &ps);

            // Background
            FillRect(hdc, &rect, bgBrush);

            // Ajout de l'image
            if (hBitmap) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
                BITMAP bm;
                GetObject(hBitmap, sizeof(bm), &bm);

                int x = (rect.right - bm.bmWidth) / 2;
                int y = 10;

                BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, hOldBitmap);
                DeleteDC(hdcMem);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDBUTTON: // Mode difficile
                    g_mode = MODE_DIFFICILE;
                    DialogBox(
                        GetModuleHandle(NULL),
                        MAKEINTRESOURCE(IDD_INPUT_DIALOG),
                        hwnd,
                        InputDlgProc
                    );
                    return 0;

                case IDBUTTON1: // Mode facile
                    g_mode = MODE_FACILE;
                    DialogBox(
                        GetModuleHandle(NULL),
                        MAKEINTRESOURCE(IDD_INPUT_DIALOG),
                        hwnd,
                        InputDlgProc
                    );
                    return 0;
            case IDBUTTON_ABOUT:
            MessageBox(hwnd,
                "Mastermind - Jeu de déduction\n\n"
                "Version Finale\n"
                "Créé par: Abdelaziz Mohammad Aamer Al Harbi et Youssef Aziz\n\n"
                "Comment jouer:\n"
                "• Le Joueur 1 entre un code secret de 5 chiffres ou génère un code alératoire\n"
                "• Le Joueur 2 essaie de deviner le code\n"
                "• Bien placés = bon chiffre, bonne position\n"
                "• Mal placés = bon chiffre, mauvaise position\n\n"
                "Nombre d\'essais: 20 essais\n"
                "Modes:\n"
                "• Débutant: Chiffres tous différents\n"
                "• Expert: Chiffres peuvent se répéter\n\n"
                "Jokers (disponibles après 10 tentatives):\n"
                "• Joker 1: Révèle un chiffre (coût: 3 tentatives)\n"
                "• Joker 2: Indique un chiffre absent (coût: 5 tentatives)\n\n"
                "Bonne chance!",
                "À propos de Mastermind",
                MB_OK | MB_ICONINFORMATION);
            return 0;
case ID_FILE_QUIT:
            PostQuitMessage(0);
            return 0;
            }
            break;

        case WM_DESTROY: {
            if (bgBrush) DeleteObject(bgBrush);
            if (hIcon) DestroyIcon(hIcon);
            if (hBitmap) DeleteObject(hBitmap);
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    srand(time(NULL));

    memset(&wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    bgBrush = CreateSolidBrush(RGB(0, 255, 0));

    hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BOARD));
    if (hBitmap == NULL) {
        MessageBox(NULL, "Image introuvable!", "Erreur!", MB_ICONERROR | MB_OK);
    }

    hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));

    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "WindowClass";
    wc.hIcon         = hIcon;
    wc.hIconSm       = hIcon;
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Erreur fatale!", "Erreur!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "WindowClass",
        "Mastermind",
        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE),
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        640,
        500,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL) {
        MessageBox(NULL, "Erreur fatale!", "Erreur!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hButton = CreateWindowEx(
        0,
        "BUTTON",
        "Mode Expert",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        220, 390,
        200, 50,
        hwnd,
        (HMENU)IDBUTTON,
        hInstance,
        NULL);

    HWND hButton1 = CreateWindowEx(
        0,
        "BUTTON",
        "Mode Débutant",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        220, 340,
        200, 50,
        hwnd,
        (HMENU)IDBUTTON1,
        hInstance,
        NULL);

    HWND hTooltip = (HWND)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (hTooltip) {
        TOOLINFO ti1 = {0};
        ti1.cbSize = sizeof(TOOLINFO);
        ti1.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
        ti1.hwnd = hwnd;
        ti1.uId = (UINT_PTR)hButton;
        ti1.lpszText = "Mode difficile - Les chiffres peuvent se répéter";
        SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti1);

        TOOLINFO ti2 = {0};
        ti2.cbSize = sizeof(TOOLINFO);
        ti2.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
        ti2.hwnd = hwnd;
        ti2.uId = (UINT_PTR)hButton1;
        ti2.lpszText = "Mode facile - Tous les chiffres doivent être distincts";
        SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti2);
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (bgBrush) DeleteObject(bgBrush);

    return (int)msg.wParam;
}
