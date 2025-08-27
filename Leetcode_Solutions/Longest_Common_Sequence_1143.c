int longestCommonSubsequence(char* text1, char* text2) {
    int n = strlen(text1);
    int m = strlen(text2);
    int LCS[n + 1][m + 1];

    for (int i = 0; i <= n; i++)
        LCS[i][0] = 0;
    for (int j = 0; j <= m; j++)
        LCS[0][j] = 0;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (text1[i - 1] == text2[j - 1])
                LCS[i][j] = 1 + LCS[i - 1][j - 1];
            else
                LCS[i][j] = (LCS[i - 1][j] > LCS[i][j - 1]) ? LCS[i - 1][j] : LCS[i][j - 1];
        }
    }

    return LCS[n][m];
}