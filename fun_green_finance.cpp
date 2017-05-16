#include "fun_head.h"

double normT(double m, double s, double min, double max);

MODELBEGIN
EQUATION("TestInnovation")
/*
Simulates the gain of the calling firm in adopting an innovation.
Perform the same computation to compute sales (PurchaseTime) but does not record sales and returns the hypothetical sales of the calling firm

*/

v[0]=v[1]=v[2]=v[3]=v[4]=v[30]=v[31]=0;
CYCLE(cur, "ConsumerClass")
 {
  v[0]=VS(cur,"ce");
  v[1]=VS(cur,"cb");
  v[2]=VS(cur,"cg");
  v[5]=VS(cur,"alpha");
  v[3]=0;
  CYCLES(cur, cur1, "CFirm")
   { 
     v[31]++;
     if(cur1->hook!=c)
      {
       v[10]=VS(cur1->hook,"e");
       v[11]=VS(cur1->hook,"b"); 
       v[12]=VS(cur1->hook,"g"); 
       v[30]+=V_CHEAT("ComputePrice",cur1->hook);
      }
     else
      {
       v[10]=VS(cur1->hook,"e_inn");
       v[11]=VS(cur1->hook,"b_inn"); 
       v[12]=VS(cur1->hook,"g_inn"); 
       v[30]+=V_CHEAT("ComputePrice",cur1->hook);       
      }        
     v[13]=pow(v[10],v[0])*pow(v[11],v[1])*pow(v[12],v[2]);
     v[14]=pow(v[13],v[5]);
     WRITES(cur1,"cfapp",v[14]);
     v[3]+=v[14];
   }
  v[32]=v[30]/v[31]; //average price
  v[34]=VS(cur,"AvPrice");
  WRITES(cur,"AvPrice",v[32]);
  v[14]=VS(cur,"ComputeDemandSize"); 
  WRITES(cur,"AvPrice",v[34]);

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
v[21]=VS(c,"Clients");
WRITELS(c,"Clients",v[4],t);
v[22]=V_CHEAT("ComputeProfits", c);
WRITELS(c,"Clients",v[21],t);
RESULT(v[22] )


EQUATION("PurchaseTime")
/*
Variable ensuring that all consumer classes completed their purchases

*/

V("PreMarketTime");
v[0]=v[1]=v[2]=v[3]=v[4]=v[30]=v[31]=0;
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
     v[30]+=V_CHEAT("ComputePrice",cur1->hook); 
     v[31]++;    
     
   }
  WRITES(cur,"AvPrice",v[30]/v[31]); 
  v[14]=VS(cur,"ComputeDemandSize");
  WRITES(cur,"NumConsumers",v[14]);

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
Total sales
*/

v[0]=0;
CYCLE(cur, "ConsumerClass")
 {
  v[0]+=VS(cur,"ComputeDemandSize");
 }

RESULT(v[0])


EQUATION("ComputeDemandSize")
/*
Comment
*/

v[0]=V("MaxDemand");
v[1]=V("cDemand");
v[2]=V("AvPrice");
v[3]=v[0]-v[1]*v[2];
if(v[3]<0)
 v[3]=0;
RESULT(v[3] )

EQUATION("ComputeProfits")
/*
Comment
*/
v[0]=VS(c,"Clients");
v[1]=V_CHEAT("ComputePrice",c);
v[2]=VS(c,"Cost");
//v[22]=V("FinancialCosts");
v[3]=v[0]*(v[1]-v[2]);
RESULT(v[3] )

EQUATION("Profit")
/*
Comment
*/

RESULT(V("ComputeProfits") )

EQUATION("Savings")
/*
Cumulated cash from profits minus expenditures
*/

v[0]=V("Profit");
v[1]=VL("Savings",1);
RESULT(v[1]+v[0] )

EQUATION("ActionMarket")
/*
Pick one firm and let her make her move
*/
cur=RNDDRAWFAIR("Firm");

VS(cur,"ActionFirm");
v[0]=VS(cur,"IdFirm");

RESULT(v[0] )

EQUATION("ActionFirm")
/*

*/

