/* C implementation QuickSort */
#include<stdio.h>

void swap(int* a, int* b) {
    /* H sunarthsh auth allazei tis thieuthunseis twn 2 deiktwn*/

    int t = *a;
    *a = *b;
    *b = t;
}

int partition (int arr[], int low, int high)  {
    /* Auth h sunarthsh pairnei san pivot to teleutaio stoixeio tou pinaka,
       to bazei sthn swsth thesh kai topothtetei ola ta megalutera stoixeia
       aristera tou kai ola ta mikrotera deksia tou*/

    int pivot = arr[high];    // pivot
    int i = (low - 1);
 
    for (int j = low; j <= high- 1; j++) {


        if (arr[j] > pivot) {
            /* Oso to stoixeio einai megalutero tou pivot allakse ta theseis
               kai aukshse twn deikth gia ta megalutera stoixeia tou pinaka*/
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksort(int a[], int left, int right) {
    /* H sunarthsh auth prokeitai gia mia parallagh ths aplhs quicksort
       sthn opoia kaloume anadromika thn quicksort gia to mikrotero "kommati"
       tou pinaka kai xrhsimopoioume to loop gia na upologisoume to allo miso.
       Etsi panta douleuei me ton mikrotero dunato upopinaka. Auto ginetai giati
       me auton ton tropo o algorithmos kanei sxedon tis mises klhseis apo oti
       mia kanonikh quicksort kai sunepws einai pio grhgoros. */

    int newPivotIdx;

    while (right > left) {

        newPivotIdx = partition(a, left, right);

        if (newPivotIdx - left > right - newPivotIdx) {

            quicksort(a, newPivotIdx+1, right);

            right = newPivotIdx - 1;

        }
        else {

            quicksort(a, left, newPivotIdx-1);

            left = newPivotIdx + 1;

        }
    }
}

void quicksortSmallestK(int a[], int left, int right, int k) {
    /* Auth h sunarthsh prokeitai gia to olo "trick" me to opoio pisteuw
       lunetai arketa grhgora kai apodotika to problhma tou sort + topk.
       Genika basistika sthn idea oti an thes na breis to k stoixeio ston
       pinaka ousiastika ksereis kai se poio kommati tou pinaka tha anhkei. */

    int newPivotIdx;

    while (right > left) {

        newPivotIdx = partition(a, left, right);

        if (k <= newPivotIdx) {

            right = newPivotIdx - 1;

        }
        else if (newPivotIdx - left > right - newPivotIdx) {

            quicksort(a, newPivotIdx+1, right);

            right = newPivotIdx - 1;

        }
        else {

            quicksort(a, left, newPivotIdx-1);

            left = newPivotIdx + 1;

        }
    }
}


/* Function to print an array */
void printArray(int arr[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", arr[i]);
    //printf("n");
}
 
// Driver program to test above functions
int main()
{
    int arr[] = {16, 16, 16, 16, 16, 12, 1, 5, 9, 13, 14, 3, 2, 7, 19};
    int n = sizeof(arr)/sizeof(arr[0]);
    quicksortSmallestK(arr, 0, n-1, 15);
    printf("Sorted array: ");
    printArray(arr, n);
    return 0;
}
