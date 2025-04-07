#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    slip,
    land,
    trailor,
    storage,
    no_place
} PlaceType;

typedef struct {
    char name[128];      
    int length;          
    PlaceType type;      
    union {
        int slipNumber;           
        char bayLetter;           
        char trailerLicense[8];   
        int storageSpace;         
    } location;
    float amountOwed;    
} Boat;


PlaceType StringToPlaceType(char *PlaceString) {
    if (!strcasecmp(PlaceString, "slip"))
        return slip;
    if (!strcasecmp(PlaceString, "land"))
        return land;
    if (!strcasecmp(PlaceString, "trailor"))
        return trailor;
    if (!strcasecmp(PlaceString, "storage"))
        return storage;
    return no_place;
}


char *PlaceToString(PlaceType Place) {
    switch (Place) {
        case slip:
            return "slip";
        case land:
            return "land";
        case trailor:
            return "trailor";
        case storage:
            return "storage";
        case no_place:
            return "no_place";
        default:
            printf("How the faaark did I get here?\n");
            exit(EXIT_FAILURE);
    }
}


int loadBoats(const char *filename, Boat boats[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return 0;
    }
    
    int numBoats = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) && numBoats < 120) {
        line[strcspn(line, "\r\n")] = '\0';
        
        char *token = strtok(line, ",");
        if (!token) continue;
        strcpy(boats[numBoats].name, token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        boats[numBoats].length = atoi(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        if (strcmp(token, "slip") == 0) {
            boats[numBoats].type = slip;
            token = strtok(NULL, ",");
            boats[numBoats].location.slipNumber = atoi(token);
        } else if (strcmp(token, "land") == 0) {
            boats[numBoats].type = land;
            token = strtok(NULL, ",");
            boats[numBoats].location.bayLetter = token[0];
        } else if (strcmp(token, "trailor") == 0) {
            boats[numBoats].type = trailor;
            token = strtok(NULL, ",");
            strcpy(boats[numBoats].location.trailerLicense, token);
        } else if (strcmp(token, "storage") == 0) {
            boats[numBoats].type = storage;
            token = strtok(NULL, ",");
            boats[numBoats].location.storageSpace = atoi(token);
        } else {
            boats[numBoats].type = no_place;
        }
        
        token = strtok(NULL, ",");
        if (!token) continue;
        boats[numBoats].amountOwed = atof(token);
        
        numBoats++;
    }
    fclose(file);
    return numBoats;
}

void displayInventory(Boat boats[], int numBoats) {
    printf("\n%-20s %-10s %-10s %-8s %10s\n", "Name", "Length", "Type", "Place", "Owes");
    for (int i = 0; i < numBoats; i++) {
        switch (boats[i].type) {
            case slip:
                printf("%-20s %-10d %-10s # %-5d Owes $%8.2f\n",
                       boats[i].name, boats[i].length, PlaceToString(boats[i].type),
                       boats[i].location.slipNumber, boats[i].amountOwed);
                break;
            case land:
                printf("%-20s %-10d %-10s %-8c Owes $%8.2f\n",
                       boats[i].name, boats[i].length, PlaceToString(boats[i].type),
                       boats[i].location.bayLetter, boats[i].amountOwed);
                break;
            case trailor:
                printf("%-20s %-10d %-10s %-8s Owes $%8.2f\n",
                       boats[i].name, boats[i].length, PlaceToString(boats[i].type),
                       boats[i].location.trailerLicense, boats[i].amountOwed);
                break;
            case storage:
                printf("%-20s %-10d %-10s # %-5d Owes $%8.2f\n",
                       boats[i].name, boats[i].length, PlaceToString(boats[i].type),
                       boats[i].location.storageSpace, boats[i].amountOwed);
                break;
            default:
                break;
        }
    }
}

void saveBoats(const char *filename, Boat boats[], int numBoats) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file for writing.\n");
        exit(1);
    }
    
    
    for (int i = 0; i < numBoats; i++) {
        switch (boats[i].type) {
            case slip:
                fprintf(file, "%s,%d,%s,%d,%.2f\n",
                        boats[i].name, boats[i].length, PlaceToString(boats[i].type),
                        boats[i].location.slipNumber, boats[i].amountOwed);
                break;
            case land:
                fprintf(file, "%s,%d,%s,%c,%.2f\n",
                        boats[i].name, boats[i].length, PlaceToString(boats[i].type),
                        boats[i].location.bayLetter, boats[i].amountOwed);
                break;
            case trailor:
                fprintf(file, "%s,%d,%s,%s,%.2f\n",
                        boats[i].name, boats[i].length, PlaceToString(boats[i].type),
                        boats[i].location.trailerLicense, boats[i].amountOwed);
                break;
            case storage:
                fprintf(file, "%s,%d,%s,%d,%.2f\n",
                        boats[i].name, boats[i].length, PlaceToString(boats[i].type),
                        boats[i].location.storageSpace, boats[i].amountOwed);
                break;
            default:
                break;
        }
    }
    fclose(file);
}

