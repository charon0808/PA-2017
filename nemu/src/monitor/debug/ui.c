#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "memory.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_info(char *args) {
	if (args[0] == 'r') {
		printf("register    Hexadecimal          Decimal\n");
		printf("  eax        0x%x        %d\n", cpu.eax, cpu.eax);
		printf("  edx        0x%x        %d\n", cpu.edx, cpu.edx);
		printf("  ecx        0x%x        %d\n", cpu.ecx, cpu.ecx);
		printf("  ebx        0x%x        %d\n", cpu.ebx, cpu.ebx);
		printf("  ebp        0x%x        %u\n", cpu.ebp, cpu.ebp);
		printf("  esi        0x%x        %d\n", cpu.esi, cpu.esi);
		printf("  edi        0x%x        %d\n", cpu.edi, cpu.edi);
		printf("  esp        0x%x        %u\n", cpu.esp, cpu.esp);
        printf("EFLAGS OF:%x  IF:%x  SF:%x  ZF:%x  CF:%x\n",cpu.EFLAGS.OF,cpu.EFLAGS.IF,cpu.EFLAGS.SF,cpu.EFLAGS.ZF,cpu.EFLAGS.CF);
	}
    else if (args[0]=='w'){
        print_wp();
    }
	else {
		printf("%s\n", "No Such Commands Found");
	}
	return 0;
}

static int cmd_x(char *args)
{
	int len = strlen(args);
	int m = 1, place = 0;
	int i;
	for (i = len - 1; i >= 0; i--) {
		if (args[i] == ' ') {
			continue;
		}
		if (args[i] == 'x') {
			break;
		}
        if (args[i]<='9'&&args[i]>='0')
		    place+=(args[i]-48)*m;
        else if (args[i]<='f'&&args[i]>='a')
            place+=(args[i]-'a'+10)*m;
        else place+=(args[i]-'A'+10)*m;
		m *= 16;
	}
	int num = 0;
	m = 1;
	for (i -= 2; i >= 0; i--) {
		if (args[i] == ' ') {
			continue;
		}
		num += (args[i] - 48)*m;
		m *= 10;
	}
	printf("Hexadecimal    Decimal\n");
	for (i = 0; i < num; i++) {
		//intptr_t p=(intptr_t)place;
		printf("0x%x     %d\n", vaddr_read(place, 4), vaddr_read(place, 4));
		place += 4;
	}
	return 0;
}

int match(int p,int q, char* args)
{
	int i = p + 1;
	while (args[i] != ')'&&i<=q) {
		if (args[i] == '(') {
			i = match(i,q, args);
		}
		i++;
	}
	if (i == p + 1&&i>q) {
		assert(0);
    }
	return i;
}



