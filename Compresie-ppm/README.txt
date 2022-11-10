USAGE: Compress: ./quadtree -c [Compression factor (try 100)] input.ppm output.out 
       Decompress: ./quadtree -d output.out reconstructed_file.ppm
       Mirror: ./quadtree -m [v or h next for vertical or horizonal flip] [Compression factor (try 100)] input.ppm output.ppm 

Implementarea temei mi-a luat in jur de 12 ore.

In main, in functie de argumente, programul porneste pe una din 3 ramuri.
Pentru orice argument, deschid in variabila FILE ppm fisierul pentru citire
binara. In cazul compresiei, apelez apoi functia create_tree.

Functia create_tree citeste si salveaza din fisierul primit ca argument doar
inaltimea, restul valorilor sunt cunoscute, deci se citesc dar nu sunt salvate.
Culorile pixelilor vor fi salvate intr-o variabila de tip pixels, o structura
cu 3 matrici de char-uri ce vor stoca hex code-urile din imagine. Alocam spatiu
pentru aceasta variabila, fiecare matrice are aceeasi dimensiune ca imaginea,
inaltime*latime. Dupa alocare, parcurgem caracter cu caracter fisierul de 
intrare si inseram valorile hex code in structura. Apoi, initializam vectorul
de QuadTreeNode, imagine. Citim separat root-ul arborelui, cu functia cul_med.

cul_med va cauta intr-o structura pixels un patrat de latura size de hex-uri,
si va face mediile hex-urilor din camp, dar nu media totala, ci doar media 
matricelor red, green, blue. In acelasi timp, aloca un QuadTreeNode
completeaza campurile red, green, blue din acesta cu rezultatele. Pentru a 
cauta pixeli in patratul corect din imaginea mare, functia primeste ca argument
coordonatele coltului de sus-stanga al patratului. Functia returneaza nodul
completat.

Dupa ce am initializat root-ul, apelam fill_vector pentru a efectua construi
arborele. Salvam in len lungimea vectorului. 

Functia fill_vector verifica daca media culorilor din nodul de pe pozitia 
i din vector, unde i este primit ca argument, este mai mare decat pragul.
Functia med_nod cauta in imaginea mare, in acelasi mod ca functia cul_med,
patratul cu coltul in coordonatele date ca argument, dar calculeaza 
media tuturor hex-urilor din patrat cu formula pentru mean data.
Daca mean-ul calculat este mai mare decat pragul, divizam patratul
in 4 parti, care sunt incarcate in vector si indexate in nodul parinte.
Copiii nodului divizat sunt mereu plasati la finalul vectorului, accesarea
se va face prin intermediul parintelui deci nu trebuie respectata o ordine
anume. Fiecare nod copil este initializat cu ajutorul cul_med, iar vectorul
de noduri este realocat si marit. La final, functia este apelata fill_vector
este apelata recursiv pentru toti copiii, acestia avand latura injumatatita
fata de parinte, si coordonate diferite in functie de pozitia in patrat.
Un nod top-right va avea coltul pe aceeasi linie ca parintele, dar coloana
va fi la jumatea patratului parinte. Daca un nod nu trebuie divizat,
el va fi frunza, indicii vor fi setati ca -1 pentru identificare si numarul
de frunze creste. Functia se executa repetat pana nu mai exista nod ce 
necesita divizare. Aceasta returneaza un vector de QuadTreeNode.

Acum vectorul este construit, si binarul trebuie scris. Functia write_binary
deschide un fisier cu numele dat ca argument la inceput pentru scriere
binara, si il completeaza cu numarul de frunze, numarul de noduri si
vectorul in sine.

Pentru decomprimare, nu mai apelam create_tree, ci decompress.
Functia deschide fiserul binar dat ca argument pentru citire binara,
si citeste numarul de frunze si numarul de noduri. Se citeste primul
element din vector pentru a afla marimea imaginii, si apoi se aloca
o variabila de tip pixels, exact ca in create_tree, unde vom stoca imaginea
decomprimata. Alocam spatiu pentru a stoca vectorul din fisier, scriem
datele de pe primele 3 linii din ppm binar in fisierul de iesire dat ca
argument, si apelam functia leaf_search pentru a gasi toate frunzele din
arborele din vector.

Functia leaf_search verifica daca nodul de pe pozitia i este frunza, cu 
ajutorul index-ului. Daca un index este -1, atunci nodul este frunza.
Daca este frunza, functia actioneaza invers fata de cul_med, punand culorile
continute in nod pe pozitiile corespunzatoare din variabila pixmap pentru
a reconstrui imaginea. Daca nodul nu este frunza, functia este apelata 
cu indexul parintelui recursiv pentru toti cei patru fii, pana se gasesc 
toate frunzele.

Dupa ce pixmap-ul a fost completat, este scris dupa campurile red, green, blue 
in fisierul decomprimat si atat memoriile sunt eliberate.

Pentru mirror, se combina procedeele de mai sus. Este apelat create_tree in 
acelasi mod, dar inainte de a scrie un binar se aplica functia mirror, ce 
parcurge vectorul de noduri si modifica ordinea indexilor in fiecare nod
pentru a obtine rezultatul dorit. Dupa modificare, este folosit un binar
temporar random.bin, ce e preluat de decompress pentru a obtine imaginea 
finala.
