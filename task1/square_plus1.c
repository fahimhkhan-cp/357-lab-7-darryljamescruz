// Task 1: Square Plus 1 (Pipes and Process Communication)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    /* Pipes: Parent -> Child1,
                            Child1 -> Child2,
                                        Child2 -> Parent */
    int p1[2], p2[2], p3[2];    
    
    // Create Pipes
    if (pipe(p1) == -1 || pipe(p2) == -1 || pipe(p3) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t child1 = fork();
    if (child1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0){
        // First Child: Squares the number
        close(p1[1]);   // Close write end of Parent -> Child 1
        close(p2[0]);   // Close read end of Child 1 -> Child 2

        int child1_num;
        while (read(p1[0], &child1_num, sizeof(child1_num)) > 0){
            int squared = child1_num * child1_num;
            write(p2[1], &squared, sizeof(squared));
        }

        close(p1[0]);
        close(p2[1]);
        exit(EXIT_SUCCESS);
    }

    pid_t child2 = fork();
    if (child2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (child2 == 0) {
        // Second Child: Adds 1 to the number
        close(p2[1]); // Close write end of Child1->Child2
        close(p3[0]); // Close read end of Child2->Parent
        
        int child2_num;
        while (read(p2[0], &child2_num, sizeof(child2_num)) > 0) {
            int incremented = child2_num + 1;
            write(p3[1], &incremented, sizeof(incremented));
        }

        close(p2[0]);   // Close read end of Child 1 -> Child 2
        close(p3[1]);   // Close write end of Child 2 -> Parent
        exit(EXIT_SUCCESS);
    }

    // Parent Process
    close(p1[0]);   // Close read end of Parent -> Child 1
    close(p2[1]);   // Close write end of Child 1 -> Child 2
    close(p3[1]);   // Close write end of Child 2 -> Parent

    int parent_num;
    while (1) {
        printf("Enter an integer (Ctrl+D to quit): ");
        fflush(stdout); // Ensure the prompt is displayed immediately

        if (scanf("%d", &parent_num) == EOF) {
            break; // Exit on EOF
        }

        write(p1[1], &parent_num, sizeof(parent_num)); // Send to first child
        read(p3[0], &parent_num, sizeof(parent_num));  // Read from second child
        printf("Result: %d\n", parent_num);
    }
    
    close(p1[1]);
    close(p3[0]);
    
    wait(NULL); // Wait for both children
    wait(NULL);
    return 0;
    
}