int eval(int p, int q, char* args,bool *success) {
    int len=strlen(args);
    if (args[p]=='*'){
        args[p]='@';
    }
    else if (args[p]=='-'){
        args[p]='Z';
    }
    for (int i=1;i<len;i++){ // to find the pointer '*' and replace it by '@'
        if (args[i]=='*'||args[i]=='-'){
            int k=i-1;
            while (args[k]==' '){
                k--;
            }
            if (args[k]=='+'||args[k]=='-'||args[k]=='*'||args[k]=='/'||args[k]=='='||args[k]=='&'||args[k]=='|'||args[k]=='('){
                if (args[i]=='*')
                   args[i]='@';
                else args[i]='Z';
            }
    }
    }
	if (p > q) {
		/* Bad expression */
		*success=false;
        return 0;
	}
	else if (p == q) {
		/* Single token.
		*      * For now this token should be a number.
		*           * Return the value of the number.
		*                */
        if (args[p]<='9'&&args[p]>='0'){
	      	return args[p] - 48;
        }  
        else {
            *success=false;
            return 0;
        }
	}
	else if (args[p] == '('&&match(p,q, args) == q) {
		/* The expression is surrounded by a matched pair of parentheses.
		*      * If that is the case, just throw away the parentheses.
		*           */
		return eval(p + 1, q - 1, args,success);

	}
	else {
		//int mark3=-1,mark2=-1,mark1 = -1, mark0 = -1;
        int mark[6]={-1,-1,-1,-1,-1,-1};
		int op = -1;
		for (int i = p; i <= q; i++) {
			if (args[i] == ' ') {
				continue;
			}
			if (args[i] == '(') {
				i = match(i,q, args);
				continue;
			}
			/* '*' and '/' -->1, '+' and '-' -->0 */
             //   if (args[i]=='!'&&args[i+1]!='='){
              //      mark[0]=i;
              //  }
			if (args[i] == '*' || args[i] == '/') {
				mark[1] = i;
			}
			else if (args[i] == '+' || args[i] == '-') {
				mark[2] = i;
			} 
            else if(args[i]=='='){
                mark[3]=i;
             }
                else if (args[i]=='&'){
                    mark[4]=i;
                }
                else if (args[i]=='|'){
                    mark[5]=i;
                }
		}/*
        if (mark2!=-1){
            op=mark2;
        }
		else if (mark1 != -1) {
			op = mark1;
		}
		else if (mark0 != -1) {
			op = mark0;
		}*/
                for (int i=5;i>=0;i--){
                    if (mark[i]!=-1){
                        op=mark[i];
                        break;
                    }
                } 
		if (op==-1) {
			if (args[p] == '$') {
				char my_reg[3] = "0";
				char* r[][8] = { { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" },{ "ax","cx","dx","bx","sp","bp","si","di" },{ "al","cl","dl","bl","ah","ch","dh","bh" } };
				my_reg[0] = args[p + 1];
				my_reg[1] = args[p + 2];
				if ((args[p + 3] <= 'Z' && args[p + 3] >= 'A') || (args[p + 3] <= 'z' && args[p+3] >= 'a')) {
                    					my_reg[2] = args[p + 3];
				}
				int s1, s2, flag = 0;
				for (s1 = 0; s1 < 3; s1++) {
					for (s2 = 0; s2 < 8; s2++) {
						if (r[s1][s2][0]==my_reg[0]&&r[s1][s2][1]==my_reg[1]) {
							flag = 1;
							break;
						}
					}
					if (flag) {
						break;
					}
				}
				if (s1 == 0) {
					return cpu.gpr[s2]._32;
				}
				else if (s1 == 1) {
					return cpu.gpr[s2]._16;
				}
				else return cpu.gpr[s2]._8[1];

			}
            char lx_pointer;
            if (args[p]=='@'){    //match pointer '*'
                lx_pointer='@';
                return vaddr_read(eval(p+1,q,args,success),4);
            }
            else if (args[p]=='!'){   //match the not '!'
                lx_pointer='!';
                return !eval(p+1,q,args,success);
            }
            else if (args[p]=='Z'){   //match the negative number '-'
                lx_pointer='Z';
                return -eval(p+1,q,args,success);
            }
            else lx_pointer=0;     // match the pure number
            if (args[p]=='('){
                //p++;
             //   q--;
                if (q==match(p,q,args)){
                    return eval(p-1,q-1,args,success);
                }
                else {
                    *success=false;
                    return 0;
                }
            }
            int pox=10;
            if (args[p]=='0'&&args[p+1]=='x'){
               // int i=p+2;
               // while((args[i]<='9'&&args[i]>='0')||(args[i]<='F'&&args[i]>='A')||(args[i]<='f'&&args[i]>='a')){
               //     i++;
               // }
               // q=i-1;
                p+=2;
                pox=16;
            }
			int power = 1, num = 0;
            for (int k=q;k>=p;k--){
                if (args[k]<='9'&&args[k]>='0'){
                    args[k]-='0';
                }
                else if (args[k]<='F'&&args[k]>='A'){
                    args[k]-=55;
                }
                else if (args[k]<='f'&&args[k]>='a'){
                    args[k]-=87;
                }
                else {
                    *success=false ;
                    return 0;
                }
            }
			for (int k = q; k >= p; k--) {
				if (args[k] == ' ') {
					continue;
				}
				num += args[k]*power;
				power *= pox;
			}
            if (lx_pointer==0)
			   return num;
        //    else if (lx_pointer=='!'){
        //        return !num;
        //    }
       // else {
         //   return vaddr_read(num,4);
       // }
          }
		//op = the position of dominant operator in the token expression;
        int val1,val2;
        if (args[op]!='='&&args[op]!='&'&&args[op]!='|'){
		val1 = eval(p, op - 1, args,success);
        }
        else val1=eval(p,op-2,args,success);
		val2 = eval(op + 1, q, args,success);
		switch (args[op]) {
		case '+': return val1 + val2;
		case '-': return val1 - val2;
		case '*': return val1 * val2;
		case '/': return val1 / val2;
        case '=':{
            if (args[op-1]=='=')
            return val1 == val2;
            else return val1!=val2;
        }
        case '&': return val1 && val2;
        case '|': return val1 || val2;
        default: {*success=false;return 0;};

	}
	}

}


int cmd_p(char *args)
{
	int len = strlen(args);
	//if (match(0,len-1, args) != len - 1) {
	//	assert(0);
   // }
    bool success=true;
    int ah=eval(0,len-1,args,&success);
    if (success)
	    printf("%d\n", ah);
    else printf("%s\n","Bad Expersion, try it again!");
	return 0;
}


int cmd_w(char *args)
{
    bool success=true;
    char lx_copy[128];
    strcpy(lx_copy,args);
    int hah=eval(0,strlen(args)-1,args,&success);
    if (!success){
        printf("%s\n","Bad Expersion, try it again!");
        return 0;
    }
    WP *ah=new_up();
    strcpy(ah->args,lx_copy);
    ah->old_value=hah;
    return 0;
}

int cmd_d(char *args)
{
    if (!free_wp(atoi(args))){
        printf("%s\n","No such watch point found, try it again!");
    }
    return 0;
}

static int cmd_help(char *args);
static int cmd_si(char *args);

static struct {
	char *name;
	char *description;
	int(*handler) (char *);
} cmd_table[] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si","run N steps", cmd_si},
	{ "info", "print", cmd_info },
	{ "x", "scan the memory",cmd_x },
	{ "p", "print expr",cmd_p },
    { "w", "watch point",cmd_w },
    { "d", "delete the watch point, and you need to input the number of the watch point",cmd_d}
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if (arg == NULL) {
		/* no argument given */
		for (i = 0; i < NR_CMD; i++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for (i = 0; i < NR_CMD; i++) {
			if (strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args)
{
	if (args == NULL) {
		cpu_exec(1);
		return 0;
	}
//	int len = strlen(args);
	/*
    int m = 0, po = 1, ma = 0;
	while (args[ma] == ' ') {      //count the " ";
		ma++;
	}
	for (int i = len - 1; i >= ma; i--)
	{
		m += (args[i] - 48)*po;
		po *= 10;
	}
	if (m == 0 || len == 0) {
		m = 1;
	}   */
	cpu_exec(atoi(args));
	return 0;
}

void ui_mainloop(int is_batch_mode) {
	if (is_batch_mode) {
		cmd_c(NULL);
		return;
	}

	while (1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if (cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		* which may need further parsing
		*/
		char *args = cmd + strlen(cmd) + 1;
		if (args >= str_end) {
			args = NULL;
		}

#ifdef HAS_IOE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for (i = 0; i < NR_CMD; i++) {
			if (strcmp(cmd, cmd_table[i].name) == 0) {
				if (cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}

