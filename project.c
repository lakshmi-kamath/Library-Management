#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Book 
{
    int id;
    char name[100];
    char author[100];
    int available;
    int isBorrowed;
    struct Book* next;
};


struct Student 
{
    char name[100];
    char srn[20];
    int borrowedBooks;
    struct Book* borrowedBooksList;
    struct Student* next;
};




// Function to create a new student node
struct Student* createStudent(char name[], char srn[]) 
{
    struct Student* newStudent = (struct Student*)malloc(sizeof(struct Student));
    strcpy(newStudent->name, name);
    strcpy(newStudent->srn, srn);
    newStudent->borrowedBooks = 0;
    newStudent->borrowedBooksList = NULL;
    newStudent->next = NULL;
    return newStudent;
}




// Function to create a new book node
struct Book* createBook(int id, char name[], char author[], int available) {
    struct Book* newBook = (struct Book*)malloc(sizeof(struct Book));
    newBook->id = id;
    strcpy(newBook->name, name);
    strcpy(newBook->author, author);
    newBook->available = available;
    newBook->isBorrowed = 0; // Initialize isBorrowed to 0
    newBook->next = NULL;
    return newBook;
}





// Function to borrow a book
void borrowBook(struct Student* student, struct Book* book) {
    // Check if the student can borrow the book and if the book is available for borrowing
    if (student->borrowedBooks < 3 && book->available > 0 && book->isBorrowed == 0) {
        book->available--;
        book->isBorrowed = 1; // Mark the book as borrowed
        student->borrowedBooks++; //Increment number of books borrowed by student

        // Add the book to the student's borrowed books list
        struct Book* borrowedBook = createBook(book->id, book->name, book->author, 1);
        
        if(student->borrowedBooksList==NULL)
        {
            student->borrowedBooksList=borrowedBook;
        }
        else{
            struct Book* b=student->borrowedBooksList;
            while(b->next!=NULL)
            {
                b=b->next;
            }
            b->next=borrowedBook;
            }
            printf("\nBook successfully borrowed by %s (SRN: %s)\n", student->name, student->srn);
    }
    else if (student->borrowedBooks >= 3) 
        {
            printf("\nSorry, you have already borrowed 3 books.\n");
        }
    else if (book->isBorrowed == 1) 
        {
            printf("\nBook is not available for borrowing as it has already been borrowed.\n");
        }
    else 
        {
            printf("\nBook is not available for borrowing.\n");
        }
}



void returnBook(struct Student* student, int bookId, struct Book** books) {
    struct Book* current = student->borrowedBooksList;
    struct Book* prev = NULL;
    int found = 0;

    // Find the book in the student's borrowed books list
    while (current != NULL) {
        if (current->id == bookId) {
            found = 1;
            break;
        }
        prev = current;
        current = current->next;
    }

    // If the book is not found in the borrowed books list
    if (!found) {
        printf("You have not borrowed a book with ID %d.\n", bookId);
    } 
    else {
        // Increment the available copies of the returned book in the original books list
        struct Book* originalBook = *books;
        while (originalBook != NULL) {
            //if the book id matched with the book id in the linked list increment availability
            if (originalBook->id == bookId) {
                originalBook->available++;
                originalBook->isBorrowed = 0; // Mark the book as not borrowed
                break;
            }
            originalBook = originalBook->next;
        }

        // Remove the returned book from the student's borrowed books list
        if (prev == NULL) {
            student->borrowedBooksList = current->next;
            free(current);
        } else {
            prev->next = current->next;
            free(current);
        }

        // Allow the student to borrow more books by decrementing the borrowed count
        student->borrowedBooks--;

        printf("Book with ID %d returned by %s (SRN: %s)\n", bookId, student->name, student->srn);
    }
}




void displayAvailableBooks(struct Book* books) {
    printf("Available Books:\n");
    struct Book* current = books;
    int count = 1;

    while (current != NULL) {
        if (current->isBorrowed == 0) {
            printf("ID: %d, Name: %s, Author: %s, Available Copies: %d\n",current->id, current->name, current->author, current->available);
            count++;
        }
        current = current->next;
    }

    if (count == 1) {
        printf("No available books.\n");
    }
}





