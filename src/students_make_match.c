#pragma warning(disable : 4996)

#define CHECK

#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<hpdf.h>

const char *file_log_path = "log.txt";
FILE *file_log = NULL;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

float colors[8][3] = {
    { 0.2, 0.4, 0.6 },
    { 0.4, 0.8, 0.4 },
    { 0.6, 0.4, 0.2 },
    { 0.8, 0.8, 0.8 },
    { 0.2, 0.2, 0.6 },
    { 0.4, 0.8, 0.4 },
    { 0.6, 0.4, 0.2 },
    { 0.8, 0.8, 0.8 },
};

const u32 colors_count = 8;

#if 0
const char names[][80] = {
	"Leana Klueser",
	"Jael",
	"Lea",
	"Anastasia Kadoglou Hallo mein",
	"Luca",
	"Leandro",
	"Emil",
	"Felix",
    "Iva",
    "Louisa",
    "Lia",
    "Jonathan",
    "Timon",
    "Robin",
    "Niklas",
    "Oskar",
};
#elif 1
typedef struct Student {
    char name[50];
    bool marked;
} Student;

Student *students = NULL;

#else
const char names[][25] = {
	"1", "2", "3", "4", "5", "6", "7", "8",
    "9", "10", "11", "12", "13", "14", "15", "16",
};
#endif

const bool names_marks[16] = {
    false, false, false
};

u16 names_count = 16;

void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	printf ("ERROR: error_no=%04X, detail_no=%d\n",
		(unsigned int) error_no, (int) detail_no);
}

void text_underline(HPDF_Page page, float text_width, float text_x, float text_y)
{
    // Draw underline
    float underline_y = text_y - 6; // Adjust position of underline
    HPDF_Page_MoveTo(page, text_x, underline_y);
    HPDF_Page_LineTo(page, text_x + text_width, underline_y);
    HPDF_Page_Stroke(page);
}

u32 students_load(char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Can't find path: %s\n", path);
        exit(1);
    }

    students = malloc(100 * sizeof(*students));

    char c = 0;
    u32 i = 0;
    u32 pos = 0;
    while ((c = fgetc(file)) != EOF)
    {
        if (c == '\n')
        {
            students[i].name[pos] = 0;
            i++;
            pos = 0;
        }
        if (c >= 0x41 && c <= 0x5a ||
            c >= 0x61 && c <= 0x7a ||
            c == ' ' ||
            c >= 0x30 && c <= 0x39)
            students[i].name[pos++] = c;
    }

    students[i].name[pos] = 0;
    i++;

    fclose(file);

    return i;
}

#include<Windows.h>
#include<tchar.h>

