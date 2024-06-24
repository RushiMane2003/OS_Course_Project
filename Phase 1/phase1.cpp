#include<iostream>
#include<fstream>
using namespace std;

class OS{
    //declaring the variables -----------------------------------------------------------------------------------------------------------------------------------------
    char M[100][4];             // Memory (10 blocks (1 block = 10 words (1 word = 4 bytes))) total 100 words
    char IR[4];                 // Instruction register
    int IC;                     // Instruction counter
    char R[4];                  // Genral purpose registor
    bool C;                     // Toggle register
    int SI;                     // System interrupt 
    char buffer[40];            // Buffer of 1 block (10 words)
 
    //declaring functions -----------------------------------------------------------------------------------------------------------------------------------------
    public:
        void INIT();            //initializing functions to initialize variables to 0
        void MOS();             //Master Mode (managing operating system)
        void read();            //Read
        void write();           //Write
        void terminate();       //Terminate function
        void load();            //Load function
        void Execute();         //Execute function to start the execution of the instructions
        void clearBuffer();     //Clear buffer
        void printMemory();     //Print the memory content
        fstream infile;         //input file
        fstream outfile;        //output file        
};

void OS::INIT() // Defining INIT function of OS class
{
    for(int i = 0; i < 100; i++) //initializes the memory to \0
        for(int j = 0; j < 4; j++)
            M[i][j] = '\0'; 
    for(int i = 0; i < 4; i++) //initializes the Instruction register to \0
        IR[i] = '\0'; 
    for(int i = 0; i < 4; i++) //initializes the general purpose register to \0
        R[i] = '\0'; 
    C = false; //initializes toggle resistor to false
}

void OS :: clearBuffer()
{
    for(int i = 0; i < 40; i++)
        buffer[i] = '\0';
}

void OS :: printMemory()
{
    printf("\n\n\n");
    for(int i = 0; i<20; i++)
    { 
        printf("M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c",i,M[i][0],M[i][1],M[i][2],M[i][3],i+20,M[i+20][0],M[i+20][1],M[i+20][2],M[i+20][3],i+40,M[i+40][0],M[i+40][1],M[i+40][2],M[i+40][3],i+60,M[i+60][0],M[i+60][1],M[i+60][2],M[i+60][3],i+60,M[i+60][0],M[i+60][1],M[i+60][2],M[i+60][3]);
        cout<<endl;
    } 
}

void OS :: load() // Defining load function of OS class
{
    printf("\nReading a card\n");
    int ptr = 0; // points to word in memory (initially 0th word)

    do //while file does not end
    {
        clearBuffer();
        infile.getline(buffer,41); // copies 41 characters or a line (whichever is small) and pastes it in buffer (41 to adjust \0 at end of the string)
        for(int i=0; i<40; i++) // prints what it reads from input.txt
            cout<<buffer[i]; 
        printf("\n");

        if(buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J') // start of Control Card
            INIT(); // initialize
        else if(buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A') // start of Data Card
        {    
            IC = 00; // as first instruction is on M[0]
            Execute();
        } 
        else if(buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D') // end of Job now start once again
        {
            ptr = 0; // as new job starts so start from allocating memory from 0 
            continue;
        } 
        else // copy instructions in memory starting from M[0]
        {
            int buffptr = 0;
            for(;ptr < 100; ++ptr) // buffptr is buffer pointer and ptr is memory pointer
            {    
                for(int j = 0 ; j < 4; ++j,buffptr++)
                    M[ptr][j] = buffer[buffptr]; // copies instructions from buffer to memory
                if(buffptr >= 40 || buffer[buffptr] == '\n' || buffer[buffptr] == ' ') // breaks at end of the instructions or when the buffer overflows
                    break;
            }    
        }
    }while(!infile.eof());
}

void OS :: Execute() // Defining Execute function of OS class 
{
    while(1)
    {
        for(int i = 0; i < 4; i++)
            IR[i] = M[IC][i]; // copies instruction from memory into the Instruction register
        IC++; // incrementing instruction registor to point at next instruction

        if (IR[0] == 'G' && IR[1] == 'D') // Get Data input.txt----> Memory
            SI = 1, MOS();
        else if (IR[0] == 'P' && IR[1] == 'D') // Put Data Memory----> output.txt
            SI = 2, MOS();
        else if (IR[0] == 'H') // Halt
        {    
            SI = 3;
            MOS();
            break;
        }
        else if (IR[0] == 'L' && IR[1] == 'R') // Load Register (To copy data at given address to genral purpose registor)
        {
            int address = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
            for(int i = 0; i < 4; i++)
                R[i] = M[address][i];
        }
        else if (IR[0] == 'S' && IR[1] == 'R') // Store Register (To copy data from genral purpose registor to the given address)
        {
            int address = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
            for(int i = 0; i < 4; i++)
                M[address][i] = R[i];
        }
        else if (IR[0] == 'C' && IR[1] == 'R') // Compare Register
        {    
            int address = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
            int flag = 0;
            for(int i = 0; i < 4; i++) // compares data in register and memory 
                if(M[address][i] != R[i])
                {
                    flag = 1;
                    break; 
                }
            if(flag == 0) C = true;// sets Toggle register to true if the data matches otherwise it is intialized to false 
        }
        else if (IR[0] == 'B' && IR[1] == 'T') // Branch on Toggle
            if(C == true)
            {        
                int address = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
                IC = address; // sets the IC to given address
            }
    }
}

void OS :: MOS() // Defining MOS function of OS class          
{
    if(SI == 1) read(); //input.txt---> Memory
    else if(SI == 2) write(); //Memory---> output.txt
    else if(SI == 3) terminate();
}

void OS :: read() // Defining read function of OS class
{
    clearBuffer();
    infile.getline(buffer,41); //reads data from file 

    int buffptr = 0; // buffer pointer
    int address = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
    for(int b = 0; b < 10; b++)// copies data from buffer to a block by splitting it into words of 4 bytes
    {
        for (int i = 0; i < 4, buffptr < 40 ; i++,buffptr++) 
            M[address][i] = buffer[buffptr];
        address++;
    }
    printMemory(); 
}

void OS :: write() // Defining write function of OS class
{
    int address = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
    for (int i = 0; i < 10; i++) // write the whole block from the Memory to output.txt in a single line
    {
        for(int j = 0; j < 4; j++){
            if(M[address][j] != '\0') outfile<<M[address][j];
            else outfile<<' '; // to prevent 'NULNULNUL' in output file
        }
        address++;
    }
    outfile<<"\n"; //for writing next block on new line
    printMemory();
}

void OS :: terminate() // Defining terminate function of OS class
{
    outfile<<"\n";
    outfile<<"\n"; // leaves 2 lines at end of each job
}
int main()
{ 
    OS os;  //creating the instance of object of Os class
    os.infile.open("input.txt", ios::in);   //input file
    os.outfile.open("output.txt", ios::out);  //output file

    if(!os.infile) printf("Failure"); //if no input.txt file exists then exits with failure message
    else printf("Success"), os.load(); // if input.txt file exists then enters load function  
    return 0;
}