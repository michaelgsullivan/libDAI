/*  Copyright (C) 2006-2008  Joris Mooij  [j dot mooij at science dot ru dot nl]
    Radboud University Nijmegen, The Netherlands

    This file is part of libDAI.

    libDAI is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    libDAI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libDAI; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef __defined_libdai_lc_h
#define __defined_libdai_lc_h


#include <string>
#include <dai/daialg.h>
#include <dai/enum.h>
#include <dai/factorgraph.h>
#include <dai/properties.h>
#include <dai/exceptions.h>


namespace dai {


class LC : public DAIAlgFG {
    private:
        std::vector<Factor>      _pancakes;      // used by all LC types (psi_I is stored in the pancake)
        std::vector<Factor>      _cavitydists;   // used by all LC types to store the approximate cavity distribution
        /// _phis[i][_I] corresponds to \f$ \phi^{\setminus i}_I(x_{I \setminus i}) \f$
        std::vector<std::vector<Factor> >      _phis;

        /// Single variable beliefs
        std::vector<Factor>      _beliefs;

        /// Maximum difference encountered so far
        double                  _maxdiff;
        /// Number of iterations needed
        size_t                  _iters;

    public:
        struct Properties {
            size_t verbose;
            size_t maxiter;
            double tol;
            bool reinit;
            double damping;
            DAI_ENUM(CavityType,FULL,PAIR,PAIR2,UNIFORM)
            CavityType cavity;
            DAI_ENUM(UpdateType,SEQFIX,SEQRND,NONE)
            UpdateType updates;
            std::string cavainame;      // FIXME: needs assignment operator?
            PropertySet cavaiopts;      // FIXME: needs assignment operator?
        } props;
        /// Name of this inference method
        static const char *Name;

    public:
        /// Default constructor
        LC() : DAIAlgFG(), _pancakes(), _cavitydists(), _phis(), _beliefs(), _maxdiff(), _iters(), props() {}

        /// Construct from FactorGraph fg and PropertySet opts
        LC( const FactorGraph &fg, const PropertySet &opts );

        /// Copy constructor
        LC( const LC &x ) : DAIAlgFG(x), _pancakes(x._pancakes), _cavitydists(x._cavitydists), _phis(x._phis), _beliefs(x._beliefs), _maxdiff(x._maxdiff), _iters(x._iters), props(x.props) {}

        /// Clone *this (virtual copy constructor)
        virtual LC* clone() const { return new LC(*this); }

        /// Create (virtual default constructor)
        virtual LC* create() const { return new LC(); }

        /// Assignment operator
        LC& operator=( const LC &x ) {
            if( this != &x ) {
                DAIAlgFG::operator=( x );
                _pancakes     = x._pancakes;
                _cavitydists  = x._cavitydists;
                _phis         = x._phis;
                _beliefs      = x._beliefs;
                _maxdiff      = x._maxdiff;
                _iters        = x._iters;
                props         = x.props;
            }
            return *this;
        }

        /// Identifies itself for logging purposes
        virtual std::string identify() const;

        /// Get single node belief
        virtual Factor belief( const Var &n ) const { return( _beliefs[findVar(n)] ); }

        /// Get general belief
        virtual Factor belief( const VarSet &/*ns*/ ) const {
            DAI_THROW(NOT_IMPLEMENTED);
            return Factor(); 
        }

        /// Get all beliefs
        virtual std::vector<Factor> beliefs() const { return _beliefs; }

        /// Get log partition sum
        virtual Real logZ() const { 
            DAI_THROW(NOT_IMPLEMENTED);
            return 0.0; 
        }

        /// Clear messages and beliefs
        virtual void init();

        /// Clear messages and beliefs corresponding to the nodes in ns
        virtual void init( const VarSet &/*ns*/ ) { init(); }

        /// The actual approximate inference algorithm
        virtual double run();

        /// Return maximum difference between single node beliefs in the last pass
        virtual double maxDiff() const { return _maxdiff; }

        /// Return number of passes over the factorgraph
        virtual size_t Iterations() const { return _iters; }

        double CalcCavityDist( size_t i, const std::string &name, const PropertySet &opts );
        double InitCavityDists( const std::string &name, const PropertySet &opts );
        long SetCavityDists( std::vector<Factor> &Q );

        Factor NewPancake (size_t i, size_t _I, bool & hasNaNs);

        void CalcBelief (size_t i);
        const Factor &belief (size_t i) const { return _beliefs[i]; };
        const Factor &pancake (size_t i) const { return _pancakes[i]; };
        const Factor &cavitydist (size_t i) const { return _cavitydists[i]; };

        void setProperties( const PropertySet &opts );
        PropertySet getProperties() const;
        std::string printProperties() const;
};


} // end of namespace dai


#endif
