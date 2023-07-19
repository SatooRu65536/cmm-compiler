#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\x1b[31m"
#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"

void runBuildCmd(char *, int);
int hasFlag(char *, char const *[]);
void outputHelp(void);
void outputError(int, char *, int);
void copy2File(FILE *, FILE *);
void addIncluede(FILE *, FILE *);
void readFile(FILE *, FILE *);
void setupFile(FILE **, FILE **, char *, char *);
void getFileName(int, const char *[], char **, char **);

int lineNum = 0;
int charNum = 0;
char *fileName = NULL;

int isNeedStdio = 0;
int isNeedStdlib = 0;
int isNeedString = 0;
int isNeedMath = 0;

int main(int argc, char const *argv[]) {
  FILE *inputFile;
  FILE *outFile;
  FILE *tmpFile;

  char *inputFileName;
  char *outputFileName = "out.c";
  char *tmpFileName = "/tmp/me.satooru.cmmtmp-c";

  if (hasFlag("-h", argv)) {
    outputHelp();
    return 0;
  }

  // 入出力ファイル名を設定する
  getFileName(argc, argv, &inputFileName, &outputFileName);

  // -r オプションがある時は出力ファイル名を変更する
  if (hasFlag("-r", argv) == 1) outputFileName = "/tmp/me.satooru.cmmout.c";

  // 入力ファイルとtmpファイルを開く
  setupFile(&inputFile, &tmpFile, inputFileName, tmpFileName);

  // ファイルを読み込む
  readFile(inputFile, tmpFile);

  // ファイルを閉じる
  fclose(inputFile);
  fclose(tmpFile);

  // tmpファイルと出力ファイルを開く
  setupFile(&tmpFile, &outFile, tmpFileName, outputFileName);

  // inclue　を追加する
  addIncluede(outFile, tmpFile);

  // tmpファイルから出力ファイルにコピーする
  copy2File(tmpFile, outFile);

  // ファイルを閉じる
  fclose(outFile);
  fclose(tmpFile);

  // -r オプションがある時は実行する
  int useClang = hasFlag("-c", argv);
  if (hasFlag("-r", argv)) runBuildCmd(outputFileName, useClang);

  return 0;
}

// gcc でコンパイルして実行する
void runBuildCmd(char *outFile, int useClang) {
  char cmd[256];

  if (useClang)
    sprintf(cmd, "clang %s -o %s", outFile, "/tmp/me.satooru.cmmtmp-bin");
  else
    sprintf(cmd, "gcc %s -o %s", outFile, "/tmp/me.satooru.cmmtmp-bin");

  system(cmd);
  system("/tmp/me.satooru.cmmtmp-bin");
}

// flag が argv に含まれているかどうか
int hasFlag(char *flag, char const *argv[]) {
  int i = 0;
  while (argv[i] != NULL) {
    // flag が完全一致するとき
    if (strstr(flag, argv[i]) != NULL) return 1;

    // -rf のとき -r と -f が含まれていると判定する
    if (argv[i][0] == '-') {
      for (int j = 1; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == flag[1]) return 1;
      }
    }
    i++;
  }
  return 0;
}

// ヘルプを表示する
void outputHelp(void) {
  printf("\n");
  printf(BOLD "C-- Language\n" RESET);
  printf("\n");
  printf("Usage: cmm <options> [input file] [output file]\n");
  printf("\n");
  printf("Flags:\n");
  printf("  -h: ヘルプを表示する.\n");
  printf("  -r: 実行する.\n");
  printf("  -c: clangコマンドでビルドする.\n");
  printf("\n");
  printf("https://github.com/SatooRu65536/cmm-compiler#readme\n");
  printf("\n");
}

// エラーを出力して終了する
void outputError(int errNum, char *word, int isExit) {
  switch (errNum) {
    case 100:
      printf(BOLD "%s:%d:%d " RESET, fileName, lineNum, charNum);
      printf(RED "Error: f文字列中の {} が閉じられていません.\n" RESET);
      break;

    case 101:
      printf(BOLD "%s:%d:%d " RESET, fileName, lineNum, charNum);
      printf(RED "Error: f文字列中の {} 内に : がありません.\n" RESET);
      break;

    case 102:
      printf(BOLD "%s:%d:%d " RESET, fileName, lineNum, charNum);
      printf(
          RED
          "Error: f文字列中でフォーマット演算子が指定されていません.\n" RESET);
      break;

    case 400:
      printf(RED "Error: ファイルが開けません.\n" RESET);
      printf("       ファイル名: %s\n", word);
      break;

    case 401:
      printf(RED "Error: 出力ファイルが開けません.\n" RESET);
      printf("       ファイル名: %s\n", word);
      break;

    case 402:
      printf(RED "Error: 入力ファイルを指定してください.\n" RESET);
      break;

    default:
      printf(RED "Error: エラー内容が未定義です.\n" RESET);
      break;
  }

  if (isExit) exit(1);
}

