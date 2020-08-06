
#ifndef RCPPDATE__RCPPDATE_H
#define RCPPDATE__RCPPDATE_H

#include <RcppCommon.h>
#include <vector>
#include "date.h"
#include <cstring>  //std::memcpy
#include <algorithm>  // std::transform



namespace Rcpp {

  namespace traits {
  
    template <class Duration> date::sys_time<Duration> as(SEXP dt);
    
    template <class Duration> std::vector<date::sys_time<Duration>> as(SEXP dt);
  }
  
  
  template <class Duration> SEXP wrap(date::sys_time<Duration> d);
  template <> SEXP wrap(date::sys_days d); // aka date::sys_time<date::days>
  template <> SEXP wrap(date::sys_time<std::chrono::nanoseconds> d);
  
  template <class Duration> SEXP wrap(const std::vector<date::sys_time<Duration>>& d);
  template <> SEXP wrap(const std::vector<date::sys_days>& d); 
  template <> SEXP wrap(const std::vector<date::sys_time<std::chrono::nanoseconds>>& d);

}


#include <Rcpp.h>

namespace Rcpp {


  template <class Duration> SEXP wrap(date::sys_time<Duration> d){
  
    auto d_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(d.time_since_epoch());
  
    Rcpp::DatetimeVector res(1, "UTC");
    res[0] = d_ns.count() * 1e-9;
    
    return res;
  };
  
  template <> SEXP wrap(date::sys_days d){
    
    Rcpp::DateVector res(1);
    res[0] = static_cast<double>(d.time_since_epoch().count());
      
    return res;
  };
  
  
  template <> SEXP wrap(date::sys_time<std::chrono::nanoseconds> d){
  
    Rcpp::NumericVector n(1);
    int64_t d_ns{d.time_since_epoch().count()};
  
    std::memcpy(&(n[0]), &d_ns, sizeof(double));
  
    Rcpp::CharacterVector cl = Rcpp::CharacterVector::create("nanotime");
    cl.attr("package") = "nanotime";
    n.attr(".S3Class") = "integer64";
    n.attr("class") = cl;
    SET_S4_OBJECT(n);
  
    return n;
  };


// std::vector
  template <class Duration> SEXP wrap(const std::vector<date::sys_time<Duration>>& dv){
    
    Rcpp::DatetimeVector res(dv.size(), "UTC");
    
    std::transform(dv.begin(), dv.end(), res.begin(), 
                   [](date::sys_time<Duration> d){ return 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(d.time_since_epoch()).count();});
    
    return res;
  };
  
  template <> SEXP wrap(const std::vector<date::sys_days>& dv){
    
    Rcpp::DateVector res(dv.size());
    
    std::transform(dv.begin(), dv.end(), res.begin(), 
                   [](date::sys_days d){ return static_cast<int>(d.time_since_epoch().count());});
    
    
    return res;
  };
  
  
  template <> SEXP wrap(const std::vector<date::sys_time<std::chrono::nanoseconds>>& dv){
    
    Rcpp::NumericVector res(dv.size());
    
    
    auto f = [](date::sys_time<std::chrono::nanoseconds> d) -> double {
      int64_t ns{d.time_since_epoch().count()};
      double dd;
      std::memcpy(&dd, &ns, sizeof(double));
      return dd;
    };
    std::transform(dv.begin(), dv.end(), res.begin(), f);
    
    
    Rcpp::CharacterVector cl = Rcpp::CharacterVector::create("nanotime");
    cl.attr("package") = "nanotime";
    res.attr(".S3Class") = "integer64";
    res.attr("class") = cl;
    SET_S4_OBJECT(res);
    
    return res;
  };


  namespace traits{
  
  
    template<class Duration>  class Exporter< date::sys_time<Duration> > {
      typedef date::sys_time<Duration> OUT ;
    
      int64_t ns;
    
    public:
      Exporter(SEXP x) {
        
        
        if (Rf_inherits(x, "Date")) {
          Rcpp::Date dt(x);
          ns = static_cast<int64_t>(dt * 86400 * 1e9);
        } else if (Rf_inherits(x, "POSIXct")) {
          Rcpp::Datetime dttm(x);
          ns = static_cast<int64_t>(dttm * 1e9);
        } else if (Rf_inherits(x, "nanotime")) {
          Rcpp::NumericVector nv(x);
          std::memcpy(&ns, &(nv[0]), sizeof(double));
        } else {
          throw std::invalid_argument("Object-class must be one of 'Date', 'POSIXct', or 'nanotime'");
        }
          
      }
      OUT get() {
    
        auto dur = std::chrono::duration_cast<Duration>( std::chrono::nanoseconds(ns));
        return OUT(dur);
      }
    };
    
    
    
    template<class Duration>  class Exporter< std::vector<date::sys_time<Duration>> > {
      
      SEXP mx;
      
    public:
      Exporter(SEXP x) {
        
        if (!Rf_inherits(x, "Date") 
              && !Rf_inherits(x, "POSIXct")
              && !Rf_inherits(x, "nanotime")) {
          throw std::invalid_argument("Object-class must be one of 'Date', 'POSIXct', or 'nanotime'");
        }
        
        mx = x;
        
      }
      
      
      std::vector<date::sys_time<Duration>> get() {
        
        auto n = Rf_xlength(mx);
        std::vector<date::sys_time<Duration>> res(n);
        
        
        if (Rf_inherits(mx, "Date")) {
          auto f = [](auto d) {
            int64_t ns = static_cast<int64_t>(d * 86400 * 1e9);
            auto dur =  std::chrono::duration_cast<Duration>( std::chrono::nanoseconds(ns));
            return date::sys_time<Duration>(dur);};
          
          if (TYPEOF(mx) == INTSXP) {
            Rcpp::IntegerVector iv(mx);
            std::transform(iv.cbegin(), iv.cend(), res.begin(), f);
            
          } else {
            Rcpp::DateVector dv(mx);
            std::transform(dv.cbegin(), dv.cend(), res.begin(), f);
            
          }
          
        } else if (Rf_inherits(mx, "POSIXct")) {
          Rcpp::DatetimeVector dv(mx);
          
          std::transform(dv.cbegin(), dv.cend(), res.begin(), 
                         [](double d) {
                           int64_t ns = static_cast<int64_t>(d * 1e9);
                           auto dur = std::chrono::duration_cast<Duration>( std::chrono::nanoseconds(ns));
                           return date::sys_time<Duration>(dur);});
          
        } else if (Rf_inherits(mx, "nanotime")) {
          Rcpp::NumericVector nv(mx);
          
          res.resize(nv.size());
          std::transform(nv.cbegin(), nv.cend(), res.begin(), 
                         [](double d) {
                           int64_t ns;
                           std::memcpy(&ns, &d, sizeof(double));
                           auto dur = std::chrono::duration_cast<Duration>( std::chrono::nanoseconds(ns));
                           return date::sys_time<Duration>(dur);});
        }
        return res;
      }
    };
  
  }
}

#endif