LPSTR OpenFileDialog() {
    OPENFILENAME ofn;       // Struktur für die Datei-Dialog Informationen
    char *result = malloc(260 * sizeof(*result));
    result[0] = 0;

    // Initialisierung der Struktur
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // Kein Besitzerfenster
    ofn.lpstrFile = result;
    ofn.nMaxFile = 260;
    // Filter für Dateitypen
    ofn.lpstrFilter = _T("Alle Dateien\0*.*\0Textdateien\0*.TXT\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Zeigt das Datei-Auswahl-Dialogfeld an
    if (GetOpenFileName(&ofn) == TRUE) {
        _tprintf(_T("Ausgewählte Datei: %s\n"), ofn.lpstrFile);
    } else {
        _tprintf(_T("Keine Datei ausgewählt oder ein Fehler ist aufgetreten.\n"));
    }

    return ofn.lpstrFile;
}
#if 0
int main(void)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
#if 1
    char *path = OpenFileDialog();
#endif
#if 0
    names_count = students_load("numbers.txt");
#elif 1
    names_count = students_load(path);
    free(path);
#else
    names_count = students_load("schueler.txt");
#endif

    u32 i;
    float content_width = 400;
    float content_height = 600;
    u16 teams_count = names_count / 2;
#ifdef CHECK
    u32 **students_check_indexs;
    students_check_indexs = malloc(names_count * sizeof(*students_check_indexs));
    for (i = 0; i < teams_count; i++)
        students_check_indexs[i] = malloc(2 * sizeof(**students_check_indexs));
#endif

    HPDF_Doc pdf;
    HPDF_Page page;

    // Create a new PDF document
    pdf = HPDF_New(error_handler, NULL);
    if (!pdf) {
        printf("ERROR: cannot create PdfDoc object\n");
        return 1;
    }

    HPDF_Font font = HPDF_GetFont(pdf, "Courier-Bold", NULL);

    u32 week_count;
    for (week_count = 0; week_count < (u32)names_count - 1; week_count++)
    {
        // Add a new page to the document
        page = HPDF_AddPage(pdf);
        // Set page size and orientation
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

        float page_height = HPDF_Page_GetHeight(page);
        float page_width = HPDF_Page_GetWidth(page);
#if 0
        printf("Page height: %.2f points\n", page_height);
        printf("Page width: %.2f points\n", page_width);
#endif

        // Set the rectangle parameters
        float rectangle_width = content_width / 2;
        float rectangle_height = content_height / teams_count;

        float x = (page_width - content_width) / 2;
        float y = (page_height - content_height) / 2;

        // Title
        HPDF_Page_SetFontAndSize(page, font, 35);
        char *text = "Lernpartner";

        float text_width = HPDF_Page_TextWidth(page, text);
        float text_x = page_width / 2 - text_width / 2;
        float text_y = page_height - 80;
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, text_x, text_y, text);
        HPDF_Page_EndText(page);

        for (i = 0; i < names_count; i++)
            students[i].marked = false;

        u8 mark_switch = 0;
        for (i = 0; i < teams_count; i++)
        {
            HPDF_Page_SetRGBFill(page,
                colors[week_count % colors_count][0],
                colors[week_count % colors_count][1],
                colors[week_count % colors_count][2]);
            HPDF_Page_Rectangle(page, x, y, rectangle_width, rectangle_height);
            HPDF_Page_Rectangle(page, x + rectangle_width, y, rectangle_width, rectangle_height);
            HPDF_Page_FillStroke(page);


            HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
            
            HPDF_Page_SetFontAndSize(page, font, 20);


            u32 names_index0 = (i == 0) ?0 :(week_count + ((names_count - 2) - (i - 1))) % (names_count - 1) + 1;
            u32 names_index1 = (week_count + i) % (names_count - 1) + 1;

            if (!(i == 0 && names_index1 == 1 ||
                (i == teams_count &&
                names_index1 % 2 == 0 && names_index0 - names_index1 == 1)))
            {
                if (mark_switch)
                {
                    students[names_index1].marked = true;
                    mark_switch = 0;
                }
                else
                {
                    students[names_index0].marked = true;
                    if (!(i == 0 &&
                        names_index1 % 2 == 1))
                        mark_switch = 1;
                }
            }
#ifdef CHECK
            students_check_indexs[i][0] = names_index0;
            students_check_indexs[i][1] = names_index1;
#endif
#if 0
            if (!(names_index0 % 2 == 0 &&
                names_index1 - names_index0 == 1 ||
                names_index1 % 2 == 0 &&
                names_index0 - names_index1 == 1))
            {
                u32 neighbour_index0;
                u32 neighbour_index1;
                neighbour_index0 = names_index0;
                neighbour_index0 += (names_index0 % 2) ? -1 : 1;
                neighbour_index1 = names_index1;
                neighbour_index1 += (names_index1 % 2) ? -1 : 1;

                if (students[neighbour_index0].marked)
                    students[names_index1].marked = true;
                else
                    students[names_index0].marked = true;

                if (students[names_index0].marked && students[names_index1].marked ||
                    students[neighbour_index0].marked && students[neighbour_index1].marked)
                    printf("fail:\nweek count: %d i: %d\n", week_count, i);
            }
#endif


#if 0
                if (students[names_index0].marked == false &&
                    students[neighbour_index].marked == false)
                {

                    students[names_index1].marked = true;
                    students[neighbour_index].marked = true;
                }
                else
                {
                    neighbour_index = names_index1;
                    neighbour_index += (names_index1 % 2) ?-1 :1;
                    if (students[names_index0].marked)
                        students[neighbour_index].marked = true;
                    else
                        students[names_index1].marked = true;
                }
            }
#endif

#if 0
            if (!(names_index0 % 2 == 0 && names_index1 - names_index0 == 1))
            {
                u32 chair_partner = names_index0 - 1;
                if (names_index0 % 2 == 0)
                    chair_partner = names_index0 + 1;

                if (!students[chair_partner].marked)
                    students[names_index0].marked = true;

                chair_partner = names_index1 - 1;
                if (names_index1 % 2 == 0)
                    chair_partner = names_index1 + 1;

                if (!students[chair_partner].marked)
                    students[names_index1].marked = true;
            }

            if (!week_count % 2 == 0)
            {
                u32 j;
                for (j = 0; j < teams_count; j += 2)
                {
                    students[j].marked = false;
                    students[j + 1].marked = true;
                }
            }
#endif

            // text 0
            const char *text;
            text = students[names_index0].name;
            float text_width = HPDF_Page_TextWidth(page, text);
            u16 font_size = 20;
            while (text_width >= rectangle_width - 20)
            {
                HPDF_Page_SetFontAndSize(page, font, --font_size);
                text_width = HPDF_Page_TextWidth(page, text);
            }

            float text_x = x + (rectangle_width - text_width) / 2;
            float text_y = y + (rectangle_height - 20) / 2;
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, text_x, text_y, text);
            HPDF_Page_EndText(page);
            if (students[names_index0].marked)
                text_underline(page, text_width, text_x, text_y);
            // text 1
            text = students[names_index1].name;
            text_width = HPDF_Page_TextWidth(page, text);
            font_size = 20;
            while (text_width >= rectangle_width - 20)
            {
                HPDF_Page_SetFontAndSize(page, font, --font_size);
                text_width = HPDF_Page_TextWidth(page, text);
            }
            text_x = x + (rectangle_width - text_width) / 2 + rectangle_width;
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, text_x, text_y, text);
            HPDF_Page_EndText(page);
            if (students[names_index1].marked)
                text_underline(page, text_width, text_x, text_y);

            y += rectangle_height;

        }
        HPDF_Page_SetFontAndSize(page, font, 35);
        char text_bottom[20];
        sprintf(text_bottom, "%d", week_count + 1);
        
        text_width = HPDF_Page_TextWidth(page, text_bottom);
        text_x = page_width / 2 - text_width / 2;
        text_y = 50;
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, text_x, text_y, text_bottom);
        HPDF_Page_EndText(page);

        // neighbour check students_check_indexs
        u32 k;
