#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;
class OS1
{
public:
    int jobID, TTL, TLL;
    int TTC, LLC;
};

class OS2
{
private:
    fstream infile;
    ofstream outfile;
    char Memory[300][4], buffer[40], IR[4], R[4];
    OS1 os1;
    bool C;
    int IC;
    int SI, PI, TI;
    int PTR;
    int visited[30];
    bool mos;
    int RA, VA;
    bool Terminate;
    int PTE;
    int page_fault = 0;
    int pageNo;
    int PageTable_ptr;

    void INIT()
    {
        fill(buffer, buffer + sizeof(buffer), '\0');
        fill(&Memory[0][0], &Memory[0][0] + sizeof(Memory), '\0');
        for (int i = 0; i < 4; i++) // Initialize IR and R
        {
            IR[i] = ' ';
            R[i] = ' ';
        }

        C = true;
        IC = 0;
        SI = PI = TI = 0;

        os1.jobID = os1.TLL = os1.TTL = os1.TTC = os1.LLC = 0;
        PTR = PTE = pageNo = -1;
        for (int i = 0; i < 30; i++)
        {
            visited[i] = 0;
        }
        PageTable_ptr = 0;
        Terminate = false;
    }

    void BUFFER()
    {
        fill(buffer, buffer + sizeof(buffer), '\0');
    }

    int ALLOCATE()
    {
        int pageNo;
        bool check = true;
        while (check)
        {
            pageNo = (rand() % 30);
            if (visited[pageNo] == 0)
            {
                visited[pageNo] = 1;
                check = false;
            }
        }

        return pageNo;
    }

    void MOS()
    {
        if (TI == 0 && SI == 1)
        {
            READ();
        }
        else if (TI == 0 && SI == 2)
        {
            WRITE();
        }
        else if (TI == 0 && SI == 3)
        {
            TERMINATE(0);
        }
        else if (TI == 2 && SI == 1)
        {
            TERMINATE(3);
        }
        else if (TI == 2 && SI == 2)
        {
            WRITE();
            TERMINATE(3);
        }
        else if (TI == 2 && SI == 3)
        {
            TERMINATE(0);
        }
        else if (TI == 0 && PI == 1)
        {
            TERMINATE(4);
        }
        else if (TI == 0 && PI == 2)
        {
            TERMINATE(5);
        }
        else if (TI == 0 && PI == 3)
        {

            if (page_fault == 1)
            {
                cout << " \nVaild Page Fault:   ";
                pageNo = ALLOCATE();
                Memory[PTE][2] = (pageNo / 10) + '0';
                Memory[PTE][3] = (pageNo % 10) + '0';
                PageTable_ptr++;
                PI = 0;
                cout << "Allocated Page Number: " << pageNo << " (Memory location: " << pageNo * 10 <<"\n";
            }
            else
            {
                os1.TTC--;
                cout << " \nInVaild Page Fault!!!";
                TERMINATE(6);
            }
        }
        else if (TI == 2 && PI == 1)
        {
            TERMINATE(3);
        }
        else if (TI == 2 && PI == 2)
        {
            TERMINATE(3);
        }
        else if (TI == 2 && PI == 3)
        {
            TERMINATE(3);
        }
        else
        {
            os1.TTC--;
            TERMINATE(3);
        }
    }

    void READ()
    {

        string data;
        getline(infile, data);
        if (data[0] == '$' && data[1] == 'E' && data[2] == 'N' && data[3] == 'D')
        {
            os1.TTC--;
            TERMINATE(1);
            return;
        }
        int len = data.size();
        for (int i = 0; i < len; i++)
        {
            buffer[i] = data[i];
        }

        int buff = 0, mem_ptr = RA, end = RA + 10;
        while (buff < 40 && buffer[buff] != '\0' && mem_ptr < end)
        {
            for (int i = 0; i < 4; i++)
            {
                Memory[mem_ptr][i] = buffer[buff];
                buff++;
            }
            mem_ptr++;
        }
        BUFFER();
        SI = 0;
    }