// ファイルに文字列を書き込む
void write2File(FILE *file, char *word) { fprintf(file, "%s", word); }

// ファイルに文字を書き込む
void put2File(FILE *file, char c) { fprintf(file, "%c", c); }

// tmpファイルから出力ファイルにコピーする
void copy2File(FILE *origin, FILE *duplicate) {
  char line[256];
  while (fgets(line, sizeof(line), origin) != NULL) {
    write2File(duplicate, line);
  }
}

// include を追加し、出力ファイルを作成する
void addIncluede(FILE *outFile, FILE *tmpFile) {
  // 追記するデータをtmpファイルに書き込む
  if (isNeedStdio) write2File(outFile, "#include <stdio.h>\n");
  if (isNeedStdlib) write2File(outFile, "#include <stdlib.h>\n");
  if (isNeedString) write2File(outFile, "#include <string.h>\n");
  if (isNeedMath) write2File(outFile, "#include <math.h>\n");

  if (isNeedStdio || isNeedStdlib || isNeedString || isNeedMath) {
    put2File(outFile, '\n');
  }
}

// 文字列の変数名をフォーマット指定子に置き換える
void replaceString(char *word, char *str) {
  // f"{hoge:d} {fuga:lf}" -> "%d %d"
  int i = 1;
  int j = 0;

  while (1) {
    char c = word[i];
    if (c == '\0') break;

    if (c != '{') {
      str[j] = c;
      j++;
    } else {
      i++;
      while (1) {
        c = word[i];
        i++;
        if (c == '}') outputError(101, word, 1);
        if (c == ':') break;
      }

      char f = 0;
      char format[5] = {'\0'};

      if (word[i] == '}') outputError(102, word, 1);
      while (1) {
        c = word[i];
        format[f] = c;

        if (c == '}') break;
        if (c == '\0') outputError(100, word, 1);

        f++;
        i++;
      }

      str[j] = '%';
      for (int k = 0; k < f; k++) str[j + k + 1] = format[k];
      j += f + 1;
    }
    i++;
  }
}

// 文字列から変数名を取り出す
void pickupVariable(char *word, char *vars) {
  // f"{hoge:d} {fuga:lf}" -> , hoge, fuga
  int i = 0;
  int j = 0;

  while (1) {
    char c = word[i];
    if (c == '\0') {
      break;
    }

    if (c == '{') {
      i++;
      vars[j] = ',';
      vars[j + 1] = ' ';
      j += 2;
      while (1) {
        c = word[i];

        if (c == ':') {
          while (c != '}') {
            c = word[i];
            i++;

            if (c == '\0') outputError(100, word, 1);
          }
          break;
        }

        vars[j] = c;
        i++;
        j++;
      }
    }
    i++;
  }
}

// f文字列を処理する
void processFString(char *word, FILE *tmpFile) {
  char str[256] = {'\0'};
  replaceString(word, str);
  write2File(tmpFile, str);

  char vars[256] = {'\0'};
  pickupVariable(word, vars);
  write2File(tmpFile, vars);
}

// 必要なライブラリを判定する
void checkLib(char *word) {
  if (isNeedStdio != 1) {
    if (strcmp(word, "p") == 0)
      isNeedStdio = 1;
    else if (strcmp(word, "print") == 0)
      isNeedStdio = 1;
    else if (strcmp(word, "printf") == 0)
      isNeedStdio = 1;
    else if (strcmp(word, "s") == 0)
      isNeedStdio = 1;
    else if (strcmp(word, "scan") == 0)
      isNeedStdio = 1;
    else if (strcmp(word, "scanf") == 0)
      isNeedStdio = 1;

    if (isNeedStdio == 1) return;
  }

  if (isNeedStdlib != 1) {
    if (strcmp(word, "rand") == 0)
      isNeedStdlib = 1;
    else if (strcmp(word, "random") == 0)
      isNeedStdlib = 1;
    else if (strcmp(word, "srand") == 0)
      isNeedStdlib = 1;

    if (isNeedStdlib == 1) return;
  }

  if (isNeedString != 1) {
    if (strcmp(word, "strlen") == 0)
      isNeedString = 1;
    else if (strcmp(word, "strcmp") == 0)
      isNeedString = 1;
    else if (strcmp(word, "strcpy") == 0)
      isNeedString = 1;
    else if (strcmp(word, "strcat") == 0)
      isNeedString = 1;

    if (isNeedString == 1) return;
  }

  if (isNeedMath != 0) {
    if (strcmp(word, "sin") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "cos") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "tan") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "asin") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "acos") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "atan") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "atan2") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "exp") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "log") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "log10") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "pow") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "sqrt") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "ceil") == 0)
      isNeedMath = 1;
    else if (strcmp(word, "floor") == 0)
      isNeedMath = 1;

    if (isNeedMath == 1) return;
  }
}

