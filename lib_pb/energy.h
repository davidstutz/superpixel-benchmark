/**
 * @file    energy.h
 * @brief   MaxFlow wrapper for adding unary and binary terms of an energy 
 *          function.
 * @details Originally written for MaxFlow v 2.2, but templated for 
 *          compatibility with v 3.0
 * @author  Pete Carr
 * @version 2 May 2008 - Initial Implementation
 * @version 13 February 2009 - Minor fixes to submodularity assertions (for floating 
 *          point precision errors)
 */


/* energy.h */

/* Slight modification of the file energy.h by Vladimir Kolmogorov,
   from energy-v1.1.src.tar.gz, available at
   http://www.adastral.ucl.ac.uk/~vladkolm/software.html

   In this version of the file submodularity is ensured by adjusting
   the values A, B, C, D in add_term2 (see code marked "CHANGED" below)
*/


/* Vladimir Kolmogorov (vnk@cs.cornell.edu), 2003. */

/*
    This software minimizes certain energy functions of binary variables, as described in 

        What Energy Functions can be Minimized via Graph Cuts?
        Vladimir Kolmogorov and Ramin Zabih. 
        In IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI), February 2004. 

    It uses maxflow algorithm described in 

        An Experimental Comparison of Min-Cut/Max-Flow Algorithms for Energy Minimization in Vision
        Yuri Boykov and Vladimir Kolmogorov.
        In IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI), September 2004.

    More specifically, it computes the global minimum of a function E of binary
    variables x_1, ..., x_n which can be written as a sum of terms involving
    at most three variables at a time:

        E(x_1, ..., x_n) = \sum_{i}     E^{i}    (x_i)
                        + \sum_{i,j}   E^{i,j}  (x_i, x_j)
                        + \sum_{i,j,k} E^{i,j,k}(x_i, x_j, x_k)

    The method works only if each term is "submodular". Definitions of submodularity
    for terms E^{i}, E^{i,j}, E^{i,j,k} are given below as comments to functions
    add_term1(), add_term2(), add_term3(). 

    This software can be used only for research purposes. IF YOU USE THIS SOFTWARE,
    YOU SHOULD CITE THE AFOREMENTIONED PAPERS IN ANY RESULTING PUBLICATION.

    In order to use it, you will also need a MAXFLOW software which can be
    obtained from http://www.cs.cornell.edu/People/vnk/software.html

    NOTE: This software minimizes functions of BINARY variables only.
    However, it can also be used for minimizing certain functions of non-binary
    (multi-label) variables via a sequence of binary moves (alpha-expansion, 
    alpha-beta swap, k-jumps, etc.) as proposed in

        Efficient Approximate Energy Minimization via Graph Cuts 
        Yuri Boykov, Olga Veksler, Ramin Zabih, 
        IEEE transactions on PAMI, vol. 20, no. 12, p. 1222-1239, November 2001.

    IF YOU USE THIS SOFTWARE FOR IMPLEMENTING ALPHA-EXPANSION OR ALPHA-BETA SWAP
    ALGORITHM, YOU SHOULD CITE THIS PAPER IN ANY RESULTING PUBLICATION.

    Also note that an implementation of minimization techniques for non-binary variables
    can be downloaded from O. Veksler's homepage: http://www.csd.uwo.ca/faculty/olga/code.html .

    ------------------------------------------------------------------------

    Example usage
    (Minimizes the following function of 3 binary variables:
    E(x, y, z) = x - 2*y + 3*(1-z) - 4*x*y + 5*|y-z|):

    ///////////////////////////////////////////////////

    #include <stdio.h>
    #include "energy.h"

    void main()
    {
        // Minimize the following function of 3 binary variables:
        // E(x, y, z) = x - 2*y + 3*(1-z) - 4*x*y + 5*|y-z|
           
        Energy::Var varx, vary, varz;
        Energy *e = new Energy();

        varx = e -> add_variable();
        vary = e -> add_variable();
        varz = e -> add_variable();

        e -> add_term1(varx, 0, 1);  // add term x 
        e -> add_term1(vary, 0, -2); // add term -2*y
        e -> add_term1(varz, 3, 0);  // add term 3*(1-z)

        e -> add_term2(x, y, 0, 0, 0, -4); // add term -4*x*y
        e -> add_term2(y, z, 0, 5, 5, 0); // add term 5*|y-z|

        Energy::TotalValue Emin = e -> minimize();
        
        printf("Minimum = %d\n", Emin);
        printf("Optimal solution:\n");
        printf("x = %d\n", e->get_var(varx));
        printf("y = %d\n", e->get_var(vary));
        printf("z = %d\n", e->get_var(varz));

        delete e;
    }

    ///////////////////////////////////////////////////
*/

