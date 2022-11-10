#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

typedef struct QuadtreeNode{
    unsigned char blue, green, red;
    uint32_t area;
    int32_t top_left, top_right;
    int32_t bottom_left, bottom_right;
}__attribute__((packed)) QuadtreeNode;

typedef struct pixels{
    unsigned char **blue, **green, **red;
} pixels;

QuadtreeNode *cul_med(pixels *pixels, int size, int i, int j)
{
    QuadtreeNode *nod;
    long long medie_red=0,medie_green=0,medie_blue=0;
    nod = malloc(sizeof(QuadtreeNode));
    for(int k=i; k<i + size ; k++) {
        for (int l=j; l < j + size; l++) {
            medie_red += (int) pixels->red[k][l];
            medie_green += (int) pixels->green[k][l];
            medie_blue += (int) pixels->blue[k][l];
        }
    }
    medie_red = medie_red / (size*size);
    medie_green = medie_green / (size*size);
    medie_blue = medie_blue / (size*size);
    nod->red = medie_red;
    nod->green = medie_green;
    nod->blue = medie_blue;
    return nod;
}

long long med_nod(QuadtreeNode *nod, int size, pixels *pixels, int i, int j)
{
    long long sum=0;
    for(int k = i; k<i + (int)sqrt(nod->area);k++)
        for(int l = j; l<j + (int)sqrt(nod->area); l++) {
            sum += pow((nod->red - pixels->red[k][l]), 2) + pow((nod->green - pixels->green[k][l]), 2) +
                   pow(nod->blue - pixels->blue[k][l], 2);
        }
    return sum/(3*nod->area);
}

QuadtreeNode **fill_vector(QuadtreeNode **imagine, int prag, int size, pixels *pixels, int j, int k, int i, int *len, int *leafnodes)
{

    if(imagine[i]==NULL)
        return imagine;
    if(med_nod(imagine[i], size, pixels, j, k) > prag)
    {
        imagine[i]->top_left = *len;
        imagine[i]->top_right = *len + 1;
        imagine[i]->bottom_right = *len + 2;
        imagine[i]->bottom_left = *len + 3;
        imagine = realloc(imagine, (*len + 4) * sizeof(QuadtreeNode*));
        imagine[*len] = cul_med(pixels, size / 2, j, k);
        imagine[*len]->area = size * size / 4;
        imagine[*len + 1] = cul_med(pixels, size / 2, j, k + size / 2);
        imagine[*len + 1]->area = size * size / 4;
        imagine[*len + 2] = cul_med(pixels, size / 2, j + size / 2, k + size / 2);
        imagine[*len + 2]->area = size * size / 4;
        imagine[*len + 3] = cul_med(pixels, size / 2, j + size / 2, k);
        imagine[*len + 3]->area = size * size / 4;
        *len +=4;
        imagine = fill_vector(imagine, prag, size/2, pixels, j, k, imagine[i]->top_left, len, leafnodes);
        imagine = fill_vector(imagine, prag, size/2, pixels, j,k+size/2, imagine[i]->top_right, len, leafnodes);
        imagine = fill_vector(imagine, prag, size/2, pixels, j+size/2,k+size/2, imagine[i]->bottom_right, len, leafnodes);
        imagine = fill_vector(imagine, prag, size/2, pixels, j+size/2, k, imagine[i]->bottom_left, len, leafnodes);
    }
    else {
        (*leafnodes)++;
        imagine[i]->top_left = -1;
        imagine[i]->top_right = -1;
        imagine[i]->bottom_right = -1;
        imagine[i]->bottom_left = -1;
    }
    return imagine;
}

