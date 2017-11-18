#include "fun_head.h"

double normT(double m, double s, double min, double max);

MODELBEGIN


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
//     v[13]=pow(v[10],v[0])*pow(v[11],v[1])*pow(v[12],v[2]);
     v[13]=v[10]*v[0]+v[11]*v[1]+v[12]*v[2];
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

EQUATION("AvProfit")
/*
Comment
*/

v[0]=V("Savings");
v[1]=V("Age");
RESULT(v[0]/v[1] )

EQUATION("ActionMarket")
/*
Pick one firm and let her make her move
*/

CYCLE(cur, "Firm")
 {
  VS(cur,"ActionFirm");
 }

RESULT(v[0] )

EQUATION("EntryExit")
/*
Comment
*/V("ActionMarket");

v[3]=v[4]=v[5]=0;
cur1=SEARCH("Supply");
v[30]=V("thresholdExit");
v[31]=V("minAgeExit");

CYCLE_SAFES(cur1, cur, "Firm")
 {
  v[0]=VS(cur,"ActionFirm");
  if(v[0]==0)
   {
    v[1]=VS(cur,"ms");
    if(v[1]<v[30] && VS(cur,"Age")>v[31])
     {
      CYCLE(cur3, "ConsumerClass")
       {
        CYCLE2_SAFES(cur3, cur5, "CFirm")
         {
          if(cur5->hook==cur)
           DELETE(cur5);
         }
       }
      DELETE(cur);
      v[3]++;
     } 
   }
 }

WRITE("numExit",v[3]);
v[0]=V("probEntry");
if(RND<v[0])
 {v[4]=1;
 v[21]=V("ShareValuesNewEntrants");
  v[7]=V("IssueID");
  cur=ADDOBJS(cur1,"Firm");
  WRITES(cur,"IdFirm",v[7]);  
  WRITES(cur,"b",V("Avb")*v[21]);   
  WRITES(cur,"g",V("Avg")*v[21]);    
  WRITES(cur,"Cost",V("AvCost")*v[21]);
  WRITES(cur,"markup",V("Avmup"));  
  WRITELS(cur,"Price",V_CHEAT("ComputePrice",cur),t);

  WRITES(cur,"e",V_CHEAT("ComputeE",c));  

  WRITES(cur,"InnType",0);   
   v[21]=RND; v[22]=RND; v[23]=RND;
   v[24]=v[21]+v[22]+v[23];
   WRITES(cur,"Pe",v[21]/v[24]);
   WRITES(cur,"Pg",v[22]/v[24]);
   WRITES(cur,"Pb",v[23]/v[24]); 
   WRITES(cur,"ms",0); 
  CYCLE(cur2, "ConsumerClass")
   {
    cur2=ADDOBJS(cur2,"CFirm");
    cur2->hook=cur;
   }
     
 }
 
v[9]=INCRS(cur1,"numFirm",v[4]-v[3]);

RESULT(v[9] )

EQUATION("BidLoan")
/*
The caller ask for a loan in order to:
1) reduce cost
2) increase brown q.
3) increase green q.
*/

v[0]=VS(c,"InnType");
if(v[0]==1)
 v[1]=V("ProbLoanCost");
if(v[0]==2)
 v[1]=V("ProbLoanBrown");
if(v[0]==3)
 v[1]=V("ProbLoanGreen");

if(RND>v[1])
 END_EQUATION(0);
if(v[0]==1)
 v[3]=V("LengthCost");
if(v[0]==2)
 v[3]=V("LengthBrown");
if(v[0]==3)
 v[3]=V("LengthGreen");
   

RESULT(v[3] )

EQUATION("ActionFirm")
/*
Choose randomly which innovation to perform
*/

v[0]=VL("ActionFirm",1);
if(v[0]>0)
 END_EQUATION(v[0]-1)

v[1]=V("InnType");
if(v[1]==0)
  { 
  v[20]=V("Pe");
  v[21]=V("Pb");
  v[22]=V("Pg");
  
  v[23]=RND;
  if(v[23]<v[20])
   v[0]=1;
  else
   {
    if(v[23]<v[20]+v[21])
      v[0]=2;
    else
      v[0]=3;  
   }
  WRITE("InnType",v[0]);  
  v[10]=V("BidLoan");
  if(v[10]==0)
   WRITE("InnType",0);
  END_EQUATION(v[10]);
 }  

