#include<iostream>
#include<fstream>
#include <ctime>
using namespace std;

struct PCB{
    int JobID = 0; // Job ID
    int TTL = 0; // Total Time Limit
    int TLL = 0; // Total Line Limit
    int TTC = 0; // Total Time Count
    int LLC = 0; // Line Limit Count
}pcb;

class OS{
    //declaring the variables -----------------------------------------------------------------------------------------------------------------------------------------
    char M[300][4];             // Memory (30 blocks (1 block = 10 words (1 word = 4 bytes))) total 300 words
    char IR[4];                 // Instruction register
    int IC;                     // Instruction counter
    char R[4];                  // Genral purpose registor
    bool C;                     // Toggle register
    int SI;                     // System interrupt 
    char buffer[40];            // Buffer of 1 block (10 words)
    int VA;                     // Virtual Address 
    int RA;                     // Real Address  
    int PI;                     // Program Interrupt  
    int TI;                     // Timer Interrupt
    int DataCardCount;          // Number of Data Card
    int NoOfGD;                 // Number of GD instructions 
    int flag[30];               // To keep track of occupied blocks in memory           
    int PTR;                    // Pointer to page table

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
        int Add_map(int VA);    //Maps Real Address and Virtual Address
        int allocate();         //Allocates a block to a Process
        void check();           //Checks if everthing is in control
        void line(string error);//Prints the error in output.txt

        fstream infile;         //input file
        fstream outfile;        //output file        
};

void OS :: line(string error) { // outfile the error lines in given format 
    outfile << "IC : " << IC << "\t";
    outfile << "IR : " << IR << "\t";
    outfile << "TTC : " << pcb.TTC << "\t";
    outfile << "TTL : " << pcb.TTL << "\t";
    outfile << "LLC : " << pcb.LLC << "\t";
    outfile << "TLL : " << pcb.TLL << "\n";
    outfile << "Program terminated abnormally because of "<<error<<" error"<<endl;
}

int OS :: Add_map(int VA){
    int frame = VA / 10; // identifies in which frame the page is located
    int offset = VA % 10; // identifies on which line of that page the data is present
    int address = 0;
    if(M[PTR + frame][0]=='*'){ // if frame is not filled a new block is allocated and add it to page table
        cout << "Page Fault Occured" << endl;
        int block = allocate();
        M[PTR + frame][0] = block / 10 + '0'; // store tens digit in page table
        M[PTR + frame][1] = block % 10 + '0'; // store units digit in page table 
        M[PTR + frame][2] = '\0'; // clear remaining parts in page table
        M[PTR + frame][3] = '\0'; // clear remaining parts in page table
        return block; // returns address of block
    }
    else{ 
        address = (M[PTR + frame][0] - '0')*100 + (M[PTR + frame][1] - '0')*10 + offset; // converts the address at page table to real address 
        return address; 
    }
}

void OS :: clearBuffer(){ // resets the buffer to \0
    for(int i = 0; i < 40; i++)
        buffer[i] = '\0';
}

void OS :: printMemory(){
    printf("\n\n");
    for(int i = 0; i<50; i++){ 
        printf("M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c\t| M[%d]\t%c%c%c%c", i,M[i][0],M[i][1],M[i][2],M[i][3],i+50,M[i+50][0],M[i+50][1],M[i+50][2],M[i+50][3],i+100,M[i+100][0],M[i+100][1],M[i+100][2],M[i+100][3],i+150,M[i+150][0],M[i+150][1],M[i+150][2],M[i+150][3],i+200,M[i+200][0],M[i+200][1],M[i+200][2],M[i+200][3],i+250,M[i+250][0],M[i+250][1],M[i+250][2],M[i+250][3]);
        cout<<endl;
    } 
}

int OS::allocate() {
    int block;
    do {
        block = rand() % 30; //selects a random block between 0 to 29
    } while (flag[block] == 1); // runs untill a empty block is found
    flag[block] = 1; //marks the block allocated 
    return block; // returns the block
}

void OS :: check(){ // checks if everthing is in control
    if (pcb.TTC >= pcb.TTL) TI = 2, MOS();
    if (pcb.LLC > pcb.TLL) PI = 2, MOS();
    if (SI == 0 && PI == 0 && TI == 0) return;
    if(DataCardCount < NoOfGD) line("Out of Data"), terminate(), exit(0);
}