// 識別子を処理する
void processIdend(char *word, FILE *tmpFile) {
  if (strcmp(word, "p") == 0 || strcmp(word, "print") == 0) {
    write2File(tmpFile, "printf");
  } else if (strcmp(word, "s") == 0 || strcmp(word, "scan") == 0) {
    write2File(tmpFile, "scanf");
  } else if (strcmp(word, "Main") == 0) {
    write2File(tmpFile, "int main");
  } else if (strcmp(word, "Arg") == 0) {
    write2File(tmpFile, "int argc, char const *argv[]");
  } else {
    write2File(tmpFile, word);
  }

  checkLib(word);
}

// 単語を評価する
void evalWord(char *word, int isFunc, FILE *tmpFile) {
  // f文字列の場合
  if (word[0] == 'f' && word[1] == '"') {
    processFString(word, tmpFile);
    return;
  }

  // 関数ではないとき
  if (!isFunc) {
    if (strcmp(word, "M_PI") == 0) isNeedMath = 1;
    write2File(tmpFile, word);
    return;
  }

  // 関数名を変換, またはそのまま出力
  processIdend(word, tmpFile);
}

// 1行をパースする
void parseLine(char *line, FILE *tmpFile) {
  int i = 0;
  char word[256] = {'\0'};

  while (1) {
    char c = *line;
    int isFunc = 0;

    switch (c) {
      case '\0':
        word[i] = '\0';
        evalWord(word, 0, tmpFile);
        charNum = 0;
        return;

      case '\n':
        word[i] = '\0';
        evalWord(word, 0, tmpFile);
        write2File(tmpFile, "\n");
        charNum = 0;
        return;

      case '"':
        // 次の " までを文字列として扱う
        word[i] = c;
        i++;
        line++;
        charNum++;
        while (1) {
          c = *line;
          word[i] = c;
          i++;
          if (c == '"') {
            break;
          }
          line++;
          charNum++;
        }
        break;

      case '(':
      case ')':
        isFunc = 1;
      case ' ':
      case '{':
      case '}':
      case ',':
      case ';':
        word[i] = '\0';
        evalWord(word, isFunc, tmpFile);
        put2File(tmpFile, c);
        i = 0;
        break;

      default:
        word[i] = c;
        i++;
        break;
    }
    line++;
    charNum++;
  }
  charNum = 0;
}

// ファイルを読み込む
void readFile(FILE *fp, FILE *tmpFile) {
  // 1行ずつ読み込む
  char line[256];
  while (fgets(line, sizeof(line), fp) != NULL) {
    lineNum++;
    parseLine(line, tmpFile);
  }
}

// ファイルを開く
void setupFile(FILE **rFile, FILE **wFile, char *rFilename, char *wFilename) {
  // ファイルを開く
  *rFile = fopen(rFilename, "r");
  if (*rFile == NULL) outputError(400, rFilename, 1);

  // 出力ファイルを開く
  *wFile = fopen(wFilename, "w");
  if (*wFile == NULL) outputError(401, wFilename, 1);
}

// 入出力ファイル名を設定する
void getFileName(int argc, const char *argv[], char **inFile, char **outFile) {
  int hasInputFile = 0;
  // 引数を解析する
  for (int i = 1; i < argc; i++) {
    // オプションではない時
    if (argv[i][0] != '-') {
      if (hasInputFile == 0) {
        // 入力ファイル名を設定する
        *inFile = (char *)argv[i];
        fileName = (char *)argv[i];
        hasInputFile = 1;
      } else {
        // 出力ファイル名を設定する
        *outFile = (char *)argv[i];
        return;
      }
    }
  }

  // 入力ファイルがなかった場合
  if (hasInputFile == 0) {
    outputError(402, NULL, 0);
    outputHelp();
    exit(0);
  }
}
