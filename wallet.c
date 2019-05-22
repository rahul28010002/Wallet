#include<stdio.h>
#include<sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


char *locDb = "wallet.db";
int loginFlag = 1,cUsernameFlag = 1, cMobFlag = 1, takenUsernameFlag = 1;
char balanceOfMainUser[20],balanceOfFriend[20];


void clear (){
    while(getchar()!='\n');
}

int callback(void *ref, int totalColumns,char **columnValue, char **columnName ){
    ref = (char *)ref;

    //printf("\n\n%s\n\n",ref);
    int i=0;
    if(!strcmp("Display!",ref)){
        for(i = 0; i < totalColumns; i++){
            printf("%s | %s \n",columnName[i], columnValue[i] ? columnValue[i] : "EMPTY" );
        }
        printf("\n");
        return 0;
    }
    if(!strcmp("check",ref)){
        //printf("\nin\n");
        loginFlag = 0;
        return 0;
    }
    if(!strcmp("checkBal",ref)){
        //printf("Your balance is Rs. %s\n",columnValue[0]);
        strcpy(balanceOfMainUser,columnValue[0]);
        //printf("Done\n");
        return 0;
    }
    if(!strcmp("add",ref)){
        printf("ACCOUNT CREATED SUCCESSFULLY!\n");
        return 0;
    }
    if(!strcmp("addM",ref)){
        printf("MONEY ADDED SUCCESSFULLY!\n");
        return 0;
    }
    if(!strcmp("checkU",ref)){
        cUsernameFlag = 0;
        return 0;
    }
    if(!strcmp("checkM",ref)){
        cMobFlag = 0;
        return 0;
    }
    if(!strcmp("checkFbal",ref)){
        strcpy(balanceOfFriend,columnValue[0]);
        //printf("Bal F : %s\n\n",balanceOfFriend);
        //printf("Done\n");
        return 0;
    }
    if(!strcmp("update",ref)){
        return 0;
    }
}

void query(char *doThis,void *inRef){
    sqlite3* db;
    int sqlExec, openDB;
    char *error;
    //printf("commands are : \n%s\n%s\n\n",doThis,locDb);
    openDB = sqlite3_open(locDb,&db);
    //printf("Db open code %d\n",openDB);
    sqlExec = sqlite3_exec(db,doThis,callback,inRef,&error);
    //printf("Exec code is %d\n",sqlExec);
    //printf("\n\n%s\n\n",error);
    if(sqlExec == 19){
        takenUsernameFlag = 0;
    }
    sqlite3_free(error);
    sqlite3_close(db);
}

int login(char *username, char *password){
    loginFlag = 1;
    char login[200];
    sprintf(login,"SELECT * FROM accinfo WHERE username = '%s' AND password = '%s';",username,password);
    query(login,"check");
    if(loginFlag == 0){
        return 0;
    }
    else{
        return 1;
    }
}

void signUp(){
    char mob[11],name[30],surname[30],username[30],password[30],secQ[100],answer[20],accinfo[1000],security[1000],amount[200],transaction[1000];
    time_t t;
    struct tm *local;

    printf("ENTER FIRST NAME :");
    gets(name);
    printf("ENTER SURNAME :");
    gets(surname);
    printf("ENTER USERNAME :");
    gets(username);
    printf("ENTER PASSWORD :");
    gets(password);
    printf("ENTER MOBILE NUMBER : ");
    gets(mob);
    printf("ENTER SECURITY QUESTION : ");
    gets(secQ);
    printf("ENTER ANSWER :");
    gets(answer);
    sprintf(accinfo,"INSERT INTO accinfo (name, surname, username, password, mob) VALUES ('%s', '%s', '%s', '%s', '%s')",name,surname,username,password,mob);
    sprintf(security,"INSERT INTO security (username, question, answer) VALUES ('%s', '%s', '%s')",username, secQ, answer);
    sprintf(amount,"INSERT INTO amount (username,mob) VALUES ('%s','%s')",username,mob);
    t=time(NULL);
    local = localtime(&t);
    sprintf(transaction,"INSERT INTO transactions (username, type, time, date) VALUES ('%s','Account created.','%d:%02d:%02d','%d-%d-%d');",
    username,local->tm_hour,local->tm_min,local->tm_sec,local->tm_mday,local->tm_mon+1,local->tm_year+1900);
    query(accinfo,"add");
    if(takenUsernameFlag == 0){
        takenUsernameFlag = 1;
        system("clear");
        printf("\n\nTHE ENTERED USERNAME IS TAKEN, PLEASE TRY AGAIN.\n\n\n");
        return;
    }
    query(security,"add");
    query(amount,"add");
    query(transaction,"add");
    //clear();
}

