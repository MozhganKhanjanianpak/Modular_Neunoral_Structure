////////////////////////////////////////////////
// Implementation code for Madular netwok for Seri2: 0.01 < eta < 0.1
// For each type of madular network, import that type
// in input_file_blocks and input_file
////////////////////////////////////////////////
using namespace std;

#include <string.h> 
#include <fstream>
#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

#define linkState(x)  ( (x==0) ? 0 : 1)  // inline function

#define N    2000   // Number of nodes
#define B    8
#define E    0.8    // percentage of Excitatory neurons
#define TE   5      // activation time of Exc. links
#define TI   7      // activation time of Inh. links

#define dEta  0.01   // eta step
#define etaFinal 0.1  // end of eta space: dEta < eta < etaFinal 

#define D    6     // thereshold value for firing
#define tmax 25000    // maximum time

int  A[N][N] = {0};               // Adjacency Matrix
int node_state[N] = {0};          // state of neurons, including: 0=inactive , 1=active
int node_input[N] = {0};          // neuron input
int block_node_number[B+1][4] = {0};   // array for saving the number of nodes in each block
int Link_counter[N][N]         = {0}; // including 0, 1, 2, ...,TE for 0<row<n_E and 0<col<N
int Link_counter_updated[N][N] = {0}; // and 0, 1, 2, ...,TI for n_E<row<N and 0<col<N