    void WRITE()
    {

        os1.LLC++;
        if (os1.LLC > os1.TLL)
        {
            os1.LLC--;
            TERMINATE(2);
            return;
        }

        outfile.open("output.txt", ios::app);
        string output;

        if (RA != -1)
        {
            for (int i = RA; i < RA + 10; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (Memory[i][j] == '\0')
                    {
                        Memory[i][j] = ' ';
                    }

                    output += Memory[i][j];
                }
            }

            outfile << output << "\n";
        }

        SI = 0;
        outfile << "\n";
        outfile.close();
    }

    void TERMINATE(int EM)
    {
        Terminate = true;
        outfile.open("output.txt", ios::app);

        outfile << "Job Id :" << os1.jobID << "\n";
        switch (EM)
        {
        case 0:
            outfile << "No Error: Program executed successfully\n";
            break;
        case 1:
            outfile << "Error: Out of Data\n";
            break;
        case 2:
            outfile << "Error: Line Limit Exceeded\n";
            break;
        case 3:
        {
            if (TI == 2 && PI == 1)
            {
                outfile << "Error: Time Limit Exceeded With Operation Code Error\n";
                break;
            }
            if (TI == 2 && PI == 2)
            {
                outfile << "Error: Time Limit Exceeded With Operand Error\n";
                break;
            }
            if (TI == 2 && PI == 3)
            {
                outfile << "Error: Time Limit Exceeded\n";
                break;
            }
        }

        case 4:
            outfile << "Error: Operation Code Error\n";
            break;
        case 5:
            os1.TTC--;
            outfile << "Error: Operand Error\n";
            break;
        case 6:
            outfile << "Error: Invalid Page Fault\n";
        }
        outfile << "IC: " << IC << "\n";
        outfile << "IR: ";
        for (int i = 0; i < 4; i++)
        {
            if (IR[i] != '\0')
                outfile << IR[i];
        }
        outfile << "\n";
        outfile << "TTC: " << os1.TTC << "\n";
        outfile << "LLC: " << os1.LLC << "\n";

        outfile << "\n\n\n";
        SI = 0;
        PI = 0;
        TI = 0;

        outfile.close();
    }

    void LOAD()
    {
        if (infile.is_open())
        {
            string s;
            while (getline(infile, s))
            {
                if (s[0] == '$' && s[1] == 'A' && s[2] == 'M' && s[3] == 'J')
                {
                    INIT();
                    cout << "New Job started\n";
                    os1.jobID = (s[4] - '0') * 1000 + (s[5] - '0') * 100 + (s[6] - '0') * 10 + (s[7] - '0');
                    os1.TTL = (s[8] - '0') * 1000 + (s[9] - '0') * 100 + (s[10] - '0') * 10 + (s[11] - '0');
                    os1.TLL = (s[12] - '0') * 1000 + (s[13] - '0') * 100 + (s[14] - '0') * 10 + (s[15] - '0');

                    // Get Frame for Page Table
                    PTR = ALLOCATE() * 10;
                    for (int i = PTR; i < PTR + 10; i++)
                    {
                        for (int j = 0; j < 4; j++)
                        {
                            Memory[i][j] = '*';
                        }
                    }
                    cout << "\nAllocated Page is for Page Table: " << PTR / 10 <<" (Memory location: " << PTR << ")\n";
                    cout << "jobID: " << os1.jobID << "\nTTL: " << os1.TTL << "\nTLL: " << os1.TLL << "\n";
                }
                else if (s[0] == '$' && s[1] == 'D' && s[2] == 'T' && s[3] == 'A')
                {
                    BUFFER();

                    // Allocate frame for the data card
                    int dataPageNo = ALLOCATE();
                    cout << "\nAllocated pageno. for data card=" << dataPageNo <<" (Memory location: " << dataPageNo * 10 << ")\n"; // Added line
                    int dataPagePtr = dataPageNo * 10;

                    // Copy data into memory
                    int buff = 0;
                    while (buff < 40 && buffer[buff] != '\0' && dataPagePtr < (dataPageNo + 1) * 10)
                    {
                        for (int j = 0; j < 4; j++)
                        {
                            Memory[dataPagePtr][j] = buffer[buff];
                            buff++;
                        }
                        dataPagePtr++;
                    }

                    STARTEXECUTION();
                }
                else if (s[0] == '$' && s[1] == 'E' && s[2] == 'N' && s[3] == 'D')
                {
                    cout << "\nEnd of Job\n";
                    for (int i = 0; i < 300; i++)
                    {
                        if (i == PTR)
                        {
                            cout << "---Page Table---\n";
                        }
                        if (i == PTR + 10)
                        {
                            cout << "--Page Table End--\n";
                        }
                        cout << "[ " << i << " ] : ";

                        for (int j = 0; j < 4; j++)
                        {
                            cout << Memory[i][j] << " ";
                        }
                        cout << '\n';
                    }
                }
                else
                {
                    BUFFER();
                    // Get Frame for Program Page
                    pageNo = ALLOCATE();
                    Memory[PTR + PageTable_ptr][2] = (pageNo / 10) + '0';
                    Memory[PTR + PageTable_ptr][3] = (pageNo % 10) + '0';
                    PageTable_ptr++;

                    cout << "\nAllocated pageno. for prog card=" << pageNo <<" (Memory location: " << pageNo * 10 << ")\n";
                    int length = s.size();

                    for (int i = 0; i < length; i++)
                    {
                        buffer[i] = s[i];
                    }

                    int buff = 0;
                    IC = pageNo * 10;
                    int end = IC + 10;

                    while (buff < 40 && buffer[buff] != '\0' && IC < end)
                    {
                        for (int j = 0; j < 4; j++)
                        {
                            if (buffer[buff] == 'H')
                            {
                                Memory[IC][j] = 'H';
                                buff++;
                                break;
                            }
                            Memory[IC][j] = buffer[buff];
                            buff++;
                        }
                        IC++;
                    }
                }
            }
            infile.close();
        }
    }


    int ADDRESSMAP(int VA)
    {
        if (0 <= VA && VA < 100)
        {
            PTE = PTR + (VA / 10);
            if (Memory[PTE][2] == '*')
            {
                PI = 3;
                MOS();
            }

            else
            {
                string p;
                p = Memory[PTE][2];
                p += Memory[PTE][3];
                int pageNo = stoi(p);
                RA = pageNo * 10 + (VA % 10);
                return RA;
            }
        }
        else
        {
            PI = 2;
            cout << "\noperand error called";

            MOS();
        }
        return pageNo * 10;
    }

    void STARTEXECUTION()
    {
        IC = 0;
        EXECUTEUSERPROGRAM();
    }

    void EXECUTEUSERPROGRAM()
    {
        while (!Terminate)
        {
            mos = false;
            RA = ADDRESSMAP(IC);
            if (PI != 0)
            {
                return;
            }

            for (int i = 0; i < 4; i++)
            {
                IR[i] = Memory[RA][i];
            }

            IC++;

            string op;
            op += IR[2];
            op += IR[3];
            cout << "\ninstruct = " << IR[0] << IR[1] << IR[2] << IR[3];
            // GD - Get Data
            if (IR[0] == 'G' && IR[1] == 'D')
            {
                SIMULATION();
                page_fault = 1;
                if (!isdigit(IR[2]) || !isdigit(IR[3]))
                {
                    PI = 2;

                    mos = true;
                }
                else
                {
                    VA = stoi(op);
                    cout << "\nvirtual address= " << VA;
                    RA = ADDRESSMAP(VA);

                    SI = 1;
                    mos = true;
                }
            }

            // PD - Print Data
            else if (IR[0] == 'P' && IR[1] == 'D')
            {

                SIMULATION();

                page_fault = 0;
                if (!isdigit(IR[2]) || !isdigit(IR[3]))
                {
                    PI = 2;
                    os1.TTC--;
                    mos = true;
                }
                else
                {
                    SI = 2;

                    VA = stoi(op);
                    RA = ADDRESSMAP(VA);
                    mos = true;
                }
            }

            // H - Halt
            else if (IR[0] == 'H' && IR[1] == '\0')
            {
                SIMULATION();
                SI = 3;
                mos = true;
                Terminate = true;
            }

            // LR - LOAD DATA (From memory to register)
            else if (IR[0] == 'L' && IR[1] == 'R')
            {
                SIMULATION();
                page_fault = 0;
                if (!isdigit(IR[2]) || !isdigit(IR[3]))
                {
                    PI = 2;
                    mos = true;
                }
                else
                {
                    VA = stoi(op);
                    RA = ADDRESSMAP(VA);
                    for (int i = 0; i < 4; i++)
                    {
                        R[i] = Memory[RA][i];
                    }
                }
            }

            // SR - STORE (Date of register into memory)
            else if (IR[0] == 'S' && IR[1] == 'R')
            {
                SIMULATION();
                page_fault = 1;
                if (!isdigit(IR[2]) || !isdigit(IR[3]))
                {
                    PI = 2;
                    mos = true;
                }

                else
                {
                    VA = stoi(op);

                    RA = ADDRESSMAP(VA);

                    for (int i = 0; i < 4; i++)
                    {
                        Memory[RA][i] = R[i];
                    }
                }
            }

            // CR - Compare
            else if (IR[0] == 'C' && IR[1] == 'R')
            {
                SIMULATION();
                page_fault = 0;
                if (!isdigit(IR[2]) || !isdigit(IR[3]))
                {
                    PI = 2;
                    mos = true;
                }
                else
                {
                    VA = stoi(op);
                    RA = ADDRESSMAP(VA);
                    string s1, s2;
                    for (int i = 0; i < 4; i++)
                    {
                        s1 += Memory[RA][i];
                        s2 += R[i];
                    }
                    if (s1 == s2)
                    {
                        C = true;
                    }
                    else
                    {
                        C = false;
                    }
                }
            }

            // BT
            else if (IR[0] == 'B' && IR[1] == 'T')
            {
                SIMULATION();
                page_fault = 0;
                if (!isdigit(IR[2]) || !isdigit(IR[3]))
                {
                    PI = 2;
                    mos = true;
                }
                else
                {
                    if (C)
                    {
                        string j;
                        j += IR[2];
                        j += IR[3];
                        IC = stoi(j);
                    }
                }
            }
            else
            {
                SIMULATION();
                PI = 1;
                SI = 0;
                os1.TTC--;
                mos = true;
            }

            if (mos)
            {
                MOS();
            }
        }
    }

    void SIMULATION()
    {
        if (IR[0] == 'G' && IR[1] == 'D')
        {
            os1.TTC += 1;
        }

        else if (IR[0] == 'P' && IR[1] == 'D')
        {
            os1.TTC += 1;
        }

        else if (IR[0] == 'H')
        {
            os1.TTC += 1;
        }

        else if (IR[0] == 'L' && IR[1] == 'R')
        {
            os1.TTC += 1;
        }

        else if (IR[0] == 'S' && IR[1] == 'R')
        {
            os1.TTC += 1;
        }

        else if (IR[0] == 'C' && IR[1] == 'R')
        {
            os1.TTC += 1;
        }

        else if (IR[0] == 'B' && IR[1] == 'T')
        {
            os1.TTC += 1;
        }
        else
        {
            os1.TTC += 1;
        }

        cout << "\nTTC= " << os1.TTC;

        if (os1.TTC > os1.TTL)
        {
            TI = 2;
            cout << "\nTime limit exeded";
            cout << "\nTI = " << TI;
            cout << "\nSI = " << SI;
            cout << "\nPI = " << PI;
            mos = true;
        }
    }

public:
    OS2()
    {
        infile.open("Input_1_2.txt", ios::in);

        INIT();
        LOAD();
    }
};

int main()
{
    OS2 os;
    return 0;
}