v[0]=V("Profit");
v[1]=V("GainPrice");
if(abs(v[1])>v[0])
 {
  v[5]=4;
  v[2]=V("delta_markup");
  v[3]=V("markup");
  if(v[1]<0)
   v[4]=v[3]*(1-v[2]);
  else
   v[4]=v[3]/(1-v[2]); 
  WRITE("markup",v[4]); 
 }
else 
 v[5]=V("Innovate");

RESULT(v[5] )


EQUATION("Innovate")
/*

*/
v[0]=V("ChooseInnovation");

v[1]=V("ImproveInn");
v[2]=V("CostInn");
v[13]=v[3]=V("Cost");
v[14]=v[4]=V("b");
v[15]=v[5]=V("g");

if(v[0]==1)
 {
  v[13]=v[3]-v[1];
  v[14]=v[4]-v[2];
  v[15]=v[5]-v[2];
 }
 
if(v[0]==2)
 {
  v[13]=v[3]+v[2];
  v[14]=v[4]+v[1];
  v[15]=v[5]-v[2];
 }
if(v[0]==3)
 {
  v[13]=v[3]+v[2];
  v[14]=v[4]-v[2];
  v[15]=v[5]+v[1];
 }

WRITE("Cost",v[13]);
WRITE("b",v[14]);
WRITE("g",v[15]);

RESULT(v[0] )

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

EQUATION("ComputeE")
/*
Efficiency, computed as the complement of price normalized between max and min price

This is a function performing the computation
*/

v[0]=V_CHEAT("ComputePrice",c);
v[1]=V("MaxE");
v[2]=V("SlopeE");

v[3]=V("CenterE");
v[3]=v[1]/(1+exp(v[2]*(v[0]-v[3])));

RESULT(v[3] )

EQUATION("ComputePrice")
/*
Comment
*/
v[0]=VS(c,"Cost");
v[1]=VS(c,"markup");
v[2]=v[0]*(1+v[1]);
RESULT(v[2] )

EQUATION("MaxPrice")
/*
Comment
*/
V("PurchaseTime");
v[0]=v[1]=v[2]=0;
V("ActionMarket");

CYCLE(cur, "Firm")
 {
  if(v[0]==0)
   {v[0]=1;
   v[1]=v[2]=VS(cur,"Price");
   }
  else
   {
    v[3]=VS(cur,"Price");
    if(v[3]>v[1])
     v[1]=v[3];
    if(v[3]<v[2])
     v[2]=v[3]; 
   } 
 }

WRITE("MinPrice",v[2]);
RESULT(v[1] )

EQUATION("Price")
/*
Price
*/

v[0]=V("ComputePrice");
RESULT(v[0] )

EQUATION("e")
/*
Efficiency
*/

v[0]=V("ComputeE");
RESULT(v[0] )

EQUATION("GainPrice")
/*
Assess the gain in changing the markup level
Return 0 in case there any change provides 
*/

v[3]=VS(c,"b");
v[4]=VS(c,"g");
WRITES(c,"b_inn",v[3]);
WRITES(c,"g_inn",v[4]);

v[0]=V("delta_markup");

v[2]=VS(c,"markup");
v[5]=v[2]*(1-v[0]);
WRITES(c,"markup",v[5]);
v[6]=V_CHEAT("ComputeE",c);
WRITES(c,"e_inn",v[6]);
v[7]=V_CHEAT("TestInnovation",c);

v[5]=v[2]/(1-v[0]);
WRITES(c,"markup",v[5]);
v[6]=V_CHEAT("ComputeE",c);
WRITES(c,"e_inn",v[6]);

v[8]=V_CHEAT("TestInnovation",c);

WRITES(c,"markup",v[2]);

if(v[8]<v[7])
 v[9]=-1*v[7]; 
else
 v[9]=v[8]; 
RESULT(v[9] )


EQUATION("GaInE")
/*
Assess the gain in improving E
*/

/*
v[10]=V_CHEAT("ApplyLoanE",c);
if(v[10]==-1)
 END_EQUATION(0);
*/
v[10]=0;

v[0]=V("ImproveInn");
v[1]=V("CostInn");

v[2]=VS(c,"Cost");
v[5]=v[2]-v[0];
WRITES(c,"Cost",v[5]);
v[6]=V_CHEAT("ComputeE",c);
WRITES(c,"e_inn",v[6]);