QuadtreeNode **create_tree(FILE **ppm, int prag, int*len, int *leaf_number)
{
    pixels *pixmap;
    int size=0, i=0, j=0;
    fscanf(*ppm, "%*s %d", &size);
    fscanf(*ppm, "%d %*d", &size);
    fscanf(*ppm, "%*c");
    pixmap = malloc(sizeof(pixels));
    pixmap->red = malloc(size * sizeof(unsigned char *));
    pixmap->green = malloc(size * sizeof(unsigned char *));
    pixmap->blue = malloc(size * sizeof(unsigned char *));
    for(i = 0; i < size; i++)
    {
        pixmap->red[i] = malloc(size * sizeof(unsigned char));
        pixmap->green[i] = malloc(size * sizeof(unsigned char));
        pixmap->blue[i] = malloc(size * sizeof(unsigned char));
    }
    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
        {
            fscanf(*ppm,"%c",&pixmap->red[i][j]);
            fscanf(*ppm,"%c",&pixmap->green[i][j]);
            fscanf(*ppm,"%c",&pixmap->blue[i][j]);
        }
    QuadtreeNode **imagine;
    imagine = malloc(sizeof(QuadtreeNode*));
    imagine[0] = cul_med(pixmap, size, 0, 0);
    imagine[0]->area = size*size;
    (*len)++;
    imagine = fill_vector(imagine, prag, size, pixmap, 0, 0, 0, len, leaf_number);
    for(i = 0; i < size; i++){
        free(pixmap->blue[i]);
        free(pixmap->red[i]);
        free(pixmap->green[i]);
    }
    free(pixmap->red);
    free(pixmap->green);
    free(pixmap->blue);
    free(pixmap);
    return imagine;
}

void leaf_search(pixels *pixmap, QuadtreeNode **imagine, int i, int j, int k, int size)
{
    QuadtreeNode *pls;
    pls = imagine[i];
    if(imagine[i] == NULL)
        return;
    if(imagine[i]->top_left == -1) {
        for (int l = j; l <j + size; l++)
            for (int m = k; m <k + size; m++) {
                pixmap->red[l][m] = imagine[i]->red;
                pixmap->green[l][m] = imagine[i]->green;
                pixmap->blue[l][m] = imagine[i]->blue;
            }
    }
    else
    {
        leaf_search(pixmap, imagine, imagine[i]->top_left,j,k,size/2);
        leaf_search(pixmap, imagine, imagine[i]->top_right,j,k+(int)sqrt(imagine[i]->area)/2,size/2);
        leaf_search(pixmap, imagine, imagine[i]->bottom_right,j+(int)sqrt(imagine[i]->area)/2,k+(int)sqrt(imagine[i]->area)/2,size/2);
        leaf_search(pixmap, imagine, imagine[i]->bottom_left,j+(int)sqrt(imagine[i]->area)/2,k,size/2);
    }
}

void decompress(char* nume_int,char* nume_ies)
{
    pixels *pixmap;
    FILE *binar;
    char him;
    binar = fopen(nume_int,"rb+");
    int i,j,leafcount, nodecount;
    fread(&leafcount    ,sizeof(int),1,binar);
    fread(&nodecount,sizeof(int),1,binar);
    QuadtreeNode **imagine;
    imagine = malloc(nodecount*sizeof(QuadtreeNode*));
    for(i = 0; i < nodecount;i++)
        imagine[i] = malloc(sizeof(QuadtreeNode));
    fread(imagine[0],sizeof(QuadtreeNode),1,binar);
    pixmap = malloc(sizeof(pixels));
    pixmap->red = malloc((int)sqrt(imagine[0]->area) * sizeof(unsigned char *));
    pixmap->green = malloc((int)sqrt(imagine[0]->area) * sizeof(unsigned char *));
    pixmap->blue = malloc((int)sqrt(imagine[0]->area) * sizeof(unsigned char *));
    for(i = 0; i < (int)sqrt(imagine[0]->area); i++)
    {
        pixmap->red[i] = malloc((int)sqrt(imagine[0]->area) * sizeof(unsigned char));
        pixmap->green[i] = malloc((int)sqrt(imagine[0]->area) * sizeof(unsigned char));
        pixmap->blue[i] = malloc((int)sqrt(imagine[0]->area) * sizeof(unsigned char));
    }
    for(i = 1; i < nodecount; i++) {
        fread(imagine[i],sizeof(QuadtreeNode),1,binar);
    }
    FILE *rez;
    rez = fopen(nume_ies,"wb");
    fprintf(rez,"P6\n");
    fprintf(rez,"%d %d\n",(int)sqrt(imagine[0]->area),(int)sqrt(imagine[0]->area));
    fprintf(rez,"255\n");
    leaf_search(pixmap,imagine,0,0,0,(int)sqrt(imagine[0]->area));
    for(i = 0; i < (int)sqrt(imagine[0]->area); i++)
        for(j = 0; j < (int)sqrt(imagine[0]->area); j++)
        {
            fwrite(&pixmap->red[i][j], sizeof(char),1,rez);
            fwrite(&pixmap->green[i][j], sizeof(char),1,rez);
            fwrite(&pixmap->blue[i][j], sizeof(char),1,rez);
        }
    fclose(rez);
    fclose(binar);
    for(i = 0; i < (int)sqrt(imagine[0]->area); i++){
        free(pixmap->blue[i]);
        free(pixmap->red[i]);
        free(pixmap->green[i]);
    }
    free(pixmap->red);
    free(pixmap->green);
    free(pixmap->blue);
    free(pixmap);
    for(int i = 0; i < nodecount; i++) {
        free(imagine[i]);
    }
    free(imagine);
}