void readBooksFromFile(struct Book** books) {
    FILE* file = fopen("books.txt", "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    int id, available;
    char name[100], author[100];

    while (fscanf(file, "%d %s %s %d", &id, name, author, &available) == 4) {
        struct Book* newBook = createBook(id, name, author, available);
        if (*books == NULL) {
            *books = newBook;
        } else {
            struct Book* temp = *books;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newBook;
        }
    }

    fclose(file);
}





void readStudentsFromFile(struct Student** students) {
    FILE* file = fopen("students.txt", "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    char name[100], srn[20];
    while (fscanf(file, "%s %s", name, srn) == 2) {
        // Create a new student node and add it to the linked list
        struct Student* newStudent = createStudent(name, srn);
        if (*students == NULL) {
        *students = newStudent;
    } else {
        struct Student* temp = *students;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newStudent;
    }
    }

    fclose(file);
}





void truncateBooksFile(struct Book* books) {
    FILE* file = fopen("books.txt", "r");
    FILE* tempFile = fopen("temp.txt", "w");
    
    if (file == NULL || tempFile == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    int id, available;
    char name[100], author[100];
    int count = 0;

    // Copy the first 50 books from the original file to the temporary file
    while (count < 50 && fscanf(file, "%d %s %s %d", &id, name, author, &available) == 4) {
        fprintf(tempFile, "%d %s %s %d\n", id, name, author, available);
        count++;
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original books.txt file with the temporary file
    remove("books.txt");
    rename("temp.txt", "books.txt");
}




// Function to truncate students file, keeping only the initial 3 students
void truncateStudentsFile(struct Student* students) {
    FILE* file = fopen("students.txt", "w");
    if (file == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    int count = 0;
    struct Student* currentStudent = students;

    // Write the information of the first 3 students to the file
    while (currentStudent != NULL && count < 3) {
        fprintf(file, "%s %s\n", currentStudent->name, currentStudent->srn);
        currentStudent = currentStudent->next;
        count++;
    }

    fclose(file);
}




// Function to add a new student node to the linked list
void addStudent(struct Student** students, struct Student* newStudent) {
    if (*students == NULL) {
        *students = newStudent;
    } else {
        struct Student* temp = *students;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newStudent;
    }
}



struct Student* students = NULL;
struct Book* books = NULL;




int main() {
    int studentCount = 0;
    int bookCount = 0;
    int choice;
    int i = 1;
    readBooksFromFile(&books);
    readStudentsFromFile(&students);
    
   

    printf("\n\nWelcome to library management system !!");

    while (1) {
        printf("\n\n-------------------------------------------------------------------------------------------------------------");
        printf("\n1. Add new student\n2. Add new book\n3. Borrow a book\n4. Return a book\n5. Display available books\n6. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

case 1: {
    char name[100];
    char srn[20];
    printf("Enter student name: ");
    scanf("%s", name);
    printf("Enter student SRN: ");
    scanf("%s", srn);

    // Check if the student with the same SRN already exists
    struct Student* currentStudent = students;
    while (currentStudent != NULL) {
        if (strcmp(currentStudent->srn, srn) == 0) {
            printf("\nStudent with SRN %s is already in the list.\n", srn);
            break;
        }
        currentStudent = currentStudent->next;
    }

    // If the student is not in the list, add the new student
    if (currentStudent == NULL) {
        struct Student* newStudent = createStudent(name, srn);
        addStudent(&students, newStudent);

        // Append the new student to the students.txt file
        FILE* file = fopen("students.txt", "a");
        if (file == NULL) {
            printf("Error opening the file.\n");
            exit(1);
        }

        fprintf(file, "%s %s\n", name, srn);
        fclose(file);

        studentCount++;
        printf("\n\nStudent added successfully.\n");
    }
    break;
}

case 2: {
    int id;
    char name[100];
    char author[100];
    // Find the maximum book ID
    struct Book* tempBook = books;
    int maxBookId = 0;

    while (tempBook != NULL) {
        if (tempBook->id > maxBookId) {
            maxBookId = tempBook->id;
        }
        tempBook = tempBook->next;
    }

    id = maxBookId + 1; // Automatically allocate book ID as maxBookId + 1

    printf("Enter book name: ");
    scanf("%s", name);
    printf("Enter book author: ");
    scanf("%s", author);
    struct Book* newBook = createBook(id, name, author, 1);
    if (books==NULL)
             {
                books=newBook;
             }
             else
             {
                struct Book *temp=books;
                while(temp->next!=NULL)
                {
                    temp=temp->next;
                }
                temp->next=newBook;
             }
             
              FILE* file = fopen("books.txt", "a");
    if (file == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    fprintf(file, "\n%d %s %s %d\n", id, name, author, 1);
    fclose(file);
    bookCount++;
    printf("\nBook added successfully with ID: %d", id);
    break;
}



        case 3: {
                char srn[20];
                int bookId;
                printf("Enter student SRN: ");
                scanf("%s", srn);
                printf("Enter book ID: ");
                scanf("%d", &bookId);

                struct Student* currentStudent = students;
                // Find the student with the given SRN
                while (currentStudent != NULL && strcmp(currentStudent->srn, srn) != 0) {
                    currentStudent = currentStudent->next;
                }

                if (currentStudent == NULL) {
                    printf("Student with SRN %s not found.\n", srn);
                } else {
                    struct Book* currentBook = books;
                    while (currentBook != NULL && currentBook->id != bookId) {
                        currentBook = currentBook->next;
                    }

                    if (currentBook == NULL) {
                        printf("Invalid book ID.\n");
                    } else {
                        borrowBook(currentStudent, currentBook);
                    }
                }
                break;
            }

            case 4: {
                char srn[20];
                int bookId;
                printf("Enter student SRN: ");
                scanf("%s", srn);
                printf("Enter book ID: ");
                scanf("%d", &bookId);

                struct Student* currentStudent = students;
                // Find the student with the given SRN
                while (currentStudent != NULL && strcmp(currentStudent->srn, srn) != 0) {
                    currentStudent = currentStudent->next;
                }

                if (currentStudent == NULL) {
                    printf("Student with SRN %s not found.\n", srn);
                } else {
                    returnBook(currentStudent, bookId,&books);
                }
                break;
            }
            case 5:
                displayAvailableBooks(books);
                break;
            

            case 6:
                // Free allocated memory for students and books
               truncateStudentsFile(students);
                while (students != NULL) {
                    struct Student* temp = students;
                    students = students->next;
                    free(temp);
                }
                 
                    truncateBooksFile(books);
                while (books != NULL) {
                    struct Book* temp = books;
                    books = books->next;
                    free(temp);
                }
                return 0;

            

            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}