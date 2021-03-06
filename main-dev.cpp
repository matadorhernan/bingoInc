#include <sstream>
#include <Windows.h>
#include <string>
#include <string.h>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cctype>
using namespace std;
namespace std
{
    template <typename T>
    std::string to_string(const T &n)
    {
        std::ostringstream s;
        s << n;
        return s.str();
    }
} // namespace std
enum GameType
{
    kHorizontal = 1,
    kFullCard,
    kMainDiagonal
};
struct Point
{
    int x;
    int y;
};
struct Area
{
    int width;
    int height;
};
struct StringVector
{
    int lineCount;
    string *content;
};
struct StringMatrix
{
    int height;
    int width;
    string **content;
};
struct IntVector
{
    int lineCount;
    int *content;
};
struct IntMatrix
{
    int height;
    int width;
    int **content;
};
struct Menu
{
    bool enabled;
    string text;
    string query;
    int itemCount;
    struct Menu *items;
};
struct Player
{
    string name;
    StringMatrix card;
    IntMatrix scored;
    int score;
    int scoredCount;
    string timestamp;
};
struct Score
{
    string name;
    string gameType;
    string score;
    string timestamp;
};
struct ScoreVector
{
    int itemCount;
    Score *score;
};
struct BingoGame
{
    string title;
    Menu menu;
    GameType gameType;
    Player *players;
    IntVector numbers;
};
void gotoxy(Point point);
string getTimeStamp();
string colorANSI(string text, int format, int foreground, int background);
int getUnformattedANSINumber(string formattedANSINumber);
StringVector splitString(string delimiter, string text);
StringVector stringRectangle(Area area, int style);
void shuffleIntVector(IntVector &vector);
void shuffleStringVector(StringVector &vector);
IntVector getVectorFromRange(int min, int max);
void getCardFromVector(StringMatrix &card, IntVector vector, int numbersPerColumn, int skip);
StringVector readFile(string filename);
void writeFile(string filename, StringVector content);
ScoreVector readScoreData(string filename);
void writeScoreData(string filename, Score score);
bool isInRange(int number, int min, int max);
bool isAlias(string alias, int min, int max);
void cleanView(Point start, Point end);
void displayFrame(string title);
void displayWatermark();
void displayMenu(Menu menu);
string getQueryMenu(Menu menu);
void displayAlias(string aliasA, string aliasB);
string getQueryAlias(string query);
void displayCard(Player player, Point point);
void displayCards(Player playerA, Player playerB);
void displayReport(Player player, Point point);
void callNumber(string text, IntVector vector, int index, int duration);
void callPlayer(string text, Player player, int duration);
void displayStats(ScoreVector vector);
BingoGame initBingo();
bool bingoCore(BingoGame &game);
void handleAlias(BingoGame &game);
void handleCards(BingoGame &game);
bool handleStats(BingoGame &game);
bool searchCard(StringMatrix &card, int number, GameType gametype);
bool checkPlayerVictory(Player &player, GameType gametype);
int main(int argc, char const *argv[])
{
    srand(time(NULL));
    system("pause");
    bool canRepeat = true;
    BingoGame game = initBingo();
    displayWatermark();
    while ((canRepeat = bingoCore(game)))
        ;
    system("cls");
    return 0;
}
int getUnformattedANSINumber(string formattedANSINumber)
{
    StringVector tokens = splitString("m", formattedANSINumber);
    return tokens.lineCount == 0 ? atoi(formattedANSINumber.c_str()) : atoi(tokens.content[1].c_str());
}
string getTimeStamp()
{
    time_t now = time(NULL);
    tm *timeStamp = localtime(&now);
    char time[32];
    strftime(time, 32, "%a, %d.%m.%Y %H:%M:%S", timeStamp);
    return string(time);
}
void gotoxy(Point point)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {(SHORT)point.x, (SHORT)point.y});
}
string colorANSI(string text, int format, int foreground, int background)
{

    return "\033[" +
           ((format >= 1 && format <= 3) ? to_string(format) : "0") + ';' +
           (foreground >= 0 && foreground <= 255 ? "38;5;" + to_string(foreground) : "39") + ';' +
           (background >= 0 && background <= 255 ? "48;5;" + to_string(background) : "49") + "m" +
           text +
           "\033[0m";
}
StringVector splitString(string delimiter, string text)
{
    string textCpy = text;
    size_t pos = 0;
    int i = 0;
    for (i = 0; (pos = textCpy.find(delimiter)) != string::npos; i++)
        textCpy.erase(0, pos + delimiter.length());
    StringVector content = {i, new string[i]};
    for (i = 0; (pos = text.find(delimiter)) != string::npos; i++)
    {
        content.content[i] = text.substr(0, pos);
        text.erase(0, pos + delimiter.length());
    }
    return content;
}
StringVector stringRectangle(Area area, int style)
{
    StringVector canvas = {area.height, new string[area.height]};
    style = (style != 2 && style != 1) ? 0 : (style - 1) * 6;
    char box[12] = {'\xDA', '\xBF', '\xC0', '\xD9', '\xB3', '\xC4', '\xC9', '\xBB', '\xC8', '\xBC', '\xBA', '\xCD'};
    string Line(area.width - 2, box[style + 5]), Fill(area.width - 2, ' ');
    canvas.content[0] = box[style] + Line + box[style + 1] + '\n';
    for (int a = 1; a < area.height - 1; a++)
        canvas.content[a] = box[style + 4] + Fill + box[style + 4] + '\n';
    canvas.content[area.height - 1] = box[style + 2] + Line + box[style + 3];
    return canvas;
}
StringVector readFile(string filename)
{
    StringVector fc;
    fc.lineCount = 0;
    string str;
    fstream file;
    file.open(filename.c_str(), ios::in);
    if (file.is_open())
    {
        int i = 0;
        while (getline(file, str))
            i++;
        fc.lineCount = i;
        file.clear();
        file.seekg(0);
        fc.content = new string[i];
        i = 0;
        while (getline(file, str))
        {
            fc.content[i] = str;
            i++;
        }
        file.close();
    }
    return fc;
}
void writeFile(string filename, StringVector content)
{
    string str;
    fstream file;
    file.open(filename.c_str(), ios::app);
    if (file.is_open())
    {
        for (int i = 0; i < content.lineCount; i++)
            file << content.content[i] << endl;
        file.close();
    }
}
ScoreVector readScoreData(string filename)
{
    StringVector scoreData = readFile(filename);
    ScoreVector scoreVector = {scoreData.lineCount, new Score[scoreData.lineCount]};
    for (int i = 0; i < scoreData.lineCount; i++)
    {
        StringVector scoreToken = splitString("_", scoreData.content[i]);
        scoreVector.score[i].name = scoreToken.content[0];
        scoreVector.score[i].gameType = scoreToken.content[1];
        scoreVector.score[i].score = scoreToken.content[2];
        scoreVector.score[i].timestamp = scoreToken.content[3];
    }
    return scoreVector;
}
void writeScoreData(string filename, Score score)
{
    StringVector scoreData = {1, new string[1]};
    scoreData.content[0] = score.name + "_" + score.gameType + "_" + score.score + "_" + score.timestamp + "_";
    writeFile(filename, scoreData);
}
void shuffleIntVector(IntVector &vector)
{
    if (vector.lineCount > 1)
        for (int i = 0; i < vector.lineCount - 1; i++)
        {
            int j = i + rand() / (RAND_MAX / (vector.lineCount - i) + 1), t = vector.content[j];
            vector.content[j] = vector.content[i];
            vector.content[i] = t;
        }
}
void shuffleStringVector(StringVector &vector)
{
    if (vector.lineCount > 1)
        for (int i = 0; i < vector.lineCount - 1; i++)
        {
            int j = i + rand() / (RAND_MAX / (vector.lineCount - i) + 1);
            string t = vector.content[j];
            vector.content[j] = vector.content[i];
            vector.content[i] = t;
        }
}
IntVector getVectorFromRange(int min, int max)
{
    IntVector vector;
    if (max - min > 1)
    {
        vector = {max - min + 1, new int[max - min + 1]};
        for (int i = 0; i < max - min + 1; i++)
            vector.content[i] = min + i;
    }
    return vector;
};
void getCardFromVector(StringMatrix &card, IntVector vector, int numbersPerColumn, int skip)
{
    card.content = new string *[card.height];
    for (int i = 0; i < card.height && i < vector.lineCount; i++)
    {
        card.content[i] = new string[card.width];
        StringVector aux = {card.width, new string[card.width]};
        for (int j = 0; j < card.width; j++)
            aux.content[j] = to_string((j < numbersPerColumn) ? vector.content[j + (i * numbersPerColumn) + skip] : 0);
        shuffleStringVector(aux);
        for (int j = 0; j < card.width; j++)
            card.content[i][j] = aux.content[j];
    }
}
bool isInRange(int number, int min, int max)
{
    return number >= min && number <= max;
}
bool isAlias(string alias, int min, int max)
{
    return isalpha(alias.at(0)) && isInRange(alias.length(), min, max);
}
void cleanView(Point start, Point end)
{
    for (int i = 0; i < end.y - start.y; i++)
    {
        gotoxy({start.x, start.y + i});
        cout << string(end.x - start.x, ' ');
    }
}
void displayFrame(string title)
{
    system("cls");
    StringVector frames[4] = {stringRectangle({147, 37}, 1), stringRectangle({147 - 4, 4}, 2),
                              stringRectangle({147 - 4, 37 - 11}, 2), stringRectangle({147 - 4, 5}, 1)};
    for (int i = 0; i < 37; i++)
    {
        gotoxy({0, i});
        cout << frames[0].content[i];
    }
    for (int i = 0; i < 37 - 11; i++)
    {
        if (i < 4)
        {
            gotoxy({2, 1 + i});
            cout << colorANSI(frames[1].content[i], 1, 192, 9);
        }
        if (i < 5)
        {
            gotoxy({2, 37 - 6 + i});
            cout << frames[3].content[i];
        }
        gotoxy({2, 5 + i});
        cout << frames[2].content[i];
    }
    string titleFrame(147 - 7, ' ');
    for (long long unsigned int i = 0; i < title.length(); i++)
        titleFrame.at(i + 3) = title.at(i);
    gotoxy({3, 3});
    cout << colorANSI(titleFrame, 1, 192, 9);
}
void displayWatermark()
{
    system("cls");
    StringVector content = readFile("data/logo.txt");
    for (int i = 0; i < content.lineCount; i++)
    {
        gotoxy({18, i + 8});
        cout << colorANSI(content.content[i], 1, i < 20 ? 32 + i : 256, 256);
    }
    gotoxy({60, 30});
    system("pause");
    system("cls");
}
void displayMenu(Menu menu)
{
    cleanView({3, 6}, {144, 30});
    gotoxy({6, 7});
    cout << menu.text;
    for (int i = 0; i < menu.itemCount; i++)
    {
        gotoxy({6, 9 + i});
        cout << i + 1 << ") " << left << setw(32) << menu.items[i].text
             << colorANSI(((menu.items[i].enabled) ? "[Hablitado]" : "[Deshablitado] "), 0, ((menu.items[i].enabled) ? 10 : 9), 256)
             << endl;
    }
}
string getQueryMenu(Menu menu)
{
    string selectedOption;
    bool invalidOption = true;
    while (invalidOption)
    {
        cleanView({3, 32}, {144, 35});
        gotoxy({6, 33});
        cout << menu.query;
        getline(cin, selectedOption);
        if (invalidOption = selectedOption.empty() ||
                            selectedOption.find_first_not_of("0123456789") != string::npos ||
                            !isInRange(atoi(selectedOption.c_str()), 1, menu.itemCount) ||
                            !menu.items[atoi(selectedOption.c_str()) - 1].enabled)
        {
            gotoxy({6, 33});
            cout << colorANSI("Esta opcion puede estar deshabilitada o ser invalida, intentalo de nuevo...", 1, 9, 256);
            Sleep(3000);
        }
    }
    return selectedOption;
}
void displayAlias(string aliasA, string aliasB)
{
    cleanView({3, 6}, {144, 30});
    StringVector playerFile = readFile("data/player.txt");
    gotoxy({30, 26});
    cout << colorANSI(aliasA.empty() ? "Jugador A" : aliasA + " (Jugador A)", 1, 196, 256);
    gotoxy({10 + 75, 26});
    cout << colorANSI(aliasB.empty() ? "Jugador B" : aliasB + " (Jugador B)", 1, 33, 256);
    for (int i = 0; i < playerFile.lineCount; i++)
    {
        gotoxy({30, 9 + i});
        cout << colorANSI(playerFile.content[i], 1, 196, 256) << endl;
        gotoxy({10 + 75, 9 + i});
        cout << colorANSI(playerFile.content[i], 1, 33, 256) << endl;
    }
}
string getQueryAlias(string query)
{
    string alias;
    bool invalidAlias = true;
    while (invalidAlias)
    {
        cleanView({3, 32}, {144, 35});
        gotoxy({6, 33});
        cout << query;
        getline(cin, alias);
        if ((invalidAlias = (alias.empty() || !isAlias(alias, 4, 12))))
        {
            gotoxy({6, 33});
            cout << colorANSI("Alias invalido, intentalo de nuevo...", 1, 9, 256);
            Sleep(3000);
        }
    }
    return alias;
}
void displayCard(Player player, Point point)
{
    for (int i = 0; i < player.card.height; i++)
        for (int j = 0; j < player.card.width; j++)
        {
            gotoxy({point.x + (4 * j), point.y + (2 * i)});
            cout << setw(4) << player.card.content[i][j];
        }
}
void callNumber(string text, IntVector vector, int index, int duration)
{
    cleanView({3, 32}, {144, 35});
    StringVector bankFrame = stringRectangle({30, 24}, 1);
    for (int i = 0; i < bankFrame.lineCount; i++)
    {
        gotoxy({112, 6 + i});
        cout << bankFrame.content[i];
    }
    gotoxy({112 + 3, 6 + 2});
    cout << colorANSI("Ya salieron: ", 1, 11, 256);
    int col = 0, row = 0;
    for (int i = 0; i < index + 1; i++)
    {
        if (col == 6)
        {
            col = 0;
            row++;
        }
        gotoxy({112 + 3 + (4 * col), 7 + 3 + row});
        cout << setw(4) << vector.content[i];
        col++;
    }
    gotoxy({6, 33});
    cout << colorANSI(string(text + to_string(vector.content[index])), 1, 33, 256);
    Sleep(duration);
}
void displayCards(Player playerA, Player playerB)
{
    gotoxy({9, 8});
    cout << colorANSI(playerA.name.empty() ? "Jugador A" : playerA.name + " (Jugador A)", 1, 196, 256);
    gotoxy({9 + 50, 8});
    cout << colorANSI(playerB.name.empty() ? "Jugador B" : playerB.name + " (Jugador B)", 1, 33, 256);
    displayCard(playerA, {9, 10});
    displayCard(playerB, {50 + 9, 10});
}
void displayReport(Player player, Point point)
{
    int k = 0, l = 0;
    for (int i = 0; i < player.scored.height; i++)
        for (int j = 0; j < player.scored.width; j++)
            if (player.scored.content[i][j] != 0)
            {
                if (k == 7)
                {
                    k = 0;
                    l++;
                }
                gotoxy({point.x + (4 * (k)) + 20, point.y + 3 + l});
                cout << left << setw(4) << player.scored.content[i][j];
                k++;
            }
    gotoxy({point.x, point.y});
    cout << left << setw(20)
         << "Tiempo: " << player.timestamp;
    gotoxy({point.x, point.y + 2});
    cout << left << setw(20)
         << "Aciertos: " << player.scoredCount;
    gotoxy({point.x, point.y + 3});
    cout << left << setw(20)
         << "Lista de Numeros: ";
}
void callPlayer(string text, Player player, int duration)
{
    StringVector tokens = splitString("#", text);
    cleanView({3, 32}, {144, 35});
    gotoxy({6, 33});
    cout << colorANSI(
        tokens.content[0] +
            (player.name.empty() ? "Jugador" : player.name) +
            tokens.content[1] +
            to_string(player.score) +
            tokens.content[2],
        1, 33, 256);
    system("pause");
}
void displayStats(ScoreVector vector)
{
    cleanView({3, 6}, {144, 30});
    StringVector tableHeaders[4] = {stringRectangle({36, 3}, 1), stringRectangle({29, 3}, 1),
                                    stringRectangle({19, 3}, 1), stringRectangle({55, 3}, 1)};
    for (int i = 0; i < tableHeaders[0].lineCount; i++)
    {
        gotoxy({4, 6 + i});
        cout << tableHeaders[0].content[i];
        gotoxy({40, 6 + i});
        cout << tableHeaders[1].content[i];
        gotoxy({69, 6 + i});
        cout << tableHeaders[2].content[i];
        gotoxy({88, 6 + i});
        cout << tableHeaders[3].content[i];
    }
    gotoxy({6, 7});
    cout << "Alias";
    gotoxy({42, 7});
    cout << "Modo";
    gotoxy({71, 7});
    cout << "Puntos";
    gotoxy({90, 7});
    cout << "Fecha";
    int pages = (int)ceil((double)(((double)vector.itemCount) / ((double)20)));
    for (int i = 0; i < pages; i++)
    {
        cleanView({3, 9}, {144, 30});
        for (int j = 0; j < 20 && (i * 20) + j != vector.itemCount; j++)
        {
            gotoxy({6, 9 + j});
            cout << vector.score[(i * 20) + j].name;
            gotoxy({42, 9 + j});
            cout << vector.score[(i * 20) + j].gameType;
            gotoxy({71, 9 + j});
            cout << vector.score[(i * 20) + j].score;
            gotoxy({90, 9 + j});
            cout << vector.score[(i * 20) + j].timestamp;
        }
        if (i != pages - 1)
        {
            cleanView({3, 32}, {144, 35});
            gotoxy({6, 33});
            cout << "Imprimiendo siguiente pagina...";
            system("pause");
        }
    }
    cleanView({3, 32}, {144, 35});
    gotoxy({6, 33});
    system("pause");
}