int checkMobNo(char *mobileNo1){
    char checkMob[200];
    sprintf(checkMob,"SELECT mob FROM accinfo WHERE mob = '%s';",mobileNo1);
    query(checkMob,"checkM");
    if(cMobFlag==0){
        cMobFlag = 1;
        system("clear");
                printf("ENTERED MOBILE NUMBER EXISTS.\n" );
        return 0;

    }
    else{
	system("clear");
        printf("ENTERED MOBILE NUMBER DOES NOT EXIST.\n" );
        return 1;
    }
}

int checkUsername(char *username1){
    char checkUsername[200];
    sprintf(checkUsername,"SELECT username FROM accinfo WHERE username = '%s';",username1);
    query(checkUsername,"checkU");
    if(cUsernameFlag==0){
        cUsernameFlag = 1;
        system("clear");
        printf("ENTERED USERNAME EXISTS.\n" );
        return 0;

    }
    else{
	system("clear");
        printf("ENTERED USERNAME DOES NOT EXIST.\n" );
        return 1;
    }
}


void afterLogin(char *username){
    char balance[200],addMoney[200], transaction[1000],username1[30],mobileNo1[11],transaction1[1000],removeMoney[200], viewTransaction[200];
    int choice,myBal,moneyToAdd,totalBalMainUser,friendBal,totalFriendBal, mFlag=1, uFlag=1, choice1, signoutFlag = 1;
    time_t t;
    struct tm *local;
    while(1){
        sprintf(balance, "SELECT balance FROM amount WHERE username = '%s';",username);
        query(balance, "checkBal");
        myBal = atoi(balanceOfMainUser);
        printf("1. VIEW BALANCE\n2. ADD MONEY\n3. SEND MONEY TO FRIEND\n4. VIEW TRANSACTIONS\n5. SIGN OUT\n");
        printf("ENTER CHOICE : ");
        scanf("%d",&choice);
        switch (choice) {
            case 1:
		system("clear");
                printf("\n\nYOUR BALANCE IS RS. %d\n\n",myBal);
                break;
            case 2:
            system("clear");
                printf("HOW MUCH MONEY DO YOU WANT TO ADD : ");
                scanf("%d",&moneyToAdd);
                if(moneyToAdd < 0){
                    printf("\nYOU CANNOT ADD NEGATIVE AMOUNT.\n\n");
                    break;
                }
                totalBalMainUser = myBal + moneyToAdd;
                sprintf(addMoney,"UPDATE amount SET balance = %d WHERE username = '%s';",totalBalMainUser, username);
                t=time(NULL);
                local = localtime(&t);
                sprintf(transaction,"INSERT INTO transactions (username, type, time, date) VALUES ('%s','Added money Rs. %d.','%d:%02d:%02d','%d-%d-%d');",
                username,moneyToAdd,local->tm_hour,local->tm_min,local->tm_sec,local->tm_mday,local->tm_mon+1,local->tm_year+1900);
                query(addMoney,"update");
                query(balance,"checkBal");
                query(transaction,"update");
                myBal = atoi(balanceOfMainUser);
                break;
            case 3:
		system("clear");
                printf("SEND MONEY BY :\n1. USERNAME\n2. MOBILE NO.\n3. EXIT\nENTER CHOICE : ");
                scanf("%d",&choice1);
                clear();
                switch (choice1) {
                    case 1:
                    	system("clear");
                        printf("ENTER USERNAME : " );
                        gets(username1);
                        uFlag = checkUsername(username1);
                        if(uFlag == 0){
                            sprintf(balance,"SELECT balance FROM amount WHERE username = '%s';",username1);
                            query(balance, "checkFbal");
                            friendBal = atoi(balanceOfFriend);
                        }
                        break;
                    case 2:
                    	system("clear");
                        printf("ENTER MOBILE NUMBER : ");
                        gets(mobileNo1);
                        mFlag = checkMobNo(mobileNo1);
                        if(mFlag == 0){
                            sprintf(balance,"SELECT balance FROM amount WHERE mob = '%s';",mobileNo1);
                            query(balance,"checkFbal");
                            friendBal = atoi(balanceOfFriend);
                        }
                        break;
                    case 3:
                    	system("clear");
                        break;
                    default:
                    	system("clear");
                        printf("WRONG INPUT.\n");

                }
                if(mFlag == 0 || uFlag == 0){
                	system("clear");
                    printf("ENTER THE AMOUNT YOU WANT TO SEND : ");
                    scanf("%d",&moneyToAdd);
                    if(moneyToAdd > myBal){
                    	system("clear");
                        printf("\n\nINSUFFICIENT BALANCE.\n\n");
                        break;
                    }
                    totalFriendBal = friendBal + moneyToAdd;
                    totalBalMainUser = myBal - moneyToAdd;
                    if(choice1 == 1)
                        sprintf(addMoney,"UPDATE amount SET balance = %d WHERE username = '%s';",totalFriendBal,username1);
                    else if(choice1 == 2 )
                        sprintf(addMoney,"UPDATE amount SET balance = %d WHERE mob = '%s';",totalFriendBal,mobileNo1);


                    sprintf(removeMoney,"UPDATE amount SET balance = %d WHERE username = '%s';",totalBalMainUser,username);
                    t=time(NULL);
                    local = localtime(&t);

                    sprintf(transaction,"INSERT INTO transactions (username, type, time, date) VALUES ('%s','Sent Rs. %d to %s.','%d:%02d:%02d','%d-%d-%d');",
                    username,moneyToAdd,username1,local->tm_hour,local->tm_min,local->tm_sec,local->tm_mday,local->tm_mon+1,local->tm_year+1900);

                    sprintf(transaction1,"INSERT INTO transactions (username, type, time, date) VALUES ('%s','Received Rs. %d from %s.','%d:%02d:%02d','%d-%d-%d');",
                    username1,moneyToAdd,username,local->tm_hour,local->tm_min,local->tm_sec,local->tm_mday,local->tm_mon+1,local->tm_year+1900);

                    query(addMoney,"update");
                    query(removeMoney,"update");
                    query(transaction,"update");
                    query(transaction1,"update");
                    mFlag = 1;
                    uFlag = 1;

                }
                break;
            case 4:
                sprintf(viewTransaction,"SELECT type,date,time FROM transactions WHERE username = '%s';",username);
                query(viewTransaction,"Display!");
                break;
            case 5:
                signoutFlag = 0;
                break;
            default:
            	system("clear");
                printf("WRONG INPUT!\n\n");

        }
        if(signoutFlag == 0){
            signoutFlag = 1;
            break;
        }
    }
}


void main(){
    char username[30],password[30];
    int flag,choice, close = 1;
    system("clear");
    printf("***********************WALLET*************************\n");

    while(1){
        printf("1. SIGN UP / CREATE AN ACCOUNT\n2. LOGIN\n3. EXIT\n");
        printf("ENTER YOUR CHOICE : ");
        scanf("%d",&choice);
        clear();
        switch (choice) {
            case 1:
                signUp();
                system("clear");
                break;
            case 2:
                printf("ENTER USERNAME :");
                gets(username);
                printf("ENTER PASSWORD :");
                gets(password);
                flag = login(username,password);
                system("clear");
                if(flag == 0){
                    printf("LOGIN SUCCESSFUL!\n");
                    afterLogin(username);
                }
                else printf("LOGIN UNSUCCESSFUL!\n");
                system("clear");
                break;
            case 3:
                close = 0;
                system("clear");
                break;
            default:
                printf("WRONG INPUT.\n" );
        }
        if(close == 0)  break;
    }
}
