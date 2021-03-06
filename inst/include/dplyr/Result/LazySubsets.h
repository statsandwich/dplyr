#ifndef dplyr_LazySubsets_H
#define dplyr_LazySubsets_H

namespace dplyr {
     
    class LazySubsets {
    public:
        typedef dplyr_hash_map<SEXP,SEXP> DataMap ;
        typedef DataMap::const_iterator const_iterator ;
        
        LazySubsets(){}
        
        LazySubsets( const DataFrame& df) : data_map(), nr(df.nrows()){
            CharacterVector names = df.names() ;
            for( int i=0; i<df.size(); i++){
                SEXP column = df[i] ;
                if( Rf_inherits( column, "data.frame" ) ){
                    DataFrame inner_df(column) ;
                    if( inner_df.nrows() != df.nrows() ){
                        std::stringstream s ;
                        s << "inner data frame with number of rows ("
                          << inner_df.nrows()
                          << ") incompatible with host data frame number of rows ("
                          << df.nrows() 
                          << ")" ;
                        stop(s.str()) ;
                    }
                    int inner_ncol = inner_df.size() ;
                    CharacterVector inner_names = inner_df.names() ;
                    for(int j=0; j<inner_ncol; j++){
                        data_map[as_symbol(inner_names[j])] = inner_df[j] ;
                    }
                }
                data_map[as_symbol(names[i])] = df[i] ;    
            }
        }
        virtual ~LazySubsets(){}
        
        virtual SEXP get_variable(SEXP symbol) const {
            DataMap::const_iterator it = data_map.find(symbol) ;
            if( it == data_map.end() ){
                std::stringstream s ;
                s << "variable '"
                  << CHAR(PRINTNAME(symbol))
                  << "' not found" ;
                Rcpp::stop(s.str()) ;
            }
            return it->second ;
        }
        virtual bool is_summary( SEXP symbol ) const {
            return false ;    
        }
        virtual int count(SEXP symbol) const{
            return data_map.count(symbol);    
        }
        
        virtual void input( SEXP symbol, SEXP x){
            data_map[symbol] = x;    
        }
        
        inline const_iterator find(SEXP x) const {
            return data_map.find(x) ;
        }
        
        inline const_iterator end() const {
            return data_map.end() ;   
        }
        
        virtual int size() const{ 
            return data_map.size() ; 
        }
        
        inline SEXP& operator[](SEXP symbol){
            return data_map[symbol] ;    
        }
        
        inline int nrows() const {
            return nr ;
        }
        
    private:
        DataMap data_map ;
        int nr ;
    } ;

}

#endif