BingoGame initBingo()
{
    BingoGame game;
    game.title = "Easy Bingo Inc. 2020";
    game.players = new Player[2];
    StringVector menuFile = readFile("data/menu.txt");
    StringVector subMenuFile = readFile("data/submenu.txt");
    game.menu.items = new Menu[menuFile.lineCount];
    game.menu.itemCount = menuFile.lineCount;
    for (int i = 0; i < menuFile.lineCount; i++)
    {
        game.menu.items[i].text = menuFile.content[i];
        game.menu.items[i].enabled = i == 0 || i == 5;
        if (i == 4)
        {
            game.menu.items[i].items = new Menu[subMenuFile.lineCount];
            game.menu.items[i].itemCount = subMenuFile.lineCount;
            for (int j = 0; j < subMenuFile.lineCount; j++)
            {
                game.menu.items[i].items[j].text = subMenuFile.content[j];
                game.menu.items[i].items[j].enabled = true;
            }
            game.menu.items[i].query = "Cual opcion desea?: ";
        }
    }
    game.menu.text = "Menu Principal";
    game.menu.query = "Cual opcion desea?: ";
    return game;
}
bool bingoCore(BingoGame &game)
{
    bool canRepeat = true, isPlaying = false;
    displayFrame(game.title);
    displayMenu(game.menu);
    int selectedMain = atoi((getQueryMenu(game.menu)).c_str());
    switch (selectedMain)
    {
    case 1:
        Beep(600, 500);
        handleAlias(game);
        break;
    case 2:
        Beep(600, 500);
        game.gameType = kHorizontal;
        isPlaying = true;
        break;
    case 3:
        Beep(600, 500);
        game.gameType = kFullCard;
        isPlaying = true;
        break;
    case 4:
        Beep(600, 500);
        game.gameType = kMainDiagonal;
        isPlaying = true;
        break;
    case 5:
    {
        Beep(600, 500);
        bool canSubRepeat = true;
        while ((canSubRepeat = handleStats(game)))
            ;
        break;
    }
    case 6:
        Beep(600, 500);
        canRepeat = false;
        break;
    }

    if (isPlaying)
    {
        int pos;
        handleCards(game);
        shuffleIntVector(game.numbers);
        bool found = false, ended = false;
        for (int i = 0; i < 99; i++)
        {
            cleanView({3, 6}, {144, 30});
            displayCards(game.players[0], game.players[1]);
            callNumber("Salio el numero ", game.numbers, i, 200);
            displayCards(game.players[0], game.players[1]);
            for (int j = 0; j < 2; j++)
                if ((found = searchCard(game.players[j].card, game.numbers.content[i], game.gameType)))
                {
                    game.players[j].scoredCount++;
                    if ((ended = checkPlayerVictory(game.players[j], game.gameType)))
                    {
                        pos = j;
                        break;
                    }
                }
            if (ended)
            {
                cleanView({3, 6}, {144, 30});
                callNumber("Salio el numero ", game.numbers, i, 50);
                displayCards(game.players[0], game.players[1]);
                for (int j = 0; j < 2; j++)
                    displayReport(game.players[j], {9 + (j == 0 ? 0 : 50), 22});
                callPlayer("Felicidades # !!!! Acumulaste: # puntos. #", game.players[pos], 4000);
                writeScoreData(
                    "data/leaderboard.txt",
                    {game.players[pos].name,
                     (game.gameType == 3 ? "Diagonal" : game.gameType == 2 ? "Carta Completa" : "Horizontal"),
                     to_string(game.players[pos].score),
                     game.players[pos].timestamp});
                break;
            }
        }
    }
    return canRepeat;
}
void handleAlias(BingoGame &game)
{
    for (int i = 0; i < 4; i++)
        game.menu.items[i + 1].enabled = true;
    for (int i = 0; i < 2; i++)
    {
        displayAlias(game.players[0].name, game.players[1].name);
        game.players[i].name = getQueryAlias(string("Nombre del Jugador" + string((i == 0) ? " A: " : " B: ")));
    }
}
void handleCards(BingoGame &game)
{
    game.numbers = getVectorFromRange(1, 99);
    shuffleIntVector(game.numbers);
    int randomDimension = rand() % (6 - 3 + 1) + 3;
    int numbersPerColumn = game.gameType == 3 ? randomDimension : game.gameType == 2 ? 9 : 5;
    int skip = game.gameType == 3 ? randomDimension * randomDimension : game.gameType == 2 ? 27 : 15;
    for (int i = 0; i < 2; i++)
    {
        game.players[i].card.height = game.gameType == 3 ? randomDimension : 3;
        game.players[i].card.width = game.gameType == 3 ? randomDimension : 9;
        int width = game.players[i].scored.width = numbersPerColumn;
        int height = game.players[i].scored.height = game.gameType != 1 ? 3 : 1;
        game.players[i].scored.content = new int *[height];
        for (int j = 0; j < height; j++)
        {
            game.players[i].scored.content[j] = new int[width];
            for (int k = 0; k < width; k++)
                game.players[i].scored.content[j][k] = 0;
        }
        game.players[i].scoredCount = 0;
        game.players[i].score = 0;
        getCardFromVector(game.players[i].card, game.numbers, numbersPerColumn, i == 0 ? i : skip);
    }
}
bool handleStats(BingoGame &game)
{
    bool canSubRepeat = true, isgameType = false;
    GameType gameType;
    ScoreVector scoreVector = readScoreData("data/leaderboard.txt");
    displayMenu(game.menu.items[4]);
    int selectedStats = atoi(getQueryMenu(game.menu.items[4]).c_str());
    switch (selectedStats)
    {
    case 1:
        Beep(600, 500);
        if (scoreVector.itemCount == 0)
        {
            cleanView({3, 32}, {144, 35});
            gotoxy({6, 33});
            cout << "Aun no a ganado nadie...";
            system("pause");
            return true;
        }
        displayStats(scoreVector);
        break;
    case 2:
    {
        Beep(600, 500);
        string aliasQuery = getQueryAlias("Nombre del jugador a buscar: ");
        ScoreVector filterPlayer = {0, new Score[scoreVector.itemCount]};
        for (int i = 0; i < scoreVector.itemCount; i++)
            if (aliasQuery.compare(scoreVector.score[i].name) == 0)
            {
                filterPlayer.score[filterPlayer.itemCount] = scoreVector.score[i];
                filterPlayer.itemCount++;
            }
        if (scoreVector.itemCount == 0 || filterPlayer.itemCount == 0)
        {
            cleanView({3, 32}, {144, 35});
            gotoxy({6, 33});
            cout << "Jugador no registrado...";
            system("pause");
            return true;
        }
        displayStats(filterPlayer);
        break;
    }
    case 3:
        Beep(600, 500);
        isgameType = true, gameType = kHorizontal;
        break;
    case 4:
        Beep(600, 500);
        isgameType = true, gameType = kFullCard;
        break;
    case 5:
        Beep(600, 500);
        isgameType = true, gameType = kMainDiagonal;
        break;
    case 6:
        Beep(600, 500);
        canSubRepeat = false;
        break;
    }
    if (isgameType)
    {
        ScoreVector filterGameType = {0, new Score[scoreVector.itemCount]};
        string gameTypeQuery = (gameType == 3 ? "Diagonal" : gameType == 2 ? "Carta Completa" : "Horizontal");
        for (int i = 0; i < scoreVector.itemCount; i++)
            if (gameTypeQuery.compare(scoreVector.score[i].gameType) == 0)
            {
                filterGameType.score[filterGameType.itemCount] = scoreVector.score[i];
                filterGameType.itemCount++;
            }
        if (scoreVector.itemCount == 0 || filterGameType.itemCount == 0)
        {
            cleanView({3, 32}, {144, 35});
            gotoxy({6, 33});
            cout << "Nadie a ganado en modo " << gameTypeQuery << "...";
            system("pause");
            return true;
        }
        displayStats(filterGameType);
    }
    return canSubRepeat;
}
bool searchCard(StringMatrix &card, int number, GameType gametype)
{
    bool found = false;
    int ri = card.height - 1;
    for (int i = 0; i < card.height; i++)
    {
        for (int j = 0; j < card.width; j++)
        {
            int cardNumber = getUnformattedANSINumber(card.content[i][j]);
            if ((found = gametype != 3 ? cardNumber == number : cardNumber == number && (j == i || j == ri)))
            {
                card.content[i][j] = colorANSI(card.content[i][j], 1, 10, 256);
                break;
            }
        }
        ri--;
        if (found)
            break;
    }
    return found;
}
bool checkPlayerVictory(Player &player, GameType gametype)
{
    player.score = 0;
    player.timestamp = getTimeStamp();
    bool ended = false, principal, secondary;
    int scoredCount = 0, rscoredCount = 0, score = 0, rscore = 0, scoredrows = 0, center = 0, ri = player.card.height - 1;
    for (int i = 0; i < player.card.height; i++)
    {
        scoredCount = (gametype != 3) ? 0 : scoredCount;
        player.score = (gametype == 1) ? 0 : player.score;
        for (int j = 0; j < player.card.width; j++)
        {
            if (((player.card.content[i][j]).at(0) == '\033'))
            {
                int number = getUnformattedANSINumber(player.card.content[i][j]);
                if (gametype != 3)
                {
                    scoredCount++;
                    player.score += number;
                    player.scored.content[gametype == 2 ? i : 0][scoredCount] = number;
                    if (scoredCount == player.scored.width)
                        scoredrows++;
                    if ((ended = (scoredrows == player.scored.height)))
                        break;
                }
                else if (gametype == 3)
                {
                    if ((principal = (i == j && ri != i)) || (secondary = (ri == j && ri != i)))
                    {
                        player.scored.content[principal ? 0 : 1][(principal ? scoredCount : rscoredCount)] = number;
                        if (principal)
                        {
                            score += number;
                            scoredCount++;
                        }
                        else
                        {
                            rscore += number;
                            rscoredCount++;
                        }
                    }
                    else if (ri == i && center == 0)
                    {
                        player.scored.content[2][0] = number;
                        center = number;
                    }
                    if ((ended = (center == 0 && (scoredCount == player.scored.width || rscoredCount == player.scored.width)) ||
                                 (center != 0 && (scoredCount == player.scored.width - 1 || rscoredCount == player.scored.width - 1))))
                    {
                        player.score = ((scoredCount == player.scored.width || scoredCount == player.scored.width - 1) ? score : rscore) + center;
                        break;
                    }
                }
            }
        }
        ri--;
        if (ended)
            break;
    }
    return ended;
}