/********************************************/
// Main
/********************************************/
int main()
{  
    unsigned seed = time(NULL);
    srand(seed);
    double r;       // double random number between (0,1)
    int t=0;        // time
    
    //-----------------------------------------------
    // INPUT; the number of neurons in each block and the label of Inhibitory ones
    ifstream input_file_blocks("./BlockNodesNumberTypeGE.txt"); 
    int num1,num2,num3,num4,bb,nn,Istart,Iend;
 	input_file_blocks >> num1 >> num2 >> num3 >> num4;
 	block_node_number[0][0] = num1;
	block_node_number[0][1] = num2;
	block_node_number[0][2] = num3;
	block_node_number[0][3] = num4;
    for (int counter=0; counter<num1; counter++){
    	input_file_blocks >> bb >> nn >> Istart >> Iend;
		block_node_number[counter+1][0] = bb;
		block_node_number[counter+1][1] = nn;
		block_node_number[counter+1][2] = Istart;
		block_node_number[counter+1][3] = Iend;
	}
        
    
    //-----------------------------------------------
    // INPUT; the adjacency matrix A    
    ifstream input_file("./MadularAdjListTypeGE.txt"); 
    int n1,n2,n3,i,j,k;
 	input_file >> n1 >> n2 >> n3 ;
 	for (int counter=0; counter<n2; counter++){
    	input_file >> i >> j >> k;
		A[i][j] = k;
	}
     
     
 //------------------------------------------------ 
 // Loop for sweeping pNodeE  
 double pNodeE;
 int R=0;
 for (pNodeE=dEta ; pNodeE<etaFinal ; pNodeE += dEta){
  	R++;
   	int t=0;        
   	// initial values 
   	for (int i=0 ; i<N ; i++){
   		node_state[i]=0;
   		for (int j=0 ; j<N ; j++){
   			Link_counter[i][j] = 0;
   			Link_counter_updated[i][j] = 0;
		}
	}
	
	// output files for each noise --> accordint to R value are separated
	string name1 = "./ActiveExcDensK200T3X150Seri2R"; 
	string name2 = "./ActiveInhDensK200T3X150Seri2R"; 
    string str1 = to_string(R);
    string str2=".txt";
    name1.append(str1);
    name1.append(str2);
    name2.append(str1);
    name2.append(str2);
	ofstream outputExc(name1); //output for active E neurons
	ofstream outputInh(name2); //output for active I neurons
	outputExc << "#time,	RhoE from block 1 to block " << B << endl;
    outputInh << "#time,	RhoI from block 1 to block " << B << endl;
	
    
    //------------------------------------------------   
    // Noise on nodes at t=0 --> becomming active (RANDOMLY) 
    int N_0=N*pNodeE;      // initial number of active excitatory neurons 
    int counter=0;
    int randNode=0;
	while (counter<N_0){
    	randNode = rand()%N;  // a random neuron
    	if (node_state[randNode] == 0){
    		node_state[randNode]=1;
    		counter++;
		}
	}
    
	
	//------------------------------------------------   
    //output at t=0
	//Excitatoty neurons
	outputExc << t << "\t";
	double act_E_density=0; // fraction of excitatory active neurons
	for (int i=0 ; i<num1 ; i++){
		act_E_density=0;
		for (int j=block_node_number[i][3] ; j<block_node_number[i+1][2] ; j++){
			act_E_density += node_state[j];
		}
		outputExc << act_E_density/N << "\t";  //block_node_number[i+1][1]
	}
	outputExc << endl;	
	
	//Inhibitory neurons
	outputInh << t << "\t";
	double act_I_density=0; // fraction of inhibitory active neurons
    for (int i=0 ; i<num1 ; i++){
		act_I_density=0;
		for (int j=block_node_number[i+1][2] ; j<block_node_number[i+1][3] ; j++){
			act_I_density += node_state[j];
		}
		outputInh << act_I_density/N << "\t"; //block_node_number[i+1][1]
	}
	outputInh << endl;
    
    
    //------------------------------------------------ 
    // Starting Dynamics
    t++;
    while ( t<=tmax ){
            
        // updating link-counters 
        for (int i=0 ; i<N ; i++){
            for (int j=0 ; j<N ; j++){
              	
				// for Excitatory links --> A[i][j] == 0 or +1  
				if ( A[i][j]==1 ){
                	
					// links that remain inactive	
              		if ( (node_state[i]==0) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 0;
                     
					// links that become active as a result of activation of thier pre-synaptic neuron  	
                    else if ( (node_state[i]==1) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 1;	
                    
					// active links whose time counter is less than TE   
                    else if ( (Link_counter[i][j] >= 1) && (Link_counter[i][j] < TE) )
                       	Link_counter_updated[i][j] = Link_counter[i][j] + 1;
                        
                    // active links whose time counter is TE --> become inactive
					else if ( Link_counter[i][j] == TE )
                       	Link_counter_updated[i][j] = 0;	
				}
				
				
				// for Inhibitory links --> A[i][j] == 0 or -1
				if ( A[i][j]==-1 ){
                	
					// links that remain inactive	
               		if ( (node_state[i]==0) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 0;
                    
					// links that become active as a result of activation of thier pre-synaptic neuron    	
                    else if ( (node_state[i]==1) && (Link_counter[i][j]==0)  )
                       	Link_counter_updated[i][j] = 1;	
                    
					// active links whose time counter is less than TI    
                    else if ( (Link_counter[i][j] >= 1) && (Link_counter[i][j] < TI) )
                       	Link_counter_updated[i][j] = Link_counter[i][j] + 1;
                    
					// active links whose time counter is TI --> become inactive    
					else if ( Link_counter[i][j] == TI )
                       	Link_counter_updated[i][j] = 0;	
				}		
        	}
    	}
    
		    
        // updating node states
        // First: node inputs
        for (int i=0 ; i<N ; i++){
            node_input[i] = 0;
			int positive_input=0;
			int negative_input=0;
            for (int j=0 ; j<N ; j++){
            	if (A[j][i]==1)
            		positive_input += linkState(Link_counter_updated[j][i]);
            	if (A[j][i]==-1)
            		negative_input += linkState(Link_counter_updated[j][i]);
			}
            // total input
            node_input[i] = positive_input - 4*negative_input;
    	}   
        // Then: node-state
        for (int i=0 ; i<N ; i++){
            node_state[i]=0;
            if ( node_input[i] >= D )
                node_state[i]=1;
        }
        
		    
        // external input on nodes at t
        for (int i=0 ; i<N ; i++){
            r = rand()/double(RAND_MAX);
            if (r<pNodeE) node_state[i]=1;
        }
		 
		 
		//output at t=0
		//Excitatoty neurons
		outputExc << t << "\t";
		for (int i=0 ; i<num1 ; i++){
			act_E_density=0;
			for (int j=block_node_number[i][3] ; j<block_node_number[i+1][2] ; j++){
				act_E_density += node_state[j];
			}
			outputExc << act_E_density/N << "\t";  //block_node_number[i+1][1]
		}
		outputExc << endl;	
		//Inhibitory neurons
		outputInh << t << "\t";
    	for (int i=0 ; i<num1 ; i++){
			act_I_density=0;
			for (int j=block_node_number[i+1][2] ; j<block_node_number[i+1][3] ; j++){
				act_I_density += node_state[j];
			}
			outputInh << act_I_density/N << "\t";  //block_node_number[i+1][1]
		}
		outputInh << endl;
        
		    
        //updating for the next time step
        t++;
        for (int i=0 ; i<N ; i++){
        	for (int j=0; j<N ; j++){
                Link_counter[i][j] = Link_counter_updated[i][j];
            }
		}
                     
    } // end of while
    
  }  // end of Eta loop       
    return 0;
}



