#include "fun_head.h"

double normT(double m, double s, double min, double max);

MODELBEGIN

EQUATION("PurchaseTime")
/*
Variable ensuring that all consumer classes completed their purchases

*/

V("PreMarketTime");
v[0]=v[1]=v[2]=v[3]=v[4]=0;
CYCLE(cur, "ConsumerClass")
 {
  v[0]=VS(cur,"ce");
  v[1]=VS(cur,"cb");
  v[2]=VS(cur,"cg");
  v[5]=VS(cur,"alpha");
  v[3]=0;
  CYCLES(cur, cur1, "CFirm")
   {
     v[10]=VS(cur1->hook,"e");
     v[11]=VS(cur1->hook,"b"); 
     v[12]=VS(cur1->hook,"g"); 
     v[13]=pow(v[10],v[0])*pow(v[11],v[1])*pow(v[12],v[2]);
     v[14]=pow(v[13],v[5]);
     WRITES(cur1,"cfapp",v[14]);
     v[3]+=v[14];
   }
  v[4]=VS(cur,"Income");

  CYCLES(cur, cur1, "CFirm")
   {
    v[6]=VS(cur1,"cfapp");
    v[7]=v[4]*v[6]/v[3];
    INCRS(cur1->hook,"Sales",v[7]);
    WRITES(cur1,"msC",v[6]/v[3]);
   }  
  }
RESULT(1 )

EQUATION("PreMarketTime")
/*
Variable preparing firms to receive information about consumers' choices. Must be computed before consumers can perform their purchases
*/
CYCLE(cur, "Firm")
 {
  WRITES(cur,"Sales",0);
 }

RESULT(1 )


EQUATION("Clients")
/*
Number of consumers currently owning the product of the firm. All the job is done in IdUsed, so the equation simply add new costomers and subtract lost ones. The only requirement is that the computation is done after PurchaseTime is computed.
*/
V("PurchaseTime");
v[0]=VL("Clients",1);
v[1]=V("Sales");

v[3]=V("sClients");
v[2]=v[0]*v[3]+v[1]*(1-v[3]);
RESULT(v[2] )

EQUATION("ms")
/*
Market shares
*/
v[0]=V("Clients");
v[1]=V("TotClients");
RESULT(v[0]/v[1] )

EQUATION("TotClients")
/*
Total income spent
*/

RESULT(SUM("Income") )

EQUATION("InitMarket")
/*
Initialization of the market. 
1) generates the set of consumers assigning a fake set of products used in the past using the lagged market shares as probabilities.
2) generates the social network structure
*/

v[0]=V("TotClients");
cur1=SEARCH("Demand");
v[2]=1;
v[1]=V("numFirm");
CYCLES(cur1, cur, "ConsumerClass")
 {
  WRITES(cur,"IdConsumer",v[2]++);
  ADDNOBJS(cur,"CFirm",v[1]-1);
  v[3]=1;
  CYCLES(cur, cur5, "CFirm")
   {
    cur2=SEARCH_CND("IdFirm",v[3]++);
    cur5->hook=cur2;
   }
 } 
v[4]=V("TotClients");
v[5]=V("numFirm");
v[6]=v[4]/v[5];
CYCLE(cur, "Firm")
 {
  WRITELS(cur,"Clients",v[6], t-1);
 }
  
PARAMETER   

RESULT(1)

EQUATION("numFirm")
/*
Number of firms
*/
v[0]=0;
CYCLE(cur, "Firm")
 {
  v[0]++;
 }
PARAMETER
RESULT(v[0] )

MODELEND




void close_sim(void)
{

}



double normT(double m, double s, double min, double max)
{
int i;
double x;

for(i=0; i<1000; i++)
 {
  x=norm(m, s);
  if(x>min && x<max)
   return x; 
 }
plog("\nFailed truncated normal draw\n");
return m; 
}