void write_binary(QuadtreeNode **imagine, int len, int leaf_count, char*nume)
{
    FILE *binar;
    int a;
    binar = fopen(nume,"wb+");
    fwrite(&leaf_count,sizeof(int),1,binar);
    fwrite(&len, sizeof(int), 1, binar);
    for(int i = 0; i < len; i++)
        fwrite(imagine[i], sizeof(QuadtreeNode), 1, binar);
    fclose(binar);
}

void mirror(QuadtreeNode **imagine, char tip, int len)
{
    int copie;
    if(tip == 'h')
    {
        for(int i = 0; i < len; i++)
        {
            copie = imagine[i]->top_right;
            imagine[i]->top_right = imagine[i]->top_left;
            imagine[i]->top_left = copie;
            copie = imagine[i]->bottom_left;
            imagine[i]->bottom_left = imagine[i]->bottom_right;
            imagine[i]->bottom_right = copie;
        }
    }
    else
        for(int i = 0; i < len; i++)
        {
            copie = imagine[i]->top_right;
            imagine[i]->top_right = imagine[i]->bottom_right;
            imagine[i]->bottom_right = copie;
            copie = imagine[i]->bottom_left;
            imagine[i]->bottom_left = imagine[i]->top_left;
            imagine[i]->top_left = copie;
        }
}

int main(int argc, char **argv) {
    FILE *ppm;
    int len = 0;
    int leaf_number = 0;
    QuadtreeNode **imagine;
    if(strcmp(argv[1],"-c")==0)
    {
        ppm = fopen(argv[3],"rb");
        imagine = create_tree(&ppm, atoi(argv[2]), &len, &leaf_number);
        write_binary(imagine, len, leaf_number, argv[4]);
        for(int i = 0; i < len; i++) {
            free(imagine[i]);
        }
        free(imagine);
        fclose(ppm);
        return 0;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        ppm = fopen(argv[2],"rb");
        decompress(argv[2], argv[3]);
        fclose(ppm);
        return 0;
    }
    else if(strcmp(argv[1],"-m")==0)
    {
        ppm = fopen(argv[4],"rb");
        imagine = create_tree(&ppm, atoi(argv[3]), &len, &leaf_number);
        mirror(imagine, argv[2][0], len);
        write_binary(imagine, len, leaf_number, "random.bin");
        decompress("random.bin", argv[5]);
        remove("random.bin");
        for(int i = 0; i < len; i++) {
            free(imagine[i]);
        }
        free(imagine);
        fclose(ppm);
        return 0;
    }
    return 0;
}