v[3]=VS(c,"b");
v[4]=VS(c,"g");


WRITES(c,"b_inn",v[3]-v[1]);
WRITES(c,"g_inn",v[4]-v[1]);
v[5]=V_CHEAT("TestInnovation",c);
WRITES(c,"Cost",v[2]);

RESULT(v[5]-v[10] )

EQUATION("GaInB")
/*
Assess the gain in improving B
*/

/*
v[10]=V_CHEAT("ApplyLoanB",c);
if(v[10]==-1)
 END_EQUATION(0);
*/

v[10]=0;

v[0]=V("ImproveInn");
v[1]=V("CostInn");

v[2]=VS(c,"Cost");
v[5]=v[2]+v[1];
WRITES(c,"Cost",v[5]);
v[6]=V_CHEAT("ComputeE",c);
WRITES(c,"e_inn",v[6]);

v[3]=VS(c,"b");
v[4]=VS(c,"g");


WRITES(c,"b_inn",v[3]+v[0]);
WRITES(c,"g_inn",v[4]-v[1]);
v[5]=V_CHEAT("TestInnovation",c);
WRITES(c,"Cost",v[2]);

RESULT(v[5]-v[10] )

EQUATION("GaInG")
/*
Assess the gain in improving G
*/

/*
v[10]=V_CHEAT("ApplyLoanG",c);
if(v[10]==-1)
 END_EQUATION(0);
*/

v[10]=0;
v[0]=V("ImproveInn");
v[1]=V("CostInn");

v[2]=VS(c,"Cost");
v[5]=v[2]+v[1];
WRITES(c,"Cost",v[5]);
v[6]=V_CHEAT("ComputeE",c);
WRITES(c,"e_inn",v[6]);

v[3]=VS(c,"b");
v[4]=VS(c,"g");


WRITES(c,"b_inn",v[3]-v[1]);
WRITES(c,"g_inn",v[4]+v[0]);
v[5]=V_CHEAT("TestInnovation",c);
WRITES(c,"Cost",v[2]);

RESULT(v[5]-v[10] )

/*********************************************
******** FINANCIAL **********************
**********************************************/

EQUATION("ComputeRataG")
/*

                                     TA / PA
C x (1 + TA / PA)**(PA x A)   ------------------------
                               (1 + TA / PA)(PA x A) -1


C = Capitale (importo del finanziamento)
TA = Tasso annuo del finanziamento espresso in decimali (0,05 per scrivere 5%)
PA = Periodi annui, cioè il numero di rate che si pagano nell'anno (per esempio indicare 12 se il pagamento è mensile)
A = Numero di anni previsti complessivamente per il rimborso

*/
v[0]=VS(cur,"TotalLoanG");
v[1]=VS(cur,"LengthLoanG");
v[2]=VS(cur,"InterestLoanG");
v[3]=VS(cur,"PeriodsLoansG");

v[4]=v[0]*pow((1+v[2]/v[3]),(v[3]*v[1])*(v[2]/v[3])/((1+v[2]/v[3])*(v[3]*v[1] - 1)));
RESULT(v[4] )

EQUATION("ApplyLoanG")
/*
Comment
*/

v[0]=V("ComputeRataG");
v[1]=VS(c,"Profit");


RESULT(-1 )



/*********************************************
******** ACCOUNTING **********************
**********************************************/




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
v[18]=V("minCost");
v[19]=V("maxCost");
CYCLE(cur, "Firm")
  {
   WRITES(cur,"IdFirm",v[7]++);
   WRITELS(cur,"Clients",v[6], t-1);
   WRITES(cur,"Cost",UNIFORM(v[18],v[19]));
   v[21]=V_CHEAT("ComputePrice",cur);
   WRITELS(cur,"Price",v[21], t);
   v[20]=V_CHEAT("ComputeE",cur);
   WRITELS(cur,"e",v[20], t);   
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

EQUATION("ce")
/*
elasticity to efficiency. It is computed to ensure the three elasticities sum up to 1
*/
v[0]=V("cb");
v[1]=V("cg");
v[2]=1-v[0]-v[1];
PARAMETER
RESULT(v[2] )


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
