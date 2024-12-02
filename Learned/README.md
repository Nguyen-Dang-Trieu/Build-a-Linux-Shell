
Reference: https://codeforwin.org/c-programming/c-double-pointer

~~~c

#include <stdio.h>
#include <stdlib.h>

// Định nghĩa struct Oxy
struct Oxy {
    int x;
    int y;
};

int main() {
    int n = 3; // Số lượng điểm cần lưu
    struct Oxy **S; // Con trỏ kép để lưu danh sách các điểm

    // Cấp phát bộ nhớ cho mảng con trỏ
    S = (struct Oxy **)malloc(n * sizeof(struct Oxy *));

    // Thêm các điểm vào danh sách
    for (int i = 0; i < n; i++) {
        S[i] = (struct Oxy *)malloc(sizeof(struct Oxy)); // Cấp phát bộ nhớ cho từng điểm
        S[i]->x = i * 10; // Gán giá trị x
        S[i]->y = i * 20; // Gán giá trị y
    }

    // In danh sách các điểm
    printf("Danh sách các điểm:\n");
    for (int i = 0; i < n; i++) {
        printf("Điểm %d: x = %d, y = %d\n", i + 1, S[i]->x, S[i]->y);
    }

    // Giải phóng bộ nhớ
    for (int i = 0; i < n; i++) {
        free(S[i]); // Giải phóng từng điểm
    }
    free(S); // Giải phóng mảng con trỏ

    return 0;
}

~~~