#ifndef __ENERGY_H__
#define __ENERGY_H__

#include <assert.h>
#include "v3p/MaxFlow/graph.h"

template <typename captype, typename tcaptype, typename flowtype>
class Energy : public Graph< captype, tcaptype, flowtype >
{
public:
    typedef typename Graph<captype,tcaptype,flowtype>::node_id Var;

    /* Types of energy values.
       Value is a type of a value in a single term
       TotalValue is a type of a value of the total energy.
       By default Value = short, TotalValue = int.
       To change it, change the corresponding types in graph.h */
    typedef captype Value;
    typedef flowtype TotalValue;

    /* interface functions */

    /* Constructor. Optional argument is the pointer to the
       function which will be called if an error occurs;
       an error message is passed to this function. If this
       argument is omitted, exit(1) will be called. */
    Energy( int maxNodes = 0, int maxEdges = 0, void (*err_function)(char *) = NULL);

    /* Destructor */
    ~Energy();

    /* Adds a new binary variable */
    Var add_variable();

    /* Adds a constant E to the energy function */
    void add_constant(Value E);

    /* Adds a new term E(x) of one binary variable
       to the energy function, where
           E(0) = E0, E(1) = E1
       E0 and E1 can be arbitrary */
    void add_term1(Var x,
                   Value E0, Value E1);

    /* Adds a new term E(x,y) of two binary variables
       to the energy function, where
           E(0,0) = E00, E(0,1) = E01
           E(1,0) = E10, E(1,1) = E11
       The term must be submodular, i.e. E00 + E11 <= E01 + E10 */
    void add_term2(Var x, Var y,
                   Value E00, Value E01,
                   Value E10, Value E11);

    /* Adds a new term E(x,y,z) of three binary variables
       to the energy function, where
           E(0,0,0) = E000, E(0,0,1) = E001
           E(0,1,0) = E010, E(0,1,1) = E011
           E(1,0,0) = E100, E(1,0,1) = E101
           E(1,1,0) = E110, E(1,1,1) = E111
       The term must be submodular. It means that if one
       of the variables is fixed (for example, y=1), then
       the resulting function of two variables must be submodular.
       Since there are 6 ways to fix one variable
       (3 variables times 2 binary values - 0 and 1),
       this is equivalent to 6 inequalities */
    void add_term3(Var x, Var y, Var z,
                   Value E000, Value E001,
                   Value E010, Value E011,
                   Value E100, Value E101,
                   Value E110, Value E111);

    /* After the energy function has been constructed,
       call this function to minimize it.
       Returns the minimum of the function */
    TotalValue minimize();

    /* After 'minimize' has been called, this function
       can be used to determine the value of variable 'x'
       in the optimal solution.
       Returns either 0 or 1 */
    int get_var(Var x);

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

private:
    /* internal variables and functions */

    TotalValue  Econst;
    void        (*error_function)(char *);  /* this function is called if a error occurs,
                                            with a corresponding error message
                                            (or exit(1) is called if it's NULL) */
};















/***********************************************************************/
/************************  Implementation ******************************/
/***********************************************************************/


template <typename captype, typename tcaptype, typename flowtype>
inline Energy<captype,tcaptype,flowtype>::Energy( int maxNodes, int maxEdges, void (*err_function)(char *)) : Graph<captype,tcaptype,flowtype>( maxNodes, maxEdges, err_function)
{
    Econst = 0;
    error_function = err_function;
}

template <typename captype, typename tcaptype, typename flowtype>
inline Energy<captype,tcaptype,flowtype>::~Energy() {}

template <typename captype, typename tcaptype, typename flowtype>
inline
typename Energy<captype,tcaptype,flowtype>::Var
Energy<captype,tcaptype,flowtype>::add_variable()
{
    return this->add_node();
}

template <typename captype, typename tcaptype, typename flowtype>
inline
void
Energy<captype,tcaptype,flowtype>::add_constant(Value A) { Econst += A; }

template <typename captype, typename tcaptype, typename flowtype>
inline
void
Energy<captype,tcaptype,flowtype>::add_term1(Var x,Value A, Value B)
{
    add_tweights(x, B, A);
}

