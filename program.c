#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#define SIZE 10
#define MAX_CHAR 256


// 標準入力から1行単位で文字列を読み取る
char *my_gets(char* s) {
    char *cpNewLineAdr;
    int  iChar;

    if(fgets(s, MAX_CHAR, stdin) == NULL) return NULL;

    // 第1引数の文字列の中から第2引数の文字を検索し、その位置のアドレスを返す
    cpNewLineAdr = strchr(s, '\n');

    if(cpNewLineAdr){
        // 改行文字があった場合、NULL文字に置き換える
        *cpNewLineAdr = '\0';
    }else{
        // 入力ストリーム上に文字が残ってる場合、改行文字が読み取られるまで空読みする
        for(;;){
            // 標準入力から1文字単位で文字を読み取る
            iChar = getchar();
            // 改行文字かEOFが出てきたら終了
            if(iChar == '\n' || iChar == EOF) break;
        }
    }
    return s;
}


// ファイルが存在するか判定する
int existFile(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }

    // return (st.st_mode & S_IFMT) == S_IFREG;
	return S_ISREG(st.st_mode);
}


// メイン
int main() {
    while (1)
    {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(-1);
        } else if (pid == 0) {
            char readline[MAX_CHAR] = {'\0'};

            if(my_gets(readline) != NULL){
                // 入力した文字列をそのまま表示
                printf("入力：%s\n", readline);
            }else{
                printf("文字列の読み取りに失敗しました\n");
            }

            // 入力された文字列を空白で分割にして配列にする
            int  i = 0;
            char *argv[SIZE];

			// 半角スペースで入力文字列を分割する
            argv[i] = strtok(readline, " ");
            do
                {
                argv[++i] = strtok(NULL, " ");
                } while ( (i < SIZE) && (argv[i] != NULL));

			// 入力されたpathにファイルが存在するか判定
            if (existFile(argv[0])) {
                puts("指定されたファイルは存在するので続けます");
                char ab_path[32];
				// pathは全て絶対pathに変換
                realpath(argv[0], ab_path);
                printf("pathは絶対pathに変換します->%s\n", ab_path);
                argv[0] = ab_path;
                puts("実行結果↓");
				// 実行する
                execv(argv[0], argv);
            }
            else {
                puts("指定されたファイルは存在しません。もう一度入力してください。");
            }

            perror("error");
            exit(-1);
        }

        // 親プロセス
        int status;
		//子プロセスの終了を待つ
        pid_t r = waitpid(pid, &status, 0);
        if (r < 0) {
            perror("waitpid");
            exit(-1);
        }
		// 子プロセスが正常終了の時
        if (WIFEXITED(status)) {
            printf("child exit-code=%d\n", WEXITSTATUS(status));
        } else {
            printf("child status=%04x\n", status);
        }
    }

	return 0;
}