void OS::INIT(){ // Defining INIT function of OS class
    for(int i = 0; i < 300; i++) //initializes the memory to \0
        for(int j = 0; j < 4; j++)
            M[i][j] = '\0'; 
    for(int i = 0; i < 4; i++) IR[i] = '\0'; //initializes the Instruction register to \0
    for(int i = 0; i < 4; i++) R[i] = '\0'; //initializes the general purpose register to \0
    C = false; //initializes toggle resistor to false
    for(int i = 0; i < 30; i++) flag[i] = 0; // intializes flag pointer to 0;
    PTR = allocate()*10 ; // selecting a random block and make it page table
    for(int i = PTR; i < PTR + 10; i++)
        for(int j = 0; j < 4; j++)
            M[i][j] = '*'; // to denote page table block
    
    DataCardCount = 0; 
    SI = 0; // initializes system interrupt to 0
    TI = 0; // initializes timer interrupt to 0
    PI = 0; // initializes program interrupt to 0
    pcb.TTC = 0; // initialize time count to 0
    pcb.LLC = 0; // initialize line count to 0
    pcb.TTL = 0; // initialize time limit to 0
    pcb.TLL = 0; // initialize line limit to 0
    NoOfGD = 0; // initializes
}

void OS :: load(){ // Defining load function of OS class
    int ptr;

    do{ //while file does not end
        clearBuffer();
        infile.getline(buffer,41); // copies 41 characters or a line (whichever is small) and pastes it in buffer (41 to adjust \0 at end of the string)
        if(buffer[0] =='\0' || buffer[0] == '\n') continue; // if blank line(s) is/are incountered
        for(int i=0; i<40; i++) cout<<buffer[i]; // prints what it reads from input.txt 
        printf("\n");

        if(buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J') // start of Control Card
        {
            INIT(); // initialize
            for(int i = 4, j = 0; i < 8; i++, j++)
                pcb.JobID = (pcb.JobID * 10) + (buffer[i] - '0'); // copies JOB ID
            for(int i = 8, j = 0; i < 12; i++, j++)
                pcb.TTL = (pcb.TTL * 10) + (buffer[i] - '0'); // copies Total time limit
            for(int i = 12, j = 0; i < 16; i++, j++)
                pcb.TLL = (pcb.TLL * 10) + (buffer[i] - '0'); // copies total line limit             
        }
        else if(buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A'){ // start of Data Card   
            IC = 00; // as program is stored from 1st page 1st line 
            DataCardCount++, Execute();
        } 
        else if(buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D'){ // end of Job now start once again
            ptr = 0; // as new job starts so start from allocating memory from 0 
            continue;
        } 
        else { // else it has instructions 
            int block = Add_map(0); // add instructions to first page as execution starts from 1st page, 1st line
            int buffptr = 0; // buffer pointer
            ptr = block * 10; // pointer to block to staore instructions at
            for(;ptr < (block * 10) + 10; ++ptr){ // buffptr is buffer pointer and ptr is memory pointer    
                for(int j = 0 ; j < 4; ++j,buffptr++)
                    M[ptr][j] = buffer[buffptr]; // copies instructions from buffer to memory
                if(buffer[buffptr] >='0' && buffer[buffptr] <= '9') PI = 2, MOS(), exit(0); // To check if the address provide is 3 digit if yes then Oprand error 
                if(buffptr >= 40 || buffer[buffptr] == '\n' || buffer[buffptr] == ' ') break; // breaks at end of the instructions or when the buffer overflows     
            } 
            printMemory();
        }
    }while(!infile.eof());
}

void OS :: Execute(){ // Defining Execute function of OS class 
    while(1){
        RA = Add_map(IC); // finds real address of the by taking the instruction counter as input   
        for(int i = 0; i < 4; i++) 
            IR[i] = M[RA][i]; // copies instruction from memory into the Instruction register
        IC++; // incrementing instruction registor to point at next instruction
        
        if (IR[0] == 'G' && IR[1] == 'D'){ // Get Data     input.txt----> Memory
            SI = 1; 
            NoOfGD++; // increments the number of GD instructions count to prevent out of data
            check(), MOS(); // checks if everything is okay
        }

        else if (IR[0] == 'P' && IR[1] == 'D') SI = 2, MOS(); // Put Data     Memory----> output.txt

        else if (IR[0] == 'H'){ // Halt   
            SI = 3, MOS();
            break;
        }

        else if (IR[0] == 'L' && IR[1] == 'R'){ // Load Register (To copy data at given address to genral purpose registor)
            pcb.TTC++, check(); // increments the total time count and checks
            int address = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
            for(int i = 0; i < 4; i++) // loads into genral purpose register
                R[i] = M[address][i];
        }

        else if (IR[0] == 'S' && IR[1] == 'R'){ // Store Register (To copy data from genral purpose registor to the given address)
            pcb.TTC += 2; // 1 to handel page fault and 1 to execute the instruction 
            check(); // checks if everthing is in control
            VA = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
            RA = Add_map(VA);
            for(int i = 0; i < 4; i++)
                M[RA][i] = R[i];
        }

        else if (IR[0] == 'C' && IR[1] == 'R'){ // Compare Register
            pcb.TTC++, check(); // increments the total time count and checks
            int VA = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
            RA = Add_map(VA);
            int flag = 0;
            for(int i = 0; i < 4; i++) // compares data in register and memory 
                if(M[RA][i] != R[i]){
                    flag = 1;
                    break; 
                }
            if(flag == 0) C = true;// sets Toggle register to true if the data matches otherwise it is intialized to false 
        }

        else if (IR[0] == 'B' && IR[1] == 'T'){ // Branch on Toggle{
            pcb.TTC++, check(); // increments the total time count and checks
            if(C == true){        
                int address = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
                IC = address; // sets the IC to given address
            }
        }
        else{ // there is no instruction that matches to given instruction
            PI = 1, MOS();
            exit(0);
        }
    }
}

void OS :: MOS(){ // Defining MOS function of OS class          
    if (TI == 0){ // no timer interrupt so normal
        if(SI == 1) read();
        else if (SI == 2) write();
        else if (SI == 3) terminate();
        else if (PI == 1) line("Operation"),terminate();
        else if (PI == 2) line("Oprand"), terminate();
        else if (PI == 3) cout << "Page fault Occured" << endl;
    }
    else if (TI == 2) {
        if (SI == 1) line("Time Limit Exceeded"),outfile,terminate();
        else if (SI == 2) write(), line("Time Limit Exceeded"), terminate();
        else if (SI == 3) terminate();
        else if (PI == 1 || PI == 2 || PI == 3) line("Time Limit Exceeded"), terminate();
    }
}

void OS :: read(){ // Defining read function of OS class
    pcb.TTC += 2; // increments total time count by 2 (1 for page fault and 1 for exectution) 
    check(); // Checks whether everything is undercontrol
    clearBuffer();
    infile.getline(buffer,41); // copies 41 characters or a line (whichever is small) and pastes it in buffer (41 to adjust \0 at end of the string)
    int buffptr = 0; // buffer pointer
    VA = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
    RA = Add_map(VA)*10; // finds the real address of with the help of given address
    for(int b = 0; b < 10; b++){ // copies data from buffer to a block by splitting it into words of 4 bytes
        for (int i = 0; i < 4, buffptr < 40 ; i++,buffptr++) 
            M[RA][i] = buffer[buffptr];
        RA++;
    }
    printMemory(); 
}

void OS :: write(){ // Defining write function of OS class
    pcb.TTC++, pcb.LLC++, check(); // increments the total time count and total line count and checks
    int VA = ((IR[2] - '0') * 10) + (IR[3]- '0'); // Address is stored in IR[2] & IR[3] in char format so to convert it into int substracting ASCII value of 0 and making it a decimal number 
    RA = Add_map(VA); // finds real address with the help of virtual address
    for (int i = 0; i < 10; i++){ // write the whole block from the Memory to output.txt in a single line
        for(int j = 0; j < 4; j++){
            if(M[RA][j] != '\0') outfile<<M[RA][j];
            else outfile<<' '; // to prevent 'NULNULNUL' in output file
        }
        RA++; // goes to next line 
    }
    outfile<<"\n"; //for writing next block on new line
    printMemory();
}

void OS :: terminate(){ outfile<<"\n\n"; } // leaves 2 lines at end of each job

int main(){ 
    OS os;  //creating the instance of object of Os class
    
    os.infile.open("input.txt", ios::in);   //input file
    os.outfile.open("output.txt", ios::out);  //output file
    srand(time(NULL));

    if(!os.infile) printf("Failure"); //if no input.txt file exists then exits with failure message
    else printf("Success"), os.load(); // if input.txt file exists then enters load function  
    return 0;
}