if(v[1]==1)
 v[6]=V("ProbSuccCost");
if(v[1]==2)
 v[6]=V("ProbSuccBrown");
if(v[1]==3)
 v[6]=V("ProbSuccGreen");

if(RND>v[6])//Innovation failed
 {
  WRITE("InnType",0);
  END_EQUATION(0);
 } 
 
v[11]=V("ImproveInn");
v[2]=V("CostInn");
v[13]=v[3]=V("Cost");
v[14]=v[4]=V("b");
v[15]=v[5]=V("g");

if(v[1]==1)
 {
  v[13]=v[3]-v[11];
  if(v[13]<=0) v[13]=0.01;
  v[14]=v[4]-v[2];
  v[15]=v[5]-v[2];
 }
 
if(v[1]==2)
 {
  v[13]=v[3]+v[2];
  v[14]=v[4]+v[11];
  v[15]=v[5]-v[2];
 }
if(v[1]==3)
 {
  v[13]=v[3]+v[2];
  v[14]=v[4]-v[2];
  v[15]=v[5]+v[11];
 }


WRITE("Cost",v[13]<0.0001?0.0001:v[13]);
WRITE("b",v[14]<0.0001?0.0001:v[14]);
WRITE("g",v[15]<0.0001?0.0001:v[15]);
WRITE("InnType",0);
RESULT(0)

EQUATION("Age")
/*
Comment
*/

RESULT(CURRENT+1 )


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
v[0]=v[1]=v[2]=v[5]=v[6]=v[7]=v[8]=v[9]=v[10]=v[11]=v[12]=v[13]=v[14]=v[15]=0;
V("ActionMarket");


v[1]=v[2]=V("Price");

CYCLE(cur, "Firm")
 {v[0]++;
  
  v[4]=VS(cur,"ms");
  v[11]+=VS(cur,"Age")*v[4];
  v[5]+=VS(cur,"Cost")*v[4];
  v[6]+=VS(cur,"b")*v[4];
  v[7]+=VS(cur,"g")*v[4];
  v[8]+=VS(cur,"e")*v[4];
  v[12]+=VS(cur,"Pb")*v[4];
  v[13]+=VS(cur,"Pg")*v[4];
  v[14]+=VS(cur,"Pe")*v[4];
  v[15]+=VS(cur,"AvProfit")*v[4];
  v[9]+=VS(cur,"markup")*v[4];
  v[10]+=VS(cur,"Price")*v[4]; 
    v[3]=VS(cur,"Price")*v[4];
    if(v[3]>v[1])
     v[1]=v[3];
    if(v[3]<v[2])
     v[2]=v[3]; 
 }

WRITE("AvAge",v[11]);
WRITE("MinPrice",v[2]);
WRITE("AvCost",v[5]);
WRITE("Avb",v[6]);
WRITE("Avg",v[7]);
WRITE("Ave",v[8]);
WRITE("AvPb",v[12]);
WRITE("AvPg",v[13]);
WRITE("AvPe",v[14]);
WRITE("AvAvProfit",v[15]);
WRITE("Avmup",v[9]);
WRITE("AvP",v[10]);

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

EQUATION("IssueID")
/*
Comment
*/

RESULT(CURRENT+1 )

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
   v[7]=V("IssueID");
   WRITES(cur,"IdFirm",v[7]);
   WRITELS(cur,"Clients",v[6], t-1);
   WRITES(cur,"Cost",UNIFORM(v[18],v[19]));
   v[21]=V_CHEAT("ComputePrice",cur);
   WRITELS(cur,"Price",v[21], t);
   v[20]=V_CHEAT("ComputeE",cur);
   WRITELS(cur,"e",v[20], t);   
   WRITES(cur,"g",UNIFORM(v[8],v[9]));
   WRITES(cur,"b",UNIFORM(v[8],v[9]));
   v[21]=RND; v[22]=RND; v[23]=RND;
   v[24]=v[21]+v[22]+v[23];
   WRITES(cur,"Pe",v[21]/v[24]);
   WRITES(cur,"Pg",v[22]/v[24]);
   WRITES(cur,"Pb",v[23]/v[24]);   
   WRITELS(cur,"ms",1/v[5], t-1);
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
