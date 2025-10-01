#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header của module directory_reader để sử dụng các hàm của nó
#include "directory_reader/directory_reader.h"

/**
 * @brief Hàm so sánh cho qsort để sắp xếp chuỗi theo alphabet.
 */
int compare_strings(const void *a, const void *b) {
    const char *str_a = *(const char**)a;
    const char *str_b = *(const char**)b;
    return strcmp(str_a, str_b);
}

int main(int argc, char *argv[]) {
    // --- SỬA LỖI ---
    // Báo cho trình biên dịch rằng chúng ta cố ý không dùng argc và argv
    (void)argc;
    (void)argv;
    // --- KẾT THÚC SỬA LỖI ---

    printf("Chương trình Lister đang chạy...\n");

    // 1. LẤY DỮ LIỆU
    DirectoryContent content = read_directory(".");

    if (content.entries == NULL) {
        fprintf(stderr, "Không thể đọc nội dung thư mục.\n");
        return 1;
    }

    // 2. SẮP XẾP DỮ LIỆU
    qsort(content.entries,
          content.count,
          sizeof(char*),
          compare_strings);

    // 3. IN KẾT QUẢ (Tạm thời in ở đây)
    printf("----------------------------------------\n");
    for (int i = 0; i < content.count; i++) {
        printf("%s\n", content.entries[i]);
    }
    printf("----------------------------------------\n");


    // 4. DỌN DẸP BỘ NHỚ
    free_directory_content(content);

    return 0;
}