#if 0

        for (k = 0; k < teams_count; k++)
        {
            u32 students_index0 = students_check_indexs[k][0];
            u32 students_index1 = students_check_indexs[k][1];
            u32 neighbour_index0;
            u32 neighbour_index1;
            neighbour_index0 = students_index0;
            neighbour_index0 += (students_index0 % 2) ? -1 : 1;
            neighbour_index1 = students_index1;
            neighbour_index1 += (students_index1 % 2) ? -1 : 1;

            bool *mark0 = &students[students_index0].marked;
            bool *mark1 = &students[students_index1].marked;
            bool *neighbour_mark0 = &students[neighbour_index0].marked;
            bool *neighbour_mark1 = &students[neighbour_index1].marked;

        }
#endif

        for (k = 0; k < teams_count; k++)
        {
            if (students[students_check_indexs[k][0]].marked && students[students_check_indexs[k][1]].marked)
                printf("failed partner");
            u32 neighbour_index0;
            u32 neighbour_index1;
            neighbour_index0 = students_check_indexs[k][0];
            neighbour_index0 += (students_check_indexs[k][0] % 2) ? -1 : 1;
            neighbour_index1 = students_check_indexs[k][1];
            neighbour_index1 += (students_check_indexs[k][1] % 2) ? -1 : 1;

            if (students_check_indexs[k][0] == neighbour_index1)
                continue;

            if (students[students_check_indexs[k][0]].marked && students[neighbour_index0].marked)
                printf("failed neighbour 0 both true: week count: %d i: %d\n", week_count, k);

            if (students[students_check_indexs[k][1]].marked && students[neighbour_index1].marked)
                printf("failed neighbour 1 both true: week count: %d i: %d\n", week_count, k);

            if (students[students_check_indexs[k][0]].marked == false && students[neighbour_index0].marked == false)
                printf("failed neighbour 0 both false: week count: %d i: %d\n", week_count, k);

            if (students[students_check_indexs[k][1]].marked == false && students[neighbour_index1].marked == false)
                printf("failed neighbour 1 both false: week count: %d i: %d\n", week_count, k);

        }
    }

    // Save the document to a file
    HPDF_SaveToFile(pdf, "./output.pdf");

    // Clean up
    HPDF_Free(pdf);

	return 0;
}
