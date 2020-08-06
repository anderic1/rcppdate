
// [[Rcpp::depends(RcppDate)]]
// [[Rcpp::plugins(cpp14)]]

#include <RcppDate.h>



using sys_ms = date::sys_time<std::chrono::microseconds>;
using sys_ns = date::sys_time<std::chrono::nanoseconds>;

// [[Rcpp::export]]
date::sys_days sysDays(date::sys_days dt) {
  
  return dt;
}
/*** R
sysDays(as.Date("2020-08-01"))
sysDays(as.POSIXct("2020-08-01 00:00", tz="Europe/Paris")) #conversion happens at the UTC time point, as in R
*/

// [[Rcpp::export]]
sys_ms mSecs(sys_ms dttm) {
  
  return dttm;
}
/*** R
mSecs(as.POSIXct("2020-08-01 13:14:15.123", tzone="Europe/Paris"))
mSecs(as.Date("2020-08-01"))
*/


// [[Rcpp::export]]
sys_ns nanos(sys_ns dttm_ns) {
  
  return dttm_ns;
}
/*** R
if (require("nanotime")) {
  nanos(nanotime("2020-08-01T13:14:15.123456789Z"))
}
*/



// [[Rcpp::export]]
std::vector<date::sys_days> todayv(const std::vector<date::sys_days>& dt) {

  return dt;
}
/*** R
todayv(Sys.Date()+1:3)
todayv(Sys.time()+1:3)
*/

// [[Rcpp::export]]
std::vector<sys_ms> nowv(const std::vector<sys_ms>& dttm) {
  
  return dttm;
}
/*** R
nowv(Sys.time()+1:3)
nowv(Sys.Date()+1:3)

*/


// [[Rcpp::export]]
std::vector<sys_ns> now_nsv(const std::vector<sys_ns>& dttm_ns) {

  return dttm_ns;
}
/*** R
if (require("nanotime")) {
  print(now_nsv(nanotime(Sys.time())+1:2))
  now_nsv(Sys.time()+1:2)
}
*/



