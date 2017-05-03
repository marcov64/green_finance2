#include "fun_head.h"

double normT(double m, double s, double min, double max);

MODELBEGIN
EQUATION("TestInnovation")
/*
Simulates the gain of the calling firm in adopting an innovation.
Perform the same computation to compute sales (PurchaseTime) but does not record sales and returns the hypothetical sales of the calling firm

*/

v[0]=v[1]=v[2]=v[3]=v[4]=0;
CYCLE(cur, "ConsumerClass")
 {
  v[0]=VS(cur,"ce");
  v[1]=VS(cur,"cb");
  v[2]=VS(cur,"cg");
  v[5]=VS(cur,"alpha");
  v[3]=0;
  CYCLES(cur, cur1, "CFirm")
   { if(cur1->hook!=c)
      {
       v[10]=VS(cur1->hook,"e");
       v[11]=VS(cur1->hook,"b"); 
       v[12]=VS(cur1->hook,"g"); 
      }
     else
      {
       v[10]=VS(cur1->hook,"e_inn");
       v[11]=VS(cur1->hook,"b_inn"); 
       v[12]=VS(cur1->hook,"g_inn"); 
      }        
     v[13]=pow(v[10],v[0])*pow(v[11],v[1])*pow(v[12],v[2]);
     v[14]=pow(v[13],v[5]);
     WRITES(cur1,"cfapp",v[14]);
     v[3]+=v[14];
   }
  v[14]=VS(cur,"Income");

  CYCLES(cur, cur1, "CFirm")
   {
    if(cur1->hook==c)
     {
      v[6]=VS(cur1,"cfapp");
      v[7]=v[14]*v[6]/v[3];
      v[4]+=v[7];
     } 
   }  
  }
RESULT(v[4] )


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
  v[14]=VS(cur,"Income");

  CYCLES(cur, cur1, "CFirm")
   {
    v[6]=VS(cur1,"cfapp");
    v[7]=v[14]*v[6]/v[3];
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

EQUATION("ChooseInnovation")
/*
Verify which innovation performs best
*/
V("PurchaseTime");

v[1]=V("GaInB");
v[2]=V("GaInG");
v[0]=V("GaInE");

v[3]=max(v[1],max(v[0],v[2]));

v[6]=V("Sales");
if(v[6]>v[3])
 END_EQUATION(0);

if(v[3]==v[0])
 END_EQUATION(1) 

if(v[3]==v[1])
 END_EQUATION(2) 

if(v[3]==v[2])
 END_EQUATION(3) 

RESULT(-1 )

EQUATION("GaInE")
/*
Assess the gain in improving E
*/

v[0]=V("ImproveInn");
v[1]=V("CostInn");
v[2]=VS(c,"e");
v[3]=VS(c,"b");
v[4]=VS(c,"g");

WRITES(c,"e_inn",v[2]+v[0]);
WRITES(c,"b_inn",v[3]-v[1]);
WRITES(c,"g_inn",v[4]-v[1]);
v[5]=V_CHEAT("TestInnovation",c);
RESULT(v[5] )

EQUATION("GaInB")
/*
Assess the gain in improving B
*/

v[0]=V("ImproveInn");
v[1]=V("CostInn");
v[2]=VS(c,"e");
v[3]=VS(c,"b");
v[4]=VS(c,"g");

WRITES(c,"e_inn",v[2]-v[1]);
WRITES(c,"b_inn",v[3]+v[0]);
WRITES(c,"g_inn",v[4]-v[1]);
v[5]=V_CHEAT("TestInnovation",c);
RESULT(v[5] )

EQUATION("GaInG")
/*
Assess the gain in improving E
*/

v[0]=V("ImproveInn");
v[1]=V("CostInn");
v[2]=VS(c,"e");
v[3]=VS(c,"b");
v[4]=VS(c,"g");

WRITES(c,"e_inn",v[2]-v[1]);
WRITES(c,"b_inn",v[3]-v[1]);
WRITES(c,"g_inn",v[4]+v[0]);
v[5]=V_CHEAT("TestInnovation",c);
RESULT(v[5] )

/*********************************************
******** INITIALIZATION **********************
**********************************************/

EQUATION("InitMarket")
/*
Initialization of the market. 
1) generates the set of consumers assigning a fake set of products used in the past using the lagged market shares as probabilities.
2) generates the social network structure
*/


v[4]=V("TotClients");
v[5]=V("numFirm");
v[6]=v[4]/v[5];
cur=SEARCH("Supply");
ADDNOBJS(cur,"Firm",v[5]-1);
v[7]=1;
v[8]=V("min");
v[9]=V("max");
CYCLE(cur, "Firm")
  {
   WRITES(cur,"IdFirm",v[7]++);
   WRITELS(cur,"Clients",v[6], t-1);
   WRITES(cur,"e",UNIFORM(v[8],v[9]));
   WRITES(cur,"g",UNIFORM(v[8],v[9]));
   WRITES(cur,"b",UNIFORM(v[8],v[9]));
   
  }
cur1=SEARCH("Demand");
v[2]=1;
CYCLES(cur1, cur, "ConsumerClass")
 {
  WRITES(cur,"IdConsumer",v[2]++);
  ADDNOBJS(cur,"CFirm",v[5]-1);
  v[3]=1;
  CYCLES(cur, cur5, "CFirm")
   {
    cur2=SEARCH_CND("IdFirm",v[3]++);
    cur5->hook=cur2;
   }
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
