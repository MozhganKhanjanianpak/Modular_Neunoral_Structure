//////////////////////////////////////
// Implementation code for Serri 1                  
//////////////////////////////////////
using namespace std;

#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

#define linkState(x)  ( (x==0) ? 0 : 1)  // inline function

#define N    2000   // Number of nodes
#define p    0.1    // probability of connections in ER network
#define E    0.8    // percentage of Excitatory neurons
#define TE   5      // activation time of Exc. links

#define dEta  0.1   // eta step
#define etaFinal 1  // end of eta space: dEta < eta < etaFinal 

#define TI   7      // activation time of Inh. links
#define D    4     // thereshold value for firing
#define tmax 20000    // maximum time
#define Icurrent   4  //current of an active inhibitory synapse -> set it based on E value
#define Ecurrent   1

int  A[N][N] = {0};               // Adjacency Matrix
int node_state[N] = {0};          // state of neurons, including: 0=inactive , 1=active
double node_input[N] = {0};          // neuron input
int Link_counter[N][N]         = {0}; // including 0, 1, 2, ...,TE for 0<row<n_E and 0<col<N
int Link_counter_updated[N][N] = {0}; // and 0, 1, 2, ...,TI for n_E<row<N and 0<col<N

/********************************************/
// Main
/********************************************/
int main()
{
    ofstream output("./Seri1N2000T4.txt"); //output for active neurons
    output << "#N,	<k> = " << N << "\t" << N*p << endl;
    output << "#deltaE,	deltaI,	Icurrent = " << TE << "\t" << TI << "\t" << Icurrent << endl;
	output << "#hor:time,	ver:Eta" << endl;
	output << "#dEta = " << dEta << endl;
      
    
    unsigned seed = time(NULL);
    srand(seed);
    double r;       // double random number between (0,1)
    int n_E = N*E;  // number of Excitatory neurons
 
    
    //-----------------------------------------------
    // making Adjacency matrix of ER network (Directed)
    for ( int i=0 ; i<N; i++ ){
        for ( int j=0 ; j<N ; j++ ){
            if (j==i) continue;
            double r = rand()/double(RAND_MAX);
            if (r<p){
                A[i][j]=1; 
            }
        }
    }
    //------------------------------------------------
    // Loop for sweeping Eta
    double eta;
    for (eta=dEta ; eta<etaFinal ; eta += dEta){
    	int t=0;        
    	// initial values 
    	for (int i=0 ; i<N ; i++){
    		node_state[i]=0;
    		for (int j=0 ; j<N ; j++){
    			Link_counter[i][j] = 0;
    			Link_counter_updated[i][j] = 0;
			}
		}    
	
	// neurons
	double act_E_density=0; // fraction of excitatory active neurons
    double act_I_density=0; // fraction of inhibitory active neurons
	    
    // Noise on nodes at t=0
    int N_E_0=n_E*eta;      // initial number of active excitatory neurons
	int N_I_0=(N-n_E)*eta;  // initial number of active inhibitory neurons
    
    
	// becomming active
	// Excitatory
    for (int i=0 ; i<N_E_0 ; i++){
        node_state[i]=1;
        act_E_density++;
    }
    // Inhibitory
    for (int i=0 ; i<N_I_0 ; i++){
        node_state[N-i-1]=1;
        act_I_density++;
    }
     
	    
    //output at t=0
    // neurons
    act_E_density /= N;
    act_I_density /= N;
    output << eta << "\t" << act_E_density + act_I_density << "\t";
    
    
    // Starting Dynamics
    t++;
    while ( t<=tmax ){
            
        // updating link-counters for Excitatory links
        for (int i=0 ; i<n_E ; i++){
            for (int j=0 ; j<N ; j++){
              	if ( A[i][j]!=0 ){
                	
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
        	}
    	}
        
		// updating link-counters for Inhibitory links
        for (int i=n_E ; i<N ; i++){
            for (int j=0 ; j<N ; j++){
              	if ( A[i][j]!=0 ){
                	
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
            // positive
			int positive_input=0;
            for (int j=0 ; j<n_E ; j++){
            	positive_input += ( A[j][i] *  linkState(Link_counter_updated[j][i]) );
			}
            // negative
            int negative_input=0;
            for (int j=n_E ; j<N ; j++){
            	negative_input += ( A[j][i] *  linkState(Link_counter_updated[j][i]) );
			}
            // total input
            node_input[i] = Ecurrent*positive_input - Icurrent*negative_input;
    	}   
        // Then: node-state
        for (int i=0 ; i<N ; i++){
            node_state[i]=0;
            if ( node_input[i] >= D )
                node_state[i]=1;
        }
        
		    
        // external input on nodes at t
        // excitatory neurons
        for (int i=0 ; i<N ; i++){
            r = rand()/double(RAND_MAX);
            if (r<eta) node_state[i]=1;
        }
         
		   
        // counting active nodes
        act_E_density=0;
        act_I_density=0;
        for (int i=0 ; i<n_E ; i++)
            if (node_state[i])
                act_E_density++;
        for (int i=n_E ; i<N ; i++)
            if (node_state[i])
                act_I_density++;    
          
		   
        // output at t
        // active neurons
        act_E_density /= N;
        act_I_density /= N;
        output << act_E_density + act_I_density << "\t";
        
		    
        //updating for the next time step
        t++;
        for (int i=0 ; i<N ; i++){
        	for (int j=0; j<N ; j++){
                Link_counter[i][j] = Link_counter_updated[i][j];
            }
		}
                     
    } // end of while
    
    output << endl;
    
 } // end of Eta loop
        
    return 0;
}



