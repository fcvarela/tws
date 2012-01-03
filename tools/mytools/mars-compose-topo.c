#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv) {
    // read two bytes
    uint64_t idx = 0;
    int16_t vals[90*128];

    FILE *one, *two, *three, *four;
    one = fopen("megt44s180hb.img", "r");
    two = fopen("megt44s270hb.img", "r");
    three = fopen("megt44s000hb.img", "r");
    four = fopen("megt44s090hb.img", "r");

    for (idx = 0; idx < 90*128; idx++) {
        vals[idx] = 0;
    }

    for (idx = 0; idx < 256; idx++) {
        fwrite(&vals, sizeof(int16_t), 90*128, stdout);
        fwrite(&vals, sizeof(int16_t), 90*128, stdout);
        fwrite(&vals, sizeof(int16_t), 90*128, stdout);
        fwrite(&vals, sizeof(int16_t), 90*128, stdout);
    }

    return;

    for (idx = 0; idx < (44*128); idx++) {
        fread(&vals, sizeof(int16_t), 90*128, one);
        fwrite(&vals, sizeof(int16_t), 90*128, stdout);

        fread(&vals, sizeof(int16_t), 90*128, two);
        fwrite(&vals, sizeof(int16_t), 90*128, stdout);

        fread(&vals, sizeof(int16_t), 90*128, three);
        fwrite(&vals, sizeof(int16_t), 90*128, stdout);

        fread(&vals, sizeof(int16_t), 90*128, four);
        fwrite(&vals, sizeof(int16_t), 90*128, stdout);
    }

    fclose(one);
    fclose(two);
    fclose(three);
    fclose(four);
}
