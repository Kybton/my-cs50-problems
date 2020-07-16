#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                j--;
                //return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    /*for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("preferences[%i][%i] is %i.\n", i, j, preferences[i][j]);
        }
    }*/

    add_pairs();

    /*for (int i = 0; i < pair_count; i++)
    {
        printf("pair %i. winner:%s. loser:%s.\n", i, candidates[pairs[i].winner], candidates[pairs[i].loser]);
    }

    for (int i = 0; i < pair_count; i++)
    {
        printf("%s beats %s with %i-%i.\n", candidates[pairs[i].winner], candidates[pairs[i].loser], preferences[pairs[i].winner][pairs[i].loser], preferences[pairs[i].loser][pairs[i].winner]);
    }

    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;*/
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    // return false when the candidate is not valid
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    int firstcan;
    int secondcan;

    for (int i = 0; i < candidate_count - 1; i++)
    {
        firstcan = ranks[i];
        
        for (int j = i + 1; j < candidate_count; j++)
        {
            secondcan = ranks[j];
            preferences[firstcan][secondcan]++;            
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int a = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[a].winner = i;
                pairs[a].loser = j;
                a++;            
            }
        }
    }
    pair_count = a;
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // setting up a tempory pair to save the primary pair values to it
    typedef struct
    {
        int winner;
        int loser;
    }
    temp_pair;

    temp_pair temp_pairs[0];
    
    for (int i = 0; i < pair_count - 1; i++)
    {
        // setting up strongest pair and it will be used to compare to other pairs
        int strongest_pair = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
        
        for (int j = i + 1; j < pair_count; j++)
        {
            int pair_diff;
            pair_diff = preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[j].loser][pairs[j].winner];

            if (strongest_pair < pair_diff)
            {
                strongest_pair =  preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[j].loser][pairs[j].winner];
                temp_pairs[0].winner = pairs[j].winner;
                temp_pairs[0].loser = pairs[j].loser;

                pairs[j].winner = pairs[i].winner;
                pairs[j].loser = pairs[i].loser;

                pairs[i].winner = temp_pairs[0].winner;
                pairs[i].loser = temp_pairs[0].loser;
            }
        }
    }

    //printf("after sorted\n");
    //printf("\n");

    /*for (int i = 0; i < pair_count; i++)
    {
        printf("%s beats %s with %i-%i.\n", candidates[pairs[i].winner], candidates[pairs[i].loser], preferences[pairs[i].winner][pairs[i].loser], preferences[pairs[i].loser][pairs[i].winner]);
    }
    return;*/
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        int locked_count = 0;

        //printf("***locked[%i][%i] has been locked.***\n", pairs[i].winner, pairs[i].loser);
        locked[pairs[i].winner][pairs[i].loser] = true;

        // this loop is just to check whether the last created lock creat a cycle or not
        for (int a = 0; a < candidate_count; a++)
        {
            for (int b = 0; b < candidate_count; b++)
            {
                //printf("checking locked[%i][%i] rn\n", b, a);
                if (locked[b][a] == true)
                {   
                    //printf("***one lock found on locked[%i][%i]***\n", b, a);
                    locked_count++;
                    b = candidate_count;
                }
            }
        }

        //printf("locked count = %i\n", locked_count);
        //printf("candidate count = %i\n", candidate_count);

        if (locked_count == candidate_count)
        {
            //printf("!!!locked[%i][%i] has been unlocked cus it created a cycle!!!\n", pairs[i].winner, pairs[i].loser);
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    bool winner;
    for (int i = 0; i < candidate_count; i++)
    {
        winner = true;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                j = candidate_count;
                winner = false;
            }
        }

        if (winner == true)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}