template <typename captype, typename tcaptype, typename flowtype>
inline
void
Energy<captype,tcaptype,flowtype>::add_term2(Var x, Var y, Value A, Value B, Value C, Value D)
{
#if 1
    /* **** CHANGED ****
     *  added code below to ensure regularity / submodularity
     */
    if ( A+D > C+B) 
    {
        Value delta = A+D-C-B;
        Value subtrA = delta/3;

        A = A-subtrA;
        C = C+subtrA;
        B = B+(delta-subtrA*2);
    }
    /* *** END CHANGE **** */
#endif
        
    /* 
       E = A A  +  0   B-A
           D D     C-D 0
       Add edges for the first term
    */
    add_tweights(x, D, A);          // The first term is not dependent on y
                                    // Add weight of D-A
    B -= A; C -= D;                 // B = B-A
                                    // C = C-D

    /* now need to represent
       0 B
       C 0
    */

    // Modified by PC on 13/02/09
    // Truncation may be necessary because of floating point precision errors.
    // If the edge weight is negative, make sure it is very close to zero.
    assert( B + C > -1e-8 );

    if (B < 0)
    {
        /* Write it as
           B B  +  -B 0  +  0   0
           0 0     -B 0     B+C 0
        */
        add_tweights(x, 0, B); /* first term */         // does not depend on y
        add_tweights(y, 0, -B); /* second term */       // does not depend on x
        add_edge(x, y, 0, B+C < 0 ? 0 : B+C ); /* third term */
    }
    else if (C < 0)
    {
        /* Write it as
           -C -C  +  C 0  +  0 B+C
            0  0     C 0     0 0
        */
        add_tweights(x, 0, -C); /* first term */
        add_tweights(y, 0, C); /* second term */
        add_edge(x, y, B+C < 0 ? 0 : B+C , 0); /* third term */
    }
    else /* B >= 0, C >= 0 */
    {
        add_edge(x, y, B, C);       // Add edges (B-A) and (C-D)
    }
}

template <typename captype, typename tcaptype, typename flowtype>
inline
void
Energy<captype,tcaptype,flowtype>::add_term3(Var x, Var y, Var z,
                              Value E000, Value E001,
                              Value E010, Value E011,
                              Value E100, Value E101,
                              Value E110, Value E111)
{
    register Value pi = (E000 + E011 + E101 + E110) - (E100 + E010 + E001 + E111);
    register Value delta;
    register Var u;

    if (pi >= 0)
    {
        Econst += E111 - (E011 + E101 + E110);

        add_tweights(x, E101, E001);
        add_tweights(y, E110, E100);
        add_tweights(z, E011, E010);

        delta = (E010 + E001) - (E000 + E011); /* -pi(E[x=0]) */
        assert(delta >= 0); /* check regularity */
        add_edge(y, z, delta, 0);

        delta = (E100 + E001) - (E000 + E101); /* -pi(E[y=0]) */
        assert(delta >= 0); /* check regularity */
        add_edge(z, x, delta, 0);

        delta = (E100 + E010) - (E000 + E110); /* -pi(E[z=0]) */
        assert(delta >= 0); /* check regularity */
        add_edge(x, y, delta, 0);

        if (pi > 0)
        {
            u = add_variable();
            add_edge(x, u, pi, 0);
            add_edge(y, u, pi, 0);
            add_edge(z, u, pi, 0);
            add_tweights(u, 0, pi);
        }
    }
    else
    {
        Econst += E000 - (E100 + E010 + E001);

        add_tweights(x, E110, E010);
        add_tweights(y, E011, E001);
        add_tweights(z, E101, E100);

        delta = (E110 + E101) - (E100 + E111); /* -pi(E[x=1]) */
        assert(delta >= 0); /* check regularity */
        add_edge(z, y, delta, 0);

        delta = (E110 + E011) - (E010 + E111); /* -pi(E[y=1]) */
        assert(delta >= 0); /* check regularity */
        add_edge(x, z, delta, 0);

        delta = (E101 + E011) - (E001 + E111); /* -pi(E[z=1]) */
        assert(delta >= 0); /* check regularity */
        add_edge(y, x, delta, 0);

        u = add_variable();
        add_edge(u, x, -pi, 0);
        add_edge(u, y, -pi, 0);
        add_edge(u, z, -pi, 0);
        add_tweights(u, -pi, 0);
    }
}

template <typename captype, typename tcaptype, typename flowtype>
inline
typename Energy<captype,tcaptype,flowtype>::TotalValue
Energy<captype,tcaptype,flowtype>::minimize()
{
    return Econst + this->maxflow();
}

template <typename captype, typename tcaptype, typename flowtype>
inline
int
Energy<captype,tcaptype,flowtype>::get_var(Var x) { return (int)what_segment(x); }

#endif