int compareBoats(const void *a, const void *b) {
    const Boat *boatA = (const Boat *)a;
    const Boat *boatB = (const Boat *)b;
    return strcasecmp(boatA->name, boatB->name);
}

void addBoat(char *boatStr, Boat boats[], int *numBoats) {
    if (*numBoats >= 120) {
        printf("Marina is full, cannot add more boats.\n");
        return;
    }
    
    char *token = strtok(boatStr, ",");
    if (!token) return;
    strcpy(boats[*numBoats].name, token);
    
    token = strtok(NULL, ",");
    if (!token) return;
    boats[*numBoats].length = atoi(token);
    
    token = strtok(NULL, ",");
    if (!token) return;
    if (strcmp(token, "slip") == 0) {
        boats[*numBoats].type = slip;
        token = strtok(NULL, ",");
        boats[*numBoats].location.slipNumber = atoi(token);
    } else if (strcmp(token, "land") == 0) {
        boats[*numBoats].type = land;
        token = strtok(NULL, ",");
        boats[*numBoats].location.bayLetter = token[0];
    } else if (strcmp(token, "trailor") == 0) {
        boats[*numBoats].type = trailor;
        token = strtok(NULL, ",");
        strcpy(boats[*numBoats].location.trailerLicense, token);
    } else if (strcmp(token, "storage") == 0) {
        boats[*numBoats].type = storage;
        token = strtok(NULL, ",");
        boats[*numBoats].location.storageSpace = atoi(token);
    } else {
        boats[*numBoats].type = no_place;
    }
    
    token = strtok(NULL, ",");
    if (!token) return;
    boats[*numBoats].amountOwed = atof(token);
    
    (*numBoats)++;
}

void removeBoat(Boat boats[], int *numBoats) {
    char boatName[128];
    int found = 0;
    printf("Enter the name of the boat to remove: ");
    fgets(boatName, sizeof(boatName), stdin);
    boatName[strcspn(boatName, "\n")] = '\0';
    
    for (int i = 0; i < *numBoats; i++) {
        if (strcasecmp(boats[i].name, boatName) == 0) {
            found = 1;
            for (int j = i; j < *numBoats - 1; j++) {
                boats[j] = boats[j + 1];
            }
            (*numBoats)--;
            break;
        }
    }
    if (!found) {
        printf("No boat with that name.\n");
    }
}


void acceptPayment(Boat boats[], int numBoats) {
    char boatName[128];
    double paymentAmount;
    int found = 0;
    
    printf("Enter the name of the boat to make a payment on: ");
    fgets(boatName, sizeof(boatName), stdin);
    boatName[strcspn(boatName, "\n")] = '\0';
    
    for (int i = 0; i < numBoats; i++) {
        if (strcasecmp(boats[i].name, boatName) == 0) {
            found = 1;
            printf("Enter the amount to be paid: ");
            scanf("%lf", &paymentAmount);
            getchar(); // Consume the newline
            if (paymentAmount > boats[i].amountOwed) {
                printf("That is more than the amount owed, $%.2f\n", boats[i].amountOwed);
                return;
            }
            boats[i].amountOwed -= paymentAmount;
            break;
        }
    }
    if (!found) {
        printf("No boat with that name.\n");
    }
}

void updatePrices(Boat boats[], int numBoats) {
    for (int i = 0; i < numBoats; i++) {
        switch (boats[i].type) {
            case slip:
                boats[i].amountOwed += 12.50 * boats[i].length;
                break;
            case land:
                boats[i].amountOwed += 14.00 * boats[i].length;
                break;
            case trailor:
                boats[i].amountOwed += 25.00 * boats[i].length;
                break;
            case storage:
                boats[i].amountOwed += 11.20 * boats[i].length;
                break;
            default:
                break;
        }
    }
}

void printMenu() {
    printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
}

int main(void) {
    int numBoats;
    char CSVfilename[] = "BoatData.csv";
    Boat boats[120]; 
    char option;
    char inputString[256];

    numBoats = loadBoats(CSVfilename, boats);
    qsort(boats, numBoats, sizeof(Boat), compareBoats);

    printMenu();
    fgets(inputString, sizeof(inputString), stdin);
    option = tolower(inputString[0]);
    
    while (1) {
        switch (option) {
            case 'i':
                displayInventory(boats, numBoats);
                break;
            case 'a':
                printf("Please enter the boat data in CSV format: ");
                fgets(inputString, sizeof(inputString), stdin);
                addBoat(inputString, boats, &numBoats);
                qsort(boats, numBoats, sizeof(Boat), compareBoats);
                break;
            case 'r':
                removeBoat(boats, &numBoats);
                break;
            case 'p':
                acceptPayment(boats, numBoats);
                break;
            case 'm':
                updatePrices(boats, numBoats);
                break;
            case 'x':
                saveBoats(CSVfilename, boats, numBoats);
                printf("Exiting the Boat Management System\n");
                return EXIT_SUCCESS;
            default:
                printf("Invalid option %c\n", option);
                break;
        }
        printMenu();
        fgets(inputString, sizeof(inputString), stdin);
        option = tolower(inputString[0]);
    }
    
    return EXIT_SUCCESS;
}

